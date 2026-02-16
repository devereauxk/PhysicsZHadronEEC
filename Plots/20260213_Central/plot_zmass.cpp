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
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_Zclosure_ZPT%s-nosub.root", mctag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_Zclosure_ZPT%s-nosub.root", mctag.c_str(), zPtRange.c_str())
    };
    
    string output =  Form("plots/zmass/%s_ZPT%s-%s", collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hZmass;
    float genscale = 1;
    float recoscale = 1;

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
        TH1D* this_hZmass = (TH1D*)fin->Get(Form("hZmassData_%s", trkPtRange.c_str()));
        this_hZmass->SetName(Form("Zmass_%d", i));

        hZmass.push_back(this_hZmass);

        if(i==0) pPb_integral = this_hZmass->Integral();
        if(i > 0) {
            float scale = pPb_integral / this_hZmass->Integral();
            this_hZmass->Scale(scale);
            cout<<"Scaling MC histogram by: "<<scale<<endl;

            if (i==1) genscale = scale;
            else if (i==2) recoscale = scale;
        }

        i++;
    }

    vector<string> labels = {
        Form("%s DATA", collisionType.c_str()),
        Form("MC Gen (x %.4f)", genscale),
        Form("MC Reco (x %.4f)", recoscale)
    };

    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 1, 0, 1};

    int max_genpeak = hZmass[1]->GetMaximum();

    // ===========================================
    // Z mass peak
    // ===========================================
    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* pTrk1 = (TPad*) plotCMSSimple(
        c1,
        hZmass, "", labels,
        lineColors, lineStyles,
        markerColors, markerStyles,
        "M_{mu mu} (GeV/c^{2})", 60, 120,
        "Entries / (2 GeV/c^{2})", -1, -1,
        false, false, false
    );

    hZmass[0]->GetYaxis()->SetRangeUser(0, max_genpeak*1.5);

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    c1->Update();

    c1->SaveAs(Form("%s.pdf", output.c_str()));
    

}
