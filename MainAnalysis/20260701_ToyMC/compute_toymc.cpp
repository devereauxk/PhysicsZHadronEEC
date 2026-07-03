// Pseudoexperiment-based p-value calibration.
// Test 1: permutation toys for pPb vs PbP (stat-only, 12 bins, unsymmetrized)
// Test 2: parametric toys for combined pPb vs pp (stat+syst, 6 unique bins)

#include <TDecompChol.h>
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
#include <random>
#include <string>
#include <vector>

#include "CommandLine.h"

using namespace std;

// ──────────────────────────────────────────────────────────────────────────────
// Data structures (matching existing jackknife/bootstrap code)
// ──────────────────────────────────────────────────────────────────────────────

struct JKEventData {
    double sigNZ = 0, mixNZ = 0;
    vector<float> sigEta, mixEta, sigPhi, mixPhi;
};

// ──────────────────────────────────────────────────────────────────────────────
// I/O helpers (from compute_bootstrap.cpp)
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
// SVD pseudoinverse chi-squared (from compute_bootstrap.cpp)
// ──────────────────────────────────────────────────────────────────────────────

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
// Jackknife covariance (per-system, unsymmetrized) — from compute_bootstrap.cpp
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
// Full covariance chi-squared (from compute_bootstrap.cpp)
// ──────────────────────────────────────────────────────────────────────────────

struct CompatResult {
    string label;
    string method;
    int nBins = 0;
    double chi2 = 0;
    int ndf = 0;
    double pValue = 0;
};

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

    auto [chi2val, rank] = svdChiSqRaw(delta, Ctot);
    r.chi2 = chi2val;
    r.ndf = rank;
    if (rank > 0) r.pValue = TMath::Prob(chi2val, rank);
    return r;
}

// ──────────────────────────────────────────────────────────────────────────────
// 1D symmetrization helpers (from compute_bootstrap.cpp)
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
// Pooled JK covariance (symmetrized) — from compute_bootstrap.cpp
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
// 2D combination helpers (from compute_bootstrap.cpp)
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
// Test 2 chi-squared helper (from compute_bootstrap.cpp)
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
// NEW: Test 1 permutation toy engine
// ──────────────────────────────────────────────────────────────────────────────

struct ToyResult1 {
    double chi2_eta = 0, pval_eta = 0;
    double chi2_phi = 0, pval_phi = 0;
    int rank_eta = 0, rank_phi = 0;
};

// Build central value histogram from a subset of JK events
TH1D *buildCentralFromEvents(const vector<JKEventData> &events,
    const vector<int> &indices, TH1D *axis, bool useEta)
{
    int n = axis->GetNbinsX();
    TH1D *h = (TH1D *)axis->Clone("_toy_central");
    h->SetDirectory(nullptr);
    h->Reset();

    double totSigNZ = 0, totMixNZ = 0;
    vector<double> totSig(n, 0), totMix(n, 0);
    for (int idx : indices) {
        const auto &ev = events[idx];
        totSigNZ += ev.sigNZ; totMixNZ += ev.mixNZ;
        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;
        for (int i = 0; i < n && i < (int)s.size(); ++i) { totSig[i] += s[i]; totMix[i] += m[i]; }
    }
    if (totSigNZ <= 0 || totMixNZ <= 0) return h;

    for (int i = 0; i < n; ++i) {
        double val = (totSig[i] / totSigNZ - totMix[i] / totMixNZ) / axis->GetBinWidth(i + 1);
        h->SetBinContent(i + 1, val);
    }
    return h;
}

