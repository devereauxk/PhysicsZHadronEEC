#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_trackCor_closure() {
    /*
    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZPT0_350-result.root"
    };
    vector<string> labels = {
        "pPb data",
        "pPb MC gen",
        "pPb MC reco",
        "PbP data",
        "PbP MC gen",
        "PbP MC reco"
    };
    const char* output =  "track_summary/20251013_PbP-PPb_compare_UE_beforeBoost_ZPT0_350-1_10";
    */

    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_yboost_ZPT0_350-result.root"
    };
    vector<string> labels = {
        "pPb MC gen",
        "pPb MC reco",
        "pPb MC reco + correction"
    };
    const char* output =  "track_summary/20251202_closure_pPb_ZPT0_350-0.5_10";
    const char* trkPT_range = "0.5_10";

    /*
    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_yboost_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_yboost_ZPT0_350-result.root"
    };
    vector<string> labels = {
        "pPb data",
        "pPb MC reco",
        "PbP data",
        "PbP MC reco"
    };
    const char* output =  "track_summary/20251017_PbP-PPb_compare_UE_afterBoost_noTrackWeight_ZPT0_350-0.5_10";
    const char* trkPT_range = "0.5_10";
    */

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;
    vector<TH1*> hMult;
    vector<TH1*> hZPt;
    vector<TH1*> hZEta;
    vector<TH1*> hVZ;
    
    // shift of y_cm = y_lab - 0.465 for pPb
    // shift of y_cm = - (y_lab + 0.465) for PbP

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH2D* this_hTrkPtEta = (TH2D*)fin->Get(Form("hTrkPtEtaData_%s", trkPT_range));
        TH3D* this_hZPtEtaMult = (TH3D*)fin->Get(Form("hZPtEtaMult_%s", trkPT_range));

        TH1D* this_hTrkPt = this_hTrkPtEta->ProjectionX(Form("trkPt_%s", labels[i].c_str()));
        TH1D* this_hTrkEta = this_hTrkPtEta->ProjectionY(Form("trkEta_%s", labels[i].c_str()));
        TH1D* this_hMult = this_hZPtEtaMult->ProjectionZ(Form("mult_%s", labels[i].c_str()));
        TH1D* this_hZPt = this_hZPtEtaMult->ProjectionX(Form("ZPt_%s", labels[i].c_str()));
        TH1D* this_hZEta = this_hZPtEtaMult->ProjectionY(Form("ZEta_%s", labels[i].c_str()));

        TH1D* this_hVZ = (TH1D*)fin->Get(Form("hVZ_%s", trkPT_range));
        this_hVZ->Scale(1.0 / this_hVZ->Integral());

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hZEta);
        divideByWidth(this_hZPt);

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);
        hMult.push_back(this_hMult);
        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);
        hVZ.push_back(this_hVZ);

        i++;
    }
    // Define style vectors to be used in all plots
    std::vector<int> linecolors = {cmsRed, cmsBlue, cmsRed, kOrange+7, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsGray};
    std::vector<int> linestyles = {0, 1, 2, 0, 1, 2, 1, 0};
    std::vector<int> markercolors = {cmsRed, cmsBlue, cmsRed, kOrange+7, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsRed, cmsRed};
    std::vector<int> markerstyles = {mCircleFill, mCircleFill, mCircleFill, mSquareFill, mCircleFill, mCircleFill};

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* p1 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "y_{ch}", -4, 4,
        "(1/N_{Z}) dN_{ch}/d y_{ch}", 0, 30,
        "Ratio to curve1", 0.5, 2,
        0,
        false, false, false
    );

    AddCMSHeader(
        p1,
        "Internal",
        true
    );

    AddUPCHeader(p1, "8 TeV", "pPb");
    p1->Update();

    c1->SaveAs(Form("%s-eta.pdf", output));

    // make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
    TPad* p2 = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "p_{T}^{ch}", 0, 15,
        "(1/N_{Z}) dN_{ch}/dp_{T}^{ch}", 1e-3, 1e3,
        "Ratio to curve1", 0.5, 2,
        0,
        false, true, false
    );

    AddCMSHeader(
        p2,
        "Internal",
        false
    );

    AddUPCHeader(p2, "8 TeV", "pPb");
    p2->Update();

    c2->SaveAs(Form("%s-pt.pdf", output));

    // make canvas
    TCanvas* c3 = new TCanvas("c3", "c3", 600, 600);
    TPad* p3 = (TPad*) plotCMSRatio(
        {hMult[0], hMult[1], hMult[2]}, "", labels,
        {cmsRed, cmsBlue, cmsTeal}, {1, 1, 1},
        markercolors, markerstyles,
        "N_{ch}", 1, 300,
        "counts", 0, 0.025,
        "Ratio to curve1", 0, 2,
        0,
        true, false, false
    );

    AddCMSHeader(
        p3,
        "Internal",
        false
    );

    AddUPCHeader(p3, "8 TeV", "pPb");
    p3->Update();

    c3->SaveAs(Form("%s-mult.pdf", output));

    // make canvas
    TCanvas* c4 = new TCanvas("c4", "c4", 600, 600);
    TPad* p4 = (TPad*) plotCMSRatio(
        hZPt, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "p_{T,Z}", 0, 400,
        "(1/N_{Z}) dN_{Z}/dp_{T,Z}", -1, -1,
        "Ratio to curve1", 0.5, 1.5,
        0,
        false, true, false
    );

    AddCMSHeader(
        p4,
        "Internal",
        false
    );

    AddUPCHeader(p4, "8 TeV", "pPb");
    p4->Update();

    c4->SaveAs(Form("%s-Zpt.pdf", output));

    // make canvas
    TCanvas* c5 = new TCanvas("c5", "c5", 600, 600);
    TPad* p5 = (TPad*) plotCMSRatio(
        hZEta, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "y_{Z}", -3, 3,
        "(1/N_{Z}) dN_{Z}/dy_{Z}", 0, 0.5,
        "Ratio to curve1", 0.75, 1.25,
        0,
        false, false, false
    );

    AddCMSHeader(
        p5,
        "Internal",
        false
    );

    AddUPCHeader(p5, "8 TeV", "pPb");
    p5->Update();

    c5->SaveAs(Form("%s-Zeta.pdf", output));

    // make canvas
    TCanvas* c6 = new TCanvas("c6", "c6", 600, 600);
    TPad* p6 = (TPad*) plotCMSRatio(
        hVZ, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "V_{Z}", -20, 20,
        "counts", -1, -1,
        "Ratio to curve1", 0.5, 1.5,
        0,
        false, false, false
    );

    AddCMSHeader(
        p6,
        "Internal",
        false
    );

    AddUPCHeader(p6, "8 TeV", "pPb");
    p6->Update();
    c6->SaveAs(Form("%s-Vz.pdf", output));

}