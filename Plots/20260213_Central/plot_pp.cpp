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
    string tag = CL.Get("pptag", "V16_nmix5");

    cout<<"=================================================="<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Track Pt Range: "<<trkPtRange<<endl;
    cout<<"pp Tag: "<<tag<<endl;

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        //Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_Gen_nominal_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_nominal_%s_ZPT%s", tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "DATA pp 5TeV [corrected]",
        "DATA pp 5TeV [uncorrected]",
        //"  & Z correction",
        //"  & Z + track correction",
        "GEN pp 5TeV",
        "RECO pp 5TeV [corrected]",
        "RECO pp 5TeV [uncorrected]"        
    };
    string output = Form("plots/pp/%s_ZPT%s_trkPT%s", tag.c_str(), zPtRange.c_str(), trkPtRange.c_str());

    // read results file
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaPhi;
    vector<TH1*> hZPt;

    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        TFile* fin = TFile::Open(Form("%s-result.root", input_ZPT.c_str()), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH1D* this_hDeltaEta = (TH1D*)fin->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
        this_hDeltaEta->SetName(Form("DeltaEta_%d", i));
        TH1D* this_hDeltaPhi = (TH1D*)fin->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
        this_hDeltaPhi->SetName(Form("DeltaPhi_%d", i));

        this_hDeltaEta->Scale(1./2);
        this_hDeltaPhi->Scale(1./2);

        cout<<"Integral of DeltaEta: "<<this_hDeltaEta->Integral()<<endl;
        cout<<"Integral of DeltaPhi: "<<this_hDeltaPhi->Integral()<<endl;

        hDeltaEta.push_back(this_hDeltaEta);
        hDeltaPhi.push_back(this_hDeltaPhi);

        TH1D* hNZ = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        cout<<"hNZ bin content: "<<hNZ->Integral()<<endl;

        i++;
        
    }

    vector<int> markerColors = {cmsBlue, kSpring-6, kMagenta-3, cmsTealL1, cmsRed, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, kSpring-6, kMagenta-3, cmsTealL1, cmsRed, cmsGray, cmsRed};
    vector<int> lineStyles = {0, 2, 1, 1, 1};

    float diffMin = (trkPtRange == "4_500") ? -0.05 : -0.5;
    float diffMax = (trkPtRange == "4_500") ? 0.05 : 0.5;

    // ===========================================
    // results
    // ===========================================
    TCanvas* cResult1 = new TCanvas("cResult1", "cResult1", 600, 600);
    TPad* pResult1 = (TPad*) plotCMSDiff(
        hDeltaEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        "pp - [GEN]", diffMin, diffMax,
        3,
        false, false, true,
        0.2
    );

    AddCMSHeader(
        pResult1,
        "Internal",
        false
    );
    AddUPCHeader(pResult1, "5.02 TeV", "pp");
    cResult1->Update();
    cResult1->SaveAs(Form("%s-DeltaEta-result.pdf", output.c_str()));

    TCanvas* cResult2 = new TCanvas("cResult2", "cResult2", 600, 600);
    TPad* pResult2 = (TPad*) plotCMSDiff(
        hDeltaPhi, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, -1,
        "pp - [GEN]", diffMin, diffMax,
        3,
        false, false, true,
        0.2
    );

    AddCMSHeader(
        pResult2,
        "Internal",
        false
    );
    AddUPCHeader(pResult2, "5.02 TeV", "pp");
    cResult2->Update();
    cResult2->SaveAs(Form("%s-DeltaPhi-result.pdf", output.c_str()));


}
