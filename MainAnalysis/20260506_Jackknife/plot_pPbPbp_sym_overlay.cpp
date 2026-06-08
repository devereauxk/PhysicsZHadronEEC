// plot_pPbPbp_sym_overlay.cpp
// Produces two-panel overlay plots comparing signed+symmetrized pPb vs Pbp results.
// One PDF per observable (DeltaEta, DeltaPhi).

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TPad.h>
#include <TStyle.h>
#include <TSystem.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

// ── Mirror functions (0-indexed, 12 bins) ──────────────────────────────────────
int etaMirror(int i) { return 11 - i; }
int phiMirror(int j) { return (j < 6) ? (5 - j) : (17 - j); }

// ── Symmetrize histogram (returns new clone) ────────────────────────────────────
TH1D *symmetrize(TH1D *h, bool useEta, const string &name)
{
    TH1D *hs = (TH1D *)h->Clone(name.c_str());
    hs->SetDirectory(nullptr);
    int n = hs->GetNbinsX();
    for (int i = 0; i < n; ++i) {
        int mi = useEta ? etaMirror(i) : phiMirror(i);
        double v  = 0.5 * (h->GetBinContent(i + 1) + h->GetBinContent(mi + 1));
        double ea = h->GetBinError(i + 1), eb = h->GetBinError(mi + 1);
        double e  = 0.5 * sqrt(ea * ea + eb * eb);
        hs->SetBinContent(i + 1, v);
        hs->SetBinError(i + 1, e);
    }
    return hs;
}

// ── Load and clone histogram ────────────────────────────────────────────────────
TH1D *loadHist(TFile &f, const string &key, const string &name)
{
    TH1D *h = (TH1D *)f.Get(key.c_str());
    if (!h) { cerr << "Missing: " << key << " in " << f.GetName() << endl; return nullptr; }
    h = (TH1D *)h->Clone(name.c_str());
    h->SetDirectory(nullptr);
    return h;
}

// ── Style helpers ───────────────────────────────────────────────────────────────
void setStyle()
{
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetFrameLineWidth(1);
    gStyle->SetEndErrorSize(3);
}

// ── Draw one observable's overlay + ratio pad ───────────────────────────────────
void drawOverlay(const string &outPath, TH1D *hpPb, TH1D *hPbP,
                 const string &xTitle, const string &obsLabel)
{
    const double scale = 0.5;
    hpPb->Scale(scale);
    hPbP->Scale(scale);

    // color / style
    const int colpPb = kAzure - 2, colPbP = kOrange + 7;
    hpPb->SetMarkerStyle(20); hpPb->SetMarkerColor(colpPb);
    hpPb->SetLineColor(colpPb); hpPb->SetMarkerSize(0.9);
    hPbP->SetMarkerStyle(24); hPbP->SetMarkerColor(colPbP);
    hPbP->SetLineColor(colPbP); hPbP->SetMarkerSize(0.9);

    // y-range: pad symmetrically around zero
    double yMax = 0;
    for (int b = 1; b <= hpPb->GetNbinsX(); ++b) {
        yMax = max(yMax, fabs(hpPb->GetBinContent(b)) + hpPb->GetBinError(b));
        yMax = max(yMax, fabs(hPbP->GetBinContent(b)) + hPbP->GetBinError(b));
    }
    yMax *= 1.35;
    hpPb->SetMinimum(-yMax);
    hpPb->SetMaximum(yMax);

    // Difference histogram
    TH1D *hDiff = (TH1D *)hpPb->Clone("hDiff");
    hDiff->SetDirectory(nullptr);
    hDiff->Add(hPbP, -1.0);
    // difference errors: sqrt(err_pPb^2 + err_PbP^2)
    for (int b = 1; b <= hDiff->GetNbinsX(); ++b) {
        double ep = hpPb->GetBinError(b), eq = hPbP->GetBinError(b);
        hDiff->SetBinError(b, sqrt(ep * ep + eq * eq));
    }
    hDiff->SetMarkerStyle(20); hDiff->SetMarkerColor(kBlack);
    hDiff->SetLineColor(kBlack); hDiff->SetMarkerSize(0.9);

    double dMax = 0;
    for (int b = 1; b <= hDiff->GetNbinsX(); ++b)
        dMax = max(dMax, fabs(hDiff->GetBinContent(b)) + hDiff->GetBinError(b));
    dMax = max(dMax * 1.5, 1e-4);

    TCanvas *c = new TCanvas("c", "c", 700, 700);
    TPad *pTop = new TPad("pTop", "", 0, 0.32, 1, 1);
    TPad *pBot = new TPad("pBot", "", 0, 0,    1, 0.32);
    pTop->SetBottomMargin(0.015);
    pBot->SetTopMargin(0.025);
    pBot->SetBottomMargin(0.30);
    pTop->Draw(); pBot->Draw();

    // ── top pad ──
    pTop->cd();
    hpPb->GetXaxis()->SetLabelSize(0);
    hpPb->GetYaxis()->SetTitle("1/(N_{Z}) d^{2}N_{ch}/d#Delta y d#Delta#phi");
    hpPb->GetYaxis()->SetTitleSize(0.048);
    hpPb->GetYaxis()->SetTitleOffset(1.25);
    hpPb->Draw("PE");
    hPbP->Draw("PE SAME");

    TLine *zero = new TLine(hpPb->GetXaxis()->GetXmin(), 0,
                            hpPb->GetXaxis()->GetXmax(), 0);
    zero->SetLineStyle(2); zero->SetLineColor(kGray + 1); zero->Draw();

    TLegend *leg = new TLegend(0.18, 0.68, 0.55, 0.88);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.042);
    leg->AddEntry(hpPb, "pPb 8.16 TeV (symmetrized)", "pe");
    leg->AddEntry(hPbP, "Pbp 8.16 TeV (symmetrized)", "pe");
    leg->Draw();

    TLatex lat;
    lat.SetNDC(); lat.SetTextSize(0.044); lat.SetTextFont(42);
    lat.DrawLatex(0.18, 0.60, "0 < p_{T}^{Z} < 500 GeV");
    lat.DrawLatex(0.18, 0.54, "0.5 < p_{T}^{trk} < 15 GeV");
    lat.SetTextFont(62);
    lat.DrawLatex(0.60, 0.88, "CMS");
    lat.SetTextFont(42);
    lat.DrawLatex(0.60, 0.82, "#it{Preliminary}");

    // ── bottom pad ──
    pBot->cd();
    hDiff->SetTitle("");
    hDiff->SetMinimum(-dMax);
    hDiff->SetMaximum(dMax);
    hDiff->GetXaxis()->SetTitle(xTitle.c_str());
    hDiff->GetXaxis()->SetTitleSize(0.10);
    hDiff->GetXaxis()->SetLabelSize(0.09);
    hDiff->GetYaxis()->SetTitle("pPb #minus Pbp");
    hDiff->GetYaxis()->SetTitleSize(0.09);
    hDiff->GetYaxis()->SetTitleOffset(0.65);
    hDiff->GetYaxis()->SetLabelSize(0.08);
    hDiff->GetYaxis()->SetNdivisions(505);
    hDiff->Draw("PE");
    TLine *z2 = new TLine(hDiff->GetXaxis()->GetXmin(), 0,
                           hDiff->GetXaxis()->GetXmax(), 0);
    z2->SetLineStyle(2); z2->SetLineColor(kGray + 1); z2->Draw();

    c->SaveAs(outPath.c_str());
    cout << "Saved: " << outPath << endl;

    delete hDiff; delete c;
}

