#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

// example usage
// root -l -b -q "ZPtEtaCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"PPb\", 1, true, false)"
// root -l -b -q "ZPtEtaCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"PPb\", 1, false, true)"

void ZPtEtaCorrection(const char *MC_reco = "pPbMC.root", const char *Data_reco = "pPb.root", const char* tag = "", int iter = 1, bool doPtFit = true, bool doEtaFit = false)
{
    // Open the input ROOT files
    TFile *fileMC = TFile::Open(MC_reco, "READ");
    TFile *fileData = TFile::Open(Data_reco, "READ");

    // Retrieve histograms from the files
    TH2D *hZPtEtaMC = (TH2D *)fileMC->Get("hZPtEtaData");
    TH1D *hNZMC = (TH1D *)fileMC->Get("hNZData");
    TH2D *hZPtEtaData = (TH2D *)fileData->Get("hZPtEtaData");
    TH1D *hNZData = (TH1D *)fileData->Get("hNZData");

    if (!hZPtEtaMC || !hNZMC || !hZPtEtaData || !hNZData) {
        std::cerr << "Error: Unable to retrieve histograms from files." << std::endl;
        fileMC->Close();
        fileData->Close();
        return;
    }

    // Normalization by N_Z
    double integralMC = hNZMC->GetBinContent(1);
    double integralData = hNZData->GetBinContent(1);
    cout<<"MC Zs:"<<integralMC<<" Data Zs:"<<integralData<<endl;
    if (integralMC > 0) {
        hZPtEtaMC->Scale(1. / integralMC);
    }
    if (integralData > 0) {
        hZPtEtaData->Scale(1. / integralData);
    }

    // get projections
    TH1D *hZPtMC = (TH1D *)hZPtEtaMC->ProjectionX("hZPtMC");
    TH1D *hZEtaMC = (TH1D *)hZPtEtaMC->ProjectionY("hZEtaMC");
    TH1D *hZPtData = (TH1D *)hZPtEtaData->ProjectionX("hZPtData");
    TH1D *hZEtaData = (TH1D *)hZPtEtaData->ProjectionY("hZEtaData");

    TH1D *hZPtRatio = (TH1D *)hZPtData->Clone("hZPtRatio");
    hZPtRatio->Divide(hZPtMC);
    TH1D *hZEtaRatio = (TH1D *)hZEtaData->Clone("hZEtaRatio");
    hZEtaRatio->Divide(hZEtaMC);

    // open output file, create if it doesn't exist
    TFile *outputFile = TFile::Open(Form("ZPtEtaCorrection-%s.root", tag), "UPDATE");
    if (!outputFile || outputFile->IsZombie()) {
        outputFile = new TFile(Form("ZPtEtaCorrection-%s.root", tag), "RECREATE");
    }

    // perform fits
    if (doPtFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_pt-%i", iter), "[0]*x*x + [1]*x + [2]", 0, 6);
        hZPtRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }
    if (doEtaFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_eta-%i", iter), "[0]*x*x + [1]*x + [2]", -2.4, 2.4);
        hZEtaRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }

    // Clean up
    fileMC->Close();
    fileData->Close();
    outputFile->Close();

}
