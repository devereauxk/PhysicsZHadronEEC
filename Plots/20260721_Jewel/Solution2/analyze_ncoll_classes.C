// Solution 2: per-Ncoll-class quenching study.
// Away-side yields (DeltaPhi in [pi/2, 3pi/2], x0.5 norm) for each Ncoll class,
// ratios to the JEWEL pp (Z pT rw) vacuum baseline, and a minimum-bias
// reweighted prediction using the Glauber P(Ncoll) kernel.
//
// Classes and their ensemble properties from the v4 allocation table
// (slot-weighted; "weight" = sum of mult x Ncoll over the class slots):
//   le3   : n = 1..3,   weight   23, <n>_hard =  2.57   (converted skim)
//   4to7  : n = 4..7,   weight  100, <n>_hard =  5.88   (interpolated)
//   8to12 : n = 8..12,  weight  379, <n>_hard = 10.19   (converted skim)
//   13to16: n = 13..16, weight  174, <n>_hard = 14.56   (interpolated)
//   ge17  : n = 17..21, weight  407, <n>_hard = 18.64   (converted skim)

double AwayYield(const char *prefix, const char *zpt, const char *trk, double &err) {
    TFile *f = TFile::Open(Form("%s_ZPT%s-result.root", prefix, zpt));
    if (!f || f->IsZombie()) { printf("ERROR: cannot open %s_ZPT%s-result.root\n", prefix, zpt); err = 0; return -1; }
    TH1D *h = (TH1D*)f->Get(Form("DeltaPhi_Result%s", trk));
    if (!h) { printf("ERROR: missing DeltaPhi_Result%s in %s_ZPT%s\n", trk, prefix, zpt); err = 0; return -1; }
    double e; double v = h->IntegralAndError(7, 12, e, "width");
    f->Close();
    err = 0.5 * e;
    return 0.5 * v;
}

