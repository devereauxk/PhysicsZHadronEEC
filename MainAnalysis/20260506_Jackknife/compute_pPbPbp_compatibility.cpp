#include <TCanvas.h>
#include <TColor.h>
#include <TDecompSVD.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TMatrixDSym.h>
#include <TMath.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVectorD.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

struct CompatibilityResult {
    string ObservableLabel;
    string ErrorModel;
    int TotalBins = 0;
    int UsedBins  = 0;
    double Chi2       = 0;
    double Chi2PerNDF = 0;
    double PValue     = 0;
};

// ──────────────────────────────────────────────────────────────────────────────
// Diagonal chi-square (existing method)
// ──────────────────────────────────────────────────────────────────────────────

CompatibilityResult computeCompatibility(TH1D *pPb, TH1D *PbP,
    const string &observableLabel, const string &errorModel)
{
    CompatibilityResult result;
    result.ObservableLabel = observableLabel;
    result.ErrorModel = errorModel;
    if (pPb == nullptr || PbP == nullptr)
        return result;

    result.TotalBins = min(pPb->GetNbinsX(), PbP->GetNbinsX());
    for (int bin = 1; bin <= result.TotalBins; ++bin) {
        const double sigma2 = pPb->GetBinError(bin) * pPb->GetBinError(bin)
            + PbP->GetBinError(bin) * PbP->GetBinError(bin);
        if (sigma2 <= 0)
            continue;

        const double delta = pPb->GetBinContent(bin) - PbP->GetBinContent(bin);
        result.Chi2 += delta * delta / sigma2;
        result.UsedBins = result.UsedBins + 1;
    }

    if (result.UsedBins > 0) {
        result.Chi2PerNDF = result.Chi2 / result.UsedBins;
        result.PValue = TMath::Prob(result.Chi2, result.UsedBins);
    }

    return result;
}

TH1D *loadHistogram(TFile &file, const string &name, const string &cloneName)
{
    TH1D *histogram = (TH1D *)file.Get(name.c_str());
    if (histogram == nullptr)
        return nullptr;

    histogram = (TH1D *)histogram->Clone(cloneName.c_str());
    histogram->SetDirectory(nullptr);
    return histogram;
}

// ──────────────────────────────────────────────────────────────────────────────
// Full covariance chi-square (new method)
// ──────────────────────────────────────────────────────────────────────────────

struct JackknifeEventData {
    double sigNZ = 0, mixNZ = 0;
    vector<float> sigEta, mixEta, sigPhi, mixPhi;
};

// Load all per-event contributions from the JackknifeProjection TTree
vector<JackknifeEventData> loadJackknifeEvents(TFile &file, const string &treeName)
{
    TTree *tree = (TTree *)file.Get(treeName.c_str());
    if (tree == nullptr) {
        cerr << "Cannot find tree " << treeName << " in " << file.GetName() << endl;
        return {};
    }

    double sigNZ = 0, mixNZ = 0;
    vector<float> *sigEta = nullptr, *mixEta = nullptr;
    vector<float> *sigPhi = nullptr, *mixPhi = nullptr;
    tree->SetBranchAddress("SignalNZ",  &sigNZ);
    tree->SetBranchAddress("MixNZ",    &mixNZ);
    tree->SetBranchAddress("SignalEta", &sigEta);
    tree->SetBranchAddress("MixEta",   &mixEta);
    tree->SetBranchAddress("SignalPhi", &sigPhi);
    tree->SetBranchAddress("MixPhi",   &mixPhi);

    long long N = tree->GetEntries();
    vector<JackknifeEventData> events(N);
    for (long long k = 0; k < N; ++k) {
        tree->GetEntry(k);
        events[k].sigNZ  = sigNZ;
        events[k].mixNZ  = mixNZ;
        events[k].sigEta = *sigEta;
        events[k].mixEta = *mixEta;
        events[k].sigPhi = *sigPhi;
        events[k].mixPhi = *mixPhi;
    }
    return events;
}

