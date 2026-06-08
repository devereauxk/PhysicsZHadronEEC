#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TSystem.h>
#include <TTree.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>

struct ResultProjectionWindow {
    int DeltaPhiXFirst = 0;
    int DeltaPhiXLast = 0;
    int DeltaEtaYFirst = 1;
    int DeltaEtaYLast = 1;
};

void SetOfficial20BinProjectionWindow(ResultProjectionWindow &window)
{
    window.DeltaPhiXFirst = 0;
    window.DeltaPhiXLast = 10;
    window.DeltaEtaYFirst = 6;
    window.DeltaEtaYLast = 10;
}

void SetShifted10x10ProjectionWindow(ResultProjectionWindow &window)
{
    window.DeltaPhiXFirst = 6;
    window.DeltaPhiXLast = 10;
    window.DeltaEtaYFirst = 4;
    window.DeltaEtaYLast = 8;
}

void SetModified12x12ProjectionWindow(ResultProjectionWindow &window)
{
    window.DeltaPhiXFirst = 7;
    window.DeltaPhiXLast = 12;
    window.DeltaEtaYFirst = 4;
    window.DeltaEtaYLast = 6;
}

bool AxisHasExactEdge(const TAxis *axis, double boundary)
{
    if(axis == nullptr)
        return false;

    double tolerance = std::max(1.0, std::abs(boundary)) * 1e-9;
    for(int i = 0; i <= axis->GetNbins(); i++) {
        double edge = axis->GetBinLowEdge(i + 1);
        if(std::abs(edge - boundary) < tolerance)
            return true;
    }
    return false;
}

bool ValidateShifted10x10Histogram(const TH2D *histogram)
{
    if(histogram == nullptr)
        return false;
    if(histogram->GetNbinsX() != 10 || histogram->GetNbinsY() != 10)
        return false;
    if(AxisHasExactEdge(histogram->GetXaxis(), 0.0) == false)
        return false;
    if(AxisHasExactEdge(histogram->GetXaxis(), 4.0) == false)
        return false;
    if(AxisHasExactEdge(histogram->GetYaxis(), 0.0) == false)
        return false;
    if(AxisHasExactEdge(histogram->GetYaxis(), M_PI) == false)
        return false;
    return true;
}

bool ValidateModified12x12Histogram(const TH2D *histogram)
{
    if(histogram == nullptr)
        return false;
    if(histogram->GetNbinsX() != 12 || histogram->GetNbinsY() != 12)
        return false;
    if(AxisHasExactEdge(histogram->GetXaxis(), 0.0) == false)
        return false;
    if(AxisHasExactEdge(histogram->GetXaxis(), 4.0) == false
        && AxisHasExactEdge(histogram->GetXaxis(), 3.87) == false)
        return false;
    if(AxisHasExactEdge(histogram->GetYaxis(), 0.0) == false)
        return false;
    if(AxisHasExactEdge(histogram->GetYaxis(), M_PI / 2) == false)
        return false;
    if(AxisHasExactEdge(histogram->GetYaxis(), M_PI) == false)
        return false;
    return true;
}

TH1D *LoadSystematicHistogram(const string &fileName, const string &histogramName,
    const string &cloneName)
{
    TFile input(fileName.c_str(), "READ");
    if (input.IsZombie())
        return nullptr;

    TH1D *histogram = (TH1D *)input.Get(histogramName.c_str());
    if (histogram == nullptr)
        return nullptr;

    histogram = (TH1D *)histogram->Clone(cloneName.c_str());
    histogram->SetDirectory(nullptr);
    return histogram;
}

struct JackknifeProjectionContribution {
    double SignalNZ = 0;
    double MixNZ = 0;
    vector<float> SignalEta;
    vector<float> MixEta;
    vector<float> SignalPhi;
    vector<float> MixPhi;
};