void analyze_ncoll_classes() {
    const char *base = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots";
    const char *zpts[] = {"0_500", "0_30", "30_500"};
    const char *trks[] = {"0.5_2", "2_4", "4_15"};

    TString ppPrefix   = Form("%s/jewelPP8160signed_MOD_zrw_scan", base);
    TString fullPrefix = Form("%s/jewelPPb8160signed_MODv4_zrw_scan", base);
    const char *classes[]    = {"le3", "8to12", "ge17"};
    double classMeanN[]      = {2.57, 10.19, 18.64};

    // 5-bin ensemble decomposition (measured classes + interpolation targets)
    const int    NENS = 5;
    double ensMeanN[NENS]  = {2.57, 5.88, 10.19, 14.56, 18.64};
    double ensWeight[NENS] = {23, 100, 379, 174, 407};       // v4 sample hard weights (mult x n summed)
    int    ensLoN[NENS]    = {1, 4, 8, 13, 17};
    int    ensHiN[NENS]    = {3, 7, 12, 16, 21};

    // Glauber MB P(n)
    double pMB[61] = {0};
    {
        FILE *fg = fopen("output/glauber_pPb_pncoll.tsv", "r");
        if (!fg) { printf("ERROR: run glauber_pPb.C first\n"); return; }
        char line[256];
        while (fgets(line, sizeof(line), fg)) {
            if (line[0] == '#') continue;
            int n; double p;
            if (sscanf(line, "%d %lf", &n, &p) == 2 && n >= 1 && n <= 60) pMB[n] = p;
        }
        fclose(fg);
    }

    gSystem->mkdir("output", true);
    gSystem->mkdir("plots", true);
    FILE *ft = fopen("output/away_ratios_ncoll.tsv", "w");
    fprintf(ft, "# zpt\ttrk\tclass\tmeanN\tR(class/pp)\terr\n");

    // R[zpt][trk][class], and full-sample R for cross-check
    double R[3][3][3], Rerr[3][3][3], Rfull[3][3], Rfullerr[3][3];

    for (int iz = 0; iz < 3; ++iz) {
        for (int it = 0; it < 3; ++it) {
            double eP; double yP = AwayYield(ppPrefix, zpts[iz], trks[it], eP);
            double eF; double yF = AwayYield(fullPrefix, zpts[iz], trks[it], eF);
            Rfull[iz][it] = yF / yP;
            Rfullerr[iz][it] = Rfull[iz][it] * sqrt(pow(eF/yF,2) + pow(eP/yP,2));
            for (int ic = 0; ic < 3; ++ic) {
                TString cp = Form("%s/jewelPPbv4Ncoll_%s_zrw_scan", base, classes[ic]);
                double eC; double yC = AwayYield(cp, zpts[iz], trks[it], eC);
                R[iz][it][ic] = yC / yP;
                Rerr[iz][it][ic] = R[iz][it][ic] * sqrt(pow(eC/yC,2) + pow(eP/yP,2));
                fprintf(ft, "%s\t%s\t%s\t%.2f\t%.4f\t%.4f\n",
                        zpts[iz], trks[it], classes[ic], classMeanN[ic], R[iz][it][ic], Rerr[iz][it][ic]);
            }
        }
    }
    fclose(ft);
    printf("Wrote output/away_ratios_ncoll.tsv\n");

    // --- MB reweighting ---
    // R(n) modeled as linear interpolation in n between measured class means,
    // constant outside [2.57, 18.64].
    auto Rofn = [&](int iz, int it, double n) {
        double x0 = 2.57, x1 = 10.19, x2 = 18.64;
        double r0 = R[iz][it][0], r1 = R[iz][it][1], r2 = R[iz][it][2];
        if (n <= x0) return r0;
        if (n >= x2) return r2;
        if (n <= x1) return r0 + (r1 - r0) * (n - x0) / (x1 - x0);
        return r1 + (r2 - r1) * (n - x1) / (x2 - x1);
    };

    FILE *fm = fopen("output/mb_reweight.tsv", "w");
    fprintf(fm, "# zpt\ttrk\tR_sample_meas\tR_sample_recon\tR_MB\tinflation(=(1-R_MB)/(1-R_meas) for R<1, else (R_meas-1)/(R_MB-1))\n");
    printf("\n%-8s %-6s | %-9s %-9s %-9s\n", "ZPT", "trk", "R_meas", "R_recon", "R_MB");
    for (int iz = 0; iz < 3; ++iz) {
        for (int it = 0; it < 3; ++it) {
            // Reconstruct the v4-sample ratio from the 5-bin decomposition (validates interpolation)
            double num = 0, den = 0;
            for (int ie = 0; ie < NENS; ++ie) {
                num += ensWeight[ie] * Rofn(iz, it, ensMeanN[ie]);
                den += ensWeight[ie];
            }
            double Rrecon = num / den;
            // MB prediction: hard-probe kernel w(n) = n * P_MB(n)
            double numMB = 0, denMB = 0;
            for (int n = 1; n <= 60; ++n) {
                double w = n * pMB[n];
                numMB += w * Rofn(iz, it, n);
                denMB += w;
            }
            double RMB = numMB / denMB;
            double infl;
            if (Rfull[iz][it] < 1) infl = (1 - Rfull[iz][it]) / std::max(1e-9, 1 - RMB);
            else                   infl = (Rfull[iz][it] - 1) / std::max(1e-9, RMB - 1);
            fprintf(fm, "%s\t%s\t%.4f\t%.4f\t%.4f\t%.3f\n", zpts[iz], trks[it], Rfull[iz][it], Rrecon, RMB, infl);
            printf("%-8s %-6s | %.3f     %.3f     %.3f\n", zpts[iz], trks[it], Rfull[iz][it], Rrecon, RMB);
        }
    }
    fclose(fm);
    printf("Wrote output/mb_reweight.tsv\n");

    // --- Plot: R vs <Ncoll>, one pad per ZPT, three trk series ---
    gStyle->SetOptStat(0);
    TCanvas c("c", "", 1500, 520);
    c.Divide(3, 1);
    int colors[] = {kRed+1, kBlue+1, kGreen+2};
    int markers[] = {20, 21, 22};
    const char *zptLabels[] = {"inclusive p_{T}^{Z}", "p_{T}^{Z} < 30 GeV", "p_{T}^{Z} > 30 GeV"};
    const char *trkLabels[] = {"0.5 < p_{T}^{ch} < 2 GeV", "2 < p_{T}^{ch} < 4 GeV", "4 < p_{T}^{ch} < 15 GeV"};

    std::vector<TObject*> keep;
    for (int iz = 0; iz < 3; ++iz) {
        c.cd(iz + 1);
        gPad->SetLeftMargin(0.13); gPad->SetBottomMargin(0.13);
        TH1F *frame = new TH1F(Form("fr%d", iz), "", 10, 0, 22);
        frame->GetYaxis()->SetRangeUser(0.4, 1.9);
        frame->GetXaxis()->SetTitle("#LTN_{coll}#GT of hydro-profile class");
        frame->GetYaxis()->SetTitle("away-side yield ratio to JEWEL pp");
        frame->GetXaxis()->SetTitleSize(0.05); frame->GetYaxis()->SetTitleSize(0.05);
        frame->GetXaxis()->SetLabelSize(0.045); frame->GetYaxis()->SetLabelSize(0.045);
        frame->Draw("AXIS");
        keep.push_back(frame);
        TLine *unity = new TLine(0, 1, 22, 1);
        unity->SetLineStyle(2); unity->Draw();
        keep.push_back(unity);

        TLegend *leg = new TLegend(0.16, 0.68, 0.60, 0.88);
        leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.042);
        for (int it = 0; it < 3; ++it) {
            TGraphErrors *g = new TGraphErrors(3);
            for (int ic = 0; ic < 3; ++ic) {
                g->SetPoint(ic, classMeanN[ic], R[iz][it][ic]);
                g->SetPointError(ic, 0, Rerr[iz][it][ic]);
            }
            g->SetMarkerStyle(markers[it]); g->SetMarkerSize(1.3);
            g->SetMarkerColor(colors[it]); g->SetLineColor(colors[it]); g->SetLineWidth(2);
            g->Draw("PL SAME");
            leg->AddEntry(g, trkLabels[it], "lp");
            keep.push_back(g);
        }
        leg->Draw(); keep.push_back(leg);
        TLatex tex; tex.SetNDC(); tex.SetTextSize(0.05);
        tex.DrawLatex(0.16, 0.92, zptLabels[iz]);
    }
    c.SaveAs("plots/away_ratio_vs_ncoll.pdf");
    printf("Wrote plots/away_ratio_vs_ncoll.pdf\n");
}
