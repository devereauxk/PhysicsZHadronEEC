#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_eventweights(const char* system = "RECO", const char* output = "plots/eventweight") {

    // The first input file is considered the baseline (Gen+EPOS)
    // currently using a certain ZPT and track PT range
    // track pT: 0.5-10 GeV
    // Z pT: 0-40 GeV

    //PbP
    vector<string> input_ZPT_files = {
        Form("output/DY-%s-eventWeight-pPb.root", system),
        Form("output/DY-%s-eventWeight-PbP.root", system)
    };
    vector<string> labels = {
        Form("MC DY-%s pPb, uncorrected", system),
        Form("MC DY-%s PbP, uncorrected", system)
    };

    vector<TH1*> hEventWeight;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // track pt eta Mult
        TH1D* this_hEventWeight = (TH1D*)fin->Get("hEventWeight");

        hEventWeight.push_back(this_hEventWeight);

        i++;
    }

    TCanvas* cZ2 = new TCanvas("cZ2", "cZ2", 600, 600);
    TPad* pZ2 = (TPad*) plotCMSSimple(
        cZ2, hEventWeight, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {1, 1, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "weight", 0, 3,
        "counts", -1, -1,
        false, false, false
    );

    cZ2->Update();
    cZ2->SaveAs(Form("%s.pdf", output));
}
