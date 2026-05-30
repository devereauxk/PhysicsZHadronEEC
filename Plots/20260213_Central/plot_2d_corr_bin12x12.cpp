#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TMath.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

// ── Utilities ──────────────────────────────────────────────────────────────

static vector<string> split(const string &s, char d)
{
    vector<string> out;
    istringstream ss(s);
    string tok;
    while (getline(ss, tok, d)) if (!tok.empty()) out.push_back(tok);
    return out;
}

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

static TH1D *loadH1(const string &fname, const string &key)
{
    TFile *f = TFile::Open(fname.c_str(), "READ");
    if (!f || f->IsZombie()) { cerr << "Cannot open " << fname << endl; return nullptr; }
    TH1D *h = (TH1D *)f->Get(key.c_str());
    if (!h) { cerr << "Missing " << key << " in " << fname << endl; f->Close(); return nullptr; }
    h = (TH1D *)h->Clone((key + "_cl").c_str());
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

// ── Map builders ───────────────────────────────────────────────────────────

// pp: result = (hData − hMixData) × 0.5 / binArea
static TH2D *buildPPMap(const string &ppFile, const string &trk, const string &name)
{
    TH2D *hD = loadH2(ppFile, "hData_" + trk);
    TH2D *hM = loadH2(ppFile, "hMixData_" + trk);
    if (!hD || !hM) return nullptr;
    hD->Add(hM, -1.0);
    hD->Scale(0.5);
    divideByArea(hD);
    TH2D *sub = extractPhysical(hD, name.c_str());
    delete hD; delete hM;
    return sub;
}

// pPb combined: undo N_Z normalization, sum pPb+PbP, renorm, subtract, ×0.5, /area
static TH2D *buildPbPbMap(const string &pPbFile, const string &PbPFile,
                           const string &trk, const string &name)
{
    TH2D *sA = loadH2(pPbFile, "hData_" + trk);
    TH2D *mA = loadH2(pPbFile, "hMixData_" + trk);
    TH1D *nA = loadH1(pPbFile, "hNZData_" + trk);
    TH1D *nmA= loadH1(pPbFile, "hNZMixData_" + trk);
    TH2D *sB = loadH2(PbPFile, "hData_" + trk);
    TH2D *mB = loadH2(PbPFile, "hMixData_" + trk);
    TH1D *nB = loadH1(PbPFile, "hNZData_" + trk);
    TH1D *nmB= loadH1(PbPFile, "hNZMixData_" + trk);
    if (!sA||!mA||!nA||!nmA||!sB||!mB||!nB||!nmB) return nullptr;

    double NA = nA->GetBinContent(1), NmA = nmA->GetBinContent(1);
    double NB = nB->GetBinContent(1), NmB = nmB->GetBinContent(1);
    sA->Scale(NA); mA->Scale(NmA);
    sB->Scale(NB); mB->Scale(NmB);
    sA->Add(sB); mA->Add(mB);
    sA->Scale(1.0 / (NA + NB));
    mA->Scale(1.0 / (NmA + NmB));
    sA->Add(mA, -1.0);
    sA->Scale(0.5);
    divideByArea(sA);
    TH2D *sub = extractPhysical(sA, name.c_str());
    delete sA; delete mA; delete nA; delete nmA;
    delete sB; delete mB; delete nB; delete nmB;
    return sub;
}

// ── Drawing ────────────────────────────────────────────────────────────────

// Set axis labels and draw COLZ. Labels drawn inside the frame at top-left corner.
// topM=0.05, bottomM=0.14 → vertical frame fraction 0.81; leftM=0.14, rightM=0.18 → 0.68.
// Square frame condition: padW × 0.68 = padH × 0.81 → padH = padW × 68/81.
static void drawPad(TVirtualPad *pad, TH2D *h,
                    const string &sysLine, const string &kinLine)
{
    pad->cd();
    pad->SetTopMargin(0.05);
    pad->SetRightMargin(0.18);
    pad->SetLeftMargin(0.14);
    pad->SetBottomMargin(0.14);

    h->GetXaxis()->SetTitle("#Delta y_{ch,Z}");
    h->GetYaxis()->SetTitle("#Delta#phi_{ch,Z}");
    h->GetZaxis()->SetTitle("1/N_{Z} d^{2}N_{ch}/d(#Deltay)d(#Delta#phi)");
    h->GetXaxis()->SetTitleSize(0.058); h->GetXaxis()->SetLabelSize(0.052);
    h->GetYaxis()->SetTitleSize(0.058); h->GetYaxis()->SetLabelSize(0.052);
    h->GetZaxis()->SetTitleSize(0.046); h->GetZaxis()->SetLabelSize(0.043);
    h->GetYaxis()->SetTitleOffset(1.05);
    h->GetZaxis()->SetTitleOffset(1.35);
    h->Draw("COLZ");

    // Draw labels inside the frame at top-left (white text for rainbow palette)
    TLatex tex;
    tex.SetNDC();
    tex.SetTextColor(kWhite);
    if (!sysLine.empty()) {
        tex.SetTextFont(62); tex.SetTextSize(0.058);
        tex.DrawLatex(0.17, 0.87, sysLine.c_str());
    }
    if (!kinLine.empty()) {
        tex.SetTextFont(42); tex.SetTextSize(0.048);
        tex.DrawLatex(0.17, 0.78, kinLine.c_str());
    }
}

// Kinematic label helpers
static string zLabel(const string &z) {
    if (z == "0_500")  return "0 < p_{T}^{Z} < 500 GeV";
    if (z == "0_30")   return "0 < p_{T}^{Z} < 30 GeV";
    if (z == "30_500") return "30 < p_{T}^{Z} < 500 GeV";
    return z;
}
static string tLabel(const string &t) {
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

    const string pPbTag   = CL.Get("PbPbTag",  "ZV9_trkV28_nmix10_bin12x12_20260507");
    const string ppTag    = CL.Get("PPTag",     "EEV5_ZV9_trkV28_nmix10_bin12x12_20260507");
    const string inputDir = CL.Get("InputDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outputDir = CL.Get("OutputDir", "plots/2d_corr_bin12x12");
    // Mode: "inclusive" (pp+pPb side-by-side) or "scan" (separate pp and pPb composites)
    const string mode     = CL.Get("Mode", "inclusive");
    const string zptStr   = CL.Get("ZPtRanges",  "0_500");
    const string trkStr   = CL.Get("TrkPtRanges", "0.5_15");
    const string outName  = CL.Get("OutputName", "2d_corr");

    vector<string> zptRanges = split(zptStr, ',');
    vector<string> trkRanges = split(trkStr, ',');
    int nZ = (int)zptRanges.size(), nT = (int)trkRanges.size();
    int nPanels = nZ * nT;

    gSystem->mkdir(outputDir.c_str(), true);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kRainbow);
    gStyle->SetNumberContours(99);

    // Build all pp and pPb maps
    vector<TH2D *> hPP(nPanels, nullptr), hPPb(nPanels, nullptr);
    for (int iZ = 0; iZ < nZ; ++iZ) {
        for (int iT = 0; iT < nT; ++iT) {
            int idx = iZ * nT + iT;
            string zpt = zptRanges[iZ], trk = trkRanges[iT];
            string ppFile  = inputDir + "/pp_trkResidual_"  + ppTag  + "_ZPT" + zpt + "-nosub.root";
            string pPbFile = inputDir + "/pPb_trkResidual_" + pPbTag + "_ZPT" + zpt + "-nosub.root";
            string PbPFile = inputDir + "/PbP_trkResidual_" + pPbTag + "_ZPT" + zpt + "-nosub.root";
            hPP[idx]  = buildPPMap(ppFile, trk, ("hPP_"  + zpt + "_" + trk).c_str());
            hPPb[idx] = buildPbPbMap(pPbFile, PbPFile, trk, ("hPPb_" + zpt + "_" + trk).c_str());
            if (!hPP[idx] || !hPPb[idx]) {
                cerr << "Failed to build maps for ZPT=" << zpt << " trkPT=" << trk << endl;
                return 1;
            }
        }
    }

    // Compute shared z-range per kinematic panel (max bounds across pp and pPb)
    vector<double> zLo(nPanels), zHi(nPanels);
    for (int i = 0; i < nPanels; ++i) {
        zLo[i] = min(hPP[i]->GetMinimum(), hPPb[i]->GetMinimum());
        zHi[i] = max(hPP[i]->GetMaximum(), hPPb[i]->GetMaximum());
        hPP[i]->GetZaxis()->SetRangeUser(zLo[i], zHi[i]);
        hPPb[i]->GetZaxis()->SetRangeUser(zLo[i], zHi[i]);
        printf("  [ZPT=%s trkPT=%s] z-range [%.4f, %.4f]\n",
               zptRanges[i / nT].c_str(), trkRanges[i % nT].c_str(), zLo[i], zHi[i]);
    }

    // ── Inclusive mode: 2-panel side-by-side (pp | pPb) ─────────────────────
    if (mode == "inclusive") {
        // Single kinematic bin expected
        if (nPanels != 1) { cerr << "Inclusive mode expects exactly 1 ZPT and 1 trkPT." << endl; return 1; }
        string kinLine = zLabel(zptRanges[0]) + ", " + tLabel(trkRanges[0]);

        // padH = padW × 68/81 so that frame (0.68 × padW = 0.81 × padH) is square
        TCanvas *c = new TCanvas("cInc", "cInc", 1020, 428);
        c->Divide(2, 1, 0, 0);
        drawPad(c->cd(1), hPP[0],  "pp 8.16 TeV",  kinLine);
        drawPad(c->cd(2), hPPb[0], "pPb 8.16 TeV", kinLine);

        string path = outputDir + "/" + outName + ".pdf";
        c->SaveAs(path.c_str());
        cout << "Saved: " << path << endl;
        delete c;
    }

    // ── Scan mode: two 2×3 composites (pp and pPb) ───────────────────────────
    else if (mode == "scan") {
        // Expect nZ rows × nT cols
        // padH = padW × 68/81 so that frame (0.68 × padW = 0.81 × padH) is square
        int padW = 510, padH = 428;
        int cW = nT * padW, cH = nZ * padH;

        auto drawComposite = [&](vector<TH2D *> &maps, const string &sysLabel,
                                  const string &outSuffix) {
            TCanvas *c = new TCanvas(("cScan_" + outSuffix).c_str(), "", cW, cH);
            c->Divide(nT, nZ, 0, 0);
            for (int iZ = 0; iZ < nZ; ++iZ) {
                for (int iT = 0; iT < nT; ++iT) {
                    int idx = iZ * nT + iT;
                    TVirtualPad *pad = c->cd(idx + 1);
                    // System label on every pad (inside frame, so not visually disruptive)
                    string sl = sysLabel;
                    string kl = zLabel(zptRanges[iZ]) + ", " + tLabel(trkRanges[iT]);
                    drawPad(pad, maps[idx], sl, kl);
                }
            }
            string path = outputDir + "/" + outName + "_" + outSuffix + ".pdf";
            c->SaveAs(path.c_str());
            cout << "Saved: " << path << endl;
            delete c;
        };

        drawComposite(hPP,  "pp 8.16 TeV",  "pp");
        drawComposite(hPPb, "pPb 8.16 TeV", "ppb");
    }

    else { cerr << "Unknown mode: " << mode << endl; return 1; }

    for (auto *h : hPP)  delete h;
    for (auto *h : hPPb) delete h;
    return 0;
}
