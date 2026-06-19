// Chi-squared stat+syst comparison: pp vs combined pPb
// Uses pooled pPb+Pbp jackknife for stat covariance, DifferenceTotal for syst.
// Outputs LaTeX tables for DeltaEta and DeltaPhi.

#include <TDecompSVD.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMatrixDSym.h>
#include <TMath.h>
#include <TVectorD.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>
using namespace std;

#include "CommandLine.h"

struct JKEvent2D { double sigNZ, mixNZ; vector<float> sigBins, mixBins; };
struct JKEvent1D { double sigNZ, mixNZ; vector<float> sigEta, mixEta, sigPhi, mixPhi; };

vector<JKEvent2D> loadJK2D(TFile* f) {
    TTree* t = (TTree*)f->Get("Jackknife2DData");
    if (!t) { cerr << "No Jackknife2DData in " << f->GetName() << endl; return {}; }
    double sigNZ = 0, mixNZ = 0;
    vector<float>* sigBins = nullptr;
    vector<float>* mixBins = nullptr;
    t->SetBranchAddress("SignalNZ",   &sigNZ);
    t->SetBranchAddress("MixNZ",     &mixNZ);
    t->SetBranchAddress("SignalBins", &sigBins);
    t->SetBranchAddress("MixBins",   &mixBins);
    long long N = t->GetEntries();
    vector<JKEvent2D> evs(N);
    for (long long k = 0; k < N; ++k) {
        t->GetEntry(k);
        evs[k] = {sigNZ, mixNZ, *sigBins, *mixBins};
    }
    return evs;
}

vector<JKEvent1D> loadJKProjected(TFile* f, const string& trkTag) {
    string treeName = "JackknifeProjection" + trkTag;
    TTree* t = (TTree*)f->Get(treeName.c_str());
    if (!t) { cerr << "No " << treeName << " in " << f->GetName() << endl; return {}; }
    double sigNZ = 0, mixNZ = 0;
    vector<float>* sigEta = nullptr; vector<float>* mixEta = nullptr;
    vector<float>* sigPhi = nullptr; vector<float>* mixPhi = nullptr;
    t->SetBranchAddress("SignalNZ", &sigNZ);
    t->SetBranchAddress("MixNZ",   &mixNZ);
    t->SetBranchAddress("SignalEta", &sigEta);
    t->SetBranchAddress("MixEta",   &mixEta);
    t->SetBranchAddress("SignalPhi", &sigPhi);
    t->SetBranchAddress("MixPhi",   &mixPhi);
    long long N = t->GetEntries();
    vector<JKEvent1D> evs(N);
    for (long long k = 0; k < N; ++k) {
        t->GetEntry(k);
        evs[k] = {sigNZ, mixNZ, *sigEta, *mixEta, *sigPhi, *mixPhi};
    }
    return evs;
}

void divideByWidth(TH1D* h) {
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
        double w = h->GetBinWidth(i);
        if (w > 0) { h->SetBinContent(i, h->GetBinContent(i) / w); h->SetBinError(i, h->GetBinError(i) / w); }
    }
}

void symmetrize2DTH2(TH2D* h) {
    int nX = h->GetNbinsX(), nY = h->GetNbinsY();
    TH2D* tmp = (TH2D*)h->Clone("_sym_tmp");
    for (int i = 1; i <= nX; ++i)
        for (int j = 1; j <= nY; ++j) {
            int mi = nX + 1 - i;
            int mj = (j <= nY / 2) ? (nY / 2 + 1 - j) : (nY + nY / 2 + 1 - j);
            h->SetBinContent(i, j, 0.25 * (tmp->GetBinContent(i, j) + tmp->GetBinContent(mi, j)
                + tmp->GetBinContent(i, mj) + tmp->GetBinContent(mi, mj)));
        }
    delete tmp;
}

void symmetrize1DTH1Eta(TH1D* h) {
    int n = h->GetNbinsX();
    for (int i = 1; i <= n / 2; ++i) {
        int mi = n + 1 - i;
        double avg = 0.5 * (h->GetBinContent(i) + h->GetBinContent(mi));
        h->SetBinContent(i, avg); h->SetBinContent(mi, avg);
    }
}

