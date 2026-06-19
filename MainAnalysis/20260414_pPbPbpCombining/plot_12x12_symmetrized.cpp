// Studies 21 and 22: symmetrized final result.
// R_sym(x) = 0.5*[R(+x) + R(-x)] applied to combined pPb+Pbp and pp.
// Stat errors from symmetrized jackknife replicas:
//   d_sym_k[i] = 0.5*(d_k[i] + d_k[n-1-i])  =>  C_sym[i,j] = 0.25*(C[i,j]+C[i,nj]+C[ni,j]+C[ni,nj])

#include <TDecompSVD.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMatrixDSym.h>
#include <TMath.h>
#include <TVectorD.h>
#include <TTree.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <cmath>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"

// ---- Project 2D → 1D, divide by bin width, apply scale ----
TH1D* proj1X(TH2D* h, const char* name, int jmin, int jmax, double sc) {
    TH1D* h1 = h->ProjectionX(name, jmin, jmax);
    h1->SetDirectory(0);
    for (int i = 1; i <= h1->GetNbinsX(); i++) {
        double w = h1->GetBinWidth(i);
        if (w > 0) { h1->SetBinContent(i, h1->GetBinContent(i)/w); h1->SetBinError(i, h1->GetBinError(i)/w); }
    }
    if (sc != 1.0) h1->Scale(sc);
    return h1;
}
TH1D* proj1Y(TH2D* h, const char* name, int imin, int imax, double sc) {
    TH1D* h1 = h->ProjectionY(name, imin, imax);
    h1->SetDirectory(0);
    for (int i = 1; i <= h1->GetNbinsX(); i++) {
        double w = h1->GetBinWidth(i);
        if (w > 0) { h1->SetBinContent(i, h1->GetBinContent(i)/w); h1->SetBinError(i, h1->GetBinError(i)/w); }
    }
    if (sc != 1.0) h1->Scale(sc);
    return h1;
}

// ---- Combine two per-N_Z-normalized TH2D by N_Z weighting ----
TH2D* combineNZ(TH2D* ha, double nza, TH2D* hb, double nzb, const char* name) {
    TH2D* out = (TH2D*)ha->Clone(name);
    out->SetDirectory(0);
    double nztot = nza + nzb;
    int nx = ha->GetNbinsX(), ny = ha->GetNbinsY();
    for (int i = 1; i <= nx; i++) for (int j = 1; j <= ny; j++) {
        double va = ha->GetBinContent(i,j), ea = ha->GetBinError(i,j);
        double vb = hb->GetBinContent(i,j), eb = hb->GetBinError(i,j);
        out->SetBinContent(i,j,(va*nza + vb*nzb)/nztot);
        out->SetBinError(i,j,sqrt(pow(ea*nza/nztot,2)+pow(eb*nzb/nztot,2)));
    }
    return out;
}

// ---- Jackknife event data ----
struct JackknifeEvent {
    double sigNZ, mixNZ;
    vector<float> sigEta, mixEta, sigPhi, mixPhi;
};

vector<JackknifeEvent> loadJackknifeEvents(TFile* f, const string& treeName) {
    TTree* t = (TTree*)f->Get(treeName.c_str());
    if (!t) { cerr << "No tree " << treeName << " in " << f->GetName() << endl; return {}; }
    double sigNZ=0, mixNZ=0;
    vector<float> *sigEta=nullptr, *mixEta=nullptr, *sigPhi=nullptr, *mixPhi=nullptr;
    t->SetBranchAddress("SignalNZ",  &sigNZ);
    t->SetBranchAddress("MixNZ",    &mixNZ);
    t->SetBranchAddress("SignalEta", &sigEta);
    t->SetBranchAddress("MixEta",   &mixEta);
    t->SetBranchAddress("SignalPhi", &sigPhi);
    t->SetBranchAddress("MixPhi",   &mixPhi);
    long long N = t->GetEntries();
    vector<JackknifeEvent> evs(N);
    for (long long k = 0; k < N; ++k) {
        t->GetEntry(k);
        evs[k] = {sigNZ, mixNZ, *sigEta, *mixEta, *sigPhi, *mixPhi};
    }
    return evs;
}