// Build JK covariance from a subset of events identified by indices into a pooled array
TMatrixDSym buildJKCovSubset(const vector<JKEventData> &events,
    const vector<int> &indices, TH1D *axis, bool useEta)
{
    int n = axis->GetNbinsX();
    TMatrixDSym C(n);
    if (indices.empty()) return C;

    double totSigNZ = 0, totMixNZ = 0;
    vector<double> totSig(n, 0), totMix(n, 0);
    for (int idx : indices) {
        const auto &ev = events[idx];
        totSigNZ += ev.sigNZ; totMixNZ += ev.mixNZ;
        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;
        for (int i = 0; i < n && i < (int)s.size(); ++i) { totSig[i] += s[i]; totMix[i] += m[i]; }
    }
    if (totSigNZ <= 0 || totMixNZ <= 0) return C;

    vector<double> bw(n), theta(n);
    for (int i = 0; i < n; ++i) { bw[i] = axis->GetBinWidth(i + 1); theta[i] = totSig[i] / totSigNZ - totMix[i] / totMixNZ; }

    int valid = 0;
    for (int idx : indices) {
        const auto &ev = events[idx];
        double lSigNZ = totSigNZ - ev.sigNZ, lMixNZ = totMixNZ - ev.mixNZ;
        if (lSigNZ <= 0 || lMixNZ <= 0) continue;
        ++valid;
        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;
        vector<double> d(n);
        for (int i = 0; i < n; ++i) {
            double loo = (totSig[i] - (i < (int)s.size() ? s[i] : 0.f)) / lSigNZ
                       - (totMix[i] - (i < (int)m.size() ? m[i] : 0.f)) / lMixNZ;
            d[i] = (loo - theta[i]) / bw[i];
        }
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                C(i, j) += d[i] * d[j];
    }
    if (valid > 1) C *= (double)(valid - 1) / valid;
    return C;
}

vector<ToyResult1> runTest1Toys(
    const vector<JKEventData> &pPbEvents,
    const vector<JKEventData> &PbPEvents,
    TH1D *etaAxis, TH1D *phiAxis,
    int nToys, unsigned int seed)
{
    int N_pPb = pPbEvents.size();
    int N_PbP = PbPEvents.size();
    int N_total = N_pPb + N_PbP;

    // Pool all events
    vector<JKEventData> pool;
    pool.reserve(N_total);
    pool.insert(pool.end(), pPbEvents.begin(), pPbEvents.end());
    pool.insert(pool.end(), PbPEvents.begin(), PbPEvents.end());

    vector<int> allIndices(N_total);
    iota(allIndices.begin(), allIndices.end(), 0);

    vector<ToyResult1> results(nToys);
    mt19937_64 rng(seed);

    for (int t = 0; t < nToys; ++t) {
        if (t % 100 == 0) cout << "    Test 1 toy " << t << "/" << nToys << "\r" << flush;

        // Fisher-Yates partial shuffle: first N_pPb elements are pseudo-pPb
        vector<int> perm = allIndices;
        for (int i = 0; i < N_pPb; ++i) {
            uniform_int_distribution<int> dist(i, N_total - 1);
            int j = dist(rng);
            swap(perm[i], perm[j]);
        }
        vector<int> idxA(perm.begin(), perm.begin() + N_pPb);
        vector<int> idxB(perm.begin() + N_pPb, perm.end());

        // DeltaEta
        TH1D *hA_eta = buildCentralFromEvents(pool, idxA, etaAxis, true);
        TH1D *hB_eta = buildCentralFromEvents(pool, idxB, etaAxis, true);
        TMatrixDSym CA_eta = buildJKCovSubset(pool, idxA, etaAxis, true);
        TMatrixDSym CB_eta = buildJKCovSubset(pool, idxB, etaAxis, true);

        int n = etaAxis->GetNbinsX();
        TMatrixDSym Ctot_eta = CA_eta + CB_eta;
        TVectorD delta_eta(n);
        for (int i = 0; i < n; ++i)
            delta_eta(i) = hA_eta->GetBinContent(i + 1) - hB_eta->GetBinContent(i + 1);
        auto [c2e, re] = svdChiSqRaw(delta_eta, Ctot_eta);
        results[t].chi2_eta = c2e;
        results[t].rank_eta = re;
        results[t].pval_eta = (re > 0) ? TMath::Prob(c2e, re) : -1;

        delete hA_eta; delete hB_eta;

        // DeltaPhi
        TH1D *hA_phi = buildCentralFromEvents(pool, idxA, phiAxis, false);
        TH1D *hB_phi = buildCentralFromEvents(pool, idxB, phiAxis, false);
        TMatrixDSym CA_phi = buildJKCovSubset(pool, idxA, phiAxis, false);
        TMatrixDSym CB_phi = buildJKCovSubset(pool, idxB, phiAxis, false);

        int np = phiAxis->GetNbinsX();
        TMatrixDSym Ctot_phi = CA_phi + CB_phi;
        TVectorD delta_phi(np);
        for (int i = 0; i < np; ++i)
            delta_phi(i) = hA_phi->GetBinContent(i + 1) - hB_phi->GetBinContent(i + 1);
        auto [c2p, rp] = svdChiSqRaw(delta_phi, Ctot_phi);
        results[t].chi2_phi = c2p;
        results[t].rank_phi = rp;
        results[t].pval_phi = (rp > 0) ? TMath::Prob(c2p, rp) : -1;

        delete hA_phi; delete hB_phi;
    }
    cout << "    Test 1 toy " << nToys << "/" << nToys << " done" << endl;
    return results;
}

