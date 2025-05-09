#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

// example usage
// root -l -b -q "TrackResidualCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/pPbMC_Gen_ZPT0_100-0_40.root\", \"residualCorrection/TrackResidualCorrection-PPb.root\", 1, false, false, true)"

void TrackResidualCorrection(const char *MCReco = "pPbMC.root", const char *MCGen = "pPb.root", const char* output = "output.root", int iter = 1, bool doPtFit = true, bool doEtaFit = false, bool doPhiFit = false)
{
    // Open the input ROOT files
    TFile *fileReco = TFile::Open(MCReco, "READ");
    TFile *fileGen = TFile::Open(MCGen, "READ");

    // Retrieve histograms from the files
    TH3D *hTrkPtEtaPhiReco = (TH3D *)fileReco->Get("hTrkPtEtaPhiData");
    TH1D *hNZReco = (TH1D *)fileReco->Get("hNZData");
    TH3D *hTrkPtEtaPhiGen = (TH3D *)fileGen->Get("hTrkPtEtaPhiData");
    TH1D *hNZGen = (TH1D *)fileGen->Get("hNZData");

    if (!hTrkPtEtaPhiReco || !hNZReco || !hTrkPtEtaPhiGen || !hNZGen) {
        std::cerr << "Error: Unable to retrieve histograms from files." << std::endl;
        fileReco->Close();
        fileGen->Close();
        return;
    }

    // Normalization by N_Z
    double integralReco = hNZReco->GetBinContent(1);
    double integralGen = hNZGen->GetBinContent(1);
    std::cout << "Reco Zs: " << integralReco << " Gen Zs: " << integralGen << std::endl;
    if (integralReco > 0) {
        hTrkPtEtaPhiReco->Scale(1. / integralReco);
    }
    if (integralGen > 0) {
        hTrkPtEtaPhiGen->Scale(1. / integralGen);
    }

    // Get projections
    TH1D *hTrkPtReco = (TH1D *)hTrkPtEtaPhiReco->ProjectionX("hTrkPtReco");
    TH1D *hTrkEtaReco = (TH1D *)hTrkPtEtaPhiReco->ProjectionY("hTrkEtaReco");
    TH1D *hTrkPhiReco = (TH1D *)hTrkPtEtaPhiReco->ProjectionZ("hTrkPhiReco");
    TH1D *hTrkPtGen = (TH1D *)hTrkPtEtaPhiGen->ProjectionX("hTrkPtGen");
    TH1D *hTrkEtaGen = (TH1D *)hTrkPtEtaPhiGen->ProjectionY("hTrkEtaGen");
    TH1D *hTrkPhiGen = (TH1D *)hTrkPtEtaPhiGen->ProjectionZ("hTrkPhiGen");

    TH1D *hTrkPtRatio = (TH1D *)hTrkPtGen->Clone("hTrkPtRatio");
    hTrkPtRatio->Divide(hTrkPtReco);
    TH1D *hTrkEtaRatio = (TH1D *)hTrkEtaGen->Clone("hTrkEtaRatio");
    hTrkEtaRatio->Divide(hTrkEtaReco);
    TH1D *hTrkPhiRatio = (TH1D *)hTrkPhiGen->Clone("hTrkPhiRatio");
    hTrkPhiRatio->Divide(hTrkPhiReco);

    // Open output file, create if it doesn't exist
    TFile *outputFile = TFile::Open(output, "UPDATE");
    if (!outputFile || outputFile->IsZombie()) {
        outputFile = new TFile(output, "RECREATE");
    }

    // Perform fits
    if (doPtFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_pt-%i", iter), "[0]*x*x*x + [1]*x*x + [2]*x + [3]", 0, 20);
        hTrkPtRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }
    if (doEtaFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_eta-%i", iter), "[0]*x*x*x + [1]*x*x + [2]*x + [3]", -2.4, 2.4);
        hTrkEtaRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }
    if (doPhiFit) {
        TF1 *fitFunc = new TF1(Form("fitFunc_phi-%i", iter), "[0]*x*x*x + [1]*x*x + [2]*x + [3]", -M_PI, M_PI);
        hTrkPhiRatio->Fit(fitFunc, "R");
        fitFunc->Write();
    }

    // Clean up
    fileReco->Close();
    fileGen->Close();
    outputFile->Close();
}
