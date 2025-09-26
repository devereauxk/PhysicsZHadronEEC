#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_trackCor_closure() {

    // The first input file is considered the baseline (Gen-level)
    // currently using a certain ZPT and track PT range
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

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;
    vector<TH1*> hMult;
    vector<TH1*> hZPt;
    vector<TH1*> hZEta;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH2D* this_hTrkPtEta = (TH2D*)fin->Get("hTrkPtEtaData_1_10");
        TH3D* this_hZPtEtaMult = (TH3D*)fin->Get("hZPtEtaMult_1_10");

        TH1D* this_hTrkPt = this_hTrkPtEta->ProjectionX(Form("trkPt_%s", labels[i].c_str()));
        TH1D* this_hTrkEta = this_hTrkPtEta->ProjectionY(Form("trkEta_%s", labels[i].c_str()));
        TH1D* this_hMult = this_hZPtEtaMult->ProjectionZ(Form("mult_%s", labels[i].c_str()));
        TH1D* this_hZPt = this_hZPtEtaMult->ProjectionX(Form("ZPt_%s", labels[i].c_str()));
        TH1D* this_hZEta = this_hZPtEtaMult->ProjectionY(Form("ZEta_%s", labels[i].c_str()));

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hZEta);
        divideByWidth(this_hZPt);

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);
        hMult.push_back(this_hMult);
        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);

        i++;
    }

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* p1 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "y_{ch}", -4, 4,
        "dN_{ch}/d y_{ch}", 0, 8e6,
        "Ratio to Gen-level", 0.5, 2.5,
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

    c1->SaveAs(Form("%s-eta.png", output));

    // make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
    TPad* p2 = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{ch}", 1, 15,
        "dN_{ch}/dp_{T}^{ch}", 1e2, 1e8,
        "Ratio to Gen-level", 0.5, 2,
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

    c2->SaveAs(Form("%s-pt.png", output));

    // make canvas
    TCanvas* c3 = new TCanvas("c3", "c3", 600, 600);
    TPad* p3 = (TPad*) plotCMSRatio(
        {hMult[0], hMult[1]}, "", labels,
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "N_{ch}", 1, 300,
        "counts", -1, -1,
        "Ratio to Gen-level", 0.5, 1.5,
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

    c3->SaveAs(Form("%s-mult.png", output));

    // make canvas
    TCanvas* c4 = new TCanvas("c4", "c4", 600, 600);
    TPad* p4 = (TPad*) plotCMSRatio(
        hZPt, "", labels,
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T,Z}", 0, 400,
        "dN_{Z}/dp_{T,Z}", -1, -1,
        "Ratio to Gen-level", 0.9, 1.1,
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

    c4->SaveAs(Form("%s-Zpt.png", output));

    // make canvas
    TCanvas* c5 = new TCanvas("c5", "c5", 600, 600);
    TPad* p5 = (TPad*) plotCMSRatio(
        hZEta, "", labels,
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "y_{Z}", -2.4, 2.4,
        "dN_{Z}/dy_{Z}", -1, -1,
        "Ratio to Gen-level", 0.9, 1.1,
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

    c5->SaveAs(Form("%s-Zeta.png", output));

}
