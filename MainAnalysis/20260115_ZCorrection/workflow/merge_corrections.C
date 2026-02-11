#include <TFile.h>
#include <TH1D.h>

void merge_corrections(const char* file1, const char* file2, const char* outputFile) {
    // Open the correction files
    TFile *fCorr1 = TFile::Open(file1);
    TFile *fCorr2 = TFile::Open(file2);

    // Check if files are opened successfully
    if (!fCorr1 || !fCorr2) {
        printf("Error opening files.\n");
        return;
    }

    // Retrieve the correction histograms
    TH1D *hPtCorr1 = (TH1D*)fCorr1->Get("hPtCorrTotal");
    TH1D *hEtaCorr1 = (TH1D*)fCorr1->Get("hEtaCorrTotal");
    TH1D *hPhiCorr1 = (TH1D*)fCorr1->Get("hPhiCorrTotal");

    TH1D *hPtCorr2 = (TH1D*)fCorr2->Get("hPtCorrTotal");
    TH1D *hEtaCorr2 = (TH1D*)fCorr2->Get("hEtaCorrTotal");
    TH1D *hPhiCorr2 = (TH1D*)fCorr2->Get("hPhiCorrTotal");

    // Check if histograms are retrieved successfully
    if (!hPtCorr1 || !hEtaCorr1 || !hPhiCorr1 || !hPtCorr2 || !hEtaCorr2 || !hPhiCorr2) {
        printf("Error retrieving histograms.\n");
        return;
    }

    // Multiply the histograms
    hPtCorr1->Multiply(hPtCorr2);
    hEtaCorr1->Multiply(hEtaCorr2);
    hPhiCorr1->Multiply(hPhiCorr2);

    // Save the merged correction histograms into a new file
    TFile *fCorrMerged = new TFile(outputFile, "RECREATE");
    hPtCorr1->Write("hPtCorrTotal");
    hEtaCorr1->Write("hEtaCorrTotal");
    hPhiCorr1->Write("hPhiCorrTotal");
    fCorrMerged->Close();

    // Clean up
    fCorr1->Close();
    fCorr2->Close();

    delete fCorr1;
    delete fCorr2;
    delete fCorrMerged;
}