// Build the full N×N jackknife covariance matrix for a 1D projected result histogram.
// Matches the LOO formula in makeProjection.C::computeProjectedJackknifeSigma exactly:
//   fullValue[i]  = totalSig[i]/totalSigNZ - totalMix[i]/totalMixNZ
//   looValue[i]   = (totalSig[i]-sig_k[i])/(totalSigNZ-sigNZ_k)
//                 - (totalMix[i]-mix_k[i])/(totalMixNZ-mixNZ_k)
// Result is in result-histogram units (already divided by binWidth).
TMatrixDSym buildCovMatrix(const vector<JackknifeEventData> &events,
    TH1D *axisHist, bool useEta)
{
    int nBins = axisHist->GetNbinsX();
    TMatrixDSym C(nBins);
    if (events.empty()) return C;

    // Accumulate totals
    vector<double> totalSig(nBins, 0), totalMix(nBins, 0);
    double totalSigNZ = 0, totalMixNZ = 0;
    for (const auto &ev : events) {
        totalSigNZ += ev.sigNZ;
        totalMixNZ += ev.mixNZ;
        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;
        for (int i = 0; i < nBins && i < (int)s.size(); ++i) {
            totalSig[i] += s[i];
            totalMix[i] += m[i];
        }
    }
    if (totalSigNZ <= 0 || totalMixNZ <= 0) return C;

    // Bin widths for converting to result-histogram units
    vector<double> bw(nBins);
    for (int i = 0; i < nBins; ++i) bw[i] = axisHist->GetBinWidth(i + 1);

    // Full estimates (pre-binWidth; same as makeProjection internal)
    vector<double> theta(nBins);
    for (int i = 0; i < nBins; ++i)
        theta[i] = totalSig[i] / totalSigNZ - totalMix[i] / totalMixNZ;

    // Accumulate outer products of LOO deviations
    int validN = 0;
    for (const auto &ev : events) {
        double leftSigNZ = totalSigNZ - ev.sigNZ;
        double leftMixNZ = totalMixNZ - ev.mixNZ;
        if (leftSigNZ <= 0 || leftMixNZ <= 0) continue;
        ++validN;

        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;

        // Deviation in result-histogram units (divided by binWidth)
        vector<double> d(nBins);
        for (int i = 0; i < nBins; ++i) {
            double thetaLOO = (totalSig[i] - s[i]) / leftSigNZ
                            - (totalMix[i] - m[i]) / leftMixNZ;
            d[i] = (thetaLOO - theta[i]) / bw[i];
        }
        for (int i = 0; i < nBins; ++i)
            for (int j = 0; j < nBins; ++j)
                C(i, j) += d[i] * d[j];
    }

    if (validN > 1)
        C *= (double)(validN - 1) / validN;

    return C;
}

// Full covariance chi-square: Δᵀ C_tot⁺ Δ  (Moore-Penrose pseudoinverse)
// ndf = effective rank of C_tot, determined by retaining singular values
// above tol = sv_max * 1e-8  (covers double-precision noise and symmetry-induced zeros)
CompatibilityResult computeFullCovCompatibility(TH1D *pPb, TH1D *PbP,
    const TMatrixDSym &CpPb, const TMatrixDSym &CPbP, const string &obsLabel)
{
    CompatibilityResult result;
    result.ObservableLabel = obsLabel;
    result.ErrorModel      = "FullCovariance";

    int nBins = min(pPb->GetNbinsX(), PbP->GetNbinsX());
    result.TotalBins = nBins;

    TMatrixDSym Ctot = CpPb + CPbP;

    TVectorD delta(nBins);
    for (int i = 0; i < nBins; ++i)
        delta(i) = pPb->GetBinContent(i + 1) - PbP->GetBinContent(i + 1);

    TDecompSVD svd(Ctot);
    const TVectorD &sv = svd.GetSig();
    int nsv = sv.GetNrows();

    // Singular value threshold: retain directions with sv > tol * sv_max.
    // 1e-8 captures double-precision noise and exact zeros from symmetry degeneracy.
    double svMax = (nsv > 0) ? sv(0) : 0;
    double tol   = svMax * 1e-8;
    int rank = 0;
    for (int k = 0; k < nsv; ++k)
        if (sv(k) > tol) ++rank;

    cout << "  [" << obsLabel << "]"
         << "  sv_max=" << svMax
         << "  sv_min=" << sv(nsv - 1)
         << "  cond=" << (sv(nsv-1) > 0 ? svMax / sv(nsv-1) : -1)
         << "  effective rank=" << rank << "/" << nBins << endl;

    if (rank == 0) {
        cerr << "Zero-rank covariance for " << obsLabel << endl;
        return result;
    }
    result.UsedBins = rank;  // ndf = effective rank

    // Build pseudoinverse: C⁺ = V · diag(1/sv_k for sv_k>tol, else 0) · Uᵀ
    const TMatrixD &U = svd.GetU();
    const TMatrixD &V = svd.GetV();

    TMatrixD Cpinv(nBins, nBins);
    for (int k = 0; k < nsv; ++k) {
        if (sv(k) <= tol) continue;
        double invSv = 1.0 / sv(k);
        for (int i = 0; i < nBins; ++i)
            for (int j = 0; j < nBins; ++j)
                Cpinv(i, j) += V(i, k) * invSv * U(j, k);
    }

    TVectorD Cpinv_delta = Cpinv * delta;
    result.Chi2       = delta * Cpinv_delta;
    result.Chi2PerNDF = result.Chi2 / rank;
    result.PValue     = TMath::Prob(result.Chi2, rank);

    return result;
}

