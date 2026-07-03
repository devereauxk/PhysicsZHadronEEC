#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TGraphAsymmErrors.h>
#include <TStyle.h>
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"

// ---------------------------------------------------------------------------
// Projection window (12x12 signed convention)
// ---------------------------------------------------------------------------
struct ResultProjectionWindow {
    int DeltaPhiXFirst = 7;
    int DeltaPhiXLast = 12;
    int DeltaEtaYFirst = 4;
    int DeltaEtaYLast = 6;
};

// ---------------------------------------------------------------------------
// 2D fourfold symmetrization (12x12)
// ---------------------------------------------------------------------------
void Symmetrize2DFourfold(TH2D *h)
{
    if (!h) return;
    int nx = h->GetNbinsX(), ny = h->GetNbinsY();
    if (nx != 12 || ny != 12) return;
    TH2D *copy = (TH2D *)h->Clone("_sym2d_tmp");
    copy->SetDirectory(nullptr);
    for (int i = 1; i <= nx; ++i) {
        int mi = 13 - i;
        for (int j = 1; j <= ny; ++j) {
            int mj = (j <= 6) ? (7 - j) : (19 - j);
            double v = 0.25 * (copy->GetBinContent(i, j) + copy->GetBinContent(mi, j)
                              + copy->GetBinContent(i, mj) + copy->GetBinContent(mi, mj));
            double e = 0.25 * sqrt(pow(copy->GetBinError(i, j), 2)
                              + pow(copy->GetBinError(mi, j), 2)
                              + pow(copy->GetBinError(i, mj), 2)
                              + pow(copy->GetBinError(mi, mj), 2));
            h->SetBinContent(i, j, v);
            h->SetBinError(i, j, e);
        }
    }
    delete copy;
}

