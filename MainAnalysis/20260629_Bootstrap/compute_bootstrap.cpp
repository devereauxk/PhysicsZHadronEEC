// Bootstrap cross-check of jackknife-derived chi-squared p-values.
// Test 1: pPb vs PbP compatibility (unsymmetrized, stat-only, 12 bins)
// Test 2: Combined pPb vs pp result agreement (symmetrized, stat+syst, 6 unique bins)

#include <TDecompSVD.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMatrixDSym.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVectorD.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "CommandLine.h"

using namespace std;

// ──────────────────────────────────────────────────────────────────────────────
// Data structures (matching existing jackknife code)
// ──────────────────────────────────────────────────────────────────────────────

struct JKEventData {
    double sigNZ = 0, mixNZ = 0;
    vector<float> sigEta, mixEta, sigPhi, mixPhi;
};

struct CompatResult {
    string label;
    string method;
    int nBins = 0;
    double chi2 = 0;
    int ndf = 0;
    double pValue = 0;
};

// ──────────────────────────────────────────────────────────────────────────────
// I/O helpers
// ──────────────────────────────────────────────────────────────────────────────

vector<JKEventData> loadJKEvents(TFile &file, const string &treeName) {
    TTree *tree = (TTree *)file.Get(treeName.c_str());
    if (!tree) { cerr << "No tree " << treeName << " in " << file.GetName() << endl; return {}; }

    double sigNZ = 0, mixNZ = 0;
    vector<float> *sigEta = nullptr, *mixEta = nullptr;
    vector<float> *sigPhi = nullptr, *mixPhi = nullptr;
    tree->SetBranchAddress("SignalNZ", &sigNZ);
    tree->SetBranchAddress("MixNZ", &mixNZ);
    tree->SetBranchAddress("SignalEta", &sigEta);
    tree->SetBranchAddress("MixEta", &mixEta);
    tree->SetBranchAddress("SignalPhi", &sigPhi);
    tree->SetBranchAddress("MixPhi", &mixPhi);

    long long N = tree->GetEntries();
    vector<JKEventData> events(N);
    for (long long k = 0; k < N; ++k) {
        tree->GetEntry(k);
        events[k] = {sigNZ, mixNZ, *sigEta, *mixEta, *sigPhi, *mixPhi};
    }
    return events;
}

TH1D *loadHist(TFile &file, const string &name, const string &clone) {
    TH1D *h = (TH1D *)file.Get(name.c_str());
    if (!h) return nullptr;
    h = (TH1D *)h->Clone(clone.c_str());
    h->SetDirectory(nullptr);
    return h;
}

// ──────────────────────────────────────────────────────────────────────────────
// Diagonal chi-squared (unchanged from existing code)
// ──────────────────────────────────────────────────────────────────────────────

CompatResult diagChiSq(TH1D *h1, TH1D *h2, const string &label, const string &method) {
    CompatResult r;
    r.label = label; r.method = method;
    int n = min(h1->GetNbinsX(), h2->GetNbinsX());
    r.nBins = n;
    for (int i = 1; i <= n; ++i) {
        double s2 = h1->GetBinError(i) * h1->GetBinError(i)
                   + h2->GetBinError(i) * h2->GetBinError(i);
        if (s2 <= 0) continue;
        double d = h1->GetBinContent(i) - h2->GetBinContent(i);
        r.chi2 += d * d / s2;
        ++r.ndf;
    }
    if (r.ndf > 0) r.pValue = TMath::Prob(r.chi2, r.ndf);
    return r;
}

// ──────────────────────────────────────────────────────────────────────────────
// SVD pseudoinverse chi-squared (unchanged from existing code)
// ──────────────────────────────────────────────────────────────────────────────

CompatResult fullCovChiSq(TH1D *h1, TH1D *h2,
    const TMatrixDSym &C1, const TMatrixDSym &C2, const string &label, const string &method)
{
    CompatResult r;
    r.label = label; r.method = method;
    int n = min(h1->GetNbinsX(), h2->GetNbinsX());
    r.nBins = n;

    TMatrixDSym Ctot = C1 + C2;
    TVectorD delta(n);
    for (int i = 0; i < n; ++i)
        delta(i) = h1->GetBinContent(i + 1) - h2->GetBinContent(i + 1);

    TDecompSVD svd(Ctot);
    const TVectorD &sv = svd.GetSig();
    int nsv = sv.GetNrows();
    double svMax = (nsv > 0) ? sv(0) : 0;
    double tol = svMax * 1e-8;
    int rank = 0;
    for (int k = 0; k < nsv; ++k) if (sv(k) > tol) ++rank;
    if (rank == 0) return r;
    r.ndf = rank;

    const TMatrixD &U = svd.GetU();
    const TMatrixD &V = svd.GetV();
    TMatrixD Cpinv(n, n);
    for (int k = 0; k < nsv; ++k) {
        if (sv(k) <= tol) continue;
        double inv = 1.0 / sv(k);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                Cpinv(i, j) += V(i, k) * inv * U(j, k);
    }
    TVectorD Cpd = Cpinv * delta;
    r.chi2 = delta * Cpd;
    r.pValue = TMath::Prob(r.chi2, rank);
    return r;
}

