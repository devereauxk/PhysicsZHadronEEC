#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

void merge_corrections_2D(const char* file1, const char* file2, const char* outputFile) {
    TFile *fCorr1 = TFile::Open(file1);
    TFile *fCorr2 = TFile::Open(file2);

    TH1D *hPtCorr1 = (TH1D*)fCorr1->Get("hPtCorrTotal");
    TH2D *hEtaPhiCorr1 = (TH2D*)fCorr1->Get("hEtaPhiCorrTotal");

    TH1D *hPtCorr2 = (TH1D*)fCorr2->Get("hPtCorrTotal");
    TH2D *hEtaPhiCorr2 = (TH2D*)fCorr2->Get("hEtaPhiCorrTotal");

    hPtCorr1->Multiply(hPtCorr2);
    hEtaPhiCorr1->Multiply(hEtaPhiCorr2);

    TFile *fCorrMerged = new TFile(outputFile, "RECREATE");
    hPtCorr1->Write("hPtCorrTotal");
    hEtaPhiCorr1->Write("hEtaPhiCorrTotal");
    fCorrMerged->Close();

    fCorr1->Close();
    fCorr2->Close();

    delete fCorr1;
    delete fCorr2;
    delete fCorrMerged;
}
