#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <iostream>


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


void makeProjection(const char *infname="output.root", const char *outfname="result.root", const char *tag="", bool doSub=1) {
    // Open the ROOT file
    TFile *file = new TFile(infname, "READ");

    // Retrieve histograms from the file
    TH2D *hData = (TH2D*)file->Get("hData"); // Replace with your histogram name
    TH2D *hNZData = (TH2D*)file->Get("hNZData"); // Replace with your histogram name
    TH1D *hMixData = (TH1D*)file->Get("hMixData"); // Replace with your histogram name
    TH1D *hNZMixData = (TH1D*)file->Get("hNZMixData"); // Replace with your histogram name

    hNZData->SetName(Form("hNZData_%s",tag));
    hNZMixData->SetName(Form("hNZMixData_%s",tag));

    // Verify that all required histograms are loaded successfully from the file
    if (!hData || !hNZData || !hMixData || !hNZMixData) {
        // Check each histogram individually to provide a specific error message
        if (!hData) {
            std::cerr << "Error: Histogram 'hData' not found in file. Ensure the histogram is correctly loaded and the file path is accurate." << std::endl;
        }
        if (!hNZData) {
            std::cerr << "Error: Histogram 'hNZData' not found in file. Verify that this histogram is defined in the input file." << std::endl;
        }
        if (!hMixData) {
            std::cerr << "Error: Histogram 'hMixData' not found in file. Check that the histogram name matches what is expected." << std::endl;
        }
        if (!hNZMixData) {
            std::cerr << "Error: Histogram 'hNZMixData' not found in file. Confirm that this histogram is included in the file." << std::endl;
        }
    
        // If any of the histograms are missing, log a general error and exit the function
        std::cerr << "Error: One or more required histograms could not be loaded from the file. Exiting function." << std::endl;
        return;
    }

    // Scale histograms
    hData->SetName(Form("hData_%s",tag));
    hData->Scale(1. / hNZData->GetBinContent(1));
    hMixData->SetName(Form("hMixData_%s",tag));
    hMixData->Scale(1. / hNZMixData->GetBinContent(1));
    TH1D *hDataAll = (TH1D*) hData->Clone(Form("hDataAll_%s",tag));
    
    // Subtract hMixData from hData
    if (doSub) hData->Add(hMixData, -1);

    // Create a canvas to draw the histogram
    TCanvas *c1 = new TCanvas("c1", "Canvas for Y projection", 800, 600);
    TH1D *hProjY = (TH1D*) hData->ProjectionY(Form("DeltaPhi_Result%s",tag),0,10); // include underflow bin
    hProjY->SetMarkerStyle(20);
    divideByWidth(hProjY);
    hProjY->Draw();

    // Creating a new canvas for the X projection
    TCanvas *c2 = new TCanvas("c2", "Canvas for X projection", 800, 600);
    TH1D *hProjX = (TH1D*) hData->ProjectionX(Form("DeltaEta_Result%s",tag),6,10);
    hProjX->SetMarkerStyle(20);
    hProjX->GetXaxis()->SetTitle("#Delta#eta");
    divideByWidth(hProjX);
    hProjX->Draw();

    // Optionally: Save the canvas as an image and write histograms to a file
    TFile *outf = new TFile(outfname, "RECREATE");
    hData->Write();
    hDataAll->Write();
    hMixData->Write();
    hProjY->Write();
    hProjX->Write();
    hNZData->Write();
    hNZMixData->Write();
    
    // Cleanup: Close the file
    //file->Close();
    //delete file;
}

int main() {
    makeProjection();
    return 0;
}
