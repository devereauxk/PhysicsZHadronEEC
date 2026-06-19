#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_closure(const char* output = "plots/isPbP") {

    //PbP
    vector<string> input_ZPT_files = {
        "output/DY-GEN.root",
        "output/DY-RECO-noResidual.root",
        "output/DY-RECO.root",
    };
    vector<string> labels = {
        "MC DY-GEN",
        "MC DY-RECO",
        "MC DY-RECO (corrected)"
    };

    vector<TH1*> hZPt;
    vector<TH1*> hZEta;

    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH2D* this_h2D = (TH2D*)fin->Get("h2D");
        TH1D* this_hZPt  = this_h2D->ProjectionX(Form("ZPt_%s",  labels[i].c_str()));
        TH1D* this_hZEta = this_h2D->ProjectionY(Form("ZEta_%s", labels[i].c_str()));

        TH1D* hNZ = (TH1D*)fin->Get("hNZ");

        cout << " " << this_h2D->Integral() << endl;
        cout << "hNZ bin content: " << hNZ->Integral() << endl;

        this_hZPt->Scale(1.0 / hNZ->GetBinContent(1));
        this_hZEta->Scale(1.0 / hNZ->GetBinContent(1));

        divideByWidth(this_hZPt);
        divideByWidth(this_hZEta);

        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);

        i++;
    }

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

    AddCMSHeader(pZ1, "Internal", false);
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
}