// ---- Build jackknife covariance matrix (regular, not yet symmetrized) ----
TMatrixDSym buildCovMatrix(const vector<JackknifeEvent>& evs, TH1D* axis, bool useEta) {
    int n = axis->GetNbinsX();
    TMatrixDSym C(n);
    if (evs.empty()) return C;

    vector<double> totSig(n,0), totMix(n,0);
    double totSigNZ=0, totMixNZ=0;
    for (const auto& ev : evs) {
        totSigNZ += ev.sigNZ; totMixNZ += ev.mixNZ;
        const auto& s = useEta ? ev.sigEta : ev.sigPhi;
        const auto& m = useEta ? ev.mixEta : ev.mixPhi;
        for (int i=0; i<n && i<(int)s.size(); ++i) { totSig[i]+=s[i]; totMix[i]+=m[i]; }
    }
    if (totSigNZ<=0 || totMixNZ<=0) return C;

    vector<double> bw(n);
    for (int i=0; i<n; ++i) bw[i] = axis->GetBinWidth(i+1);

    vector<double> theta(n);
    for (int i=0; i<n; ++i)
        theta[i] = totSig[i]/totSigNZ - totMix[i]/totMixNZ;

    int validN=0;
    for (const auto& ev : evs) {
        double lSigNZ = totSigNZ - ev.sigNZ, lMixNZ = totMixNZ - ev.mixNZ;
        if (lSigNZ<=0 || lMixNZ<=0) continue;
        const auto& s = useEta ? ev.sigEta : ev.sigPhi;
        const auto& m = useEta ? ev.mixEta : ev.mixPhi;
        vector<double> d(n);
        for (int i=0; i<n; ++i) {
            double loo = (totSig[i]-s[i])/lSigNZ - (totMix[i]-m[i])/lMixNZ;
            d[i] = (loo - theta[i]) / bw[i];
        }
        for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) C(i,j) += d[i]*d[j];
        ++validN;
    }
    if (validN > 1) C *= (double)(validN-1)/validN;
    return C;
}

// ---- Load raw 2D jackknife data from Jackknife2DData tree ----
// Flat index convention (from makeProjection.C::getFlatIndex):
//   idx = (xBin-1)*nYBins + (yBin-1),  xBin=DeltaEta, yBin=DeltaPhi (both 1-indexed)
struct JackknifeEvent2D { double sigNZ, mixNZ; vector<float> sigBins, mixBins; };

vector<JackknifeEvent2D> loadJackknifeEvents2D(TFile* f) {
    TTree* t = (TTree*)f->Get("Jackknife2DData");
    if (!t) { cerr << "No Jackknife2DData in " << f->GetName() << endl; return {}; }
    double sigNZ=0, mixNZ=0;
    vector<float>* sigBins=nullptr, *mixBins=nullptr;
    t->SetBranchAddress("SignalNZ",  &sigNZ);
    t->SetBranchAddress("MixNZ",    &mixNZ);
    t->SetBranchAddress("SignalBins", &sigBins);
    t->SetBranchAddress("MixBins",   &mixBins);
    long long N = t->GetEntries();
    vector<JackknifeEvent2D> evs(N);
    for (long long k = 0; k < N; ++k) {
        t->GetEntry(k);
        evs[k] = {sigNZ, mixNZ, *sigBins, *mixBins};
    }
    return evs;
}

