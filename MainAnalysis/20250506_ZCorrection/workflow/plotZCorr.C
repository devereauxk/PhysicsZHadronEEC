#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

// example usage
// root -l -b -q "plotZCorr.C(\"0_100\", \"0_40\", \"PPb\", true)"
// root -l -b -q "plotZCorr.C(\"0_100\", \"0_40\", \"PbP\", false)"

const int rcolors[8] = {kAzure, kGreen+2, kOrange, kRed, kMagenta, kViolet+6, kBlue, kBlack};
const int ccolors[8] = {kAzure, kGreen+2, kOrange, kRed, kMagenta, kViolet+6, kBlue, kBlack};

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

void plotZCorr(const char *zpt_select, const char *pt_select, const char* tag = "", bool PPb = true) {

    int baseline = 0;
    const char * fname[8];
    /*
    const char * pPb_name[8] = {"data", "MC reco_0 no corr", "MC reco_1 pt", "MC reco_1 pt+eta", "MC reco_1 pt+eta+mult", "MC reco_2 pt", "MC reco_2 pt+eta", "MC reco_2 pt+eta+mult"};
    if (PPb) {
        fname[0] = "output/1pPb_ZPT0_100-0_40.root";
        fname[1] = "output/pPbMC_ZPT0_100-0_40-0.root";
        fname[2] = "output/pPbMC_ZPT0_100-0_40-1-pt.root";
        fname[3] = "output/pPbMC_ZPT0_100-0_40-1-pteta.root";
        fname[4] = "output/pPbMC_ZPT0_100-0_40-1-ptetamult.root";
        fname[5] = "output/pPbMC_ZPT0_100-0_40-2-pt.root";
        fname[6] = "output/pPbMC_ZPT0_100-0_40-2-pteta.root";
        fname[7] = "output/pPbMC_ZPT0_100-0_40-2-ptetamult.root";
    } else {
        fname[0] = "output/0pPb_ZPT0_100-0_40.root";
        fname[1] = "output/PbPMC_ZPT0_100-0_40-0.root";
        fname[2] = "output/PbPMC_ZPT0_100-0_40-1-pt.root";
        fname[3] = "output/PbPMC_ZPT0_100-0_40-1-pteta.root";
        fname[4] = "output/PbPMC_ZPT0_100-0_40-1-ptetamult.root";
        fname[5] = "output/PbPMC_ZPT0_100-0_40-2-pt.root";
        fname[6] = "output/PbPMC_ZPT0_100-0_40-2-pteta.root";
        fname[7] = "output/PbPMC_ZPT0_100-0_40-2-ptetamult.root";
    }
    */
    //const char * pPb_name[8] = {"data", "MC reco_0 no corr", "MC reco_1 pt+eta+mult", "MC reco_2 pt+eta+mult", "MC reco_3 pt+eta+mult", "MC reco_4 pt+eta+mult","",""};
    const char * pPb_name[8] = {"data PPb", "MC reco_0 PPb", "data PbP", "MC reco_0 PbP", "","","", ""};
    if (PPb) {
        fname[0] = "output/1pPb_ZPT0_100-0_40.root";
        fname[1] = "output/pPbMC_ZPT0_100-0_40-0.root";
        fname[2] = "output/0pPb_ZPT0_100-0_40.root";
        fname[3] = "output/PbPMC_ZPT0_100-0_40-0.root";
        //fname[1] = "output/pPbMC_ZPT0_100-0_40-0.root";
        //fname[2] = "output/pPbMC_ZPT0_100-0_40-1-ptetamult.root";
        //fname[3] = "output/pPbMC_ZPT0_100-0_40-2-ptetamult.root";
        fname[4] = "output/pPbMC_ZPT0_100-0_40-3-ptetamult.root";
        fname[5] = "output/pPbMC_ZPT0_100-0_40-4-ptetamult.root";
        fname[6] = "";
        fname[7] = "";
    } else {
        fname[0] = "output/0pPb_ZPT0_100-0_40.root";
        fname[1] = "output/PbPMC_ZPT0_100-0_40-0.root";
        fname[2] = "output/PbPMC_ZPT0_100-0_40-1-ptetamult.root";
        fname[3] = "output/PbPMC_ZPT0_100-0_40-2-ptetamult.root";
        fname[4] = "output/PbPMC_ZPT0_100-0_40-3-ptetamult.root";
        fname[5] = "output/PbPMC_ZPT0_100-0_40-4-ptetamult.root";
        fname[6] = "";
        fname[7] = "";
    }

    int ncontours = 4;

    TH1D* hTrkPt[ncontours];
    TH1D* hTrkEta[ncontours];
    TH1D* hTrkPhi[ncontours];
    TH1D* hLeadingPt[ncontours];
    TH1D* hLeadingEta[ncontours];
    TH1D* hZPt[ncontours];
    TH1D* hZEta[ncontours];
    TH1D* hZMass[ncontours];
    TH1D* hNZ[ncontours];
    TH1D* hMult[ncontours];
    TH1D* hTrkWeight[ncontours];
    
    const int nptbin = 3;
    const int ptbin_lo[nptbin] = {0, 20, 40};
    const int ptbin_hi[nptbin] = {20, 40, 200};
    TH1D* hMult_ptbin[ncontours][nptbin];

    // Load histograms for PbPb
    for (int i = 0; i < ncontours; i++) {
        TFile *file =  new TFile(fname[i], "READ");
        cout<<"Loading file: " << fname[i] << endl;

        hLeadingPt[i] = (TH1D*)file->Get("hLeadingPtData");
        hLeadingEta[i] = (TH1D*)file->Get("hLeadingEtaData");
        hZMass[i] = (TH1D*)file->Get("hZMassData");
        hNZ[i] = (TH1D*)file->Get("hNZData");
        hMult[i] = (TH1D*)file->Get("hMultData");
        hTrkWeight[i] = (TH1D*)file->Get("hTrkWeightData");

        TH3D *hTrkPtEtaPhi = (TH3D*)file->Get("hTrkPtEtaPhiData");
        hTrkPt[i] = (TH1D*)hTrkPtEtaPhi->ProjectionX("hTrkPtData");
        hTrkEta[i] = (TH1D*)hTrkPtEtaPhi->ProjectionY("hTrkEtaData");
        hTrkPhi[i] = (TH1D*)hTrkPtEtaPhi->ProjectionZ("hTrkPhiData");

        TH3D *hZPtEtaMult = (TH3D*)file->Get("hZPtEtaMultData");
        hZPt[i] = (TH1D*)hZPtEtaMult->ProjectionX("hZPtData");
        hZEta[i] = (TH1D*)hZPtEtaMult->ProjectionY("hZEtaData");
        hMult[i] = (TH1D*)hZPtEtaMult->ProjectionZ("hZMultData");

        // Create histograms for different pt bins
        for (int j = 0; j < nptbin; j++) {
            hMult_ptbin[i][j] = (TH1D*)hZPtEtaMult->ProjectionZ(
            Form("hMult_ptbin_%d_%d", i, j),
            hZPtEtaMult->GetXaxis()->FindBin(ptbin_lo[j] + 1e-6),
            hZPtEtaMult->GetXaxis()->FindBin(ptbin_hi[j] - 1e-6)
            );
        }

        // Normalize histograms
        double integral = hNZ[i]->GetBinContent(1);
        hTrkPt[i]->Scale(1. / integral);
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
        divideByWidth(hLeadingPt[i]);
        divideByWidth(hTrkEta[i]);
        divideByWidth(hLeadingEta[i]);
        divideByWidth(hZPt[i]);
        divideByWidth(hZEta[i]);
        divideByWidth(hZMass[i]);
        divideByWidth(hTrkWeight[i]);

        // Set stats off
        hTrkPt[i]->SetStats(0);
        hLeadingPt[i]->SetStats(0);
        hTrkEta[i]->SetStats(0);
        hLeadingEta[i]->SetStats(0);
        hZPt[i]->SetStats(0);
        hZEta[i]->SetStats(0);
        hZMass[i]->SetStats(0);
        hMult[i]->SetStats(0);
        hTrkWeight[i]->SetStats(0);
        for (int j = 0; j < nptbin; j++) {
            hMult_ptbin[i][j]->SetStats(0);
        }
    }

    // Create a canvas to draw the histograms for PbPb
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1600, 2900);
    c1->Divide(2, 5);

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    c1->cd(1);
    TPad *pad1 = new TPad("pad1_1", "pad1", 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetLogy();
    pad1->Draw();
    TPad *pad2 = new TPad("pad2_1", "pad2", 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    for (int i = 0; i < ncontours; i++) {
        pad1->cd();
        hTrkPt[i]->SetTitle("Track pT");
        hTrkPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hTrkPt[i]->GetYaxis()->SetTitle("(1/N_{Z}) dN/dp_{T}");
        hTrkPt[i]->GetYaxis()->SetTitleSize(0.05);
        hTrkPt[i]->GetXaxis()->SetRangeUser(0, 20);
        hTrkPt[i]->SetLineColor(ccolors[i]);
        if (i == 0) {
            hTrkPt[i]->Draw("E SAME");
        } else {
            hTrkPt[i]->Draw("HIST SAME");
        }
        leg->AddEntry(hLeadingPt[i], pPb_name[i], "l");

        pad2->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hTrkPt[i]->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hTrkPt[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hTrkPt[i], hTrkPt[baseline]);

            hRatio->Draw("E SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");

    c1->cd(2);
    TPad *pad3 = new TPad("pad1_2", "pad1", 0, 0.3, 1, 1);
    pad3->SetBottomMargin(0);
    pad3->SetLogy();
    pad3->Draw();
    TPad *pad4 = new TPad("pad2_2", "pad2", 0, 0, 1, 0.3);
    pad4->SetTopMargin(0);
    pad4->SetBottomMargin(0.2);
    pad4->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad3->cd();
        hLeadingPt[i]->SetTitle("Leading Track pT");
        hLeadingPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hLeadingPt[i]->GetYaxis()->SetTitle("(1/N_{Z}) dN/dp_{T}");
        hLeadingPt[i]->GetYaxis()->SetTitleSize(0.05);
        hLeadingPt[i]->GetXaxis()->SetRangeUser(0, 20);
        hLeadingPt[i]->SetLineColor(ccolors[i]);
        if (i == 0) {
            hLeadingPt[i]->Draw("E SAME");
        } else {
            hLeadingPt[i]->Draw("HIST SAME");
        }

        pad4->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hLeadingPt[i]->Clone(Form("ratio_LeadingPt_%d", i));
            hRatio->Divide(hLeadingPt[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hLeadingPt[i], hLeadingPt[baseline]);

            hRatio->Draw("E SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad3->cd();
    leg->Draw("SAME");

    c1->cd(3);
    TPad *pad5 = new TPad("pad1_3", "pad1", 0, 0.3, 1, 1);
    pad5->SetBottomMargin(0);
    pad5->SetLogy();
    pad5->Draw();
    TPad *pad6 = new TPad("pad2_3", "pad2", 0, 0, 1, 0.3);
    pad6->SetTopMargin(0);
    pad6->SetBottomMargin(0.2);
    pad6->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad5->cd();
        hZPt[i]->SetTitle("Z pT Distribution");
        hZPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hZPt[i]->GetXaxis()->SetRangeUser(0, 25);
        hZPt[i]->GetYaxis()->SetTitle("(1/N_{Z}) dN/dp_{T}");
        hZPt[i]->GetYaxis()->SetTitleSize(0.05);
        hZPt[i]->SetLineColor(ccolors[i]);
        if (i == 0) {
            hZPt[i]->Draw("E SAME");
        } else {
            hZPt[i]->Draw("HIST SAME");
        }

        pad6->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hZPt[i]->Clone(Form("ratio_ZPt_%d", i));
            hRatio->Divide(hZPt[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0.6, 1.4);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hZPt[i], hZPt[baseline]);

            hRatio->Draw("E SAME"); // Add error bars

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad5->cd();
    leg->Draw("SAME");

    c1->cd(4);
    TPad *pad71 = new TPad("pad1_41", "pad1", 0, 0.3, 1, 1);
    pad71->SetBottomMargin(0);
    pad71->SetLogy();
    pad71->Draw();
    TPad *pad81 = new TPad("pad2_41", "pad2", 0, 0, 1, 0.3);
    pad81->SetTopMargin(0);
    pad81->SetBottomMargin(0.2);
    pad81->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad71->cd();
        hZEta[i]->SetTitle("Z eta");
        hZEta[i]->GetXaxis()->SetTitle("eta");
        hZEta[i]->GetYaxis()->SetTitle("(1/N_{Z}) dN/d#eta");
        hZEta[i]->GetYaxis()->SetRangeUser(0.02, 0.8);
        hZEta[i]->GetYaxis()->SetTitleSize(0.05);
        hZEta[i]->SetLineColor(ccolors[i]);
        if (i == 0) {
            hZEta[i]->Draw("E SAME");
        } else {
            hZEta[i]->Draw("HIST SAME");
        }

        pad81->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hZEta[i]->Clone(Form("ratio_ZEta_%d", i));
            hRatio->Divide(hZEta[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("eta");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0.6, 1.4);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hZEta[i], hZEta[baseline]);

            hRatio->Draw("E SAME"); // Add error bars

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad71->cd();
    leg->Draw("SAME");

    c1->cd(5);
    TPad *pad9 = new TPad("pad1_5", "pad1", 0, 0.3, 1, 1);
    pad9->SetBottomMargin(0);
    pad9->SetLogy();
    pad9->Draw();
    TPad *pad10 = new TPad("pad2_5", "pad2", 0, 0, 1, 0.3);
    pad10->SetTopMargin(0);
    pad10->SetBottomMargin(0.2);
    pad10->Draw();
    for (int i = 0; i < ncontours; i++) {
        pad9->cd();
        hTrkEta[i]->SetTitle("Track Eta");
        hTrkEta[i]->GetXaxis()->SetTitle("Eta");
        hTrkEta[i]->GetYaxis()->SetTitle("(1/N_{Z}) dN/d#eta");
        hTrkEta[i]->SetLineColor(ccolors[i]);
        hTrkEta[i]->Draw("HIST SAME");

        pad10->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hTrkEta[i]->Clone(Form("ratio_TrkEta_%d", i));
            hRatio->Divide(hTrkEta[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Eta");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0.6, 1.4);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad9->cd();
    leg->Draw("SAME");

    c1->cd(6);
    TPad *pad11 = new TPad("pad1_6", "pad1", 0, 0.3, 1, 1);
    pad11->SetBottomMargin(0);
    pad11->SetLogy();
    pad11->Draw();
    TPad *pad12 = new TPad("pad2_6", "pad2", 0, 0, 1, 0.3);
    pad12->SetTopMargin(0);
    pad12->SetBottomMargin(0.2);
    pad12->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad11->cd();
        hLeadingEta[i]->SetTitle("Leading Track Eta");
        hLeadingEta[i]->GetXaxis()->SetTitle("Eta");
        hLeadingEta[i]->GetYaxis()->SetTitle("(1/N_{Z}) dN/d#eta");
        hLeadingEta[i]->SetLineColor(ccolors[i]);
        hLeadingEta[i]->Draw("HIST SAME");

        pad12->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hLeadingEta[i]->Clone(Form("ratio_LeadingEta_%d", i));
            hRatio->Divide(hLeadingEta[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Eta");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0.6, 1.4);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    c1->cd(7);
    TPad *pad7 = new TPad("pad1_4", "pad1", 0, 0.3, 1, 1);
    pad7->SetBottomMargin(0);
    pad7->SetLogy();
    pad7->Draw();
    TPad *pad8 = new TPad("pad2_4", "pad2", 0, 0, 1, 0.3);
    pad8->SetTopMargin(0);
    pad8->SetBottomMargin(0.2);
    pad8->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad7->cd();
        hZMass[i]->SetTitle("Z Mass");
        hZMass[i]->GetXaxis()->SetTitle("Mass (GeV/c^2)");
        hZMass[i]->GetYaxis()->SetTitle("(1/N_{Z}) dN/dM");
        hZMass[i]->SetLineColor(ccolors[i]);
        hZMass[i]->Draw("HIST SAME");

        pad8->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hZMass[i]->Clone(Form("ratio_ZMass_%d", i));
            hRatio->Divide(hZMass[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Mass (GeV/c^2)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0.6, 1.4);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    c1->cd(8);
    TPad *pad91 = new TPad("pad1_8", "pad1", 0, 0.3, 1, 1);
    pad91->SetBottomMargin(0);
    pad91->SetLogy();
    pad91->Draw();
    TPad *pad111 = new TPad("pad2_8", "pad2", 0, 0, 1, 0.3);
    pad111->SetTopMargin(0);
    pad111->SetBottomMargin(0.2);
    pad111->Draw();
    TLegend *leg2 = new TLegend(0.65, 0.55, 0.85, 0.85);
    leg2->SetBorderSize(0); // Remove legend box
    leg2->SetTextSize(0.04); // Reduce font size
    for (int i = 0; i < nptbin; i++) {
        for (int j = 0; j < 2; j++) {
            pad91->cd();
            hMult_ptbin[j][i]->SetTitle("Event multiplicity");
            hMult_ptbin[j][i]->GetXaxis()->SetTitle("N_{ch}");
            hMult_ptbin[j][i]->GetXaxis()->SetRangeUser(0, 100);
            hMult_ptbin[j][i]->GetYaxis()->SetTitle("Entries (normed to 1)");
            hMult_ptbin[j][i]->GetYaxis()->SetTitleSize(0.05);
            hMult_ptbin[j][i]->GetYaxis()->SetRangeUser(0.0001, 1);
            hMult_ptbin[j][i]->SetLineColor(ccolors[i]);
            hMult_ptbin[j][i]->SetLineStyle(j+1);
            hMult_ptbin[j][i]->Draw("HIST SAME");
            leg2->AddEntry(hMult_ptbin[j][i], Form("%s ZpT %d-%d", ((j==0) ? "data" : "MC[reco]"), ptbin_lo[i], ptbin_hi[i]), "l");
        }

        pad111->cd();
        TH1D* hRatio = (TH1D*)hMult_ptbin[1][i]->Clone(Form("ratio_ZMass_%d", i));
        hRatio->Divide(hMult_ptbin[0][i]);
        hRatio->SetTitle("");
        hRatio->SetStats(0);
        hRatio->SetLineStyle(1);
        hRatio->GetXaxis()->SetTitle("N_{ch}");
        hRatio->GetXaxis()->SetTitleSize(0.15);
        hRatio->GetXaxis()->SetLabelSize(0.08);
        hRatio->GetXaxis()->SetTitleOffset(0.4);
        hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
        hRatio->GetYaxis()->SetTitleSize(0.1);
        hRatio->GetYaxis()->SetLabelSize(0.08);
        hRatio->GetYaxis()->SetTitleOffset(0.4);
        hRatio->GetYaxis()->SetRangeUser(0, 4.5);
        hRatio->SetLineColor(ccolors[i]);

        // Calculate statistical errors for the ratio
        setErrors(hRatio, hMult_ptbin[1][i], hMult_ptbin[0][i]);
        hRatio->Draw("E SAME");

        TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    
    }
    pad91->cd();
    leg2->Draw("SAME");

    c1->cd(9);
    TPad *pad19 = new TPad("pad1_9", "pad1", 0, 0, 1, 1);
    pad19->SetLogy();
    pad19->Draw();
    for (int i = 0; i < 2; i++) {
        pad19->cd();
        hTrkWeight[i]->SetTitle("Trk weight dist, no norm");
        hTrkWeight[i]->GetXaxis()->SetTitle("weight");
        hTrkWeight[i]->GetYaxis()->SetTitle("counts / N_{Z}");
        hTrkWeight[i]->GetYaxis()->SetRangeUser(1, 2e3);
        hTrkWeight[i]->GetYaxis()->SetTitleSize(0.05);
        hTrkWeight[i]->SetLineColor(ccolors[i]);
        hTrkWeight[i]->Draw("HIST SAME");
    }
    pad19->cd();
    leg->Draw("SAME");

    c1->cd(10);
    TPad *pad10_1 = new TPad("pad10_1", "pad1", 0, 0.3, 1, 1);
    pad10_1->SetBottomMargin(0);
    pad10_1->SetLogy();
    pad10_1->Draw();
    TPad *pad10_2 = new TPad("pad10_2", "pad2", 0, 0, 1, 0.3);
    pad10_2->SetTopMargin(0);
    pad10_2->SetBottomMargin(0.2);
    pad10_2->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad10_1->cd();
        hMult[i]->SetTitle("Multiplicity Distribution");
        hMult[i]->GetXaxis()->SetTitle("N_{ch}");
        hMult[i]->GetXaxis()->SetRangeUser(0, 100);
        hMult[i]->GetYaxis()->SetTitle("Entries (normed to 1)");
        hMult[i]->GetYaxis()->SetTitleSize(0.05);
        hMult[i]->SetLineColor(ccolors[i]);
        if (i == 0) {
            hMult[i]->Draw("E SAME");
        } else {
            hMult[i]->Draw("HIST SAME");
        }

        pad10_2->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hMult[i]->Clone(Form("ratio_mult_%d", i));
            hRatio->Divide(hMult[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("N_{ch}");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("MC reco / %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 4);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hMult[i], hMult[baseline]);

            hRatio->Draw("E SAME"); // Add error bars

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad10_1->cd();
    leg->Draw("SAME");

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("diagnostic/Zcorr_pPb-%s.png", tag));

}