void AppendJackknifeProjectionContributions(TFile *file, const string &treeName,
    vector<JackknifeProjectionContribution> &events)
{
    if (file == nullptr)
        return;

    TTree *tree = (TTree *)file->Get(treeName.c_str());
    if (tree == nullptr)
        return;

    double signalNZ = 0;
    double mixNZ = 0;
    vector<float> *signalEta = nullptr;
    vector<float> *mixEta = nullptr;
    vector<float> *signalPhi = nullptr;
    vector<float> *mixPhi = nullptr;

    tree->SetBranchAddress("SignalNZ", &signalNZ);
    tree->SetBranchAddress("MixNZ", &mixNZ);
    tree->SetBranchAddress("SignalEta", &signalEta);
    tree->SetBranchAddress("MixEta", &mixEta);
    tree->SetBranchAddress("SignalPhi", &signalPhi);
    tree->SetBranchAddress("MixPhi", &mixPhi);

    Long64_t entryCount = tree->GetEntries();
    for (Long64_t entry = 0; entry < entryCount; ++entry) {
        tree->GetEntry(entry);
        JackknifeProjectionContribution current;
        current.SignalNZ = signalNZ;
        current.MixNZ = mixNZ;
        current.SignalEta = *signalEta;
        current.MixEta = *mixEta;
        current.SignalPhi = *signalPhi;
        current.MixPhi = *mixPhi;
        events.push_back(current);
    }
}

vector<double> ComputeProjectedJackknifeSigma(const vector<JackknifeProjectionContribution> &events,
    const TH1D *fullHistogram, bool useEta)
{
    vector<double> sigma(fullHistogram->GetNbinsX(), 0);
    if (events.size() < 2)
        return sigma;

    double totalSignalNZ = 0;
    double totalMixNZ = 0;
    vector<double> fullSignal(fullHistogram->GetNbinsX(), 0);
    vector<double> fullMix(fullHistogram->GetNbinsX(), 0);
    for (const JackknifeProjectionContribution &event : events) {
        totalSignalNZ += event.SignalNZ;
        totalMixNZ += event.MixNZ;
        for (int i = 0; i < fullHistogram->GetNbinsX(); ++i) {
            fullSignal[i] += (useEta ? event.SignalEta[i] : event.SignalPhi[i]);
            fullMix[i] += (useEta ? event.MixEta[i] : event.MixPhi[i]);
        }
    }

    int validEvents = 0;
    for (const JackknifeProjectionContribution &event : events) {
        if (totalSignalNZ - event.SignalNZ <= 0)
            continue;
        if (totalMixNZ - event.MixNZ <= 0)
            continue;
        validEvents = validEvents + 1;
    }
    if (validEvents < 2)
        return sigma;

    for (int i = 1; i <= fullHistogram->GetNbinsX(); ++i) {
        double fullValue = fullSignal[i - 1] / totalSignalNZ - fullMix[i - 1] / totalMixNZ;
        double varianceSum = 0;
        for (const JackknifeProjectionContribution &event : events) {
            if (totalSignalNZ - event.SignalNZ <= 0)
                continue;
            if (totalMixNZ - event.MixNZ <= 0)
                continue;

            double signalWithoutEvent = (fullSignal[i - 1] - (useEta ? event.SignalEta[i - 1] : event.SignalPhi[i - 1]))
                / (totalSignalNZ - event.SignalNZ);
            double mixWithoutEvent = (fullMix[i - 1] - (useEta ? event.MixEta[i - 1] : event.MixPhi[i - 1]))
                / (totalMixNZ - event.MixNZ);
            double valueWithoutEvent = signalWithoutEvent - mixWithoutEvent;
            double delta = valueWithoutEvent - fullValue;
            varianceSum += delta * delta;
        }

        sigma[i - 1] = std::sqrt((validEvents - 1.0) / validEvents * varianceSum);
    }

    return sigma;
}

void ApplyProjectedJackknifeErrors(TH1D *etaHistogram, TH1D *phiHistogram,
    const vector<JackknifeProjectionContribution> &events)
{
    if (events.size() < 2)
        return;

    vector<double> etaSigma = ComputeProjectedJackknifeSigma(events, etaHistogram, true);
    vector<double> phiSigma = ComputeProjectedJackknifeSigma(events, phiHistogram, false);

    for (int i = 1; i <= etaHistogram->GetNbinsX(); ++i)
        etaHistogram->SetBinError(i, etaSigma[i - 1]);
    for (int i = 1; i <= phiHistogram->GetNbinsX(); ++i)
        phiHistogram->SetBinError(i, phiSigma[i - 1]);
}

