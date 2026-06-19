#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLine.h"
#include "TTree.h"

#include "CommandLine.h"
#include "KylesPlotting.h"
#include "SetStyle.h"

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

void ReverseEtaBins(TH1D *h) {
    int n = h->GetNbinsX();
    vector<double> vals(n), errs(n);
    for (int i = 0; i < n; i++) {
        vals[i] = h->GetBinContent(i+1);
        errs[i] = h->GetBinError(i+1);
    }
    for (int i = 0; i < n; i++) {
        h->SetBinContent(i+1, vals[n-1-i]);
        h->SetBinError(i+1, errs[n-1-i]);
    }
}

void ApplyDiagonalJackknifeErrors(TFile *file, const string &trkPtRange,
    TH1D *hEta, TH1D *hPhi)
{
    if (!file) return;
    TTree *tree = (TTree *)file->Get(Form("JackknifeProjection%s", trkPtRange.c_str()));
    if (!tree) return;

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

    int nBins = hEta->GetNbinsX();
    double totalSNZ = 0, totalMNZ = 0;
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
        hEta->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varEta) / hEta->GetBinWidth(b + 1));
        hPhi->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varPhi) / hPhi->GetBinWidth(b + 1));
    }
}

pair<double,double> AutoYRange(vector<TH1D*> hists, double padLo = 0.10, double padHi = 0.35) {
    double mn = 1e30, mx = -1e30;
    for (auto *h : hists)
        for (int b = 1; b <= h->GetNbinsX(); b++) {
            double v = h->GetBinContent(b);
            double e = h->GetBinError(b);
            mn = min(mn, v - e);
            mx = max(mx, v + e);
        }
    double range = mx - mn;
    return {mn - range * padLo, mx + range * padHi};
}

bool IsSolidBin(const string &obs, int bin) {
    if (obs == "DeltaEta")
        return bin >= 7;
    return (bin >= 4 && bin <= 9);
}

