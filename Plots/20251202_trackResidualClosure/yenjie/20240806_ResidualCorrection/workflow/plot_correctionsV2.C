// plot_corrections.cc
#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TError.h>
#include <TSystem.h>
#include <iostream>
#include <cstdlib>

static TH1D* getH(TFile* f, const char* name) {
    if (!f) return nullptr;
    auto h = dynamic_cast<TH1D*>(f->Get(name));
    if (!h) {
        std::cerr << "Missing histogram: " << name << " in " << f->GetName() << std::endl;
        return nullptr;
    }
    h->SetDirectory(nullptr);  // detach from file for speed and safety
    return h;
}

void plot_correctionsV2() {
    gROOT->SetBatch(kTRUE);           // no windows ever
    gErrorIgnoreLevel = kWarning;     // less chatter
    gStyle->SetOptStat(0);
    gStyle->SetCanvasPreferGL(0);     // avoid GL paths in batch
    gStyle->SetTitleY(1.01);
    gStyle->SetTitleSize(0.9);

    // Open files (read-only)
    TFile fCorr1("output/correction_1.root", "READ");
    TFile fCorr2("output/correction_2.root", "READ");
    TFile fCorr3("output/correction_3.root", "READ");
    if (fCorr1.IsZombie() || fCorr2.IsZombie() || fCorr3.IsZombie()) {
        std::cerr << "Error opening one or more files." << std::endl;
        return;
    }

    // Grab hists and detach
    TH1D *hPtCorr1 = getH(&fCorr1, "hPtCorrTotal");
    TH1D *hEtaCorr1 = getH(&fCorr1, "hEtaCorrTotal");
    TH1D *hPhiCorr1 = getH(&fCorr1, "hPhiCorrTotal");

    TH1D *hPtCorr2 = getH(&fCorr2, "hPtCorrTotal");
    TH1D *hEtaCorr2 = getH(&fCorr2, "hEtaCorrTotal");
    TH1D *hPhiCorr2 = getH(&fCorr2, "hPhiCorrTotal");

    TH1D *hPtCorr3 = getH(&fCorr3, "hPtCorrTotal");
    TH1D *hEtaCorr3 = getH(&fCorr3, "hEtaCorrTotal");
    TH1D *hPhiCorr3 = getH(&fCorr3, "hPhiCorrTotal");

    if (!hPtCorr1 || !hEtaCorr1 || !hPhiCorr1 ||
        !hPtCorr2 || !hEtaCorr2 || !hPhiCorr2 ||
        !hPtCorr3 || !hEtaCorr3 || !hPhiCorr3) {
        std::cerr << "Error retrieving histograms." << std::endl;
        return;
    }
    // Files can be closed now
    fCorr1.Close(); fCorr2.Close(); fCorr3.Close();

    auto setup_axis = [](TH1* h, const char* title) {
        h->SetTitle(title);
        h->GetYaxis()->SetRangeUser(0.8, 1.2);
        h->GetXaxis()->CenterTitle(true);
        h->GetYaxis()->CenterTitle(true);
        h->SetTitleOffset(1.1, "X");
        h->SetTitleSize(0.055, "X");
        h->GetYaxis()->SetTitle("Correction");
        h->SetLineWidth(2);
    };

    TCanvas c("c", "Corrections", 1200, 1200);
    c.Divide(2, 2, 0.001, 0.001); // tiny gaps, a bit faster

    // pT
    c.cd(1)->SetLogx();
    setup_axis(hPtCorr1, "Track p_{T} Dependent Correction");
    hPtCorr1->SetLineColor(kRed);       hPtCorr1->Draw("HIST");
    hPtCorr2->SetLineColor(kBlue);      hPtCorr2->SetLineWidth(2);      hPtCorr2->Draw("HIST SAME");
    hPtCorr3->SetLineColor(kGreen + 2); hPtCorr3->SetLineWidth(2);      hPtCorr3->Draw("HIST SAME");

    // eta
    c.cd(2);
    setup_axis(hEtaCorr1, "Track #eta Dependent Correction");
    hEtaCorr1->SetLineColor(kRed);       hEtaCorr1->Draw("HIST");
    hEtaCorr2->SetLineColor(kBlue);      hEtaCorr2->SetLineWidth(2);     hEtaCorr2->Draw("HIST SAME");
    hEtaCorr3->SetLineColor(kGreen + 2); hEtaCorr3->SetLineWidth(2);     hEtaCorr3->Draw("HIST SAME");

    // phi
    c.cd(3);
    setup_axis(hPhiCorr1, "Track #phi Dependent Correction");
    hPhiCorr1->SetLineColor(kRed);       hPhiCorr1->Draw("HIST");
    hPhiCorr2->SetLineColor(kBlue);      hPhiCorr2->SetLineWidth(2);     hPhiCorr2->Draw("HIST SAME");
    hPhiCorr3->SetLineColor(kGreen + 2); hPhiCorr3->SetLineWidth(2);     hPhiCorr3->Draw("HIST SAME");

    // legend
    c.cd(4);
    gPad->SetTicks(0,0);
    gPad->SetFrameLineColor(0);
    TLegend leg(0.1, 0.7, 0.9, 0.9);
    leg.SetBorderSize(0);
    leg.SetFillStyle(0);
    leg.AddEntry(hPtCorr1, "Iteration 1", "l");
    leg.AddEntry(hPtCorr2, "Iteration 2", "l");
    leg.AddEntry(hPtCorr3, "Iteration 3", "l");
    leg.Draw();

    c.SaveAs("corrections.pdf");
}

int main() {
    unsetenv("DISPLAY");  // extra guard against accidental X11
    plot_correctionsV2();
    return 0;
}