// ---------------------------------------------------------------------------
// 1D symmetrization for DeltaPhi (12 bins)
// ---------------------------------------------------------------------------
void Symmetrize1DPhi(TH1D *h)
{
    if (!h || h->GetNbinsX() != 12) return;
    for (int j = 0; j < 3; ++j) {
        int mj = 5 - j;
        double v = 0.5 * (h->GetBinContent(j+1) + h->GetBinContent(mj+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j+1), 2) + pow(h->GetBinError(mj+1), 2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
    for (int j = 6; j < 9; ++j) {
        int mj = 17 - j;
        double v = 0.5 * (h->GetBinContent(j+1) + h->GetBinContent(mj+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j+1), 2) + pow(h->GetBinError(mj+1), 2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
}

// ---------------------------------------------------------------------------
// 1D symmetrization for DeltaEta (12 bins): mirror i <-> n-1-i (0-indexed)
// ---------------------------------------------------------------------------
void Symmetrize1DEta(TH1D *h)
{
    if (!h || h->GetNbinsX() != 12) return;
    int n = h->GetNbinsX();
    for (int i = 0; i < n / 2; ++i) {
        int mi = n - 1 - i;
        double v = 0.5 * (h->GetBinContent(i+1) + h->GetBinContent(mi+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(i+1), 2) + pow(h->GetBinError(mi+1), 2));
        h->SetBinContent(i+1, v); h->SetBinError(i+1, e);
        h->SetBinContent(mi+1, v); h->SetBinError(mi+1, e);
    }
}

// ---------------------------------------------------------------------------
// Jackknife helpers
// ---------------------------------------------------------------------------
struct JackknifeProjectionContribution {
    double SignalNZ = 0;
    double MixNZ = 0;
    vector<float> SignalEta;
    vector<float> MixEta;
    vector<float> SignalPhi;
    vector<float> MixPhi;
};

void AppendJackknifeProjectionContributions(TFile *file, const string &treeName,
    vector<JackknifeProjectionContribution> &events)
{
    if (!file) return;
    TTree *tree = (TTree *)file->Get(treeName.c_str());
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

    for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
        tree->GetEntry(entry);
        JackknifeProjectionContribution c;
        c.SignalNZ = signalNZ;  c.MixNZ = mixNZ;
        c.SignalEta = *signalEta; c.MixEta = *mixEta;
        c.SignalPhi = *signalPhi; c.MixPhi = *mixPhi;
        events.push_back(c);
    }
}

vector<double> ComputeProjectedJackknifeSigma(const vector<JackknifeProjectionContribution> &events,
    const TH1D *fullHistogram, bool useEta)
{
    int nBins = fullHistogram->GetNbinsX();
    vector<double> sigma(nBins, 0);
    if (events.size() < 2) return sigma;

    double totalSignalNZ = 0, totalMixNZ = 0;
    vector<double> fullSignal(nBins, 0), fullMix(nBins, 0);
    for (auto &ev : events) {
        totalSignalNZ += ev.SignalNZ;
        totalMixNZ += ev.MixNZ;
        for (int i = 0; i < nBins; ++i) {
            fullSignal[i] += (useEta ? ev.SignalEta[i] : ev.SignalPhi[i]);
            fullMix[i] += (useEta ? ev.MixEta[i] : ev.MixPhi[i]);
        }
    }

    int validEvents = 0;
    for (auto &ev : events) {
        if (totalSignalNZ - ev.SignalNZ > 0 && totalMixNZ - ev.MixNZ > 0)
            validEvents++;
    }
    if (validEvents < 2) return sigma;

    for (int i = 1; i <= nBins; ++i) {
        double fullValue = fullSignal[i-1] / totalSignalNZ - fullMix[i-1] / totalMixNZ;
        double varianceSum = 0;
        for (auto &ev : events) {
            if (totalSignalNZ - ev.SignalNZ <= 0 || totalMixNZ - ev.MixNZ <= 0) continue;
            double sig = (fullSignal[i-1] - (useEta ? ev.SignalEta[i-1] : ev.SignalPhi[i-1]))
                / (totalSignalNZ - ev.SignalNZ);
            double mix = (fullMix[i-1] - (useEta ? ev.MixEta[i-1] : ev.MixPhi[i-1]))
                / (totalMixNZ - ev.MixNZ);
            double delta = (sig - mix) - fullValue;
            varianceSum += delta * delta;
        }
        sigma[i-1] = sqrt((validEvents - 1.0) / validEvents * varianceSum);
    }
    return sigma;
}

void ApplyCombinedDiagonalJackknifeErrors(TH1D *etaHist, TH1D *phiHist,
    const vector<JackknifeProjectionContribution> &eventsPPb,
    const vector<JackknifeProjectionContribution> &eventsPbP,
    double NZ_pPb, double NZ_PbP)
{
    double totalNZ = NZ_pPb + NZ_PbP;
    double w_pPb = NZ_pPb / totalNZ, w_PbP = NZ_PbP / totalNZ;

    auto etaSigPPb = ComputeProjectedJackknifeSigma(eventsPPb, etaHist, true);
    auto phiSigPPb = ComputeProjectedJackknifeSigma(eventsPPb, phiHist, false);
    auto etaSigPbP = ComputeProjectedJackknifeSigma(eventsPbP, etaHist, true);
    auto phiSigPbP = ComputeProjectedJackknifeSigma(eventsPbP, phiHist, false);

    for (int i = 1; i <= etaHist->GetNbinsX(); ++i)
        etaHist->SetBinError(i, sqrt(w_pPb*w_pPb*etaSigPPb[i-1]*etaSigPPb[i-1]
                                    + w_PbP*w_PbP*etaSigPbP[i-1]*etaSigPbP[i-1]));
    for (int i = 1; i <= phiHist->GetNbinsX(); ++i)
        phiHist->SetBinError(i, sqrt(w_pPb*w_pPb*phiSigPPb[i-1]*phiSigPPb[i-1]
                                    + w_PbP*w_PbP*phiSigPbP[i-1]*phiSigPbP[i-1]));
}

// ---------------------------------------------------------------------------
// CSV reader for HEPData
// ---------------------------------------------------------------------------
struct HEPDataResult {
    TH1D *hist = nullptr;
    TGraphAsymmErrors *systGraph = nullptr;
};

HEPDataResult ReadHEPDataCSV(const string &filename, const string &name)
{
    HEPDataResult result;

    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: cannot open " << filename << endl;
        return result;
    }

    vector<double> centers, lows, highs, values, stats, systs;
    string line;
    while (getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '"') continue;

        istringstream iss(line);
        double center, low, high, val, stat_p, stat_m, syst_p, syst_m;
        char comma;
        iss >> center >> comma >> low >> comma >> high >> comma >> val
            >> comma >> stat_p >> comma >> stat_m >> comma >> syst_p >> comma >> syst_m;

        centers.push_back(center);
        lows.push_back(low);
        highs.push_back(high);
        values.push_back(val);
        stats.push_back(stat_p);
        systs.push_back(fabs(syst_p));
    }

    int n = centers.size();
    if (n == 0) return result;

    vector<double> edges;
    for (int i = 0; i < n; ++i) edges.push_back(lows[i]);
    edges.push_back(highs[n-1]);

    result.hist = new TH1D(name.c_str(), "", n, edges.data());
    result.hist->SetDirectory(nullptr);
    result.systGraph = new TGraphAsymmErrors(n);

    for (int i = 0; i < n; ++i) {
        result.hist->SetBinContent(i+1, values[i]);
        result.hist->SetBinError(i+1, stats[i]);
        result.systGraph->SetPoint(i, centers[i], values[i]);
        double halfWidth = (highs[i] - lows[i]) / 2.0;
        result.systGraph->SetPointError(i, halfWidth, halfWidth, systs[i], systs[i]);
    }

    return result;
}

// ---------------------------------------------------------------------------
// Build a systematic band TGraphAsymmErrors from a central TH1D and a
// systematics TH1D (whose bin content = absolute magnitude)
// ---------------------------------------------------------------------------
TGraphAsymmErrors *BuildSystGraph(TH1D *central, TH1D *syst)
{
    int n = central->GetNbinsX();
    TGraphAsymmErrors *g = new TGraphAsymmErrors(n);
    for (int i = 0; i < n; ++i) {
        double x = central->GetBinCenter(i+1);
        double y = central->GetBinContent(i+1);
        double hw = central->GetBinWidth(i+1) / 2.0;
        double err = syst ? fabs(syst->GetBinContent(i+1)) : 0;
        g->SetPoint(i, x, y);
        g->SetPointError(i, hw, hw, err, err);
    }
    return g;
}

// ---------------------------------------------------------------------------
// Compute HI - pp difference histogram (same binning required)
// ---------------------------------------------------------------------------
TH1D *ComputeDifference(TH1D *hHI, TH1D *hPP, const char *name)
{
    TH1D *diff = (TH1D *)hHI->Clone(name);
    diff->SetDirectory(nullptr);
    diff->Add(hPP, -1);
    for (int i = 1; i <= diff->GetNbinsX(); ++i) {
        double eHI = hHI->GetBinError(i);
        double ePP = hPP->GetBinError(i);
        diff->SetBinError(i, sqrt(eHI*eHI + ePP*ePP));
    }
    return diff;
}

// ---------------------------------------------------------------------------
// Compute difference systematic band from two syst TGraphAsymmErrors
// ---------------------------------------------------------------------------
TGraphAsymmErrors *ComputeDiffSystGraph(TH1D *diffHist,
    TGraphAsymmErrors *systHI, TGraphAsymmErrors *systPP)
{
    int n = diffHist->GetNbinsX();
    TGraphAsymmErrors *g = new TGraphAsymmErrors(n);
    for (int i = 0; i < n; ++i) {
        double x = diffHist->GetBinCenter(i+1);
        double y = diffHist->GetBinContent(i+1);
        double hw = diffHist->GetBinWidth(i+1) / 2.0;
        double eHI_lo = 0, eHI_hi = 0, ePP_lo = 0, ePP_hi = 0;
        if (systHI && i < systHI->GetN()) {
            eHI_lo = systHI->GetErrorYlow(i);
            eHI_hi = systHI->GetErrorYhigh(i);
        }
        if (systPP && i < systPP->GetN()) {
            ePP_lo = systPP->GetErrorYlow(i);
            ePP_hi = systPP->GetErrorYhigh(i);
        }
        g->SetPoint(i, x, y);
        g->SetPointError(i, hw, hw,
            sqrt(eHI_lo*eHI_lo + ePP_lo*ePP_lo),
            sqrt(eHI_hi*eHI_hi + ePP_hi*ePP_hi));
    }
    return g;
}

// ---------------------------------------------------------------------------
// Compute difference for HEPData (may have different binning from pPb)
// ---------------------------------------------------------------------------
TH1D *ComputeHEPDataDifference(TH1D *hHI, TH1D *hPP, const char *name)
{
    TH1D *diff = (TH1D *)hHI->Clone(name);
    diff->SetDirectory(nullptr);
    for (int i = 1; i <= diff->GetNbinsX(); ++i) {
        double lo = diff->GetBinLowEdge(i);
        double hi = lo + diff->GetBinWidth(i);
        double sumVal = 0, sumErr2 = 0;
        int count = 0;
        for (int j = 1; j <= hPP->GetNbinsX(); ++j) {
            double ppC = hPP->GetBinCenter(j);
            if (ppC > lo + 1e-6 && ppC < hi - 1e-6) {
                sumVal += hPP->GetBinContent(j);
                sumErr2 += pow(hPP->GetBinError(j), 2);
                count++;
            }
        }
        double ppVal = (count > 0) ? sumVal / count : 0;
        double ppErr = (count > 0) ? sqrt(sumErr2) / count : 0;
        diff->SetBinContent(i, hHI->GetBinContent(i) - ppVal);
        diff->SetBinError(i, sqrt(pow(hHI->GetBinError(i), 2) + ppErr*ppErr));
    }
    return diff;
}

TGraphAsymmErrors *ComputeHEPDataDiffSystGraph(TH1D *diffHist,
    TGraphAsymmErrors *systHI, TGraphAsymmErrors *systPP)
{
    int n = diffHist->GetNbinsX();
    TGraphAsymmErrors *g = new TGraphAsymmErrors(n);
    for (int i = 0; i < n; ++i) {
        double x = diffHist->GetBinCenter(i+1);
        double y = diffHist->GetBinContent(i+1);
        double hw = diffHist->GetBinWidth(i+1) / 2.0;
        double lo = diffHist->GetBinLowEdge(i+1);
        double hi = lo + diffHist->GetBinWidth(i+1);
        double eHI_lo = systHI->GetErrorYlow(i);
        double eHI_hi = systHI->GetErrorYhigh(i);
        double sumLo = 0, sumHi = 0;
        int count = 0;
        for (int j = 0; j < systPP->GetN(); ++j) {
            double px, py;
            systPP->GetPoint(j, px, py);
            if (px > lo + 1e-6 && px < hi - 1e-6) {
                sumLo += systPP->GetErrorYlow(j);
                sumHi += systPP->GetErrorYhigh(j);
                count++;
            }
        }
        double ePP_lo = (count > 0) ? sumLo / count : 0;
        double ePP_hi = (count > 0) ? sumHi / count : 0;
        g->SetPoint(i, x, y);
        g->SetPointError(i, hw, hw,
            sqrt(eHI_lo*eHI_lo + ePP_lo*ePP_lo),
            sqrt(eHI_hi*eHI_hi + ePP_hi*ePP_hi));
    }
    return g;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    string ppResultFile = CL.Get("PPResultFile");
    string ppbNosubFile = CL.Get("PPbNosubFile");
    string pbpNosubFile = CL.Get("PbPNosubFile");
    string ppSystFile   = CL.Get("PPSystematicsFile");
    string ppbSystFile  = CL.Get("PPbSystematicsFile");
    string hepPPFile    = CL.Get("HEPDataPPFile");
    string hepPbPbFile  = CL.Get("HEPDataPbPbFile");
    string outputFile   = CL.Get("Output", "plots/pbpb_pPb_overlay_DeltaPhi.pdf");
    string trkPtRange   = CL.Get("trkPtRange", "0.5_2");
    string mode         = CL.Get("Mode", "DeltaPhi"); // DeltaPhi or DeltaEta
    bool isDeltaEta = (mode == "DeltaEta");
    bool doDifference   = CL.GetBool("Difference", false);

    SetTDRStyle();

    // -----------------------------------------------------------------------
    // 1) HEPData curves (PbPb study)
    // -----------------------------------------------------------------------
    HEPDataResult hepPP   = ReadHEPDataCSV(hepPPFile, "hep_pp");
    HEPDataResult hepPbPb = ReadHEPDataCSV(hepPbPbFile, "hep_pbpb");

    if (!hepPP.hist || !hepPbPb.hist) {
        cerr << "Error: failed to read HEPData CSVs" << endl;
        return 1;
    }

    // -----------------------------------------------------------------------
    // 2) pp from this study (already processed result file)
    // -----------------------------------------------------------------------
    TFile *fppResult = TFile::Open(ppResultFile.c_str(), "READ");
    if (!fppResult || fppResult->IsZombie()) {
        cerr << "Error: cannot open " << ppResultFile << endl;
        return 1;
    }
    string histName = isDeltaEta
        ? Form("DeltaEta_Result%s", trkPtRange.c_str())
        : Form("DeltaPhi_Result%s", trkPtRange.c_str());
    TH1D *h_pp = (TH1D *)fppResult->Get(histName.c_str());
    if (!h_pp) {
        cerr << "Error: " << histName << " not found in " << ppResultFile << endl;
        return 1;
    }
    h_pp = (TH1D *)h_pp->Clone("h_pp_clone");
    h_pp->SetDirectory(nullptr);
    h_pp->Scale(0.5);
    if (isDeltaEta) Symmetrize1DEta(h_pp);
    else            Symmetrize1DPhi(h_pp);

    TH1D *ppSystHist = nullptr;
    {
        TFile fSyst(ppSystFile.c_str(), "READ");
        if (!fSyst.IsZombie()) {
            string systName = isDeltaEta ? "Total_DeltaEta" : "Total_DeltaPhi";
            TH1D *h = (TH1D *)fSyst.Get(systName.c_str());
            if (h) { ppSystHist = (TH1D *)h->Clone("ppSystClone"); ppSystHist->SetDirectory(nullptr); }
        }
    }

    // -----------------------------------------------------------------------
    // 3) Combined pPb from this study (pPb + PbP nosub files)
    // -----------------------------------------------------------------------
    TFile *fPPb = TFile::Open(ppbNosubFile.c_str(), "READ");
    TFile *fPbP = TFile::Open(pbpNosubFile.c_str(), "READ");
    if (!fPPb || fPPb->IsZombie() || !fPbP || fPbP->IsZombie()) {
        cerr << "Error: cannot open nosub files" << endl;
        return 1;
    }

    TH2D *hData_ppb   = (TH2D *)fPPb->Get(Form("hData_%s", trkPtRange.c_str()));
    TH2D *hMix_ppb    = (TH2D *)fPPb->Get(Form("hMixData_%s", trkPtRange.c_str()));
    TH1D *hNZ_ppb     = (TH1D *)fPPb->Get(Form("hNZData_%s", trkPtRange.c_str()));
    TH1D *hNZMix_ppb  = (TH1D *)fPPb->Get(Form("hNZMixData_%s", trkPtRange.c_str()));

    TH2D *hData_pbp   = (TH2D *)fPbP->Get(Form("hData_%s", trkPtRange.c_str()));
    TH2D *hMix_pbp    = (TH2D *)fPbP->Get(Form("hMixData_%s", trkPtRange.c_str()));
    TH1D *hNZ_pbp     = (TH1D *)fPbP->Get(Form("hNZData_%s", trkPtRange.c_str()));
    TH1D *hNZMix_pbp  = (TH1D *)fPbP->Get(Form("hNZMixData_%s", trkPtRange.c_str()));

    if (!hData_ppb || !hData_pbp) {
        cerr << "Error: missing 2D histograms in nosub files" << endl;
        return 1;
    }

    // jackknife contributions
    vector<JackknifeProjectionContribution> jkPPb, jkPbP;
    AppendJackknifeProjectionContributions(fPPb,
        Form("JackknifeProjection%s", trkPtRange.c_str()), jkPPb);
    AppendJackknifeProjectionContributions(fPbP,
        Form("JackknifeProjection%s", trkPtRange.c_str()), jkPbP);

    double NZ_pPb = hNZ_ppb->GetBinContent(1);
    double NZ_PbP = hNZ_pbp->GetBinContent(1);
    double NZMix_pPb = hNZMix_ppb->GetBinContent(1);
    double NZMix_PbP = hNZMix_pbp->GetBinContent(1);

    // undo NZ normalization
    hData_ppb->Scale(NZ_pPb);
    hMix_ppb->Scale(NZMix_pPb);
    hData_pbp->Scale(NZ_PbP);
    hMix_pbp->Scale(NZMix_PbP);

    // combine signal
    TH2D *S_combined = (TH2D *)hData_ppb->Clone("S_combined");
    S_combined->Add(hData_pbp);
    S_combined->Scale(1.0 / (NZ_pPb + NZ_PbP));

    // combine background
    TH2D *B_combined = (TH2D *)hMix_ppb->Clone("B_combined");
    B_combined->Add(hMix_pbp);
    B_combined->Scale(1.0 / (NZMix_pPb + NZMix_PbP));

    // background subtraction
    S_combined->Add(B_combined, -1);

    // 2D symmetrization
    Symmetrize2DFourfold(S_combined);

    // project both observables (JK needs both)
    ResultProjectionWindow pw;
    TH1D *hPhi_pPb = (TH1D *)S_combined->ProjectionY("DeltaPhi_pPb",
        pw.DeltaPhiXFirst, pw.DeltaPhiXLast);
    TH1D *hEta_pPb = (TH1D *)S_combined->ProjectionX("DeltaEta_pPb",
        pw.DeltaEtaYFirst, pw.DeltaEtaYLast);

    ApplyCombinedDiagonalJackknifeErrors(hEta_pPb, hPhi_pPb,
        jkPPb, jkPbP, NZ_pPb, NZ_PbP);

    divideByWidth(hPhi_pPb);
    hPhi_pPb->Scale(1.0 / 2);
    Symmetrize1DPhi(hPhi_pPb);

    divideByWidth(hEta_pPb);
    hEta_pPb->Scale(1.0 / 2);
    Symmetrize1DEta(hEta_pPb);

    TH1D *h_pPb = isDeltaEta ? hEta_pPb : hPhi_pPb;

    TH1D *ppbSystHist = nullptr;
    {
        TFile fSyst(ppbSystFile.c_str(), "READ");
        if (!fSyst.IsZombie()) {
            string systName = isDeltaEta ? "Total_DeltaEta" : "Total_DeltaPhi";
            TH1D *h = (TH1D *)fSyst.Get(systName.c_str());
            if (h) { ppbSystHist = (TH1D *)h->Clone("ppbSystClone"); ppbSystHist->SetDirectory(nullptr); }
        }
    }

    cout << "Data loaded successfully." << endl;
    cout << "pp (this study): " << h_pp->GetNbinsX() << " bins" << endl;
    cout << "pPb (this study): " << h_pPb->GetNbinsX() << " bins" << endl;
    cout << "pp (PbPb study): " << hepPP.hist->GetNbinsX() << " bins" << endl;
    cout << "PbPb (PbPb study): " << hepPbPb.hist->GetNbinsX() << " bins" << endl;

    // -----------------------------------------------------------------------
    // 4) Build systematic band graphs
    // -----------------------------------------------------------------------
    TGraphAsymmErrors *gSyst_pp   = BuildSystGraph(h_pp, ppSystHist);
    TGraphAsymmErrors *gSyst_pPb  = BuildSystGraph(h_pPb, ppbSystHist);
    // HEPData graphs already built by the CSV reader
    TGraphAsymmErrors *gSyst_hepPP   = hepPP.systGraph;
    TGraphAsymmErrors *gSyst_hepPbPb = hepPbPb.systGraph;

    // -----------------------------------------------------------------------
    // 5) Colors and marker shapes
    // -----------------------------------------------------------------------
    int colHepPP   = cmsPurple;  // PbPb study pp
    int colHepPbPb = cmsYellow;  // PbPb study PbPb
    int colPPbPP   = cmsBlue;   // pPb study pp
    int colPPbHI   = cmsRed;    // pPb study pPb
    double signalMin = isDeltaEta ? 0.0 : 0.0;
    double signalMax = isDeltaEta ? 3.87 : M_PI;
    float mSize = 1.2;

    // pp curves: square markers
    int mkPbPbStudyPP   = 21;
    int mkPPbStudyPP    = 21;

    // HI curves: circle markers
    int mkPbPbStudyPbPb = 20;
    int mkPPbStudyPPb   = 20;

    // systematic band styles
    auto styleSystBand = [](TGraphAsymmErrors *g, int color) {
        g->SetFillColorAlpha(color, 0.25);
        g->SetLineWidth(0);
        g->SetMarkerSize(0);
    };
    styleSystBand(gSyst_hepPP, colHepPP);
    styleSystBand(gSyst_hepPbPb, colHepPbPb);
    styleSystBand(gSyst_pp, colPPbPP);
    styleSystBand(gSyst_pPb, colPPbHI);

    // -----------------------------------------------------------------------
    // 6) Canvas and drawing
    // -----------------------------------------------------------------------
    TCanvas *canvas = new TCanvas("c", "c", 600, 780);
    canvas->SetLeftMargin(0.15);
    canvas->SetRightMargin(0.05);
    canvas->SetTopMargin(0.08);
    canvas->SetBottomMargin(0.13);

    // frame
    double xMin = isDeltaEta ? -3.87 : -M_PI/2;
    double xMax = isDeltaEta ?  3.87 : 3*M_PI/2;
    TH1D *frame = new TH1D("frame", "", 1, xMin, xMax);
    if (isDeltaEta) {
        frame->GetXaxis()->SetTitle("#Deltay_{ch,Z}");
        frame->GetYaxis()->SetTitle("d#LT#DeltaN_{ch}#GT/d#Deltay_{ch,Z}");
    } else {
        frame->GetXaxis()->SetTitle("#Delta#varphi_{ch,Z}");
        frame->GetYaxis()->SetTitle("d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}");
    }
    frame->GetXaxis()->SetTitleSize(0.045);
    frame->GetYaxis()->SetTitleSize(0.045);
    frame->GetXaxis()->SetLabelSize(0.04);
    frame->GetYaxis()->SetLabelSize(0.04);
    frame->GetYaxis()->SetTitleOffset(1.5);

    // legend proxy helpers (filled marker for legend icon)
    auto makeProxy = [](int color, int style, float size) {
        TGraph *g = new TGraph(1);
        g->SetMarkerColor(color);
        g->SetMarkerStyle(style);
        g->SetMarkerSize(size);
        g->SetLineColor(color);
        return g;
    };

    if (doDifference) {
        // ===================================================================
        // DIFFERENCE MODE: draw (PbPb - pp) and (pPb - pp)
        // ===================================================================
        TH1D *hepDiff = ComputeHEPDataDifference(hepPbPb.hist, hepPP.hist, "hepDiff");
        TH1D *pPbDiff = ComputeDifference(h_pPb, h_pp, "pPbDiff");

        TGraphAsymmErrors *gDiffSyst_hep = ComputeHEPDataDiffSystGraph(
            hepDiff, gSyst_hepPbPb, gSyst_hepPP);
        TGraphAsymmErrors *gDiffSyst_pPb = ComputeDiffSystGraph(
            pPbDiff, gSyst_pPb, gSyst_pp);

        styleSystBand(gDiffSyst_hep, colHepPbPb);
        styleSystBand(gDiffSyst_pPb, colPPbHI);

        frame->SetMinimum(isDeltaEta ? -1.5 : -2.5);
        frame->SetMaximum(isDeltaEta ?  1.5 :  4.0);
        frame->Draw();

        gDiffSyst_pPb->Draw("2 SAME");
        gDiffSyst_hep->Draw("2 SAME");

        int mkHep = 20;  // circle for both
        int mkPPb = 20;

        struct CurveSpec { TH1D *h; int color; int filledStyle; };
        vector<CurveSpec> curves = {
            {hepDiff, colHepPbPb, mkHep},
            {pPbDiff, colPPbHI,   mkPPb},
        };
        for (int ic = 0; ic < (int)curves.size(); ++ic) {
            auto &c = curves[ic];
            TGraphErrors *gSig = HistToRegionGraph(c.h, signalMin, signalMax, true,
                Form("sig_%d", ic));
            if (gSig) {
                gSig->SetMarkerColor(c.color);
                gSig->SetMarkerStyle(c.filledStyle);
                gSig->SetMarkerSize(mSize);
                gSig->SetLineColor(c.color);
                gSig->Draw("PE SAME");
            }
            TGraphErrors *gOut = HistToRegionGraph(c.h, signalMin, signalMax, false,
                Form("out_%d", ic));
            if (gOut) {
                gOut->SetMarkerColor(c.color);
                gOut->SetMarkerStyle(OpenMarkerStyle(c.filledStyle));
                gOut->SetMarkerSize(mSize);
                gOut->SetLineColor(c.color);
                gOut->Draw("PE SAME");
            }
        }

        // CMS header
        AddCMSHeader(canvas, "Preliminary", false);

        float headerSize = 0.042;
        float legTextSize = 0.036;
        float selTextSize = 0.032;
        float lineStep = 0.032;

        TLatex latex;
        latex.SetNDC();
        latex.SetTextFont(42);

        if (isDeltaEta) {
            // --- Side-by-side layout for DeltaEta ---
            float colL = 0.18;
            float colR = 0.55;
            float topY = 0.86;
            float curY = topY;

            latex.SetTextSize(headerSize);
            latex.DrawLatex(colL, curY, "#bf{8.16 TeV}");
            latex.DrawLatex(colR, curY, "#bf{5.02 TeV}");
            curY -= 1.2 * lineStep;

            latex.SetTextSize(selTextSize);
            latex.DrawLatex(colL, curY,              "30 < p_{T}^{Z} < 500 GeV");
            latex.DrawLatex(colR, curY,              "40 < p_{T}^{Z} < 350 GeV");
            latex.DrawLatex(colL, curY - lineStep,   "0.5 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(colR, curY - lineStep,   "1 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(colL, curY - 2*lineStep, "|y_{cm}| < 1.935");
            latex.DrawLatex(colR, curY - 2*lineStep, "|y_{lab}| < 2.4");
            curY -= 2.8 * lineStep;

            float legBot = curY - 1.2 * lineStep;
            TLegend *leg1 = new TLegend(colL - 0.01, legBot, colL + 0.42, curY + 0.005);
            leg1->SetBorderSize(0); leg1->SetFillStyle(0);
            leg1->SetTextFont(42); leg1->SetTextSize(legTextSize);
            leg1->AddEntry(makeProxy(colPPbHI, mkPPb, mSize), "pPb #minus pp", "P");
            leg1->Draw();

            TLegend *leg2 = new TLegend(colR - 0.01, legBot, colR + 0.42, curY + 0.005);
            leg2->SetBorderSize(0); leg2->SetFillStyle(0);
            leg2->SetTextFont(42); leg2->SetTextSize(legTextSize);
            leg2->AddEntry(makeProxy(colHepPbPb, mkHep, mSize), "PbPb 0-30% #minus pp", "P");
            leg2->Draw();
        } else {
            // --- Stacked vertical layout on the LEFT for DeltaPhi ---
            float legX = 0.19;
            float topY = 0.86;
            float curY = topY;

            // 8.16 TeV block (top)
            latex.SetTextSize(headerSize);
            latex.DrawLatex(legX, curY, "#bf{8.16 TeV}");
            curY -= 1.2 * lineStep;

            latex.SetTextSize(selTextSize);
            latex.DrawLatex(legX, curY,                "30 < p_{T}^{Z} < 500 GeV");
            latex.DrawLatex(legX, curY - lineStep,     "0.5 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(legX, curY - 2*lineStep,   "|y_{cm}| < 1.935");
            curY -= 2.8 * lineStep;

            float leg1Bot = curY - 1.2 * lineStep;
            TLegend *leg1 = new TLegend(legX - 0.01, leg1Bot, legX + 0.42, curY + 0.005);
            leg1->SetBorderSize(0); leg1->SetFillStyle(0);
            leg1->SetTextFont(42); leg1->SetTextSize(legTextSize);
            leg1->AddEntry(makeProxy(colPPbHI, mkPPb, mSize), "pPb #minus pp", "P");
            leg1->Draw();

            // 5.02 TeV block (bottom)
            curY = leg1Bot - 1.2 * lineStep;
            latex.SetTextSize(headerSize);
            latex.DrawLatex(legX, curY, "#bf{5.02 TeV}");
            curY -= 1.2 * lineStep;

            latex.SetTextSize(selTextSize);
            latex.DrawLatex(legX, curY,                "40 < p_{T}^{Z} < 350 GeV");
            latex.DrawLatex(legX, curY - lineStep,     "1 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(legX, curY - 2*lineStep,   "|y_{lab}| < 2.4");
            curY -= 2.8 * lineStep;

            float leg2Bot = curY - 1.2 * lineStep;
            TLegend *leg2 = new TLegend(legX - 0.01, leg2Bot, legX + 0.42, curY + 0.005);
            leg2->SetBorderSize(0); leg2->SetFillStyle(0);
            leg2->SetTextFont(42); leg2->SetTextSize(legTextSize);
            leg2->AddEntry(makeProxy(colHepPbPb, mkHep, mSize), "PbPb 0-30% #minus pp", "P");
            leg2->Draw();
        }

    } else {
        // ===================================================================
        // OVERLAY MODE: four curves
        // ===================================================================
        frame->SetMinimum(isDeltaEta ? -1.7 : -2.7);
        frame->SetMaximum(isDeltaEta ?  1.7 :  5.0);
        frame->Draw();

        // draw systematic bands (all four, back to front)
        gSyst_pPb->Draw("2 SAME");
        gSyst_pp->Draw("2 SAME");
        gSyst_hepPbPb->Draw("2 SAME");
        gSyst_hepPP->Draw("2 SAME");

        // helper: draw one curve with region-based filled/open markers
        struct CurveSpec { TH1D *h; int color; int filledStyle; };
        vector<CurveSpec> curves = {
            {hepPP.hist,   colHepPP,   mkPbPbStudyPP},
            {hepPbPb.hist, colHepPbPb, mkPbPbStudyPbPb},
            {h_pp,         colPPbPP,   mkPPbStudyPP},
            {h_pPb,        colPPbHI,   mkPPbStudyPPb},
        };
        for (int ic = 0; ic < (int)curves.size(); ++ic) {
            auto &c = curves[ic];
            TGraphErrors *gSig = HistToRegionGraph(c.h, signalMin, signalMax, true,
                Form("sig_%d", ic));
            if (gSig) {
                gSig->SetMarkerColor(c.color);
                gSig->SetMarkerStyle(c.filledStyle);
                gSig->SetMarkerSize(mSize);
                gSig->SetLineColor(c.color);
                gSig->Draw("PE SAME");
            }
            TGraphErrors *gOut = HistToRegionGraph(c.h, signalMin, signalMax, false,
                Form("out_%d", ic));
            if (gOut) {
                gOut->SetMarkerColor(c.color);
                gOut->SetMarkerStyle(OpenMarkerStyle(c.filledStyle));
                gOut->SetMarkerSize(mSize);
                gOut->SetLineColor(c.color);
                gOut->Draw("PE SAME");
            }
        }

        // CMS header
        AddCMSHeader(canvas, "Preliminary", false);

        // Legends
        float headerSize = 0.042;
        float legTextSize = 0.036;
        float selTextSize = 0.032;
        float legX = 0.19;
        float topY = 0.86;
        float lineStep = 0.032;

        TLatex latex;
        latex.SetNDC();
        latex.SetTextFont(42);

        if (isDeltaEta) {
            float colL = 0.18;
            float colR = 0.55;
            float curY = topY;

            latex.SetTextSize(headerSize);
            latex.DrawLatex(colL, curY, "#bf{8.16 TeV}");
            latex.DrawLatex(colR, curY, "#bf{5.02 TeV}");
            curY -= 1.2 * lineStep;

            latex.SetTextSize(selTextSize);
            latex.DrawLatex(colL, curY,                "30 < p_{T}^{Z} < 500 GeV");
            latex.DrawLatex(colR, curY,                "40 < p_{T}^{Z} < 350 GeV");
            latex.DrawLatex(colL, curY - lineStep,     "0.5 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(colR, curY - lineStep,     "1 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(colL, curY - 2*lineStep,   "|y_{cm}| < 1.935");
            latex.DrawLatex(colR, curY - 2*lineStep,   "|y_{lab}| < 2.4");
            curY -= 2.8 * lineStep;

            float legBot = curY - 2.0 * lineStep;
            TLegend *leg1 = new TLegend(colL - 0.01, legBot, colL + 0.42, curY + 0.005);
            leg1->SetBorderSize(0); leg1->SetFillStyle(0);
            leg1->SetTextFont(42); leg1->SetTextSize(legTextSize);
            leg1->AddEntry(makeProxy(colPPbPP, mkPPbStudyPP, mSize), "pp (extrapolated)", "P");
            leg1->AddEntry(makeProxy(colPPbHI, mkPPbStudyPPb, mSize), "pPb", "P");
            leg1->Draw();

            TLegend *leg2 = new TLegend(colR - 0.01, legBot, colR + 0.34, curY + 0.005);
            leg2->SetBorderSize(0); leg2->SetFillStyle(0);
            leg2->SetTextFont(42); leg2->SetTextSize(legTextSize);
            leg2->AddEntry(makeProxy(colHepPP, mkPbPbStudyPP, mSize), "pp", "P");
            leg2->AddEntry(makeProxy(colHepPbPb, mkPbPbStudyPbPb, mSize), "PbPb 0-30%", "P");
            leg2->Draw();
        } else {
            float curY = topY;
            latex.SetTextSize(headerSize);
            latex.DrawLatex(legX, curY, "#bf{8.16 TeV}");
            curY -= 1.2 * lineStep;

            latex.SetTextSize(selTextSize);
            latex.DrawLatex(legX, curY,                "30 < p_{T}^{Z} < 500 GeV");
            latex.DrawLatex(legX, curY - lineStep,     "0.5 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(legX, curY - 2*lineStep,   "|y_{cm}| < 1.935");
            curY -= 2.8 * lineStep;

            float leg1Bot = curY - 2.0 * lineStep;
            TLegend *leg1 = new TLegend(legX - 0.01, leg1Bot, legX + 0.48, curY + 0.005);
            leg1->SetBorderSize(0); leg1->SetFillStyle(0);
            leg1->SetTextFont(42); leg1->SetTextSize(legTextSize);
            leg1->AddEntry(makeProxy(colPPbPP, mkPPbStudyPP, mSize), "pp (extrapolated)", "P");
            leg1->AddEntry(makeProxy(colPPbHI, mkPPbStudyPPb, mSize), "pPb", "P");
            leg1->Draw();

            curY = leg1Bot - 1.2 * lineStep;
            latex.SetTextSize(headerSize);
            latex.DrawLatex(legX, curY, "#bf{5.02 TeV}");
            curY -= 1.2 * lineStep;

            latex.SetTextSize(selTextSize);
            latex.DrawLatex(legX, curY,                "40 < p_{T}^{Z} < 350 GeV");
            latex.DrawLatex(legX, curY - lineStep,     "1 < p_{T}^{ch} < 2 GeV");
            latex.DrawLatex(legX, curY - 2*lineStep,   "|y_{lab}| < 2.4");
            curY -= 2.8 * lineStep;

            float leg2Bot = curY - 2.0 * lineStep;
            TLegend *leg2 = new TLegend(legX - 0.01, leg2Bot, legX + 0.42, curY + 0.005);
            leg2->SetBorderSize(0); leg2->SetFillStyle(0);
            leg2->SetTextFont(42); leg2->SetTextSize(legTextSize);
            leg2->AddEntry(makeProxy(colHepPP, mkPbPbStudyPP, mSize), "pp", "P");
            leg2->AddEntry(makeProxy(colHepPbPb, mkPbPbStudyPbPb, mSize), "PbPb 0-30%", "P");
            leg2->Draw();
        }
    }

    // -----------------------------------------------------------------------
    // 9) Save
    // -----------------------------------------------------------------------
    canvas->SaveAs(outputFile.c_str());
    cout << "Saved: " << outputFile << endl;

    return 0;
}
