#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TEllipse.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>

string GetInputPrefix(const string &collisionType, const string &sample)
{
    if(collisionType == "pp")
    {
        if(sample == "Data")   return "ppData";
        if(sample == "Reco")   return "ppMC_Reco";
        if(sample == "Gen")    return "ppMC_Gen";
    }
    if(collisionType == "pPb")
    {
        if(sample == "Data")   return "pPbData";
        if(sample == "Reco")   return "pPbMC_Reco";
        if(sample == "Gen")    return "pPbMC_Gen";
    }
    if(collisionType == "PbP")
    {
        if(sample == "Data")   return "PbPData";
        if(sample == "Reco")   return "PbPMC_Reco";
        if(sample == "Gen")    return "PbPMC_Gen";
    }
    return "";
}

string GetSampleLabel(const string &collisionType, const string &sample)
{
    string label = collisionType + " ";
    if(sample == "Data")
        return label + "data";
    if(sample == "Reco")
        return label + "MC Reco";
    if(sample == "Gen")
        return label + "MC Gen";
    return label + sample;
}

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("tag", "V16_nmix5");
    string sample = CL.Get("sample", "Gen");
    string inputPrefix = GetInputPrefix(collisionType, sample);

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;
    cout<<"Sample: "<<sample<<endl;

    if(inputPrefix == "")
    {
        cerr << "Unsupported collisionType/sample combination" << endl;
        return 1;
    }

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp/output/%s_%s_ZPT%s", inputPrefix.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        GetSampleLabel(collisionType, sample)
    };
    string output = Form("plots/%s_ZPT%s_%s", inputPrefix.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH2D*> hDeltaRMuTrk;

    // Loop over nosub files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        cout<<"opening file: "<<Form("%s.root", input_ZPT.c_str())<<endl;

        TFile* fin = TFile::Open(Form("%s.root", input_ZPT.c_str()), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // muon-track deltaR distribution
        TH2D* this_hDeltaRMuTrk = (TH2D*)fin->Get("hDeltaRMuTrkData");
        this_hDeltaRMuTrk->SetName(Form("hDeltaRMuTrk_%d", i));
        hDeltaRMuTrk.push_back(this_hDeltaRMuTrk);

        i++;
    }

    // make canvas

    gStyle->SetPalette(kRainbow);

    TCanvas* c2D = new TCanvas("c2D", "c2D", 600, 600);
    c2D->SetLeftMargin(0.15);
    c2D->SetBottomMargin(0.15);
    c2D->SetRightMargin(0.15);
    
    hDeltaRMuTrk[0]->SetTitle(Form("%s track-muon #Delta#eta vs. #Delta#phi", labels[0].c_str()));
    hDeltaRMuTrk[0]->GetXaxis()->SetTitle("#Delta #eta_{mu,ch}");
    hDeltaRMuTrk[0]->GetXaxis()->SetRangeUser(-0.01, 0.01);
    hDeltaRMuTrk[0]->GetYaxis()->SetTitle("#Delta#phi_{mu,ch}");
    hDeltaRMuTrk[0]->GetYaxis()->SetRangeUser(-0.01, 0.01);
    hDeltaRMuTrk[0]->GetZaxis()->SetTitle("Counts");
    hDeltaRMuTrk[0]->SetStats(0);
    
    gPad->SetLogz();
    hDeltaRMuTrk[0]->Draw("COLZ");
    
    TEllipse* circle = new TEllipse(0, 0, 0.0025, 0.0025);
    circle->SetLineColor(kBlack);
    circle->SetLineWidth(3);
    circle->SetFillStyle(0);
    circle->Draw();
    
    TEllipse* circle2 = new TEllipse(0, 0, 0.01, 0.01);
    circle2->SetLineColor(kBlack);
    circle2->SetLineWidth(3);
    circle2->SetFillStyle(0);
    circle2->Draw();

    TEllipse* circle3 = new TEllipse(0, 0, 0.004, 0.004);
    circle3->SetLineColor(kBlack);
    circle3->SetLineWidth(3);
    circle3->SetFillStyle(0);
    circle3->Draw();
    
    c2D->Update();

    c2D->SaveAs(Form("%s-Delta2D-muTrk.pdf", output.c_str()));

    return 0;
}
