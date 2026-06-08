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

#include <TTree.h>

#include <vector>
#include <string>
#include <cmath>

void Symmetrize1DEta(TH1D *h) {
    int n = h->GetNbinsX();
    for (int i = 0; i < n / 2; ++i) {
        int mi = n - 1 - i;
        double v = 0.5 * (h->GetBinContent(i+1) + h->GetBinContent(mi+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(i+1),2) + pow(h->GetBinError(mi+1),2));
        h->SetBinContent(i+1, v); h->SetBinError(i+1, e);
        h->SetBinContent(mi+1, v); h->SetBinError(mi+1, e);
    }
}

void Symmetrize1DPhi(TH1D *h) {
    int n = h->GetNbinsX();
    if (n != 12) return;
    for (int j = 0; j < 3; ++j) {
        int mj = 5 - j;
        double v = 0.5 * (h->GetBinContent(j+1) + h->GetBinContent(mj+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j+1),2) + pow(h->GetBinError(mj+1),2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
    for (int j = 6; j < 9; ++j) {
        int mj = 17 - j;
        double v = 0.5 * (h->GetBinContent(j+1) + h->GetBinContent(mj+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j+1),2) + pow(h->GetBinError(mj+1),2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
}

void ApplyDiagonalJackknifeErrors(TFile *file, const string &trkPtRange,
    TH1D *hEta, TH1D *hPhi)
{
    if (file == nullptr) return;
    TTree *tree = (TTree *)file->Get(Form("JackknifeProjection%s", trkPtRange.c_str()));
    if (tree == nullptr) return;

    double signalNZ = 0, mixNZ = 0;
    vector<float> *signalEta = nullptr, *mixEta = nullptr;
    vector<float> *signalPhi = nullptr, *mixPhi = nullptr;
    tree->SetBranchAddress("SignalNZ", &signalNZ);
    tree->SetBranchAddress("MixNZ", &mixNZ);
    tree->SetBranchAddress("SignalEta", &signalEta);
    tree->SetBranchAddress("MixEta", &mixEta);
    tree->SetBranchAddress("SignalPhi", &signalPhi);
    tree->SetBranchAddress("MixPhi", &mixPhi);

    Long64_t n = tree->GetEntries();
    if (n < 2) return;

    double totalSNZ = 0, totalMNZ = 0;
    int nBins = hEta->GetNbinsX();
    vector<double> fullSEta(nBins, 0), fullMEta(nBins, 0);
    vector<double> fullSPhi(nBins, 0), fullMPhi(nBins, 0);

    struct Entry { double sNZ, mNZ; vector<float> sEta, mEta, sPhi, mPhi; };
    vector<Entry> entries(n);
    for (Long64_t i = 0; i < n; i++) {
        tree->GetEntry(i);
        entries[i] = {signalNZ, mixNZ, *signalEta, *mixEta, *signalPhi, *mixPhi};
        totalSNZ += signalNZ;
        totalMNZ += mixNZ;
        for (int b = 0; b < nBins; b++) {
            fullSEta[b] += (*signalEta)[b]; fullMEta[b] += (*mixEta)[b];
            fullSPhi[b] += (*signalPhi)[b]; fullMPhi[b] += (*mixPhi)[b];
        }
    }

    int validN = 0;
    for (auto &e : entries)
        if (totalSNZ - e.sNZ > 0 && totalMNZ - e.mNZ > 0) validN++;
    if (validN < 2) return;

    for (int b = 0; b < nBins; b++) {
        double fullEta = fullSEta[b] / totalSNZ - fullMEta[b] / totalMNZ;
        double fullPhi = fullSPhi[b] / totalSNZ - fullMPhi[b] / totalMNZ;
        double varEta = 0, varPhi = 0;
        for (auto &e : entries) {
            if (totalSNZ - e.sNZ <= 0 || totalMNZ - e.mNZ <= 0) continue;
            double dEta = (fullSEta[b] - e.sEta[b]) / (totalSNZ - e.sNZ)
                        - (fullMEta[b] - e.mEta[b]) / (totalMNZ - e.mNZ) - fullEta;
            double dPhi = (fullSPhi[b] - e.sPhi[b]) / (totalSNZ - e.sNZ)
                        - (fullMPhi[b] - e.mPhi[b]) / (totalMNZ - e.mNZ) - fullPhi;
            varEta += dEta * dEta;
            varPhi += dPhi * dPhi;
        }
        hEta->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varEta));
        hPhi->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varPhi));
    }
}

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("pPbtag", "V16_nmix5");
    string mcTag = CL.Get("pPbMCTag", tag);
    string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");

    cout<<"=================================================="<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Track Pt Range: "<<trkPtRange<<endl;
    cout<<"pPb Tag: "<<tag<<endl;

    // files to load
    vector<string> input_ZPT_files = {
        //Form("%s/pPb_nominal_%s_ZPT%s", baseDir.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("%s/pPb_trkResidual_%s_ZPT%s", baseDir.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("%s/pPbMC_Gen_nominal_%s_ZPT%s", baseDir.c_str(), mcTag.c_str(), zPtRange.c_str()),
        //Form("%s/PbP_nominal_%s_ZPT%s", baseDir.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("%s/PbP_trkResidual_%s_ZPT%s", baseDir.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("%s/PbPMC_Gen_nominal_%s_ZPT%s", baseDir.c_str(), mcTag.c_str(), zPtRange.c_str())
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

        if (i % 2 == 0)
            ApplyDiagonalJackknifeErrors(fin, trkPtRange, this_hDeltaEta, this_hDeltaPhi);

        this_hDeltaEta->Scale(1./2);
        this_hDeltaPhi->Scale(1./2);

        if (this_hDeltaEta->GetNbinsX() == 12)
            Symmetrize1DEta(this_hDeltaEta);
        if (this_hDeltaPhi->GetNbinsX() == 12)
            Symmetrize1DPhi(this_hDeltaPhi);

        hDeltaEta_combined.push_back(this_hDeltaEta);
        hDeltaPhi_combined.push_back(this_hDeltaPhi);
        if (i < (int)labels.size())      usedLabels.push_back(labels[i]);
        if (i < (int)markerColors.size()) usedMarkerColors.push_back(markerColors[i]);
        if (i < (int)markerStyles.size()) usedMarkerStyles.push_back(markerStyles[i]);
        if (i < (int)lineColors.size())   usedLineColors.push_back(lineColors[i]);
        if (i < (int)lineStyles.size())   usedLineStyles.push_back(lineStyles[i]);

        i++;
    }

    // ===========================================
    // Auto-compute difference ranges from data
    // ===========================================
    auto AutoDiffRange = [](const vector<TH1*> &h, float padding = 1.8) -> pair<float,float> {
        if (h.size() < 2) return {-0.1f, 0.1f};
        float mx = 0;
        for (int b = 1; b <= h[0]->GetNbinsX(); b++) {
            float d = fabs(h[0]->GetBinContent(b) - h[1]->GetBinContent(b));
            float e = sqrt(pow(h[0]->GetBinError(b),2) + pow(h[1]->GetBinError(b),2));
            mx = max(mx, d + e);
        }
        if (mx < 1e-6) mx = 0.01;
        return {-mx * padding, mx * padding};
    };

    auto [diffMinEta, diffMaxEta] = AutoDiffRange(hDeltaEta_combined);
    auto [diffMinPhi, diffMaxPhi] = AutoDiffRange(hDeltaPhi_combined);

    // ===========================================
    // results
    // ===========================================
    bool is12bin = (!hDeltaEta_combined.empty() && hDeltaEta_combined[0]->GetNbinsX() == 12);
    double etaXMin = is12bin ? -3.87 : -4;
    double etaXMax = is12bin ? 3.87 : 4;

    TCanvas* cResult1 = new TCanvas("cResult1", "cResult1", 600, 600);
    TPad* pResult1 = (TPad*) plotCMSDiff(
        hDeltaEta_combined, "", usedLabels,
        usedLineColors, usedLineStyles,
        usedMarkerColors, usedMarkerStyles,
        "#Delta y_{ch,Z}", etaXMin, etaXMax,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        "Difference wrt PPb DATA", diffMinEta, diffMaxEta,
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
        "Difference wrt PPb DATA", diffMinPhi, diffMaxPhi,
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
