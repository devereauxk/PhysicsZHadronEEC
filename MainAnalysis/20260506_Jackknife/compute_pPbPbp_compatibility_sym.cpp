// compute_pPbPbp_compatibility_sym.cpp
// Chi-square and KS compatibility tests for signed-then-symmetrized pPb vs Pbp.
//
// Reads signed pPb and Pbp result files (produced with --FillSigned true),
// symmetrizes each orientation's 1D result histograms and per-event JK arrays,
// then runs the same Sumw2 / diagonal JK / full-covariance chi-square tests
// and the two-sample KS test from the unsymmetrized tool.
//
// Symmetry mirrors (0-indexed, 12 bins each):
//   DeltaEta: i <-> 11-i
//   DeltaPhi near-side (j=0..5): j <-> 5-j
//   DeltaPhi away-side (j=6..11): j <-> 17-j
// After symmetrization the effective rank of each covariance matrix is 6.

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

#include <Math/GoFTest.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

// ── Mirror functions ────────────────────────────────────────────────────────────

int etaMirror(int i)  { return 11 - i; }
int phiMirror(int j)  { return (j < 6) ? (5 - j) : (17 - j); }

// ── Data structures ─────────────────────────────────────────────────────────────

struct CompatibilityResult {
    string ObservableLabel;
    string ErrorModel;
    int TotalBins = 0, UsedBins = 0;
    double Chi2 = 0, Chi2PerNDF = 0, PValue = 0;
};

struct KSResult {
    string ObservableLabel;
    int    N_bins = 0;
    double D_KS = 0, KS_PValue = -1;
};

struct JackknifeEventData {
    double sigNZ = 0, mixNZ = 0;
    vector<float> sigEta, mixEta, sigPhi, mixPhi;
};

// ── Helper: load histogram ──────────────────────────────────────────────────────

TH1D *loadHistogram(TFile &file, const string &name, const string &cloneName)
{
    TH1D *h = (TH1D *)file.Get(name.c_str());
    if (!h) return nullptr;
    h = (TH1D *)h->Clone(cloneName.c_str());
    h->SetDirectory(nullptr);
    return h;
}

// ── Symmetrize a histogram (returns a new clone) ────────────────────────────────
// Error propagation: sigma_sym[i] = 0.5 * sqrt(sigma[i]^2 + sigma[mirror]^2)

TH1D *symmetrizeHist(TH1D *h, bool useEta, const string &cloneName)
{
    TH1D *hs = (TH1D *)h->Clone(cloneName.c_str());
    hs->SetDirectory(nullptr);
    int n = hs->GetNbinsX();
    for (int i = 0; i < n; ++i) {
        int mi = useEta ? etaMirror(i) : phiMirror(i);
        double v = 0.5 * (h->GetBinContent(i + 1) + h->GetBinContent(mi + 1));
        double ea = h->GetBinError(i + 1), eb = h->GetBinError(mi + 1);
        double e = 0.5 * sqrt(ea * ea + eb * eb);
        hs->SetBinContent(i + 1, v);
        hs->SetBinError(i + 1, e);
    }
    return hs;
}

// ── Extract unique bins from a symmetrized histogram ───────────────────────────

TH1D *extractUniqueBins(TH1D *h, int first0, int last0, const string &name)
{
    int nU = last0 - first0 + 1;
    vector<double> edges(nU + 1);
    for (int k = 0; k < nU; ++k)
        edges[k] = h->GetBinLowEdge(first0 + 1 + k);
    edges[nU] = h->GetBinLowEdge(last0 + 1) + h->GetBinWidth(last0 + 1);
    TH1D *out = new TH1D(name.c_str(), "", nU, edges.data());
    out->SetDirectory(nullptr);
    for (int k = 0; k < nU; ++k) {
        out->SetBinContent(k + 1, h->GetBinContent(first0 + 1 + k));
        out->SetBinError(k + 1, h->GetBinError(first0 + 1 + k));
    }
    return out;
}

// ── Extract unique bins from per-event JK arrays ──────────────────────────────

