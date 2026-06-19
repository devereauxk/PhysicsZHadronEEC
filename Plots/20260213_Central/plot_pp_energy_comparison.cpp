#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <TSystem.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"
#include "SetStyle.h"

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
    if (file == nullptr) return;
    TTree *tree = (TTree *)file->Get(treeName.c_str());
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

    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        JackknifeProjectionContribution c;
        c.SignalNZ = signalNZ;
        c.MixNZ = mixNZ;
        c.SignalEta = *signalEta;
        c.MixEta = *mixEta;
        c.SignalPhi = *signalPhi;
        c.MixPhi = *mixPhi;
        events.push_back(c);
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

void ApplyProjectedJackknifeErrors(TH1D *hEta, TH1D *hPhi,
    const vector<JackknifeProjectionContribution> &events)
{
    if (events.size() < 2)
        return;

    vector<double> etaSigma = ComputeProjectedJackknifeSigma(events, hEta, true);
    vector<double> phiSigma = ComputeProjectedJackknifeSigma(events, hPhi, false);

    for (int i = 1; i <= hEta->GetNbinsX(); ++i)
        hEta->SetBinError(i, etaSigma[i - 1]);
    for (int i = 1; i <= hPhi->GetNbinsX(); ++i)
        hPhi->SetBinError(i, phiSigma[i - 1]);
}

void Symmetrize1DEta(TH1D *h)
{
    if (h == nullptr) return;
    int n = h->GetNbinsX();
    if (n != 12) return;
    for (int i = 0; i < 6; ++i) {
        int mi = 11 - i;
        double v = 0.5 * (h->GetBinContent(i + 1) + h->GetBinContent(mi + 1));
        double e = 0.5 * sqrt(pow(h->GetBinError(i + 1), 2) + pow(h->GetBinError(mi + 1), 2));
        h->SetBinContent(i + 1, v); h->SetBinError(i + 1, e);
        h->SetBinContent(mi + 1, v); h->SetBinError(mi + 1, e);
    }
}

void Symmetrize1DPhi(TH1D *h)
{
    if (h == nullptr) return;
    int n = h->GetNbinsX();
    if (n != 12) return;
    for (int j = 0; j < 3; ++j) {
        int mj = 5 - j;
        double v = 0.5 * (h->GetBinContent(j + 1) + h->GetBinContent(mj + 1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j + 1), 2) + pow(h->GetBinError(mj + 1), 2));
        h->SetBinContent(j + 1, v); h->SetBinError(j + 1, e);
        h->SetBinContent(mj + 1, v); h->SetBinError(mj + 1, e);
    }
    for (int j = 6; j < 9; ++j) {
        int mj = 17 - j;
        double v = 0.5 * (h->GetBinContent(j + 1) + h->GetBinContent(mj + 1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j + 1), 2) + pow(h->GetBinError(mj + 1), 2));
        h->SetBinContent(j + 1, v); h->SetBinError(j + 1, e);
        h->SetBinContent(mj + 1, v); h->SetBinError(mj + 1, e);
    }
}

TH1D *BuildDifferenceSystBand(TH1D *syst1, TH1D *syst2, const char *name)
{
    if (syst1 == nullptr && syst2 == nullptr) return nullptr;
    TH1D *ref = (syst1 != nullptr) ? syst1 : syst2;
    TH1D *h = (TH1D *)ref->Clone(name);
    h->SetDirectory(nullptr);
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
        double s1 = (syst1 != nullptr) ? syst1->GetBinContent(i) : 0;
        double s2 = (syst2 != nullptr) ? syst2->GetBinContent(i) : 0;
        h->SetBinContent(i, sqrt(s1 * s1 + s2 * s2));
    }
    return h;
}

struct PPResult {
    TH1D *hEta = nullptr;
    TH1D *hPhi = nullptr;
};

