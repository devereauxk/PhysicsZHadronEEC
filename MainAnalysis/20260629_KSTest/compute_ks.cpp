#include <TFile.h>
#include <TH1D.h>
#include <TSystem.h>

#include <Math/GoFTest.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

struct KSResult {
    string ObservableLabel;
    int    N_bins           = 0;
    double D_KS             = 0;
    double KS_PValue        = -1;
    double Permutation_PValue = -1;
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

double computeDKS(const vector<Double_t> &a, const vector<Double_t> &b)
{
    ROOT::Math::GoFTest gof(a.size(), a.data(), b.size(), b.data());
    Double_t pvalue = -1, testStat = -1;
    gof.KolmogorovSmirnov2SamplesTest(pvalue, testStat);
    return testStat;
}

KSResult computeKS(TH1D *pPb, TH1D *PbP, const string &label, int nPerm)
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

    // Asymptotic p-value
    ROOT::Math::GoFTest gof(N, vp.data(), N, vq.data());
    Double_t pvalue = -1, testStat = -1;
    gof.KolmogorovSmirnov2SamplesTest(pvalue, testStat);
    result.D_KS      = testStat;
    result.KS_PValue = pvalue;

    // Permutation test
    vector<Double_t> pooled(2 * N);
    copy(vp.begin(), vp.end(), pooled.begin());
    copy(vq.begin(), vq.end(), pooled.begin() + N);

    mt19937_64 rng(42);
    int nExceed = 0;
    for (int t = 0; t < nPerm; ++t) {
        shuffle(pooled.begin(), pooled.end(), rng);
        vector<Double_t> sa(pooled.begin(), pooled.begin() + N);
        vector<Double_t> sb(pooled.begin() + N, pooled.end());
        double dPerm = computeDKS(sa, sb);
        if (dPerm >= result.D_KS)
            ++nExceed;
    }
    result.Permutation_PValue = (double)nExceed / nPerm;

    return result;
}

// ──────────────────────────────────────────────────────────────────────────────
// Output writers
// ──────────────────────────────────────────────────────────────────────────────

void writeKSMarkdown(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << "# pPb vs Pbp KS shape compatibility (GoFTest two-sample)\n\n";
    out << "| Observable | N_bins | D_KS | Asymp. p-value | Perm. p-value |\n";
    out << "| --- | ---: | ---: | ---: | ---: |\n";
    out << setprecision(6);
    for (const KSResult &r : results)
        out << "| `" << r.ObservableLabel << "` | " << r.N_bins
            << " | " << r.D_KS
            << " | " << r.KS_PValue
            << " | " << r.Permutation_PValue << " |\n";
}

void writeKSTSV(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << "Observable\tN_bins\tD_KS\tAsymp_PValue\tPerm_PValue\n";
    out << setprecision(10);
    for (const KSResult &r : results)
        out << r.ObservableLabel << '\t' << r.N_bins
            << '\t' << r.D_KS
            << '\t' << r.KS_PValue
            << '\t' << r.Permutation_PValue << '\n';
}

void writeKSTeXBare(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << fixed << setprecision(4);
    out << "\\begin{tabular}{|l|r|r|r|r|}\n\\hline\n";
    out << "Observable & $N_{\\rm bins}$ & $D_{\\rm KS}$ & Asymp.\\ $p$-value & Perm.\\ $p$-value \\\\\n\\hline\n";
    for (const KSResult &r : results) {
        string obs = r.ObservableLabel;
        string obsEsc;
        for (char ch : obs)
            obsEsc += (ch == '_') ? "\\textunderscore " : string(1, ch);

        auto fmtPval = [](double p) -> string {
            if (p < 0) return "---";
            if (p == 0.0) return "$\\ll 10^{-10}$";
            if (p < 0.001) {
                char buf[64];
                snprintf(buf, sizeof(buf), "$%.2e$", p);
                return buf;
            }
            char buf[32];
            snprintf(buf, sizeof(buf), "%.3f", p);
            return buf;
        };

        out << "\\texttt{" << obsEsc << "} & "
            << r.N_bins << " & "
            << r.D_KS << " & "
            << fmtPval(r.KS_PValue) << " & "
            << fmtPval(r.Permutation_PValue) << " \\\\\n";
    }
    out << "\\hline\n\\end{tabular}\n";
}

