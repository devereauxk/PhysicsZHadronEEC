#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_closure(const char* output = "output/closure.pdf") {

    // The first input file is considered the baseline (Gen+EPOS)
    // currently using a certain ZPT and track PT range
    // track pT: 0.5-10 GeV
    // Z pT: 0-40 GeV

    //PbP
    vector<string> input_ZPT_files = {
        "output/pPb-PbP-816.root",
        "output/pp-502.root",
        "output/pp-816.root"
    };
    vector<string> labels = {
        "pPb 8.16 TeV",
        "pp 5.02 TeV",
        "pp extrapolated 8.16 TeV"
    };

    vector<TH1*> hZPt;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // track pt eta Phi
        TH3D* this_hZPtEtaPhi = (TH3D*)fin->Get("h3D");
        TH1D* this_hZPt = this_hZPtEtaPhi->ProjectionX(Form("ZPt_%s", labels[i].c_str()));

        TH1D* hNZ = (TH1D*)fin->Get("hNZ");

        cout<<" "<<this_hZPtEtaPhi->Integral()<<endl;
        cout<<"hNZ bin content: "<<hNZ->Integral()<<endl;

        this_hZPt->Scale(1.0 / hNZ->GetBinContent(1));

        divideByWidth(this_hZPt);

        hZPt.push_back(this_hZPt);

        i++;
    }

    // make canvas
    TCanvas* cZ1 = new TCanvas("cZ", "cZ", 600, 600);

    TPad* pZ1 = (TPad*) plotCMSRatio(
        hZPt, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{Z}", 0, 500,
        "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", -1, -1,
        "Ratio to pPb", 0.9, 1.1,
        0,
        true, false, false
    );

    AddCMSHeader(
        pZ1,
        "Internal",
        false
    );

    AddUPCHeader(pZ1, "8 TeV", "pPb (pp)");

    cZ1->Update();
    cZ1->SaveAs(Form("%s", output));


}
