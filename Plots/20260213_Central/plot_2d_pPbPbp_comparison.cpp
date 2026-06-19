#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TMath.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

// ── Utilities ──────────────────────────────────────────────────────────────

static TH2D *loadH2(const string &fname, const string &key)
{
    TFile *f = TFile::Open(fname.c_str(), "READ");
    if (!f || f->IsZombie()) { cerr << "Cannot open " << fname << endl; return nullptr; }
    TH2D *h = (TH2D *)f->Get(key.c_str());
    if (!h) { cerr << "Missing " << key << " in " << fname << endl; f->Close(); return nullptr; }
    h = (TH2D *)h->Clone((key + "_cl").c_str());
    h->SetDirectory(nullptr);
    f->Close();
    return h;
}

static void divideByArea(TH2D *h)
{
    for (int ix = 1; ix <= h->GetNbinsX(); ++ix)
        for (int iy = 1; iy <= h->GetNbinsY(); ++iy) {
            double a = h->GetXaxis()->GetBinWidth(ix) * h->GetYaxis()->GetBinWidth(iy);
            if (a > 0) h->SetBinContent(ix, iy, h->GetBinContent(ix, iy) / a);
        }
}

// Extract physical 6×6 sub-histogram: xbins 7–12 → ΔEta [0,4], ybins 4–9 → ΔPhi [0,π]
static TH2D *extractPhysical(TH2D *full, const char *name)
{
    const int x0 = 7, x1 = 12, y0 = 4, y1 = 9;
    int nx = x1 - x0 + 1, ny = y1 - y0 + 1;
    TH2D *sub = new TH2D(name, "",
        nx, full->GetXaxis()->GetBinLowEdge(x0), full->GetXaxis()->GetBinUpEdge(x1),
        ny, full->GetYaxis()->GetBinLowEdge(y0), full->GetYaxis()->GetBinUpEdge(y1));
    sub->SetDirectory(nullptr);
    for (int ix = 0; ix < nx; ++ix)
        for (int iy = 0; iy < ny; ++iy)
            sub->SetBinContent(ix + 1, iy + 1, full->GetBinContent(x0 + ix, y0 + iy));
    return sub;
}

// Build 2D result for a single system: (hData - hMixData) × 0.5 / binArea
// hData is already normalized by N_Z from the -nosub.root file
static TH2D *buildSystemMap(const string &file, const string &trk, const char *name)
{
    TH2D *hD = loadH2(file, "hData_" + trk);
    TH2D *hM = loadH2(file, "hMixData_" + trk);
    if (!hD || !hM) return nullptr;
    hD->Add(hM, -1.0);
    hD->Scale(0.5);
    divideByArea(hD);
    hD->SetName(name);
    delete hM;
    return hD;
}

// Bin-by-bin difference pPb - Pbp (same units as each map).
static TH2D *buildDiff(TH2D *hPPb, TH2D *hPbP, const char *name)
{
    TH2D *diff = (TH2D *)hPPb->Clone(name);
    diff->SetDirectory(nullptr);
    diff->Add(hPbP, -1.0);
    for (int ix = 1; ix <= diff->GetNbinsX(); ++ix)
        for (int iy = 1; iy <= diff->GetNbinsY(); ++iy)
            diff->SetBinError(ix, iy, 0.0);
    return diff;
}

// ── Drawing ────────────────────────────────────────────────────────────────

// topM=0.05, bottomM=0.14 → vertical fraction 0.81; leftM=0.14, rightM=0.18 → 0.68.
// Square frames: padW × 0.68 = padH × 0.81 → padH = padW × 68/81.
// Labels drawn inside frame at top-left in black. kRainbow set globally in main().
static void drawPad(TVirtualPad *pad, TH2D *h,
                    const string &sysLine, const string &kinLine,
                    const string &zTitle)
{
    pad->cd();
    pad->SetTopMargin(0.12);
    pad->SetRightMargin(0.18);
    pad->SetLeftMargin(0.14);
    pad->SetBottomMargin(0.14);

    h->GetXaxis()->SetTitle("#Delta y_{ch,Z}");
    h->GetYaxis()->SetTitle("#Delta#phi_{ch,Z}");
    h->GetZaxis()->SetTitle(zTitle.c_str());
    h->GetXaxis()->SetTitleSize(0.052); h->GetXaxis()->SetLabelSize(0.048);
    h->GetYaxis()->SetTitleSize(0.052); h->GetYaxis()->SetLabelSize(0.048);
    h->GetZaxis()->SetTitleSize(0.042); h->GetZaxis()->SetLabelSize(0.040);
    h->GetYaxis()->SetTitleOffset(1.10);
    h->GetZaxis()->SetTitleOffset(1.40);

    h->Draw("COLZ");

    TLatex tex; tex.SetNDC();
    tex.SetTextColor(kBlack);
    if (!sysLine.empty()) {
        tex.SetTextFont(62); tex.SetTextSize(0.050);
        tex.DrawLatex(0.14, 0.92, sysLine.c_str());
    }
    if (!kinLine.empty()) {
        tex.SetTextFont(42); tex.SetTextSize(0.040);
        tex.DrawLatex(0.42, 0.92, kinLine.c_str());
    }
}

// ── Label helpers ──────────────────────────────────────────────────────────

