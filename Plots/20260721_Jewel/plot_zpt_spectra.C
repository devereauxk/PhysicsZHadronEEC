// Z pT spectrum comparison: Pythia+MG vs JEWEL ptmin=0 (raw + reweighted) vs JEWEL ptmin=15.
// All spectra from analysis-level hZPtEtaPhi (same Z selection); reweighted = ptmin=0 x weight file.
void plot_zpt_spectra() {
    const char *base = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots";

    TFile *fM = TFile::Open(Form("%s/pythiaMC_Gen_EExtrapolation_EEV6_ZV10_trkV29_nmix10_ZPT0_500-nosub.root", base));
    TFile *fJ = TFile::Open(Form("%s/jewelPP8160signed_MOD_ZPT0_500-nosub.root", base));
    TFile *fJ15 = TFile::Open(Form("%s/jewelPP8160signed_MOD15_ZPT0_500-nosub.root", base));
    TFile *fW = TFile::Open("/home/kdeverea/PhysicsZHadronEEC/Plots/20260721_Jewel/output/jewel_pp_zptweight.root");

    TH1D *hM = ((TH3D*)fM->Get("hZPtEtaPhi_0.5_15"))->ProjectionX("hM");
    TH1D *hJ = ((TH3D*)fJ->Get("hZPtEtaPhi_0.5_15"))->ProjectionX("hJ");
    TH1D *hJ15 = ((TH3D*)fJ15->Get("hZPtEtaPhi_0.5_15"))->ProjectionX("hJ15");
    TH1D *hW = (TH1D*)fW->Get("hPtCorrTotal");

    // Reweighted ptmin=0 spectrum: bin-by-bin product with the weight
    TH1D *hJrw = (TH1D*)hJ->Clone("hJrw");
    for (int i = 1; i <= hJrw->GetNbinsX(); ++i) {
        double pt = hJrw->GetBinCenter(i);
        double w = hW->GetBinContent(hW->FindBin(pt));
        hJrw->SetBinContent(i, hJ->GetBinContent(i) * w);
        hJrw->SetBinError(i, hJ->GetBinError(i) * w);
    }

    TH1D *hists[] = {hM, hJ, hJrw, hJ15};
    const char *labels[] = {"Pythia8+MG", "JEWEL pp (ptmin=0)", "JEWEL pp (ptmin=0, Z p_{T} rw)", "JEWEL pp (ptmin=15)"};
    int colors[] = {kRed+1, kBlue+1, kGreen+2, kMagenta+1};
    int markers[] = {20, 24, 21, 25};

    for (auto *h : hists) {
        h->Scale(1.0 / h->Integral(), "width");
        h->SetTitle("");
    }
    double gmax = 0;
    for (auto *h : hists) gmax = TMath::Max(gmax, h->GetMaximum());

    gStyle->SetOptStat(0);
    TCanvas c("c", "", 700, 800);
    TPad *pTop = new TPad("pTop", "", 0, 0.35, 1, 1);
    TPad *pBot = new TPad("pBot", "", 0, 0, 1, 0.35);
    pTop->SetBottomMargin(0.02); pTop->SetLeftMargin(0.15);
    pTop->SetLogx(); pTop->SetLogy();
    pBot->SetTopMargin(0.02); pBot->SetBottomMargin(0.3); pBot->SetLeftMargin(0.15);
    pBot->SetLogx();
    pTop->Draw(); pBot->Draw();

    pTop->cd();
    for (int i = 0; i < 4; ++i) {
        hists[i]->SetLineColor(colors[i]);
        hists[i]->SetLineWidth(2);
        hists[i]->SetMarkerColor(colors[i]);
        hists[i]->SetMarkerStyle(markers[i]);
        hists[i]->SetMarkerSize(0.8);
    }
    hM->GetYaxis()->SetTitle("(1/N_{Z}) dN_{Z}/dp_{T}^{Z} (GeV^{-1})");
    hM->GetYaxis()->SetTitleSize(0.05);
    hM->GetYaxis()->SetTitleOffset(1.3);
    hM->GetXaxis()->SetLabelSize(0);
    hM->GetXaxis()->SetRangeUser(0.5, 350);
    hM->SetMinimum(1e-7);
    hM->SetMaximum(gmax * 3);
    hM->Draw("EP");
    for (int i = 1; i < 4; ++i) hists[i]->Draw("EP SAME");

    TLegend *leg = new TLegend(0.18, 0.10, 0.60, 0.32);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.035);
    for (int i = 0; i < 4; ++i) leg->AddEntry(hists[i], labels[i], "lep");
    leg->Draw();

    TLatex tex; tex.SetNDC(); tex.SetTextSize(0.035);
    tex.DrawLatex(0.55, 0.85, "normalized Z p_{T} spectra");

    pBot->cd();
    TH1D *r[3];
    for (int i = 0; i < 3; ++i) {
        r[i] = (TH1D*)hists[i+1]->Clone(Form("r%d", i));
        r[i]->Divide(hM);
        r[i]->SetTitle("");
    }
    r[0]->GetYaxis()->SetTitle("ratio to Pythia8+MG");
    r[0]->GetYaxis()->SetTitleSize(0.08); r[0]->GetYaxis()->SetTitleOffset(0.75);
    r[0]->GetYaxis()->SetLabelSize(0.07); r[0]->GetYaxis()->SetNdivisions(505);
    r[0]->GetXaxis()->SetTitle("p_{T}^{Z} (GeV)");
    r[0]->GetXaxis()->SetTitleSize(0.1); r[0]->GetXaxis()->SetTitleOffset(1.2);
    r[0]->GetXaxis()->SetLabelSize(0.08);
    r[0]->GetXaxis()->SetRangeUser(0.5, 350);
    r[0]->GetYaxis()->SetRangeUser(0, 3.5);
    r[0]->Draw("EP");
    r[1]->Draw("EP SAME");
    r[2]->Draw("EP SAME");
    TLine unity(0.5, 1, 350, 1); unity.SetLineStyle(2); unity.Draw();

    c.SaveAs("/home/kdeverea/PhysicsZHadronEEC/Plots/20260721_Jewel/plots/jewel_zpt_spectra.pdf");
    printf("Saved jewel_zpt_spectra.pdf\n");
}
