#include <TFile.h>
#include <TH1D.h>
#include <TSystem.h>

#include <Math/GoFTest.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

// ──────────────────────────────────────────────────────────────────────────────

struct KSResult {
    string ObservableLabel;
    int    N_bins    = 0;
    double D_KS      = 0;
    double KS_PValue = -1;
};

TH1D *loadHistogram(TFile &file, const string &name, const string &cloneName)
{
    TH1D *h = (TH1D *)file.Get(name.c_str());
    if (h == nullptr)
        return nullptr;
    h = (TH1D *)h->Clone(cloneName.c_str());
    h->SetDirectory(nullptr);
    return h;
}

// Two-sample KS test via ROOT::Math::GoFTest.
// Treats the N bin contents of each histogram as two independent data samples.
// D_KS = max|F_pPb - F_Pbp| over sorted values; p-value uses the asymptotic
// Kolmogorov distribution with n_eff = n1*n2/(n1+n2) = N/2 (N bins per sample).
// Negative bin values are handled natively — no shift needed.
KSResult computeKS(TH1D *pPb, TH1D *PbP, const string &label)
{
    KSResult result;
    result.ObservableLabel = label;
    if (pPb == nullptr || PbP == nullptr)
        return result;

    int N = min(pPb->GetNbinsX(), PbP->GetNbinsX());
    result.N_bins = N;

    vector<Double_t> vp(N), vq(N);
    for (int i = 0; i < N; ++i) {
        vp[i] = pPb->GetBinContent(i + 1);
        vq[i] = PbP->GetBinContent(i + 1);
    }

    ROOT::Math::GoFTest gof(N, vp.data(), N, vq.data());
    Double_t pvalue = -1, testStat = -1;
    gof.KolmogorovSmirnov2SamplesTest(pvalue, testStat);
    result.D_KS      = testStat;
    result.KS_PValue = pvalue;
    return result;
}

// ──────────────────────────────────────────────────────────────────────────────
// Output writers
// ──────────────────────────────────────────────────────────────────────────────

void writeKSMarkdown(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << "# pPb vs Pbp KS shape compatibility (GoFTest two-sample)\n\n";
    out << "| Observable | N_bins | D_KS | KS_p-value |\n";
    out << "| --- | ---: | ---: | ---: |\n";
    out << setprecision(6);
    for (const KSResult &r : results)
        out << "| `" << r.ObservableLabel << "` | " << r.N_bins
            << " | " << r.D_KS
            << " | " << r.KS_PValue << " |\n";
}

void writeKSTSV(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << "Observable\tN_bins\tD_KS\tKS_PValue\n";
    out << setprecision(10);
    for (const KSResult &r : results)
        out << r.ObservableLabel << '\t' << r.N_bins
            << '\t' << r.D_KS
            << '\t' << r.KS_PValue << '\n';
}

// Bare tabular (no float wrapper) for \input{} in beamer.
void writeKSTeXBare(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << fixed << setprecision(4);
    out << "\\begin{tabular}{|l|r|r|r|}\n\\hline\n";
    out << "Observable & $N_{\\rm bins}$ & $D_{\\rm KS}$ & KS $p$-value \\\\\n\\hline\n";
    for (const KSResult &r : results) {
        string obs = r.ObservableLabel;
        string obsEsc;
        for (char ch : obs)
            obsEsc += (ch == '_') ? "\\textunderscore " : string(1, ch);

        string pval;
        if (r.KS_PValue < 0) {
            pval = "---";
        } else if (r.KS_PValue == 0.0) {
            pval = "$\\ll 10^{-10}$";
        } else if (r.KS_PValue < 0.001) {
            char buf[64];
            snprintf(buf, sizeof(buf), "$%.2e$", r.KS_PValue);
            pval = buf;
        } else {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.3f", r.KS_PValue);
            pval = buf;
        }

        out << "\\texttt{" << obsEsc << "} & "
            << r.N_bins << " & "
            << r.D_KS << " & "
            << pval << " \\\\\n";
    }
    out << "\\hline\n\\end{tabular}\n";
}

