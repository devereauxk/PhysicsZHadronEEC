#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TStyle.h>
#include <iostream>
#include <TROOT.h>

void plot_corrections() {
    gROOT->SetBatch(kTRUE);
    gStyle->SetTitleY(1.01);
    gStyle->SetTitleSize(0.9);

    TFile *fCorr1 = TFile::Open("output/correction_1.root");
    TFile *fCorr2 = TFile::Open("output/correction_2.root");
    TFile *fCorr3 = TFile::Open("output/correction_3.root");

    if (!fCorr1 || !fCorr2 || !fCorr3) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }

    TH1D *hPtCorr1  = (TH1D*)fCorr1->Get("hPtCorrTotal");  hPtCorr1->SetName("hPtCorr1");
    TH1D *hEtaCorr1 = (TH1D*)fCorr1->Get("hEtaCorrTotal"); hEtaCorr1->SetName("hEtaCorr1");
    TH1D *hPtCorr2  = (TH1D*)fCorr2->Get("hPtCorrTotal");  hPtCorr2->SetName("hPtCorr2");
    TH1D *hEtaCorr2 = (TH1D*)fCorr2->Get("hEtaCorrTotal"); hEtaCorr2->SetName("hEtaCorr2");
    TH1D *hPtCorr3  = (TH1D*)fCorr3->Get("hPtCorrTotal");  hPtCorr3->SetName("hPtCorr3");
    TH1D *hEtaCorr3 = (TH1D*)fCorr3->Get("hEtaCorrTotal"); hEtaCorr3->SetName("hEtaCorr3");

    if (!hPtCorr1 || !hEtaCorr1 || !hPtCorr2 || !hEtaCorr2 || !hPtCorr3 || !hEtaCorr3) {
        std::cerr << "Error retrieving histograms." << std::endl;
        return;
    }

    TCanvas *c = new TCanvas("c", "Corrections", 1800, 600);

    TPad *p1 = new TPad("p1", "", 0.00, 0.00, 0.40, 1.00); p1->Draw();
    TPad *p2 = new TPad("p2", "", 0.40, 0.00, 0.80, 1.00); p2->Draw();
    TPad *p3 = new TPad("p3", "", 0.80, 0.60, 1.00, 1.00); p3->Draw();

    p1->cd(); p1->SetLogx();
    hPtCorr1->SetLineColor(kRed);
    hPtCorr1->SetTitle("Z p_{T} Dependent Correction");
    hPtCorr1->GetYaxis()->SetRangeUser(0.8, 1.2);
    hPtCorr1->GetXaxis()->CenterTitle();
    hPtCorr1->SetTitleOffset(1.1, "X");
    hPtCorr1->SetTitleSize(0.055, "X");
    hPtCorr1->SetYTitle("Correction");
    hPtCorr1->GetYaxis()->CenterTitle();
    hPtCorr1->Draw("HIST");
    hPtCorr2->SetLineColor(kBlue);    hPtCorr2->Draw("HIST SAME");
    hPtCorr3->SetLineColor(kGreen+2); hPtCorr3->Draw("HIST SAME");

    p2->cd();
    hEtaCorr1->SetLineColor(kRed);
    hEtaCorr1->SetTitle("Z #eta Dependent Correction");
    hEtaCorr1->GetYaxis()->SetRangeUser(0.8, 1.2);
    hEtaCorr1->GetXaxis()->CenterTitle();
    hEtaCorr1->SetTitleOffset(1.1, "X");
    hEtaCorr1->SetTitleSize(0.055, "X");
    hEtaCorr1->SetYTitle("Correction");
    hEtaCorr1->GetYaxis()->CenterTitle();
    hEtaCorr1->Draw("HIST");
    hEtaCorr2->SetLineColor(kBlue);    hEtaCorr2->Draw("HIST SAME");
    hEtaCorr3->SetLineColor(kGreen+2); hEtaCorr3->Draw("HIST SAME");

    p3->cd();
    TLegend *legend = new TLegend(0.05, 0.05, 0.95, 0.95);
    legend->AddEntry(hPtCorr1, "Iteration 1", "l");
    legend->AddEntry(hPtCorr2, "Iteration 2", "l");
    legend->AddEntry(hPtCorr3, "Iteration 3", "l");
    legend->Draw();

    c->SaveAs("corrections.pdf");
}

int main() {
    plot_corrections();
    return 0;
}
