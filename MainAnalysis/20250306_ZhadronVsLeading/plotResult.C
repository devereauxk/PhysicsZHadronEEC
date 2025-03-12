#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

const int rcolors[4] = {kRed-4, kOrange+1, kSpring-8, kTeal-2};

void plotResult(const char* input, const char* output = "") {
    // Open the ROOT file
    TFile *file = new TFile(input, "READ");

    // Retrieve histograms from the file
    TH1D *hTrkPt = (TH1D*)file->Get("hTrkPtData");
    TH1D *hLeadingPt = (TH1D*)file->Get("hLeadingPtData");
    TH1D *hZPt = (TH1D*)file->Get("hZPtData");
    TH1D *hZMass = (TH1D*)file->Get("hZMassData");

    const int nptbins = 4;
    float ptbinlo[nptbins] = {4, 5, 6, 10};
    TH3D* hLeadingVsZ[nptbins];
    TH1D* hNZ[nptbins];
    for (int i = 0; i < nptbins; i++) {
        hLeadingVsZ[i] = (TH3D*)file->Get(Form("hLeadingVsZData_%i", static_cast<int>(ptbinlo[i])));
        hNZ[i] = (TH1D*)file->Get(Form("hNZData_%i", static_cast<int>(ptbinlo[i])));

        // apply normalization
        hLeadingVsZ[i]->Scale(1. / hNZ[i]->GetBinContent(1));
    }

    // Check if histograms are loaded
    if (!hTrkPt || !hLeadingPt || !hZPt || !hZMass) {
        std::cerr << "Error: Histogram not found in file" << std::endl;
        return;
    }


    // Subtract hMixData from hData
    //hData->Add(hMixData, -1);

    // Set titles and axis labels
    hTrkPt->SetTitle("Track pT");
    hTrkPt->GetXaxis()->SetTitle("pT (GeV/c)");
    hTrkPt->GetYaxis()->SetTitle("Entries");

    hLeadingPt->SetTitle("Leading Track pT");
    hLeadingPt->GetXaxis()->SetTitle("pT (GeV/c)");
    hLeadingPt->GetYaxis()->SetTitle("Entries");

    hZPt->SetTitle("Z pT Distribution");
    hZPt->GetXaxis()->SetTitle("pT (GeV/c)");
    hZPt->GetYaxis()->SetTitle("Entries");

    hZMass->SetTitle("Z Mass");
    hZMass->GetXaxis()->SetTitle("Mass (GeV/c^2)");
    hZMass->GetYaxis()->SetTitle("Entries");

    // Create a canvas to draw the histogram
    TCanvas *c1 = new TCanvas("c1", "Canvas", 1600, 1200);
    c1->Divide(2, 2);
    
    // Draw each histogram in a separate pad with log scale on y-axis
    c1->cd(1);
    gPad->SetLogy();
    hTrkPt->Draw();

    c1->cd(2);
    gPad->SetLogy();
    hLeadingPt->Draw();

    c1->cd(3);
    gPad->SetLogy();
    hZPt->Draw();

    c1->cd(4);
    gPad->SetLogy();
    hZMass->Draw();

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("randoinfo_%s.png", output));

    // Create a canvas to draw the histogram
    TCanvas *c2 = new TCanvas("c2", "Canvas", 1600, 1200);
    c2->Divide(2, 2);

    c2->cd(1);
    TLegend *leg1 = new TLegend(0.15, 0.65, 0.35, 0.85);
    leg1->SetBorderSize(0); // Remove legend box
    leg1->SetTextSize(0.04); // Reduce font size
    for(int i = 0; i < nptbins; i++) {
        TH1D *hProjX = hLeadingVsZ[i]->ProjectionX();
        hProjX->Scale(1. / hProjX->GetBinWidth(1)); // Scale by 1/binsize
        hProjX->SetLineColor(rcolors[i]); // Set color
        hProjX->SetTitle("deta");
        hProjX->SetStats(0); // Turn off statistics box
        hProjX->GetXaxis()->SetTitle("#Delta#eta");
        hProjX->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta#eta)");
        hProjX->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjX->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjX->GetYaxis()->SetRangeUser(0, 1.5); // Set plot range
        hProjX->Draw("HIST SAME"); // Draw histogram in step style
        leg1->AddEntry(hProjX, Form("p_{T} > %g GeV", ptbinlo[i]), "l");
    }
    leg1->Draw();

    c2->cd(2);
    TLegend *leg2 = new TLegend(0.15, 0.65, 0.35, 0.85);
    leg2->SetBorderSize(0); // Remove legend box
    leg2->SetTextSize(0.04); // Reduce font size
    for(int i = 0; i < nptbins; i++) {
        TH1D *hProjY = hLeadingVsZ[i]->ProjectionY();
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
    }
    leg2->Draw();

    c2->cd(3);
    TLegend *leg3 = new TLegend(0.15, 0.65, 0.35, 0.85);
    leg3->SetBorderSize(0); // Remove legend box
    leg3->SetTextSize(0.04); // Reduce font size
    for(int i = 0; i < nptbins; i++) {
        TH1D *hProjZ = hLeadingVsZ[i]->ProjectionZ();
        hProjZ->Scale(1. / hProjZ->GetBinWidth(1)); // Scale by 1/binsize
        hProjZ->SetLineColor(rcolors[i]); // Set color
        hProjZ->SetTitle("R");
        hProjZ->SetStats(0); // Turn off statistics box
        hProjZ->GetXaxis()->SetTitle("#Delta R");
        hProjZ->GetYaxis()->SetTitle("(1/N_{Z})dN/d(#Delta R)");
        hProjZ->GetXaxis()->SetTitleSize(0.05); // Increase font size
        hProjZ->GetYaxis()->SetTitleSize(0.05); // Increase font size
        hProjZ->GetYaxis()->SetRangeUser(0, 2.4); // Set plot range
        hProjZ->Draw("HIST SAME"); // Draw histogram in step style
        leg3->AddEntry(hProjZ, Form("p_{T} > %g GeV", ptbinlo[i]), "l");
    }
    leg3->Draw();

    // Optionally: Save the canvas as an image
    c2->SaveAs(Form("output_%s.png", output));

    // Cleanup: Close the file
    //file->Close();
    //delete file;
}

