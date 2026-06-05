#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <iostream>
#include <TROOT.h>

void plot_corrections(const char* inputFileName = "correction.root",
                      const char* outputFileName = "corrections.pdf") {
    gROOT->SetBatch(kTRUE);
    gStyle->SetTitleY(1.01);
    gStyle->SetTitleSize(0.8);
    // Open the correction files
    TFile *fCorr1 = TFile::Open(inputFileName);

    // Retrieve the histograms
    TH1D *hPtCorr1 = (TH1D*)fCorr1->Get("hPtCorrTotal");
    hPtCorr1->SetName("hPtCorr1");

    // Set up a canvas with 2x2 pads
    TCanvas *c = new TCanvas("c", "Corrections", 600, 600);

    // Plot hPtCorrTotal
    c->cd();
    c->SetLogx();
    hPtCorr1->SetLineColor(kRed);
    hPtCorr1->SetTitle("Z p_{T} Dependent Correction");
    hPtCorr1->GetYaxis()->SetRangeUser(0,2);
    hPtCorr1->GetXaxis()->SetRangeUser(0.5, 500);
    hPtCorr1->GetXaxis()->CenterTitle();
    hPtCorr1->SetTitleOffset(1.1,"X");
    hPtCorr1->SetTitleSize(0.04,"X");
    hPtCorr1->SetLabelSize(0.035,"X");
    hPtCorr1->SetYTitle("Correction");
    hPtCorr1->GetYaxis()->CenterTitle();
    hPtCorr1->Draw("HIST");

    // Save the canvas as an image
    c->SaveAs(outputFileName);

}

int main() {
    plot_corrections();
    return 0;
}