PPResult LoadPPResult(const string &fileBase, const string &trkPtRange)
{
    PPResult r;
    TFile *f = TFile::Open(Form("%s-result.root", fileBase.c_str()), "READ");
    if (!f || f->IsZombie()) {
        cerr << "Error: cannot open " << fileBase << "-result.root" << endl;
        return r;
    }

    r.hPhi = (TH1D *)((TH1D *)f->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str())))->Clone("tmp_phi");
    r.hEta = (TH1D *)((TH1D *)f->Get(Form("DeltaEta_Result%s", trkPtRange.c_str())))->Clone("tmp_eta");
    r.hPhi->SetDirectory(nullptr);
    r.hEta->SetDirectory(nullptr);

    vector<JackknifeProjectionContribution> jk;
    AppendJackknifeProjectionContributions(f, Form("JackknifeProjection%s", trkPtRange.c_str()), jk);
    if (jk.size() >= 2) {
        ApplyProjectedJackknifeErrors(r.hEta, r.hPhi, jk);
        for (int i = 1; i <= r.hEta->GetNbinsX(); ++i)
            r.hEta->SetBinError(i, r.hEta->GetBinError(i) / r.hEta->GetBinWidth(i));
        for (int i = 1; i <= r.hPhi->GetNbinsX(); ++i)
            r.hPhi->SetBinError(i, r.hPhi->GetBinError(i) / r.hPhi->GetBinWidth(i));
    }

    r.hPhi->Scale(0.5);
    r.hEta->Scale(0.5);

    Symmetrize1DEta(r.hEta);
    Symmetrize1DPhi(r.hPhi);

    return r;
}

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    string zPtRange = CL.Get("zPtRange", "0_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_15");
    string tag5 = CL.Get("pp5tevTag", "5tev_ZV10_trkV29_nmix10");
    string tag8 = CL.Get("pp8tevTag", "EEV6_ZV10_trkV29_nmix10");
    string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    string systDir5 = CL.Get("systematicsDir5tev",
        "/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output");
    string systDir8 = CL.Get("systematicsDir8tev",
        "/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output");
    string outputBase = CL.Get("outputBase", "plots/pp_energy_comparison");
    bool isScan = CL.GetBool("isScan", false);

    string tag5Full = isScan ? tag5 + "_scan" : tag5;
    string tag8Full = isScan ? tag8 + "_scan" : tag8;

    cout << "=== pp Energy Comparison ===" << endl;
    cout << "ZPT: " << zPtRange << " trkPT: " << trkPtRange << endl;
    cout << "5 TeV tag: " << tag5Full << " 8 TeV tag: " << tag8Full << endl;

    string file5 = Form("%s/pp_trkResidual_%s_ZPT%s", baseDir.c_str(), tag5Full.c_str(), zPtRange.c_str());
    string file8 = Form("%s/pp_trkResidual_%s_ZPT%s", baseDir.c_str(), tag8Full.c_str(), zPtRange.c_str());

    PPResult pp5 = LoadPPResult(file5, trkPtRange);
    PPResult pp8 = LoadPPResult(file8, trkPtRange);
    if (!pp5.hEta || !pp8.hEta) return 1;

    string systFile5 = Form("%s/pp5tev_%s_ZPT%s_trkPT%s-systematics.root",
        systDir5.c_str(), tag5Full.c_str(), zPtRange.c_str(), trkPtRange.c_str());
    string systFile8 = Form("%s/pp_%s_ZPT%s_trkPT%s-systematics.root",
        systDir8.c_str(), tag8Full.c_str(), zPtRange.c_str(), trkPtRange.c_str());

    cout << "Syst 5 TeV: " << systFile5 << endl;
    cout << "Syst 8 TeV: " << systFile8 << endl;

    TH1D *systEta5 = LoadSystematicHistogram(systFile5, "Total_DeltaEta", "systEta5");
    TH1D *systPhi5 = LoadSystematicHistogram(systFile5, "Total_DeltaPhi", "systPhi5");
    TH1D *systEta8 = LoadSystematicHistogram(systFile8, "Total_DeltaEta", "systEta8");
    TH1D *systPhi8 = LoadSystematicHistogram(systFile8, "Total_DeltaPhi", "systPhi8");

    TH1D *diffSystEta = BuildDifferenceSystBand(systEta5, systEta8, "diffSystEta");
    TH1D *diffSystPhi = BuildDifferenceSystBand(systPhi5, systPhi8, "diffSystPhi");

    vector<TH1 *> hEta = {(TH1 *)pp5.hEta, (TH1 *)pp8.hEta};
    vector<TH1 *> hPhi = {(TH1 *)pp5.hPhi, (TH1 *)pp8.hPhi};
    vector<TH1 *> topSystEta = {(TH1 *)systEta5, (TH1 *)systEta8};
    vector<TH1 *> topSystPhi = {(TH1 *)systPhi5, (TH1 *)systPhi8};
    vector<TH1 *> botSystEta = {nullptr, (TH1 *)diffSystEta};
    vector<TH1 *> botSystPhi = {nullptr, (TH1 *)diffSystPhi};

    vector<string> labels = {"pp (5.02 TeV)", "pp (8.16 TeV, extrapolated)"};
    vector<int> markerColors = {cmsYellow, cmsBlue};
    vector<int> markerStyles = {mCircleFill, mSquareFill};
    vector<int> lineColors = {cmsYellow, cmsBlue};
    vector<int> lineStyles = {0, 0};

    auto ParseRange = [](const string &range) -> pair<string, string> {
        size_t pos = range.find('_');
        if (pos == string::npos) return {range, ""};
        return {range.substr(0, pos), range.substr(pos + 1)};
    };
    pair<string, string> zRange = ParseRange(zPtRange);
    pair<string, string> trkRange = ParseRange(trkPtRange);

    gSystem->mkdir(Form("%s/%s", outputBase.c_str(), tag8.c_str()), true);
    string output = Form("%s/%s/pp_energy_ZPT%s_trkPT%s",
        outputBase.c_str(), tag8.c_str(), zPtRange.c_str(), trkPtRange.c_str());

    float resultTextScale = 1.3;
    float plotTextSize = 0.035 * resultTextScale;

    // DeltaEta
    TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
    TPad *p1 = (TPad *)PlotCMSDiffResultRegion(
        hEta, topSystEta, botSystEta, "", labels,
        lineColors, lineStyles, markerColors, markerStyles,
        "#Delta y_{ch,Z}", -3.87, 3.87,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        "8TeV#minus5TeV", -1, -1,
        0, 4,
        0,
        false, false, true,
        2.0, resultTextScale, 0.05, 3.0
    );

    {
        p1->cd();
        TLatex lumiLatex;
        lumiLatex.SetNDC();
        lumiLatex.SetTextFont(42);
        lumiLatex.SetTextAlign(31);
        lumiLatex.SetTextSize(plotTextSize);
        float t1 = p1->GetTopMargin();
        float r1 = p1->GetRightMargin();
        lumiLatex.DrawLatex(1 - r1, 1 - t1 + 0.015,
            "pp  5.02, 8.16 TeV  301 pb^{-1}");
    }

    p1->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(42);
    latex.SetTextAlign(11);
    latex.SetTextSize(plotTextSize);
    float labelY1 = 0.82;
    if (zPtRange != "0_500") {
        string zPtLabel = (zRange.first == "0") ?
            Form("p_{T}^{Z} < %s GeV", zRange.second.c_str()) :
            Form("p_{T}^{Z} > %s GeV", zRange.first.c_str());
        latex.DrawLatex(0.25, labelY1, zPtLabel.c_str());
        labelY1 -= 0.06;
    }
    latex.DrawLatex(0.25, labelY1, Form("%s < p_{T}^{ch} < %s GeV", trkRange.first.c_str(), trkRange.second.c_str()));
    latex.DrawLatex(0.25, labelY1 - 0.06, "|y_{CM}| < 1.935, 0 < #Delta#varphi_{ch,Z} < #frac{#pi}{2}");

    c1->Update();
    c1->SaveAs(Form("%s-DeltaEta-result.pdf", output.c_str()));
    delete c1;

    // DeltaPhi
    TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);
    TPad *p2 = (TPad *)PlotCMSDiffResultRegion(
        hPhi, topSystPhi, botSystPhi, "", labels,
        lineColors, lineStyles, markerColors, markerStyles,
        "#Delta#varphi_{ch,Z}", -1.5707, 4.7123,
        "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}", -1, -1,
        "8TeV#minus5TeV", -1, -1,
        0, M_PI,
        0,
        false, false, true,
        0.23, resultTextScale, 0.50
    );

    {
        p2->cd();
        TLatex lumiLatex2;
        lumiLatex2.SetNDC();
        lumiLatex2.SetTextFont(42);
        lumiLatex2.SetTextAlign(31);
        lumiLatex2.SetTextSize(plotTextSize);
        float t2 = p2->GetTopMargin();
        float r2 = p2->GetRightMargin();
        lumiLatex2.DrawLatex(1 - r2, 1 - t2 + 0.015,
            "pp  5.02, 8.16 TeV  301 pb^{-1}");
    }

    p2->cd();
    {
        TLegend* legReflPhi = new TLegend(0.21, 0.38, 0.61, 0.50);
        legReflPhi->SetBorderSize(0);
        legReflPhi->SetFillStyle(0);
        legReflPhi->SetTextFont(42);
        legReflPhi->SetTextSize(plotTextSize);
        TGraph* gRefPP5phi = new TGraph(1);
        gRefPP5phi->SetMarkerColor(cmsYellow);
        gRefPP5phi->SetMarkerStyle(OpenMarkerStyle(mCircleFill));
        gRefPP5phi->SetMarkerSize(resultTextScale);
        gRefPP5phi->SetLineColor(cmsYellow);
        legReflPhi->AddEntry(gRefPP5phi, "5.02 TeV reflected", "p");
        TGraph* gRefPP8phi = new TGraph(1);
        gRefPP8phi->SetMarkerColor(cmsBlue);
        gRefPP8phi->SetMarkerStyle(OpenMarkerStyle(mSquareFill));
        gRefPP8phi->SetMarkerSize(resultTextScale);
        gRefPP8phi->SetLineColor(cmsBlue);
        legReflPhi->AddEntry(gRefPP8phi, "8.16 TeV reflected", "p");
        legReflPhi->Draw("SAME");
    }

    TLatex latex2;
    latex2.SetNDC();
    latex2.SetTextFont(42);
    latex2.SetTextAlign(11);
    latex2.SetTextSize(plotTextSize);
    float labelY2 = 0.82;
    if (zPtRange != "0_500") {
        string zPtLabel2 = (zRange.first == "0") ?
            Form("p_{T}^{Z} < %s GeV", zRange.second.c_str()) :
            Form("p_{T}^{Z} > %s GeV", zRange.first.c_str());
        latex2.DrawLatex(0.25, labelY2, zPtLabel2.c_str());
        labelY2 -= 0.06;
    }
    latex2.DrawLatex(0.25, labelY2, Form("%s < p_{T}^{ch} < %s GeV", trkRange.first.c_str(), trkRange.second.c_str()));
    latex2.DrawLatex(0.25, labelY2 - 0.06, "|y_{CM}| < 1.935");

    c2->Update();
    c2->SaveAs(Form("%s-DeltaPhi-result.pdf", output.c_str()));
    delete c2;

    return 0;
}
