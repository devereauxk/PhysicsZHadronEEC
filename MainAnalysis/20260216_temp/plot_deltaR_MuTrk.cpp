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

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp/plots/%sMC_Gen_%s_ZPT%s", collisionType.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "MC DY-GEN"
    };
    string output = Form("plots/%s_ZPT%s_%s-closure", collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hDeltaRMuTrk;

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
        TH1D* this_hDeltaRMuTrk = (TH1D*)fin->Get("hDeltaRMuTrkData");
        this_hDeltaRMuTrk->SetName(Form("hDeltaRMuTrk_%d", i));
        hDeltaRMuTrk.push_back(this_hDeltaRMuTrk);

        i++;
    }

    // make canvas

    TCanvas* c2D_muTrk = new TCanvas("c2D_muTrk", "c2D_muTrk", 600, 600);
    TPad* p2D_muTrk = (TPad*) plotCMSSimple(
        c2D_muTrk, hDeltaRMuTrk, "GEN muon vs track #Delta R", {"#Delta R_{mu,trk}"},
        {cmsBlue},
        {0},
        {cmsBlue},
        {0},
        "#Delta R_{mu,trk}", 0, 0.005,
        "counts", -1, -1,
        false, true
    );

    c2D_muTrk->SaveAs(Form("%s-Delta2D-muTrk.pdf", output.c_str()));

    return 0;
}
