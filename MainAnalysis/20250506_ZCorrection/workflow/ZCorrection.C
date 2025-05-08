#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

// example usage
// root -l -b -q "ZCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"PPb\", 1, false, false, true)"

void ZCorrection(const char *MC_reco = "pPbMC.root", const char *Data_reco = "pPb.root", const char* tag = "", int iter = 1, bool doPtFit = true, bool doEtaFit = false, bool doMultFit = false)
{
    // Open the input ROOT files
    TFile *fileMC = TFile::Open(MC_reco, "READ");
    TFile *fileData = TFile::Open(Data_reco, "READ");

    // Retrieve histograms from the files
    TH3D *hZPtEtaMultMC = (TH3D *)fileMC->Get("hZPtEtaMultData");
    TH1D *hNZMC = (TH1D *)fileMC->Get("hNZData");
    TH3D *hZPtEtaMultData = (TH3D *)fileData->Get("hZPtEtaMultData");
    TH1D *hNZData = (TH1D *)fileData->Get("hNZData");

    if (!hZPtEtaMultMC || !hNZMC || !hZPtEtaMultData || !hNZData) {
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
        hZPtEtaMultMC->Scale(1. / integralMC);
    }
    if (integralData > 0) {
        hZPtEtaMultData->Scale(1. / integralData);
    }

    // get projections
    TH1D *hZPtMC = (TH1D *)hZPtEtaMultMC->ProjectionX("hZPtMC");
    TH1D *hZEtaMC = (TH1D *)hZPtEtaMultMC->ProjectionY("hZEtaMC");
    TH1D *hMultMC = (TH1D *)hZPtEtaMultMC->ProjectionZ("hZMultMC");
    TH1D *hZPtData = (TH1D *)hZPtEtaMultData->ProjectionX("hZPtData");
    TH1D *hZEtaData = (TH1D *)hZPtEtaMultData->ProjectionY("hZEtaData");
    TH1D *hMultData = (TH1D *)hZPtEtaMultData->ProjectionZ("hZMultData");

    TH1D *hZPtRatio = (TH1D *)hZPtData->Clone("hZPtRatio");
    hZPtRatio->Divide(hZPtMC);
    TH1D *hZEtaRatio = (TH1D *)hZEtaData->Clone("hZEtaRatio");
    hZEtaRatio->Divide(hZEtaMC);
    TH1D *hMultRatio = (TH1D *)hMultData->Clone("hMultRatio");
    hMultRatio->Divide(hMultMC);

    // open output file, create if it doesn't exist
    TFile *outputFile = TFile::Open(Form("ZCorrection-%s.root", tag), "UPDATE");
    if (!outputFile || outputFile->IsZombie()) {
        outputFile = new TFile(Form("ZCorrection-%s.root", tag), "RECREATE");
    }

    // perform fits
    if (doPtFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_pt-%i", iter), "[0]*x*x*x + [1]*x*x + [2]*x + [3]", 0, 15);
        hZPtRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }
    if (doEtaFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_eta-%i", iter), "[0]*x*x*x + [1]*x*x + [2]*x + [3]", -2.4, 2.4);
        hZEtaRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }
    if (doMultFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_mult-%i", iter), "[0]*x*x*x + [1]*x*x + [2]*x + [3]", 0, 100);
        hMultRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }

    // Clean up
    fileMC->Close();
    fileData->Close();
    outputFile->Close();

}
