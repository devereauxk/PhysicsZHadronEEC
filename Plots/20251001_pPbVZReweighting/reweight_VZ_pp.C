#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void reweight_VZ_pp() {

    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_nominal_noEvtWeight_nmix1_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_noEvtWeight_nmix1_ZPT0_350-result.root"
    };
    vector<string> labels = {
        "Reco pp",
        "Data pp"
    };
    const char* output =  "summary/20260308_ZPT0_350";

    vector<TH1*> hVZ;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH1D* this_hVZ = (TH1D*)fin->Get("hVZ_1_10");

        this_hVZ->Scale(1.0 / this_hVZ->Integral());

        hVZ.push_back(this_hVZ);

        i++;
    }

    // fit quartic function to Data / MC VZ Ratio
    TH1D* hVZRatio_pp = (TH1D*)hVZ[1]->Clone("hVZRatio_pp");
    hVZRatio_pp->Divide(hVZ[0]);
    TF1* fitFunc_pp = new TF1("VZ_reweight_pp", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x", -20, 20);
    fitFunc_pp->SetParameters(1, 0, 0, 0, 0); // Initial guess for parameters
    hVZRatio_pp->Fit(fitFunc_pp, "R");

    // save fits to root file
    TFile* fout = TFile::Open("20260308_VzReweightFits_pp.root", "RECREATE");
    fitFunc_pp->Write();
    fout->Close();

    //reweight the MC
    TH1D* hVZReweighted_pp = (TH1D*)hVZ[0]->Clone("hVZReweighted_pp");
    for (int bin = 1; bin <= hVZReweighted_pp->GetNbinsX(); ++bin) {
        double vz = hVZReweighted_pp->GetBinCenter(bin);
        double weight = fitFunc_pp->Eval(vz);
        double originalContent = hVZReweighted_pp->GetBinContent(bin);
        hVZReweighted_pp->SetBinContent(bin, originalContent * weight);
    }

    // ================================================== //
    // pp plots
    // ================================================== //
    // make canvas
    TCanvas* c6 = new TCanvas("c6", "c6", 600, 600);
    TPad* p6 = (TPad*) plotCMSRatio(
        {hVZ[1], hVZ[0], hVZReweighted_pp}, "", {labels[1], labels[0], "Reco pp (reweighted)"},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsGray}, {1, 2, 1, 2, 1, 0},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "V_{Z}", -20, 20,
        "counts [unit normalization]", -1, -1,
        "MC / Data", 0, 2,
        0,
        false, false, false
    );

    AddCMSHeader(
        p6,
        "Internal",
        false
    );

    AddUPCHeader(p6, "8.16 TeV", "pp");
    p6->Update();
    c6->SaveAs(Form("%s-Vz_pp.pdf", output));


    // make canvas
    TCanvas* c7 = new TCanvas("c7", "c7", 650, 600);
    TPad* p7 = (TPad*) plotCMSSimple(
        c7, {hVZRatio_pp}, "", {"Data / MC VZ Ratio"},
        {cmsRed}, {0},
        {cmsRed}, {mCircleFill},
        "V_{Z}", -20, 20,
        "Data / MC V_{Z} Ratio", 0, 2,
        false, false, false, 1
    );

    AddCMSHeader(
        p7,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    fitFunc_pp->SetLineColor(kBlue+2);
    fitFunc_pp->Draw("same");

    AddUPCHeader(p7, "5.02 TeV", "pp");
    p7->Update();
    c7->SaveAs(Form("%s-VzRatio_pp.pdf", output));


}