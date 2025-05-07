#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

const int rcolors[7] = {kGreen+3, kYellow+1, kOrange, kRed, kMagenta, kViolet, kBlue};
const int ccolors[7] = {kGreen+3, kYellow+1, kOrange, kRed, kMagenta, kViolet, kBlue};

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

void overlay_generators(const char *zpt_select, const char *pt_select) {

    const int ncontours = 3;
    const char *pp_names[ncontours] = {"pythia", "jewelPP", "hybridPP"};
    const char *PbPb_names[ncontours] = {"DY0_30", "jewelPbPb030", "hybridPbPb030"};

    TH3D* hLeadingVsZ_pp[ncontours];
    TH3D* hLeadingVsZ_PbPb[ncontours];

    // Load histograms for pp
    for (int i = 0; i < ncontours; i++) {
        TFile *file = new TFile(Form("output/%s_ZPT%s-%s.root", pp_names[i], zpt_select, pt_select), "READ");
        hLeadingVsZ_pp[i] = (TH3D*)file->Get("hLeadingVsZData");
        TH1D* hNZ = (TH1D*)file->Get("hNZData");
        // Normalize histograms
        hLeadingVsZ_pp[i]->Scale(1. / hNZ->GetBinContent(1));
        hLeadingVsZ_pp[i]->Scale(0.5); // since each hist filled twice
    }

    // Load histograms for PbPb
    for (int i = 0; i < ncontours; i++) {
        TFile *file = new TFile(Form("output/%s_ZPT%s-%s.root", PbPb_names[i], zpt_select, pt_select), "READ");
        hLeadingVsZ_PbPb[i] = (TH3D*)file->Get("hLeadingVsZData");
        TH1D* hNZ = (TH1D*)file->Get("hNZData");
        // Normalize histograms
        hLeadingVsZ_PbPb[i]->Scale(1. / hNZ->GetBinContent(1));
        hLeadingVsZ_PbPb[i]->Scale(0.5); // since each hist filled twice
    }
    // Create a canvas to draw the histograms
    TCanvas *c2 = new TCanvas("c2", "Canvas", 1600, 800);
    c2->Divide(2);

    // Left panel: deta
    c2->cd(1);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();

    pad1->cd();
    TLegend *leg1 = new TLegend(0.15, 0.6, 0.35, 0.85);
    leg1->SetBorderSize(0);
    leg1->SetTextSize(0.04);

    for (int i = 0; i < ncontours; i++) {
        TH1D* hProjX_pp = hLeadingVsZ_pp[i]->ProjectionX(Form("pp_deta_%d", i));
        divideByWidth(hProjX_pp);
        hProjX_pp->SetStats(0);
        hProjX_pp->SetLineColor(rcolors[i]);
        hProjX_pp->SetLineStyle(1);
        hProjX_pp->SetTitle("deta");
        hProjX_pp->GetXaxis()->SetTitle("#Delta#eta");
        hProjX_pp->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta#eta)");
        hProjX_pp->GetYaxis()->SetTitleSize(0.06); // Increased font size
        hProjX_pp->GetYaxis()->SetRangeUser(0, 1.4);
        hProjX_pp->Draw("HIST SAME");
        leg1->AddEntry(hProjX_pp, Form("%s", pp_names[i]), "l");

        TH1D* hProjX_PbPb = hLeadingVsZ_PbPb[i]->ProjectionX(Form("PbPb_deta_%d", i));
        divideByWidth(hProjX_PbPb);
        hProjX_PbPb->SetStats(0);
        hProjX_PbPb->SetLineColor(rcolors[i]);
        hProjX_PbPb->SetLineStyle(2);
        hProjX_PbPb->Draw("HIST SAME");
        leg1->AddEntry(hProjX_PbPb, Form("%s", PbPb_names[i]), "l");
    }
    leg1->Draw();

    pad2->cd();
    for (int i = 0; i < ncontours; i++) {
        TH1D* hProjX_pp = hLeadingVsZ_pp[i]->ProjectionX(Form("pp_deta_%d2", i));
        TH1D* hProjX_PbPb = hLeadingVsZ_PbPb[i]->ProjectionX(Form("PbPb_deta_%d2", i));
        TH1D* hRatio = (TH1D*)hProjX_PbPb->Clone(Form("ratio_deta_%d", i));
        hRatio->Divide(hProjX_pp);
        hRatio->SetStats(0);
        hRatio->SetTitle("");
        hRatio->SetLineColor(rcolors[i]);
        hRatio->SetLineStyle(1);
        hRatio->GetXaxis()->SetTitle("#Delta#eta");
        hRatio->GetXaxis()->SetTitleSize(0.1);
        hRatio->GetXaxis()->SetLabelSize(0.08);
        hRatio->GetXaxis()->SetTitleOffset(0.4);
        hRatio->GetYaxis()->SetTitle("PbPb/pp");
        hRatio->GetYaxis()->SetTitleSize(0.1);
        hRatio->GetYaxis()->SetLabelSize(0.08);
        hRatio->GetYaxis()->SetTitleOffset(0.4);
        hRatio->GetYaxis()->SetRangeUser(0, 4);
        hRatio->Draw("HIST SAME");

        TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    // Right panel: dphi
    c2->cd(2);
    TPad *pad3 = new TPad("pad3", "pad3", 0, 0.3, 1, 1);
    pad3->SetBottomMargin(0);
    pad3->Draw();
    TPad *pad4 = new TPad("pad4", "pad4", 0, 0, 1, 0.3);
    pad4->SetTopMargin(0);
    pad4->SetBottomMargin(0.2);
    pad4->Draw();

    pad3->cd();
    TLegend *leg2 = new TLegend(0.15, 0.6, 0.35, 0.85);
    leg2->SetBorderSize(0);
    leg2->SetTextSize(0.04);

    for (int i = 0; i < ncontours; i++) {
        TH1D* hProjY_pp = hLeadingVsZ_pp[i]->ProjectionY(Form("pp_dphi_%d", i));
        divideByWidth(hProjY_pp);
        hProjY_pp->SetStats(0);
        hProjY_pp->SetLineColor(rcolors[i]);
        hProjY_pp->SetLineStyle(1);
        hProjY_pp->SetTitle("dphi");
        hProjY_pp->GetXaxis()->SetTitle("#Delta#phi");
        hProjY_pp->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta#phi)");
        hProjY_pp->GetYaxis()->SetTitleSize(0.06); // Increased font size
        hProjY_pp->GetYaxis()->SetRangeUser(0, 2.5);
        hProjY_pp->Draw("HIST SAME");
        leg2->AddEntry(hProjY_pp, Form("%s", pp_names[i]), "l");

        TH1D* hProjY_PbPb = hLeadingVsZ_PbPb[i]->ProjectionY(Form("PbPb_dphi_%d", i));
        divideByWidth(hProjY_PbPb);
        hProjY_PbPb->SetStats(0);
        hProjY_PbPb->SetLineColor(rcolors[i]);
        hProjY_PbPb->SetLineStyle(2);
        hProjY_PbPb->Draw("HIST SAME");
        leg2->AddEntry(hProjY_PbPb, Form("%s", PbPb_names[i]), "l");
    }
    leg2->Draw();

    pad4->cd();
    for (int i = 0; i < ncontours; i++) {
        TH1D* hProjY_pp = hLeadingVsZ_pp[i]->ProjectionY(Form("pp_dphi_%d2", i));
        TH1D* hProjY_PbPb = hLeadingVsZ_PbPb[i]->ProjectionY(Form("PbPb_dphi_%d2", i));
        TH1D* hRatio = (TH1D*)hProjY_PbPb->Clone(Form("ratio_dphi_%d", i));
        hRatio->Divide(hProjY_pp);
        hRatio->SetStats(0);
        hRatio->SetTitle("");
        hRatio->SetLineColor(rcolors[i]);
        hRatio->SetLineStyle(1);
        hRatio->GetXaxis()->SetTitle("#Delta#phi");
        hRatio->GetXaxis()->SetTitleSize(0.1);
        hRatio->GetXaxis()->SetLabelSize(0.08);
        hRatio->GetXaxis()->SetTitleOffset(0.4);
        hRatio->GetYaxis()->SetTitle("PbPb/pp");
        hRatio->GetYaxis()->SetTitleSize(0.1);
        hRatio->GetYaxis()->SetLabelSize(0.08);
        hRatio->GetYaxis()->SetTitleOffset(0.4);
        hRatio->GetYaxis()->SetRangeUser(0, 8);
        hRatio->Draw("HIST SAME");

        TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    // Save the canvas as an image
    c2->SaveAs(Form("diagnostic/overlay_generators_%s-%s.png", zpt_select, pt_select));
}

void overlay_basic_pp(const char *zpt_select, const char *pt_select) {

    const int ncontours = 4;
    const char *pp_names[ncontours] = {"pp", "pythia", "jewelPP", "hybridPP"};
    const char *PbPb_names[ncontours] = {"PbPb0_30", "DY0_30", "jewelPbPb030", "hybridPbPb030"};

    TH1D* hTrkPt[ncontours];
    TH1D* hLeadingPt[ncontours];
    TH1D* hTrkEta[ncontours];
    TH1D* hLeadingEta[ncontours];
    TH1D* hZPt[ncontours];
    TH1D* hZMass[ncontours];
    TH1D* hNZ[ncontours];
    
    // Load histograms for pp
    for (int i = 0; i < ncontours; i++) {
        TFile *file = new TFile(Form("output/%s_ZPT%s-%s.root", pp_names[i], zpt_select, pt_select), "READ");
        hTrkPt[i] = (TH1D*)file->Get("hTrkPtData");
        hLeadingPt[i] = (TH1D*)file->Get("hLeadingPtData");
        hTrkEta[i] = (TH1D*)file->Get("hTrkEtaData");
        hLeadingEta[i] = (TH1D*)file->Get("hLeadingEtaData");
        hZPt[i] = (TH1D*)file->Get("hZPtData");
        hZMass[i] = (TH1D*)file->Get("hZMassData");
        hNZ[i] = (TH1D*)file->Get("hNZData");

        // Normalize histograms
        double integral = hNZ[i]->GetBinContent(1);
        hTrkPt[i]->Scale(1. / integral);
        hLeadingPt[i]->Scale(1. / integral);
        hTrkEta[i]->Scale(1. / integral);
        hLeadingEta[i]->Scale(1. / integral);
        hZPt[i]->Scale(1. / integral);
        hZMass[i]->Scale(1. / integral);

        // divide by bin width
        divideByWidth(hTrkPt[i]);
        divideByWidth(hLeadingPt[i]);
        divideByWidth(hTrkEta[i]);
        divideByWidth(hLeadingEta[i]);
        divideByWidth(hZPt[i]);
        divideByWidth(hZMass[i]);

        // Set stats off
        hTrkPt[i]->SetStats(0);
        hLeadingPt[i]->SetStats(0);
        hTrkEta[i]->SetStats(0);
        hLeadingEta[i]->SetStats(0);
        hZPt[i]->SetStats(0);
        hZMass[i]->SetStats(0);
    }

    // Create a canvas to draw the histograms for pp
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1600, 2000);
    c1->Divide(2, 3);

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
        hTrkPt[i]->GetXaxis()->SetRangeUser(0, 10);
        hTrkPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hTrkPt[i]->SetLineColor(ccolors[i]);
        hTrkPt[i]->Draw("HIST SAME");

        pad2->cd();
        if (i != 1) { // Use pythia (index 1) as the baseline
            TH1D* hRatio = (TH1D*)hTrkPt[i]->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hTrkPt[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetRangeUser(0,10);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    c1->cd(2);
    TPad *pad3 = new TPad("pad1_2", "pad1", 0, 0.3, 1, 1);
    pad3->SetBottomMargin(0);
    pad3->SetLogy();
    pad3->Draw();
    TPad *pad4 = new TPad("pad2_2", "pad2", 0, 0, 1, 0.3);
    pad4->SetTopMargin(0);
    pad4->SetBottomMargin(0.2);
    pad4->Draw();
    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size
    for (int i = 0; i < ncontours; i++) {
        pad3->cd();
        hLeadingPt[i]->SetTitle("Leading Track pT");
        hLeadingPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hLeadingPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hLeadingPt[i]->SetLineColor(ccolors[i]);
        hLeadingPt[i]->Draw("HIST SAME");
        leg->AddEntry(hLeadingPt[i], pp_names[i], "l");

        pad4->cd();
        if (i != 1) { // Use pythia (index 1) as the baseline
            TH1D* hRatio = (TH1D*)hLeadingPt[i]->Clone(Form("ratio_LeadingPt_%d", i));
            hRatio->Divide(hLeadingPt[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

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
        hZPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hZPt[i]->SetLineColor(ccolors[i]);
        hZPt[i]->Draw("HIST SAME");

        pad6->cd();
        if (i != 1) { // Use pythia (index 1) as the baseline
            TH1D* hRatio = (TH1D*)hZPt[i]->Clone(Form("ratio_ZPt_%d", i));
            hRatio->Divide(hZPt[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    c1->cd(4);
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
        hZMass[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hZMass[i]->SetLineColor(ccolors[i]);
        hZMass[i]->Draw("HIST SAME");

        pad8->cd();
        if (i != 1) { // Use pythia (index 1) as the baseline
            TH1D* hRatio = (TH1D*)hZMass[i]->Clone(Form("ratio_ZMass_%d", i));
            hRatio->Divide(hZMass[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Mass (GeV/c^2)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("generator/pythia");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

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
        hTrkEta[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hTrkEta[i]->SetLineColor(ccolors[i]);
        hTrkEta[i]->Draw("HIST SAME");

        pad10->cd();
        if (i != 1) { // Use pythia (index 1) as the baseline
            TH1D* hRatio = (TH1D*)hTrkEta[i]->Clone(Form("ratio_TrkEta_%d", i));
            hRatio->Divide(hTrkEta[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Eta");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

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
        hLeadingEta[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hLeadingEta[i]->SetLineColor(ccolors[i]);
        hLeadingEta[i]->Draw("HIST SAME");

        pad12->cd();
        if (i != 1) { // Use pythia (index 1) as the baseline
            TH1D* hRatio = (TH1D*)hLeadingEta[i]->Clone(Form("ratio_LeadingEta_%d", i));
            hRatio->Divide(hLeadingEta[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Eta");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("diagnostic/overlay_basic_pp_%s-%s.png", zpt_select, pt_select));

}

void overlay_basic_PbPb(const char *zpt_select, const char *pt_select) {

    const int ncontours = 4;
    const char *pp_names[ncontours] = {"pp", "pythia", "jewelPP", "hybridPP"};
    const char *PbPb_names[ncontours] = {"PbPb0_30", "DY0_30", "jewelPbPb030", "hybridPbPb030"};

    TH1D* hTrkPt[ncontours];
    TH1D* hLeadingPt[ncontours];
    TH1D* hTrkEta[ncontours];
    TH1D* hLeadingEta[ncontours];
    TH1D* hZPt[ncontours];
    TH1D* hZMass[ncontours];
    TH1D* hNZ[ncontours];

    // Load histograms for PbPb
    for (int i = 0; i < ncontours; i++) {
        TFile *file = new TFile(Form("output/%s_ZPT%s-%s.root", PbPb_names[i], zpt_select, pt_select), "READ");
        hTrkPt[i] = (TH1D*)file->Get("hTrkPtData");
        hLeadingPt[i] = (TH1D*)file->Get("hLeadingPtData");
        hTrkEta[i] = (TH1D*)file->Get("hTrkEtaData");
        hLeadingEta[i] = (TH1D*)file->Get("hLeadingEtaData");
        hZPt[i] = (TH1D*)file->Get("hZPtData");
        hZMass[i] = (TH1D*)file->Get("hZMassData");
        hNZ[i] = (TH1D*)file->Get("hNZData");

        // Normalize histograms
        double integral = hNZ[i]->GetBinContent(1);
        hTrkPt[i]->Scale(1. / integral);
        hLeadingPt[i]->Scale(1. / integral);
        hTrkEta[i]->Scale(1. / integral);
        hLeadingEta[i]->Scale(1. / integral);
        hZPt[i]->Scale(1. / integral);
        hZMass[i]->Scale(1. / integral);

        // divide by bin width
        divideByWidth(hTrkPt[i]);
        divideByWidth(hLeadingPt[i]);
        divideByWidth(hTrkEta[i]);
        divideByWidth(hLeadingEta[i]);
        divideByWidth(hZPt[i]);
        divideByWidth(hZMass[i]);

        // Set stats off
        hTrkPt[i]->SetStats(0);
        hLeadingPt[i]->SetStats(0);
        hTrkEta[i]->SetStats(0);
        hLeadingEta[i]->SetStats(0);
        hZPt[i]->SetStats(0);
        hZMass[i]->SetStats(0);
    }

    // Create a canvas to draw the histograms for PbPb
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1600, 2000);
    c1->Divide(2, 3);

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
        hTrkPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hTrkPt[i]->SetLineColor(ccolors[i]);
        hTrkPt[i]->Draw("HIST SAME");

        pad2->cd();
        if (i != 1) {
            TH1D* hRatio = (TH1D*)hTrkPt[i]->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hTrkPt[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    c1->cd(2);
    TPad *pad3 = new TPad("pad1_2", "pad1", 0, 0.3, 1, 1);
    pad3->SetBottomMargin(0);
    pad3->SetLogy();
    pad3->Draw();
    TPad *pad4 = new TPad("pad2_2", "pad2", 0, 0, 1, 0.3);
    pad4->SetTopMargin(0);
    pad4->SetBottomMargin(0.2);
    pad4->Draw();

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    for (int i = 0; i < ncontours; i++) {
        pad3->cd();
        hLeadingPt[i]->SetTitle("Leading Track pT");
        hLeadingPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hLeadingPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hLeadingPt[i]->SetLineColor(ccolors[i]);
        hLeadingPt[i]->Draw("HIST SAME");
        leg->AddEntry(hLeadingPt[i], PbPb_names[i], "l");

        pad4->cd();
        if (i != 1) {
            TH1D* hRatio = (TH1D*)hLeadingPt[i]->Clone(Form("ratio_LeadingPt_%d", i));
            hRatio->Divide(hLeadingPt[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

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
        hZPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hZPt[i]->SetLineColor(ccolors[i]);
        hZPt[i]->Draw("HIST SAME");

        pad6->cd();
        if (i != 1) {
            TH1D* hRatio = (TH1D*)hZPt[i]->Clone(Form("ratio_ZPt_%d", i));
            hRatio->Divide(hZPt[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    c1->cd(4);
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
        hZMass[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hZMass[i]->SetLineColor(ccolors[i]);
        hZMass[i]->Draw("HIST SAME");

        pad8->cd();
        if (i != 1) {
            TH1D* hRatio = (TH1D*)hZMass[i]->Clone(Form("ratio_ZMass_%d", i));
            hRatio->Divide(hZMass[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Mass (GeV/c^2)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

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
        hTrkEta[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hTrkEta[i]->SetLineColor(ccolors[i]);
        hTrkEta[i]->Draw("HIST SAME");

        pad10->cd();
        if (i != 1) {
            TH1D* hRatio = (TH1D*)hTrkEta[i]->Clone(Form("ratio_TrkEta_%d", i));
            hRatio->Divide(hTrkEta[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Eta");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

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
        hLeadingEta[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hLeadingEta[i]->SetLineColor(ccolors[i]);
        hLeadingEta[i]->Draw("HIST SAME");

        pad12->cd();
        if (i != 1) {
            TH1D* hRatio = (TH1D*)hLeadingEta[i]->Clone(Form("ratio_LeadingEta_%d", i));
            hRatio->Divide(hLeadingEta[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Eta");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("diagnostic/overlay_basic_PbPb_%s-%s.png", zpt_select, pt_select));

}

void overlay_basic_pPb(const char *zpt_select, const char *pt_select, const char *pPb_name[], int ncontours, int baseline, const char* tag = "") {

    TH1D* hTrkPt[ncontours];
    TH1D* hLeadingPt[ncontours];
    TH1D* hTrkEta[ncontours];
    TH1D* hLeadingEta[ncontours];
    TH1D* hZPt[ncontours];
    TH1D* hZEta[ncontours];
    TH1D* hZMass[ncontours];
    TH1D* hNZ[ncontours];

    // Load histograms for PbPb
    for (int i = 0; i < ncontours; i++) {
        TFile *file = new TFile(Form("output/%s_ZPT%s-%s.root", pPb_name[i], zpt_select, pt_select), "READ");
        hTrkPt[i] = (TH1D*)file->Get("hTrkPtData");
        hLeadingPt[i] = (TH1D*)file->Get("hLeadingPtData");
        hTrkEta[i] = (TH1D*)file->Get("hTrkEtaData");
        hLeadingEta[i] = (TH1D*)file->Get("hLeadingEtaData");
        hZMass[i] = (TH1D*)file->Get("hZMassData");
        hNZ[i] = (TH1D*)file->Get("hNZData");

        TH2D *hZPtEta = (TH2D*)file->Get("hZPtEtaData");
        hZPt[i] = (TH1D*)hZPtEta->ProjectionX("hZPtData");
        hZEta[i] = (TH1D*)hZPtEta->ProjectionY("hZEtaData");

        // Normalize histograms
        double integral = hNZ[i]->GetBinContent(1);
        hTrkPt[i]->Scale(1. / integral);
        hLeadingPt[i]->Scale(1. / integral);
        hTrkEta[i]->Scale(1. / integral);
        hLeadingEta[i]->Scale(1. / integral);
        hZPt[i]->Scale(1. / integral);
        hZEta[i]->Scale(1. / integral);
        hZMass[i]->Scale(1. / integral);

        // divide by bin width
        divideByWidth(hTrkPt[i]);
        divideByWidth(hLeadingPt[i]);
        divideByWidth(hTrkEta[i]);
        divideByWidth(hLeadingEta[i]);
        divideByWidth(hZPt[i]);
        divideByWidth(hZEta[i]);
        divideByWidth(hZMass[i]);

        // Set stats off
        hTrkPt[i]->SetStats(0);
        hLeadingPt[i]->SetStats(0);
        hTrkEta[i]->SetStats(0);
        hLeadingEta[i]->SetStats(0);
        hZPt[i]->SetStats(0);
        hZEta[i]->SetStats(0);
        hZMass[i]->SetStats(0);
    }

    // Create a canvas to draw the histograms for PbPb
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1600, 2300);
    c1->Divide(2, 4);

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
        hTrkPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hTrkPt[i]->SetLineColor(ccolors[i]);
        hTrkPt[i]->Draw("HIST SAME");

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
            hRatio->GetYaxis()->SetTitle(Form("ratio wrt %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    c1->cd(2);
    TPad *pad3 = new TPad("pad1_2", "pad1", 0, 0.3, 1, 1);
    pad3->SetBottomMargin(0);
    pad3->SetLogy();
    pad3->Draw();
    TPad *pad4 = new TPad("pad2_2", "pad2", 0, 0, 1, 0.3);
    pad4->SetTopMargin(0);
    pad4->SetBottomMargin(0.2);
    pad4->Draw();

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    for (int i = 0; i < ncontours; i++) {
        pad3->cd();
        hLeadingPt[i]->SetTitle("Leading Track pT");
        hLeadingPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hLeadingPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hLeadingPt[i]->SetLineColor(ccolors[i]);
        hLeadingPt[i]->Draw("HIST SAME");
        leg->AddEntry(hLeadingPt[i], pPb_name[i], "l");

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
            hRatio->GetYaxis()->SetTitle(Form("ratio wrt %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

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
        hZPt[i]->GetXaxis()->SetRangeUser(0, 20);
        hZPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hZPt[i]->SetLineColor(ccolors[i]);
        hZPt[i]->Draw("HIST SAME");

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
            hRatio->GetYaxis()->SetTitle(Form("ratio wrt %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

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
        hZEta[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hZEta[i]->SetLineColor(ccolors[i]);
        hZEta[i]->Draw("HIST SAME");

        pad81->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hZEta[i]->Clone(Form("ratio_ZEta_%d", i));
            hRatio->Divide(hZEta[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("Mass (GeV/c^2)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(Form("ratio wrt %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

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
        hTrkEta[i]->GetYaxis()->SetTitle("Entries / N_Z");
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
            hRatio->GetYaxis()->SetTitle(Form("ratio wrt %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

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
        hLeadingEta[i]->GetYaxis()->SetTitle("Entries / N_Z");
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
            hRatio->GetYaxis()->SetTitle(Form("ratio wrt %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
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
        hZMass[i]->GetYaxis()->SetTitle("Entries / N_Z");
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
            hRatio->GetYaxis()->SetTitle(Form("ratio wrt %s", pPb_name[baseline]));
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("diagnostic/overlay_basic_pPb_%s-%s-%s.png", zpt_select, pt_select, tag));

}

void overlay_basic_PbPb_pp_ratio(const char *zpt_select, const char *pt_select) {

    const int ncontours = 4;
    const char *pp_names[ncontours] = {"pp", "pythia", "jewelPP", "hybridPP"};
    const char *PbPb_names[ncontours] = {"PbPb0_30", "DY0_30", "jewelPbPb030", "hybridPbPb030"};

    TH1D* hTrkPt[ncontours];
    TH1D* hLeadingPt[ncontours];
    TH1D* hTrkEta[ncontours];
    TH1D* hLeadingEta[ncontours];
    TH1D* hZPt[ncontours];
    TH1D* hZMass[ncontours];
    TH1D* hNZ_PbPb[ncontours];
    TH1D* hNZ_pp[ncontours];

    // Load histograms for PbPb and pp
    for (int i = 0; i < ncontours; i++) {
        TFile *file_PbPb = new TFile(Form("output/%s_ZPT%s-%s.root", PbPb_names[i], zpt_select, pt_select), "READ");
        TFile *file_pp = new TFile(Form("output/%s_ZPT%s-%s.root", pp_names[i], zpt_select, pt_select), "READ");

        hTrkPt[i] = (TH1D*)file_PbPb->Get("hTrkPtData");
        hLeadingPt[i] = (TH1D*)file_PbPb->Get("hLeadingPtData");
        hTrkEta[i] = (TH1D*)file_PbPb->Get("hTrkEtaData");
        hLeadingEta[i] = (TH1D*)file_PbPb->Get("hLeadingEtaData");
        hZPt[i] = (TH1D*)file_PbPb->Get("hZPtData");
        hZMass[i] = (TH1D*)file_PbPb->Get("hZMassData");
        hNZ_PbPb[i] = (TH1D*)file_PbPb->Get("hNZData");
        hNZ_pp[i] = (TH1D*)file_pp->Get("hNZData");

        // Normalize histograms
        double integral_PbPb = hNZ_PbPb[i]->GetBinContent(1);
        double integral_pp = hNZ_pp[i]->GetBinContent(1);
        double scale_factor = integral_pp / integral_PbPb;

        // Divide by corresponding pp histograms and scale by (N_Z_pp / N_Z_PbPb)
        hTrkPt[i]->Divide((TH1D*)file_pp->Get("hTrkPtData"));
        hTrkPt[i]->Scale(scale_factor);
        hLeadingPt[i]->Divide((TH1D*)file_pp->Get("hLeadingPtData"));
        hLeadingPt[i]->Scale(scale_factor);
        hTrkEta[i]->Divide((TH1D*)file_pp->Get("hTrkEtaData"));
        hTrkEta[i]->Scale(scale_factor);
        hLeadingEta[i]->Divide((TH1D*)file_pp->Get("hLeadingEtaData"));
        hLeadingEta[i]->Scale(scale_factor);
        hZPt[i]->Divide((TH1D*)file_pp->Get("hZPtData"));
        hZPt[i]->Scale(scale_factor);
        hZMass[i]->Divide((TH1D*)file_pp->Get("hZMassData"));
        hZMass[i]->Scale(scale_factor);

        // Set stats off
        hTrkPt[i]->SetStats(0);
        hLeadingPt[i]->SetStats(0);
        hTrkEta[i]->SetStats(0);
        hLeadingEta[i]->SetStats(0);
        hZPt[i]->SetStats(0);
        hZMass[i]->SetStats(0);
    }
    // Create a canvas to draw the histograms for PbPb/pp ratios
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1600, 2000);
    c1->Divide(2, 3);

    c1->cd(1);
    for (int i = 0; i < ncontours; i++) {
        hTrkPt[i]->SetTitle("Track pT Ratio");
        hTrkPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hTrkPt[i]->GetYaxis()->SetTitle("PbPb/pp");
        hTrkPt[i]->GetYaxis()->SetRangeUser(0, 3);
        hTrkPt[i]->SetLineColor(ccolors[i]);
        hTrkPt[i]->Draw("HIST SAME");

        TLine *line = new TLine(hTrkPt[i]->GetXaxis()->GetXmin(), 1, hTrkPt[i]->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    c1->cd(2);
    for (int i = 0; i < ncontours; i++) {
        hLeadingPt[i]->SetTitle("Leading Track pT Ratio");
        hLeadingPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hLeadingPt[i]->GetYaxis()->SetTitle("PbPb/pp");
        hLeadingPt[i]->GetYaxis()->SetRangeUser(0, 3);
        hLeadingPt[i]->SetLineColor(ccolors[i]);
        hLeadingPt[i]->Draw("HIST SAME");

        TLine *line = new TLine(hLeadingPt[i]->GetXaxis()->GetXmin(), 1, hLeadingPt[i]->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    c1->cd(3);
    for (int i = 0; i < ncontours; i++) {
        hZPt[i]->SetTitle("Z pT Ratio");
        hZPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hZPt[i]->GetYaxis()->SetTitle("PbPb/pp");
        hZPt[i]->GetYaxis()->SetRangeUser(0, 3);
        hZPt[i]->SetLineColor(ccolors[i]);
        hZPt[i]->Draw("HIST SAME");

        TLine *line = new TLine(hZPt[i]->GetXaxis()->GetXmin(), 1, hZPt[i]->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    c1->cd(4);
    for (int i = 0; i < ncontours; i++) {
        hZMass[i]->SetTitle("Z Mass Ratio");
        hZMass[i]->GetXaxis()->SetTitle("Mass (GeV/c^2)");
        hZMass[i]->GetYaxis()->SetTitle("PbPb/pp");
        hZMass[i]->GetYaxis()->SetRangeUser(0, 3);
        hZMass[i]->SetLineColor(ccolors[i]);
        hZMass[i]->Draw("HIST SAME");

        TLine *line = new TLine(hZMass[i]->GetXaxis()->GetXmin(), 1, hZMass[i]->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    c1->cd(5);
    for (int i = 0; i < ncontours; i++) {
        hTrkEta[i]->SetTitle("Track Eta Ratio");
        hTrkEta[i]->GetXaxis()->SetTitle("Eta");
        hTrkEta[i]->GetYaxis()->SetTitle("PbPb/pp");
        hTrkEta[i]->GetYaxis()->SetRangeUser(0, 3);
        hTrkEta[i]->SetLineColor(ccolors[i]);
        hTrkEta[i]->Draw("HIST SAME");

        TLine *line = new TLine(hTrkEta[i]->GetXaxis()->GetXmin(), 1, hTrkEta[i]->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    c1->cd(6);
    for (int i = 0; i < ncontours; i++) {
        hLeadingEta[i]->SetTitle("Leading Track Eta Ratio");
        hLeadingEta[i]->GetXaxis()->SetTitle("Eta");
        hLeadingEta[i]->GetYaxis()->SetTitle("PbPb/pp");
        hLeadingEta[i]->GetYaxis()->SetRangeUser(0, 3);
        hLeadingEta[i]->SetLineColor(ccolors[i]);
        hLeadingEta[i]->Draw("HIST SAME");

        TLine *line = new TLine(hLeadingEta[i]->GetXaxis()->GetXmin(), 1, hLeadingEta[i]->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    // Add legend to the leading track pT plot
    c1->cd(2);
    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size
    const char *ratio_labels[ncontours] = {"data ratio", "pythia ratio", "jewel ratio", "hybrid ratio"};
    for (int i = 0; i < ncontours; i++) {
        leg->AddEntry(hLeadingPt[i], ratio_labels[i], "l");
    }
    leg->Draw("SAME");

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("diagnostic/overlay_basic_PbPb_pp_ratio_%s-%s.png", zpt_select, pt_select));
}

void plotOverlay(const char* zpt_select, const char* pt_select) {

    //overlay_generators(zpt_select, pt_select);

    //overlay_basic_pp(zpt_select, pt_select);
    //overlay_basic_PbPb(zpt_select, pt_select);
    //overlay_basic_PbPb_pp_ratio(zpt_select[i], pt_select[j]);

    const int ncontours = 3;
    //const char *pPb_name[ncontours] = {"pp", "pPb", "pPb", "pPbMC", "pPbMC_Gen", "PbPMC", "PbPMC_Gen"};
    const char *pPb_name[ncontours] = {"1pPb", "pPbMC", "pPbMC_Gen"};
    int baseline = 1; // Define baseline
    overlay_basic_pPb(zpt_select, pt_select, pPb_name, ncontours, baseline, "PPb");

    const char *pPb_name_0[ncontours] = {"0pPb", "PbPMC", "PbPMC_Gen"};
    //overlay_basic_pPb(zpt_select, pt_select, pPb_name_0, ncontours, baseline, "PbP");

}