pair<double, int> svdChiSqRaw(const TVectorD &delta, const TMatrixDSym &C) {
    int n = delta.GetNrows();
    TDecompSVD svd(C);
    const TVectorD &sv = svd.GetSig();
    int nsv = sv.GetNrows();
    double svMax = (nsv > 0) ? sv(0) : 0;
    double tol = svMax * 1e-8;
    int rank = 0;
    for (int k = 0; k < nsv; ++k) if (sv(k) > tol) ++rank;
    if (rank == 0) return {0.0, 0};

    const TMatrixD &U = svd.GetU();
    const TMatrixD &V = svd.GetV();
    TMatrixD Cpinv(n, n);
    for (int k = 0; k < nsv; ++k) {
        if (sv(k) <= tol) continue;
        double inv = 1.0 / sv(k);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                Cpinv(i, j) += V(i, k) * inv * U(j, k);
    }
    TVectorD Cpd = Cpinv * delta;
    return {delta * Cpd, rank};
}

// ──────────────────────────────────────────────────────────────────────────────
// Jackknife covariance (per-system, unsymmetrized) — reproduces existing code
// ──────────────────────────────────────────────────────────────────────────────

TMatrixDSym buildJKCov(const vector<JKEventData> &events, TH1D *axis, bool useEta) {
    int n = axis->GetNbinsX();
    TMatrixDSym C(n);
    if (events.empty()) return C;

    vector<double> totSig(n, 0), totMix(n, 0);
    double totSigNZ = 0, totMixNZ = 0;
    for (const auto &ev : events) {
        totSigNZ += ev.sigNZ; totMixNZ += ev.mixNZ;
        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;
        for (int i = 0; i < n && i < (int)s.size(); ++i) { totSig[i] += s[i]; totMix[i] += m[i]; }
    }
    if (totSigNZ <= 0 || totMixNZ <= 0) return C;

    vector<double> bw(n), theta(n);
    for (int i = 0; i < n; ++i) { bw[i] = axis->GetBinWidth(i + 1); theta[i] = totSig[i] / totSigNZ - totMix[i] / totMixNZ; }

    int valid = 0;
    for (const auto &ev : events) {
        double lSigNZ = totSigNZ - ev.sigNZ, lMixNZ = totMixNZ - ev.mixNZ;
        if (lSigNZ <= 0 || lMixNZ <= 0) continue;
        ++valid;
        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;
        vector<double> d(n);
        for (int i = 0; i < n; ++i) {
            double loo = (totSig[i] - s[i]) / lSigNZ - (totMix[i] - m[i]) / lMixNZ;
            d[i] = (loo - theta[i]) / bw[i];
        }
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                C(i, j) += d[i] * d[j];
    }
    if (valid > 1) C *= (double)(valid - 1) / valid;
    return C;
}

// ──────────────────────────────────────────────────────────────────────────────
// Bootstrap covariance (per-system, unsymmetrized) — NEW
// ──────────────────────────────────────────────────────────────────────────────

TMatrixDSym buildBootstrapCov(const vector<JKEventData> &events, TH1D *axis,
    bool useEta, int B, unsigned int seed)
{
    int n = axis->GetNbinsX();
    int N = events.size();
    TMatrixDSym C(n);
    if (N == 0) return C;

    vector<double> bw(n);
    for (int i = 0; i < n; ++i) bw[i] = axis->GetBinWidth(i + 1);

    vector<vector<double>> thetaAll(B, vector<double>(n, 0));
    TRandom3 rng(seed);

    for (int b = 0; b < B; ++b) {
        vector<double> bSig(n, 0), bMix(n, 0);
        double bSigNZ = 0, bMixNZ = 0;
        for (int draw = 0; draw < N; ++draw) {
            int idx = rng.Integer(N);
            const auto &ev = events[idx];
            bSigNZ += ev.sigNZ; bMixNZ += ev.mixNZ;
            const auto &s = useEta ? ev.sigEta : ev.sigPhi;
            const auto &m = useEta ? ev.mixEta : ev.mixPhi;
            for (int i = 0; i < n && i < (int)s.size(); ++i) { bSig[i] += s[i]; bMix[i] += m[i]; }
        }
        if (bSigNZ <= 0 || bMixNZ <= 0) continue;
        for (int i = 0; i < n; ++i)
            thetaAll[b][i] = (bSig[i] / bSigNZ - bMix[i] / bMixNZ) / bw[i];
    }

    vector<double> thetaBar(n, 0);
    for (int b = 0; b < B; ++b)
        for (int i = 0; i < n; ++i)
            thetaBar[i] += thetaAll[b][i];
    for (int i = 0; i < n; ++i) thetaBar[i] /= B;

    for (int b = 0; b < B; ++b)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                C(i, j) += (thetaAll[b][i] - thetaBar[i]) * (thetaAll[b][j] - thetaBar[j]);
    if (B > 1) C *= 1.0 / (B - 1);
    return C;
}