void DrawSimplePlot(TH1D *frame, vector<TH1D*> hists,
    vector<Int_t> colors, vector<Int_t> markers, vector<string> labels,
    const string &xLabel, const string &yLabel,
    double xLo, double xHi, double yLo, double yHi,
    const string &annotation, const string &outputPath,
    double legX1 = 0.55, double legY1 = 0.70, double legX2 = 0.92, double legY2 = 0.90,
    vector<double> vlines = {})
{
    TCanvas c("c", "", 600, 500);
    c.SetLeftMargin(0.16);
    c.SetRightMargin(0.05);
    c.SetTopMargin(0.08);
    c.SetBottomMargin(0.13);

    frame->GetXaxis()->SetTitle(xLabel.c_str());
    frame->GetYaxis()->SetTitle(yLabel.c_str());
    frame->GetXaxis()->SetRangeUser(xLo, xHi);
    frame->GetYaxis()->SetRangeUser(yLo, yHi);
    frame->GetYaxis()->SetTitleOffset(1.7);
    frame->Draw("AXIS");

    for (double xv : vlines) {
        TLine *line = new TLine(xv, yLo, xv, yHi);
        line->SetLineStyle(2);
        line->SetLineColor(kGray+2);
        line->Draw();
    }

    for (size_t i = 0; i < hists.size(); i++) {
        hists[i]->SetLineColor(colors[i]);
        hists[i]->SetMarkerColor(colors[i]);
        hists[i]->SetMarkerStyle(markers[i]);
        hists[i]->SetMarkerSize(1.0);
        hists[i]->Draw("EP SAME");
    }

    TLegend leg(legX1, legY1, legX2, legY2);
    leg.SetBorderSize(0);
    leg.SetFillStyle(0);
    leg.SetTextFont(42);
    leg.SetTextSize(0.035);
    for (size_t i = 0; i < hists.size(); i++)
        leg.AddEntry(hists[i], labels[i].c_str(), "lp");
    leg.Draw();

    AddCMSHeader((TPad*)gPad, "Internal", false);
    AddUPCHeader((TPad*)gPad, "8.16 TeV", "pPb");

    if (!annotation.empty()) {
        TLatex lx;
        lx.SetNDC(); lx.SetTextFont(42); lx.SetTextSize(0.030);
        lx.DrawLatex(0.19, 0.84, annotation.c_str());
    }

    c.Update();
    c.SaveAs(outputPath.c_str());
}

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);
    gROOT->SetBatch(kTRUE);
    SetThesisStyle();
    gStyle->SetOptStat(0);

    string baseDir    = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    string tag        = CL.Get("Tag", "ZV10_trkV29_nmix10");
    string zptRange   = CL.Get("ZPTRange", "0_500");
    string trkRange   = CL.Get("TrackPTRange", "0.5_15");
    string outputBase = CL.Get("OutputBase", "plots/pPbPbp_unsymmetrized/pPbPbp");

    string pPbBase = baseDir + "/pPb_trkResidual_" + tag + "_ZPT" + zptRange;
    string PbPBase = baseDir + "/PbP_trkResidual_" + tag + "_ZPT" + zptRange;

    TFile *fpPb   = TFile::Open((pPbBase + "-result.root").c_str());
    TFile *fPbP   = TFile::Open((PbPBase + "-result.root").c_str());
    TFile *fpPbNS = TFile::Open((pPbBase + "-nosub.root").c_str());
    TFile *fPbPNS = TFile::Open((PbPBase + "-nosub.root").c_str());

    if (!fpPb || !fPbP || !fpPbNS || !fPbPNS) {
        cerr << "Cannot open input files" << endl;
        return 1;
    }

    double NZ_pPb = ((TH1D*)fpPbNS->Get(("hNZData_" + trkRange).c_str()))->GetBinContent(1);
    double NZ_PbP = ((TH1D*)fPbPNS->Get(("hNZData_" + trkRange).c_str()))->GetBinContent(1);
    double NZ_tot = NZ_pPb + NZ_PbP;
    double w1 = NZ_pPb / NZ_tot;
    double w2 = NZ_PbP / NZ_tot;
    cout << "pPb N_Z = " << NZ_pPb << "  PbP N_Z = " << NZ_PbP
         << "  weights: " << w1 << " / " << w2 << endl;

    string etaName = "DeltaEta_Result" + trkRange;
    string phiName = "DeltaPhi_Result" + trkRange;

    // --- Load and prepare histograms ---
    TH1D *hEta_pPb = (TH1D*)((TH1D*)fpPb->Get(etaName.c_str()))->Clone("hEta_pPb");
    TH1D *hPhi_pPb = (TH1D*)((TH1D*)fpPb->Get(phiName.c_str()))->Clone("hPhi_pPb");
    TH1D *hEta_PbP = (TH1D*)((TH1D*)fPbP->Get(etaName.c_str()))->Clone("hEta_PbP");
    TH1D *hPhi_PbP = (TH1D*)((TH1D*)fPbP->Get(phiName.c_str()))->Clone("hPhi_PbP");
    hEta_pPb->SetDirectory(nullptr); hPhi_pPb->SetDirectory(nullptr);
    hEta_PbP->SetDirectory(nullptr); hPhi_PbP->SetDirectory(nullptr);

    ApplyDiagonalJackknifeErrors(fpPb, trkRange, hEta_pPb, hPhi_pPb);
    ApplyDiagonalJackknifeErrors(fPbP, trkRange, hEta_PbP, hPhi_PbP);

    hEta_pPb->Scale(0.5); hPhi_pPb->Scale(0.5);
    hEta_PbP->Scale(0.5); hPhi_PbP->Scale(0.5);

    // Raw Pbp: undo the FlipDeltaEta for DeltaEta; DeltaPhi unaffected
    TH1D *hEta_PbP_raw = (TH1D*)hEta_PbP->Clone("hEta_PbP_raw");
    TH1D *hPhi_PbP_raw = (TH1D*)hPhi_PbP->Clone("hPhi_PbP_raw");
    ReverseEtaBins(hEta_PbP_raw);

    // Combined
    auto CombineHists = [&](TH1D *h1, TH1D *h2, const char *name) -> TH1D* {
        TH1D *hc = (TH1D*)h1->Clone(name);
        for (int b = 1; b <= hc->GetNbinsX(); b++) {
            hc->SetBinContent(b, w1 * h1->GetBinContent(b) + w2 * h2->GetBinContent(b));
            hc->SetBinError(b, sqrt(w1*w1 * pow(h1->GetBinError(b),2)
                                  + w2*w2 * pow(h2->GetBinError(b),2)));
        }
        return hc;
    };

    TH1D *hEta_comb = CombineHists(hEta_pPb, hEta_PbP, "hEta_comb");
    TH1D *hPhi_comb = CombineHists(hPhi_pPb, hPhi_PbP, "hPhi_comb");

    // Symmetrized
    TH1D *hEta_sym = (TH1D*)hEta_comb->Clone("hEta_sym");
    TH1D *hPhi_sym = (TH1D*)hPhi_comb->Clone("hPhi_sym");
    Symmetrize1DEta(hEta_sym);
    Symmetrize1DPhi(hPhi_sym);

    // --- Global y-ranges per observable ---
    auto [yLoEta, yHiEta] = AutoYRange({hEta_pPb, hEta_PbP, hEta_PbP_raw,
                                         hEta_comb, hEta_sym});
    auto [yLoPhi, yHiPhi] = AutoYRange({hPhi_pPb, hPhi_PbP, hPhi_PbP_raw,
                                         hPhi_comb, hPhi_sym});

    Int_t colBlue   = cmsBlue;
    Int_t colGreen  = kGreen + 2;
    Int_t colOrange = kOrange + 1;
    Int_t colPurple = kViolet + 1;

    struct ObsInfo {
        string name; string xLabel; double xLo, xHi, yLo, yHi;
        TH1D *h_pPb, *h_PbP, *h_PbP_raw, *h_comb, *h_sym;
    };
    vector<ObsInfo> obs = {
        {"DeltaEta", "#Deltay_{ch,Z}", -3.87, 3.87, yLoEta, yHiEta,
         hEta_pPb, hEta_PbP, hEta_PbP_raw, hEta_comb, hEta_sym},
        {"DeltaPhi", "#Delta#phi_{ch,Z}", -1.5708, 4.7124, yLoPhi, yHiPhi,
         hPhi_pPb, hPhi_PbP, hPhi_PbP_raw, hPhi_comb, hPhi_sym}
    };

    for (auto &o : obs) {
        string yLabel = "d#LT#DeltaN_{ch}#GT/d" + o.xLabel;
        TH1D *frame = (TH1D*)o.h_pPb->Clone("frame");
        frame->Reset();

        bool isPhi = (o.name == "DeltaPhi");
        vector<double> vlines = isPhi ? vector<double>{0.0, M_PI} : vector<double>{0.0};
        double lx1 = isPhi ? 0.19 : 0.55;
        double ly1 = isPhi ? 0.42 : 0.70;
        double lx2 = isPhi ? 0.55 : 0.92;
        double ly2 = isPhi ? 0.58 : 0.90;

        // ======== Plot 0: Raw (no eta flip for Pbp) ========
        DrawSimplePlot(frame, {o.h_pPb, o.h_PbP_raw},
            {colBlue, colGreen}, {20, 21}, {"pPb", "Pbp (no #Deltay flip)"},
            o.xLabel, yLabel, o.xLo, o.xHi, o.yLo, o.yHi,
            "Raw (lab frame Pbp)",
            Form("%s-raw-%s.pdf", outputBase.c_str(), o.name.c_str()),
            lx1, ly1, lx2, ly2, vlines);

        // ======== Plot 1: Separate (common CM) ========
        DrawSimplePlot(frame, {o.h_pPb, o.h_PbP},
            {colBlue, colGreen}, {20, 21}, {"pPb", "Pbp"},
            o.xLabel, yLabel, o.xLo, o.xHi, o.yLo, o.yHi,
            "Unsymmetrized",
            Form("%s-separate-%s.pdf", outputBase.c_str(), o.name.c_str()),
            lx1, ly1, lx2, ly2, vlines);

        // ======== Plot 2: Combined unsymmetrized ========
        DrawSimplePlot(frame, {o.h_comb},
            {colOrange}, {20}, {"pPb + Pbp combined"},
            o.xLabel, yLabel, o.xLo, o.xHi, o.yLo, o.yHi,
            "Unsymmetrized",
            Form("%s-combined-%s.pdf", outputBase.c_str(), o.name.c_str()),
            isPhi ? 0.19 : 0.45, isPhi ? 0.46 : 0.78,
            isPhi ? 0.55 : 0.92, isPhi ? 0.55 : 0.90, vlines);

        // ======== Plot 3: Symmetrized, solid/open markers, purple ========
        {
            TCanvas c("c", "", 600, 500);
            c.SetLeftMargin(0.16);
            c.SetRightMargin(0.05);
            c.SetTopMargin(0.08);
            c.SetBottomMargin(0.13);

            TH1D *fr = (TH1D*)frame->Clone("fr3");
            fr->GetXaxis()->SetTitle(o.xLabel.c_str());
            fr->GetYaxis()->SetTitle(yLabel.c_str());
            fr->GetXaxis()->SetRangeUser(o.xLo, o.xHi);
            fr->GetYaxis()->SetRangeUser(o.yLo, o.yHi);
            fr->GetYaxis()->SetTitleOffset(1.7);
            fr->Draw("AXIS");

            for (double xv : vlines) {
                TLine *line = new TLine(xv, o.yLo, xv, o.yHi);
                line->SetLineStyle(2);
                line->SetLineColor(kGray+2);
                line->Draw();
            }

            TGraphErrors *gSolid = new TGraphErrors();
            TGraphErrors *gOpen  = new TGraphErrors();
            for (int b = 1; b <= o.h_sym->GetNbinsX(); b++) {
                double x  = o.h_sym->GetBinCenter(b);
                double y  = o.h_sym->GetBinContent(b);
                double ey = o.h_sym->GetBinError(b);
                TGraphErrors *g = IsSolidBin(o.name, b) ? gSolid : gOpen;
                int n = g->GetN();
                g->SetPoint(n, x, y);
                g->SetPointError(n, 0, ey);
            }

            gSolid->SetMarkerStyle(20); gSolid->SetMarkerSize(1.0);
            gSolid->SetMarkerColor(colPurple); gSolid->SetLineColor(colPurple);
            gSolid->Draw("P SAME");

            gOpen->SetMarkerStyle(24); gOpen->SetMarkerSize(1.0);
            gOpen->SetMarkerColor(colPurple); gOpen->SetLineColor(colPurple);
            gOpen->Draw("P SAME");

            double sl1 = isPhi ? 0.19 : 0.55;
            double sl2 = isPhi ? 0.55 : 0.92;
            double sly1 = isPhi ? 0.42 : 0.74;
            double sly2 = isPhi ? 0.58 : 0.90;
            TLegend leg(sl1, sly1, sl2, sly2);
            leg.SetBorderSize(0);
            leg.SetFillStyle(0);
            leg.SetTextFont(42);
            leg.SetTextSize(0.035);
            leg.AddEntry(gSolid, "Measured bins", "lp");
            leg.AddEntry(gOpen,  "Reflected bins", "lp");
            leg.Draw();

            AddCMSHeader((TPad*)gPad, "Internal", false);
            AddUPCHeader((TPad*)gPad, "8.16 TeV", "pPb");

            TLatex lx;
            lx.SetNDC(); lx.SetTextFont(42); lx.SetTextSize(0.030);
            lx.DrawLatex(0.19, 0.84, "Symmetrized pPb + Pbp");

            c.Update();
            c.SaveAs(Form("%s-symmetrized-%s.pdf", outputBase.c_str(), o.name.c_str()));
        }
    }

    return 0;
}
