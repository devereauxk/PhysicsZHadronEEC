#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

// example usage
// root -l -b -q "plotResidualCor.C(\"0_100\", \"0_40\", \"PPb\", true)"
// root -l -b -q "plotResidualCor.C(\"0_100\", \"0_40\", \"PbP\", false)"

const int ccolors[14] = {
    kBlue+2,     // deep blue
    kAzure+7,    // cyan
    kTeal+3,     // teal
    kGreen+2,    // green
    kSpring+7,   // turquoise-green
    kOrange+7,   // orange (avoiding yellow)
    kRed,        // red
    kPink+7,     // pink
    kMagenta+2,  // magenta
    kViolet+7,   // purple
    kGray+2,     // gray
    kBlack,      // black
    kOrange+2,   // extra orange
    kCyan+2      // extra cyan
};

void divideByWidth(TH1D* input) {
    if (!input) {
        std::cerr << "Error: Null histogram pointer passed to divideByWidth function." << std::endl;
        return;
    }

    int nBins = input->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double binContent = input->GetBinContent(i);
        double binError = input->GetBinError(i);
        double binWidth = input->GetBinWidth(i);

        if (binWidth != 0) {
            input->SetBinContent(i, binContent / binWidth);
            input->SetBinError(i, binError / binWidth);
        } else {
            std::cerr << "Warning: Bin width is zero for bin " << i << ". Skipping division for this bin." << std::endl;
        }
    }
}

void setErrors(TH1D* hRatio, TH1D* num, TH1D* denom){
    for (int bin = 1; bin <= hRatio->GetNbinsX(); bin++) {
        double content1 = num->GetBinContent(bin);
        double error1 = num->GetBinError(bin);
        double content2 = denom->GetBinContent(bin);
        double error2 = denom->GetBinError(bin);

        double ratioError = 0;
        if (content2 > 0) {
            ratioError = sqrt((error1 * error1) / (content2 * content2) +
                              (content1 * content1 * error2 * error2) / (content2 * content2 * content2 * content2));
        }
        hRatio->SetBinError(bin, ratioError);
    }
}