// ──────────────────────────────────────────────────────────────────────────────
// 1D symmetrization helpers (matching ComputeChiSquared.cpp)
// ──────────────────────────────────────────────────────────────────────────────

void symmetrize1DEta(vector<double> &v) {
    int n = v.size();
    for (int i = 0; i < n / 2; ++i) {
        double avg = 0.5 * (v[i] + v[n - 1 - i]);
        v[i] = v[n - 1 - i] = avg;
    }
}

void symmetrize1DPhi(vector<double> &v) {
    for (int j = 0; j < 3; ++j) {
        double avg = 0.5 * (v[j] + v[5 - j]);
        v[j] = v[5 - j] = avg;
    }
    for (int j = 6; j < 9; ++j) {
        double avg = 0.5 * (v[j] + v[17 - j]);
        v[j] = v[17 - j] = avg;
    }
}

// ──────────────────────────────────────────────────────────────────────────────
// Pooled JK covariance (symmetrized) — reproduces ComputeChiSquared.cpp
// ──────────────────────────────────────────────────────────────────────────────

struct CovResult {
    vector<double> central;
    TMatrixDSym cov;
    int validEvents;
};

CovResult buildPooledJKCov1D(const vector<JKEventData> &events, bool doEta, const TH1D *axis) {
    int n = axis->GetNbinsX();
    vector<double> totSig(n, 0), totMix(n, 0);
    double totSigNZ = 0, totMixNZ = 0;
    for (const auto &ev : events) {
        totSigNZ += ev.sigNZ; totMixNZ += ev.mixNZ;
        const auto &sig = doEta ? ev.sigEta : ev.sigPhi;
        const auto &mix = doEta ? ev.mixEta : ev.mixPhi;
        for (int i = 0; i < n && i < (int)sig.size(); ++i) { totSig[i] += sig[i]; totMix[i] += mix[i]; }
    }

    vector<double> central(n);
    for (int i = 0; i < n; ++i) {
        double val = (totSigNZ > 0 ? totSig[i] / totSigNZ : 0)
                   - (totMixNZ > 0 ? totMix[i] / totMixNZ : 0);
        central[i] = val / axis->GetBinWidth(i + 1);
    }
    if (doEta) symmetrize1DEta(central);
    else       symmetrize1DPhi(central);

    TMatrixDSym cov(n);
    int valid = 0;
    for (int e = 0; e < (int)events.size(); ++e) {
        double lSigNZ = totSigNZ - events[e].sigNZ;
        double lMixNZ = totMixNZ - events[e].mixNZ;
        if (lSigNZ <= 0 || lMixNZ <= 0) continue;
        const auto &sig = doEta ? events[e].sigEta : events[e].sigPhi;
        const auto &mix = doEta ? events[e].mixEta : events[e].mixPhi;
        vector<double> loo(n);
        for (int i = 0; i < n; ++i) {
            double ls = totSig[i] - (i < (int)sig.size() ? sig[i] : 0);
            double lm = totMix[i] - (i < (int)mix.size() ? mix[i] : 0);
            loo[i] = (ls / lSigNZ - lm / lMixNZ) / axis->GetBinWidth(i + 1);
        }
        if (doEta) symmetrize1DEta(loo);
        else       symmetrize1DPhi(loo);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                cov(i, j) += (loo[i] - central[i]) * (loo[j] - central[j]);
        ++valid;
    }
    if (valid > 1) cov *= (double)(valid - 1) / valid;
    return {central, cov, valid};
}

// ──────────────────────────────────────────────────────────────────────────────
// Pooled bootstrap covariance (symmetrized) — NEW
// ──────────────────────────────────────────────────────────────────────────────