void writeKSTeX(const string &path, const vector<KSResult> &results)
{
    ofstream out(path);
    out << fixed << setprecision(4);
    out << "\\begin{table}[hbtp]\n\\centering\n";
    out << "\\begin{tabular}{|l|r|r|r|r|}\n\\hline\n";
    out << "Observable & $N_{\\rm bins}$ & $D_{\\rm KS}$ & Asymp.\\ $p$-value & Perm.\\ $p$-value \\\\\n\\hline\n";
    for (const KSResult &r : results) {
        string obs = r.ObservableLabel;
        string obsMacro = (obs.find("Eta") != string::npos) ? "$\\dytz$" : "$\\dphitz$";

        auto fmtPval = [](double p) -> string {
            if (p < 0) return "---";
            if (p == 0.0) return "$\\ll 10^{-10}$";
            if (p < 0.001) {
                char buf[64];
                snprintf(buf, sizeof(buf), "$%.2e$", p);
                return buf;
            }
            char buf[32];
            snprintf(buf, sizeof(buf), "%.3f", p);
            return buf;
        };

        out << obsMacro << " & " << r.N_bins
            << " & " << r.D_KS
            << " & " << fmtPval(r.KS_PValue)
            << " & " << fmtPval(r.Permutation_PValue) << " \\\\\n";
    }
    out << "\\hline\n\\end{tabular}\n";
    out << "\\caption{Two-sample KS statistic $D_{\\rm KS}$ for pPb vs Pbp. "
        << "Asymptotic $p$-value from \\texttt{ROOT::Math::GoFTest}; "
        << "permutation $p$-value from $10^5$ random relabellings of the 24 pooled bin values.}\n";
    out << "\\label{table:combining_ks}\n\\end{table}\n";
}

// ──────────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string tag      = CL.Get("Tag",      "ZV10_trkV29_nmix10");
    const string baseDir  = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outDir   = CL.Get("OutputDir", "output");
    const string trkRange = CL.Get("TrkRange",  "0.5_15");
    const int    nPerm    = CL.GetInt("NPerm",  100000);

    const string pPbFileOverride = CL.Get("pPbFile", "");
    const string PbPFileOverride = CL.Get("PbPFile", "");
    const string pPbFile = pPbFileOverride.empty()
        ? (baseDir + "/pPb_trkResidual_" + tag + "_ZPT0_500-result.root")
        : pPbFileOverride;
    const string PbPFile = PbPFileOverride.empty()
        ? (baseDir + "/PbP_trkResidual_" + tag + "_ZPT0_500-result.root")
        : PbPFileOverride;

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

    cout << "Running KS test with " << nPerm << " permutations..." << endl;

    vector<KSResult> results;
    results.push_back(computeKS(pPbEta, PbPEta, etaKey, nPerm));
    results.push_back(computeKS(pPbPhi, PbPPhi, phiKey, nPerm));

    gSystem->mkdir(outDir.c_str(), true);
    writeKSMarkdown (outDir + "/pPbPbp_ks.md",       results);
    writeKSTSV      (outDir + "/pPbPbp_ks.tsv",      results);
    writeKSTeX      (outDir + "/pPbPbp_ks.tex",      results);
    writeKSTeXBare  (outDir + "/pPbPbp_ks_bare.tex", results);

    cout << "\nKS results (tag=" << tag << ", nPerm=" << nPerm << "):\n";
    for (const KSResult &r : results)
        cout << r.ObservableLabel
             << "  N=" << r.N_bins
             << "  D_KS=" << r.D_KS
             << "  asymp_p=" << r.KS_PValue
             << "  perm_p=" << r.Permutation_PValue << endl;

    return 0;
}
