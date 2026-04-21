#include <TFile.h>
#include <TH1D.h>
#include <iostream>

void merge_z_corrections(const char* file1, const char* file2, const char* outputFile) {
    TFile *f1 = TFile::Open(file1);
    TFile *f2 = TFile::Open(file2);

    TH1D *hPt1 = (TH1D*)f1->Get("hPtCorrTotal")->Clone("hPt1");
    TH1D *hEta1 = (TH1D*)f1->Get("hEtaCorrTotal")->Clone("hEta1");
    TH1D *hPhi1 = (TH1D*)f1->Get("hPhiCorrTotal")->Clone("hPhi1");
    hPt1->SetDirectory(nullptr);
    hEta1->SetDirectory(nullptr);
    hPhi1->SetDirectory(nullptr);

    TH1D *hPt2 = (TH1D*)f2->Get("hPtCorrTotal");
    TH1D *hEta2 = (TH1D*)f2->Get("hEtaCorrTotal");
    TH1D *hPhi2 = (TH1D*)f2->Get("hPhiCorrTotal");

    hPt1->Multiply(hPt2);
    hEta1->Multiply(hEta2);
    hPhi1->Multiply(hPhi2);

    TFile *fOut = new TFile(outputFile, "RECREATE");
    hPt1->Write("hPtCorrTotal");
    hEta1->Write("hEtaCorrTotal");
    hPhi1->Write("hPhiCorrTotal");
    fOut->Close();

    std::cout << "Merged correction written to " << outputFile << std::endl;

    f1->Close();
    f2->Close();
    delete fOut;
}