CovResult buildPooledBootstrapCov1D(const vector<JKEventData> &events, bool doEta,
    const TH1D *axis, int B, unsigned int seed)
{
    int n = axis->GetNbinsX();
    int N = events.size();
    CovResult result;
    result.cov.ResizeTo(n, n);
    result.validEvents = N;
    if (N == 0) { result.central.resize(n, 0); return result; }

    vector<double> bw(n);
    for (int i = 0; i < n; ++i) bw[i] = axis->GetBinWidth(i + 1);

    vector<vector<double>> thetaAll(B, vector<double>(n, 0));
    TRandom3 rng(seed);

    for (int b = 0; b < B; ++b) {
        vector<double> bSig(n, 0), bMix(n, 0);
        double bSigNZ = 0, bMixNZ = 0;
        for (int draw = 0; draw < N; ++draw) {
            int idx = rng.Integer(N);
            const auto &ev = events[idx];
            bSigNZ += ev.sigNZ; bMixNZ += ev.mixNZ;
            const auto &sig = doEta ? ev.sigEta : ev.sigPhi;
            const auto &mix = doEta ? ev.mixEta : ev.mixPhi;
            for (int i = 0; i < n && i < (int)sig.size(); ++i) { bSig[i] += sig[i]; bMix[i] += mix[i]; }
        }
        if (bSigNZ <= 0 || bMixNZ <= 0) continue;
        for (int i = 0; i < n; ++i)
            thetaAll[b][i] = (bSig[i] / bSigNZ - bMix[i] / bMixNZ) / bw[i];
        if (doEta) symmetrize1DEta(thetaAll[b]);
        else       symmetrize1DPhi(thetaAll[b]);
    }

    vector<double> thetaBar(n, 0);
    for (int b = 0; b < B; ++b)
        for (int i = 0; i < n; ++i)
            thetaBar[i] += thetaAll[b][i];
    for (int i = 0; i < n; ++i) thetaBar[i] /= B;
    result.central = thetaBar;

    for (int b = 0; b < B; ++b)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                result.cov(i, j) += (thetaAll[b][i] - thetaBar[i]) * (thetaAll[b][j] - thetaBar[j]);
    if (B > 1) result.cov *= 1.0 / (B - 1);
    return result;
}

// ──────────────────────────────────────────────────────────────────────────────
// 2D central value computation (matching ComputeChiSquared.cpp::compute2DCombinedCentrals)
// ──────────────────────────────────────────────────────────────────────────────

void divideByWidth(TH1D *h) {
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
        double w = h->GetBinWidth(i);
        if (w > 0) { h->SetBinContent(i, h->GetBinContent(i) / w); h->SetBinError(i, h->GetBinError(i) / w); }
    }
}

void symmetrize2DTH2(TH2D *h) {
    int nX = h->GetNbinsX(), nY = h->GetNbinsY();
    TH2D *tmp = (TH2D *)h->Clone("_sym_tmp");
    for (int i = 1; i <= nX; ++i)
        for (int j = 1; j <= nY; ++j) {
            int mi = nX + 1 - i;
            int mj = (j <= nY / 2) ? (nY / 2 + 1 - j) : (nY + nY / 2 + 1 - j);
            h->SetBinContent(i, j, 0.25 * (tmp->GetBinContent(i, j) + tmp->GetBinContent(mi, j)
                + tmp->GetBinContent(i, mj) + tmp->GetBinContent(mi, mj)));
        }
    delete tmp;
}

void symmetrize1DTH1Eta(TH1D *h) {
    int n = h->GetNbinsX();
    for (int i = 1; i <= n / 2; ++i) {
        int mi = n + 1 - i;
        double avg = 0.5 * (h->GetBinContent(i) + h->GetBinContent(mi));
        h->SetBinContent(i, avg); h->SetBinContent(mi, avg);
    }
}

void symmetrize1DTH1Phi(TH1D *h) {
    for (int j = 1; j <= 3; ++j) {
        int mj = 7 - j;
        double avg = 0.5 * (h->GetBinContent(j) + h->GetBinContent(mj));
        h->SetBinContent(j, avg); h->SetBinContent(mj, avg);
    }
    for (int j = 7; j <= 9; ++j) {
        int mj = 19 - j;
        double avg = 0.5 * (h->GetBinContent(j) + h->GetBinContent(mj));
        h->SetBinContent(j, avg); h->SetBinContent(mj, avg);
    }
}

struct CombinedCentrals {
    vector<double> hiEta, hiPhi, ppEta, ppPhi;
};

