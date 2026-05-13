// plot_pp_ee.cpp
//
// pp energy-extrapolation MC vs DATA overlay. Four curves per plot:
//   (1) pp pythia+madgraph 5.02 TeV MC gen-level
//   (2) fully corrected (no EE weight) pp 5.02 TeV data
//   (3) pp pythia+madgraph 8.16 TeV MC gen-level
//   (4) fully corrected (with EE weight) pp 8.16 TeV data
//
// Two top-level variants produced via --eeMode {data,mc}, swapping curve (4)
// between the data-driven and MC-driven energy-extrapolation weight inputs.
//
// Observables: Zpt (ratio bottom panel), DeltaPhi / DeltaEta (difference bottom panel).
// Style matched to PlotCMSDiffResult / systematics plots.

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH3D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"
#include "SetStyle.h"

namespace {

// Layout helper: top panel with all hists, bottom panel with explicit
// (numerator, denominator) pairs as ratio or difference.
// Modelled after PlotCMSDiffResult in KylesPlotting.h.
//
// rmin < rmax  -> fixed bottom range.
// rmin >= rmax -> caller is expected to have pre-computed a sensible range,
//                 but the condition also triggers ROOT auto as a fallback.
// showPairLegend=false -> no legend in the bottom pad (used for diff panels).
TPad* plotPaired(
    TCanvas* c,
    const vector<TH1*>& hists,
    const vector<string>& labels,
    const vector<int>& lineColors,
    const vector<int>& lineStyles,
    const vector<int>& markerColors,
    const vector<int>& markerStyles,
    const vector<pair<int,int>>& pairs,
    const vector<string>& pairLabels,
    const vector<int>& pairColors,
    const vector<int>& pairMarkers,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    bool logx, bool logy,
    bool doDifference,
    float xLegend,
    bool showPairLegend = true
) {
    const double border = 0.06;
    c->cd();
    TPad* pad1 = new TPad(Form("pad1_%s", c->GetName()), "",
                          border, 0.25 + border, 1.0 - border, 1.0 - border);
    pad1->SetBottomMargin(0);
    if (logy) pad1->SetLogy();
    if (logx) pad1->SetLogx();
    pad1->Draw();
    TPad* pad2 = new TPad(Form("pad2_%s", c->GetName()), "",
                          border, border, 1.0 - border, 0.25 + border);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    if (logx) pad2->SetLogx();
    pad2->Draw();

    TLegend* leg = new TLegend(xLegend, (labels.size() > 4) ? 0.55 : 0.62,
                               xLegend + 0.32, 0.86);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.032);

    // Pre-compute global min/max for top-panel auto-scaling (mirrors PlotCMSDiffResult).
    double global_min = 1e30, global_max = -1e30;
    if (!(ymin < ymax)) {
        for (size_t i = 0; i < hists.size(); ++i) {
            TH1* h = hists[i];
            if (!h) continue;
            int blo = (xmin < xmax) ? h->GetXaxis()->FindBin(xmin) : 1;
            int bhi = (xmin < xmax) ? h->GetXaxis()->FindBin(xmax) : h->GetNbinsX();
            for (int b = blo; b <= bhi; ++b) {
                double v = h->GetBinContent(b);
                double e = (lineStyles[i] == 0 || lineStyles[i] == -1) ? h->GetBinError(b) : 0;
                global_min = min(global_min, v - e);
                global_max = max(global_max, v + e);
            }
        }
        double mg;
        if (logy) {
            double lo = (global_min > 0) ? global_min : 1e-10;
            mg = exp((log(global_max) - log(lo)) * 0.2);
        } else {
            mg = 0.2 * (global_max - global_min);
        }
        global_min -= mg;
        global_max += mg;
    }

