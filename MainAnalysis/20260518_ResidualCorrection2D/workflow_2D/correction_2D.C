#include <TFile.h>
#include <TH3D.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>

void scaleTH3D(TH3D* h3D, const TH1D* h1D, const char axis) {
    int nBinsX = h3D->GetNbinsX();
    int nBinsY = h3D->GetNbinsY();
    int nBinsZ = h3D->GetNbinsZ();

    for (int ix = 1; ix <= nBinsX; ++ix) {
        for (int iy = 1; iy <= nBinsY; ++iy) {
            for (int iz = 1; iz <= nBinsZ; ++iz) {
                double binContent = h3D->GetBinContent(ix, iy, iz);
                double scaleFactor = 1.0;
                if (axis == 'x') scaleFactor = h1D->GetBinContent(ix);
                else if (axis == 'y') scaleFactor = h1D->GetBinContent(iy);
                h3D->SetBinContent(ix, iy, iz, binContent * scaleFactor);
            }
        }
    }
}


void correction_2D(const char* recoFileName = "output/DY-RECO.root",
                   const char* genFileName = "output/DY-GEN.root") {
    TFile *fReco = TFile::Open(recoFileName);
    TFile *fGen = TFile::Open(genFileName);

    TH3D *hReco3D = (TH3D*)fReco->Get("hTrkPtEtaPhiData");
    hReco3D->SetName("h3D_reco");
    TH3D *hGen3D = (TH3D*)fGen->Get("hTrkPtEtaPhiData");
    hGen3D->SetName("h3D_gen");
    TH1D *hNZReco = (TH1D*)fReco->Get("hNZData");
    TH1D *hNZGen = (TH1D*)fGen->Get("hNZData");
    const double nZReco = hNZReco->GetBinContent(1);
    const double nZGen = hNZGen->GetBinContent(1);

    // 1) pT correction (1D)
    TH1D *hPtGen = hGen3D->ProjectionX("hPtGen");
    hPtGen->Scale(1.0 / nZGen);
    TH1D *hPtReco = hReco3D->ProjectionX("hPtReco");
    hPtReco->Scale(1.0 / nZReco);
    TH1D *hPtCorrTotal = (TH1D*)hPtGen->Clone("hPtCorrTotal");
    hPtCorrTotal->Divide(hPtReco);

    // Apply pT correction to reco 3D before eta-phi projection
    scaleTH3D(hReco3D, hPtCorrTotal, 'x');

    // 2) eta-phi correction (2D simultaneous)
    TH2D *hEtaPhiGen = (TH2D*)hGen3D->Project3D("zy");
    hEtaPhiGen->SetName("hEtaPhiGen");
    hEtaPhiGen->Scale(1.0 / nZGen);

    TH2D *hEtaPhiReco = (TH2D*)hReco3D->Project3D("zy");
    hEtaPhiReco->SetName("hEtaPhiReco");
    hEtaPhiReco->Scale(1.0 / nZReco);

    TH2D *hEtaPhiCorrTotal = (TH2D*)hEtaPhiGen->Clone("hEtaPhiCorrTotal");
    hEtaPhiCorrTotal->Divide(hEtaPhiReco);

    // Save
    TFile *fCorr = new TFile("correction.root", "RECREATE");
    hPtCorrTotal->Write();
    hEtaPhiCorrTotal->Write();
    fCorr->Close();

    fReco->Close();
    fGen->Close();

    delete fReco;
    delete fGen;
    delete fCorr;
}
