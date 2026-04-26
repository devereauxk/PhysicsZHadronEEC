#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TSystem.h>
#include <algorithm>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>

namespace
{
   bool FileExists(const string &path)
   {
      return gSystem->AccessPathName(path.c_str()) == false;
   }

   string PickExistingFile(const vector<string> &candidates, const string &label)
   {
      for(const string &candidate : candidates)
         if(FileExists(candidate))
            return candidate;

      cerr << "Error: unable to resolve " << label << " input from candidates:" << endl;
      for(const string &candidate : candidates)
         cerr << "  " << candidate << endl;
      return "";
   }
}

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
    string mcTagName = tag;
    if(collisionType == "pp")
    {
        const string interiorFragment = "_EEV3_";
        const string prefixFragment = "EEV3_";
        const string replacementFragment = "vz20260320_";
        size_t position = mcTagName.find(interiorFragment);
        if(position != string::npos)
            mcTagName.replace(position, interiorFragment.size(), "_" + replacementFragment);
        else if(mcTagName.rfind(prefixFragment, 0) == 0)
            mcTagName.replace(0, prefixFragment.size(), replacementFragment);
    }

    vector<string> dataCandidates = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%s_nominal_%s_ZPT%s-nosub.root", collisionType.c_str(), tag.c_str(), zPtRange.c_str())
    };
    if(collisionType != "pp")
        dataCandidates.push_back(Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%s_nominal_%s_ZPT5_500-nosub.root", collisionType.c_str(), tag.c_str()));
    string dataFile = PickExistingFile(dataCandidates, Form("%s data", collisionType.c_str()));

    string mcGenFile = "";
    string mcRecoFile = "";
    string mcClosureGenFile = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_Zclosure_ZPT%s-nosub.root", mctag.c_str(), zPtRange.c_str());
    string mcClosureRecoFile = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_Zclosure_ZPT%s-nosub.root", mctag.c_str(), zPtRange.c_str());
    string mcTaggedGenFile = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s-nosub.root", mctag.c_str(), mcTagName.c_str(), zPtRange.c_str());
    string mcTaggedRecoFile = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_%s_ZPT%s-nosub.root", mctag.c_str(), mcTagName.c_str(), zPtRange.c_str());

    if(FileExists(mcTaggedGenFile) && FileExists(mcTaggedRecoFile)) {
        mcGenFile = mcTaggedGenFile;
        mcRecoFile = mcTaggedRecoFile;
    }
    else if(FileExists(mcClosureGenFile) && FileExists(mcClosureRecoFile)) {
        mcGenFile = mcClosureGenFile;
        mcRecoFile = mcClosureRecoFile;
    }
    else {
        mcGenFile = PickExistingFile({
            mcTaggedGenFile,
            Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT0_500-nosub.root", mctag.c_str(), mcTagName.c_str()),
            Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT5_500-nosub.root", mctag.c_str(), mcTagName.c_str()),
            mcClosureGenFile
        }, Form("%s MC Gen", collisionType.c_str()));
        mcRecoFile = PickExistingFile({
            mcTaggedRecoFile,
            Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_%s_ZPT0_500-nosub.root", mctag.c_str(), mcTagName.c_str()),
            Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_%s_ZPT5_500-nosub.root", mctag.c_str(), mcTagName.c_str()),
            mcClosureRecoFile
        }, Form("%s MC Reco", collisionType.c_str()));
    }

    if(dataFile == "" || mcGenFile == "" || mcRecoFile == "")
        return 1;

    // files to load
    vector<string> input_ZPT_files = {
        dataFile,
        mcGenFile,
        mcRecoFile
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

        // track pt eta phi — try requested range first, fall back to 0.5_500
        TH1D* this_hZmass = (TH1D*)fin->Get(Form("hZmassData_%s", trkPtRange.c_str()));
        if(this_hZmass == nullptr && trkPtRange != "0.5_500")
            this_hZmass = (TH1D*)fin->Get("hZmassData_0.5_500");
        if(this_hZmass == nullptr) {
            cerr << "Error: histogram hZmassData_" << trkPtRange << " missing in " << input_ZPT << endl;
            return 1;
        }
        this_hZmass->SetName(Form("Zmass_%d", i));
        this_hZmass->SetDirectory(0);
        fin->Close();

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

    if(hZmass.size() != 3) {
        cerr << "Error: expected 3 z-mass histograms, found " << hZmass.size() << endl;
        return 1;
    }

    vector<string> labels = {
        Form("%s DATA", collisionType.c_str()),
        Form("MC Gen (x %.4f)", genscale),
        Form("MC Reco (x %.4f)", recoscale)
    };

    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6};
    vector<int> markerStyles = {mCircleFill, mSquareFill, mDiamondFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6};
    vector<int> lineStyles = {-1, 2, 1};

    double max_peak = 0;
    for(TH1* hist : hZmass)
        max_peak = max(max_peak, hist->GetMaximum());

    // ===========================================
    // Z mass peak
    // ===========================================
    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 700, 700);

    TPad* pTrk1 = plotCMSRatio(
        hZmass, "pad_zmass", labels,
        lineColors, lineStyles,
        markerColors, markerStyles,
        "M_{#mu#mu} (GeV/c^{2})", 60, 120,
        "Entries / (2 GeV/c^{2})", 0, max_peak * 1.60,
        "MC / data", 0.5, 1.5,
        0,
        false, false, true, 0.50
    );

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    c1->Update();

    c1->SaveAs(Form("%s.pdf", output.c_str()));
    

}