    pad1->cd();
    for (size_t i = 0; i < hists.size(); ++i) {
        TH1* h = hists[i];
        if (!h) continue;
        h->SetTitle("");
        h->SetLineColor(lineColors[i]);
        h->SetLineStyle(lineStyles[i] > 0 ? lineStyles[i] : 1);
        h->SetMarkerColor(markerColors[i]);
        h->SetMarkerStyle(markerStyles[i]);
        h->SetLineWidth(lineStyles[i] == 0 ? 3 : 2);

        h->GetXaxis()->SetTitle(xTitle);
        h->GetXaxis()->SetRangeUser(xmin, xmax);
        h->GetYaxis()->SetTitle(yTitle);
        h->GetYaxis()->SetTitleSize(0.05);
        h->GetYaxis()->SetTitleOffset(0.7);

        if (ymin < ymax) {
            if (logy && ymin <= 0) h->GetYaxis()->SetRangeUser(1, ymax);
            else                   h->GetYaxis()->SetRangeUser(ymin, ymax);
        } else {
            if (logy && global_min <= 0) h->GetYaxis()->SetRangeUser(1, global_max);
            else                          h->GetYaxis()->SetRangeUser(global_min, global_max);
        }

        string drawCmd = "SAME";
        if (lineStyles[i] == -1) drawCmd = "P E SAME";
        else if (lineStyles[i] != 0) drawCmd = "HIST SAME";
        h->Draw(drawCmd.c_str());

        if (lineStyles[i] == 0)       leg->AddEntry(h, labels[i].c_str(), "pl");
        else if (lineStyles[i] == -1) leg->AddEntry(h, labels[i].c_str(), "p");
        else                          leg->AddEntry(h, labels[i].c_str(), "l");
    }
    leg->Draw("SAME");

    // Bottom panel.
    pad2->cd();
    TLegend* legPair = new TLegend(xLegend, 0.62, xLegend + 0.32, 0.95);
    legPair->SetBorderSize(0);
    legPair->SetFillStyle(0);
    legPair->SetTextFont(42);
    legPair->SetTextSize(0.08);

    bool first = true;
    for (size_t p = 0; p < pairs.size(); ++p) {
        int iN = pairs[p].first;
        int iD = pairs[p].second;
        TH1* num = hists[iN];
        TH1* den = hists[iD];
        if (!num || !den) continue;

        TH1* hPair = (TH1*)num->Clone(Form("pair_%s_%zu", c->GetName(), p));
        hPair->SetDirectory(nullptr);
        hPair->SetTitle("");
        if (doDifference) hPair->Add(den, -1);
        else              hPair->Divide(den);

        hPair->SetLineColor(pairColors[p]);
        hPair->SetMarkerColor(pairColors[p]);
        hPair->SetMarkerStyle(pairMarkers[p]);
        hPair->SetLineWidth(2);

        hPair->GetXaxis()->SetTitle(xTitle);
        hPair->GetXaxis()->SetTitleSize(0.1);
        hPair->GetXaxis()->SetLabelSize(0.08);
        hPair->GetXaxis()->SetTitleOffset(1.0);
        hPair->GetXaxis()->SetRangeUser(xmin, xmax);

        hPair->GetYaxis()->SetTitle(rTitle);
        hPair->GetYaxis()->SetTitleSize(0.08);
        hPair->GetYaxis()->SetLabelSize(0.06);
        hPair->GetYaxis()->SetTitleOffset(0.5);
        if (rmin < rmax) hPair->GetYaxis()->SetRangeUser(rmin, rmax);

        hPair->Draw(first ? "P E" : "P E SAME");
        legPair->AddEntry(hPair, pairLabels[p].c_str(), "pl");
        first = false;
    }

    double refY = doDifference ? 0.0 : 1.0;
    TLine* line = new TLine(xmin, refY, xmax, refY);
    line->SetLineColor(kGray + 2);
    line->SetLineStyle(2);
    line->Draw("SAME");

    if (showPairLegend) legPair->Draw("SAME");

    return pad1;
}

void DrawKinematicLabels(TPad* pad, const string& zPtRange, const string& trkPtRange) {
    pad->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(42);
    latex.SetTextSize(0.032);
    auto formatRange = [](const string& r, const string& sym) -> string {
        size_t split = r.find('_');
        string lo = (split == string::npos) ? r : r.substr(0, split);
        string hi = (split == string::npos) ? string() : r.substr(split + 1);
        if (hi.empty() || hi == "500") return sym + " > " + lo + " GeV";
        return lo + " < " + sym + " < " + hi + " GeV";
    };
    latex.DrawLatex(0.18, 0.79, formatRange(trkPtRange, "p_{T}^{ch}").c_str());
    latex.DrawLatex(0.18, 0.74, formatRange(zPtRange, "p_{T}^{Z}").c_str());
}