vector<JackknifeEventData> extractUniqueJKEvents(
    const vector<JackknifeEventData> &events, bool useEta, int first0, int last0)
{
    int nU = last0 - first0 + 1;
    auto out = events;
    for (auto &ev : out) {
        auto &s = useEta ? ev.sigEta : ev.sigPhi;
        auto &m = useEta ? ev.mixEta : ev.mixPhi;
        vector<float> sNew(nU), mNew(nU);
        for (int k = 0; k < nU; ++k) {
            int idx = first0 + k;
            sNew[k] = (idx < (int)s.size()) ? s[idx] : 0;
            mNew[k] = (idx < (int)m.size()) ? m[idx] : 0;
        }
        s = sNew;
        m = mNew;
    }
    return out;
}

// ── Load jackknife events from a TTree ─────────────────────────────────────────

vector<JackknifeEventData> loadJackknifeEvents(TFile &file, const string &treeName)
{
    TTree *tree = (TTree *)file.Get(treeName.c_str());
    if (!tree) {
        cerr << "Cannot find tree " << treeName << " in " << file.GetName() << endl;
        return {};
    }
    double sigNZ = 0, mixNZ = 0;
    vector<float> *sigEta = nullptr, *mixEta = nullptr;
    vector<float> *sigPhi = nullptr, *mixPhi = nullptr;
    tree->SetBranchAddress("SignalNZ",  &sigNZ);
    tree->SetBranchAddress("MixNZ",     &mixNZ);
    tree->SetBranchAddress("SignalEta", &sigEta);
    tree->SetBranchAddress("MixEta",    &mixEta);
    tree->SetBranchAddress("SignalPhi", &sigPhi);
    tree->SetBranchAddress("MixPhi",    &mixPhi);
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

// ── Symmetrize per-event arrays ─────────────────────────────────────────────────
// Returns a copy with the nominated observable's arrays symmetrized.

vector<JackknifeEventData> makeSymEvents(const vector<JackknifeEventData> &orig, bool useEta)
{
    auto events = orig;
    for (auto &ev : events) {
        auto &s = useEta ? ev.sigEta : ev.sigPhi;
        auto &m = useEta ? ev.mixEta : ev.mixPhi;
        int n = (int)s.size();
        for (int i = 0; i < n; ++i) {
            int mi = useEta ? etaMirror(i) : phiMirror(i);
            if (mi > i && mi < n) {
                float sv = 0.5f * (s[i] + s[mi]);
                float mv = 0.5f * (m[i] + m[mi]);
                s[i] = s[mi] = sv;
                m[i] = m[mi] = mv;
            }
        }
    }
    return events;
}

// ── Jackknife covariance matrix ─────────────────────────────────────────────────
// Identical to the unsymmetrized tool; pass symmetrized event arrays.

TMatrixDSym buildCovMatrix(const vector<JackknifeEventData> &events,
    TH1D *axisHist, bool useEta)
{
    int nBins = axisHist->GetNbinsX();
    TMatrixDSym C(nBins);
    if (events.empty()) return C;

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

    vector<double> bw(nBins);
    for (int i = 0; i < nBins; ++i) bw[i] = axisHist->GetBinWidth(i + 1);

    vector<double> theta(nBins);
    for (int i = 0; i < nBins; ++i)
        theta[i] = totalSig[i] / totalSigNZ - totalMix[i] / totalMixNZ;

    int validN = 0;
    for (const auto &ev : events) {
        double leftSigNZ = totalSigNZ - ev.sigNZ;
        double leftMixNZ = totalMixNZ - ev.mixNZ;
        if (leftSigNZ <= 0 || leftMixNZ <= 0) continue;
        ++validN;
        const auto &s = useEta ? ev.sigEta : ev.sigPhi;
        const auto &m = useEta ? ev.mixEta : ev.mixPhi;
        vector<double> d(nBins);
        for (int i = 0; i < nBins; ++i) {
            double tLOO = (totalSig[i] - s[i]) / leftSigNZ
                        - (totalMix[i] - m[i]) / leftMixNZ;
            d[i] = (tLOO - theta[i]) / bw[i];
        }
        for (int i = 0; i < nBins; ++i)
            for (int j = 0; j < nBins; ++j)
                C(i, j) += d[i] * d[j];
    }
    if (validN > 1) C *= (double)(validN - 1) / validN;
    return C;
}

// ── Diagonal chi-square ─────────────────────────────────────────────────────────

CompatibilityResult computeCompatibility(TH1D *pPb, TH1D *PbP,
    const string &obsLabel, const string &errorModel)
{
    CompatibilityResult result;
    result.ObservableLabel = obsLabel;
    result.ErrorModel      = errorModel;
    if (!pPb || !PbP) return result;
    result.TotalBins = min(pPb->GetNbinsX(), PbP->GetNbinsX());
    for (int bin = 1; bin <= result.TotalBins; ++bin) {
        double s2 = pPb->GetBinError(bin) * pPb->GetBinError(bin)
                  + PbP->GetBinError(bin) * PbP->GetBinError(bin);
        if (s2 <= 0) continue;
        double d = pPb->GetBinContent(bin) - PbP->GetBinContent(bin);
        result.Chi2 += d * d / s2;
        ++result.UsedBins;
    }
    if (result.UsedBins > 0) {
        result.Chi2PerNDF = result.Chi2 / result.UsedBins;
        result.PValue     = TMath::Prob(result.Chi2, result.UsedBins);
    }
    return result;
}

// ── Full covariance chi-square ──────────────────────────────────────────────────

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
    int nsv  = sv.GetNrows();
    double svMax = (nsv > 0) ? sv(0) : 0;
    double tol   = svMax * 1e-8;
    int rank = 0;
    for (int k = 0; k < nsv; ++k) if (sv(k) > tol) ++rank;

    cout << "  [" << obsLabel << "]"
         << "  sv_max=" << svMax
         << "  cond="   << (sv(nsv - 1) > 0 ? svMax / sv(nsv - 1) : -1)
         << "  effective rank=" << rank << "/" << nBins << endl;

    if (rank == 0) { cerr << "Zero-rank covariance for " << obsLabel << endl; return result; }
    result.UsedBins = rank;

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

// ── Two-sample KS test ──────────────────────────────────────────────────────────

KSResult computeKS(TH1D *pPb, TH1D *PbP, const string &label)
{
    KSResult result;
    result.ObservableLabel = label;
    if (!pPb || !PbP) return result;
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

// ── Correlation heatmap helpers ─────────────────────────────────────────────────

void setDivergingPalette() {
    const int nC = 255, nP = 3;
    double stops[]={0.00,0.50,1.00}, red[]={0.12,1.00,0.84};
    double green[]={0.47,1.00,0.10}, blue[]={0.71,1.00,0.11};
    TColor::CreateGradientColorTable(nP,stops,red,green,blue,nC);
    gStyle->SetNumberContours(nC);
}
void setSequentialPalette() {
    const int nC=255, nP=2;
    double stops[]={0.00,1.00}, red[]={1.00,0.12};
    double green[]={1.00,0.47}, blue[]={1.00,0.71};
    TColor::CreateGradientColorTable(nP,stops,red,green,blue,nC);
    gStyle->SetNumberContours(nC);
}

TH2D *makeCorrelationH2(const TMatrixDSym &C, TH1D *ax, const char *name,
                         int first, int last)
{
    if (last < 0) last = ax->GetNbinsX()-1;
    int n = last-first+1;
    double lo=ax->GetBinLowEdge(first+1);
    double hi=ax->GetBinLowEdge(last+1)+ax->GetBinWidth(last+1);
    TH2D *h = new TH2D(name,"",n,lo,hi,n,lo,hi);
    for (int i=0;i<n;++i){
        int ii=first+i; double sii=(C(ii,ii)>0)?sqrt(C(ii,ii)):0;
        for (int j=0;j<n;++j){
            int jj=first+j; double sjj=(C(jj,jj)>0)?sqrt(C(jj,jj)):0;
            h->SetBinContent(i+1,j+1,(sii>0&&sjj>0)?C(ii,jj)/(sii*sjj):0);
        }
    }
    return h;
}

void saveCorrelationHeatmaps(const TMatrixDSym &CpPb, const TMatrixDSym &CPbP,
    TH1D *ax, const string &obsKey, const string &path, int first, int last)
{
    string axT=(obsKey=="Eta")?"#Delta y_{ch,Z}":"#Delta#phi_{ch,Z}";
    gStyle->SetOptStat(0);
    TH2D *hp=makeCorrelationH2(CpPb,ax,("hCorrpPb_"+obsKey).c_str(),first,last);
    TH2D *hq=makeCorrelationH2(CPbP,ax,("hCorrPbP_"+obsKey).c_str(),first,last);
    int n=last-first+1;
    double minV=0;
    for(int ix=1;ix<=n;++ix) for(int iy=1;iy<=n;++iy){
        minV=min(minV,hp->GetBinContent(ix,iy));
        minV=min(minV,hq->GetBinContent(ix,iy));
    }
    bool neg=(minV<-1e-9);
    if(neg) setDivergingPalette(); else setSequentialPalette();
    double zLo=neg?-1.0:0.0;
    hp->GetZaxis()->SetRangeUser(zLo,1.0);
    hq->GetZaxis()->SetRangeUser(zLo,1.0);
    TCanvas *c=new TCanvas("cCorr","cCorr",1400,600);
    c->Divide(2,1);
    auto draw=[&](TVirtualPad *pad,TH2D *h,const char *lbl){
        pad->cd(); pad->SetRightMargin(0.15);
        h->GetXaxis()->SetTitle(axT.c_str());
        h->GetYaxis()->SetTitle(axT.c_str());
        h->GetZaxis()->SetTitle("#rho");
        h->Draw("COLZ");
        TLatex L; L.SetNDC(); L.SetTextSize(0.052); L.SetTextFont(62);
        L.DrawLatex(0.17,0.88,lbl);
    };
    draw(c->cd(1),hp,"pPb 8.16 TeV");
    draw(c->cd(2),hq,"Pbp 8.16 TeV");
    c->SaveAs(path.c_str());
    delete hp; delete hq; delete c;
}

// ── Output writers ──────────────────────────────────────────────────────────────

void writeMarkdown(const string &path, const vector<CompatibilityResult> &r)
{
    ofstream out(path);
    out<<"# pPb vs Pbp compatibility (signed+symmetrized): Sumw2 / JK / full-cov\n\n";
    out<<"| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |\n";
    out<<"| --- | --- | ---: | ---: | ---: | ---: | ---: |\n";
    out<<setprecision(17);
    for(const auto &x:r)
        out<<"| `"<<x.ObservableLabel<<"` | **"<<x.ErrorModel<<"** | "
           <<x.UsedBins<<" | "<<x.Chi2<<" | "<<x.UsedBins<<" | "
           <<x.Chi2PerNDF<<" | "<<x.PValue<<" |\n";
}

void writeTSV(const string &path, const vector<CompatibilityResult> &r)
{
    ofstream out(path);
    out<<"Observable\tErrorModel\tN_bins\tchi2\tndf\tchi2_per_ndf\tp_value\n";
    out<<setprecision(17);
    for(const auto &x:r)
        out<<x.ObservableLabel<<'\t'<<x.ErrorModel<<'\t'<<x.UsedBins<<'\t'
           <<x.Chi2<<'\t'<<x.UsedBins<<'\t'<<x.Chi2PerNDF<<'\t'<<x.PValue<<'\n';
}

void writeTeX(const string &path, const vector<CompatibilityResult> &r)
{
    ofstream out(path);
    out<<"\\begin{tabular}{|l|l|r|r|r|r|r|}\n\\hline\n";
    out<<"Observable & Error model & $N_{\\mathrm{bins}}$ & $\\chi^2$ & ndf & "
       <<"$\\chi^2/\\mathrm{ndf}$ & $p$-value \\\\\n\\hline\n";
    auto esc=[](const string &s){string t;for(char c:s){if(c=='_')t+="\\_";else t+=c;}return t;};
    out<<setprecision(4);
    for(const auto &x:r)
        out<<"\\texttt{"<<esc(x.ObservableLabel)<<"} & "<<x.ErrorModel<<" & "
           <<x.UsedBins<<" & "<<x.Chi2<<" & "<<x.UsedBins<<" & "
           <<x.Chi2PerNDF<<" & "<<x.PValue<<" \\\\\n";
    out<<"\\hline\n\\end{tabular}\n";
}

void writeKSTeX(const string &path, const vector<KSResult> &r)
{
    ofstream out(path);
    out<<fixed<<setprecision(4);
    out<<"\\begin{table}[hbtp]\n\\centering\n";
    out<<"\\begin{tabular}{|l|r|r|r|}\n\\hline\n";
    out<<"Observable & $N_{\\rm bins}$ & $D_{\\rm KS}$ & KS $p$-value \\\\\n\\hline\n";
    for(const auto &x:r){
        string macro=(x.ObservableLabel.find("Eta")!=string::npos)?"$\\dytz$":"$\\dphitz$";
        string pval;
        if(x.KS_PValue<0) pval="---";
        else if(x.KS_PValue==0.0) pval="$\\ll 10^{-10}$";
        else if(x.KS_PValue<0.001){char buf[64];snprintf(buf,sizeof(buf),"$%.2e$",x.KS_PValue);pval=buf;}
        else{char buf[32];snprintf(buf,sizeof(buf),"%.3f",x.KS_PValue);pval=buf;}
        out<<macro<<" & "<<x.N_bins<<" & "<<x.D_KS<<" & "<<pval<<" \\\\\n";
    }
    out<<"\\hline\n\\end{tabular}\n";
    out<<"\\caption{Two-sample KS statistic $D_{\\rm KS}$ for pPb vs Pbp "
       <<"using the 6 unique bins of the signed-then-symmetrized result histograms. "
       <<"The $p$-value uses the asymptotic Kolmogorov distribution with "
       <<"$n_{\\rm eff} = N_{\\rm bins} = 6$.}\n";
    out<<"\\label{table:combining_ks}\n\\end{table}\n";
}

void writeKSTSV(const string &path, const vector<KSResult> &r)
{
    ofstream out(path);
    out<<"Observable\tN_bins\tD_KS\tKS_PValue\n"<<setprecision(10);
    for(const auto &x:r)
        out<<x.ObservableLabel<<'\t'<<x.N_bins<<'\t'<<x.D_KS<<'\t'<<x.KS_PValue<<'\n';
}

// ── main ────────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string baseDir   = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outputDir = CL.Get("OutputDir", "output_signed_sym");
    const string trkRange  = CL.Get("TrkRange",  "0.5_15");
    const string tag       = CL.Get("Tag",       "ZV10_trkV29_nmix10");
    // Heatmap sub-range (0-indexed): physical half of the symmetrized 12-bin axis.
    // DeltaEta positive half: bins 6..11; DeltaPhi first half: bins 3..8
    const int etaFirst = CL.GetInt("EtaFirstBin", 6);
    const int etaLast  = CL.GetInt("EtaLastBin",  11);
    const int phiFirst = CL.GetInt("PhiFirstBin",  3);
    const int phiLast  = CL.GetInt("PhiLastBin",   8);

    const string pPbOverride = CL.Get("pPbFile", "");
    const string PbPOverride = CL.Get("PbPFile", "");
    const string pPbFile = pPbOverride.empty()
        ? (baseDir + "/pPb_trkResidual_" + tag + "_ZPT0_500-result.root")
        : pPbOverride;
    const string PbPFile = PbPOverride.empty()
        ? (baseDir + "/PbP_trkResidual_" + tag + "_ZPT0_500-result.root")
        : PbPOverride;

    cout << "pPb file: " << pPbFile << endl;
    cout << "PbP file: " << PbPFile << endl;

    TFile fpPb(pPbFile.c_str(), "READ");
    TFile fPbP(PbPFile.c_str(), "READ");
    if (fpPb.IsZombie() || fPbP.IsZombie()) {
        cerr << "Failed to open input files." << endl;
        return 1;
    }

    // Raw histogram keys
    const string jkEtaKey  = "DeltaEta_Result"      + trkRange;
    const string jkPhiKey  = "DeltaPhi_Result"      + trkRange;
    const string sw2EtaKey = "DeltaEta_ResultSumw2" + trkRange;
    const string sw2PhiKey = "DeltaPhi_ResultSumw2" + trkRange;

    // Load raw histograms
    TH1D *pPbJkEta  = loadHistogram(fpPb, jkEtaKey,  "pPb_jk_eta_raw");
    TH1D *PbPJkEta  = loadHistogram(fPbP, jkEtaKey,  "PbP_jk_eta_raw");
    TH1D *pPbSw2Eta = loadHistogram(fpPb, sw2EtaKey, "pPb_sw2_eta_raw");
    TH1D *PbPSw2Eta = loadHistogram(fPbP, sw2EtaKey, "PbP_sw2_eta_raw");
    TH1D *pPbJkPhi  = loadHistogram(fpPb, jkPhiKey,  "pPb_jk_phi_raw");
    TH1D *PbPJkPhi  = loadHistogram(fPbP, jkPhiKey,  "PbP_jk_phi_raw");
    TH1D *pPbSw2Phi = loadHistogram(fpPb, sw2PhiKey, "pPb_sw2_phi_raw");
    TH1D *PbPSw2Phi = loadHistogram(fPbP, sw2PhiKey, "PbP_sw2_phi_raw");

    if (!pPbJkEta||!PbPJkEta||!pPbSw2Eta||!PbPSw2Eta||
        !pPbJkPhi||!PbPJkPhi||!pPbSw2Phi||!PbPSw2Phi) {
        cerr << "Missing one or more result histograms." << endl;
        return 1;
    }

    // Symmetrize histograms
    TH1D *pPbJkEtaS  = symmetrizeHist(pPbJkEta,  true,  "pPb_jk_eta_sym");
    TH1D *PbPJkEtaS  = symmetrizeHist(PbPJkEta,  true,  "PbP_jk_eta_sym");
    TH1D *pPbSw2EtaS = symmetrizeHist(pPbSw2Eta, true,  "pPb_sw2_eta_sym");
    TH1D *PbPSw2EtaS = symmetrizeHist(PbPSw2Eta, true,  "PbP_sw2_eta_sym");
    TH1D *pPbJkPhiS  = symmetrizeHist(pPbJkPhi,  false, "pPb_jk_phi_sym");
    TH1D *PbPJkPhiS  = symmetrizeHist(PbPJkPhi,  false, "PbP_jk_phi_sym");
    TH1D *pPbSw2PhiS = symmetrizeHist(pPbSw2Phi, false, "pPb_sw2_phi_sym");
    TH1D *PbPSw2PhiS = symmetrizeHist(PbPSw2Phi, false, "PbP_sw2_phi_sym");

    // ── Extract 6 unique bins from symmetrized 12-bin histograms ──────────────
    TH1D *pPbJkEtaU  = extractUniqueBins(pPbJkEtaS,  etaFirst, etaLast, "pPb_jk_eta_uniq");
    TH1D *PbPJkEtaU  = extractUniqueBins(PbPJkEtaS,  etaFirst, etaLast, "PbP_jk_eta_uniq");
    TH1D *pPbSw2EtaU = extractUniqueBins(pPbSw2EtaS, etaFirst, etaLast, "pPb_sw2_eta_uniq");
    TH1D *PbPSw2EtaU = extractUniqueBins(PbPSw2EtaS, etaFirst, etaLast, "PbP_sw2_eta_uniq");
    TH1D *pPbJkPhiU  = extractUniqueBins(pPbJkPhiS,  phiFirst, phiLast, "pPb_jk_phi_uniq");
    TH1D *PbPJkPhiU  = extractUniqueBins(PbPJkPhiS,  phiFirst, phiLast, "PbP_jk_phi_uniq");
    TH1D *pPbSw2PhiU = extractUniqueBins(pPbSw2PhiS, phiFirst, phiLast, "pPb_sw2_phi_uniq");
    TH1D *PbPSw2PhiU = extractUniqueBins(PbPSw2PhiS, phiFirst, phiLast, "PbP_sw2_phi_uniq");

    int nUniqEta = etaLast - etaFirst + 1;
    int nUniqPhi = phiLast - phiFirst + 1;
    cout << "Using " << nUniqEta << " unique DeltaEta bins (0-indexed "
         << etaFirst << ".." << etaLast << ")" << endl;
    cout << "Using " << nUniqPhi << " unique DeltaPhi bins (0-indexed "
         << phiFirst << ".." << phiLast << ")" << endl;

    // ── Diagonal chi-square on unique bins ─────────────────────────────────────
    vector<CompatibilityResult> results;
    results.push_back(computeCompatibility(pPbSw2EtaU, PbPSw2EtaU, jkEtaKey, "Sumw2"));
    results.push_back(computeCompatibility(pPbJkEtaU,  PbPJkEtaU,  jkEtaKey, "Jackknife"));
    results.push_back(computeCompatibility(pPbSw2PhiU, PbPSw2PhiU, jkPhiKey, "Sumw2"));
    results.push_back(computeCompatibility(pPbJkPhiU,  PbPJkPhiU,  jkPhiKey, "Jackknife"));

    // ── Full covariance on unique bins of symmetrized event arrays ─────────────
    const string treeName = "JackknifeProjection" + trkRange;
    cout << "Loading JK events from pPb..." << endl;
    vector<JackknifeEventData> pPbOrig = loadJackknifeEvents(fpPb, treeName);
    cout << "Loading JK events from PbP..." << endl;
    vector<JackknifeEventData> PbPOrig = loadJackknifeEvents(fPbP, treeName);
    cout << "pPb: " << pPbOrig.size() << " events, PbP: " << PbPOrig.size() << " events" << endl;

    if (!pPbOrig.empty() && !PbPOrig.empty()) {
        cout << "Building symmetrized+extracted DeltaEta covariance..." << endl;
        auto pPbEtaSym  = makeSymEvents(pPbOrig, true);
        auto PbPEtaSym  = makeSymEvents(PbPOrig, true);
        auto pPbEtaUniq = extractUniqueJKEvents(pPbEtaSym, true, etaFirst, etaLast);
        auto PbPEtaUniq = extractUniqueJKEvents(PbPEtaSym, true, etaFirst, etaLast);
        TMatrixDSym CpPb_eta = buildCovMatrix(pPbEtaUniq, pPbJkEtaU, true);
        TMatrixDSym CPbP_eta = buildCovMatrix(PbPEtaUniq, PbPJkEtaU, true);

        cout << "Building symmetrized+extracted DeltaPhi covariance..." << endl;
        auto pPbPhiSym  = makeSymEvents(pPbOrig, false);
        auto PbPPhiSym  = makeSymEvents(PbPOrig, false);
        auto pPbPhiUniq = extractUniqueJKEvents(pPbPhiSym, false, phiFirst, phiLast);
        auto PbPPhiUniq = extractUniqueJKEvents(PbPPhiSym, false, phiFirst, phiLast);
        TMatrixDSym CpPb_phi = buildCovMatrix(pPbPhiUniq, pPbJkPhiU, false);
        TMatrixDSym CPbP_phi = buildCovMatrix(PbPPhiUniq, PbPJkPhiU, false);

        CompatibilityResult fcEta = computeFullCovCompatibility(
            pPbJkEtaU, PbPJkEtaU, CpPb_eta, CPbP_eta, jkEtaKey);
        CompatibilityResult fcPhi = computeFullCovCompatibility(
            pPbJkPhiU, PbPJkPhiU, CpPb_phi, CPbP_phi, jkPhiKey);

        results.insert(results.begin() + 2, fcEta);
        results.push_back(fcPhi);

        gSystem->mkdir(outputDir.c_str(), true);
        cout << "Saving correlation heatmaps..." << endl;
        saveCorrelationHeatmaps(CpPb_eta, CPbP_eta, pPbJkEtaU, "Eta",
            outputDir + "/pPbPbp_DeltaEta_correlation_matrix.pdf", 0, nUniqEta - 1);
        saveCorrelationHeatmaps(CpPb_phi, CPbP_phi, pPbJkPhiU, "Phi",
            outputDir + "/pPbPbp_DeltaPhi_correlation_matrix.pdf", 0, nUniqPhi - 1);
    } else {
        cerr << "Could not load JK event trees — skipping full covariance." << endl;
    }

    // ── KS test on unique bins ─────────────────────────────────────────────────
    vector<KSResult> ksResults;
    ksResults.push_back(computeKS(pPbJkEtaU, PbPJkEtaU, jkEtaKey));
    ksResults.push_back(computeKS(pPbJkPhiU, PbPJkPhiU, jkPhiKey));

    // ── Write outputs ──────────────────────────────────────────────────────────
    gSystem->mkdir(outputDir.c_str(), true);
    writeMarkdown(outputDir + "/pPbPbp_compatibility.md",  results);
    writeTSV     (outputDir + "/pPbPbp_compatibility.tsv", results);
    writeTeX     (outputDir + "/pPbPbp_compatibility.tex", results);
    writeKSTSV   (outputDir + "/pPbPbp_ks.tsv",           ksResults);
    writeKSTeX   (outputDir + "/pPbPbp_ks.tex",           ksResults);

    cout << "\nChi-square results (signed+symmetrized):\n";
    for (const auto &x : results)
        cout << x.ObservableLabel << " [" << x.ErrorModel << "]"
             << "  chi2=" << x.Chi2 << "  ndf=" << x.UsedBins
             << "  p=" << x.PValue << endl;

    cout << "\nKS results (signed+symmetrized):\n";
    for (const auto &x : ksResults)
        cout << x.ObservableLabel
             << "  N=" << x.N_bins
             << "  D_KS=" << x.D_KS
             << "  p=" << x.KS_PValue << endl;

    return 0;
}
