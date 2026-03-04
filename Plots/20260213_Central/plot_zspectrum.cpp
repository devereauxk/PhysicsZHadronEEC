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

    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("pPbtag", "V16_nmix5");
    string tag_pp = CL.Get("pptag", "V16_nmix5");
    bool doCombine = CL.GetBool("doCombine", false);

    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Track Pt Range: "<<trkPtRange<<endl;
    cout<<"pPb Tag: "<<tag<<endl;
    cout<<"pp Tag: "<<tag_pp<<endl;

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_ZResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_%s_ZPT%s", tag_pp.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EE_%s_ZPT%s", tag_pp.c_str(), zPtRange.c_str())
    };
    string input_ZPT_file_pbp = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_ZResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str());
    vector<string> labels = {
        "pPb 8 TeV, corrected",
        "pp 5 TeV, corrected",
        "pp extrapolated 8 TeV"
    };
    string file_tag = (doCombine) ? "all" : "pppPb";
    string output = Form("plots/zspectrum/%s_ZPT%s_trkPT%s_%s", file_tag.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag_pp.c_str());

    // read results file
    vector<TH1*> hZPt;

    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        TFile* fin = TFile::Open(Form("%s-result.root", input_ZPT.c_str()), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // Z pt eta Phi
        TH3D* this_hZPtEtaPhi = (TH3D*)fin->Get(Form("hZPtEtaPhi_%s", trkPtRange.c_str()));
        TH1D* this_hZPt = this_hZPtEtaPhi->ProjectionX(Form("ZPt_%s", labels[i].c_str()));

        TH1D* hNZ = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        float nZ = hNZ->GetBinContent(1);

        //undo nZ normalization
        this_hZPt->Scale(nZ);

        if (doCombine && i == 0) {
            TFile* fin_pbp = TFile::Open(Form("%s-result.root", input_ZPT_file_pbp.c_str()), "READ");

            TH3D* this_hZPtEtaPhi_pbp = (TH3D*)fin_pbp->Get(Form("hZPtEtaPhi_%s", trkPtRange.c_str()));
            TH1D* this_hZPt_pbp = this_hZPtEtaPhi_pbp->ProjectionX(Form("ZPt_pbp_%s", labels[i].c_str()));
            TH1D* hNZ_pbp = (TH1D*)fin_pbp->Get(Form("hNZData_%s", trkPtRange.c_str()));
            float nZ_pbp = hNZ_pbp->GetBinContent(1);
            this_hZPt_pbp->Scale(nZ_pbp);

            this_hZPt->Add(this_hZPt_pbp);
            nZ += nZ_pbp;
        }

        this_hZPt->Scale(1.0 / nZ);
        divideByWidth(this_hZPt);

        hZPt.push_back(this_hZPt);

        i++;
        
    }

    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 1, 0, 1};


    // ===========================================
    // Z pt spectrum
    // ===========================================

    TCanvas* cZ1 = new TCanvas("cZ", "cZ", 600, 600);

    TPad* pZ1 = (TPad*) plotCMSRatio(
        hZPt, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{Z}", 0.5, 1000,
        "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", -1, -1,
        "pPb / pp", 0.5, 1.5,
        0,
        true, false, false
    );

    AddCMSHeader(
        pZ1,
        "Internal",
        false
    );

    AddUPCHeader(pZ1, "8 TeV", "pPb MC");

    cZ1->Update();
    cZ1->SaveAs(Form("%s.pdf", output.c_str()));
    

}
