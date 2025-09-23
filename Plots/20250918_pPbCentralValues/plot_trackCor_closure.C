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
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nocor_ZPT0_40-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_EventCorOnly_ZPT0_40-result.root"
        //"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_TrackCorOnly_ZPT0_40-result.root",
        //"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZPT0_40-result.root"
    };
    vector<string> labels = {
        "Gen",
        "Reco (no correction)",
        "Reco (event correction only)",
        "Reco (track correction only)",
        "Reco (nominal weighting)"
    };
    const char* output =  "plots/pPb_TrackCor_closure_ZPT0_40-1_10";

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH2D* this_hTrkPtEta = (TH2D*)fin->Get("hTrkPtEtaData_1_10");

        TH1D* this_hTrkPt = this_hTrkPtEta->ProjectionX(Form("trkPt_%s", labels[i].c_str()));
        TH1D* this_hTrkEta = this_hTrkPtEta->ProjectionY(Form("trkEta_%s", labels[i].c_str()));

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);

        i++;
    }

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* p1 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta y_{ch,Z}", -4, 4,
        "dN_{ch}/d#Delta y_{ch,Z}", -1, -1,
        "Ratio to Gen-level", 0.5, 2.5,
        0,
        false, false, false
    );

    AddCMSHeader(
        p1,
        "Internal",
        false
    );

    AddUPCHeader(p1, "8 TeV", "pPb");
    p1->Update();

    c1->SaveAs(Form("%s-eta.png", output));

    // make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
    TPad* p2 = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{ch} [GeV]", 1, 10,
        "dN_{ch}/dp_{T}^{ch}", -1, -1,
        "Ratio to Gen-level", 0.5, 1.5,
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

}
