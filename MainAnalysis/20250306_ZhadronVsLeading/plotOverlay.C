#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

const int rcolors[4] = {kRed-4, kOrange+1, kSpring-8, kTeal-2};

void plotOverlay() {

    const int nptbins = 3;
    float ptbinlo[nptbins] = {4, 6, 10};
    TH3D* hLeadingVsZ_pp[nptbins];
    TH3D* hLeadingVsZ_PbPb[nptbins];
    TH1D* hNZ_pp[nptbins];
    TH1D* hNZ_PbPb[nptbins];

    // Open the ROOT files
    TFile *file_pp = new TFile("output_pp.root", "READ");
    TFile *file_PbPb = new TFile("output_PbPb.root", "READ");

    for (int i = 0; i < nptbins; i++) {
        hLeadingVsZ_pp[i] = (TH3D*)file_pp->Get(Form("hLeadingVsZData_%i", static_cast<int>(ptbinlo[i])));
        hNZ_pp[i] = (TH1D*)file_pp->Get(Form("hNZData_%i", static_cast<int>(ptbinlo[i])));
        hLeadingVsZ_PbPb[i] = (TH3D*)file_PbPb->Get(Form("hLeadingVsZData_%i", static_cast<int>(ptbinlo[i])));
        hNZ_PbPb[i] = (TH1D*)file_PbPb->Get(Form("hNZData_%i", static_cast<int>(ptbinlo[i])));

        // apply normalization
        hLeadingVsZ_pp[i]->Scale(1. / hNZ_pp[i]->GetBinContent(1));
        hLeadingVsZ_PbPb[i]->Scale(1. / hNZ_PbPb[i]->GetBinContent(1));
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