static string zLabel(const string &z)
{
    if (z == "0_500")  return "0 < p_{T}^{Z} < 500 GeV";
    if (z == "0_30")   return "0 < p_{T}^{Z} < 30 GeV";
    if (z == "30_500") return "30 < p_{T}^{Z} < 500 GeV";
    return z;
}
static string tLabel(const string &t)
{
    if (t == "0.5_15") return "0.5 < p_{T}^{trk} < 15 GeV";
    if (t == "0.5_2")  return "0.5 < p_{T}^{trk} < 2 GeV";
    if (t == "2_4")    return "2 < p_{T}^{trk} < 4 GeV";
    if (t == "4_15")   return "4 < p_{T}^{trk} < 15 GeV";
    return t;
}

// ── main ───────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string tag      = CL.Get("Tag", "ZV9_trkV28_nmix10_bin12x12_20260507");
    const string inputDir = CL.Get("InputDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outputDir = CL.Get("OutputDir", "plots/2d_pPbPbp_comparison");
    const string zpt      = CL.Get("ZPtRange", "0_500");
    const string trkStr   = CL.Get("TrkPtRanges", "0.5_15");
    const string outName  = CL.Get("OutputName", "2d_pPbPbp");
    const double diffZMax = CL.GetDouble("DiffZMax", -1);

    // Parse trkPT list
    vector<string> trkRanges;
    { istringstream ss(trkStr); string tok;
      while (getline(ss, tok, ',')) if (!tok.empty()) trkRanges.push_back(tok); }
    int nT = (int)trkRanges.size();

    gSystem->mkdir(outputDir.c_str(), true);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kRainbow);
    gStyle->SetNumberContours(99);

    string pPbFile = inputDir + "/pPb_trkResidual_" + tag + "_ZPT" + zpt + "-nosub.root";
    string PbPFile = inputDir + "/PbP_trkResidual_" + tag + "_ZPT" + zpt + "-nosub.root";

    // Build maps
    vector<TH2D *> hPPb(nT), hPbP(nT), hDiff(nT);
    for (int iT = 0; iT < nT; ++iT) {
        const string &trk = trkRanges[iT];
        hPPb[iT]  = buildSystemMap(pPbFile, trk, ("hPPb_"  + zpt + "_" + trk).c_str());
        hPbP[iT]  = buildSystemMap(PbPFile, trk, ("hPbP_"  + zpt + "_" + trk).c_str());
        if (!hPPb[iT] || !hPbP[iT]) {
            cerr << "Failed for ZPT=" << zpt << " trkPT=" << trk << endl; return 1;
        }
        hDiff[iT] = buildDiff(hPPb[iT], hPbP[iT], ("hDiff_" + zpt + "_" + trk).c_str());
    }

    // Per-row z-ranges: shared for pPb+Pbp; difference symmetric about 0
    for (int iT = 0; iT < nT; ++iT) {
        double lo = min(hPPb[iT]->GetMinimum(), hPbP[iT]->GetMinimum());
        double hi = max(hPPb[iT]->GetMaximum(), hPbP[iT]->GetMaximum());
        hPPb[iT]->GetZaxis()->SetRangeUser(lo, hi);
        hPbP[iT]->GetZaxis()->SetRangeUser(lo, hi);

        double dAbsMax = (diffZMax > 0) ? diffZMax
            : max(fabs(hDiff[iT]->GetMinimum()), fabs(hDiff[iT]->GetMaximum()));
        if (dAbsMax < 1e-6) dAbsMax = 0.01;
        hDiff[iT]->GetZaxis()->SetRangeUser(-dAbsMax, dAbsMax);

        printf("  [ZPT=%s trkPT=%s] data z=[%.4f,%.4f]  diff=[%.4f,%.4f]\n",
               zpt.c_str(), trkRanges[iT].c_str(), lo, hi, -dAbsMax, dAbsMax);
    }

    // Canvas: nT rows × 3 cols (pPb | Pbp | diff)
    // Square condition: padW × 0.68 = padH × 0.74 → padH = padW × 68/74 ≈ padW × 0.919
    // padW=500, padH=460: frame 340 × 340.4 px (square)
    const int padW = 500, padH = 460;
    TCanvas *c = new TCanvas("cComp", "", 3 * padW, nT * padH);
    c->Divide(3, nT, 0, 0);

    for (int iT = 0; iT < nT; ++iT) {
        string kin = zLabel(zpt) + ", " + tLabel(trkRanges[iT]);
        const string dataTitle = "1/N_{Z} d^{2}N_{ch}/d(#Deltay)d(#Delta#phi)";
        drawPad(c->cd(iT * 3 + 1), hPPb[iT],  "pPb 8.16 TeV",   kin, dataTitle);
        drawPad(c->cd(iT * 3 + 2), hPbP[iT],  "Pbp 8.16 TeV",   kin, dataTitle);
        drawPad(c->cd(iT * 3 + 3), hDiff[iT], "pPb #minus Pbp", kin, dataTitle);
    }

    string path = outputDir + "/" + outName + ".pdf";
    c->SaveAs(path.c_str());
    cout << "Saved: " << path << endl;
    delete c;

    for (auto *h : hPPb)  delete h;
    for (auto *h : hPbP)  delete h;
    for (auto *h : hDiff) delete h;
    return 0;
}