// Full standalone table (for note use).
void writeKSTeX(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << fixed << setprecision(4);
    out << "\\begin{table}[hbtp]\n\\centering\n";
    out << "\\begin{tabular}{|l|r|r|r|}\n\\hline\n";
    out << "Observable & $N_{\\rm bins}$ & $D_{\\rm KS}$ & KS $p$-value \\\\\n\\hline\n";
    for (const KSResult &r : results) {
        string obs = r.ObservableLabel;
        string obsMacro = (obs.find("Eta") != string::npos) ? "$\\dytz$" : "$\\dphitz$";

        string pval;
        if (r.KS_PValue < 0) {
            pval = "---";
        } else if (r.KS_PValue == 0.0) {
            pval = "$\\ll 10^{-10}$";
        } else if (r.KS_PValue < 0.001) {
            char buf[64];
            snprintf(buf, sizeof(buf), "$%.2e$", r.KS_PValue);
            pval = buf;
        } else {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.3f", r.KS_PValue);
            pval = buf;
        }

        out << obsMacro << " & " << r.N_bins
            << " & " << r.D_KS
            << " & " << pval << " \\\\\n";
    }
    out << "\\hline\n\\end{tabular}\n";
    out << "\\caption{Two-sample KS statistic $D_{\\rm KS}$ for pPb vs Pbp "
        << "computed via \\texttt{ROOT::Math::GoFTest} with the 12 result-histogram "
        << "bin values as input. The $p$-value uses the asymptotic Kolmogorov distribution "
        << "with $n_{\\rm eff} = N_{\\rm bins}/2 = 6$.}\n";
    out << "\\label{table:combining_ks}\n\\end{table}\n";
}

// ──────────────────────────────────────────────────────────────────────────────
// main
// ──────────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string tag      = CL.Get("Tag",      "ZV9_trkV28_nmix10");
    const string baseDir  = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outDir   = CL.Get("OutputDir", "output");
    const string trkRange = CL.Get("TrkRange",  "0.5_15");

    const string pPbFile = baseDir + "/pPb_trkResidual_" + tag + "_ZPT0_500-result.root";
    const string PbPFile = baseDir + "/PbP_trkResidual_" + tag + "_ZPT0_500-result.root";

    TFile fpPb(pPbFile.c_str(), "READ");
    TFile fPbP(PbPFile.c_str(), "READ");
    if (fpPb.IsZombie() || fPbP.IsZombie()) {
        cerr << "Failed to open input files:\n  " << pPbFile << "\n  " << PbPFile << endl;
        return 1;
    }

    const string etaKey = "DeltaEta_Result" + trkRange;
    const string phiKey = "DeltaPhi_Result" + trkRange;

    TH1D *pPbEta = loadHistogram(fpPb, etaKey, "pPb_eta");
    TH1D *PbPEta = loadHistogram(fPbP, etaKey, "PbP_eta");
    TH1D *pPbPhi = loadHistogram(fpPb, phiKey, "pPb_phi");
    TH1D *PbPPhi = loadHistogram(fPbP, phiKey, "PbP_phi");

    if (!pPbEta || !PbPEta || !pPbPhi || !PbPPhi) {
        cerr << "Missing one or more histograms." << endl;
        return 1;
    }

    vector<KSResult> results;
    results.push_back(computeKS(pPbEta, PbPEta, etaKey));
    results.push_back(computeKS(pPbPhi, PbPPhi, phiKey));

    gSystem->mkdir(outDir.c_str(), true);
    writeKSMarkdown (outDir + "/pPbPbp_ks.md",       results);
    writeKSTSV      (outDir + "/pPbPbp_ks.tsv",      results);
    writeKSTeX      (outDir + "/pPbPbp_ks.tex",      results);
    writeKSTeXBare  (outDir + "/pPbPbp_ks_bare.tex", results);

    cout << "\nKS results (tag=" << tag << "):\n";
    for (const KSResult &r : results)
        cout << r.ObservableLabel
             << "  N=" << r.N_bins
             << "  D_KS=" << r.D_KS
             << "  p=" << r.KS_PValue << endl;

    return 0;
}
