#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <TFile.h>
#include <TH1D.h>

#include "../../CommonCode/include/KylesPlotting.h"

#include "CommandLine.h"
#include "SetStyle.h"

void Symmetrize1DEta(TH1D *h)
{
    if(h == nullptr) return;
    int n = h->GetNbinsX();
    for(int i = 0; i < n / 2; ++i)
    {
        int mi = n - 1 - i;
        double v = 0.5 * (h->GetBinContent(i + 1) + h->GetBinContent(mi + 1));
        double e = 0.5 * sqrt(pow(h->GetBinError(i + 1), 2) + pow(h->GetBinError(mi + 1), 2));
        h->SetBinContent(i + 1, v); h->SetBinError(i + 1, e);
        h->SetBinContent(mi + 1, v); h->SetBinError(mi + 1, e);
    }
}

void Symmetrize1DPhi(TH1D *h)
{
    if(h == nullptr) return;
    int n = h->GetNbinsX();
    if(n != 12) return;
    for(int j = 0; j < 3; ++j)
    {
        int mj = 5 - j;
        double v = 0.5 * (h->GetBinContent(j + 1) + h->GetBinContent(mj + 1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j + 1), 2) + pow(h->GetBinError(mj + 1), 2));
        h->SetBinContent(j + 1, v); h->SetBinError(j + 1, e);
        h->SetBinContent(mj + 1, v); h->SetBinError(mj + 1, e);
    }
    for(int j = 6; j < 9; ++j)
    {
        int mj = 17 - j;
        double v = 0.5 * (h->GetBinContent(j + 1) + h->GetBinContent(mj + 1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j + 1), 2) + pow(h->GetBinError(mj + 1), 2));
        h->SetBinContent(j + 1, v); h->SetBinError(j + 1, e);
        h->SetBinContent(mj + 1, v); h->SetBinError(mj + 1, e);
    }
}

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string trkPtRange = CL.Get("trkPtRange", "0.5_15");
    string tag_pp = CL.Get("pptag", "EEV6_ZV10_trkV29_nmix10");
    string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    string outputDir = CL.Get("outputDir", "plots/energyExtrapolation");

    string fileNoEE = Form("%s/pp_trkResidual_%s_noEE_ZPT0_500-result.root",
        baseDir.c_str(), tag_pp.c_str());
    string fileWithEE = Form("%s/pp_trkResidual_%s_ZPT0_500-result.root",
        baseDir.c_str(), tag_pp.c_str());

    cout << "Without EE: " << fileNoEE << endl;
    cout << "With EE:    " << fileWithEE << endl;

    TFile *fNoEE = TFile::Open(fileNoEE.c_str(), "READ");
    TFile *fWithEE = TFile::Open(fileWithEE.c_str(), "READ");
    if(!fNoEE || fNoEE->IsZombie() || !fWithEE || fWithEE->IsZombie())
    {
        cerr << "Error: cannot open input files" << endl;
        return 1;
    }

    TH1D *hEtaNoEE = (TH1D *)fNoEE->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
    TH1D *hPhiNoEE = (TH1D *)fNoEE->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
    TH1D *hEtaWithEE = (TH1D *)fWithEE->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
    TH1D *hPhiWithEE = (TH1D *)fWithEE->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));

    if(!hEtaNoEE || !hPhiNoEE || !hEtaWithEE || !hPhiWithEE)
    {
        cerr << "Error: missing histograms for trkPtRange=" << trkPtRange << endl;
        return 1;
    }

    hEtaNoEE->SetName("DeltaEta_noEE");
    hPhiNoEE->SetName("DeltaPhi_noEE");
    hEtaWithEE->SetName("DeltaEta_withEE");
    hPhiWithEE->SetName("DeltaPhi_withEE");

    hEtaNoEE->Scale(0.5);
    hPhiNoEE->Scale(0.5);
    hEtaWithEE->Scale(0.5);
    hPhiWithEE->Scale(0.5);

    Symmetrize1DEta(hEtaNoEE);
    Symmetrize1DEta(hEtaWithEE);
    Symmetrize1DPhi(hPhiNoEE);
    Symmetrize1DPhi(hPhiWithEE);

    vector<string> labels = {
        "pp (5.02 TeV)",
        "pp (8.16 TeV, extrapolated)"
    };

    vector<int> markerColors = {cmsBlue, cmsRed};
    vector<int> markerStyles = {mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed};
    vector<int> lineStyles = {0, 0};

    system(Form("mkdir -p %s", outputDir.c_str()));

    string output = Form("%s/%s_ZPT0_500_trkPT%s",
        outputDir.c_str(), tag_pp.c_str(), trkPtRange.c_str());

    // DeltaEta
    {
        vector<TH1 *> hists = {hEtaNoEE, hEtaWithEE};
        TCanvas *c = new TCanvas("cEta", "cEta", 600, 600);
        plotCMSDiff(
            hists, "", labels,
            lineColors, lineStyles,
            markerColors, markerStyles,
            "#Delta y_{ch,Z}", -3.87, 3.87,
            "d#LT#DeltaN_{ch}#GT/d#Deltay_{ch,Z}", -1, -1,
            "8TeV-5TeV", -0.025, 0.025,
            0,
            false, false, true,
            0.2
        );
        c->SaveAs(Form("%s-DeltaEta-EEcomparison.pdf", output.c_str()));
        delete c;
    }

    // DeltaPhi
    {
        vector<TH1 *> hists = {hPhiNoEE, hPhiWithEE};
        TCanvas *c = new TCanvas("cPhi", "cPhi", 600, 600);
        plotCMSDiff(
            hists, "", labels,
            lineColors, lineStyles,
            markerColors, markerStyles,
            "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
            "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, -1,
            "8TeV-5TeV", -0.075, 0.075,
            0,
            false, false, true,
            0.2
        );
        c->SaveAs(Form("%s-DeltaPhi-EEcomparison.pdf", output.c_str()));
        delete c;
    }

    return 0;
}