CombinedCentrals compute2DCombinedCentrals(TFile *fPPb, TFile *fPbP, TFile *fpp, const string &trkTag) {
    TH2D *hDA_pPb = (TH2D *)((TH2D *)fPPb->Get(("hDataAll_" + trkTag).c_str()))->Clone("DA_pPb");
    TH2D *hM_pPb  = (TH2D *)((TH2D *)fPPb->Get(("hMixData_" + trkTag).c_str()))->Clone("M_pPb");
    double NZ_pPb  = ((TH1D *)fPPb->Get(("hNZData_" + trkTag).c_str()))->GetBinContent(1);
    double NZM_pPb = ((TH1D *)fPPb->Get(("hNZMixData_" + trkTag).c_str()))->GetBinContent(1);

    TH2D *hDA_PbP = (TH2D *)((TH2D *)fPbP->Get(("hDataAll_" + trkTag).c_str()))->Clone("DA_PbP");
    TH2D *hM_PbP  = (TH2D *)((TH2D *)fPbP->Get(("hMixData_" + trkTag).c_str()))->Clone("M_PbP");
    double NZ_PbP  = ((TH1D *)fPbP->Get(("hNZData_" + trkTag).c_str()))->GetBinContent(1);
    double NZM_PbP = ((TH1D *)fPbP->Get(("hNZMixData_" + trkTag).c_str()))->GetBinContent(1);

    hDA_pPb->Scale(NZ_pPb); hDA_PbP->Scale(NZ_PbP);
    hM_pPb->Scale(NZM_pPb); hM_PbP->Scale(NZM_PbP);

    TH2D *S = (TH2D *)hDA_pPb->Clone("S_comb"); S->Add(hDA_PbP);
    S->Scale(1.0 / (NZ_pPb + NZ_PbP));
    TH2D *B = (TH2D *)hM_pPb->Clone("B_comb"); B->Add(hM_PbP);
    B->Scale(1.0 / (NZM_pPb + NZM_PbP));
    S->Add(B, -1);

    symmetrize2DTH2(S);

    TH1D *hiPhiProj = (TH1D *)S->ProjectionY("hiPhi2D", 7, 12);
    TH1D *hiEtaProj = (TH1D *)S->ProjectionX("hiEta2D", 4, 6);
    divideByWidth(hiPhiProj); divideByWidth(hiEtaProj);
    hiPhiProj->Scale(0.5); hiEtaProj->Scale(0.5);
    symmetrize1DTH1Phi(hiPhiProj);
    symmetrize1DTH1Eta(hiEtaProj);

    TH1D *ppPhi = (TH1D *)((TH1D *)fpp->Get(("DeltaPhi_Result" + trkTag).c_str()))->Clone("ppPhi2D");
    TH1D *ppEta = (TH1D *)((TH1D *)fpp->Get(("DeltaEta_Result" + trkTag).c_str()))->Clone("ppEta2D");
    ppPhi->Scale(0.5); ppEta->Scale(0.5);
    symmetrize1DTH1Phi(ppPhi);
    symmetrize1DTH1Eta(ppEta);

    CombinedCentrals result;
    int nEta = hiEtaProj->GetNbinsX(), nPhi = hiPhiProj->GetNbinsX();
    result.hiEta.resize(nEta); result.hiPhi.resize(nPhi);
    result.ppEta.resize(nEta); result.ppPhi.resize(nPhi);
    for (int i = 0; i < nEta; ++i) { result.hiEta[i] = hiEtaProj->GetBinContent(i + 1); result.ppEta[i] = ppEta->GetBinContent(i + 1); }
    for (int i = 0; i < nPhi; ++i) { result.hiPhi[i] = hiPhiProj->GetBinContent(i + 1); result.ppPhi[i] = ppPhi->GetBinContent(i + 1); }

    delete hDA_pPb; delete hDA_PbP; delete hM_pPb; delete hM_PbP; delete S; delete B;
    delete hiPhiProj; delete hiEtaProj; delete ppPhi; delete ppEta;
    return result;
}

// ──────────────────────────────────────────────────────────────────────────────
// Output helpers
// ──────────────────────────────────────────────────────────────────────────────

void writeComparisonMD(const string &path, const string &title, const vector<CompatResult> &results,
    int nBoot, int seed, int nPPb, int nPbP, int nPP = -1)
{
    ofstream out(path);
    out << "# " << title << "\n\n";
    out << "- NBootstrap: " << nBoot << ", Seed: " << seed << "\n";
    out << "- pPb events: " << nPPb << ", PbP events: " << nPbP;
    if (nPP > 0) out << ", pp events: " << nPP;
    out << "\n\n";
    out << "| Observable | Error model | chi2 | ndf | p-value |\n";
    out << "| --- | --- | ---: | ---: | ---: |\n";
    out << setprecision(4) << fixed;
    for (const auto &r : results)
        out << "| " << r.label << " | " << r.method << " | " << r.chi2
            << " | " << r.ndf << " | " << r.pValue << " |\n";
}

void writeComparisonTSV(const string &path, const vector<CompatResult> &results) {
    ofstream out(path);
    out << "Observable\tErrorModel\tchi2\tndf\tp_value\n";
    out << setprecision(17);
    for (const auto &r : results)
        out << r.label << '\t' << r.method << '\t' << r.chi2 << '\t' << r.ndf << '\t' << r.pValue << '\n';
}

void writeVarianceRatioMD(const string &path, const string &sysLabel,
    const TMatrixDSym &Cjk, const TMatrixDSym &Cboot, TH1D *axis, const string &obsLabel)
{
    ofstream out(path, ios::app);
    out << "\n## " << sysLabel << " " << obsLabel << "\n\n";
    out << "| Bin | Center | sigma_JK | sigma_Boot | Ratio |\n";
    out << "| ---: | ---: | ---: | ---: | ---: |\n";
    out << setprecision(6) << fixed;
    int n = axis->GetNbinsX();
    for (int i = 0; i < n; ++i) {
        double sjk = sqrt(max(0.0, Cjk(i, i)));
        double sbt = sqrt(max(0.0, Cboot(i, i)));
        double ratio = (sbt > 0) ? sjk / sbt : 0;
        out << "| " << (i + 1) << " | " << axis->GetBinCenter(i + 1) << " | " << sjk << " | " << sbt << " | " << ratio << " |\n";
    }
}

