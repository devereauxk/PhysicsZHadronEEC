#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("tag", "V16_nmix5");

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s", collisionType.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_trkResidual_%s_ZPT%s", collisionType.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "MC DY-GEN",
        "MC DY-RECO (corrected)"
    };
    string output = Form("plots/trackResiduals/%s_ZPT%s_trkPT%s_%s", collisionType.c_str(), collisionType.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag.c_str());

    vector<TH1*> hTrkPtCorr_all;
    vector<TH1*> hTrkEtaCorr_all;
    vector<TH1*> hTrkPhiCorr_all;

    vector<TH1*> hTrkPtCorr_bkg;
    vector<TH1*> hTrkEtaCorr_bkg;
    vector<TH1*> hTrkPhiCorr_bkg;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // corrections sig+bkg
        TH3D* hTrkResidualCorrection = (TH3D*)fin->Get("hTrkResidualCorrection");
        TH1D* this_hTrkPtCorr = hTrkResidualCorrection->ProjectionX(Form("trkPtCorr_%s", labels[i].c_str()));
        TH1D* this_hTrkEtaCorr = hTrkResidualCorrection->ProjectionY(Form("trkEtaCorr_%s", labels[i].c_str()));
        TH1D* this_hTrkPhiCorr = hTrkResidualCorrection->ProjectionZ(Form("trkPhiCorr_%s", labels[i].c_str()));

        hTrkPtCorr_all.push_back(this_hTrkPtCorr);
        hTrkEtaCorr_all.push_back(this_hTrkEtaCorr);
        hTrkPhiCorr_all.push_back(this_hTrkPhiCorr);

        // correction bkg
        TH3D* hTrkResidualCorrectionMix = (TH3D*)fin->Get("hTrkResidualCorrectionMix");
        TH1D* this_hTrkPtCorrMix = hTrkResidualCorrectionMix->ProjectionX(Form("trkPtCorr_%s", labels[i].c_str()));
        TH1D* this_hTrkEtaCorrMix = hTrkResidualCorrectionMix->ProjectionY(Form("trkEtaCorr_%s", labels[i].c_str()));
        TH1D* this_hTrkPhiCorrMix = hTrkResidualCorrectionMix->ProjectionZ(Form("trkPhiCorr_%s", labels[i].c_str()));

        hTrkPtCorr_bkg.push_back(this_hTrkPtCorrMix);
        hTrkEtaCorr_bkg.push_back(this_hTrkEtaCorrMix);
        hTrkPhiCorr_bkg.push_back(this_hTrkPhiCorrMix);

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

    return 0;

}
