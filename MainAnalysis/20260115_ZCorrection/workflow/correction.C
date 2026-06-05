#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>
#include <iostream>

void scaleTH2D(TH2D* h2D, const TH1D* h1D, const char axis) {
    int nX = h2D->GetNbinsX(), nY = h2D->GetNbinsY();
    for (int ix = 1; ix <= nX; ix++)
    for (int iy = 1; iy <= nY; iy++) {
        double sf = (axis == 'x') ? h1D->GetBinContent(ix) : h1D->GetBinContent(iy);
        h2D->SetBinContent(ix, iy, h2D->GetBinContent(ix, iy) * sf);
    }
}


void correction(const char* recoFileName = "output/DY-RECO.root",
                const char* genFileName = "output/DY-GEN.root") {
    TFile *fReco = TFile::Open(recoFileName);
    TFile *fGen  = TFile::Open(genFileName);

    if (!fReco || fReco->IsZombie()) { std::cerr << "Cannot open RECO file: " << recoFileName << std::endl; return; }
    if (!fGen  || fGen->IsZombie())  { std::cerr << "Cannot open GEN file: "  << genFileName  << std::endl; return; }

    TH2D *hReco2D = (TH2D*)fReco->Get("h2D");
    TH2D *hGen2D  = (TH2D*)fGen->Get("h2D");

    if (!hReco2D) { std::cerr << "h2D not found in " << recoFileName << std::endl; return; }
    if (!hGen2D)  { std::cerr << "h2D not found in " << genFileName  << std::endl; return; }

    hReco2D->SetDirectory(nullptr);
    hGen2D->SetDirectory(nullptr);
    fReco->Close();
    fGen->Close();

    TH1D *hPtGen  = hGen2D->ProjectionX("hPtGen");
    TH1D *hEtaGen = hGen2D->ProjectionY("hEtaGen");

    TH1D *hPtReco = hReco2D->ProjectionX("hPtReco");
    TH1D *hPtCorrTotal = (TH1D*)hPtGen->Clone("hPtCorrTotal");
    hPtCorrTotal->Divide(hPtReco);

    scaleTH2D(hReco2D, hPtCorrTotal, 'x');
    TH1D *hEtaReco = hReco2D->ProjectionY("hEtaReco");
    TH1D *hEtaCorrTotal = (TH1D*)hEtaGen->Clone("hEtaCorrTotal");
    hEtaCorrTotal->Divide(hEtaReco);

    TFile *fCorr = new TFile("correction.root", "RECREATE");
    hPtCorrTotal->Write();
    hEtaCorrTotal->Write();
    fCorr->Close();

    delete fReco;
    delete fGen;
    delete fCorr;
}
