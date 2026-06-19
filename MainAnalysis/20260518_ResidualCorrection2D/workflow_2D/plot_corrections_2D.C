#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TROOT.h>
#include <iostream>

void plot_corrections_2D() {
    gROOT->SetBatch(kTRUE);
    gStyle->SetTitleY(1.01);
    gStyle->SetOptStat(0);

    TFile *fCorr1 = TFile::Open("output/correction_1.root");
    TFile *fCorr2 = TFile::Open("output/correction_2.root");
    TFile *fCorr3 = TFile::Open("output/correction_3.root");

    TH1D *hPt1 = (TH1D*)fCorr1->Get("hPtCorrTotal");
    TH2D *hEtaPhi1 = (TH2D*)fCorr1->Get("hEtaPhiCorrTotal");
    TH1D *hPt2 = (TH1D*)fCorr2->Get("hPtCorrTotal");
    TH2D *hEtaPhi2 = (TH2D*)fCorr2->Get("hEtaPhiCorrTotal");
    TH1D *hPt3 = (TH1D*)fCorr3->Get("hPtCorrTotal");
    TH2D *hEtaPhi3 = (TH2D*)fCorr3->Get("hEtaPhiCorrTotal");

    TCanvas *c = new TCanvas("c", "2D Residual Corrections", 1200, 1800);
    c->Divide(2, 3);

    // Row 1: Iteration 1
    c->cd(1)->SetLogx();
    double ptMin = hPt1->GetXaxis()->GetBinLowEdge(1);
    double ptMax = hPt1->GetXaxis()->GetBinUpEdge(hPt1->GetNbinsX());
    hPt1->SetTitle("Iteration 1: p_{T} Correction");
    hPt1->GetYaxis()->SetRangeUser(0.8, 1.2);
    hPt1->GetXaxis()->SetRangeUser(ptMin, ptMax);
    hPt1->SetYTitle("Correction");
    hPt1->GetYaxis()->CenterTitle();
    hPt1->GetXaxis()->CenterTitle();
    hPt1->SetLineColor(kRed);
    hPt1->SetLineWidth(2);
    hPt1->Draw("HIST");

    c->cd(2);
    hEtaPhi1->SetTitle("Iteration 1: #eta-#phi Correction");
    hEtaPhi1->GetZaxis()->SetRangeUser(0.8, 1.2);
    hEtaPhi1->Draw("COLZ");

    // Row 2: Iteration 2
    c->cd(3)->SetLogx();
    hPt2->SetTitle("Iteration 2: p_{T} Correction");
    hPt2->GetYaxis()->SetRangeUser(0.8, 1.2);
    hPt2->GetXaxis()->SetRangeUser(ptMin, ptMax);
    hPt2->SetYTitle("Correction");
    hPt2->GetYaxis()->CenterTitle();
    hPt2->GetXaxis()->CenterTitle();
    hPt2->SetLineColor(kBlue);
    hPt2->SetLineWidth(2);
    hPt2->Draw("HIST");

    c->cd(4);
    hEtaPhi2->SetTitle("Iteration 2: #eta-#phi Correction");
    hEtaPhi2->GetZaxis()->SetRangeUser(0.8, 1.2);
    hEtaPhi2->Draw("COLZ");

    // Row 3: Iteration 3
    c->cd(5)->SetLogx();
    hPt3->SetTitle("Iteration 3: p_{T} Correction");
    hPt3->GetYaxis()->SetRangeUser(0.8, 1.2);
    hPt3->GetXaxis()->SetRangeUser(ptMin, ptMax);
    hPt3->SetYTitle("Correction");
    hPt3->GetYaxis()->CenterTitle();
    hPt3->GetXaxis()->CenterTitle();
    hPt3->SetLineColor(kGreen+2);
    hPt3->SetLineWidth(2);
    hPt3->Draw("HIST");

    c->cd(6);
    hEtaPhi3->SetTitle("Iteration 3: #eta-#phi Correction");
    hEtaPhi3->GetZaxis()->SetRangeUser(0.8, 1.2);
    hEtaPhi3->Draw("COLZ");

    c->SaveAs("corrections_2D.pdf");
}

int main() {
    plot_corrections_2D();
    return 0;
}