// ---- Build JK covariance for full-eta phi projection ----
// Sums over ALL DeltaEta X bins (0..nEta-1) for each DeltaPhi Y bin.
// Flat index: (xBin_0idx)*nPhi + (yBin_0idx)
TMatrixDSym buildCovMatrixFullEtaPhi(const vector<JackknifeEvent2D>& evs, TH1D* phiAxis, int nEta) {
    int nPhi = phiAxis->GetNbinsX();
    TMatrixDSym C(nPhi);
    if (evs.empty()) return C;

    vector<double> totSig(nPhi,0), totMix(nPhi,0);
    double totSigNZ=0, totMixNZ=0;
    for (const auto& ev : evs) {
        totSigNZ += ev.sigNZ; totMixNZ += ev.mixNZ;
        for (int j=0; j<nPhi; ++j)
            for (int i=0; i<nEta; ++i) {
                int idx = i*nPhi + j;
                if (idx < (int)ev.sigBins.size()) totSig[j] += ev.sigBins[idx];
                if (idx < (int)ev.mixBins.size()) totMix[j] += ev.mixBins[idx];
            }
    }
    if (totSigNZ<=0 || totMixNZ<=0) return C;

    vector<double> bw(nPhi);
    for (int j=0; j<nPhi; ++j) bw[j] = phiAxis->GetBinWidth(j+1);

    vector<double> theta(nPhi);
    for (int j=0; j<nPhi; ++j) theta[j] = totSig[j]/totSigNZ - totMix[j]/totMixNZ;

    int validN=0;
    for (const auto& ev : evs) {
        double lSigNZ = totSigNZ - ev.sigNZ, lMixNZ = totMixNZ - ev.mixNZ;
        if (lSigNZ<=0 || lMixNZ<=0) continue;
        vector<double> d(nPhi, 0);
        for (int j=0; j<nPhi; ++j) {
            double ls=0, lm=0;
            for (int i=0; i<nEta; ++i) {
                int idx = i*nPhi + j;
                if (idx < (int)ev.sigBins.size()) ls += ev.sigBins[idx];
                if (idx < (int)ev.mixBins.size()) lm += ev.mixBins[idx];
            }
            double loo = (totSig[j]-ls)/lSigNZ - (totMix[j]-lm)/lMixNZ;
            d[j] = (loo - theta[j]) / bw[j];
        }
        for (int i=0; i<nPhi; ++i) for (int j=0; j<nPhi; ++j) C(i,j) += d[i]*d[j];
        ++validN;
    }
    if (validN > 1) C *= (double)(validN-1)/validN;
    return C;
}

// ---- Symmetrize covariance matrix (DeltaEta): symmetric about axis center (eta=0).
//   partner: i <-> n-1-i (0-indexed), correct since [-4,4] has bin edge at 0.
TMatrixDSym buildSymCovMatrix(const TMatrixDSym& C) {
    int n = C.GetNrows();
    TMatrixDSym Cs(n);
    for (int i=0; i<n; i++) {
        int ni = n-1-i;
        for (int j=0; j<n; j++) {
            int nj = n-1-j;
            Cs(i,j) = 0.25*(C(i,j) + C(i,nj) + C(ni,j) + C(ni,nj));
        }
    }
    return Cs;
}

// ---- Symmetrize covariance matrix (DeltaPhi): phi -> -phi symmetry about phi=0.
//   On [-pi/2, 3pi/2] axis: near-side [0,5] <-> 5-i; away-side [6,11] <-> 17-i (0-indexed).
TMatrixDSym buildSymCovMatrixPhi(const TMatrixDSym& C) {
    int n = C.GetNrows();
    TMatrixDSym Cs(n);
    for (int i=0; i<n; i++) {
        int ni = (i < n/2) ? (n/2-1-i) : (3*n/2-1-i);
        for (int j=0; j<n; j++) {
            int nj = (j < n/2) ? (n/2-1-j) : (3*n/2-1-j);
            Cs(i,j) = 0.25*(C(i,j) + C(i,nj) + C(ni,j) + C(ni,nj));
        }
    }
    return Cs;
}

// ---- Symmetrize histogram (DeltaEta): h[i] = 0.5*(h[i] + h[n+1-i]).
//   Iterate only i=1..n/2 and set BOTH h[i] and h[ni] to avoid reading modified values.
void symmetrizeHist(TH1D* h) {
    int n = h->GetNbinsX();
    for (int i=1; i<=n/2; i++) {
        int ni = n+1-i;
        double vs = 0.5*(h->GetBinContent(i) + h->GetBinContent(ni));
        h->SetBinContent(i,  vs); h->SetBinContent(ni, vs);
        h->SetBinError(i,  0);   h->SetBinError(ni, 0);
    }
}

// ---- Symmetrize histogram (DeltaPhi): phi -> -phi symmetry about phi=0.
//   Near-side (1..n/2): pairs (1,6),(2,5),(3,4) — iterate i=1..n/4, set both.
//   Away-side (n/2+1..n): pairs (7,12),(8,11),(9,10) — iterate i=n/2+1..3n/4, set both.
void symmetrizeHistPhi(TH1D* h) {
    int n = h->GetNbinsX();
    for (int i=1; i<=n/4; i++) {
        int ni = n/2+1-i;
        double vs = 0.5*(h->GetBinContent(i) + h->GetBinContent(ni));
        h->SetBinContent(i,  vs); h->SetBinContent(ni, vs);
        h->SetBinError(i,  0);   h->SetBinError(ni, 0);
    }
    for (int i=n/2+1; i<=3*n/4; i++) {
        int ni = 3*n/2+1-i;
        double vs = 0.5*(h->GetBinContent(i) + h->GetBinContent(ni));
        h->SetBinContent(i,  vs); h->SetBinContent(ni, vs);
        h->SetBinError(i,  0);   h->SetBinError(ni, 0);
    }
}