// ──────────────────────────────────────────────────────────────────────────────
// Test 2 chi-squared helper (matching runChiSq from ComputeChiSquared.cpp)
// ──────────────────────────────────────────────────────────────────────────────

void computeTest2ChiSq(const CovResult &hi, const CovResult &pp,
    TH1D *hSyst, bool isEta, const string &obsLabel, const string &methodPrefix,
    vector<CompatResult> &results)
{
    int n = hi.central.size();
    TVectorD delta(n);
    for (int i = 0; i < n; ++i) delta(i) = hi.central[i] - pp.central[i];

    TMatrixDSym C_stat(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            C_stat(i, j) = hi.cov(i, j) + pp.cov(i, j);

    TMatrixDSym C_syst(n);
    for (int i = 0; i < n; ++i)
        C_syst(i, i) = pow(hSyst->GetBinContent(i + 1), 2);

    TMatrixDSym C_total(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            C_total(i, j) = C_stat(i, j) + C_syst(i, j);

    // Diagonal chi-squared on unique bins
    double chi2_diag = 0;
    int ndf_diag = 0;
    if (isEta) {
        for (int i = 0; i < n / 2; ++i) {
            double sigma2 = C_total(i, i);
            if (sigma2 <= 0) continue;
            chi2_diag += delta(i) * delta(i) / sigma2;
            ++ndf_diag;
        }
    } else {
        for (int i = 0; i < 3; ++i) { double s2 = C_total(i, i); if (s2 <= 0) continue; chi2_diag += delta(i) * delta(i) / s2; ++ndf_diag; }
        for (int i = 6; i < 9; ++i) { double s2 = C_total(i, i); if (s2 <= 0) continue; chi2_diag += delta(i) * delta(i) / s2; ++ndf_diag; }
    }
    double pval_diag = (ndf_diag > 0) ? TMath::Prob(chi2_diag, ndf_diag) : -1;
    results.push_back({obsLabel, "Diagonal (" + methodPrefix + ")", n, chi2_diag, ndf_diag, pval_diag});

    // Full covariance on unique bins
    vector<int> uniqueBins;
    if (isEta) { for (int i = 0; i < n / 2; ++i) uniqueBins.push_back(i); }
    else { for (int i = 0; i < 3; ++i) uniqueBins.push_back(i); for (int i = 6; i < 9; ++i) uniqueBins.push_back(i); }

    int nU = uniqueBins.size();
    TVectorD delta_u(nU);
    TMatrixDSym C_u(nU);
    for (int a = 0; a < nU; ++a) {
        delta_u(a) = delta(uniqueBins[a]);
        for (int b = 0; b < nU; ++b)
            C_u(a, b) = C_total(uniqueBins[a], uniqueBins[b]);
    }
    auto [chi2_full, ndf_full] = svdChiSqRaw(delta_u, C_u);
    double pval_full = (ndf_full > 0) ? TMath::Prob(chi2_full, ndf_full) : -1;
    results.push_back({obsLabel, "FullCovariance (" + methodPrefix + ")", n, chi2_full, ndf_full, pval_full});
}

// ──────────────────────────────────────────────────────────────────────────────
// main
// ──────────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);

    const string pPbFileName  = CL.Get("pPbFile");
    const string PbPFileName  = CL.Get("PbPFile");
    const string ppFileName   = CL.Get("ppFile");
    const string systFileName = CL.Get("SystematicsFile");
    const string trkRange     = CL.Get("TrkRange", "0.5_15");
    const string outputDir    = CL.Get("OutputDir", "output");
    const string binLabel     = CL.Get("BinLabel", "");
    const int    nBoot        = CL.GetInt("NBootstrap", 2000);
    const int    seed         = CL.GetInt("Seed", 42);
    const bool   appendTSV    = CL.Get("AppendTSV", "false") == "true";

    gSystem->mkdir(outputDir.c_str(), true);

    TFile pPbFile(pPbFileName.c_str(), "READ");
    TFile PbPFile(PbPFileName.c_str(), "READ");
    TFile ppFile(ppFileName.c_str(), "READ");
    if (pPbFile.IsZombie() || PbPFile.IsZombie() || ppFile.IsZombie()) {
        cerr << "Failed to open one or more input files." << endl;
        return 1;
    }

    TFile *fSyst = TFile::Open(systFileName.c_str(), "READ");
    if (!fSyst || fSyst->IsZombie()) { cerr << "Cannot open systematics: " << systFileName << endl; return 1; }
    TH1D *hSystEta = (TH1D *)fSyst->Get("DifferenceTotal_DeltaEta");
    TH1D *hSystPhi = (TH1D *)fSyst->Get("DifferenceTotal_DeltaPhi");
    if (!hSystEta || !hSystPhi) { cerr << "Missing DifferenceTotal histograms" << endl; return 1; }
    hSystEta->SetDirectory(0); hSystPhi->SetDirectory(0);
    fSyst->Close();

    const string treeName  = "JackknifeProjection" + trkRange;
    const string jkEtaKey  = "DeltaEta_Result" + trkRange;
    const string jkPhiKey  = "DeltaPhi_Result" + trkRange;
    const string sw2EtaKey = "DeltaEta_ResultSumw2" + trkRange;
    const string sw2PhiKey = "DeltaPhi_ResultSumw2" + trkRange;

    string prefix = binLabel.empty() ? "" : (binLabel + " ");

    // Load histograms
    TH1D *pPbJkEta  = loadHist(pPbFile, jkEtaKey, "pPb_jk_eta");
    TH1D *PbPJkEta  = loadHist(PbPFile, jkEtaKey, "PbP_jk_eta");
    TH1D *pPbSw2Eta = loadHist(pPbFile, sw2EtaKey, "pPb_sw2_eta");
    TH1D *PbPSw2Eta = loadHist(PbPFile, sw2EtaKey, "PbP_sw2_eta");
    TH1D *pPbJkPhi  = loadHist(pPbFile, jkPhiKey, "pPb_jk_phi");
    TH1D *PbPJkPhi  = loadHist(PbPFile, jkPhiKey, "PbP_jk_phi");
    TH1D *pPbSw2Phi = loadHist(pPbFile, sw2PhiKey, "pPb_sw2_phi");
    TH1D *PbPSw2Phi = loadHist(PbPFile, sw2PhiKey, "PbP_sw2_phi");

    if (!pPbJkEta || !PbPJkEta || !pPbJkPhi || !PbPJkPhi) {
        cerr << "Missing result histograms for trkRange " << trkRange << endl;
        return 1;
    }

    // Load JK event trees
    cout << "Loading jackknife events for " << (binLabel.empty() ? trkRange : binLabel) << "..." << endl;
    auto pPbEvents = loadJKEvents(pPbFile, treeName);
    auto PbPEvents = loadJKEvents(PbPFile, treeName);
    auto ppEvents  = loadJKEvents(ppFile, treeName);
    cout << "  pPb: " << pPbEvents.size() << "  PbP: " << PbPEvents.size()
         << "  pp: " << ppEvents.size() << endl;

    if (pPbEvents.empty() || PbPEvents.empty() || ppEvents.empty()) {
        cerr << "Missing jackknife event trees." << endl;
        return 1;
    }

    // =====================================================================
    // TEST 1: pPb vs PbP (unsymmetrized, stat-only, 12 bins)
    // =====================================================================
    cout << "\n--- Test 1: pPb vs PbP [" << prefix << "] ---" << endl;

    TMatrixDSym CjkPPb_eta = buildJKCov(pPbEvents, pPbJkEta, true);
    TMatrixDSym CjkPbP_eta = buildJKCov(PbPEvents, PbPJkEta, true);
    TMatrixDSym CjkPPb_phi = buildJKCov(pPbEvents, pPbJkPhi, false);
    TMatrixDSym CjkPbP_phi = buildJKCov(PbPEvents, PbPJkPhi, false);

    TMatrixDSym CbtPPb_eta = buildBootstrapCov(pPbEvents, pPbJkEta, true,  nBoot, seed);
    TMatrixDSym CbtPbP_eta = buildBootstrapCov(PbPEvents, PbPJkEta, true,  nBoot, seed + 1);
    TMatrixDSym CbtPPb_phi = buildBootstrapCov(pPbEvents, pPbJkPhi, false, nBoot, seed + 2);
    TMatrixDSym CbtPbP_phi = buildBootstrapCov(PbPEvents, PbPJkPhi, false, nBoot, seed + 3);

    vector<CompatResult> test1;
    test1.push_back(fullCovChiSq(pPbJkEta, PbPJkEta, CjkPPb_eta, CjkPbP_eta,
        prefix + "DeltaEta", "FullCov (JK)"));
    test1.push_back(fullCovChiSq(pPbJkEta, PbPJkEta, CbtPPb_eta, CbtPbP_eta,
        prefix + "DeltaEta", "FullCov (Bootstrap)"));
    test1.push_back(fullCovChiSq(pPbJkPhi, PbPJkPhi, CjkPPb_phi, CjkPbP_phi,
        prefix + "DeltaPhi", "FullCov (JK)"));
    test1.push_back(fullCovChiSq(pPbJkPhi, PbPJkPhi, CbtPPb_phi, CbtPbP_phi,
        prefix + "DeltaPhi", "FullCov (Bootstrap)"));

    for (const auto &r : test1)
        cout << "  " << r.label << " [" << r.method << "]  chi2="
             << Form("%.2f", r.chi2) << "  ndf=" << r.ndf << "  p=" << Form("%.4f", r.pValue) << endl;

    // Write Test 1 TSV
    {
        string t1path = outputDir + "/test1_allbins.tsv";
        ios_base::openmode mode = appendTSV ? (ios::out | ios::app) : ios::out;
        ofstream out(t1path, mode);
        if (!appendTSV) out << "BinLabel\tObservable\tMethod\tchi2\tndf\tp_value\n";
        out << setprecision(6) << fixed;
        for (const auto &r : test1)
            out << binLabel << '\t' << r.label << '\t' << r.method << '\t'
                << r.chi2 << '\t' << r.ndf << '\t' << r.pValue << '\n';
    }

    // =====================================================================
    // TEST 2: Combined pPb vs pp (symmetrized, stat+syst, 6 unique bins)
    // =====================================================================
    cout << "\n--- Test 2: Combined pPb vs pp [" << prefix << "] ---" << endl;

    vector<JKEventData> evsHI;
    evsHI.reserve(pPbEvents.size() + PbPEvents.size());
    evsHI.insert(evsHI.end(), pPbEvents.begin(), pPbEvents.end());
    evsHI.insert(evsHI.end(), PbPEvents.begin(), PbPEvents.end());

    TH1D *etaAxis = (TH1D *)pPbFile.Get(jkEtaKey.c_str());
    TH1D *phiAxis = (TH1D *)pPbFile.Get(jkPhiKey.c_str());
    etaAxis->SetDirectory(0); phiAxis->SetDirectory(0);

    auto hiEta_jk = buildPooledJKCov1D(evsHI, true, etaAxis);
    auto hiPhi_jk = buildPooledJKCov1D(evsHI, false, phiAxis);
    auto ppEta_jk = buildPooledJKCov1D(ppEvents, true, etaAxis);
    auto ppPhi_jk = buildPooledJKCov1D(ppEvents, false, phiAxis);

    auto hiEta_bt = buildPooledBootstrapCov1D(evsHI, true, etaAxis, nBoot, seed + 10);
    auto hiPhi_bt = buildPooledBootstrapCov1D(evsHI, false, phiAxis, nBoot, seed + 11);
    auto ppEta_bt = buildPooledBootstrapCov1D(ppEvents, true, etaAxis, nBoot, seed + 12);
    auto ppPhi_bt = buildPooledBootstrapCov1D(ppEvents, false, phiAxis, nBoot, seed + 13);

    auto centrals = compute2DCombinedCentrals(&pPbFile, &PbPFile, &ppFile, trkRange);

    int nE = hiEta_jk.central.size(), nP = hiPhi_jk.central.size();
    for (int i = 0; i < nE; ++i) for (int j = 0; j < nE; ++j) {
        hiEta_jk.cov(i, j) *= 0.25; ppEta_jk.cov(i, j) *= 0.25;
        hiEta_bt.cov(i, j) *= 0.25; ppEta_bt.cov(i, j) *= 0.25;
    }
    for (int i = 0; i < nP; ++i) for (int j = 0; j < nP; ++j) {
        hiPhi_jk.cov(i, j) *= 0.25; ppPhi_jk.cov(i, j) *= 0.25;
        hiPhi_bt.cov(i, j) *= 0.25; ppPhi_bt.cov(i, j) *= 0.25;
    }

    hiEta_jk.central = centrals.hiEta; hiPhi_jk.central = centrals.hiPhi;
    ppEta_jk.central = centrals.ppEta; ppPhi_jk.central = centrals.ppPhi;
    hiEta_bt.central = centrals.hiEta; hiPhi_bt.central = centrals.hiPhi;
    ppEta_bt.central = centrals.ppEta; ppPhi_bt.central = centrals.ppPhi;

    vector<CompatResult> test2;
    computeTest2ChiSq(hiEta_jk, ppEta_jk, hSystEta, true,  prefix + "DeltaEta", "JK", test2);
    computeTest2ChiSq(hiEta_bt, ppEta_bt, hSystEta, true,  prefix + "DeltaEta", "Bootstrap", test2);
    computeTest2ChiSq(hiPhi_jk, ppPhi_jk, hSystPhi, false, prefix + "DeltaPhi", "JK", test2);
    computeTest2ChiSq(hiPhi_bt, ppPhi_bt, hSystPhi, false, prefix + "DeltaPhi", "Bootstrap", test2);

    for (const auto &r : test2)
        cout << "  " << r.label << " [" << r.method << "]  chi2="
             << Form("%.2f", r.chi2) << "  ndf=" << r.ndf << "  p=" << Form("%.4f", r.pValue) << endl;

    // Write Test 2 TSV
    {
        string t2path = outputDir + "/test2_allbins.tsv";
        ios_base::openmode mode = appendTSV ? (ios::out | ios::app) : ios::out;
        ofstream out(t2path, mode);
        if (!appendTSV) out << "BinLabel\tObservable\tMethod\tchi2\tndf\tp_value\n";
        out << setprecision(6) << fixed;
        for (const auto &r : test2)
            out << binLabel << '\t' << r.label << '\t' << r.method << '\t'
                << r.chi2 << '\t' << r.ndf << '\t' << r.pValue << '\n';
    }

    return 0;
}