// ──────────────────────────────────────────────────────────────────────────────
// NEW: Test 2 parametric toy engine
// ──────────────────────────────────────────────────────────────────────────────

struct ToyResult2 {
    double chi2_diag = 0, pval_diag = 0;
    double chi2_full = 0, pval_full = 0;
    int ndf_diag = 0, ndf_full = 0;
};

vector<int> getUniqueBins(bool isEta, int n) {
    vector<int> bins;
    if (isEta) { for (int i = 0; i < n / 2; ++i) bins.push_back(i); }
    else { for (int i = 0; i < 3; ++i) bins.push_back(i); for (int i = 6; i < 9; ++i) bins.push_back(i); }
    return bins;
}

vector<ToyResult2> runTest2Toys(
    const CovResult &hiStat, const CovResult &ppStat,
    TH1D *hSyst, bool isEta,
    int nToys, unsigned int seed)
{
    int n = hiStat.central.size();
    vector<int> uniqueBins = getUniqueBins(isEta, n);
    int nU = uniqueBins.size();

    // Build C_total on unique bins
    TMatrixDSym C_u(nU);
    for (int a = 0; a < nU; ++a) {
        int ia = uniqueBins[a];
        for (int b = 0; b < nU; ++b) {
            int ib = uniqueBins[b];
            double cstat = hiStat.cov(ia, ib) + ppStat.cov(ia, ib);
            double csyst = (a == b) ? pow(hSyst->GetBinContent(ia + 1), 2) : 0;
            C_u(a, b) = cstat + csyst;
        }
    }

    // Cholesky decomposition
    TDecompChol chol(C_u);
    bool cholOK = chol.Decompose();
    TMatrixD L(nU, nU);
    if (cholOK) {
        L = chol.GetU();
        L.Transpose(L);
    } else {
        // Fallback: eigendecomposition
        cerr << "Cholesky failed, using eigendecomposition fallback" << endl;
        TDecompSVD svd(C_u);
        const TVectorD &sv = svd.GetSig();
        const TMatrixD &U = svd.GetU();
        L.Zero();
        for (int k = 0; k < nU; ++k) {
            if (sv(k) > 0) {
                double sqrtS = sqrt(sv(k));
                for (int i = 0; i < nU; ++i) L(i, k) = U(i, k) * sqrtS;
            }
        }
    }

    vector<ToyResult2> results(nToys);
    TRandom3 rng(seed);

    for (int t = 0; t < nToys; ++t) {
        TVectorD z(nU);
        for (int i = 0; i < nU; ++i) z(i) = rng.Gaus();
        TVectorD delta = L * z;

        // Diagonal chi-squared
        double chi2d = 0; int ndfd = 0;
        for (int a = 0; a < nU; ++a) {
            double s2 = C_u(a, a);
            if (s2 <= 0) continue;
            chi2d += delta(a) * delta(a) / s2;
            ++ndfd;
        }
        results[t].chi2_diag = chi2d;
        results[t].ndf_diag = ndfd;
        results[t].pval_diag = (ndfd > 0) ? TMath::Prob(chi2d, ndfd) : -1;

        // Full covariance chi-squared
        auto [c2f, nf] = svdChiSqRaw(delta, C_u);
        results[t].chi2_full = c2f;
        results[t].ndf_full = nf;
        results[t].pval_full = (nf > 0) ? TMath::Prob(c2f, nf) : -1;
    }
    return results;
}

// ──────────────────────────────────────────────────────────────────────────────
// KS test of p-values vs Uniform(0,1)
// ──────────────────────────────────────────────────────────────────────────────