// ──────────────────────────────────────────────────────────────────────────────
// Correlation heatmaps
// ──────────────────────────────────────────────────────────────────────────────

void setDivergingPalette()
{
    // Blue → white → red, 255 colours
    const int nColors = 255, nPts = 3;
    double stops[] = {0.00, 0.50, 1.00};
    double red[]   = {0.12, 1.00, 0.84};
    double green[] = {0.47, 1.00, 0.10};
    double blue[]  = {0.71, 1.00, 0.11};
    TColor::CreateGradientColorTable(nPts, stops, red, green, blue, nColors);
    gStyle->SetNumberContours(nColors);
}

void setSequentialPalette()
{
    // White → blue, 255 colours
    const int nColors = 255, nPts = 2;
    double stops[] = {0.00, 1.00};
    double red[]   = {1.00, 0.12};
    double green[] = {1.00, 0.47};
    double blue[]  = {1.00, 0.71};
    TColor::CreateGradientColorTable(nPts, stops, red, green, blue, nColors);
    gStyle->SetNumberContours(nColors);
}

// Build a correlation TH2D using only the sub-range [firstBin, lastBin] (0-indexed).
// The resulting TH2D has (lastBin-firstBin+1) bins on each axis matching the axis range.
TH2D *makeCorrelationH2(const TMatrixDSym &C, TH1D *axisHist, const char *name,
                         int firstBin, int lastBin)
{
    if (lastBin < 0) lastBin = axisHist->GetNbinsX() - 1;
    int n   = lastBin - firstBin + 1;
    double lo = axisHist->GetBinLowEdge(firstBin + 1);
    double hi = axisHist->GetBinLowEdge(lastBin + 1) + axisHist->GetBinWidth(lastBin + 1);

    TH2D *h = new TH2D(name, "", n, lo, hi, n, lo, hi);
    for (int i = 0; i < n; ++i) {
        int ii  = firstBin + i;
        double sii = (C(ii,ii) > 0) ? sqrt(C(ii,ii)) : 0;
        for (int j = 0; j < n; ++j) {
            int jj = firstBin + j;
            double sjj = (C(jj,jj) > 0) ? sqrt(C(jj,jj)) : 0;
            double rho = (sii > 0 && sjj > 0) ? C(ii,jj) / (sii * sjj) : 0;
            h->SetBinContent(i + 1, j + 1, rho);
        }
    }
    return h;
}