// ---- Apply JK diagonal errors ----
void applyJKErrors(TH1D* h, const TMatrixDSym& C) {
    int n = min(h->GetNbinsX(), C.GetNrows());
    for (int i=0; i<n; ++i) h->SetBinError(i+1, sqrt(max(0.0, C(i,i))));
}

// ---- Full covariance chi-square via SVD pseudoinverse ----
pair<double,int> fullCovChiSq(TH1D* hHI, TH1D* hpp,
                               const TMatrixDSym& C_HI, const TMatrixDSym& C_pp) {
    int n = min({hHI->GetNbinsX(), hpp->GetNbinsX(), C_HI.GetNrows(), C_pp.GetNrows()});
    TMatrixDSym Ctot(n);
    for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) Ctot(i,j) = C_HI(i,j) + C_pp(i,j);

    TVectorD delta(n);
    for (int i=0; i<n; ++i) delta(i) = hHI->GetBinContent(i+1) - hpp->GetBinContent(i+1);

    TDecompSVD svd(Ctot);
    const TVectorD& sv = svd.GetSig();
    int nsv = sv.GetNrows();
    double svMax = (nsv>0) ? sv(0) : 0;
    double tol   = svMax * 1e-8;
    int rank=0;
    for (int k=0; k<nsv; ++k) if (sv(k)>tol) ++rank;

    cout << "  SVD rank=" << rank << "/" << n << "  sv_max=" << svMax << endl;
    if (rank==0) return {0.0, 0};

    const TMatrixD& U = svd.GetU();
    const TMatrixD& V = svd.GetV();
    TMatrixD Cpinv(n,n);
    for (int k=0; k<nsv; ++k) {
        if (sv(k)<=tol) continue;
        double inv = 1.0/sv(k);
        for (int i=0; i<n; ++i) for (int j=0; j<n; ++j)
            Cpinv(i,j) += V(i,k)*inv*U(j,k);
    }
    TVectorD Cpd = Cpinv * delta;
    return {delta*Cpd, rank};
}