void plotRatioLogy(vector<TH1D*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logy = true) {

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    TPad *pad1 = new TPad(title, title, 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1D* hist = hists[i];

        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetRangeUser(ymin, ymax);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->SetLineColor(ccolors[i]);

        if (i == baseline) {
            hist->Draw("E SAME");
        } else {
            hist->Draw("HIST SAME");
        }
        leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hist->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hists[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(rTitle);
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(rmin, rmax);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hist, hists.at(baseline));

            cout<<title<<" "<<labels[i]<<" "<<hRatio->Integral()<<endl;

            hRatio->Draw("E SAME");

            TLine *line = new TLine(xmin, 1, xmax, 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");
}

void plot2D(TH2D* hist, const char* title,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    pad1->SetLogx(0);
    pad1->SetLogy(0);
    pad1->SetLogz(1);
    pad1->Draw();
    pad1->cd();

    hist->SetStats(0);
    hist->SetTitle(title);
    hist->SetTitleSize(0.03, "XYZ"); // Decrease title size
    hist->GetXaxis()->SetTitle(xTitle);
    hist->GetXaxis()->SetTitleSize(0.05);
    hist->GetXaxis()->SetLabelSize(0.045);
    hist->GetXaxis()->SetRangeUser(xmin, xmax);
    hist->GetYaxis()->SetTitle(yTitle);
    hist->GetYaxis()->SetTitleSize(0.05);
    hist->GetYaxis()->SetLabelSize(0.045);
    hist->GetYaxis()->SetRangeUser(ymin, ymax);

    hist->Draw("COLZ");
}

void plotRatioConvergence(vector<TH1D*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax) {

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    TPad *pad1 = new TPad(title, title, 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetLogy(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1D* hist = hists[i];

        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetRangeUser(ymin, ymax);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->SetLineColor(ccolors[i]);

        if (i == 0) {
            hist->Draw("E SAME");
        } else {
            hist->Draw("HIST SAME");
        }
        leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        TLine *line2 = new TLine(xmin, 1, xmax, 1);
        line2->SetLineColor(kGray+2);
        line2->SetLineStyle(2);
        line2->Draw("SAME");

        pad2->cd();
        if (i != 0) {
            TH1D* hRatio = (TH1D*)hist->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hists[i-1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(rTitle);
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(rmin, rmax);
            hRatio->SetLineColor(ccolors[i]);

            // Force statistical errors for the ratio to be zero
            for (int bin = 1; bin <= hRatio->GetNbinsX(); ++bin) {
                hRatio->SetBinError(bin, 0);
            }

            cout<<title<<" "<<labels[i]<<" "<<hRatio->Integral()<<endl;

            hRatio->Draw("SAME");

            TLine *line = new TLine(xmin, 1, xmax, 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");
}


void plotResidualCor(const char *zpt_select, const char *pt_select, const char* tag = "", bool PPb = true) {

    /*
    vector<string> labels = {"data(reco)", "MC(gen)", "MC(reco) i=0",
                             "MC(reco) i=1 (pt)", "MC(reco) i=1 (pt+eta)",
                             "MC(reco) i=2 (pt)", "MC(reco) i=2 (pt+eta)"};
    vector<string> fname;
    if (PPb) {
        fname = {"output/1pPb_ZPT0_100-0_40.root", "output/pPbMC_Gen_ZPT0_100-0_40.root",
                 "output/pPbMC_ZPT0_100-0_40-0.root",
                 "output/pPbMC_ZPT0_100-0_40-1-pt.root", "output/pPbMC_ZPT0_100-0_40-1.root",
                 "output/pPbMC_ZPT0_100-0_40-2-pt.root", "output/pPbMC_ZPT0_100-0_40-2.root"};
    }
    vector<string> labels_Zratio = {"0", "1_pt", "1_eta", "2_pt", "2_eta"};
    */

    vector<string> labels = {"data(reco)", "MC(gen)", "MC(reco) i=0",
                             "MC(reco) i=1 (pt+eta)",
                             "MC(reco) i=2 (pt+eta)",
                             "MC(reco) i=3 (pt+eta)",
                             "MC(reco) i=4 (pt+eta)"};
    vector<string> fname;
    if (PPb) {
        fname = {"output/1pPb_ZPT0_100-0_40.root", "output/pPbMC_Gen_ZPT0_100-0_40.root",
                 "output/pPbMC_ZPT0_100-0_40-0.root",
                 "output/pPbMC_ZPT0_100-0_40-1.root", "output/pPbMC_ZPT0_100-0_40-2.root",
                    "output/pPbMC_ZPT0_100-0_40-3.root", "output/pPbMC_ZPT0_100-0_40-4.root"};
    }
    vector<string> labels_Zratio = {"0", "1_pteta", "2_pteta", "3_pteta", "4_pteta"};
    

    int ncontours = 2;
    int baseline = 0;
    const char *rTitle = "hist / data(reco)";

    vector<TH1D*> hTrkPt;
    vector<TH1D*> hTrkEta;
    vector<TH1D*> hTrkPhi;
    vector<TH1D*> hLeadingPt;
    vector<TH1D*> hLeadingEta;
    vector<TH1D*> hZPt;
    vector<TH1D*> hZEta;
    vector<TH1D*> hZMass;
    vector<TH1D*> hNZ;
    vector<TH1D*> hMult;
    vector<TH1D*> hTrkWeight;

    vector<TH2D*> hTrkPtEta;
    vector<TH2D*> hTrkPtPhi;

    const int nptbin = 3;
    const int ptbin_lo[nptbin] = {0, 20, 40};
    const int ptbin_hi[nptbin] = {20, 40, 200};
    vector<vector<TH1D*>> hMult_ptbin;

    // Load histograms for PbPb
    for (int i = 0; i < fname.size(); i++) {
        TFile *file = new TFile(fname[i].c_str(), "READ");

        hLeadingPt.push_back((TH1D*)file->Get("hLeadingPtData"));
        hLeadingEta.push_back((TH1D*)file->Get("hLeadingEtaData"));
        hZMass.push_back((TH1D*)file->Get("hZMassData"));
        hNZ.push_back((TH1D*)file->Get("hNZData"));
        hTrkWeight.push_back((TH1D*)file->Get("hTrkWeightData"));

        TH3D *hTrkPtEtaPhi = (TH3D*)file->Get("hTrkPtEtaPhiData");
        /*
        hTrkPt.push_back((TH1D*)hTrkPtEtaPhi->ProjectionX("hTrkPtData"));
        hTrkEta.push_back((TH1D*)hTrkPtEtaPhi->ProjectionY("hTrkEtaData"));
        hTrkPhi.push_back((TH1D*)hTrkPtEtaPhi->ProjectionZ("hTrkPhiData"));
        */
        hTrkPtEta.push_back((TH2D*) hTrkPtEtaPhi->Project3D("yx"));
        hTrkPtPhi.push_back((TH2D*) hTrkPtEtaPhi->Project3D("zx"));
        
        hTrkPt.push_back((TH1D*)file->Get("hTrkPtData"));
        hTrkEta.push_back((TH1D*)file->Get("hTrkEtaData"));
        hTrkPhi.push_back((TH1D*)file->Get("hTrkPhiData"));

        cout<<"("<<hTrkPt[i]->Integral()<<" "<<hTrkEta[i]->Integral()<<" "<<hTrkPhi[i]->Integral()<<") "<<hNZ[i]->GetBinContent(1)<<endl;

        TH3D *hZPtEtaMult = (TH3D*)file->Get("hZPtEtaMultData");
        hZPt.push_back((TH1D*)hZPtEtaMult->ProjectionX("hZPtData"));
        hZEta.push_back((TH1D*)hZPtEtaMult->ProjectionY("hZEtaData"));
        hMult.push_back((TH1D*)hZPtEtaMult->ProjectionZ("hZMultData"));

        // Create histograms for different pt bins
        vector<TH1D*> hMult_ptbin_temp;
        for (int j = 0; j < nptbin; j++) {
            hMult_ptbin_temp.push_back((TH1D*)hZPtEtaMult->ProjectionZ(
            Form("hMult_ptbin_%d_%d", i, j),
            hZPtEtaMult->GetXaxis()->FindBin(ptbin_lo[j] + 1e-6),
            hZPtEtaMult->GetXaxis()->FindBin(ptbin_hi[j] - 1e-6)
            ));
        }
        hMult_ptbin.push_back(hMult_ptbin_temp);

        // Normalize histograms
        double integral = hNZ[i]->GetBinContent(1);
        hTrkPt[i]->Scale(1. / integral);
        hTrkPhi[i]->Scale(1. / integral);
        hLeadingPt[i]->Scale(1. / integral);
        hTrkEta[i]->Scale(1. / integral);
        hLeadingEta[i]->Scale(1. / integral);
        hZPt[i]->Scale(1. / integral);
        hZEta[i]->Scale(1. / integral);
        hZMass[i]->Scale(1. / integral);
        hTrkWeight[i]->Scale(1. / integral);

        for (int j = 0; j < nptbin; j++) {
            hMult_ptbin[i][j]->Scale(1. / hMult_ptbin[i][j]->Integral());
        }
        hMult[i]->Scale(1. / hMult[i]->Integral());

        // divide by width
        divideByWidth(hTrkPt[i]);
        divideByWidth(hTrkPhi[i]);
        divideByWidth(hLeadingPt[i]);
        divideByWidth(hTrkEta[i]);
        divideByWidth(hLeadingEta[i]);
        divideByWidth(hZPt[i]);
        divideByWidth(hZEta[i]);
        divideByWidth(hZMass[i]);
        divideByWidth(hTrkWeight[i]);

    }

    // Z correction ratios
    vector<TH1D*> hZPtRatio;
    vector<TH1D*> hZEtaRatio;
    for (int i = 0; i < fname.size()-2; i++) {

        hZPtRatio.push_back((TH1D*)hZPt[i+2]->Clone(Form("hZPtRatio_%d", i)));
        hZEtaRatio.push_back((TH1D*)hZEta[i+2]->Clone(Form("hZEtaRatio_%d", i)));
        hZPtRatio[i]->Divide(hZPt[0]);
        hZEtaRatio[i]->Divide(hZEta[0]);
        
    }

    // Create a canvas to draw the histograms for PbPb
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1600, 2900);
    c1->Divide(2, 5);

    c1->cd(1);
    plotRatioLogy(
        hTrkPt,
        "Track pT",
        labels,
        "pT (GeV/c)", 0, 40,
        "(1/N_{Z}) dN/dp_{T}", 1e-3, 1e2,
        rTitle, 0, 3.5,
        baseline
    );

    c1->cd(2);
    plotRatioLogy(
        hTrkEta,
        "Track eta",
        labels,
        "eta", -3, 3,
        "(1/N_{Z}) dN/d#eta", 3, 2e1,
        rTitle, 0, 2,
        baseline
    );

    c1->cd(3);
    plotRatioLogy(
        hTrkPhi,
        "Track phi",
        labels,
        "phi", -1.5*M_PI, 1.5*M_PI,
        "(1/N_{Z}) dN/d#phi", 3, 1e1,
        rTitle, 0, 2,
        baseline
    );

    c1->cd(4);
    plotRatioLogy(
        hMult,
        "Multiplicity",
        labels,
        "N_{ch}", 0, 100,
        "Entries (normed to 1)", 1e-4, 2e-1,
        rTitle, 0, 4,
        baseline
    );

    c1->cd(5);
    plotRatioLogy(
        hZPt,
        "Z pT",
        labels,
        "pT (GeV/c)", 0, 25,
        "(1/N_{Z}) dN/dp_{T}", 5e-3, 1e-1,
        rTitle, 0.6, 1.4,
        baseline
    );

    c1->cd(6);
    plotRatioLogy(
        hZEta,
        "Z eta",
        labels,
        "eta", -2.4, 2.4,
        "(1/N_{Z}) dN/d#eta", 2e-2, 1,
        rTitle, 0.6, 1.4,
        baseline
    );

    c1->cd(7);
    plotRatioLogy(
        hLeadingPt,
        "Leading Track pT",
        labels,
        "pT (GeV/c)", 0, 20,
        "(1/N_{Z}) dN/dp_{T}", 1e-3, 5e-1,
        rTitle, 0, 2,
        baseline
    );

    c1->cd(8);
    plotRatioLogy(
        hLeadingEta,
        "Leading Track eta",
        labels,
        "eta", -2.4, 2.4,
        "(1/N_{Z}) dN/d#eta", 5e-2, 1e0,
        rTitle, 0.6, 1.4,
        baseline
    );

    c1->cd(9);
    plotRatioConvergence(
        hZPtRatio,
        "Z pT, c=Data(reco)/MC(reco)",
        labels_Zratio,
        "pT (Gev/c)", 0, 25,
        "correction factor", 0.6, 1.4,
        "c(n)/c(n-1)", 0.85, 1.15
    );

    c1->cd(10);
    plotRatioConvergence(
        hZEtaRatio,
        "Z eta, c=Data(reco)/MC(reco)",
        labels_Zratio,
        "eta", -2.4, 2.4,
        "correction factor", 0.6, 1.4,
        "c(n)/c(n-1)", 0.85, 1.15
    );

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("diagnostic/residualCorr_pPb-%s.png", tag));

}
