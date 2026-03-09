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
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_%s_ZPT%s", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_ZResidual_%s_ZPT%s", mctag.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "MC DY-GEN",
        "MC DY-RECO",
        "MC DY-RECO (corrected)"
    };
    string output = Form("plots/%s/ZPT%s_%s-closure", collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hZPt;
    vector<TH1*> hZEta;
    vector<TH1*> hZPhi;
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaPhi;

    // Loop over nosub files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        cout<<"opening file: "<<Form("%s-nosub.root", input_ZPT.c_str())<<endl;

        TFile* fin = TFile::Open(Form("%s-nosub.root", input_ZPT.c_str()), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // Z pt eta Phi
        TH3D* this_hZPtEtaPhi = (TH3D*)fin->Get(Form("hZPtEtaPhi_%s", trkPtRange.c_str()));
        TH1D* this_hZPt = this_hZPtEtaPhi->ProjectionX(Form("ZPt_%s", labels[i].c_str()));
        TH1D* this_hZEta = this_hZPtEtaPhi->ProjectionY(Form("ZEta_%s", labels[i].c_str()));
        TH1D* this_hZPhi = this_hZPtEtaPhi->ProjectionZ(Form("ZPhi_%s", labels[i].c_str()));

        TH1D* hNZ = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));

        cout<<" "<<this_hZPtEtaPhi->Integral()<<endl;
        cout<<"hNZ bin content: "<<hNZ->Integral()<<endl;

        divideByWidth(this_hZPt);
        divideByWidth(this_hZEta);
        divideByWidth(this_hZPhi);

        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);
        hZPhi.push_back(this_hZPhi);

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
        "Ratio to GEN", 0.9, 1.1,
        0,
        true, false, false
    );

    AddCMSHeader(
        pZ1,
        "Internal",
        false
    );

    AddUPCHeader(pZ1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);

    cZ1->Update();
    cZ1->SaveAs(Form("%s-pt.pdf", output.c_str()));

    TCanvas* cZ2 = new TCanvas("cZ2", "cZ2", 600, 600);
    TPad* pZ2 = (TPad*) plotCMSRatio(
        hZEta, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "y_{Z}", -4, 4,
        "(1/N_{Z}) dN_{Z}/d y_{Z}", 0, 0.4,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, false
    );

    cZ2->Update();
    cZ2->SaveAs(Form("%s-eta.pdf", output.c_str()));

    TCanvas* cZ3 = new TCanvas("cZ3", "cZ3", 600, 600);
    TPad* pZ3 = (TPad*) plotCMSRatio(
        hZPhi, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#phi_{Z}", 0, 2*M_PI,
        "(1/N_{Z}) dN_{Z}/d #phi_{Z}", 0, 0.3,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, false
    );

    cZ3->Update();
    cZ3->SaveAs(Form("%s-phi.pdf", output.c_str()));
    

    return 0;
}
