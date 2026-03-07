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
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_Zclosure_ZPT%s-nosub.root", mctag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_Zclosure_ZPT%s-nosub.root", mctag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%s_nominal_%s_ZPT%s-nosub.root", collisionType.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%s_trkResidual_%s_ZPT%s-nosub.root", collisionType.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        Form("MC Gen"),
        Form("MC Reco"),
        Form("%s DATA", collisionType.c_str()),
        Form("  + correction", collisionType.c_str()),
    };
    
    string output =  Form("plots/dataMCComparison/%s_ZPT%s-%s", collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hZMass;
    vector<TH1*> hZPt;
    vector<TH1*> hZEta;

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        cout<<"Opening file: "<<input_ZPT<<endl;

        // Z level
        TH1D* this_hZMass = (TH1D*)fin->Get(Form("hZmassData_%s", trkPtRange.c_str()));
        this_hZMass->SetName(Form("Zmass_%d", i));
        hZMass.push_back(this_hZMass);

        TH3D* this_hZPtEtaPhi = (TH3D*)fin->Get(Form("hZPtEtaPhi_%s", trkPtRange.c_str()));
        TH1D* this_hZPt = this_hZPtEtaPhi->ProjectionX(Form("ZPt_%d", i));
        TH1D* this_hZEta = this_hZPtEtaPhi->ProjectionY(Form("ZEta_%d", i));
        TH1D* this_hZPhi = this_hZPtEtaPhi->ProjectionZ(Form("ZPhi_%d", i));

        TH1D* hNZ = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        cout<<" "<<this_hZPtEtaPhi->Integral()<<endl;
        cout<<"hNZ bin content: "<<hNZ->Integral()<<endl;

        divideByWidth(this_hZPt);
        divideByWidth(this_hZEta);
        divideByWidth(this_hZPhi);

        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);


        // track level
        TH3D* this_hTrkPtEtaPhi = (TH3D*)fin->Get(Form("hTrkPtEtaPhiData_%s", trkPtRange.c_str()));
        TH1D* this_hTrkPt = this_hTrkPtEtaPhi->ProjectionX(Form("trkPt_%d", i));
        TH1D* this_hTrkEta = this_hTrkPtEtaPhi->ProjectionY(Form("trkEta_%d", i));
        TH1D* this_hTrkPhi = this_hTrkPtEtaPhi->ProjectionZ(Form("trkPhi_%d", i));

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hTrkPhi);

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);

        i++;
    }

    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 1, 0, 1};


    // ===========================================
    // Z level comparisons
    // ===========================================
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
    TPad* pZ0 = (TPad*) plotCMSSimple(
        c1,
        hZMass, "", labels,
        lineColors, lineStyles,
        markerColors, markerStyles,
        "M_{#mu} (GeV/c^{2})", 60, 120,
        "Entries / (2 GeV/c^{2})", -1, -1,
        false, false, false
    );

    AddUPCHeader(pZ0, (collisionType == "pp") ? "5 TeV" : "8 TeV", collisionType);
    c1->Update();
    c1->SaveAs(Form("%s-Zmass.pdf", output.c_str()));


    TCanvas* cZ1 = new TCanvas("cZ", "cZ", 600, 600);
    TPad* pZ1 = (TPad*) plotCMSRatio(
        hZPt, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{Z}", 0, 50,
        "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", -1, -1,
        "Ratio to GEN", 0.5, 1.5,
        0,
        true, false, false
    );

    AddUPCHeader(pZ1, (collisionType == "pp") ? "5 TeV" : "8 TeV", collisionType);
    cZ1->Update();
    cZ1->SaveAs(Form("%s-Zpt.pdf", output.c_str()));

    TCanvas* cZ2 = new TCanvas("cZ2", "cZ2", 600, 600);
    TPad* pZ2 = (TPad*) plotCMSRatio(
        hZEta, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "y_{Z}", -4, 4,
        "(1/N_{Z}) dN_{Z}/d y_{Z}", 0, 0.5,
        "Ratio to GEN", 0.5, 1.5,
        0,
        false, false, false
    ); 

    AddUPCHeader(pZ2, (collisionType == "pp") ? "5 TeV" : "8 TeV", collisionType);
    cZ2->Update();
    cZ2->SaveAs(Form("%s-Zeta.pdf", output.c_str()));


    // ===========================================
    // Track level comparisons
    // ===========================================
    TCanvas* cTrk1 = new TCanvas("cTrk", "cTrk", 600, 600);
    TPad* pTrk1 = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "p_{T}^{ch}", 0, 10,
        "(1/N_{Z}) dN_{ch}/dp_{T}^{ch}", -27, 160,
        "Ratio to GEN", 0.5, 1.5,
        0,
        true, false, false
    );

    AddUPCHeader(pTrk1, (collisionType == "pp") ? "5 TeV" : "8 TeV", collisionType);
    cTrk1->Update();
    cTrk1->SaveAs(Form("%s-pt.pdf", output.c_str()));


    TCanvas* cTrk2 = new TCanvas("cTrk2", "cTrk2", 600, 600);
    TPad* pTrk2 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "y_{ch}", -4, 4,
        "(1/N_{Z}) dN_{ch}/d y_{ch}", (collisionType == "pp" ? 2 : 8), (collisionType == "pp" ? 8 : 27),
        "Ratio to GEN", 0.5, 1.5,
        0,
        false, false, false
    );

    AddUPCHeader(pTrk2, (collisionType == "pp") ? "5 TeV" : "8 TeV", collisionType);
    cTrk2->Update();
    cTrk2->SaveAs(Form("%s-eta.pdf", output.c_str()));
    

}
