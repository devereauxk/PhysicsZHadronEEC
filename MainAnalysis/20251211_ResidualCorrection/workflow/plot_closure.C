#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../../../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_closure() {

    // The first input file is considered the baseline (Gen+EPOS)
    // currently using a certain ZPT and track PT range
    // track pT: 0.5-10 GeV
    // Z pT: 0-40 GeV

    //PbP
    vector<string> input_ZPT_files = {
        "output/DY-GEN.root",
        "output/DY-RECO.root",
    };
    vector<string> labels = {
        "MC Gen + EPOS",
        "MC Reco"
    };
    const char* output =  "plots/isPP";


    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;
    vector<TH1*> hTrkPhi;
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaPhi;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // track pt eta phi
        TH3D* this_hTrkPtEtaPhi = (TH3D*)fin->Get("h3D");
        TH1D* this_hTrkPt = this_hTrkPtEtaPhi->ProjectionX(Form("trkPt_%s", labels[i].c_str()));
        TH1D* this_hTrkEta = this_hTrkPtEtaPhi->ProjectionY(Form("trkEta_%s", labels[i].c_str()));
        TH1D* this_hTrkPhi = this_hTrkPtEtaPhi->ProjectionZ(Form("trkPhi_%s", labels[i].c_str()));

        TH1D* hNZ = (TH1D*)fin->Get("hNZ");

        this_hTrkPt->Scale(1.0 / hNZ->GetBinContent(1));
        this_hTrkEta->Scale(1.0 / hNZ->GetBinContent(1));
        this_hTrkPhi->Scale(1.0 / hNZ->GetBinContent(1));

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hTrkPhi);

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);
        hTrkPhi.push_back(this_hTrkPhi);

        i++;
    }

    // make canvas
    TCanvas* cTrk1 = new TCanvas("cTrk", "cTrk", 600, 600);

    TPad* pTrk1 = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{ch}", 0, 10,
        "(1/N_{Z}) dN_{ch}/dp_{T}^{ch}", -1, -1,
        "Ratio to Gen+EPOS", 0.9, 1.1,
        0,
        true, false, false
    );

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, "8 TeV", "pPb MC");

    cTrk1->Update();
    cTrk1->SaveAs(Form("%s-pt.pdf", output));

    TCanvas* cTrk2 = new TCanvas("cTrk2", "cTrk2", 600, 600);
    TPad* pTrk2 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "y_{ch}", -4, 4,
        "(1/N_{Z}) dN_{ch}/d y_{ch}", 8, 22,
        "Ratio to Gen+EPOS", 0.8, 1.2,
        0,
        false, false, false
    );

    cTrk2->Update();
    cTrk2->SaveAs(Form("%s-eta.pdf", output));

    TCanvas* cTrk3 = new TCanvas("cTrk3", "cTrk3", 600, 600);
    TPad* pTrk3 = (TPad*) plotCMSRatio(
        hTrkPhi, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#phi_{ch}", -3.2, 3.2,
        "(1/N_{Z}) dN_{ch}/d #phi_{ch}", 6, 18,
        "Ratio to Gen+EPOS", 0.8, 1.2,
        0,
        false, false, false
    );

    cTrk3->Update();
    cTrk3->SaveAs(Form("%s-phi.pdf", output));

}
