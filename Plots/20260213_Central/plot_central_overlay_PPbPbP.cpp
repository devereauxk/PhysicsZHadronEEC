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
    string mcTag = CL.Get("pPbMCTag", tag);

    cout<<"=================================================="<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Track Pt Range: "<<trkPtRange<<endl;
    cout<<"pPb Tag: "<<tag<<endl;

    // files to load
    vector<string> input_ZPT_files = {
        //Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_%s_ZPT%s", mcTag.c_str(), zPtRange.c_str()),
        //Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_%s_ZPT%s", mcTag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        //"PPb DATA, uncorrected",
        "PPb DATA, corrected",
        "PPb GEN-DY+EPOS",
        //"PbP DATA, uncorrected",
        "PbP DATA, corrected",
        "PbP GEN-DY+EPOS"
    };
    string output = Form("plots/central_overlay_PPbPbP/%s_ZPT%s_trkPT%s", tag.c_str(), zPtRange.c_str(), trkPtRange.c_str());

    // full per-file style tables (indexed by original file order)
    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kMagenta-3, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors   = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kMagenta-3, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles   = {0, 2, 0, 2};

    // plotted histograms — used* vectors stay in sync with the histogram vectors
    vector<TH1*> hDeltaEta_combined;
    vector<TH1*> hDeltaPhi_combined;
    vector<string> usedLabels;
    vector<int> usedMarkerColors;
    vector<int> usedMarkerStyles;
    vector<int> usedLineColors;
    vector<int> usedLineStyles;


    // ============================
    // pPb and PbP
    // ============================

    // read results file ppb and pbp files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        TFile* fin = TFile::Open(Form("%s-result.root", input_ZPT.c_str()), "READ");

        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            i++;
            continue;
        }

        // read results
        TH1D* this_hDeltaEta = (TH1D*)fin->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
        TH1D* this_hDeltaPhi = (TH1D*)fin->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));

        if (!this_hDeltaEta || !this_hDeltaPhi) {
            std::cerr << "Warning: histogram DeltaEta/Phi_Result" << trkPtRange
                      << " not found in " << input_ZPT << ", skipping" << std::endl;
            i++;
            continue;
        }

        this_hDeltaEta->SetName(Form("DeltaEta_%d", i));
        this_hDeltaPhi->SetName(Form("DeltaPhi_%d", i));

        this_hDeltaEta->Scale(1./2);
        this_hDeltaPhi->Scale(1./2);

        hDeltaEta_combined.push_back(this_hDeltaEta);
        hDeltaPhi_combined.push_back(this_hDeltaPhi);
        if (i < (int)labels.size())      usedLabels.push_back(labels[i]);
        if (i < (int)markerColors.size()) usedMarkerColors.push_back(markerColors[i]);
        if (i < (int)markerStyles.size()) usedMarkerStyles.push_back(markerStyles[i]);
        if (i < (int)lineColors.size())   usedLineColors.push_back(lineColors[i]);
        if (i < (int)lineStyles.size())   usedLineStyles.push_back(lineStyles[i]);

        i++;
    }

    float diffMin = (trkPtRange == "4_500") ? -0.1 : -0.35;
    float diffMax = (trkPtRange == "4_500") ? 0.1 : 0.35;

    // ===========================================
    // results
    // ===========================================
    TCanvas* cResult1 = new TCanvas("cResult1", "cResult1", 600, 600);
    TPad* pResult1 = (TPad*) plotCMSDiff(
        hDeltaEta_combined, "", usedLabels,
        usedLineColors, usedLineStyles,
        usedMarkerColors, usedMarkerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        "Difference wrt PPb DATA", diffMin, diffMax,
        0,
        false, false, true,
        0.2
    );

    AddCMSHeader(
        pResult1,
        "Internal",
        false
    );
    AddUPCHeader(pResult1, "8.16 TeV", "PPb");
    cResult1->Update();
    cResult1->SaveAs(Form("%s-DeltaEta-overlay.pdf", output.c_str()));

    TCanvas* cResult2 = new TCanvas("cResult2", "cResult2", 600, 600);
    TPad* pResult2 = (TPad*) plotCMSDiff(
        hDeltaPhi_combined, "", usedLabels,
        usedLineColors, usedLineStyles,
        usedMarkerColors, usedMarkerStyles,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, -1,
        "Difference wrt PPb DATA", diffMin, diffMax,
        0,
        false, false, true,
        0.2
    );

    AddCMSHeader(
        pResult2,
        "Internal",
        false
    );
    AddUPCHeader(pResult2, "8.16 TeV", "PPb");
    cResult2->Update();
    cResult2->SaveAs(Form("%s-DeltaPhi-overlay.pdf", output.c_str()));


}
