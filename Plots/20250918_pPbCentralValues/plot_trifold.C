#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plot_trifold() {

    // pp
    const char* input_ZPT =  {"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZPT10_20-result.root"};
    const char* output =  "plots/20250918_pp_ZPT10_20-Trifold-DeltaEta.png";

    //pPb
    /*
    const char* input_ZPT =  {"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/1pPb_ZPT10_20-result.root"};
    const char* output =  "plots/20250918_pPb_ZPT10_20.png";
    */

    TFile* fin = TFile::Open(input_ZPT, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin << std::endl;
        return;
    }

    TH1D* hDeltaPhi1_2 = (TH1D*)fin->Get("DeltaPhi_Result1_2");
    TH1D* hDeltaPhi2_4 = (TH1D*)fin->Get("DeltaPhi_Result2_4");
    TH1D* hDeltaPhi4_10 = (TH1D*)fin->Get("DeltaPhi_Result4_10");

    TH1D* hDeltaEta1_2 = (TH1D*)fin->Get("DeltaEta_Result1_2");
    TH1D* hDeltaEta2_4 = (TH1D*)fin->Get("DeltaEta_Result2_4");
    TH1D* hDeltaEta4_10 = (TH1D*)fin->Get("DeltaEta_Result4_10");

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 1200, 600);

    plotCMSTrifold(
        c1, {{hDeltaEta1_2}, {hDeltaEta2_4}, {hDeltaEta4_10}}, "", {"1<p_{T}^{trk}<2 GeV", "2<p_{T}^{trk}<4 GeV", "4<p_{T}^{trk}<10 GeV"},
        {cmsRed, cmsYellow, cmsTealL1, cmsYellow, cmsGray}, {0, 0, 0, 1, 1},
        {cmsRed, cmsYellow, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta y_{ch,Z}", -4, 4,
        "dN/d#Delta y_{ch,Z}", -1, 1,
        false, false, false
    );

    /*
    AddCMSHeader(
        p1,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    AddUPCHeader(p1, "5.02 TeV", "pp");
    p1->Update();
    */

    c1->SaveAs(Form("%s.png", output));

}