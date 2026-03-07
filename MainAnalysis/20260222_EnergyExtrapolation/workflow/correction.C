#include <TFile.h>
#include <TH3D.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>

using namespace std;

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


void correction(const char* ppFileName = "output/pp-502.root", 
                const char* pPbFileName = "output/pPb-PbP-816.root") {
    
    // Open RECO.root and GEN.root files using the input file names
    TFile *fpp = TFile::Open(ppFileName);
    TFile *fpPb = TFile::Open(pPbFileName);

    // Retrieve 3D histograms
    TH3D *h3pp = (TH3D*)fpp->Get("h3D");
    TH3D *h3pPb = (TH3D*)fpPb->Get("h3D");

    // Project into 1D histograms for pt, eta, and Phi
    TH1D *hPt_pPb = h3pPb->ProjectionX("hPt_pPb");
    TH1D *hNZ_pPb = (TH1D*) fpPb->Get("hNZ");
    float NZ_pPb = hNZ_pPb->GetBinContent(1);
    
    TH1D *hPt_pp = h3pp->ProjectionX("hPt_pp");
    TH1D *hNZ_pp = (TH1D*) fpp->Get("hNZ");
    float NZ_pp = hNZ_pp->GetBinContent(1);

    cout<<"NZ_pPb: "<<NZ_pPb<<endl;
    cout<<"NZ_pp: "<<NZ_pp<<endl;

    // Calculate the ratios GEN/RECO
    hPt_pPb->Scale(1.0 / NZ_pPb);
    hPt_pp->Scale(1.0 / NZ_pp);

    TH1D *hPtCorrTotal = (TH1D*)hPt_pPb->Clone("hPtCorrTotal");
    hPtCorrTotal->Divide(hPt_pp);

    cout<<"Integral of hPtCorrTotal: "<<hPtCorrTotal->Integral()<<endl;
    
    // dummy content for other dimensions, we dont correct for these
    TH1D *hEtaCorrTotal = new TH1D("hEtaCorrTotal", "hEtaCorrTotal", 1, 0.5, 1.5);
    hEtaCorrTotal->SetBinContent(1, 1);

    TH1D *hPhiCorrTotal = new TH1D("hPhiCorrTotal", "hPhiCorrTotal", 1, 0.5, 1.5);
    hPhiCorrTotal->SetBinContent(1, 1);

    /*
    scaleTH3D(hReco3D, hPtCorrTotal, 'x');
    TH1D *hEtaReco = hReco3D->ProjectionY("hEtaReco");
    TH1D *hEtaCorrTotal = (TH1D*)hEtaGen->Clone("hEtaCorrTotal");
    hEtaCorrTotal->Divide(hEtaReco);

    scaleTH3D(hReco3D, hEtaCorrTotal, 'y');
    TH1D *hPhiReco = hReco3D->ProjectionZ("hPhiReco");
    TH1D *hPhiCorrTotal = (TH1D*)hPhiGen->Clone("hPhiCorrTotal");
    hPhiCorrTotal->Divide(hPhiReco);   
    */

    // Save the correction histograms into correction.root
    TFile *fCorr = new TFile("correction.root", "RECREATE");
    hPtCorrTotal->Write();
    hEtaCorrTotal->Write();
    hPhiCorrTotal->Write();
    fCorr->Close();

    // Clean up
    fpp->Close();
    fpPb->Close();

    delete fpp;
    delete fpPb;
    delete fCorr;
}