TH1D* loadZPt(TFile* fin, const string& trkPtRange, const string& tag) {
    TH3D* h3 = (TH3D*)fin->Get(Form("hZPtEtaPhi_%s", trkPtRange.c_str()));
    if (!h3) {
        cerr << "Missing hZPtEtaPhi_" << trkPtRange << " in " << fin->GetName() << endl;
        return nullptr;
    }
    TH1D* h = h3->ProjectionX(Form("ZPt_%s", tag.c_str()));
    h->SetDirectory(nullptr);
    divideByWidth(h);
    return h;
}

TH1D* loadDelta(TFile* fin, const string& observable, const string& trkPtRange, const string& tag) {
    TH1D* h = (TH1D*)fin->Get(Form("%s_Result%s", observable.c_str(), trkPtRange.c_str()));
    if (!h) {
        cerr << "Missing " << observable << "_Result" << trkPtRange
             << " in " << fin->GetName() << endl;
        return nullptr;
    }
    h = (TH1D*)h->Clone(Form("%s_%s", observable.c_str(), tag.c_str()));
    h->SetDirectory(nullptr);
    // Maintained 0.5 normalization: analysis double-fills the result histograms.
    h->Scale(0.5);
    return h;
}

// Compute auto-range for ratio bottom panel: actual min of num/den pairs, top fixed at 1.
// Returns {rmin, 1.0} with 15% headroom below the min.
pair<double,double> ratioAutoRange(const vector<TH1*>& hists,
                                   const vector<pair<int,int>>& pairs,
                                   double xmin, double xmax) {
    double minVal = 1.0;
    for (auto& pr : pairs) {
        TH1* num = hists[pr.first];
        TH1* den = hists[pr.second];
        if (!num || !den) continue;
        TH1* tmp = (TH1*)num->Clone("_tmpRatio");
        tmp->Divide(den);
        int blo = (xmin < xmax) ? tmp->GetXaxis()->FindBin(xmin) : 1;
        int bhi = (xmin < xmax) ? tmp->GetXaxis()->FindBin(xmax) : tmp->GetNbinsX();
        for (int b = blo; b <= bhi; ++b) {
            double v = tmp->GetBinContent(b);
            if (v > 0) minVal = min(minVal, v);
        }
        delete tmp;
    }
    double top = 1.0;
    double headroom = 0.15 * (top - minVal);
    return {minVal - headroom, top};
}

// Compute auto-range for difference bottom panel (15% margin, matching PlotCMSDiffResult).
pair<double,double> diffAutoRange(const vector<TH1*>& hists,
                                  const vector<pair<int,int>>& pairs,
                                  double xmin, double xmax) {
    double dmin = 1e30, dmax = -1e30;
    for (auto& pr : pairs) {
        TH1* num = hists[pr.first];
        TH1* den = hists[pr.second];
        if (!num || !den) continue;
        TH1* tmp = (TH1*)num->Clone("_tmpDiff");
        tmp->Add(den, -1.0);
        int blo = (xmin < xmax) ? tmp->GetXaxis()->FindBin(xmin) : 1;
        int bhi = (xmin < xmax) ? tmp->GetXaxis()->FindBin(xmax) : tmp->GetNbinsX();
        for (int b = blo; b <= bhi; ++b) {
            double v = tmp->GetBinContent(b);
            double e = tmp->GetBinError(b);
            dmin = min(dmin, v - e);
            dmax = max(dmax, v + e);
        }
        delete tmp;
    }
    if (dmin > dmax) { dmin = -0.1; dmax = 0.1; }
    double mg = 0.15 * (dmax - dmin);
    if (mg <= 0) mg = max(fabs(dmax), 0.1) * 0.15;
    return {dmin - mg, dmax + mg};
}

}  // namespace