void ApplyCombinedDiagonalJackknifeErrors(TH1D *etaHistogram, TH1D *phiHistogram,
    const vector<JackknifeProjectionContribution> &eventsPPb,
    const vector<JackknifeProjectionContribution> &eventsPbP,
    double NZ_pPb, double NZ_PbP)
{
    double totalNZ = NZ_pPb + NZ_PbP;
    double w_pPb = NZ_pPb / totalNZ;
    double w_PbP = NZ_PbP / totalNZ;

    vector<double> etaSigmaPPb = ComputeProjectedJackknifeSigma(eventsPPb, etaHistogram, true);
    vector<double> phiSigmaPPb = ComputeProjectedJackknifeSigma(eventsPPb, phiHistogram, false);
    vector<double> etaSigmaPbP = ComputeProjectedJackknifeSigma(eventsPbP, etaHistogram, true);
    vector<double> phiSigmaPbP = ComputeProjectedJackknifeSigma(eventsPbP, phiHistogram, false);

    for (int i = 1; i <= etaHistogram->GetNbinsX(); ++i) {
        double sigma = sqrt(w_pPb * w_pPb * etaSigmaPPb[i-1] * etaSigmaPPb[i-1]
                          + w_PbP * w_PbP * etaSigmaPbP[i-1] * etaSigmaPbP[i-1]);
        etaHistogram->SetBinError(i, sigma);
    }
    for (int i = 1; i <= phiHistogram->GetNbinsX(); ++i) {
        double sigma = sqrt(w_pPb * w_pPb * phiSigmaPPb[i-1] * phiSigmaPPb[i-1]
                          + w_PbP * w_PbP * phiSigmaPbP[i-1] * phiSigmaPbP[i-1]);
        phiHistogram->SetBinError(i, sigma);
    }
}

