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

static string ReplaceAll(string Value, const string &Needle, const string &Replacement)
{
    size_t Position = 0;
    while((Position = Value.find(Needle, Position)) != string::npos)
    {
        Value.replace(Position, Needle.size(), Replacement);
        Position += Replacement.size();
    }
    return Value;
}

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("tag", "V16_nmix5");
    string inputTag = CL.Get("inputTag", "");
    string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    string closureInputBaseDir = CL.Get("ClosureInputBaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow/output/closure_inputs");
    bool UseWorkflowInputs = (inputTag.empty() == false);

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;

    vector<string> input_ZPT_files;
    if(inputTag.empty() == false)
    {
        string zPtRangeDash = ReplaceAll(zPtRange, "_", "-");
        input_ZPT_files = {
            Form("%s/%s_%s_zPt%s_gen.root", closureInputBaseDir.c_str(), collisionType.c_str(), inputTag.c_str(), zPtRangeDash.c_str()),
            Form("%s/%s_%s_zPt%s_reco.root", closureInputBaseDir.c_str(), collisionType.c_str(), inputTag.c_str(), zPtRangeDash.c_str()),
            Form("%s/%s_%s_zPt%s_corrected.root", closureInputBaseDir.c_str(), collisionType.c_str(), inputTag.c_str(), zPtRangeDash.c_str())
        };
    }
    else
    {
        string mctag = (collisionType == "pp") ? "pythia" : collisionType;
        input_ZPT_files = {
            Form("%s/%sMC_Gen_nominal_%s_ZPT%s-nosub.root", baseDir.c_str(), mctag.c_str(), tag.c_str(), zPtRange.c_str()),
            Form("%s/%sMC_nominal_%s_ZPT%s-nosub.root", baseDir.c_str(), mctag.c_str(), tag.c_str(), zPtRange.c_str()),
            Form("%s/%sMC_ZResidual_%s_ZPT%s-nosub.root", baseDir.c_str(), mctag.c_str(), tag.c_str(), zPtRange.c_str())
        };
    }
    vector<string> labels = {
        "MC DY-GEN",
        "MC DY-RECO",
        "MC DY-RECO (corrected)"
    };
    string output = Form("plots/%s/ZPT%s_%s-closure", collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hZPt;
    vector<TH1*> hZEta;

    // Loop over nosub files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        cout<<"opening file: "<<input_ZPT<<endl;

        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH1D* this_hZPt  = nullptr;
        TH1D* this_hZEta = nullptr;
        TH1D* hNZ = nullptr;

        if(UseWorkflowInputs == true)
        {
            // Workflow closure inputs use h2D (pT × eta, 2D Z correction)
            TH2D* this_h2D = (TH2D*)fin->Get("h2D");
            hNZ = (TH1D*)fin->Get("hNZ");
            if(this_h2D == nullptr || hNZ == nullptr)
            {
                cerr << "Missing closure histograms in " << input_ZPT << endl;
                continue;
            }
            this_hZPt  = this_h2D->ProjectionX(Form("ZPt_%s",  labels[i].c_str()));
            this_hZEta = this_h2D->ProjectionY(Form("ZEta_%s", labels[i].c_str()));
            cout<<" "<<this_h2D->Integral()<<endl;
        }
        else
        {
            // Non-workflow inputs from main analysis nosub files (still TH3D hZPtEtaPhi)
            TH3D* this_hZPtEtaPhi = (TH3D*)fin->Get(Form("hZPtEtaPhi_%s", trkPtRange.c_str()));
            hNZ = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
            if(this_hZPtEtaPhi == nullptr || hNZ == nullptr)
            {
                cerr << "Missing closure histograms in " << input_ZPT << endl;
                continue;
            }
            this_hZPt  = this_hZPtEtaPhi->ProjectionX(Form("ZPt_%s",  labels[i].c_str()));
            this_hZEta = this_hZPtEtaPhi->ProjectionY(Form("ZEta_%s", labels[i].c_str()));
            cout<<" "<<this_hZPtEtaPhi->Integral()<<endl;
        }

        cout<<"hNZ bin content: "<<hNZ->Integral()<<endl;

        if(UseWorkflowInputs == true)
        {
            double NZ = hNZ->GetBinContent(1);
            if(NZ > 0)
            {
                this_hZPt->Scale(1.0 / NZ);
                this_hZEta->Scale(1.0 / NZ);
            }
        }

        divideByWidth(this_hZPt);
        divideByWidth(this_hZEta);

        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);

        i++;
    }

    // make canvas
    TCanvas* cZ1 = new TCanvas("cZ", "cZ", 600, 600);

    TPad* pZ1 = (TPad*) plotCMSRatio(
        hZPt, "", labels,
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray}, {0, 2, 1, 1, 1},
        {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "p_{T}^{Z}", 0.5, 500,
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

    return 0;
}