// ---- Make 2-curve comparison plot ----
static int gUID = 0;
void makePlot(TH1D* hComb, TH1D* hPP,
              const string& combLabel, const string& ppLabel,
              const char* xTitle, double xMin, double xMax,
              const char* yTitle, const string& outPath) {
    SetTDRStyle();
    string uid = Form("sy%d", gUID++);
    vector<TH1*> hv = {hPP, hComb};
    vector<string> labels = {ppLabel, combLabel};
    vector<int> cols = {kRed+1, cmsBlue};
    vector<int> ls = {0, 0}, ms = {mCircleFill, mCircleFill};

    double lo=1e30, hi=-1e30;
    for (auto h : hv) {
        int b1=max(1,h->FindBin(xMin+1e-9)), b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
        for (int b=b1; b<=b2; b++) { double v=h->GetBinContent(b),e=h->GetBinError(b); lo=min(lo,v-e); hi=max(hi,v+e); }
    }
    double rng=hi-lo, p=0.15/(1.0-2*0.15);
    double tlo=lo-p*rng, thi=hi+p*rng;

    double dmax=1e-6;
    for (int i=1;i<=hComb->GetNbinsX();i++) { double d=hComb->GetBinContent(i)-hPP->GetBinContent(i); dmax=max(dmax,fabs(d)+hypot(hComb->GetBinError(i),hPP->GetBinError(i))); }
    dmax/=0.85;

    TCanvas* c = new TCanvas(uid.c_str(), uid.c_str(), 600, 600);
    TPad* pad = (TPad*)plotCMSDiff(hv,(uid+"d").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Diff comb.#minuspp",-dmax,dmax,0,false,false,true,0.12);
    if (pad) { AddCMSHeader(pad,"Internal",false); AddUPCHeader(pad,"8.16 TeV","PPb"); }
    c->Update(); c->SaveAs(outPath.c_str()); delete c;
}

int main(int argc, char* argv[]) {
    CommandLine CL(argc, argv);
    string pPbFile    = CL.Get("pPbFile");
    string PbPFile    = CL.Get("PbPFile");
    string ppFile     = CL.Get("ppFile");
    string pPbResFile = CL.Get("pPbResultFile", "");
    string PbPResFile = CL.Get("PbPResultFile", "");
    string ppResFile  = CL.Get("ppResultFile",  "");
    // Raw intermediate files (contain Jackknife2DData for full-eta phi JK covariance)
    string pPbRawFile = CL.Get("pPbRawFile", "");
    string PbPRawFile = CL.Get("PbPRawFile", "");
    string ppRawFile  = CL.Get("ppRawFile",  "");
    string outDir     = CL.Get("OutputDir", "plots/12x12_symmetrized");
    string tag        = CL.Get("Tag", "sym");
    string key        = CL.Get("TrkPtKey", "0.5_15");
    mkdir(outDir.c_str(), 0755);

    // ---- Load nosub inputs ----
    TFile* fpPb = TFile::Open(pPbFile.c_str(), "READ");
    TFile* fPbP = TFile::Open(PbPFile.c_str(), "READ");
    TFile* fpp  = TFile::Open(ppFile.c_str(),  "READ");
    for (auto& [nm,f] : vector<pair<string,TFile*>>{{pPbFile,fpPb},{PbPFile,fPbP},{ppFile,fpp}}) {
        if (!f||f->IsZombie()) { cerr<<"Cannot open: "<<nm<<endl; return 1; }
    }

    TH2D *sePPb=(TH2D*)fpPb->Get(Form("hData_%s",    key.c_str()));
    TH2D *mePPb=(TH2D*)fpPb->Get(Form("hMixData_%s", key.c_str()));
    TH1D *nzPPb=(TH1D*)fpPb->Get(Form("hNZData_%s",  key.c_str()));
    TH2D *sePbP=(TH2D*)fPbP->Get(Form("hData_%s",    key.c_str()));
    TH2D *mePbP=(TH2D*)fPbP->Get(Form("hMixData_%s", key.c_str()));
    TH1D *nzPbP=(TH1D*)fPbP->Get(Form("hNZData_%s",  key.c_str()));
    TH2D *sepp =(TH2D*)fpp ->Get(Form("hData_%s",    key.c_str()));
    TH2D *mepp =(TH2D*)fpp ->Get(Form("hMixData_%s", key.c_str()));
    for (auto& [nm,h] : vector<pair<string,void*>>{
            {"sePPb",sePPb},{"mePPb",mePPb},{"nzPPb",nzPPb},
            {"sePbP",sePbP},{"mePbP",mePbP},{"nzPbP",nzPbP},
            {"sepp",sepp},{"mepp",mepp}}) {
        if (!h) { cerr<<"Missing: "<<nm<<" key="<<key<<endl; return 1; }
    }

    double NZ_pPb = nzPPb->GetBinContent(1);
    double NZ_PbP = nzPbP->GetBinContent(1);
    double NZ_tot = NZ_pPb + NZ_PbP;
    cout << "N_Z: pPb=" << NZ_pPb << "  Pbp=" << NZ_PbP << endl;

    // ---- Combine pPb + Pbp (N_Z weighted) ----
    TH2D* seCombo = combineNZ(sePPb, NZ_pPb, sePbP, NZ_PbP, "seCombo");
    TH2D* meCombo = combineNZ(mePPb, NZ_pPb, mePbP, NZ_PbP, "meCombo");
    TH2D* resCombo = (TH2D*)seCombo->Clone("resCombo"); resCombo->Add(meCombo, -1.0);
    TH2D* respp    = (TH2D*)sepp   ->Clone("respp");    respp   ->Add(mepp,    -1.0);

    // ---- 1D projections ----
    // DeltaEta: project over phi bins 4-6 (|DeltaPhi| in [0, pi/2])
    // DeltaPhi: project over full eta range (bins 1-12)
    TH1D* reEtaComb = proj1X(resCombo, "reEtaComb", 4, 6,  0.5);
    // DeltaPhi: project over full DeltaEta range (all 12 X bins, scale 1.0).
    // JK errors come from Jackknife2DData (full 2D raw data) via buildCovMatrixFullEtaPhi,
    // NOT from the JackknifeProjection tree whose SignalPhi only covers pos-eta (bins 7-12).
    TH1D* rePhiComb = proj1Y(resCombo, "rePhiComb", 1, 12, 1.0);
    TH1D* reEtapp   = proj1X(respp,    "reEtapp",   4, 6,  0.5);
    TH1D* rePhipp   = proj1Y(respp,    "rePhipp",   1, 12, 1.0);

    // ---- Symmetrize ----
    symmetrizeHist(reEtaComb);
    symmetrizeHistPhi(rePhiComb);
    symmetrizeHist(reEtapp);
    symmetrizeHistPhi(rePhipp);

    // ---- Jackknife covariance ----
    // Eta JK: from JackknifeProjection tree (SignalEta covers phi bins 4-6, matching eta projection).
    // Phi JK: from Jackknife2DData raw tree (full 2D), summed over ALL DeltaEta bins.
    bool haveJK    = (!pPbResFile.empty() && !PbPResFile.empty() && !ppResFile.empty());
    bool haveJK2D  = (!pPbRawFile.empty() && !PbPRawFile.empty() && !ppRawFile.empty());
    TMatrixDSym C_combEtaSym(12), C_combPhiSym(12), C_ppEtaSym(12), C_ppPhiSym(12);

    if (haveJK) {
        TFile* fPPbR = TFile::Open(pPbResFile.c_str(), "READ");
        TFile* fPbPR = TFile::Open(PbPResFile.c_str(), "READ");
        TFile* fppR  = TFile::Open(ppResFile.c_str(),  "READ");
        for (auto& [nm,f] : vector<pair<string,TFile*>>{{pPbResFile,fPPbR},{PbPResFile,fPbPR},{ppResFile,fppR}}) {
            if (!f||f->IsZombie()) { cerr<<"Cannot open result file: "<<nm<<"\nFalling back to Sumw2 errors."<<endl; haveJK=false; break; }
        }

        if (haveJK) {
            string treeName = "JackknifeProjection" + key;
            auto evsPPb = loadJackknifeEvents(fPPbR, treeName);
            auto evsPbP = loadJackknifeEvents(fPbPR, treeName);
            auto evspp  = loadJackknifeEvents(fppR,  treeName);
            cout << "Jackknife events: pPb=" << evsPPb.size() << "  Pbp=" << evsPbP.size() << "  pp=" << evspp.size() << endl;

            // Eta covariance from JackknifeProjection (SignalEta: phi bins 4-6, correct for eta proj)
            TMatrixDSym C_pPbEta = buildCovMatrix(evsPPb, reEtaComb, true);
            TMatrixDSym C_PbPEta = buildCovMatrix(evsPbP, reEtaComb, true);
            TMatrixDSym C_ppEta  = buildCovMatrix(evspp,  reEtapp,  true);

            double wPPb = NZ_pPb/NZ_tot, wPbP = NZ_PbP/NZ_tot;
            int nEta = reEtaComb->GetNbinsX(), nPhi = rePhiComb->GetNbinsX();
            TMatrixDSym C_combEta(nEta);
            for (int i=0; i<nEta; ++i) for (int j=0; j<nEta; ++j)
                C_combEta(i,j) = wPPb*wPPb*C_pPbEta(i,j) + wPbP*wPbP*C_PbPEta(i,j);
            C_combEtaSym = buildSymCovMatrix(C_combEta);
            C_ppEtaSym   = buildSymCovMatrix(C_ppEta);
            applyJKErrors(reEtaComb, C_combEtaSym);
            applyJKErrors(reEtapp,   C_ppEtaSym);

            // Phi covariance: use full-eta 2D JK if raw files available, else fall back to
            // pos-eta SignalPhi (which would inflate HI errors due to eta cancellation).
            if (haveJK2D) {
                TFile* fPPbRaw = TFile::Open(pPbRawFile.c_str(), "READ");
                TFile* fPbPRaw = TFile::Open(PbPRawFile.c_str(), "READ");
                TFile* fppRaw  = TFile::Open(ppRawFile.c_str(),  "READ");
                bool ok2D = true;
                for (auto& [nm,f] : vector<pair<string,TFile*>>{{pPbRawFile,fPPbRaw},{PbPRawFile,fPbPRaw},{ppRawFile,fppRaw}})
                    if (!f||f->IsZombie()) { cerr<<"Cannot open raw file: "<<nm<<endl; ok2D=false; break; }

                if (ok2D) {
                    auto evs2DPPb = loadJackknifeEvents2D(fPPbRaw);
                    auto evs2DPbP = loadJackknifeEvents2D(fPbPRaw);
                    auto evs2Dpp  = loadJackknifeEvents2D(fppRaw);
                    cout << "2D JK events (full-eta phi): pPb=" << evs2DPPb.size()
                         << "  Pbp=" << evs2DPbP.size() << "  pp=" << evs2Dpp.size() << endl;

                    int nEtaBins = sePPb->GetNbinsX();
                    TMatrixDSym C_pPbPhi = buildCovMatrixFullEtaPhi(evs2DPPb, rePhiComb, nEtaBins);
                    TMatrixDSym C_PbPPhi = buildCovMatrixFullEtaPhi(evs2DPbP, rePhiComb, nEtaBins);
                    TMatrixDSym C_ppPhi  = buildCovMatrixFullEtaPhi(evs2Dpp,  rePhipp,   nEtaBins);
                    TMatrixDSym C_combPhi(nPhi);
                    for (int i=0; i<nPhi; ++i) for (int j=0; j<nPhi; ++j)
                        C_combPhi(i,j) = wPPb*wPPb*C_pPbPhi(i,j) + wPbP*wPbP*C_PbPPhi(i,j);
                    C_combPhiSym = buildSymCovMatrixPhi(C_combPhi);
                    C_ppPhiSym   = buildSymCovMatrixPhi(C_ppPhi);
                    applyJKErrors(rePhiComb, C_combPhiSym);
                    applyJKErrors(rePhipp,   C_ppPhiSym);
                    fPPbRaw->Close(); fPbPRaw->Close(); fppRaw->Close();
                    cout << "Full-eta phi JK errors applied." << endl;
                } else {
                    haveJK2D = false;
                }
            }
            if (!haveJK2D) {
                cerr << "Warning: no raw files for phi JK — phi error bars will be Sumw2." << endl;
            }

            fPPbR->Close(); fPbPR->Close(); fppR->Close();
            cout << "Symmetrized jackknife errors applied." << endl;
        }
    }

    // ---- Plots ----
    auto path = [&](const string& suf) { return outDir+"/"+tag+"-"+suf+".pdf"; };
    string combLabel = "pPb+Pbp combined (sym.)";
    string ppLabel   = "pp (8.16 TeV, sym.)";
    const double etaMin=-4.0, etaMax=4.0;
    const double phiMin=-M_PI/2.0, phiMax=3.0*M_PI/2.0;

    makePlot(reEtaComb, reEtapp, combLabel, ppLabel,
             "#Delta y_{ch,Z}", etaMin, etaMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}",
             path("Result-DeltaEta"));
    makePlot(rePhiComb, rePhipp, combLabel, ppLabel,
             "#Delta#phi_{ch,Z}", phiMin, phiMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}",
             path("Result-DeltaPhi"));

    // ---- P-value table ----
    double pval_eta_diag=-1, pval_phi_diag=-1;
    double pval_eta_cov=-1,  pval_phi_cov=-1;
    int ndf_eta_diag=0, ndf_phi_diag=0;
    double chi2_eta_diag=0, chi2_phi_diag=0;
    int rank_eta=0, rank_phi=0;
    double chi2_eta_cov=0, chi2_phi_cov=0;

    // After symmetrization bins i and ni are identical; iterate only the 6 unique first-of-pair bins.
    for (int i=1; i<=reEtaComb->GetNbinsX()/2; i++) {
        double e2 = pow(reEtaComb->GetBinError(i),2)+pow(reEtapp->GetBinError(i),2);
        if (e2<=0) continue;
        chi2_eta_diag += pow(reEtaComb->GetBinContent(i)-reEtapp->GetBinContent(i),2)/e2;
        ++ndf_eta_diag;
    }
    // Near-side unique bins: 1..3; away-side unique bins: 7..9 (12-bin phi axis).
    { int nPhi = rePhiComb->GetNbinsX();
      for (int i=1; i<=nPhi/4; i++) {
          double e2 = pow(rePhiComb->GetBinError(i),2)+pow(rePhipp->GetBinError(i),2);
          if (e2<=0) continue;
          chi2_phi_diag += pow(rePhiComb->GetBinContent(i)-rePhipp->GetBinContent(i),2)/e2;
          ++ndf_phi_diag;
      }
      for (int i=nPhi/2+1; i<=3*nPhi/4; i++) {
          double e2 = pow(rePhiComb->GetBinError(i),2)+pow(rePhipp->GetBinError(i),2);
          if (e2<=0) continue;
          chi2_phi_diag += pow(rePhiComb->GetBinContent(i)-rePhipp->GetBinContent(i),2)/e2;
          ++ndf_phi_diag;
      }
    }
    pval_eta_diag = (ndf_eta_diag>0) ? TMath::Prob(chi2_eta_diag, ndf_eta_diag) : -1;
    pval_phi_diag = (ndf_phi_diag>0) ? TMath::Prob(chi2_phi_diag, ndf_phi_diag) : -1;

    if (haveJK) {
        cout << "Full covariance DeltaEta (symmetrized):" << endl;
        auto [c2eta, rketa] = fullCovChiSq(reEtaComb, reEtapp, C_combEtaSym, C_ppEtaSym);
        chi2_eta_cov=c2eta; rank_eta=rketa;
        pval_eta_cov = (rketa>0) ? TMath::Prob(c2eta, rketa) : -1;

        cout << "Full covariance DeltaPhi (symmetrized):" << endl;
        auto [c2phi, rkphi] = fullCovChiSq(rePhiComb, rePhipp, C_combPhiSym, C_ppPhiSym);
        chi2_phi_cov=c2phi; rank_phi=rkphi;
        pval_phi_cov = (rkphi>0) ? TMath::Prob(c2phi, rkphi) : -1;
    }

    cout << "\n--- Chi-square summary (symmetrized) ---\n";
    cout << Form("DeltaEta diagonal(JK): chi2=%.3g  ndf=%d  p=%.4g\n", chi2_eta_diag, ndf_eta_diag, pval_eta_diag);
    cout << Form("DeltaPhi diagonal(JK): chi2=%.3g  ndf=%d  p=%.4g\n", chi2_phi_diag, ndf_phi_diag, pval_phi_diag);
    if (haveJK) {
        cout << Form("DeltaEta fullCov:       chi2=%.3g  ndf=%d  p=%.4g\n", chi2_eta_cov, rank_eta, pval_eta_cov);
        cout << Form("DeltaPhi fullCov:       chi2=%.3g  ndf=%d  p=%.4g\n", chi2_phi_cov, rank_phi, pval_phi_cov);
    }

    string texPath = outDir + "/" + tag + "-pvalue.tex";
    ofstream tex(texPath);
    tex << "\\begin{tabular}{|l|r|r|r|r|}\n\\hline\n";
    tex << "Observable & $\\chi^2$ & ndf & $\\chi^2/\\mathrm{ndf}$ & $p$-value \\\\\n\\hline\n";
    auto fmtRow = [&](const string& obs, double c2, int ndf, double pv) {
        tex << Form("%s & %.3g & %d & %.3g & %.4g \\\\\n", obs.c_str(), c2, ndf, ndf>0?c2/ndf:-1.0, pv);
    };
    fmtRow("$\\Delta y_{ch,Z}$ sym.\\ diagonal (JK)", chi2_eta_diag, ndf_eta_diag, pval_eta_diag);
    fmtRow("$\\Delta\\phi_{ch,Z}$ sym.\\ diagonal (JK)", chi2_phi_diag, ndf_phi_diag, pval_phi_diag);
    if (haveJK) {
        fmtRow("$\\Delta y_{ch,Z}$ sym.\\ full covariance", chi2_eta_cov, rank_eta, pval_eta_cov);
        fmtRow("$\\Delta\\phi_{ch,Z}$ sym.\\ full covariance", chi2_phi_cov, rank_phi, pval_phi_cov);
    }
    tex << "\\hline\n\\end{tabular}\n";
    tex.close();
    cout << "P-value table: " << texPath << endl;

    fpPb->Close(); fPbP->Close(); fpp->Close();
    cout << "=== Done: " << outDir << "/" << tag << "-*.pdf ===" << endl;
    return 0;
}
