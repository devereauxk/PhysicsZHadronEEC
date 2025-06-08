#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

// example usage
// root -l -b -q "ZCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"Zcorrectionfactor/totalCorrection.root\", true, false)"

void ZCorrection(const char *MC_reco = "pPbMC.root", const char *Data_reco = "pPb.root",
                 const char* output = "", 
                 bool doPtFit = false, bool doEtaFit = false) {

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

    // write to file, if file exists, apply existing corrections
    TFile *outputFile = TFile::Open(output, "UPDATE");
    TH1D *hZPtRatioOld = (TH1D *)outputFile->Get("hPtCorrTotal");
    TH1D *hZEtaRatioOld = (TH1D *)outputFile->Get("hEtaCorrTotal");
    if (doPtFit) {
        // print all bincontent for hZPtRatio
        /*
        for (int i = 1; i <= hZPtRatio->GetNbinsX(); i++) {
            cout << hZPtRatio->GetBinContent(i) << " ";
        }
        cout << endl;
        */
        
        if (hZPtRatioOld) {
            cout<<"pT hist already there "<<hZPtRatioOld->Integral()<<endl;
            hZPtRatioOld->Multiply(hZPtRatio);
            cout<<"new "<<" "<<hZPtRatioOld->Integral()<<endl;
            hZPtRatioOld->Write();
        } else {
            hZPtRatio->SetName("hPtCorrTotal");
            hZPtRatio->Write();
        }
    }
    if (doEtaFit) {
        if (hZEtaRatioOld) {
            cout<<"eta hist already there "<<hZEtaRatioOld->Integral()<<endl;
            hZEtaRatioOld->Multiply(hZEtaRatio);
            cout<<"new "<<" "<<hZEtaRatioOld->Integral()<<endl;
            hZEtaRatioOld->Write();
        } else {
            hZEtaRatio->SetName("hEtaCorrTotal");
            hZEtaRatio->Write();
        }
    }

    // clean up
    outputFile->Write();
    outputFile->Close();
    fileMC->Close();
    fileData->Close();

}
