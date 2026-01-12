#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_residualcorrection() {

    // The first input file is considered the baseline (Gen+EPOS)
    // currently using a certain ZPT and track PT range

    // PbP or PPb
    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_residual_V16_nmix5_ZPT40_500-result.root"
    };
    vector<string> labels = {
        "MC Reco (residual)"
    };
    const char* output =  "plots/PbP_corr_ZPT40_500-0.5_500_nmix5";

    vector<TH1*> hTrkPtCorr;
    vector<TH1*> hTrkEtaCorr;
    vector<TH1*> hTrkPhiCorr;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // corrections
        TH3D* hTrkResidualCorrectionPtEtaPhi = (TH3D*)fin->Get("hTrkResidualCorrectionPtEtaPhi_0.5_500");
        TH1D* this_hTrkPtCorr = hTrkResidualCorrectionPtEtaPhi->ProjectionX(Form("trkPtCorr_%s", labels[i].c_str()));
        TH1D* this_hTrkEtaCorr = hTrkResidualCorrectionPtEtaPhi->ProjectionY(Form("trkEtaCorr_%s", labels[i].c_str()));
        TH1D* this_hTrkPhiCorr = hTrkResidualCorrectionPtEtaPhi->ProjectionZ(Form("trkPhiCorr_%s", labels[i].c_str()));

        hTrkPtCorr.push_back(this_hTrkPtCorr);
        hTrkEtaCorr.push_back(this_hTrkEtaCorr);
        hTrkPhiCorr.push_back(this_hTrkPhiCorr);


        i++;
    }

    // make canvas
    TCanvas* cTrk1 = new TCanvas("cTrk", "cTrk", 600, 600);

    TPad* pTrk1 = (TPad*) plotCMSSimple(
        cTrk1, hTrkPtCorr, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{ch}", 0, 100,
        "correction", -1, -1,
        true, false, false
    );

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, "8 TeV", "pPb MC");

    cTrk1->SaveAs(Form("%s-pt.pdf", output));

    TCanvas* cTrk2 = new TCanvas("cTrk2", "cTrk2", 600, 600);

    TPad* pTrk2 = (TPad*) plotCMSSimple(
        cTrk2, hTrkEtaCorr, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#eta^{ch}", -2.4, 2.4,
        "correction", -1, -1,
        false, false, false
    );

    AddCMSHeader(
        pTrk2,
        "Internal",
        false
    );

    AddUPCHeader(pTrk2, "8 TeV", "pPb MC");

    cTrk2->SaveAs(Form("%s-eta.pdf", output));

    TCanvas* cTrk3 = new TCanvas("cTrk3", "cTrk3", 600, 600);

    TPad* pTrk3 = (TPad*) plotCMSSimple(
        cTrk3, hTrkPhiCorr, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#phi^{ch}", -3.2, 3.2,
        "correction", -1, -1,
        false, false, false
    );

    AddCMSHeader(
        pTrk3,
        "Internal",
        false
    );

    AddUPCHeader(pTrk3, "8 TeV", "pPb MC");

    cTrk3->SaveAs(Form("%s-phi.pdf", output));

}