// Produces a two-panel (pPb | PbP) correlation matrix PDF.
// firstBin/lastBin are 0-indexed into the full covariance matrix / axisHist.
// Automatically selects z-range: [-1,1] if any bin is negative, else [0,1].
void saveCorrelationHeatmaps(const TMatrixDSym &CpPb, const TMatrixDSym &CPbP,
    TH1D *axisHist, const string &obsKey, const string &outPath,
    int firstBin, int lastBin)
{
    string axTitle = (obsKey == "Eta") ? "#Delta y_{ch,Z}" : "#Delta#phi_{ch,Z}";

    gStyle->SetOptStat(0);
    gStyle->SetPadRightMargin(0.15);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadTopMargin(0.08);

    // Build both histograms first so we can check for negative values
    TH2D *hpPb = makeCorrelationH2(CpPb, axisHist,
                                    ("hCorrpPb_" + obsKey).c_str(), firstBin, lastBin);
    TH2D *hPbP = makeCorrelationH2(CPbP, axisHist,
                                    ("hCorrPbP_" + obsKey).c_str(), firstBin, lastBin);

    int n = lastBin - firstBin + 1;
    double minVal = 0;
    for (int ix = 1; ix <= n; ++ix)
        for (int iy = 1; iy <= n; ++iy) {
            minVal = min(minVal, hpPb->GetBinContent(ix, iy));
            minVal = min(minVal, hPbP->GetBinContent(ix, iy));
        }

    bool hasNegative = (minVal < -1e-9);
    double zLo = hasNegative ? -1.0 : 0.0;
    cout << "  [" << obsKey << "] min correlation = " << minVal
         << "  => z-range [" << zLo << ", 1]" << endl;

    if (hasNegative) setDivergingPalette();
    else             setSequentialPalette();

    hpPb->GetZaxis()->SetRangeUser(zLo, 1.0);
    hPbP->GetZaxis()->SetRangeUser(zLo, 1.0);

    TCanvas *c = new TCanvas("cCorr", "cCorr", 1400, 600);
    c->Divide(2, 1);

    auto drawPanel = [&](TVirtualPad *pad, TH2D *h, const char *label) {
        pad->cd();
        pad->SetRightMargin(0.15);
        h->GetXaxis()->SetTitle(axTitle.c_str());
        h->GetYaxis()->SetTitle(axTitle.c_str());
        h->GetZaxis()->SetTitle("#rho");
        h->Draw("COLZ");
        TLatex latex;
        latex.SetNDC(); latex.SetTextSize(0.052); latex.SetTextFont(62);
        latex.DrawLatex(0.17, 0.88, label);
    };

    drawPanel(c->cd(1), hpPb, "pPb 8.16 TeV");
    drawPanel(c->cd(2), hPbP, "Pbp 8.16 TeV");

    c->SaveAs(outPath.c_str());
    delete hpPb;
    delete hPbP;
    delete c;
}

// ──────────────────────────────────────────────────────────────────────────────
// Output writers
// ──────────────────────────────────────────────────────────────────────────────

void writeMarkdown(const string &path, const vector<CompatibilityResult> &results)
{
    ofstream out(path);
    out << "# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance\n\n";
    out << "| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |\n";
    out << "| --- | --- | ---: | ---: | ---: | ---: | ---: |\n";
    out << setprecision(17);
    for (const CompatibilityResult &r : results)
        out << "| `" << r.ObservableLabel << "` | **" << r.ErrorModel << "** | "
            << r.UsedBins << " | " << r.Chi2 << " | " << r.UsedBins << " | "
            << r.Chi2PerNDF << " | " << r.PValue << " |\n";
    out << "\n";
    for (const CompatibilityResult &r : results)
        out << "- `" << r.ObservableLabel << "` / **" << r.ErrorModel << "** excluded "
            << (r.TotalBins - r.UsedBins) << " bins because the combined variance was zero.\n";
}

void writeTSV(const string &path, const vector<CompatibilityResult> &results)
{
    ofstream out(path);
    out << "Observable\tErrorModel\tN_bins\tchi2\tndf\tchi2_per_ndf\tp_value\tExcludedBins\n";
    out << setprecision(17);
    for (const CompatibilityResult &r : results)
        out << r.ObservableLabel << '\t' << r.ErrorModel << '\t' << r.UsedBins << '\t'
            << r.Chi2 << '\t' << r.UsedBins << '\t' << r.Chi2PerNDF << '\t'
            << r.PValue << '\t' << (r.TotalBins - r.UsedBins) << '\n';
}

