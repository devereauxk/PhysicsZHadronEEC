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
    string tag = CL.Get("tag", "V16_nmix5");

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Tag: "<<tag<<endl;

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp/plots/%sMC_Gen_%s_ZPT0_10", collisionType.c_str(), tag.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp/plots/%sMC_Gen_%s_ZPT10_20", collisionType.c_str(), tag.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp/plots/%sMC_Gen_%s_ZPT20_40", collisionType.c_str(), tag.c_str())
    };
    vector<string> labels = {
        "MC DY-GEN, Z pT 0-10",
        "MC DY-GEN, Z pT 10-20",
        "MC DY-GEN, Z pT 20-40"
    };
    string output = Form("plots/%s_ZPTs_%s", collisionType.c_str(), tag.c_str());

    vector<TH2*> hDeltaRZGenReco;
    vector<TH1*> hDeltaPhiZGenReco;
    vector<TH1*> hDeltaEtaZGenReco;

    // Loop over nosub files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        cout<<"opening file: "<<Form("%s.root", input_ZPT.c_str())<<endl;

        TFile* fin = TFile::Open(Form("%s.root", input_ZPT.c_str()), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // Z reco-Z gen deltaR distribution
        TH2D* this_hDeltaRZGenReco = (TH2D*)fin->Get("hDeltaRZGenRecoData");
        this_hDeltaRZGenReco->SetName(Form("hDeltaRZGenReco_%d", i));
        hDeltaRZGenReco.push_back(this_hDeltaRZGenReco);

        TH1D* this_hDeltaPhiZGenReco = (TH1D*)this_hDeltaRZGenReco->ProjectionY(Form("hDeltaPhiZGenReco_%d", i), 0, 100);
        this_hDeltaPhiZGenReco->SetName(Form("hDeltaPhiZGenReco_%d", i));
        this_hDeltaPhiZGenReco->Scale(1.0 / this_hDeltaPhiZGenReco->Integral());
        hDeltaPhiZGenReco.push_back(this_hDeltaPhiZGenReco);

        TH1D* this_hDeltaEtaZGenReco = (TH1D*)this_hDeltaRZGenReco->ProjectionX(Form("hDeltaEtaZGenReco_%d", i), 0, 100);
        this_hDeltaEtaZGenReco->SetName(Form("hDeltaEtaZGenReco_%d", i));
        this_hDeltaEtaZGenReco->Scale(1.0 / this_hDeltaEtaZGenReco->Integral());
        hDeltaEtaZGenReco.push_back(this_hDeltaEtaZGenReco);

        i++;
    }

    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 0, 0, 1, 1};

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
    TPad* p1 = (TPad*) plotCMSRatio(
        hDeltaEtaZGenReco, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{Z,Z}", -0.5, 0.5,
        "counts [unit normalization]", 1e-6, 1,
        "Ratio to 20-40", 0, 2,
        2,
        false, true, false
    );

    AddCMSHeader(
        p1,
        "Internal",
        false
    );

    AddUPCHeader(p1, (collisionType == "pythia") ? "5 TeV" : "8 TeV", collisionType);
    p1->Update();
    c1->SaveAs(Form("%s-DeltaEta-ZGenReco.pdf", output.c_str()));

    TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
    TPad* p2 = (TPad*) plotCMSRatio(
        hDeltaPhiZGenReco, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta #phi_{Z,Z}", -1, 1,
        "counts [unit normalization]", 1e-6, 1,
        "Ratio to 20-40", 0, 2,
        2,
        false, true, false
    );

    AddCMSHeader(
        p2,
        "Internal",
        false
    );

    AddUPCHeader(p2, (collisionType == "pythia") ? "5 TeV" : "8 TeV", collisionType);
    p2->Update();
    c2->SaveAs(Form("%s-DeltaPhi-ZGenReco.pdf", output.c_str()));

    return 0;
}
