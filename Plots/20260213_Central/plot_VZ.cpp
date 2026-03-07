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

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;

    string mctag = (collisionType == "pp") ? "pythia" : collisionType;

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%s_nominal_%s_ZPT%s-nosub.root", collisionType.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s-nosub.root", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_%s_ZPT%s-nosub.root", mctag.c_str(), tag.c_str(), zPtRange.c_str())
    };
    
    string output =  Form("plots/VZ/%s_ZPT%s-%s", collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hVZ;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        cout<<"Opening file: "<<input_ZPT<<endl;

        // track pt eta phi
        TH1D* this_hVZ = (TH1D*)fin->Get(Form("hVZ_%s", trkPtRange.c_str()));
        this_hVZ->SetName(Form("VZ_%d", i));

        this_hVZ->Scale(1.0 / this_hVZ->Integral());

        hVZ.push_back(this_hVZ);

        i++;
    }

    vector<string> labels = {
        Form("%s DATA", collisionType.c_str()),
        "MC Gen",
        "MC Reco"
    };

    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 1, 0, 1};

    // ===========================================
    // VZ 
    // ===========================================
    TCanvas* c6 = new TCanvas("c6", "c6", 600, 600);
    TPad* p6 = (TPad*) plotCMSRatio(
        hVZ, "", labels,
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
    c6->SaveAs(Form("%s-Vz.pdf", output.c_str()));

}
