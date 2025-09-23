#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plot_single(const char* input_ZPT, const char* output, const char* header_label) {

    // pp
    //const char* input_ZPT =  {"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZPT10_20-result.root"};
    //const char* output =  "plots/20250918_pp_ZPT10_20";
    //const char* header_label = "pp";

    //pPb
    //const char* input_ZPT =  {"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/1pPb_ZPT10_20-result.root"};
    //const char* output =  "plots/20250918_pPb_ZPT10_20";
    //const char* header_label = "pPb";

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

    // scale by 0.5 since we double count during filling
    hDeltaPhi1_2->Scale(0.5);
    hDeltaPhi2_4->Scale(0.5);
    hDeltaPhi4_10->Scale(0.5);

    hDeltaEta1_2->Scale(0.5);
    hDeltaEta2_4->Scale(0.5);
    hDeltaEta4_10->Scale(0.5);

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 650, 600);

    TPad* p1 = (TPad*) plotCMSSimple(
        c1, {hDeltaEta1_2, hDeltaEta2_4, hDeltaEta4_10}, "", {"1<p_{T}^{trk}<2 GeV", "2<p_{T}^{trk}<4 GeV", "4<p_{T}^{trk}<10 GeV"},
        {cmsRed, cmsYellow, cmsTealL1, cmsYellow, cmsGray}, {0, 0, 0, 1, 1},
        {cmsRed, cmsYellow, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -0.5, 0.5,
        false, false, false, 0
    );

    AddCMSHeader(
        p1,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    AddUPCHeader(p1, "5.02 TeV", header_label);
    p1->Update();

    c1->SaveAs(Form("%s-DeltaEta.png", output));

    //make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 650, 600);
    TPad* p2 = (TPad*) plotCMSSimple(
        c2, {hDeltaPhi1_2, hDeltaPhi2_4, hDeltaPhi4_10}, "", {"1<p_{T}^{trk}<2 GeV", "2<p_{T}^{trk}<4 GeV", "4<p_{T}^{trk}<10 GeV"},
        {cmsRed, cmsYellow, cmsTealL1, cmsYellow, cmsGray}, {0, 0, 0, 1, 1},
        {cmsRed, cmsYellow, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta#phi_{ch,Z}", -1.5758, 4.7275,
        "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, 2,
        false, false, false, 0
    );

    AddCMSHeader(
        p2,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    AddUPCHeader(p2, "5.02 TeV", header_label);
    p2->Update();

    c2->SaveAs(Form("%s-DeltaPhi.png", output));

}