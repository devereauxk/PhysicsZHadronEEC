#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_Zpeak_pPb() {

    // The first input file is considered the baseline (Gen+EPOS)
    // currently using a certain ZPT and track PT range

    //pPb
    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_residual_V16_ZPT40_500-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_V16_ZPT40_500-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_residual_V16_ZPT40_500-result.root",
    };
    vector<string> labels = {
        "pPb DATA",
        "MC Gen + EPOS (x 0.0196)",
        "MC Reco (x 0.0169)" 
    };
    const char* pT_range = "0.5_500";
    const char* output =  "plots/pp-pPb_ZPT40_500-0.5_500-Zmass";

    vector<TH1*> hZmass;

    // Z mass raw counts distribution
    // raw counts for pPb data
    // but MC integral scaled to pPb so it is more readable

    // Loop over all input files
    int i = 0;
    float pPb_integral = 1.0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        cout<<"Opening file: "<<input_ZPT<<endl;

        // track pt eta phi
        TH1D* this_hZmass = (TH1D*)fin->Get(Form("hZmassData_%s", pT_range));
        this_hZmass->SetName(Form("Zmass_%d", i));

        hZmass.push_back(this_hZmass);

        if(i==0) pPb_integral = this_hZmass->Integral();
        if(i > 0) {
            float scale = pPb_integral / this_hZmass->Integral();
            this_hZmass->Scale(scale);
            cout<<"Scaling MC histogram by: "<<scale<<endl;
        }

        i++;
    }

    vector<int> markerColors = {cmsBlue, cmsRed, cmsYellow, kSpring+7, cmsTealL1, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, cmsYellow, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 1, 2, 0, 0, 1};

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* pTrk1 = (TPad*) plotCMSSimple(
        c1,
        hZmass, "", labels,
        lineColors, lineStyles,
        markerColors, markerStyles,
        "M_{#mu} (GeV/c^{2})", 60, 120,
        "Entries / (2 GeV/c^{2})", 0, 350,
        false, false, false
    );

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, "8 TeV", "pPb");
    c1->Update();

    c1->SaveAs(Form("%s-Zmass.pdf", output));

}
