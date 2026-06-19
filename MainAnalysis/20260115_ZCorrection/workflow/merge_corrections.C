#include <TFile.h>
#include <TH1D.h>

void merge_corrections(const char* file1, const char* file2, const char* outputFile) {
    TFile *fCorr1 = TFile::Open(file1);
    TFile *fCorr2 = TFile::Open(file2);

    if (!fCorr1 || !fCorr2) {
        printf("Error opening files.\n");
        return;
    }

    TH1D *hPtCorr1  = (TH1D*)fCorr1->Get("hPtCorrTotal");
    TH1D *hEtaCorr1 = (TH1D*)fCorr1->Get("hEtaCorrTotal");
    TH1D *hPtCorr2  = (TH1D*)fCorr2->Get("hPtCorrTotal");
    TH1D *hEtaCorr2 = (TH1D*)fCorr2->Get("hEtaCorrTotal");

    if (!hPtCorr1 || !hEtaCorr1 || !hPtCorr2 || !hEtaCorr2) {
        printf("Error retrieving histograms.\n");
        return;
    }

    hPtCorr1->Multiply(hPtCorr2);
    hEtaCorr1->Multiply(hEtaCorr2);

    TFile *fCorrMerged = new TFile(outputFile, "RECREATE");
    hPtCorr1->Write("hPtCorrTotal");
    hEtaCorr1->Write("hEtaCorrTotal");
    fCorrMerged->Close();

    fCorr1->Close();
    fCorr2->Close();
    delete fCorr1;
    delete fCorr2;
    delete fCorrMerged;
}
