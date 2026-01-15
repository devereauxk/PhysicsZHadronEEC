#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

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
    const char* output =  "plots/isPbP";


    vector<TH1*> hZPt;
    vector<TH1*> hZEta;
    vector<TH1*> hZMult;
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaMult;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // track pt eta Mult
        TH3D* this_hZPtEtaMult = (TH3D*)fin->Get("h3D");
        TH1D* this_hZPt = this_hZPtEtaMult->ProjectionX(Form("ZPt_%s", labels[i].c_str()));
        TH1D* this_hZEta = this_hZPtEtaMult->ProjectionY(Form("ZEta_%s", labels[i].c_str()));
        TH1D* this_hZMult = this_hZPtEtaMult->ProjectionZ(Form("ZMult_%s", labels[i].c_str()));

        TH1D* hNZ = (TH1D*)fin->Get("hNZ");

        this_hZPt->Scale(1.0 / hNZ->GetBinContent(1));
        this_hZEta->Scale(1.0 / hNZ->GetBinContent(1));
        this_hZMult->Scale(1.0 / hNZ->GetBinContent(1));

        divideByWidth(this_hZPt);
        divideByWidth(this_hZEta);
        divideByWidth(this_hZMult);

        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);
        hZMult.push_back(this_hZMult);

        i++;
    }

    // make canvas
    TCanvas* cZ1 = new TCanvas("cZ", "cZ", 600, 600);

    TPad* pZ1 = (TPad*) plotCMSRatio(
        hZPt, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{Z}", 0, 50,
        "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", -1, -1,
        "Ratio to Gen+EPOS", 0.9, 1.1,
        0,
        true, false, false
    );

    AddCMSHeader(
        pZ1,
        "Internal",
        false
    );

    AddUPCHeader(pZ1, "8 TeV", "pPb MC");

    cZ1->Update();
    cZ1->SaveAs(Form("%s-pt.pdf", output));

    TCanvas* cZ2 = new TCanvas("cZ2", "cZ2", 600, 600);
    TPad* pZ2 = (TPad*) plotCMSRatio(
        hZEta, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "y_{Z}", -4, 4,
        "(1/N_{Z}) dN_{Z}/d y_{Z}", -1, -1,
        "Ratio to Gen+EPOS", 0.8, 1.2,
        0,
        false, false, false
    );

    cZ2->Update();
    cZ2->SaveAs(Form("%s-eta.pdf", output));

    TCanvas* cZ3 = new TCanvas("cZ3", "cZ3", 600, 600);
    TPad* pZ3 = (TPad*) plotCMSRatio(
        hZMult, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Mult_{Z}", 2, 500,
        "(1/N_{Z}) dN_{Z}/d #Mult_{Z}", -1, -1,
        "Ratio to Gen+EPOS", 0.8, 1.2,
        0,
        true, false, false
    );

    cZ3->Update();
    cZ3->SaveAs(Form("%s-Mult.pdf", output));

}
