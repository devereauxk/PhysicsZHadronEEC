#include <TFile.h>
#include <TH3D.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>

void scaleTH3D(TH3D* h3D, const TH1D* h1D, const char axis) {
    // Check if the axis is valid
    if (axis != 'x' && axis != 'y') {
        std::cerr << "Invalid axis. Use 'x' or 'y'." << std::endl;
        return;
    }

    // Get the number of bins in the TH3D
    int nBinsX = h3D->GetNbinsX();
    int nBinsY = h3D->GetNbinsY();
    int nBinsZ = h3D->GetNbinsZ();

    // Scale the content of TH3D according to the TH1D
    for (int ix = 1; ix <= nBinsX; ++ix) {
        for (int iy = 1; iy <= nBinsY; ++iy) {
            for (int iz = 1; iz <= nBinsZ; ++iz) {
                double binContent = h3D->GetBinContent(ix, iy, iz);
                double scaleFactor = 1.0;

                if (axis == 'x') {
                    scaleFactor = h1D->GetBinContent(ix);
                } else if (axis == 'y') {
                    scaleFactor = h1D->GetBinContent(iy);
                }

                h3D->SetBinContent(ix, iy, iz, binContent * scaleFactor);
            }
        }
    }
}


void correction(const char* recoFileName = "output/DY-RECO.root", 
                const char* genFileName = "output/DY-GEN.root") {
    // Open RECO.root and GEN.root files using the input file names
    TFile *fReco = TFile::Open(recoFileName);
    TFile *fGen = TFile::Open(genFileName);
    //
    // Retrieve 3D histograms
    TH3D *hReco3D = (TH3D*)fReco->Get("h3D");
    TH3D *hGen3D = (TH3D*)fGen->Get("h3D");

    // Project into 1D histograms for pt, eta, and phi

    TH1D *hPtGen = hGen3D->ProjectionX("hPtGen");
    TH1D *hEtaGen = hGen3D->ProjectionY("hEtaGen");
    TH1D *hPhiGen = hGen3D->ProjectionZ("hPhiGen");

    // Calculate the ratios GEN/RECO
    TH1D *hPtReco = hReco3D->ProjectionX("hPtReco");
    TH1D *hPtCorrTotal = (TH1D*)hPtGen->Clone("hPtCorrTotal");
    hPtCorrTotal->Divide(hPtReco);
    
    scaleTH3D(hReco3D, hPtCorrTotal, 'x');
    TH1D *hEtaReco = hReco3D->ProjectionY("hEtaReco");
    TH1D *hEtaCorrTotal = (TH1D*)hEtaGen->Clone("hEtaCorrTotal");
    hEtaCorrTotal->Divide(hEtaReco);

    scaleTH3D(hReco3D, hEtaCorrTotal, 'y');
    TH1D *hPhiReco = hReco3D->ProjectionZ("hPhiReco");
    TH1D *hPhiCorrTotal = (TH1D*)hPhiGen->Clone("hPhiCorrTotal");
    hPhiCorrTotal->Divide(hPhiReco);

    // Save the correction histograms into correction.root
    TFile *fCorr = new TFile("correction.root", "RECREATE");
    hPtCorrTotal->Write();
    hEtaCorrTotal->Write();
    hPhiCorrTotal->Write();
    fCorr->Close();

    // Clean up
    fReco->Close();
    fGen->Close();

    delete fReco;
    delete fGen;
    delete fCorr;
}