int main(int argc, char* argv[]) {
    CommandLine CL(argc, argv);

    const string zPtRange   = CL.Get("zPtRange",   "0_500");
    const string trkPtRange = CL.Get("trkPtRange", "0.5_15");
    const string pptag      = CL.Get("pptag",      "EEV5_ZV9_trkV27_nmix10");
    const string tag5020    = CL.Get("tag5020",    "madgraphPP5020_nmix10");
    const string tag8160    = CL.Get("tag8160",    "madgraphPP8160_nmix10");
    const string eeMode     = CL.Get("eeMode",     "data");   // "data" | "mc"
    const string inputDir   = CL.Get("inputDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outputDir  = CL.Get("outputDir",  "plots/pp_ee");

    gROOT->SetBatch(kTRUE);
    SetThesisStyle();
    gStyle->SetOptStat(0);
    gSystem->mkdir(outputDir.c_str(), true);

    if (eeMode != "data" && eeMode != "mc") {
        cerr << "Unknown --eeMode " << eeMode << " (expected 'data' or 'mc')" << endl;
        return 1;
    }

    const string genTag5020 = Form("madgraphMC_Gen_nominal_%s",  tag5020.c_str());
    const string genTag8160 = Form("madgraphMC_Gen_nominal_%s",  tag8160.c_str());
    const string dataNoEE   = Form("pp_EEtrkResidual_%s",        pptag.c_str());
    const string dataYesEE  = (eeMode == "mc")
        ? Form("pp_trkResidual_%s_EEPrivate", pptag.c_str())
        : Form("pp_trkResidual_%s",           pptag.c_str());

    vector<string> bases = {
        Form("%s/%s_ZPT%s", inputDir.c_str(), genTag5020.c_str(),  zPtRange.c_str()),
        Form("%s/%s_ZPT%s", inputDir.c_str(), dataNoEE.c_str(),    zPtRange.c_str()),
        Form("%s/%s_ZPT%s", inputDir.c_str(), genTag8160.c_str(),  zPtRange.c_str()),
        Form("%s/%s_ZPT%s", inputDir.c_str(), dataYesEE.c_str(),   zPtRange.c_str()),
    };
    vector<string> shortTags = {"mc5020", "data5020", "mc8160", "data8160"};
    vector<string> labels = {
        "Pythia+Madgraph GEN 5.02 TeV",
        "pp DATA 5.02 TeV (no EE weight)",
        "Pythia+Madgraph GEN 8.16 TeV",
        Form("pp DATA 8.16 TeV (%s EE)", eeMode == "mc" ? "MC-driven" : "data-driven"),
    };

    // gen MC as dashed lines; data as points+line.
    vector<int> lineColors   = {cmsBlue,     cmsBlue,     cmsRed,     cmsRed};
    vector<int> lineStyles   = {2,           0,           2,          0};
    vector<int> markerColors = {cmsBlue,     cmsBlue,     cmsRed,     cmsRed};
    vector<int> markerStyles = {mSquareFill, mCircleFill, mSquareFill, mCircleFill};

    // Bottom-panel pairs: (1)/(2) at 5.02 TeV, (3)/(4) at 8.16 TeV.
    vector<pair<int,int>> pairs    = {{0, 1}, {2, 3}};
    vector<string>        pairLabels = {"5.02 TeV MC / DATA", "8.16 TeV MC / DATA"};
    vector<int>           pairColors  = {cmsBlue, cmsRed};
    vector<int>           pairMarkers = {mCircleFill, mCircleFill};

    // Open all four input file pairs.
    vector<TFile*> resultFiles(4, nullptr);
    vector<TFile*> nosubFiles(4, nullptr);
    bool ok = true;
    for (size_t i = 0; i < bases.size(); ++i) {
        resultFiles[i] = TFile::Open((bases[i] + "-result.root").c_str(), "READ");
        if (!resultFiles[i] || resultFiles[i]->IsZombie()) {
            cerr << "Unable to open " << bases[i] << "-result.root" << endl; ok = false;
        }
        nosubFiles[i] = TFile::Open((bases[i] + "-nosub.root").c_str(), "READ");
        if (!nosubFiles[i] || nosubFiles[i]->IsZombie()) {
            cerr << "Unable to open " << bases[i] << "-nosub.root" << endl; ok = false;
        }
    }
    if (!ok) return 1;

    // ── ZPt ──────────────────────────────────────────────────────────────────
    vector<TH1*> hZPt(4, nullptr);
    for (size_t i = 0; i < bases.size(); ++i) {
        hZPt[i] = loadZPt(nosubFiles[i], trkPtRange, shortTags[i]);
        if (!hZPt[i]) return 1;
    }

    // Ratio bottom range: auto-detect min of MC/DATA, top fixed at 1.
    auto [zPtRmin, zPtRmax] = ratioAutoRange(hZPt, pairs, 5.0, 200.0);

    const string outBase = Form("%s/pp_ee_%s_ZPT%s_trkPT%s",
                                outputDir.c_str(), eeMode.c_str(),
                                zPtRange.c_str(), trkPtRange.c_str());
    {
        TCanvas* c = new TCanvas(Form("cZPt_%s", eeMode.c_str()), "", 600, 600);
        TPad* pad = plotPaired(
            c, hZPt, labels,
            lineColors, lineStyles, markerColors, markerStyles,
            pairs, pairLabels, pairColors, pairMarkers,
            "p_{T}^{Z} (GeV)", 5, 200,
            "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", 2e-5, 1,
            "MC / DATA", 0.4, 1.6,
            true, true,
            false,           // ratio panel
            0.45,
            true             // show pair legend on ZPt ratio
        );
        AddCMSHeader(pad, "Internal", false);
        AddUPCHeader(pad, "5.02, 8.16 TeV", "pp");
        DrawKinematicLabels(pad, zPtRange, trkPtRange);
        c->Update();
        c->SaveAs(Form("%s-ZPt.pdf", outBase.c_str()));
        delete c;
    }

    // ── DeltaPhi / DeltaEta ──────────────────────────────────────────────────
    for (const string& obs : {string("DeltaPhi"), string("DeltaEta")}) {
        vector<TH1*> hObs(4, nullptr);
        for (size_t i = 0; i < bases.size(); ++i) {
            hObs[i] = loadDelta(resultFiles[i], obs, trkPtRange, shortTags[i]);
            if (!hObs[i]) return 1;
        }

        double xmin = (obs == "DeltaPhi") ? -1.5707 : -4.0;
        double xmax = (obs == "DeltaPhi") ?  4.7123 :  4.0;
        double ymin = (obs == "DeltaPhi") ? -1 : -1;
        double ymax = (obs == "DeltaPhi") ? 3.5 : -1;
        string xTitle = (obs == "DeltaPhi") ? "#Delta#phi_{ch,Z}" : "#Delta y_{ch,Z}";
        string yTitle = (obs == "DeltaPhi")
            ? "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}"
            : "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}";

        // Auto-scale difference panel matching PlotCMSDiffResult (15% margin).
        auto [rminD, rmaxD] = diffAutoRange(hObs, pairs, xmin, xmax);

        TCanvas* c = new TCanvas(Form("c%s_%s", obs.c_str(), eeMode.c_str()), "", 600, 600);
        TPad* pad = plotPaired(
            c, hObs, labels,
            lineColors, lineStyles, markerColors, markerStyles,
            pairs, pairLabels, pairColors, pairMarkers,
            xTitle.c_str(), xmin, xmax,
            yTitle.c_str(), ymin, ymax,
            "MC #minus DATA", rminD, rmaxD,
            false, false,
            true,            // difference panel
            0.42,
            false            // no legend in difference subplot
        );
        AddCMSHeader(pad, "Internal", false);
        AddUPCHeader(pad, "5.02, 8.16 TeV", "pp");
        DrawKinematicLabels(pad, zPtRange, trkPtRange);
        c->Update();
        c->SaveAs(Form("%s-%s.pdf", outBase.c_str(), obs.c_str()));
        delete c;
    }

    for (TFile* f : resultFiles) if (f) { f->Close(); delete f; }
    for (TFile* f : nosubFiles)  if (f) { f->Close(); delete f; }

    cout << "Wrote plots to " << outputDir << " (eeMode=" << eeMode << ")" << endl;
    return 0;
}
