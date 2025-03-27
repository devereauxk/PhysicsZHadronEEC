#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

const int rcolors[4] = {kRed-4, kOrange+1, kSpring-8, kTeal-2};
const int ccolors[4] = {kBlack,  kGreen, kViolet, kOrange+1};

void overlay_pt() {

    const int nptbins = 3;
    float ptbinlo[nptbins] = {4, 6, 10};
    TH3D* hLeadingVsZ_pp[nptbins];
    TH3D* hLeadingVsZ_PbPb[nptbins];

    // Open the ROOT files
    TFile *file_pp = new TFile("output/pp-4_20.root", "READ");
    TFile *file_PbPb = new TFile("output/PbPb0_30-4_20.root", "READ");
    TH1D* hNZ_pp = (TH1D*)file_pp->Get("hNZData");
    TH1D* hNZ_PbPb = (TH1D*)file_PbPb->Get("hNZData");

    for (int i = 0; i < nptbins; i++) {
        hLeadingVsZ_pp[i] = (TH3D*)file_pp->Get(Form("hLeadingVsZData_%i", static_cast<int>(ptbinlo[i])));
        hLeadingVsZ_PbPb[i] = (TH3D*)file_PbPb->Get(Form("hLeadingVsZData_%i", static_cast<int>(ptbinlo[i])));

        // apply normalization
        hLeadingVsZ_pp[i]->Scale(1. / hNZ_pp->Integral());
        hLeadingVsZ_PbPb[i]->Scale(1. / hNZ_PbPb->Integral());
    }

    // Create a canvas to draw the histogram
    TCanvas *c2 = new TCanvas("c2", "Canvas", 2400, 800);
    c2->Divide(3);

    c2->cd(1);
    TLegend *leg1 = new TLegend(0.15, 0.65, 0.35, 0.85);
    leg1->SetBorderSize(0); // Remove legend box
    leg1->SetTextSize(0.04); // Reduce font size

    TPad *pad1 = new TPad("pad1","pad1",0,0.3,1,1);
    pad1->SetBottomMargin(0);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.3);
    pad2->SetTopMargin(0);
    pad2->Draw();
    
    for(int i = 0; i < nptbins; i++) {
        pad1->cd();
        TH1D* hProjX = hLeadingVsZ_pp[i]->ProjectionX(Form("pp_eta_%i", i));

        hProjX->Scale(1. / hProjX->GetBinWidth(1)); // Scale by 1/binsize
        hProjX->SetLineColor(rcolors[i]); // Set color
        hProjX->SetTitle("deta");
        hProjX->SetStats(0); // Turn off statistics box
        hProjX->GetXaxis()->SetTitle("#Delta#eta");
        hProjX->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta#eta)");
        hProjX->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjX->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjX->GetYaxis()->SetRangeUser(0, 1.5); // Set plot range
        hProjX->Draw("HIST SAME");
        leg1->AddEntry(hProjX, Form("p_{T} > %g GeV", ptbinlo[i]), "l");

        TH1D* hProjX_PbPb = hLeadingVsZ_PbPb[i]->ProjectionX(Form("PbPb_eta_%i", i));
        hProjX_PbPb->Scale(1. / hProjX_PbPb->GetBinWidth(1)); // Scale by 1/binsize
        hProjX_PbPb->SetLineColor(rcolors[i]+1); // Set color
        hProjX_PbPb->SetLineStyle(2); // Set line style
        hProjX_PbPb->Draw("HIST SAME");

        pad2->cd();
        TH1D* hRatio = (TH1D*)hProjX_PbPb->Clone(Form("ratio_%i", i));
        hRatio->Divide(hProjX);
        hRatio->SetTitle("");
        hRatio->SetStats(0);
        hRatio->GetXaxis()->SetTitle("#Delta#eta");
        hRatio->GetXaxis()->SetTitleSize(0.1); // Increase font size
        hRatio->GetXaxis()->SetLabelSize(0.08); // Increase label size
        hRatio->GetXaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
        hRatio->GetYaxis()->SetTitle("PbPb/pp");
        hRatio->GetYaxis()->SetTitleSize(0.1); // Increase font size
        hRatio->GetYaxis()->SetLabelSize(0.06); // Increase label size
        hRatio->GetYaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
        hRatio->GetYaxis()->SetRangeUser(0, 3);
        hRatio->SetLineColor(rcolors[i]);
        hRatio->SetLineStyle(1);
        hRatio->Draw("HIST SAME");

        // Add horizontal dashed grey line at 0
        TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }
    pad1->cd();
    leg1->Draw("SAME");

    c2->cd(2);
    TLegend *leg2 = new TLegend(0.15, 0.65, 0.35, 0.85);
    leg2->SetBorderSize(0); // Remove legend box
    leg2->SetTextSize(0.04); // Reduce font size

    TPad *pad3 = new TPad("pad3","pad3",0,0.3,1,1);
    pad3->SetBottomMargin(0);
    pad3->Draw();
    TPad *pad4 = new TPad("pad4","pad4",0,0,1,0.3);
    pad4->SetTopMargin(0);
    pad4->Draw();

    for(int i = 0; i < nptbins; i++) {
        pad3->cd();
        TH1D *hProjY = hLeadingVsZ_pp[i]->ProjectionY(Form("pp_phi_%i", i));
        hProjY->Scale(1. / hProjY->GetBinWidth(1)); // Scale by 1/binsize
        hProjY->SetLineColor(rcolors[i]); // Set color
        hProjY->SetTitle("dphi");
        hProjY->SetStats(0); // Turn off statistics box
        hProjY->GetXaxis()->SetTitle("#Delta#phi");
        hProjY->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta#phi)");
        hProjY->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjY->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjY->GetYaxis()->SetRangeUser(0, 4); // Set plot range
        hProjY->Draw("HIST SAME"); // Draw histogram in step style
        leg2->AddEntry(hProjY, Form("p_{T} > %g GeV", ptbinlo[i]), "l");

        TH1D* hProjY_PbPb = hLeadingVsZ_PbPb[i]->ProjectionY(Form("PbPb_phi_%i", i));
        hProjY_PbPb->Scale(1. / hProjY_PbPb->GetBinWidth(1)); // Scale by 1/binsize
        hProjY_PbPb->SetLineColor(rcolors[i]+1); // Set color
        hProjY_PbPb->SetLineStyle(2); // Set line style
        hProjY_PbPb->Draw("HIST SAME");

        pad4->cd();
        TH1D* hRatioY = (TH1D*)hProjY_PbPb->Clone(Form("ratioY_%i", i));
        hRatioY->Divide(hProjY);
        hRatioY->SetTitle("");
        hRatioY->SetStats(0);
        hRatioY->GetXaxis()->SetTitle("#Delta#phi");
        hRatioY->GetXaxis()->SetTitleSize(0.1); // Increase font size
        hRatioY->GetXaxis()->SetLabelSize(0.08); // Increase label size
        hRatioY->GetXaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
        hRatioY->GetYaxis()->SetTitle("PbPb/pp");
        hRatioY->GetYaxis()->SetTitleSize(0.1); // Increase font size
        hRatioY->GetYaxis()->SetLabelSize(0.06); // Increase label size
        hRatioY->GetYaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
        hRatioY->GetYaxis()->SetRangeUser(0, 10);
        hRatioY->SetLineColor(rcolors[i]);
        hRatioY->SetLineStyle(1);
        hRatioY->Draw("HIST SAME");

        // Add horizontal dashed grey line at 0
        TLine *lineY = new TLine(hRatioY->GetXaxis()->GetXmin(), 1, hRatioY->GetXaxis()->GetXmax(), 1);
        lineY->SetLineColor(kGray+2);
        lineY->SetLineStyle(2);
        lineY->Draw("SAME");
    }
    pad3->cd();
    leg2->Draw("SAME");

    c2->cd(3);
    TLegend *leg3 = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg3->SetBorderSize(0); // Remove legend box
    leg3->SetTextSize(0.04); // Reduce font size

    TPad *pad5 = new TPad("pad5","pad5",0,0.3,1,1);
    pad5->SetBottomMargin(0);
    pad5->Draw();
    TPad *pad6 = new TPad("pad6","pad6",0,0,1,0.3);
    pad6->SetTopMargin(0);
    pad6->Draw();

    for(int i = 0; i < nptbins; i++) {
        pad5->cd();
        TH1D *hProjZ = hLeadingVsZ_pp[i]->ProjectionZ(Form("pp_R_%i", i));
        hProjZ->Scale(1. / hProjZ->GetBinWidth(1)); // Scale by 1/binsize
        hProjZ->SetLineColor(rcolors[i]); // Set color
        hProjZ->SetTitle("R");
        hProjZ->SetStats(0); // Turn off statistics box
        hProjZ->GetXaxis()->SetTitle("#Delta R");
        hProjZ->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta R)");
        hProjZ->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjZ->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjZ->GetXaxis()->SetRangeUser(0, 4); // Set plot range
        hProjZ->GetYaxis()->SetRangeUser(0, 2.4); // Set plot range
        hProjZ->Draw("HIST SAME"); // Draw histogram in step style
        leg3->AddEntry(hProjZ, Form("p_{T} > %g GeV", ptbinlo[i]), "l");

        TH1D* hProjZ_PbPb = hLeadingVsZ_PbPb[i]->ProjectionZ(Form("PbPb_R_%i", i));
        hProjZ_PbPb->Scale(1. / hProjZ_PbPb->GetBinWidth(1)); // Scale by 1/binsize
        hProjZ_PbPb->SetLineColor(rcolors[i]+1); // Set color
        hProjZ_PbPb->SetLineStyle(2); // Set line style
        hProjZ_PbPb->Draw("HIST SAME");

        pad6->cd();
        TH1D* hRatioZ = (TH1D*)hProjZ_PbPb->Clone(Form("ratioZ_%i", i));
        hRatioZ->Divide(hProjZ);
        hRatioZ->SetTitle("");
        hRatioZ->SetStats(0);
        hRatioZ->GetXaxis()->SetTitle("#Delta R");
        hRatioZ->GetXaxis()->SetTitleSize(0.1); // Increase font size
        hRatioZ->GetXaxis()->SetLabelSize(0.08); // Increase label size
        hRatioZ->GetXaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
        hRatioZ->GetXaxis()->SetRangeUser(0, 4);
        hRatioZ->GetYaxis()->SetTitle("PbPb/pp");
        hRatioZ->GetYaxis()->SetTitleSize(0.1); // Increase font size
        hRatioZ->GetYaxis()->SetLabelSize(0.06); // Increase label size
        hRatioZ->GetYaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
        hRatioZ->GetYaxis()->SetRangeUser(0, 6);
        hRatioZ->SetLineColor(rcolors[i]);
        hRatioZ->SetLineStyle(1);
        hRatioZ->Draw("HIST SAME");

        // Add horizontal dashed grey line at 0
        TLine *lineZ = new TLine(0, 1, hRatioZ->GetXaxis()->GetXmax(), 1);
        lineZ->SetLineColor(kGray+2);
        lineZ->SetLineStyle(2);
        lineZ->Draw("SAME");
    }
    pad5->cd();
    leg3->Draw("SAME");

    // Optionally: Save the canvas as an image
    c2->SaveAs("overlay_histogram.png");

    // Cleanup: Close the files
    file_pp->Close();
    file_PbPb->Close();
    delete file_pp;
    delete file_PbPb;
}