void Symmetrize2DFourfold(TH2D *h)
{
    if(h == nullptr) return;
    int nx = h->GetNbinsX();
    int ny = h->GetNbinsY();
    if(nx != 12 || ny != 12) return;

    TH2D *copy = (TH2D *)h->Clone("_sym2d_tmp");
    copy->SetDirectory(nullptr);

    for(int i = 1; i <= nx; ++i)
    {
        int mi = 13 - i;
        for(int j = 1; j <= ny; ++j)
        {
            int mj = (j <= 6) ? (7 - j) : (19 - j);
            double v = 0.25 * (copy->GetBinContent(i, j) + copy->GetBinContent(mi, j)
                             + copy->GetBinContent(i, mj) + copy->GetBinContent(mi, mj));
            double e = 0.25 * sqrt(pow(copy->GetBinError(i, j), 2)
                             + pow(copy->GetBinError(mi, j), 2)
                             + pow(copy->GetBinError(i, mj), 2)
                             + pow(copy->GetBinError(mi, mj), 2));
            h->SetBinContent(i, j, v);
            h->SetBinError(i, j, e);
        }
    }
    delete copy;
}

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

    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("pPbtag", "V16_nmix5");
    string mcTag = CL.Get("pPbMCTag", tag);
    string systematicsTag = CL.Get("pPbSystematicsTag", tag);
    string tag_pp = CL.Get("pptag", "V16_nmix5");
    string systematicsTagPP = CL.Get("ppSystematicsTag", tag_pp);
    bool doCombine = CL.GetBool("doCombine", false);
    bool includeMC = CL.GetBool("includeMC", true);
    bool useSystematics = CL.GetBool("UseSystematics", true);
    bool useShifted10x10 = CL.GetBool("UseShifted10x10", false);
    bool useModified12x12 = CL.GetBool("UseModified12x12", false);
    if(useShifted10x10 == true && useModified12x12 == true) {
        std::cerr << "Error: UseShifted10x10 and UseModified12x12 cannot both be true." << std::endl;
        return 1;
    }
    string collisionType = CL.Get("collisionType", "pPb");
    string outputBase = CL.Get("outputBase", "plots/central_combined");
    string systematicsDir = CL.Get("systematicsDir",
        "/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output");
    string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    cout<<"=================================================="<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Track Pt Range: "<<trkPtRange<<endl;
    cout<<"pPb Tag: "<<tag<<endl;
    cout<<"pp Tag: "<<tag_pp<<endl;

    // files to load
    string input_ZPT_files_pp = Form("%s/pp_trkResidual_%s_ZPT%s", baseDir.c_str(), tag_pp.c_str(), zPtRange.c_str());
    vector<string> input_ZPT_files = {
        Form("%s/pPb_trkResidual_%s_ZPT%s", baseDir.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> input_ZPT_files_pbp = {
        Form("%s/PbP_trkResidual_%s_ZPT%s", baseDir.c_str(), tag.c_str(), zPtRange.c_str())
    };
    if (includeMC) {
        input_ZPT_files.push_back(Form("%s/pPbMC_Gen_nominal_%s_ZPT%s", baseDir.c_str(), mcTag.c_str(), zPtRange.c_str()));
        input_ZPT_files_pbp.push_back(Form("%s/PbPMC_Gen_nominal_%s_ZPT%s", baseDir.c_str(), mcTag.c_str(), zPtRange.c_str()));
    }
    vector<string> labels = {
        "pp",
        Form("%s", collisionType.c_str())
    };
    if (includeMC)
        labels.push_back("Powheg+EPOS");
    string file_tag = (doCombine) ? "all" : collisionType;
    gSystem->mkdir(Form("%s/%s", outputBase.c_str(), tag_pp.c_str()), true);
    string output = Form("%s/%s/%s_ZPT%s_trkPT%s_%s", outputBase.c_str(), tag_pp.c_str(), file_tag.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag.c_str());

    // plotted histograms
    vector<TH1*> hDeltaEta_combined;
    vector<TH1*> hDeltaPhi_combined;

    // ============================
    // pp
    // ============================
    TH1D* hDeltaEta_pp;
    TH1D* hDeltaPhi_pp;

    TFile* fin_pp = TFile::Open(Form("%s-result.root", input_ZPT_files_pp.c_str()), "READ");
    if (!fin_pp || fin_pp->IsZombie()) {
        std::cerr << "Error: Unable to open file " << input_ZPT_files_pp << std::endl;
        return 1;
    }
    hDeltaPhi_pp = (TH1D*)fin_pp->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
    hDeltaEta_pp = (TH1D*)fin_pp->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
    if (hDeltaPhi_pp == nullptr || hDeltaEta_pp == nullptr) {
        std::cerr << "Error: Missing pp result histograms in " << input_ZPT_files_pp << std::endl;
        return 1;
    }
    hDeltaPhi_pp = (TH1D*)hDeltaPhi_pp->Clone("hDeltaPhi_pp");
    hDeltaEta_pp = (TH1D*)hDeltaEta_pp->Clone("hDeltaEta_pp");
    hDeltaPhi_pp->SetDirectory(nullptr);
    hDeltaEta_pp->SetDirectory(nullptr);

    vector<JackknifeProjectionContribution> jackknifePP;
    AppendJackknifeProjectionContributions(fin_pp, Form("JackknifeProjection%s", trkPtRange.c_str()), jackknifePP);
    if (jackknifePP.size() >= 2) {
        ApplyProjectedJackknifeErrors(hDeltaEta_pp, hDeltaPhi_pp, jackknifePP);
    }

    hDeltaPhi_pp->Scale(1. / 2);
    hDeltaEta_pp->Scale(1. / 2);

    if (useModified12x12 && hDeltaEta_pp->GetNbinsX() == 12) {
        Symmetrize1DEta(hDeltaEta_pp);
        Symmetrize1DPhi(hDeltaPhi_pp);
    }

    cout<<"pp DeltaPhi integral: "<<hDeltaPhi_pp->Integral()<<endl;
    cout<<"pp DeltaEta integral: "<<hDeltaEta_pp->Integral()<<endl;

    hDeltaPhi_combined.push_back(hDeltaPhi_pp);
    hDeltaEta_combined.push_back(hDeltaEta_pp);


    // ============================
    // pPb and PbP
    // ============================

    // read results file ppb
    vector<TH2*> hData_ppb;
    vector<TH2*> hMixData_ppb;
    vector<TH1*> hNZData_ppb;
    vector<TH1*> hNZMixData_ppb;
    vector<JackknifeProjectionContribution> jackknifePPb;
    for (const auto& input_ZPT : input_ZPT_files) {

        TFile* fin = TFile::Open(Form("%s-nosub.root", input_ZPT.c_str()), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // read results
        TH2D* this_hData = (TH2D*)fin->Get(Form("hData_%s", trkPtRange.c_str()));
        this_hData->SetName(Form("hData_%s", input_ZPT.c_str()));
        TH2D* this_hMixData = (TH2D*)fin->Get(Form("hMixData_%s", trkPtRange.c_str()));
        this_hMixData->SetName(Form("hMixData_%s", input_ZPT.c_str()));
        TH1D* this_hNZData = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        this_hNZData->SetName(Form("hNZData_%s", input_ZPT.c_str()));
        TH1D* this_hNZMixData = (TH1D*)fin->Get(Form("hNZMixData_%s", trkPtRange.c_str()));
        this_hNZMixData->SetName(Form("hNZMixData_%s", input_ZPT.c_str()));

        // undo NZ normalization
        this_hData->Scale(this_hNZData->GetBinContent(1));
        this_hMixData->Scale(this_hNZMixData->GetBinContent(1));

        cout<<"pPb hNZ integral: "<<this_hNZData->Integral()<<endl;

        hData_ppb.push_back(this_hData);
        hMixData_ppb.push_back(this_hMixData);
        hNZData_ppb.push_back(this_hNZData);
        hNZMixData_ppb.push_back(this_hNZMixData);
        if (hData_ppb.size() == 1)
            AppendJackknifeProjectionContributions(fin, Form("JackknifeProjection%s", trkPtRange.c_str()), jackknifePPb);
    }

    // read results file pbp
    vector<TH2*> hData_pbp;
    vector<TH2*> hMixData_pbp;
    vector<TH1*> hNZData_pbp;
    vector<TH1*> hNZMixData_pbp;
    vector<JackknifeProjectionContribution> jackknifePbP;
    for (const auto& input_ZPT : input_ZPT_files_pbp) {

        TFile* fin = TFile::Open(Form("%s-nosub.root", input_ZPT.c_str()), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // read results
        TH2D* this_hData = (TH2D*)fin->Get(Form("hData_%s", trkPtRange.c_str()));
        this_hData->SetName(Form("hData_pbp_%s", input_ZPT.c_str()));
        TH2D* this_hMixData = (TH2D*)fin->Get(Form("hMixData_%s", trkPtRange.c_str()));
        this_hMixData->SetName(Form("hMixData_pbp_%s", input_ZPT.c_str()));
        TH1D* this_hNZData = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        this_hNZData->SetName(Form("hNZData_pbp_%s", input_ZPT.c_str()));
        TH1D* this_hNZMixData = (TH1D*)fin->Get(Form("hNZMixData_%s", trkPtRange.c_str()));
        this_hNZMixData->SetName(Form("hNZMixData_pbp_%s", input_ZPT.c_str()));

        // undo NZ normalization
        this_hData->Scale(this_hNZData->GetBinContent(1));
        this_hMixData->Scale(this_hNZMixData->GetBinContent(1));

        cout<<"PbP hNZ integral: "<<this_hNZData->Integral()<<endl;

        hData_pbp.push_back(this_hData);
        hMixData_pbp.push_back(this_hMixData);
        hNZData_pbp.push_back(this_hNZData);
        hNZMixData_pbp.push_back(this_hNZMixData);
        if (hData_pbp.size() == 1)
            AppendJackknifeProjectionContributions(fin, Form("JackknifeProjection%s", trkPtRange.c_str()), jackknifePbP);
    }

    // mix it up if only one of pPb or PbP
    if (!doCombine && collisionType == "PbP") {
        hData_ppb = hData_pbp;
        hMixData_ppb = hMixData_pbp;
        hNZData_ppb = hNZData_pbp;
        hNZMixData_ppb = hNZMixData_pbp;
    }


    // combined results
    for (int i = 0; i < input_ZPT_files.size(); i++) {

        cout<<"Combining ppb and pbp for index "<<i<<endl;
        
        // sig+bkg
        TH2D* S_combined = (TH2D*) hData_ppb[i]->Clone("S_combined");
        if(doCombine) S_combined->Add(hData_pbp[i]);
        cout<<"combined S integral: "<<S_combined->Integral()<<endl;

        float S_NZ = hNZData_ppb[i]->GetBinContent(1);
        if(doCombine) S_NZ += hNZData_pbp[i]->GetBinContent(1);
        S_combined->Scale(1. / S_NZ);
        cout<<"combined S NZ: "<<S_NZ<<endl;

        // bkg
        TH2D* B_combined = (TH2D*) hMixData_ppb[i]->Clone("B_combined");
        if(doCombine) B_combined->Add(hMixData_pbp[i]);
        cout<<"combined B integral: "<<B_combined->Integral()<<endl;

        float B_NZ = hNZMixData_ppb[i]->GetBinContent(1);
        if(doCombine) B_NZ += hNZMixData_pbp[i]->GetBinContent(1);
        B_combined->Scale(1. / B_NZ);
        cout<<"combined B NZ: "<<B_NZ<<endl;

        // bkg subtraction
        S_combined->Add(B_combined, -1);
        cout<<"combined S-B integral: "<<S_combined->Integral()<<endl;
        cout<<"SUBTRACTION EFFICIENCY: "<<S_combined->Integral() / B_combined->Integral()<<endl;

        if(useModified12x12)
            Symmetrize2DFourfold(S_combined);

        // projections
        ResultProjectionWindow projectionWindow;
        if(useModified12x12 == true) {
            if(ValidateModified12x12Histogram(S_combined) == false) {
                std::cerr << "Error: combined modified 12x12 histogram validation failed." << std::endl;
                return 1;
            }
            SetModified12x12ProjectionWindow(projectionWindow);
        }
        else if(useShifted10x10 == true) {
            if(ValidateShifted10x10Histogram(S_combined) == false) {
                std::cerr << "Error: combined shifted 10x10 histogram validation failed." << std::endl;
                return 1;
            }
            SetShifted10x10ProjectionWindow(projectionWindow);
        }
        else
            SetOfficial20BinProjectionWindow(projectionWindow);
        TH1D* hProjY = (TH1D*) S_combined->ProjectionY(Form("DeltaPhi_Result%i",i),
            projectionWindow.DeltaPhiXFirst, projectionWindow.DeltaPhiXLast);
        TH1D* hProjX = (TH1D*) S_combined->ProjectionX(Form("DeltaEta_Result%i",i),
            projectionWindow.DeltaEtaYFirst, projectionWindow.DeltaEtaYLast);
        if (i == 0) {
            if (doCombine) {
                double NZ_pPb = hNZData_ppb[0]->GetBinContent(1);
                double NZ_PbP = hNZData_pbp[0]->GetBinContent(1);
                ApplyCombinedDiagonalJackknifeErrors(hProjX, hProjY,
                    jackknifePPb, jackknifePbP, NZ_pPb, NZ_PbP);
            } else {
                ApplyProjectedJackknifeErrors(hProjX, hProjY,
                    collisionType == "PbP" ? jackknifePbP : jackknifePPb);
            }
        }
        divideByWidth(hProjY);
        hProjY->Scale(1./2);
        divideByWidth(hProjX);
        hProjX->Scale(1./2);
        if (useModified12x12 && i == 0) {
            Symmetrize1DEta(hProjX);
            Symmetrize1DPhi(hProjY);
        }
        hDeltaPhi_combined.push_back(hProjY);
        hDeltaEta_combined.push_back(hProjX);
        cout<<"DeltaPhi combined integral: "<<hProjY->Integral()<<endl;
        cout<<"DeltaEta combined integral: "<<hProjX->Integral()<<endl;

    }

    /*
    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kMagenta-3, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kMagenta-3, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 2, 0, 1};
    */
    
    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-8, kMagenta-3, cmsYellow, cmsGray};
    vector<int> markerStyles = {mSquareFill, mCircleFill, 0, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-8, kMagenta-3, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 0, 1};

    // ===========================================
    // results
    // ===========================================
    // Parse pT ranges from "LOW_HIGH" strings
    auto ParseRange = [](const string &range) -> pair<string, string> {
        size_t pos = range.find('_');
        if (pos == string::npos) return {range, ""};
        return {range.substr(0, pos), range.substr(pos + 1)};
    };

    pair<string, string> zRange = ParseRange(zPtRange);
    pair<string, string> trkRange = ParseRange(trkPtRange);

    string currentSystem = doCombine ? "pPbPbp" : collisionType;
    string ppSystematicsFile = Form("%s/pp_%s_ZPT%s_trkPT%s-systematics.root",
        systematicsDir.c_str(), systematicsTagPP.c_str(), zPtRange.c_str(), trkPtRange.c_str());
    string currentSystematicsFile = Form("%s/%s_%s_ZPT%s_trkPT%s-systematics.root",
        systematicsDir.c_str(), currentSystem.c_str(), systematicsTag.c_str(), zPtRange.c_str(), trkPtRange.c_str());

    vector<TH1 *> topSystematicsEta(hDeltaEta_combined.size(), nullptr);
    vector<TH1 *> topSystematicsPhi(hDeltaPhi_combined.size(), nullptr);
    vector<TH1 *> differenceSystematicsEta(hDeltaEta_combined.size(), nullptr);
    vector<TH1 *> differenceSystematicsPhi(hDeltaPhi_combined.size(), nullptr);
    if (useSystematics) {
        topSystematicsEta[0] = LoadSystematicHistogram(ppSystematicsFile, "Total_DeltaEta", "PPSystematicsEta");
        topSystematicsPhi[0] = LoadSystematicHistogram(ppSystematicsFile, "Total_DeltaPhi", "PPSystematicsPhi");
        if (hDeltaEta_combined.size() > 1) {
            topSystematicsEta[1] = LoadSystematicHistogram(currentSystematicsFile, "Total_DeltaEta", "CurrentSystematicsEta");
            topSystematicsPhi[1] = LoadSystematicHistogram(currentSystematicsFile, "Total_DeltaPhi", "CurrentSystematicsPhi");
            if (doCombine) {
                differenceSystematicsEta[1] = LoadSystematicHistogram(currentSystematicsFile, "DifferenceTotal_DeltaEta", "DifferenceSystematicsEta");
                differenceSystematicsPhi[1] = LoadSystematicHistogram(currentSystematicsFile, "DifferenceTotal_DeltaPhi", "DifferenceSystematicsPhi");
            }
        }
    }

    string differenceLabel = doCombine ? "pPb - pp" : Form("%s - pp", collisionType.c_str());
    string lumiLabel = doCombine ? "pPb, pp" : Form("%s, pp", collisionType.c_str());

    string deltaEtaProjectionLabel = "0 < #Delta#varphi_{ch,Z} < #pi/2";
    if(useShifted10x10 == true)
        deltaEtaProjectionLabel = "0 < #Delta#varphi_{ch,Z} < #pi";

    TCanvas* cResult1 = new TCanvas("cResult1", "cResult1", 600, 600);
    TPad* pResult1 = (TPad*) PlotCMSDiffResultRegion(
        hDeltaEta_combined, topSystematicsEta, differenceSystematicsEta, "", labels,
        lineColors, lineStyles,
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", useModified12x12 ? -3.87 : -4, useModified12x12 ? 3.87 : 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        differenceLabel.c_str(), -1, -1,
        0, 4,
        0,
        false, false, true,
        0.35
    );

    AddUPCHeader(pResult1, "8.16 TeV", lumiLabel.c_str());

    // Top-right eta-plot label
    pResult1->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAlign(31);   // right-aligned
    latex.SetTextSize(0.035);
    latex.DrawLatex(0.75, 0.82, Form("%s < p_{T}^{Z} < %s", zRange.first.c_str(), zRange.second.c_str()));
    latex.DrawLatex(0.75, 0.77, Form("%s < p_{T}^{ch} < %s", trkRange.first.c_str(), trkRange.second.c_str()));
    latex.DrawLatex(0.75, 0.72, Form("|y_{Z}| < 2.4, %s", deltaEtaProjectionLabel.c_str()));

    cResult1->Update();
    cResult1->SaveAs(Form("%s-DeltaEta-result.pdf", output.c_str()));

    TCanvas* cResult2 = new TCanvas("cResult2", "cResult2", 600, 600);
    TPad* pResult2 = (TPad*) PlotCMSDiffResultRegion(
        hDeltaPhi_combined, topSystematicsPhi, differenceSystematicsPhi, "", labels,
        lineColors, lineStyles,
        markerColors, markerStyles,
        "#Delta#varphi_{ch,Z}", useShifted10x10 ? -3 * M_PI / 5 : -1.5707, useShifted10x10 ? 7 * M_PI / 5 : 4.7123,
        "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}", -1, -1,
        differenceLabel.c_str(), -1, -1,
        0, M_PI,
        0,
        false, false, true,
        0.2
    );

    AddUPCHeader(pResult2, "8.16 TeV", lumiLabel.c_str());

    // Phi-plot labels (lowest line at x=0.5, y=0.3)
    pResult2->cd();
    TLatex latex2;
    latex2.SetNDC();
    latex2.SetTextAlign(11);   // centered
    latex2.SetTextSize(0.035);
    latex2.DrawLatex(0.21, 0.60, Form("%s < p_{T}^{Z} < %s", zRange.first.c_str(), zRange.second.c_str()));
    latex2.DrawLatex(0.21, 0.55, Form("%s < p_{T}^{ch} < %s", trkRange.first.c_str(), trkRange.second.c_str()));
    latex2.DrawLatex(0.21, 0.5, "|y_{Z}| < 2.4");

    cResult2->Update();
    cResult2->SaveAs(Form("%s-DeltaPhi-result.pdf", output.c_str()));


}
