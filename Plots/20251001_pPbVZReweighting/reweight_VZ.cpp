#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("tag", "V16_nmix5");
    string outputTag = CL.Get("output", "20260308_ZPT0_350");

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;

    string mctag = (collisionType == "pp") ? "pythia" : collisionType;

    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_%s_ZPT%s-result.root", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%s_nominal_%s_ZPT%s-result.root", collisionType.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "Reco pp",
        "Data pp"
    };
    string output =  Form("summary/%s", outputTag.c_str());

    vector<TH1*> hVZ;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH1D* this_hVZ = (TH1D*)fin->Get(Form("hVZ_%s", trkPtRange.c_str()));

        this_hVZ->Scale(1.0 / this_hVZ->Integral());

        hVZ.push_back(this_hVZ);

        i++;
    }

    // fit quartic function to Data / MC VZ Ratio
    TH1D* hVZRatio = (TH1D*)hVZ[1]->Clone("hVZRatio");
    hVZRatio->Divide(hVZ[0]);
    TF1* fitFunc_pp = new TF1("VZ_reweight", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x", -20, 20);
    fitFunc_pp->SetParameters(1, 0, 0, 0, 0); // Initial guess for parameters
    hVZRatio->Fit(fitFunc_pp, "R");

    // save fits to root file
    TFile* fout = TFile::Open(Form("%s_VzReweightFits_%s.root", output.c_str(), collisionType.c_str()), "RECREATE");
    fitFunc_pp->Write();
    fout->Close();

    //reweight the MC
    TH1D* hVZReweighted = (TH1D*)hVZ[0]->Clone("hVZReweighted");
    for (int bin = 1; bin <= hVZReweighted->GetNbinsX(); ++bin) {
        double vz = hVZReweighted->GetBinCenter(bin);
        double weight = fitFunc_pp->Eval(vz);
        double originalContent = hVZReweighted->GetBinContent(bin);
        hVZReweighted->SetBinContent(bin, originalContent * weight);
    }

    // ================================================== //
    // pp plots
    // ================================================== //
    // make canvas
    TCanvas* c6 = new TCanvas("c6", "c6", 600, 600);
    TPad* p6 = (TPad*) plotCMSRatio(
        {hVZ[1], hVZ[0], hVZReweighted}, "", {labels[1], labels[0], "Reco pp (reweighted)"},
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

    AddUPCHeader(p6, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    p6->Update();
    c6->SaveAs(Form("%s-Vz_%s.pdf", output.c_str(), collisionType.c_str()));


    // make canvas
    TCanvas* c7 = new TCanvas("c7", "c7", 650, 600);
    TPad* p7 = (TPad*) plotCMSSimple(
        c7, {hVZRatio}, "", {"Data / MC VZ Ratio"},
        {cmsRed}, {0},
        {cmsRed}, {mCircleFill},
        "V_{Z}", -20, 20,
        "Data / MC V_{Z} Ratio", 0, 2,
        false, false, false
    );

    AddCMSHeader(
        p7,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    fitFunc_pp->SetLineColor(kBlue+2);
    fitFunc_pp->Draw("same");

    AddUPCHeader(p7, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    p7->Update();
    c7->SaveAs(Form("%s-VzRatio_%s.pdf", output.c_str(), collisionType.c_str()));

    return 0;


}