void overlay_generators() {

    int ptlo_select = 10;

    const int ncontours = 4;
    //const char * names[ncontours] = {"pp", "PbPb", "hybrid pp", "hybrid PbPb"};
    //const char * names[ncontours] = {"pp", "PbPb", "jewel pp", "jewel PbPb"};
    const char * names[ncontours] = {"pp", "PbPb", "pythia", "pythia+hydjet"};

    //const char * file_names[ncontours] = {"output/pp-4_20.root", "output/PbPb0_30-4_20.root", "output/hybridPP-4_20.root", "output/hybridPbPb030-4_20.root"};
    //const char * file_names[ncontours] = {"output/pp-4_20.root", "output/PbPb0_30-4_20.root",  "output/jewelPP-4_20.root", "output/jewelPbPb030-4_20.root"};
    const char * file_names[ncontours] = {"output/pp-4_20.root", "output/PbPb0_30-4_20.root",  "output/pythia-4_20.root", "output/DY0_30-4_20.root"};

    TH3D* hLeadingVsZ[ncontours];
    TH1D* hNZ[ncontours];

    for (int i = 0; i < ncontours; i++) {
        // Open the ROOT files
        TFile *file = new TFile(file_names[i], "READ");
        hLeadingVsZ[i] = (TH3D*)file->Get(Form("hLeadingVsZData_%i", static_cast<int>(ptlo_select)));
        hNZ[i] = (TH1D*)file->Get("hNZData");

        // apply normalization
        hLeadingVsZ[i]->Scale(1. / hNZ[i]->Integral());
    }

    // Create a canvas to draw the histogram
    TCanvas *c2 = new TCanvas("c2", "Canvas", 2400, 800);
    c2->Divide(3);

    // Plot for #Delta#eta
    c2->cd(1);
    TLegend *leg1 = new TLegend(0.15, 0.65, 0.35, 0.85);
    leg1->SetBorderSize(0); // Remove legend box
    leg1->SetTextSize(0.04); // Reduce font size

    TPad *pad1 = new TPad("pad1","pad1",0,0.3,1,1);
    pad1->SetBottomMargin(0);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.3);
    pad2->SetTopMargin(0);
    pad2->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad1->cd();
        TH1D* hProjX = hLeadingVsZ[i]->ProjectionX(Form("%s_eta", names[i]));
        hProjX->Scale(1. / hProjX->GetBinWidth(1)); // Scale by 1/binsize
        if (i == 0 || i == 1) {
            hProjX->SetLineColor(ccolors[0]); // Set color
            if (i == 1) hProjX->SetLineStyle(2); // Set line style for PbPb
        } else {
            hProjX->SetLineColor(ccolors[1]); // Set color
            if (i == 3) hProjX->SetLineStyle(2); // Set line style for pythia+hydjet
        }
        hProjX->SetTitle("deta");
        hProjX->SetStats(0); // Turn off statistics box
        hProjX->GetXaxis()->SetTitle("#Delta#eta");
        hProjX->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta#eta)");
        hProjX->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjX->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjX->GetYaxis()->SetRangeUser(0, 1.5); // Set plot range
        hProjX->Draw("HIST SAME");
        leg1->AddEntry(hProjX, names[i], "l");
    }
    leg1->Draw("SAME");

    pad2->cd();
    TH1D* hRatio1 = (TH1D*)hLeadingVsZ[1]->ProjectionX("PbPb1_eta")->Clone("ratio1_eta");
    hRatio1->Divide(hLeadingVsZ[0]->ProjectionX("pp1_eta"));
    hRatio1->SetTitle("");
    hRatio1->SetStats(0);
    hRatio1->GetXaxis()->SetTitle("#Delta#eta");
    hRatio1->GetXaxis()->SetTitleSize(0.1); // Increase font size
    hRatio1->GetXaxis()->SetLabelSize(0.08); // Increase label size
    hRatio1->GetXaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
    hRatio1->GetYaxis()->SetTitle("PbPb/pp");
    hRatio1->GetYaxis()->SetTitleSize(0.1); // Increase font size
    hRatio1->GetYaxis()->SetLabelSize(0.06); // Increase label size
    hRatio1->GetYaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
    hRatio1->GetYaxis()->SetRangeUser(0, 3);
    hRatio1->SetLineColor(ccolors[0]);
    hRatio1->SetLineStyle(1);
    hRatio1->Draw("HIST SAME");

    TH1D* hRatio2 = (TH1D*)hLeadingVsZ[3]->ProjectionX("pythia1_hydjet_eta")->Clone("ratio2_eta");
    hRatio2->Divide(hLeadingVsZ[2]->ProjectionX("pythia1_eta"));
    hRatio2->SetLineColor(ccolors[1]);
    hRatio2->SetLineStyle(1);
    hRatio2->Draw("HIST SAME");

    TLine *line = new TLine(hRatio1->GetXaxis()->GetXmin(), 1, hRatio1->GetXaxis()->GetXmax(), 1);
    line->SetLineColor(kGray+2);
    line->SetLineStyle(2);
    line->Draw("SAME");

    // Plot for #Delta#phi
    c2->cd(2);
    TLegend *leg2 = new TLegend(0.15, 0.65, 0.35, 0.85);
    leg2->SetBorderSize(0); // Remove legend box
    leg2->SetTextSize(0.04); // Reduce font size

    TPad *pad3 = new TPad("pad3","pad3",0,0.3,1,1);
    pad3->SetBottomMargin(0);
    pad3->Draw();
    TPad *pad4 = new TPad("pad4","pad4",0,0,1,0.3);
    pad4->SetTopMargin(0);
    pad4->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad3->cd();
        TH1D* hProjY = hLeadingVsZ[i]->ProjectionY(Form("%s_phi", names[i]));
        hProjY->Scale(1. / hProjY->GetBinWidth(1)); // Scale by 1/binsize
        if (i == 0 || i == 1) {
            hProjY->SetLineColor(ccolors[0]); // Set color
            if (i == 1) hProjY->SetLineStyle(2); // Set line style for PbPb
        } else {
            hProjY->SetLineColor(ccolors[1]); // Set color
            if (i == 3) hProjY->SetLineStyle(2); // Set line style for pythia+hydjet
        }
        hProjY->SetTitle("dphi");
        hProjY->SetStats(0); // Turn off statistics box
        hProjY->GetXaxis()->SetTitle("#Delta#phi");
        hProjY->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta#phi)");
        hProjY->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjY->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjY->GetYaxis()->SetRangeUser(0, 5); // Set plot range
        hProjY->Draw("HIST SAME");
        leg2->AddEntry(hProjY, names[i], "l");
    }
    leg2->Draw("SAME");

    pad4->cd();
    TH1D* hRatioY1 = (TH1D*)hLeadingVsZ[1]->ProjectionY("PbPb1_phi")->Clone("ratio1_phi");
    hRatioY1->Divide(hLeadingVsZ[0]->ProjectionY("pp1_phi"));
    hRatioY1->SetTitle("");
    hRatioY1->SetStats(0);
    hRatioY1->GetXaxis()->SetTitle("#Delta#phi");
    hRatioY1->GetXaxis()->SetTitleSize(0.1); // Increase font size
    hRatioY1->GetXaxis()->SetLabelSize(0.08); // Increase label size
    hRatioY1->GetXaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
    hRatioY1->GetYaxis()->SetTitle("PbPb/pp");
    hRatioY1->GetYaxis()->SetTitleSize(0.1); // Increase font size
    hRatioY1->GetYaxis()->SetLabelSize(0.06); // Increase label size
    hRatioY1->GetYaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
    hRatioY1->GetYaxis()->SetRangeUser(0, 10);
    hRatioY1->SetLineColor(ccolors[0]);
    hRatioY1->SetLineStyle(1);
    hRatioY1->Draw("HIST SAME");

    TH1D* hRatioY2 = (TH1D*)hLeadingVsZ[3]->ProjectionY("pythia1_hydjet_phi")->Clone("ratio2_phi");
    hRatioY2->Divide(hLeadingVsZ[2]->ProjectionY("pythia1_phi"));
    hRatioY2->SetLineColor(ccolors[1]);
    hRatioY2->SetLineStyle(1);
    hRatioY2->Draw("HIST SAME");

    TLine *lineY = new TLine(hRatioY1->GetXaxis()->GetXmin(), 1, hRatioY1->GetXaxis()->GetXmax(), 1);
    lineY->SetLineColor(kGray+2);
    lineY->SetLineStyle(2);
    lineY->Draw("SAME");

    // Plot for #Delta R
    c2->cd(3);
    TLegend *leg3 = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg3->SetBorderSize(0); // Remove legend box
    leg3->SetTextSize(0.04); // Reduce font size

    TPad *pad5 = new TPad("pad5","pad5",0,0.3,1,1);
    pad5->SetBottomMargin(0);
    pad5->Draw();
    TPad *pad6 = new TPad("pad6","pad6",0,0,1,0.3);
    pad6->SetTopMargin(0);
    pad6->Draw();

    for (int i = 0; i < ncontours; i++) {
        pad5->cd();
        TH1D* hProjZ = hLeadingVsZ[i]->ProjectionZ(Form("%s_R", names[i]));
        hProjZ->Scale(1. / hProjZ->GetBinWidth(1)); // Scale by 1/binsize
        if (i == 0 || i == 1) {
            hProjZ->SetLineColor(ccolors[0]); // Set color
            if (i == 1) hProjZ->SetLineStyle(2); // Set line style for PbPb
        } else {
            hProjZ->SetLineColor(ccolors[1]); // Set color
            if (i == 3) hProjZ->SetLineStyle(2); // Set line style for pythia+hydjet
        }
        hProjZ->SetTitle("R");
        hProjZ->SetStats(0); // Turn off statistics box
        hProjZ->GetXaxis()->SetTitle("#Delta R");
        hProjZ->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta R)");
        hProjZ->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjZ->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjZ->GetXaxis()->SetRangeUser(0, 4); // Set plot range
        hProjZ->GetYaxis()->SetRangeUser(0, 2.8); // Set plot range
        hProjZ->Draw("HIST SAME");
        leg3->AddEntry(hProjZ, names[i], "l");
    }
    leg3->Draw("SAME");

    pad6->cd();
    TH1D* hRatioZ1 = (TH1D*)hLeadingVsZ[1]->ProjectionZ("PbPb1_R")->Clone("ratio1_R");
    hRatioZ1->Divide(hLeadingVsZ[0]->ProjectionZ("pp1_R"));
    hRatioZ1->SetTitle("");
    hRatioZ1->SetStats(0);
    hRatioZ1->GetXaxis()->SetTitle("#Delta R");
    hRatioZ1->GetXaxis()->SetTitleSize(0.1); // Increase font size
    hRatioZ1->GetXaxis()->SetLabelSize(0.08); // Increase label size
    hRatioZ1->GetXaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
    hRatioZ1->GetYaxis()->SetTitle("PbPb/pp");
    hRatioZ1->GetYaxis()->SetTitleSize(0.1); // Increase font size
    hRatioZ1->GetYaxis()->SetLabelSize(0.06); // Increase label size
    hRatioZ1->GetYaxis()->SetTitleOffset(0.4); // Decrease distance between axis label and axis
    hRatioZ1->GetXaxis()->SetRangeUser(0, 4);
    hRatioZ1->GetYaxis()->SetRangeUser(0, 6);
    hRatioZ1->SetLineColor(ccolors[0]);
    hRatioZ1->SetLineStyle(1);
    hRatioZ1->Draw("HIST SAME");

    TH1D* hRatioZ2 = (TH1D*)hLeadingVsZ[3]->ProjectionZ("pythia1_hydjet_R")->Clone("ratio2_R");
    hRatioZ2->Divide(hLeadingVsZ[2]->ProjectionZ("pythia1_R"));
    hRatioZ2->SetLineColor(ccolors[1]);
    hRatioZ2->SetLineStyle(1);
    hRatioZ2->Draw("HIST SAME");

    TLine *lineZ = new TLine(0, 1, hRatioZ1->GetXaxis()->GetXmax(), 1);
    lineZ->SetLineColor(kGray+2);
    lineZ->SetLineStyle(2);
    lineZ->Draw("SAME");

    // Optionally: Save the canvas as an image
    c2->SaveAs("overlay_histogram_generators.png");

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
        TFile *file = new TFile(Form("output/%s_%s-%s.root", pp_names[i], zpt_select, pt_select), "READ");
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
    c1->SaveAs(Form("overlay_basic_pp_%s-%s.png", zpt_select, pt_select));

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
        TFile *file = new TFile(Form("output/%s_%s-%s.root", PbPb_names[i], zpt_select, pt_select), "READ");
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
    c1->SaveAs(Form("overlay_basic_PbPb_%s-%s.png", zpt_select, pt_select));

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
        TFile *file_PbPb = new TFile(Form("output/%s_%s-%s.root", PbPb_names[i], zpt_select, pt_select), "READ");
        TFile *file_pp = new TFile(Form("output/%s_%s-%s.root", pp_names[i], zpt_select, pt_select), "READ");

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
    c1->SaveAs(Form("overlay_basic_PbPb_pp_ratio_%s-%s.png", zpt_select, pt_select));
}

void plotOverlay() {
    //overlay_pt();
    //overlay_generators();

    const char* zpt_select[3] = {"ZPT40_350", "ZPT20_60", "ZPT80_350"};
    const char* pt_select = "1_40";

    for (int i = 0; i < 3; i++) {
        overlay_basic_pp(zpt_select[i], pt_select);
        overlay_basic_PbPb(zpt_select[i], pt_select);
        overlay_basic_PbPb_pp_ratio(zpt_select[i], pt_select);
    }   
}
