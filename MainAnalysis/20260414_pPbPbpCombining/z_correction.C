// z_correction.C - Iterative Z correction from pPb to PbP
#include <TFile.h>
#include <TH3D.h>
#include <TH1D.h>
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
                else if (axis == 'z') scaleFactor = h1D->GetBinContent(iz);
                h3D->SetBinContent(ix, iy, iz, binContent * scaleFactor);
            }
        }
    }
}

void z_correction(const char* pPbFile, const char* PbPFile, const char* outputFile) {
    TFile *fPPb = TFile::Open(pPbFile);
    TFile *fPbP = TFile::Open(PbPFile);

    TH3D *hPPb = (TH3D*)fPPb->Get("hZ3D")->Clone("hPPb3D");
    TH3D *hPbP = (TH3D*)fPbP->Get("hZ3D")->Clone("hPbP3D");
    hPPb->SetDirectory(nullptr);
    hPbP->SetDirectory(nullptr);

    // Correct pT
    TH1D *hPtPPb = hPPb->ProjectionX("hPtPPb");
    TH1D *hPtPbP = hPbP->ProjectionX("hPtPbP");
    TH1D *hPtCorr = (TH1D*)hPtPbP->Clone("hPtCorrTotal");
    hPtCorr->Divide(hPtPPb);
    for (int i = 0; i <= hPtCorr->GetNbinsX()+1; i++)
        if (hPtPPb->GetBinContent(i) < 1) hPtCorr->SetBinContent(i, 1.0);
    scaleTH3D(hPPb, hPtCorr, 'x');

    // Correct y
    TH1D *hYPPb = hPPb->ProjectionY("hYPPb");
    TH1D *hYPbP = hPbP->ProjectionY("hYPbP");
    TH1D *hEtaCorr = (TH1D*)hYPbP->Clone("hEtaCorrTotal");
    hEtaCorr->Divide(hYPPb);
    for (int i = 0; i <= hEtaCorr->GetNbinsX()+1; i++)
        if (hYPPb->GetBinContent(i) < 1) hEtaCorr->SetBinContent(i, 1.0);
    scaleTH3D(hPPb, hEtaCorr, 'y');

    // Correct phi
    TH1D *hPhiPPb = hPPb->ProjectionZ("hPhiPPb");
    TH1D *hPhiPbP = hPbP->ProjectionZ("hPhiPbP");
    TH1D *hPhiCorr = (TH1D*)hPhiPbP->Clone("hPhiCorrTotal");
    hPhiCorr->Divide(hPhiPPb);
    for (int i = 0; i <= hPhiCorr->GetNbinsX()+1; i++)
        if (hPhiPPb->GetBinContent(i) < 1) hPhiCorr->SetBinContent(i, 1.0);

    TFile *fOut = new TFile(outputFile, "RECREATE");
    hPtCorr->Write();
    hEtaCorr->Write();
    hPhiCorr->Write();
    fOut->Close();

    std::cout << "Z correction written to " << outputFile << std::endl;

    fPPb->Close();
    fPbP->Close();
    delete fOut;
}
