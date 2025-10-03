#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_comparison_PbP_PPb() {

    // The first input file is considered the baseline (Gen-level)
    // currently using a certain ZPT and track PT range
    /**
    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_ZPT0_40-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_noweight_ZPT0_40-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_TrackWeightOnly_ZPT0_40-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_EventWeightOnly_ZPT0_40-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZPT0_40-result.root"
    };
    vector<string> labels = {
        "Gen",
        "Reco (no weight)",
        "Reco (track weight)",
        "Reco (event weight)",
        "Reco (nominal weighting)"
    };
    const char* output =  "plots/pPb_TrackCor_closure_ZPT0_40-1_10";
    */

    vector<vector<string>> input_ZPT_files = {
        {"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZPT0_350-result.root",
            "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZPT0_350-result.root"},
        {"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_w1_ZPT0_350-result.root",
            "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_w0_ZPT0_350-result.root"}
    };

    vector<string> labels = {
        "reco PPb / PbP",
        "data PPb / PbP"
    };

    const char* output =  "track_summary/20251002_PbP-PPbRatio_ZPT0_350-1_10";

    vector<TH1*> hZPt;
    vector<TH1*> hZEta;

    // Loop over all input files
    for (int i = 0; i < 2; i++) {
        TFile* fin_ppb = TFile::Open(input_ZPT_files[i][0].c_str(), "READ");
        TFile* fin_pbp = TFile::Open(input_ZPT_files[i][1].c_str(), "READ");

        TH3D* this_hZPtEtaMult_ppb = (TH3D*)fin_ppb->Get("hZPtEtaMult_1_10");
        TH3D* this_hZPtEtaMult_pbp = (TH3D*)fin_pbp->Get("hZPtEtaMult_1_10");

        TH1D* this_hZPt_ppb = this_hZPtEtaMult_ppb->ProjectionX(Form("ZPt_%s-1", labels[i].c_str()));
        TH1D* this_hZPt_pbp = this_hZPtEtaMult_pbp->ProjectionX(Form("ZPt_%s-2", labels[i].c_str()));
        this_hZPt_ppb->Divide(this_hZPt_pbp);

        TH1D* this_hZEta_ppb = this_hZPtEtaMult_ppb->ProjectionY(Form("ZEta_%s-1", labels[i].c_str()));
        TH1D* this_hZEta_pbp = this_hZPtEtaMult_pbp->ProjectionY(Form("ZEta_%s-2", labels[i].c_str()));
        this_hZEta_ppb->Divide(this_hZEta_pbp);

        hZPt.push_back(this_hZPt_ppb);
        hZEta.push_back(this_hZEta_ppb);
    }


    // Define style vectors to be used in all plots
    std::vector<int> linecolors = {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsGray};
    std::vector<int> linestyles = {1, 1, 2, 1, 1, 0};
    std::vector<int> markercolors = {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsRed, cmsRed};
    std::vector<int> markerstyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};

    // make canvas
    TCanvas* c4 = new TCanvas("c4", "c4", 600, 600);
    TPad* p4 = (TPad*) plotCMSSimple(
        c4, hZPt, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "p_{T,Z}", 0, 200,
        "dN_{Z}/dp_{T,Z}", -1, -1,
        false, false, false, "1"
    );

    AddCMSHeader(
        p4,
        "Internal",
        false
    );

    AddUPCHeader(p4, "8 TeV", "pPb");
    p4->Update();

    c4->SaveAs(Form("%s-PPb_over_PbP_Zpt.pdf", output));

    // make canvas
    TCanvas* c5 = new TCanvas("c5", "c5", 600, 600);
    TPad* p5 = (TPad*) plotCMSSimple(
        c5, hZEta, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "y_{Z}", -2.4, 2.4,
        "dN_{Z}/dy_{Z}", -1, -1,
        false, false, false, "1"
    );

    AddCMSHeader(
        p5,
        "Internal",
        false
    );

    AddUPCHeader(p5, "8 TeV", "pPb");
    p5->Update();

    c5->SaveAs(Form("%s-PPb_over_PbP_Zeta.pdf", output));

}