pair<double, double> ksTestVsUniform(const vector<double> &pvals) {
    vector<double> sorted;
    for (double p : pvals) if (p >= 0) sorted.push_back(p);
    sort(sorted.begin(), sorted.end());
    int n = sorted.size();
    if (n == 0) return {0, 1};

    double D = 0;
    for (int i = 0; i < n; ++i) {
        double cdf = (double)(i + 1) / n;
        double cdfMinus = (double)i / n;
        D = max(D, fabs(cdf - sorted[i]));
        D = max(D, fabs(cdfMinus - sorted[i]));
    }
    double pval = TMath::KolmogorovProb(D * sqrt((double)n));
    return {D, pval};
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
    const int    nToys1       = CL.GetInt("NToys1", 500);
    const int    nToys2       = CL.GetInt("NToys2", 10000);
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

    string prefix = binLabel.empty() ? "" : (binLabel + " ");

    TH1D *pPbJkEta = loadHist(pPbFile, jkEtaKey, "pPb_jk_eta");
    TH1D *PbPJkEta = loadHist(PbPFile, jkEtaKey, "PbP_jk_eta");
    TH1D *pPbJkPhi = loadHist(pPbFile, jkPhiKey, "pPb_jk_phi");
    TH1D *PbPJkPhi = loadHist(PbPFile, jkPhiKey, "PbP_jk_phi");

    if (!pPbJkEta || !PbPJkEta || !pPbJkPhi || !PbPJkPhi) {
        cerr << "Missing result histograms for trkRange " << trkRange << endl;
        return 1;
    }

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
    // Compute data test statistics for comparison
    // =====================================================================

    // Test 1 data: pPb vs PbP
    TMatrixDSym CjkPPb_eta = buildJKCov(pPbEvents, pPbJkEta, true);
    TMatrixDSym CjkPbP_eta = buildJKCov(PbPEvents, PbPJkEta, true);
    TMatrixDSym CjkPPb_phi = buildJKCov(pPbEvents, pPbJkPhi, false);
    TMatrixDSym CjkPbP_phi = buildJKCov(PbPEvents, PbPJkPhi, false);

    CompatResult dataT1_eta = fullCovChiSq(pPbJkEta, PbPJkEta, CjkPPb_eta, CjkPbP_eta,
        prefix + "DeltaEta", "FullCov (JK)");
    CompatResult dataT1_phi = fullCovChiSq(pPbJkPhi, PbPJkPhi, CjkPPb_phi, CjkPbP_phi,
        prefix + "DeltaPhi", "FullCov (JK)");

    cout << "\nData Test 1 (pPb vs PbP):" << endl;
    cout << "  DeltaEta: chi2=" << Form("%.4f", dataT1_eta.chi2) << " ndf=" << dataT1_eta.ndf
         << " p=" << Form("%.6f", dataT1_eta.pValue) << endl;
    cout << "  DeltaPhi: chi2=" << Form("%.4f", dataT1_phi.chi2) << " ndf=" << dataT1_phi.ndf
         << " p=" << Form("%.6f", dataT1_phi.pValue) << endl;

    // Test 2 data: combined pPb vs pp
    TH1D *etaAxis = (TH1D *)pPbFile.Get(jkEtaKey.c_str());
    TH1D *phiAxis = (TH1D *)pPbFile.Get(jkPhiKey.c_str());
    etaAxis->SetDirectory(0); phiAxis->SetDirectory(0);

    vector<JKEventData> evsHI;
    evsHI.reserve(pPbEvents.size() + PbPEvents.size());
    evsHI.insert(evsHI.end(), pPbEvents.begin(), pPbEvents.end());
    evsHI.insert(evsHI.end(), PbPEvents.begin(), PbPEvents.end());

    auto hiEta_jk = buildPooledJKCov1D(evsHI, true, etaAxis);
    auto hiPhi_jk = buildPooledJKCov1D(evsHI, false, phiAxis);
    auto ppEta_jk = buildPooledJKCov1D(ppEvents, true, etaAxis);
    auto ppPhi_jk = buildPooledJKCov1D(ppEvents, false, phiAxis);

    auto centrals = compute2DCombinedCentrals(&pPbFile, &PbPFile, &ppFile, trkRange);

    int nE = hiEta_jk.central.size(), nP = hiPhi_jk.central.size();
    for (int i = 0; i < nE; ++i) for (int j = 0; j < nE; ++j) {
        hiEta_jk.cov(i, j) *= 0.25; ppEta_jk.cov(i, j) *= 0.25;
    }
    for (int i = 0; i < nP; ++i) for (int j = 0; j < nP; ++j) {
        hiPhi_jk.cov(i, j) *= 0.25; ppPhi_jk.cov(i, j) *= 0.25;
    }

    hiEta_jk.central = centrals.hiEta; hiPhi_jk.central = centrals.hiPhi;
    ppEta_jk.central = centrals.ppEta; ppPhi_jk.central = centrals.ppPhi;

    vector<CompatResult> dataT2;
    computeTest2ChiSq(hiEta_jk, ppEta_jk, hSystEta, true,  prefix + "DeltaEta", "JK", dataT2);
    computeTest2ChiSq(hiPhi_jk, ppPhi_jk, hSystPhi, false, prefix + "DeltaPhi", "JK", dataT2);

    cout << "\nData Test 2 (combined pPb vs pp):" << endl;
    for (const auto &r : dataT2)
        cout << "  " << r.label << " [" << r.method << "]  chi2="
             << Form("%.4f", r.chi2) << "  ndf=" << r.ndf << "  p=" << Form("%.6f", r.pValue) << endl;

    // =====================================================================
    // TEST 1 TOYS: permutation
    // =====================================================================
    cout << "\n--- Running Test 1 permutation toys (" << nToys1 << ") ---" << endl;
    auto toys1 = runTest1Toys(pPbEvents, PbPEvents, pPbJkEta, pPbJkPhi, nToys1, seed);

    // Collect p-values and chi2 for diagnostics
    vector<double> t1_pvals_eta, t1_pvals_phi, t1_chi2_eta, t1_chi2_phi;
    for (const auto &t : toys1) {
        if (t.pval_eta >= 0) { t1_pvals_eta.push_back(t.pval_eta); t1_chi2_eta.push_back(t.chi2_eta); }
        if (t.pval_phi >= 0) { t1_pvals_phi.push_back(t.pval_phi); t1_chi2_phi.push_back(t.chi2_phi); }
    }

    auto meanRMS = [](const vector<double> &v) -> pair<double, double> {
        if (v.empty()) return {0, 0};
        double s = 0, s2 = 0;
        for (double x : v) { s += x; s2 += x * x; }
        double mean = s / v.size();
        double rms = sqrt(s2 / v.size() - mean * mean);
        return {mean, rms};
    };

    auto empPval = [](const vector<double> &chi2s, double dataChi2) -> double {
        int n = 0, exceed = 0;
        for (double c : chi2s) { ++n; if (c >= dataChi2) ++exceed; }
        return n > 0 ? (double)exceed / n : -1;
    };

    auto [t1_mean_eta, t1_rms_eta] = meanRMS(t1_chi2_eta);
    auto [t1_mean_phi, t1_rms_phi] = meanRMS(t1_chi2_phi);
    double t1_emp_eta = empPval(t1_chi2_eta, dataT1_eta.chi2);
    double t1_emp_phi = empPval(t1_chi2_phi, dataT1_phi.chi2);
    auto [t1_ks_D_eta, t1_ks_p_eta] = ksTestVsUniform(t1_pvals_eta);
    auto [t1_ks_D_phi, t1_ks_p_phi] = ksTestVsUniform(t1_pvals_phi);

    cout << "  DeltaEta: toy <chi2>=" << Form("%.2f", t1_mean_eta) << " RMS=" << Form("%.2f", t1_rms_eta)
         << " expected " << dataT1_eta.ndf << " / " << Form("%.2f", sqrt(2.0 * dataT1_eta.ndf))
         << "  emp_p=" << Form("%.4f", t1_emp_eta)
         << "  KS(toys vs U): D=" << Form("%.4f", t1_ks_D_eta) << " p=" << Form("%.4f", t1_ks_p_eta) << endl;
    cout << "  DeltaPhi: toy <chi2>=" << Form("%.2f", t1_mean_phi) << " RMS=" << Form("%.2f", t1_rms_phi)
         << " expected " << dataT1_phi.ndf << " / " << Form("%.2f", sqrt(2.0 * dataT1_phi.ndf))
         << "  emp_p=" << Form("%.4f", t1_emp_phi)
         << "  KS(toys vs U): D=" << Form("%.4f", t1_ks_D_phi) << " p=" << Form("%.4f", t1_ks_p_phi) << endl;

    // Write Test 1 TSV
    {
        string path = outputDir + "/test1_toymc.tsv";
        ios_base::openmode mode = appendTSV ? (ios::out | ios::app) : ios::out;
        ofstream out(path, mode);
        if (!appendTSV) out << "BinLabel\tObservable\tNToys\tDataChi2\tDataNdf\tDataPval\tToyMeanChi2\tToyRMSChi2\tExpectedMeanChi2\tExpectedRMSChi2\tEmpiricalPval\tKS_D_vs_Uniform\tKS_pval_vs_Uniform\n";
        out << setprecision(6) << fixed;
        out << binLabel << "\tDeltaEta\t" << nToys1 << "\t" << dataT1_eta.chi2 << "\t" << dataT1_eta.ndf << "\t" << dataT1_eta.pValue << "\t"
            << t1_mean_eta << "\t" << t1_rms_eta << "\t" << (double)dataT1_eta.ndf << "\t" << sqrt(2.0 * dataT1_eta.ndf) << "\t"
            << t1_emp_eta << "\t" << t1_ks_D_eta << "\t" << t1_ks_p_eta << "\n";
        out << binLabel << "\tDeltaPhi\t" << nToys1 << "\t" << dataT1_phi.chi2 << "\t" << dataT1_phi.ndf << "\t" << dataT1_phi.pValue << "\t"
            << t1_mean_phi << "\t" << t1_rms_phi << "\t" << (double)dataT1_phi.ndf << "\t" << sqrt(2.0 * dataT1_phi.ndf) << "\t"
            << t1_emp_phi << "\t" << t1_ks_D_phi << "\t" << t1_ks_p_phi << "\n";
    }

    // =====================================================================
    // TEST 2 TOYS: parametric
    // =====================================================================
    cout << "\n--- Running Test 2 parametric toys (" << nToys2 << ") ---" << endl;

    auto toys2_eta = runTest2Toys(hiEta_jk, ppEta_jk, hSystEta, true, nToys2, seed + 100);
    auto toys2_phi = runTest2Toys(hiPhi_jk, ppPhi_jk, hSystPhi, false, nToys2, seed + 200);

    // Find matching data results
    CompatResult dataT2_eta_diag, dataT2_eta_full, dataT2_phi_diag, dataT2_phi_full;
    for (const auto &r : dataT2) {
        if (r.label.find("DeltaEta") != string::npos) {
            if (r.method.find("Diagonal") != string::npos) dataT2_eta_diag = r;
            else dataT2_eta_full = r;
        } else {
            if (r.method.find("Diagonal") != string::npos) dataT2_phi_diag = r;
            else dataT2_phi_full = r;
        }
    }

    struct T2Summary {
        string obs, method;
        double dataChi2, dataPval;
        int dataNdf;
        double toyMean, toyRMS, empP, ksD, ksP;
    };

    auto summarizeT2 = [&](const vector<ToyResult2> &toys, const string &obs,
        bool useFull, const CompatResult &dataR) -> T2Summary
    {
        T2Summary s;
        s.obs = obs; s.method = useFull ? "FullCovariance" : "Diagonal";
        s.dataChi2 = dataR.chi2; s.dataNdf = dataR.ndf; s.dataPval = dataR.pValue;

        vector<double> chi2s, pvals;
        for (const auto &t : toys) {
            double c = useFull ? t.chi2_full : t.chi2_diag;
            double p = useFull ? t.pval_full : t.pval_diag;
            if (p >= 0) { chi2s.push_back(c); pvals.push_back(p); }
        }
        auto [m, r] = meanRMS(chi2s);
        s.toyMean = m; s.toyRMS = r;
        s.empP = empPval(chi2s, dataR.chi2);
        auto [d, p] = ksTestVsUniform(pvals);
        s.ksD = d; s.ksP = p;
        return s;
    };

    vector<T2Summary> t2sums;
    t2sums.push_back(summarizeT2(toys2_eta, "DeltaEta", false, dataT2_eta_diag));
    t2sums.push_back(summarizeT2(toys2_eta, "DeltaEta", true,  dataT2_eta_full));
    t2sums.push_back(summarizeT2(toys2_phi, "DeltaPhi", false, dataT2_phi_diag));
    t2sums.push_back(summarizeT2(toys2_phi, "DeltaPhi", true,  dataT2_phi_full));

    for (const auto &s : t2sums) {
        cout << "  " << s.obs << " [" << s.method << "]: toy <chi2>=" << Form("%.2f", s.toyMean)
             << " RMS=" << Form("%.2f", s.toyRMS) << " expected " << s.dataNdf << " / "
             << Form("%.2f", sqrt(2.0 * s.dataNdf))
             << "  emp_p=" << Form("%.4f", s.empP)
             << "  KS: D=" << Form("%.4f", s.ksD) << " p=" << Form("%.4f", s.ksP) << endl;
    }

    // Write Test 2 TSV
    {
        string path = outputDir + "/test2_toymc.tsv";
        ios_base::openmode mode = appendTSV ? (ios::out | ios::app) : ios::out;
        ofstream out(path, mode);
        if (!appendTSV) out << "BinLabel\tObservable\tMethod\tNToys\tDataChi2\tDataNdf\tDataPval\tToyMeanChi2\tToyRMSChi2\tExpectedMeanChi2\tExpectedRMSChi2\tEmpiricalPval\tKS_D_vs_Uniform\tKS_pval_vs_Uniform\n";
        out << setprecision(6) << fixed;
        for (const auto &s : t2sums) {
            out << binLabel << "\t" << s.obs << "\t" << s.method << "\t" << nToys2 << "\t"
                << s.dataChi2 << "\t" << s.dataNdf << "\t" << s.dataPval << "\t"
                << s.toyMean << "\t" << s.toyRMS << "\t" << (double)s.dataNdf << "\t" << sqrt(2.0 * s.dataNdf) << "\t"
                << s.empP << "\t" << s.ksD << "\t" << s.ksP << "\n";
        }
    }

    // Write ROOT histograms
    {
        string rootPath = outputDir + "/toymc_histograms.root";
        TFile *fOut = TFile::Open(rootPath.c_str(), appendTSV ? "UPDATE" : "RECREATE");
        string safe = binLabel;
        replace(safe.begin(), safe.end(), ' ', '_');
        replace(safe.begin(), safe.end(), '.', 'p');

        // Test 1 histograms
        auto makeHists = [&](const string &tag, const vector<double> &pvals, const vector<double> &chi2s,
            double dataChi2, int dataNdf)
        {
            TH1D hP(("hPval_" + tag).c_str(), ("p-value distribution " + tag).c_str(), 20, 0, 1);
            for (double p : pvals) hP.Fill(p);
            hP.Write();

            double maxChi2 = 3.0 * dataNdf;
            if (!chi2s.empty()) maxChi2 = max(maxChi2, *max_element(chi2s.begin(), chi2s.end()) * 1.1);
            TH1D hC(("hChi2_" + tag).c_str(), ("chi2 distribution " + tag).c_str(), 50, 0, maxChi2);
            for (double c : chi2s) hC.Fill(c);
            hC.Write();

            TH1D hD(("hDataChi2_" + tag).c_str(), "", 1, dataChi2 - 0.01, dataChi2 + 0.01);
            hD.SetBinContent(1, dataChi2);
            hD.Write();
        };

        makeHists("test1_eta_" + safe, t1_pvals_eta, t1_chi2_eta, dataT1_eta.chi2, dataT1_eta.ndf);
        makeHists("test1_phi_" + safe, t1_pvals_phi, t1_chi2_phi, dataT1_phi.chi2, dataT1_phi.ndf);

        // Test 2 histograms
        for (const auto &obs : {"eta", "phi"}) {
            bool isEta = (string(obs) == "eta");
            const auto &toys = isEta ? toys2_eta : toys2_phi;
            const auto &dataFull = isEta ? dataT2_eta_full : dataT2_phi_full;
            vector<double> pvals, chi2s;
            for (const auto &t : toys) {
                if (t.pval_full >= 0) { pvals.push_back(t.pval_full); chi2s.push_back(t.chi2_full); }
            }
            makeHists("test2_" + string(obs) + "_" + safe, pvals, chi2s, dataFull.chi2, dataFull.ndf);
        }

        fOut->Close();
    }

    cout << "\nDone: " << binLabel << endl;
    return 0;
}
