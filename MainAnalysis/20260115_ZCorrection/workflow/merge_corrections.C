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
    TH1D *hMultCorr1 = (TH1D*)fCorr1->Get("hMultCorrTotal");

    TH1D *hPtCorr2 = (TH1D*)fCorr2->Get("hPtCorrTotal");
    TH1D *hEtaCorr2 = (TH1D*)fCorr2->Get("hEtaCorrTotal");
    TH1D *hMultCorr2 = (TH1D*)fCorr2->Get("hMultCorrTotal");

    // Check if histograms are retrieved successfully
    if (!hPtCorr1 || !hEtaCorr1 || !hMultCorr1 || !hPtCorr2 || !hEtaCorr2 || !hMultCorr2) {
        printf("Error retrieving histograms.\n");
        return;
    }

    // Multiply the histograms
    hPtCorr1->Multiply(hPtCorr2);
    hEtaCorr1->Multiply(hEtaCorr2);
    hMultCorr1->Multiply(hMultCorr2);

    // Save the merged correction histograms into a new file
    TFile *fCorrMerged = new TFile(outputFile, "RECREATE");
    hPtCorr1->Write("hPtCorrTotal");
    hEtaCorr1->Write("hEtaCorrTotal");
    hMultCorr1->Write("hMultCorrTotal");
    fCorrMerged->Close();

    // Clean up
    fCorr1->Close();
    fCorr2->Close();

    delete fCorr1;
    delete fCorr2;
    delete fCorrMerged;
}