// ── main ────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string baseDir  = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outDir   = CL.Get("OutputDir", "output_signed_sym");
    const string trkRange = CL.Get("TrkRange",  "0.5_15");
    const string tag      = CL.Get("Tag",        "ZV10_trkV29_nmix10");

    const string pPbOvr = CL.Get("pPbFile", "");
    const string PbPOvr = CL.Get("PbPFile", "");
    const string pPbPath = pPbOvr.empty()
        ? (baseDir + "/pPb_trkResidual_" + tag + "_ZPT0_500-result.root") : pPbOvr;
    const string PbPPath = PbPOvr.empty()
        ? (baseDir + "/PbP_trkResidual_" + tag + "_ZPT0_500-result.root") : PbPOvr;

    TFile fpPb(pPbPath.c_str(), "READ");
    TFile fPbP(PbPPath.c_str(), "READ");
    if (fpPb.IsZombie() || fPbP.IsZombie()) {
        cerr << "Failed to open input files." << endl;
        return 1;
    }

    const string etaKey = "DeltaEta_Result" + trkRange;
    const string phiKey = "DeltaPhi_Result" + trkRange;

    TH1D *pPbEta = loadHist(fpPb, etaKey, "pPb_eta");
    TH1D *PbPEta = loadHist(fPbP, etaKey, "PbP_eta");
    TH1D *pPbPhi = loadHist(fpPb, phiKey, "pPb_phi");
    TH1D *PbPPhi = loadHist(fPbP, phiKey, "PbP_phi");

    if (!pPbEta || !PbPEta || !pPbPhi || !PbPPhi) {
        cerr << "Missing histograms." << endl;
        return 1;
    }

    TH1D *pPbEtaS = symmetrize(pPbEta, true,  "pPb_eta_sym");
    TH1D *PbPEtaS = symmetrize(PbPEta, true,  "PbP_eta_sym");
    TH1D *pPbPhiS = symmetrize(pPbPhi, false, "pPb_phi_sym");
    TH1D *PbPPhiS = symmetrize(PbPPhi, false, "PbP_phi_sym");

    setStyle();
    gSystem->mkdir(outDir.c_str(), true);

    // DeltaEta overlay
    drawOverlay(outDir + "/pPbPbp_signed_sym_ZPT0_500_trkPT" + trkRange + "-DeltaEta-overlay.pdf",
                pPbEtaS, PbPEtaS,
                "#Delta y_{ch,Z}", "DeltaEta");

    // DeltaPhi overlay
    drawOverlay(outDir + "/pPbPbp_signed_sym_ZPT0_500_trkPT" + trkRange + "-DeltaPhi-overlay.pdf",
                pPbPhiS, PbPPhiS,
                "#Delta#phi_{ch,Z}", "DeltaPhi");

    return 0;
}