void writeTeX(const string &path, const vector<CompatibilityResult> &results)
{
    ofstream out(path);
    out << "\\begin{table}[htbp]\n\\centering\n";
    out << "\\begin{tabular}{|l|l|r|r|r|r|r|}\n\\hline\n";
    out << "Observable & Error model & $N_{\\mathrm{bins}}$ & $\\chi^2$ & ndf & $\\chi^2/\\mathrm{ndf}$ & $p$-value \\\\\n\\hline\n";
    out << setprecision(4);
    for (const CompatibilityResult &r : results)
        out << "\\texttt{" << r.ObservableLabel << "} & " << r.ErrorModel << " & "
            << r.UsedBins << " & " << r.Chi2 << " & " << r.UsedBins << " & "
            << r.Chi2PerNDF << " & " << r.PValue << " \\\\\n";
    out << "\\hline\n\\end{tabular}\n";
    out << "\\caption{pPb vs Pbp compatibility comparison using Sumw2, diagonal jackknife, and full jackknife covariance statistical uncertainties.}\n";
    out << "\\label{tab:ppbp-compatibility}\n\\end{table}\n";
}

// ──────────────────────────────────────────────────────────────────────────────
// main
// ──────────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string tag       = CL.Get("Tag",       "ZV9_trkV27_nmix10");
    const string baseDir   = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outputDir = CL.Get("OutputDir", "output");
    const string trkRange  = CL.Get("TrkRange",  "0.5_15");
    // Heatmap sub-range (0-indexed into the full N-bin histogram):
    //   20-bin defaults: DeltaEta 0-4 → 10..19, DeltaPhi 0-pi → 5..14
    //   12-bin defaults: DeltaEta 0-4 →  6..11, DeltaPhi 0-pi → 3..8
    const int etaFirstBin  = CL.GetInt("EtaFirstBin", 10);
    const int etaLastBin   = CL.GetInt("EtaLastBin",  19);
    const int phiFirstBin  = CL.GetInt("PhiFirstBin",  5);
    const int phiLastBin   = CL.GetInt("PhiLastBin",  14);

    const string pPbFileName = baseDir + "/pPb_trkResidual_" + tag + "_ZPT0_500-result.root";
    const string PbPFileName = baseDir + "/PbP_trkResidual_" + tag + "_ZPT0_500-result.root";

    TFile pPbFile(pPbFileName.c_str(), "READ");
    TFile PbPFile(PbPFileName.c_str(), "READ");
    if (pPbFile.IsZombie() || PbPFile.IsZombie()) {
        cerr << "Failed to open compatibility inputs:\n"
            << "  " << pPbFileName << "\n"
            << "  " << PbPFileName << endl;
        return 1;
    }

    const string jackEtaKey = "DeltaEta_Result" + trkRange;
    const string jackPhiKey = "DeltaPhi_Result" + trkRange;
    const string sw2EtaKey  = "DeltaEta_ResultSumw2" + trkRange;
    const string sw2PhiKey  = "DeltaPhi_ResultSumw2" + trkRange;

    TH1D *pPbJkEta  = loadHistogram(pPbFile, jackEtaKey, "pPb_jk_eta");
    TH1D *PbPJkEta  = loadHistogram(PbPFile, jackEtaKey, "PbP_jk_eta");
    TH1D *pPbSw2Eta = loadHistogram(pPbFile, sw2EtaKey,  "pPb_sw2_eta");
    TH1D *PbPSw2Eta = loadHistogram(PbPFile, sw2EtaKey,  "PbP_sw2_eta");

    TH1D *pPbJkPhi  = loadHistogram(pPbFile, jackPhiKey, "pPb_jk_phi");
    TH1D *PbPJkPhi  = loadHistogram(PbPFile, jackPhiKey, "PbP_jk_phi");
    TH1D *pPbSw2Phi = loadHistogram(pPbFile, sw2PhiKey,  "pPb_sw2_phi");
    TH1D *PbPSw2Phi = loadHistogram(PbPFile, sw2PhiKey,  "PbP_sw2_phi");

    if (!pPbJkEta || !PbPJkEta || !pPbSw2Eta || !PbPSw2Eta ||
        !pPbJkPhi || !PbPJkPhi || !pPbSw2Phi || !PbPSw2Phi) {
        cerr << "Missing one or more compatibility histograms." << endl;
        return 1;
    }

    // ── Diagonal chi-square (Sumw2 and diagonal Jackknife) ──────────────────
    vector<CompatibilityResult> results;
    results.push_back(computeCompatibility(pPbSw2Eta, PbPSw2Eta, jackEtaKey, "Sumw2"));
    results.push_back(computeCompatibility(pPbJkEta,  PbPJkEta,  jackEtaKey, "Jackknife"));

    results.push_back(computeCompatibility(pPbSw2Phi, PbPSw2Phi, jackPhiKey, "Sumw2"));
    results.push_back(computeCompatibility(pPbJkPhi,  PbPJkPhi,  jackPhiKey, "Jackknife"));

    // ── Full covariance chi-square ───────────────────────────────────────────
    const string treeName = "JackknifeProjection" + trkRange;
    cout << "Loading jackknife events from pPb..." << endl;
    vector<JackknifeEventData> pPbEvents = loadJackknifeEvents(pPbFile, treeName);
    cout << "Loading jackknife events from PbP..." << endl;
    vector<JackknifeEventData> PbPEvents = loadJackknifeEvents(PbPFile, treeName);

    if (pPbEvents.empty() || PbPEvents.empty()) {
        cerr << "Could not load jackknife event trees — skipping full covariance." << endl;
    } else {
        cout << "pPb events: " << pPbEvents.size()
             << "  PbP events: " << PbPEvents.size() << endl;

        cout << "Building DeltaEta covariance matrices..." << endl;
        TMatrixDSym CpPb_eta = buildCovMatrix(pPbEvents, pPbJkEta, true);
        TMatrixDSym CPbP_eta = buildCovMatrix(PbPEvents, PbPJkEta, true);

        cout << "Building DeltaPhi covariance matrices..." << endl;
        TMatrixDSym CpPb_phi = buildCovMatrix(pPbEvents, pPbJkPhi, false);
        TMatrixDSym CPbP_phi = buildCovMatrix(PbPEvents, PbPJkPhi, false);

        // Insert FullCovariance right after the Jackknife row for each observable
        // (results currently: sw2_eta, jk_eta, sw2_phi, jk_phi → insert at 2 and 5)
        CompatibilityResult fcEta = computeFullCovCompatibility(
            pPbJkEta, PbPJkEta, CpPb_eta, CPbP_eta, jackEtaKey);
        CompatibilityResult fcPhi = computeFullCovCompatibility(
            pPbJkPhi, PbPJkPhi, CpPb_phi, CPbP_phi, jackPhiKey);

        results.insert(results.begin() + 2, fcEta);
        results.push_back(fcPhi);

        // Heatmaps — physical half only (EtaFirstBin..EtaLastBin, PhiFirstBin..PhiLastBin)
        gSystem->mkdir(outputDir.c_str(), true);
        cout << "Saving DeltaEta correlation heatmap (bins " << etaFirstBin << ".." << etaLastBin << ")..." << endl;
        saveCorrelationHeatmaps(CpPb_eta, CPbP_eta, pPbJkEta, "Eta",
            outputDir + "/pPbPbp_DeltaEta_correlation_matrix.pdf", etaFirstBin, etaLastBin);
        cout << "Saving DeltaPhi correlation heatmap (bins " << phiFirstBin << ".." << phiLastBin << ")..." << endl;
        saveCorrelationHeatmaps(CpPb_phi, CPbP_phi, pPbJkPhi, "Phi",
            outputDir + "/pPbPbp_DeltaPhi_correlation_matrix.pdf", phiFirstBin, phiLastBin);
    }

    // ── Write outputs ────────────────────────────────────────────────────────
    gSystem->mkdir(outputDir.c_str(), true);
    writeMarkdown(outputDir + "/pPbPbp_compatibility.md", results);
    writeTSV(outputDir + "/pPbPbp_compatibility.tsv", results);
    writeTeX(outputDir + "/pPbPbp_compatibility.tex", results);

    cout << "\nResults:\n";
    for (const CompatibilityResult &r : results)
        cout << r.ObservableLabel << " [" << r.ErrorModel << "]"
             << "  chi2=" << r.Chi2 << "  ndf=" << r.UsedBins
             << "  p=" << r.PValue << endl;

    return 0;
}