void symmetrize1DTH1Phi(TH1D* h) {
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

struct Combined2DCentrals {
    vector<double> hiEta, hiPhi, ppEta, ppPhi;
};

Combined2DCentrals compute2DCombinedCentrals(TFile* fPPb, TFile* fPbP, TFile* fpp, const string& trkTag) {
    string tag = trkTag;
    TH2D* hDA_pPb = (TH2D*)((TH2D*)fPPb->Get(("hDataAll_" + tag).c_str()))->Clone("DA_pPb");
    TH2D* hM_pPb  = (TH2D*)((TH2D*)fPPb->Get(("hMixData_" + tag).c_str()))->Clone("M_pPb");
    double NZ_pPb  = ((TH1D*)fPPb->Get(("hNZData_" + tag).c_str()))->GetBinContent(1);
    double NZM_pPb = ((TH1D*)fPPb->Get(("hNZMixData_" + tag).c_str()))->GetBinContent(1);

    TH2D* hDA_PbP = (TH2D*)((TH2D*)fPbP->Get(("hDataAll_" + tag).c_str()))->Clone("DA_PbP");
    TH2D* hM_PbP  = (TH2D*)((TH2D*)fPbP->Get(("hMixData_" + tag).c_str()))->Clone("M_PbP");
    double NZ_PbP  = ((TH1D*)fPbP->Get(("hNZData_" + tag).c_str()))->GetBinContent(1);
    double NZM_PbP = ((TH1D*)fPbP->Get(("hNZMixData_" + tag).c_str()))->GetBinContent(1);

    hDA_pPb->Scale(NZ_pPb);  hDA_PbP->Scale(NZ_PbP);
    hM_pPb->Scale(NZM_pPb);  hM_PbP->Scale(NZM_PbP);

    TH2D* S = (TH2D*)hDA_pPb->Clone("S_comb"); S->Add(hDA_PbP);
    S->Scale(1.0 / (NZ_pPb + NZ_PbP));
    TH2D* B = (TH2D*)hM_pPb->Clone("B_comb"); B->Add(hM_PbP);
    B->Scale(1.0 / (NZM_pPb + NZM_PbP));
    S->Add(B, -1);

    symmetrize2DTH2(S);

    TH1D* hiPhiProj = (TH1D*)S->ProjectionY("hiPhi2D", 7, 12);
    TH1D* hiEtaProj = (TH1D*)S->ProjectionX("hiEta2D", 4, 6);
    divideByWidth(hiPhiProj); divideByWidth(hiEtaProj);
    hiPhiProj->Scale(0.5);   hiEtaProj->Scale(0.5);
    symmetrize1DTH1Phi(hiPhiProj);
    symmetrize1DTH1Eta(hiEtaProj);

    TH1D* ppPhi = (TH1D*)((TH1D*)fpp->Get(("DeltaPhi_Result" + tag).c_str()))->Clone("ppPhi2D");
    TH1D* ppEta = (TH1D*)((TH1D*)fpp->Get(("DeltaEta_Result" + tag).c_str()))->Clone("ppEta2D");
    ppPhi->Scale(0.5); ppEta->Scale(0.5);
    symmetrize1DTH1Phi(ppPhi);
    symmetrize1DTH1Eta(ppEta);

    Combined2DCentrals result;
    int nEta = hiEtaProj->GetNbinsX(), nPhi = hiPhiProj->GetNbinsX();
    result.hiEta.resize(nEta); result.hiPhi.resize(nPhi);
    result.ppEta.resize(nEta); result.ppPhi.resize(nPhi);
    for (int i = 0; i < nEta; ++i) { result.hiEta[i] = hiEtaProj->GetBinContent(i + 1); result.ppEta[i] = ppEta->GetBinContent(i + 1); }
    for (int i = 0; i < nPhi; ++i) { result.hiPhi[i] = hiPhiProj->GetBinContent(i + 1); result.ppPhi[i] = ppPhi->GetBinContent(i + 1); }

    delete hDA_pPb; delete hDA_PbP; delete hM_pPb; delete hM_PbP; delete S; delete B;
    delete hiPhiProj; delete hiEtaProj; delete ppPhi; delete ppEta;
    return result;
}

void symmetrize1DEta(vector<double>& v) {
    int n = v.size();
    for (int i = 0; i < n / 2; ++i) {
        double avg = 0.5 * (v[i] + v[n - 1 - i]);
        v[i] = v[n - 1 - i] = avg;
    }
}

void symmetrize1DPhi(vector<double>& v) {
    for (int j = 0; j < 3; ++j) {
        double avg = 0.5 * (v[j] + v[5 - j]);
        v[j] = v[5 - j] = avg;
    }
    for (int j = 6; j < 9; ++j) {
        double avg = 0.5 * (v[j] + v[17 - j]);
        v[j] = v[17 - j] = avg;
    }
}

struct JKCovResult {
    vector<double> central;
    TMatrixDSym cov;
    int validEvents;
};

JKCovResult computePooledJKCov1D(const vector<JKEvent1D>& events, bool doEta,
                                  const TH1D* axis) {
    int n = axis->GetNbinsX();
    vector<double> totSig(n, 0), totMix(n, 0);
    double totSigNZ = 0, totMixNZ = 0;
    for (const auto& ev : events) {
        totSigNZ += ev.sigNZ;
        totMixNZ += ev.mixNZ;
        const auto& sig = doEta ? ev.sigEta : ev.sigPhi;
        const auto& mix = doEta ? ev.mixEta : ev.mixPhi;
        for (int i = 0; i < n && i < (int)sig.size(); ++i) {
            totSig[i] += sig[i];
            totMix[i] += mix[i];
        }
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
        const auto& sig = doEta ? events[e].sigEta : events[e].sigPhi;
        const auto& mix = doEta ? events[e].mixEta : events[e].mixPhi;
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

int etaMirror(int i) { return 13 - i; }
int phiMirror(int j) { return (j <= 6) ? (7 - j) : (19 - j); }
int flatIdx(int ix, int jy, int nPhi) { return (ix - 1) * nPhi + (jy - 1); }

void compute2DResult(const vector<double>& totSig, const vector<double>& totMix,
                     double totSigNZ, double totMixNZ, int nEta, int nPhi,
                     vector<double>& result) {
    int nBins = nEta * nPhi;
    result.resize(nBins);
    for (int b = 0; b < nBins; ++b)
        result[b] = (totSigNZ > 0 ? totSig[b] / totSigNZ : 0)
                  - (totMixNZ > 0 ? totMix[b] / totMixNZ : 0);
}

void symmetrize2D(vector<double>& r, int nEta, int nPhi) {
    vector<double> sym(r.size(), 0);
    for (int i = 1; i <= nEta; ++i)
        for (int j = 1; j <= nPhi; ++j) {
            int ni = etaMirror(i), nj = phiMirror(j);
            sym[flatIdx(i, j, nPhi)] = 0.25 * (
                r[flatIdx(i, j, nPhi)] + r[flatIdx(ni, j, nPhi)]
              + r[flatIdx(i, nj, nPhi)] + r[flatIdx(ni, nj, nPhi)]);
        }
    r = sym;
}

void projectEta(const vector<double>& r2d, int nEta, int nPhi,
                int phiFirst, int phiLast, double scale,
                const TH1D* axis, vector<double>& proj) {
    proj.resize(nEta, 0);
    for (int i = 1; i <= nEta; ++i) {
        double sum = 0;
        for (int j = phiFirst; j <= phiLast; ++j) sum += r2d[flatIdx(i, j, nPhi)];
        double bw = axis->GetBinWidth(i);
        proj[i - 1] = (bw > 0 ? sum / bw : 0) * scale;
    }
}

void projectPhi(const vector<double>& r2d, int nEta, int nPhi,
                double scale, const TH1D* axis, vector<double>& proj) {
    proj.resize(nPhi, 0);
    for (int j = 1; j <= nPhi; ++j) {
        double sum = 0;
        for (int i = 1; i <= nEta; ++i) sum += r2d[flatIdx(i, j, nPhi)];
        double bw = axis->GetBinWidth(j);
        proj[j - 1] = (bw > 0 ? sum / bw : 0) * scale;
    }
}

JKCovResult computePooledJKCovariance(
    const vector<JKEvent2D>& events, int nEta, int nPhi,
    int phiFirst, int phiLast, double etaScale, double phiScale,
    const TH1D* etaAxis, const TH1D* phiAxis, bool doEta)
{
    int nBins = nEta * nPhi;
    int nProj = doEta ? nEta : nPhi;

    vector<double> totSig(nBins, 0), totMix(nBins, 0);
    double totSigNZ = 0, totMixNZ = 0;
    for (const auto& ev : events) {
        totSigNZ += ev.sigNZ;
        totMixNZ += ev.mixNZ;
        for (int b = 0; b < nBins && b < (int)ev.sigBins.size(); ++b) totSig[b] += ev.sigBins[b];
        for (int b = 0; b < nBins && b < (int)ev.mixBins.size(); ++b) totMix[b] += ev.mixBins[b];
    }

    vector<double> r2d;
    compute2DResult(totSig, totMix, totSigNZ, totMixNZ, nEta, nPhi, r2d);
    symmetrize2D(r2d, nEta, nPhi);

    vector<double> central;
    if (doEta) projectEta(r2d, nEta, nPhi, phiFirst, phiLast, etaScale, etaAxis, central);
    else       projectPhi(r2d, nEta, nPhi, phiScale, phiAxis, central);

    TMatrixDSym cov(nProj);
    int valid = 0;
    for (int e = 0; e < (int)events.size(); ++e) {
        double lSigNZ = totSigNZ - events[e].sigNZ;
        double lMixNZ = totMixNZ - events[e].mixNZ;
        if (lSigNZ <= 0 || lMixNZ <= 0) continue;

        vector<double> r2d_loo(nBins);
        for (int b = 0; b < nBins; ++b) {
            double ls = totSig[b] - (b < (int)events[e].sigBins.size() ? events[e].sigBins[b] : 0);
            double lm = totMix[b] - (b < (int)events[e].mixBins.size() ? events[e].mixBins[b] : 0);
            r2d_loo[b] = ls / lSigNZ - lm / lMixNZ;
        }
        symmetrize2D(r2d_loo, nEta, nPhi);

        vector<double> proj_loo;
        if (doEta) projectEta(r2d_loo, nEta, nPhi, phiFirst, phiLast, etaScale, etaAxis, proj_loo);
        else       projectPhi(r2d_loo, nEta, nPhi, phiScale, phiAxis, proj_loo);

        for (int i = 0; i < nProj; ++i)
            for (int j = 0; j < nProj; ++j)
                cov(i, j) += (proj_loo[i] - central[i]) * (proj_loo[j] - central[j]);
        ++valid;
    }
    if (valid > 1) cov *= (double)(valid - 1) / valid;

    return {central, cov, valid};
}

pair<double, int> svdChiSq(const TVectorD& delta, const TMatrixDSym& C) {
    int n = delta.GetNrows();
    TDecompSVD svd(C);
    const TVectorD& sv = svd.GetSig();
    int nsv = sv.GetNrows();
    double svMax = (nsv > 0) ? sv(0) : 0;
    double tol = svMax * 1e-8;
    int rank = 0;
    for (int k = 0; k < nsv; ++k)
        if (sv(k) > tol) ++rank;
    if (rank == 0) return {0.0, 0};

    const TMatrixD& U = svd.GetU();
    const TMatrixD& V = svd.GetV();
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

void writeTexTable(ostream& out, const string& observable,
                   double chi2_diag, int ndf_diag, double pval_diag,
                   double chi2_full, int ndf_full, double pval_full)
{
    out << "\\begin{table}[htp!]\n";
    out << "\\centering\n";
    out << "\\begin{tabular}{|l|r|r|r|}\n\\hline\n";
    out << "Method & $\\chi^2$/ndf & ndf & $p$-value \\\\\n\\hline\n";

    auto fmtRow = [&](const string& method, double c2, int ndf, double pv) {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s & %.2f/%d & %d & %.3f \\\\",
                 method.c_str(), c2, ndf, ndf, pv);
        out << buf << "\n";
    };

    fmtRow("Diagonal", chi2_diag, ndf_diag, pval_diag);
    fmtRow("Full covariance", chi2_full, ndf_full, pval_full);

    out << "\\hline\n\\end{tabular}\n";
    out << "\\caption{Compatibility between combined pPb and pp " << observable
        << " distributions using combined statistical and systematic uncertainties"
        << " for inclusive $0 < \\ptz < 500$\\GeV and $0.5 < \\ptt < 15$\\GeV.}\n";
    out << "\\label{table:chisq_" << (observable.find("phi") != string::npos ? "dphi" : "deta") << "}\n";
    out << "\\end{table}\n";
}

void runChiSq(const JKCovResult& hiEta, const JKCovResult& hiPhi,
              const JKCovResult& ppEta, const JKCovResult& ppPhi,
              TH1D* hSystEta, TH1D* hSystPhi,
              const string& outDir, const string& suffix) {
    for (const string& obs : {"DeltaEta", "DeltaPhi"}) {
        bool isEta = (obs == "DeltaEta");
        const auto& hiRes = isEta ? hiEta : hiPhi;
        const auto& ppRes = isEta ? ppEta : ppPhi;
        TH1D* hSyst = isEta ? hSystEta : hSystPhi;
        int n = hiRes.central.size();

        TVectorD delta(n);
        for (int i = 0; i < n; ++i)
            delta(i) = hiRes.central[i] - ppRes.central[i];

        TMatrixDSym C_stat(n);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                C_stat(i, j) = hiRes.cov(i, j) + ppRes.cov(i, j);

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
            for (int i = 0; i < 3; ++i) {
                double sigma2 = C_total(i, i);
                if (sigma2 <= 0) continue;
                chi2_diag += delta(i) * delta(i) / sigma2;
                ++ndf_diag;
            }
            for (int i = 6; i < 9; ++i) {
                double sigma2 = C_total(i, i);
                if (sigma2 <= 0) continue;
                chi2_diag += delta(i) * delta(i) / sigma2;
                ++ndf_diag;
            }
        }
        double pval_diag = (ndf_diag > 0) ? TMath::Prob(chi2_diag, ndf_diag) : -1;

        vector<int> uniqueBins;
        if (isEta) {
            for (int i = 0; i < n / 2; ++i) uniqueBins.push_back(i);
        } else {
            for (int i = 0; i < 3; ++i) uniqueBins.push_back(i);
            for (int i = 6; i < 9; ++i) uniqueBins.push_back(i);
        }
        int nUnique = uniqueBins.size();
        TVectorD delta_unique(nUnique);
        TMatrixDSym C_unique(nUnique);
        for (int a = 0; a < nUnique; ++a) {
            delta_unique(a) = delta(uniqueBins[a]);
            for (int b = 0; b < nUnique; ++b)
                C_unique(a, b) = C_total(uniqueBins[a], uniqueBins[b]);
        }
        auto [chi2_full, ndf_full] = svdChiSq(delta_unique, C_unique);
        double pval_full = (ndf_full > 0) ? TMath::Prob(chi2_full, ndf_full) : -1;

        cout << "\n=== " << obs << suffix << " ===" << endl;
        cout << "Central values (HI): ";
        for (int i = 0; i < n; ++i) cout << hiRes.central[i] << " ";
        cout << endl;
        cout << "Central values (pp): ";
        for (int i = 0; i < n; ++i) cout << ppRes.central[i] << " ";
        cout << endl;
        cout << "Stat sigma (HI): ";
        for (int i = 0; i < n; ++i) cout << sqrt(max(0.0, hiRes.cov(i, i))) << " ";
        cout << endl;
        cout << "Stat sigma (pp): ";
        for (int i = 0; i < n; ++i) cout << sqrt(max(0.0, ppRes.cov(i, i))) << " ";
        cout << endl;
        cout << "Syst sigma (diff): ";
        for (int i = 0; i < n; ++i) cout << hSyst->GetBinContent(i + 1) << " ";
        cout << endl;
        cout << Form("Diagonal:        chi2=%.2f  ndf=%d  p=%.4f", chi2_diag, ndf_diag, pval_diag) << endl;
        cout << Form("Full covariance: chi2=%.2f  ndf=%d  p=%.4f", chi2_full, ndf_full, pval_full) << endl;

        string texPath = outDir + "/chisq_" + obs + suffix + ".tex";
        ofstream tex(texPath);
        string obsLabel = isEta ? "$\\dytz$" : "$\\dphitz$";
        writeTexTable(tex, obsLabel, chi2_diag, ndf_diag, pval_diag, chi2_full, ndf_full, pval_full);
        tex.close();
        cout << "Wrote: " << texPath << endl;
    }
}

int main(int argc, char* argv[]) {
    CommandLine CL(argc, argv);
    string mode = CL.Get("Mode", "2D");
    string systFile = CL.Get("SystematicsFile");
    string outDir   = CL.Get("OutputDir", "output");
    mkdir(outDir.c_str(), 0755);

    TFile* fSyst = TFile::Open(systFile.c_str(), "READ");
    if (!fSyst || fSyst->IsZombie()) { cerr << "Cannot open syst: " << systFile << endl; return 1; }
    TH1D* hSystEta = (TH1D*)fSyst->Get("DifferenceTotal_DeltaEta");
    TH1D* hSystPhi = (TH1D*)fSyst->Get("DifferenceTotal_DeltaPhi");
    if (!hSystEta || !hSystPhi) { cerr << "Missing DifferenceTotal histograms" << endl; return 1; }
    hSystEta->SetDirectory(0); hSystPhi->SetDirectory(0);
    fSyst->Close();

    if (mode == "2D") {
        string pPbRawFile = CL.Get("pPbRaw");
        string PbPRawFile = CL.Get("PbPRaw");
        string ppRawFile  = CL.Get("ppRaw");
        string pPbNosubFile = CL.Get("pPbNosub");

        TFile* fPPbRaw = TFile::Open(pPbRawFile.c_str(), "READ");
        TFile* fPbPRaw = TFile::Open(PbPRawFile.c_str(), "READ");
        TFile* fppRaw  = TFile::Open(ppRawFile.c_str(), "READ");
        for (auto& [nm, f] : vector<pair<string, TFile*>>{
                {pPbRawFile, fPPbRaw}, {PbPRawFile, fPbPRaw}, {ppRawFile, fppRaw}})
            if (!f || f->IsZombie()) { cerr << "Cannot open: " << nm << endl; return 1; }

        auto evsPPb = loadJK2D(fPPbRaw);
        auto evsPbP = loadJK2D(fPbPRaw);
        auto evspp  = loadJK2D(fppRaw);
        cout << "JK events: pPb=" << evsPPb.size() << " Pbp=" << evsPbP.size()
             << " pp=" << evspp.size() << endl;

        vector<JKEvent2D> evsHI;
        evsHI.reserve(evsPPb.size() + evsPbP.size());
        evsHI.insert(evsHI.end(), evsPPb.begin(), evsPPb.end());
        evsHI.insert(evsHI.end(), evsPbP.begin(), evsPbP.end());

        TFile* fPPbNosub = TFile::Open(pPbNosubFile.c_str(), "READ");
        TH2D* hRef = (TH2D*)fPPbNosub->Get("hData_0.5_15");
        int nEta = hRef->GetNbinsX(), nPhi = hRef->GetNbinsY();
        TH1D* etaAxis = hRef->ProjectionX("_etaAxis"); etaAxis->Reset(); etaAxis->SetDirectory(0);
        TH1D* phiAxis = hRef->ProjectionY("_phiAxis"); phiAxis->Reset(); phiAxis->SetDirectory(0);
        fPPbNosub->Close();

        int phiFirst = 4, phiLast = 6;
        double etaScale = 0.5, phiScale = 1.0;

        cout << "Computing pooled HI jackknife covariance (2D)..." << endl;
        auto hiEta = computePooledJKCovariance(evsHI, nEta, nPhi, phiFirst, phiLast, etaScale, phiScale, etaAxis, phiAxis, true);
        auto hiPhi = computePooledJKCovariance(evsHI, nEta, nPhi, phiFirst, phiLast, etaScale, phiScale, etaAxis, phiAxis, false);
        cout << "HI JK valid: eta=" << hiEta.validEvents << " phi=" << hiPhi.validEvents << endl;

        cout << "Computing pp jackknife covariance (2D)..." << endl;
        auto ppEta = computePooledJKCovariance(evspp, nEta, nPhi, phiFirst, phiLast, etaScale, phiScale, etaAxis, phiAxis, true);
        auto ppPhi = computePooledJKCovariance(evspp, nEta, nPhi, phiFirst, phiLast, etaScale, phiScale, etaAxis, phiAxis, false);
        cout << "pp JK valid: eta=" << ppEta.validEvents << " phi=" << ppPhi.validEvents << endl;

        runChiSq(hiEta, hiPhi, ppEta, ppPhi, hSystEta, hSystPhi, outDir, "");

        fPPbRaw->Close(); fPbPRaw->Close(); fppRaw->Close();
    }
    else if (mode == "projected") {
        string pPbFile = CL.Get("pPbResult");
        string PbPFile = CL.Get("PbPResult");
        string ppFile  = CL.Get("ppResult");
        string trkTag  = CL.Get("TrkTag");

        TFile* fPPb = TFile::Open(pPbFile.c_str(), "READ");
        TFile* fPbP = TFile::Open(PbPFile.c_str(), "READ");
        TFile* fpp  = TFile::Open(ppFile.c_str(), "READ");
        for (auto& [nm, f] : vector<pair<string, TFile*>>{
                {pPbFile, fPPb}, {PbPFile, fPbP}, {ppFile, fpp}})
            if (!f || f->IsZombie()) { cerr << "Cannot open: " << nm << endl; return 1; }

        cout << "Loading JK projections for trkPT " << trkTag << "..." << endl;
        auto evsPPb = loadJKProjected(fPPb, trkTag);
        auto evsPbP = loadJKProjected(fPbP, trkTag);
        auto evspp  = loadJKProjected(fpp, trkTag);
        cout << "JK events: pPb=" << evsPPb.size() << " Pbp=" << evsPbP.size()
             << " pp=" << evspp.size() << endl;

        vector<JKEvent1D> evsHI;
        evsHI.reserve(evsPPb.size() + evsPbP.size());
        evsHI.insert(evsHI.end(), evsPPb.begin(), evsPPb.end());
        evsHI.insert(evsHI.end(), evsPbP.begin(), evsPbP.end());

        TH1D* etaAxis = (TH1D*)fPPb->Get(("DeltaEta_Result" + trkTag).c_str());
        TH1D* phiAxis = (TH1D*)fPPb->Get(("DeltaPhi_Result" + trkTag).c_str());
        if (!etaAxis || !phiAxis) { cerr << "Missing axis histograms" << endl; return 1; }
        etaAxis->SetDirectory(0); phiAxis->SetDirectory(0);

        cout << "Computing pooled HI jackknife covariance (projected)..." << endl;
        auto hiEta = computePooledJKCov1D(evsHI, true, etaAxis);
        auto hiPhi = computePooledJKCov1D(evsHI, false, phiAxis);
        cout << "HI JK valid: eta=" << hiEta.validEvents << " phi=" << hiPhi.validEvents << endl;

        cout << "Computing pp jackknife covariance (projected)..." << endl;
        auto ppEta = computePooledJKCov1D(evspp, true, etaAxis);
        auto ppPhi = computePooledJKCov1D(evspp, false, phiAxis);
        cout << "pp JK valid: eta=" << ppEta.validEvents << " phi=" << ppPhi.validEvents << endl;

        cout << "Computing correct central values from 2D combination..." << endl;
        auto centrals2D = compute2DCombinedCentrals(fPPb, fPbP, fpp, trkTag);
        hiEta.central = centrals2D.hiEta;
        hiPhi.central = centrals2D.hiPhi;
        ppEta.central = centrals2D.ppEta;
        ppPhi.central = centrals2D.ppPhi;

        int nE = hiEta.central.size(), nP = hiPhi.central.size();
        for (int i = 0; i < nE; ++i) for (int j = 0; j < nE; ++j) { hiEta.cov(i, j) *= 0.25; ppEta.cov(i, j) *= 0.25; }
        for (int i = 0; i < nP; ++i) for (int j = 0; j < nP; ++j) { hiPhi.cov(i, j) *= 0.25; ppPhi.cov(i, j) *= 0.25; }

        string suffix = "_trkPT" + trkTag;
        runChiSq(hiEta, hiPhi, ppEta, ppPhi, hSystEta, hSystPhi, outDir, suffix);

        fPPb->Close(); fPbP->Close(); fpp->Close();
    }
    else {
        cerr << "Unknown mode: " << mode << endl;
        return 1;
    }

    return 0;
}
