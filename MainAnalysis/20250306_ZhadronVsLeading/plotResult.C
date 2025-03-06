#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

void plotResult() {
    // Open the ROOT file
    TFile *file = new TFile("output.root", "READ");

    // Retrieve histograms from the file
    TH1D *hTrkPt = (TH1D*)file->Get("hTrkPtData");
    TH1D *hLeadingPt = (TH1D*)file->Get("hLeadingPtData");
    TH1D *hZPt = (TH1D*)file->Get("hZPtData");
    TH1D *hZMass = (TH1D*)file->Get("hZMassData");

    TH1D *hNz = (TH1D*)file->Get("hNzData");

    const int nptbins = 4;
    float ptbinlo[nptbins] = {4, 5, 6, 10};
    TH2D* hLeadingVsZ[nptbins];
    for (int i = 0; i < nptbins; i++) {
        hLeadingVsZ[i] = (TH2D*)file->Get(Form("hLeadingVsZData_%i", static_cast<int>(ptbinlo[i])));
    }

    // Check if histograms are loaded
    if (!hTrkPt || !hLeadingPt || !hZPt || !hZMass || !hNz) {
        std::cerr << "Error: Histogram not found in file" << std::endl;
        return;
    }

    // Scale histograms
    //hData->Scale(1. / hNtrk->GetBinContent(1));
    //hMixData->Scale(1. / hNZMixData->GetBinContent(1));

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
    c1->SaveAs("randoinfo_histogram.png");

    // Create a canvas to draw the histogram
    TCanvas *c2 = new TCanvas("c2", "Canvas", 1600, 600);
    c2->Divide(2);

    c2->cd(1);
    for(int i = 0; i < nptbins; i++) {
        hLeadingVsZ[i]->ProjectionX()->SetLineColor(i + 1); // Set color
        if (i == 0) {
            hLeadingVsZ[i]->ProjectionX()->Draw(); // Draw the first histogram
        } else {
            hLeadingVsZ[i]->ProjectionX()->Draw("SAME"); // Overlay subsequent histograms
        }
    }

    TLegend *leg1 = new TLegend(0.1, 0.7, 0.3, 0.9);
    leg1->SetBorderSize(0); // Remove legend box
    for(int i = 0; i < nptbins; i++) {
        leg1->AddEntry(hLeadingVsZ[i]->ProjectionX(), Form("p_{T} > %g GeV", ptbinlo[i]), "l");
    }
    leg1->Draw();

    c2->cd(2);
    for(int i = 0; i < nptbins; i++) {
        hLeadingVsZ[i]->ProjectionY()->SetLineColor(i + 1); // Set color
        if (i == 0) {
            hLeadingVsZ[i]->ProjectionY()->Draw(); // Draw the first histogram
        } else {
            hLeadingVsZ[i]->ProjectionY()->Draw("SAME"); // Overlay subsequent histograms
        }
    }

    TLegend *leg2 = new TLegend(0.1, 0.7, 0.3, 0.9);
    leg2->SetBorderSize(0); // Remove legend box
    for(int i = 0; i < nptbins; i++) {
        leg2->AddEntry(hLeadingVsZ[i]->ProjectionY(), Form("p_{T} > %g GeV", ptbinlo[i]), "l");
    }
    leg2->Draw();

    // Optionally: Save the canvas as an image
    c2->SaveAs("output_histogram.png");

    // Cleanup: Close the file
    //file->Close();
    //delete file;
}

int main() {
    plotResult();
    return 0;
}
