#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange      = CL.Get("zPtRange",      "0_10");
    string trkPtRange    = CL.Get("trkPtRange",     "0.5_15");
    string tag           = CL.Get("tag",            "ZV9_trkV28_nmix10_vzmatch1cm");

    string mctag = (collisionType == "pp") ? "pythia" : collisionType;
    string plotsDir = Form("plots/vzmatch/%s", collisionType.c_str());
    string outputBase = Form("%s/%s_ZPT%s_trkPT%s_%s-vzmatch-nosub",
        plotsDir.c_str(), collisionType.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag.c_str());

    // two input files: Gen MC and fully corrected reco MC
    string genFile  = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s-nosub.root",
        mctag.c_str(), tag.c_str(), zPtRange.c_str());
    string recoFile = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_trkResidual_%s_ZPT%s-nosub.root",
        mctag.c_str(), tag.c_str(), zPtRange.c_str());

    cout << "Gen file:  " << genFile  << endl;
    cout << "Reco file: " << recoFile << endl;

    auto loadHist = [&](const string &fname, const string &hname, int idx) -> TH1D* {
        TFile *f = TFile::Open(fname.c_str(), "READ");
        if (!f || f->IsZombie()) {
            cerr << "Error: cannot open " << fname << endl;
            return nullptr;
        }
        TH1D *h = (TH1D*)f->Get(hname.c_str());
        if (!h) {
            cerr << "Error: histogram " << hname << " not found in " << fname << endl;
            return nullptr;
        }
        h->SetDirectory(nullptr);
        h->SetName(Form("%s_%d", h->GetName(), idx));
        h->Scale(1./2);
        f->Close();
        return h;
    };

    TH1D *hGenEta  = loadHist(genFile,  Form("DeltaEta_Result%s", trkPtRange.c_str()), 0);
    TH1D *hRecoEta = loadHist(recoFile, Form("DeltaEta_Result%s", trkPtRange.c_str()), 1);
    TH1D *hGenPhi  = loadHist(genFile,  Form("DeltaPhi_Result%s", trkPtRange.c_str()), 2);
    TH1D *hRecoPhi = loadHist(recoFile, Form("DeltaPhi_Result%s", trkPtRange.c_str()), 3);

    if (!hGenEta || !hRecoEta || !hGenPhi || !hRecoPhi) return 1;

    vector<TH1*> hEtas = {hGenEta, hRecoEta};
    vector<TH1*> hPhis = {hGenPhi, hRecoPhi};
    vector<string> labels = {"MC DY-GEN", "MC DY-RECO (fully corrected)"};

    vector<int> colors = {cmsBlue, cmsRed};
    vector<int> styles = {0, 2};
    vector<int> markers = {mCircleFill, mCircleFill};

    string energy = (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV";

    // --- DeltaEta ---
    TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
    TPad *p1 = (TPad*) plotCMSRatio(
        hEtas, "", labels,
        colors, styles,
        colors, markers,
        "#Delta y_{ch,Z}", -4, 4,
        "Signal d#DeltaN_{ch}/d#Delta y_{ch,Z}", -1, -1,
        "RECO / GEN", 0.92, 1.08,
        0,
        false, false, true,
        0.6
    );
    AddCMSHeader(p1, "Internal", false);
    AddUPCHeader(p1, energy.c_str(), collisionType);
    p1->Update();
    c1->SaveAs(Form("%s-DeltaEta.pdf", outputBase.c_str()));

    // --- DeltaPhi ---
    TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);
    TPad *p2 = (TPad*) plotCMSRatio(
        hPhis, "", labels,
        colors, styles,
        colors, markers,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Signal d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", -1, -1,
        "RECO / GEN", 0.92, 1.08,
        0,
        false, false, true,
        0.2
    );
    AddCMSHeader(p2, "Internal", false);
    AddUPCHeader(p2, energy.c_str(), collisionType);
    p2->Update();
    c2->SaveAs(Form("%s-DeltaPhi.pdf", outputBase.c_str()));

    return 0;
}
