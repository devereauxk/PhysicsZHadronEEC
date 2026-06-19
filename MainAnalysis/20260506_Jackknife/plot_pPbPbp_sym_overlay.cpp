// plot_pPbPbp_sym_overlay.cpp
// Produces TDR-style overlay plots comparing signed+symmetrized pPb vs Pbp results.
// Uses PlotCMSDiffResultRegion from KylesPlotting.h (same style as result plots).

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TSystem.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "../../CommonCode/include/CommandLine.h"

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

    // Symmetrize
    TH1D *pPbEtaS = symmetrize(pPbEta, true,  "pPb_eta_sym");
    TH1D *PbPEtaS = symmetrize(PbPEta, true,  "PbP_eta_sym");
    TH1D *pPbPhiS = symmetrize(pPbPhi, false, "pPb_phi_sym");
    TH1D *PbPPhiS = symmetrize(PbPPhi, false, "PbP_phi_sym");

    // Apply 0.5 normalization
    pPbEtaS->Scale(0.5);
    PbPEtaS->Scale(0.5);
    pPbPhiS->Scale(0.5);
    PbPPhiS->Scale(0.5);

    gSystem->mkdir(outDir.c_str(), true);

    // Style vectors: pPb first (baseline=0), Pbp second
    vector<string> labels = {"pPb (8.16 TeV)", "Pbp (8.16 TeV)"};
    vector<Int_t> lineColors  = {kAzure - 2, kOrange + 7};
    vector<Int_t> lineStyles  = {-1, -1};
    vector<Int_t> markerColors = {kAzure - 2, kOrange + 7};
    vector<Int_t> markerStyles = {20, 21};

    vector<TH1*> emptySystematics;

    // ── DeltaEta overlay ──
    {
        vector<TH1*> histsEta = {pPbEtaS, PbPEtaS};

        TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
        TPad *p1 = (TPad*)PlotCMSDiffResultRegion(
            histsEta, emptySystematics, emptySystematics, "eta_overlay", labels,
            lineColors, lineStyles,
            markerColors, markerStyles,
            "#Delta y_{ch,Z}", -3.87, 3.87,
            "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
            "Pbp #minus pPb", -1, -1,
            0, 3.87,
            0,
            false, false, true,
            0.2
        );

        AddUPCHeader(p1, "8.16 TeV", "pPb, Pbp");

        p1->cd();
        TLatex latex;
        latex.SetNDC();
        latex.SetTextAlign(31);
        latex.SetTextSize(0.035);
        latex.DrawLatex(0.75, 0.82, "0 < p_{T}^{Z} < 500 GeV");
        latex.DrawLatex(0.75, 0.77, "0.5 < p_{T}^{ch} < 15 GeV");
        latex.DrawLatex(0.75, 0.72, "|y^{Z}_{CM}| < 1.935, 0 < #Delta#varphi_{ch,Z} < #pi/2");

        c1->Update();
        string etaOut = outDir + "/pPbPbp_signed_sym_ZPT0_500_trkPT" + trkRange + "-DeltaEta-overlay.pdf";
        c1->SaveAs(etaOut.c_str());
        cout << "Saved: " << etaOut << endl;
        delete c1;
    }

    // ── DeltaPhi overlay ──
    {
        vector<TH1*> histsPhi = {pPbPhiS, PbPPhiS};

        TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);
        TPad *p2 = (TPad*)PlotCMSDiffResultRegion(
            histsPhi, emptySystematics, emptySystematics, "phi_overlay", labels,
            lineColors, lineStyles,
            markerColors, markerStyles,
            "#Delta#varphi_{ch,Z}", -1.5707, 4.7123,
            "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}", -1, -1,
            "Pbp #minus pPb", -1, -1,
            0, M_PI,
            0,
            false, false, true,
            0.2
        );

        AddUPCHeader(p2, "8.16 TeV", "pPb, Pbp");

        p2->cd();
        TLatex latex2;
        latex2.SetNDC();
        latex2.SetTextAlign(11);
        latex2.SetTextSize(0.035);
        latex2.DrawLatex(0.21, 0.60, "0 < p_{T}^{Z} < 500 GeV");
        latex2.DrawLatex(0.21, 0.55, "0.5 < p_{T}^{ch} < 15 GeV");
        latex2.DrawLatex(0.21, 0.50, "|y^{Z}_{CM}| < 1.935");

        c2->Update();
        string phiOut = outDir + "/pPbPbp_signed_sym_ZPT0_500_trkPT" + trkRange + "-DeltaPhi-overlay.pdf";
        c2->SaveAs(phiOut.c_str());
        cout << "Saved: " << phiOut << endl;
        delete c2;
    }

    return 0;
}
