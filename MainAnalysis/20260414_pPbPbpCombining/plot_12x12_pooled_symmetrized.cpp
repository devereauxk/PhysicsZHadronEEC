// Pooled JK combination + 2D symmetrization for signed common-CM results.
// Pools pPb + Pbp jackknife events, computes combined estimator,
// symmetrizes at the replica level, and projects to 1D.

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
#include <numeric>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"

struct JKEvent2D { double sigNZ, mixNZ; vector<float> sigBins, mixBins; };

vector<JKEvent2D> loadJK2D(TFile* f) {
    TTree* t = (TTree*)f->Get("Jackknife2DData");
    if (!t) { cerr << "No Jackknife2DData in " << f->GetName() << endl; return {}; }
    double sigNZ=0, mixNZ=0;
    vector<float>* sigBins=nullptr, *mixBins=nullptr;
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

// 12x12 bin mirror indices (1-indexed)
// DeltaEta: i <-> 13-i
// DeltaPhi on [-pi/2, 3pi/2]: near-side bins 1..6 mirror as 7-i; away-side 7..12 mirror as 19-i
int etaMirror(int i) { return 13 - i; }
int phiMirror(int j) {
    if (j <= 6) return 7 - j;
    return 19 - j;
}

// Flat index: (xBin-1)*nPhi + (yBin-1), xBin=DeltaEta, yBin=DeltaPhi (1-indexed)
int flatIdx(int ix, int jy, int nPhi) { return (ix-1)*nPhi + (jy-1); }

// Compute 2D result from totals
void compute2DResult(const vector<double>& totSig, const vector<double>& totMix,
                     double totSigNZ, double totMixNZ,
                     int nEta, int nPhi, vector<double>& result) {
    int nBins = nEta * nPhi;
    result.resize(nBins);
    for (int b = 0; b < nBins; ++b)
        result[b] = (totSigNZ > 0 ? totSig[b]/totSigNZ : 0) - (totMixNZ > 0 ? totMix[b]/totMixNZ : 0);
}

// 2D fourfold symmetrization: R_sym(i,j) = 1/4*(R(i,j)+R(ni,j)+R(i,nj)+R(ni,nj))
void symmetrize2D(vector<double>& r, int nEta, int nPhi) {
    vector<double> sym(r.size(), 0);
    for (int i = 1; i <= nEta; ++i) for (int j = 1; j <= nPhi; ++j) {
        int ni = etaMirror(i), nj = phiMirror(j);
        double avg = 0.25 * (r[flatIdx(i,j,nPhi)] + r[flatIdx(ni,j,nPhi)]
                            + r[flatIdx(i,nj,nPhi)] + r[flatIdx(ni,nj,nPhi)]);
        sym[flatIdx(i,j,nPhi)] = avg;
    }
    r = sym;
}

// Project symmetrized 2D → 1D DeltaEta (sum over phi window, divide by bin width)
void projectEta(const vector<double>& r2d, int nEta, int nPhi,
                int phiFirst, int phiLast, double scale,
                const TH1D* axis, vector<double>& proj) {
    proj.resize(nEta, 0);
    for (int i = 1; i <= nEta; ++i) {
        double sum = 0;
        for (int j = phiFirst; j <= phiLast; ++j) sum += r2d[flatIdx(i,j,nPhi)];
        double bw = axis->GetBinWidth(i);
        proj[i-1] = (bw > 0 ? sum / bw : 0) * scale;
    }
}

// Project symmetrized 2D → 1D DeltaPhi (sum over full eta, divide by bin width)
void projectPhi(const vector<double>& r2d, int nEta, int nPhi,
                double scale, const TH1D* axis, vector<double>& proj) {
    proj.resize(nPhi, 0);
    for (int j = 1; j <= nPhi; ++j) {
        double sum = 0;
        for (int i = 1; i <= nEta; ++i) sum += r2d[flatIdx(i,j,nPhi)];
        double bw = axis->GetBinWidth(j);
        proj[j-1] = (bw > 0 ? sum / bw : 0) * scale;
    }
}

// Full covariance chi-square via SVD pseudoinverse
pair<double,int> fullCovChiSq(TH1D* hA, TH1D* hB, const TMatrixDSym& CA, const TMatrixDSym& CB) {
    int n = min({hA->GetNbinsX(), hB->GetNbinsX(), CA.GetNrows(), CB.GetNrows()});
    TMatrixDSym Ctot(n);
    for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) Ctot(i,j) = CA(i,j) + CB(i,j);
    TVectorD delta(n);
    for (int i=0; i<n; ++i) delta(i) = hA->GetBinContent(i+1) - hB->GetBinContent(i+1);
    TDecompSVD svd(Ctot);
    const TVectorD& sv = svd.GetSig();
    int nsv = sv.GetNrows();
    double svMax = (nsv>0) ? sv(0) : 0;
    double tol = svMax * 1e-8;
    int rank=0;
    for (int k=0; k<nsv; ++k) if (sv(k)>tol) ++rank;
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

static int gUID = 0;
void makePlot(TH1D* hComb, TH1D* hPP,
              const string& combLabel, const string& ppLabel,
              const char* xTitle, double xMin, double xMax,
              const char* yTitle, const string& outPath) {
    SetTDRStyle();
    string uid = Form("ps%d", gUID++);
    vector<TH1*> hv = {hPP, hComb};
    vector<string> labels = {ppLabel, combLabel};
    vector<int> cols = {kRed+1, cmsBlue}, ls = {0,0}, ms = {mCircleFill, mCircleFill};
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
    string pPbRawFile = CL.Get("pPbRawFile");
    string PbPRawFile = CL.Get("PbPRawFile");
    string ppRawFile  = CL.Get("ppRawFile");
    string outDir     = CL.Get("OutputDir", "plots/pooled_symmetrized");
    string tag        = CL.Get("Tag", "pooled_sym");
    string key        = CL.Get("TrkPtKey", "0.5_15");
    mkdir(outDir.c_str(), 0755);

    // Load nosub inputs
    TFile* fpPb = TFile::Open(pPbFile.c_str(), "READ");
    TFile* fPbP = TFile::Open(PbPFile.c_str(), "READ");
    TFile* fpp  = TFile::Open(ppFile.c_str(),  "READ");
    for (auto& [nm,f] : vector<pair<string,TFile*>>{{pPbFile,fpPb},{PbPFile,fPbP},{ppFile,fpp}})
        if (!f||f->IsZombie()) { cerr<<"Cannot open: "<<nm<<endl; return 1; }

    TH2D *sePPb=(TH2D*)fpPb->Get(Form("hData_%s",    key.c_str()));
    TH2D *mePPb=(TH2D*)fpPb->Get(Form("hMixData_%s", key.c_str()));
    TH1D *nzPPb=(TH1D*)fpPb->Get(Form("hNZData_%s",  key.c_str()));
    TH2D *sePbP=(TH2D*)fPbP->Get(Form("hData_%s",    key.c_str()));
    TH2D *mePbP=(TH2D*)fPbP->Get(Form("hMixData_%s", key.c_str()));
    TH1D *nzPbP=(TH1D*)fPbP->Get(Form("hNZData_%s",  key.c_str()));
    TH2D *sepp =(TH2D*)fpp ->Get(Form("hData_%s",    key.c_str()));
    TH2D *mepp =(TH2D*)fpp ->Get(Form("hMixData_%s", key.c_str()));
    TH1D *nzpp =(TH1D*)fpp ->Get(Form("hNZData_%s",  key.c_str()));
    for (auto& [nm,h] : vector<pair<string,void*>>{
            {"sePPb",sePPb},{"mePPb",mePPb},{"nzPPb",nzPPb},
            {"sePbP",sePbP},{"mePbP",mePbP},{"nzPbP",nzPbP},
            {"sepp",sepp},{"mepp",mepp},{"nzpp",nzpp}})
        if (!h) { cerr<<"Missing: "<<nm<<" key="<<key<<endl; return 1; }

    int nEta = sePPb->GetNbinsX(), nPhi = sePPb->GetNbinsY();
    int nBins = nEta * nPhi;
    cout << "Histogram bins: " << nEta << "x" << nPhi << " = " << nBins << endl;
    double NZ_pPb = nzPPb->GetBinContent(1);
    double NZ_PbP = nzPbP->GetBinContent(1);
    cout << "N_Z: pPb=" << NZ_pPb << "  Pbp=" << NZ_PbP << endl;

    // Load raw JK events
    TFile* fPPbRaw = TFile::Open(pPbRawFile.c_str(), "READ");
    TFile* fPbPRaw = TFile::Open(PbPRawFile.c_str(), "READ");
    TFile* fppRaw  = TFile::Open(ppRawFile.c_str(),  "READ");
    for (auto& [nm,f] : vector<pair<string,TFile*>>{{pPbRawFile,fPPbRaw},{PbPRawFile,fPbPRaw},{ppRawFile,fppRaw}})
        if (!f||f->IsZombie()) { cerr<<"Cannot open raw: "<<nm<<endl; return 1; }

    auto evsPPb = loadJK2D(fPPbRaw);
    auto evsPbP = loadJK2D(fPbPRaw);
    auto evspp  = loadJK2D(fppRaw);
    cout << "JK events: pPb=" << evsPPb.size() << " Pbp=" << evsPbP.size() << " pp=" << evspp.size() << endl;

    // ====== Pool pPb + Pbp ======
    vector<JKEvent2D> evsHI;
    evsHI.reserve(evsPPb.size() + evsPbP.size());
    evsHI.insert(evsHI.end(), evsPPb.begin(), evsPPb.end());
    evsHI.insert(evsHI.end(), evsPbP.begin(), evsPbP.end());

    // Compute pooled totals
    vector<double> totSigHI(nBins, 0), totMixHI(nBins, 0);
    double totSigNZ_HI = 0, totMixNZ_HI = 0;
    for (const auto& ev : evsHI) {
        totSigNZ_HI += ev.sigNZ; totMixNZ_HI += ev.mixNZ;
        for (int b = 0; b < nBins && b < (int)ev.sigBins.size(); ++b) totSigHI[b] += ev.sigBins[b];
        for (int b = 0; b < nBins && b < (int)ev.mixBins.size(); ++b) totMixHI[b] += ev.mixBins[b];
    }
    cout << "Pooled HI: sigNZ=" << totSigNZ_HI << " mixNZ=" << totMixNZ_HI << " events=" << evsHI.size() << endl;

    // Full-sample 2D estimator
    vector<double> r2d_HI;
    compute2DResult(totSigHI, totMixHI, totSigNZ_HI, totMixNZ_HI, nEta, nPhi, r2d_HI);

    // Symmetrize full-sample
    vector<double> r2d_HI_sym = r2d_HI;
    symmetrize2D(r2d_HI_sym, nEta, nPhi);

    // Same for pp
    vector<double> totSigPP(nBins, 0), totMixPP(nBins, 0);
    double totSigNZ_PP = 0, totMixNZ_PP = 0;
    for (const auto& ev : evspp) {
        totSigNZ_PP += ev.sigNZ; totMixNZ_PP += ev.mixNZ;
        for (int b = 0; b < nBins && b < (int)ev.sigBins.size(); ++b) totSigPP[b] += ev.sigBins[b];
        for (int b = 0; b < nBins && b < (int)ev.mixBins.size(); ++b) totMixPP[b] += ev.mixBins[b];
    }
    vector<double> r2d_PP;
    compute2DResult(totSigPP, totMixPP, totSigNZ_PP, totMixNZ_PP, nEta, nPhi, r2d_PP);
    vector<double> r2d_PP_sym = r2d_PP;
    symmetrize2D(r2d_PP_sym, nEta, nPhi);

    // Create 1D axes from the 2D histogram
    TH1D* etaAxis = sePPb->ProjectionX("_etaAxis"); etaAxis->Reset();
    TH1D* phiAxis = sePPb->ProjectionY("_phiAxis"); phiAxis->Reset();

    // Projection windows (12x12): DeltaEta projection over phi bins 4-6 (DeltaPhi 0..pi/2)
    int phiFirst = 4, phiLast = 6;
    double etaScale = 0.5;
    double phiScale = 1.0;

    // Project full-sample symmetrized central values
    vector<double> etaHI, phiHI, etaPP, phiPP;
    projectEta(r2d_HI_sym, nEta, nPhi, phiFirst, phiLast, etaScale, etaAxis, etaHI);
    projectPhi(r2d_HI_sym, nEta, nPhi, phiScale, phiAxis, phiHI);
    projectEta(r2d_PP_sym, nEta, nPhi, phiFirst, phiLast, etaScale, etaAxis, etaPP);
    projectPhi(r2d_PP_sym, nEta, nPhi, phiScale, phiAxis, phiPP);

    // ====== Pooled JK covariance for HI ======
    int N_HI = evsHI.size();
    TMatrixDSym C_HI_eta(nEta), C_HI_phi(nPhi);
    int validHI = 0;
    for (int e = 0; e < N_HI; ++e) {
        double lSigNZ = totSigNZ_HI - evsHI[e].sigNZ;
        double lMixNZ = totMixNZ_HI - evsHI[e].mixNZ;
        if (lSigNZ <= 0 || lMixNZ <= 0) continue;

        vector<double> r2d_loo(nBins);
        for (int b = 0; b < nBins; ++b) {
            double ls = totSigHI[b] - (b < (int)evsHI[e].sigBins.size() ? evsHI[e].sigBins[b] : 0);
            double lm = totMixHI[b] - (b < (int)evsHI[e].mixBins.size() ? evsHI[e].mixBins[b] : 0);
            r2d_loo[b] = ls/lSigNZ - lm/lMixNZ;
        }
        symmetrize2D(r2d_loo, nEta, nPhi);

        vector<double> etaLoo, phiLoo;
        projectEta(r2d_loo, nEta, nPhi, phiFirst, phiLast, etaScale, etaAxis, etaLoo);
        projectPhi(r2d_loo, nEta, nPhi, phiScale, phiAxis, phiLoo);

        for (int i = 0; i < nEta; ++i) for (int j = 0; j < nEta; ++j)
            C_HI_eta(i,j) += (etaLoo[i] - etaHI[i]) * (etaLoo[j] - etaHI[j]);
        for (int i = 0; i < nPhi; ++i) for (int j = 0; j < nPhi; ++j)
            C_HI_phi(i,j) += (phiLoo[i] - phiHI[i]) * (phiLoo[j] - phiHI[j]);
        ++validHI;
    }
    if (validHI > 1) { C_HI_eta *= (double)(validHI-1)/validHI; C_HI_phi *= (double)(validHI-1)/validHI; }
    cout << "Pooled HI JK: " << validHI << " valid replicas" << endl;

    // ====== pp JK covariance ======
    int N_PP = evspp.size();
    TMatrixDSym C_PP_eta(nEta), C_PP_phi(nPhi);
    int validPP = 0;
    for (int e = 0; e < N_PP; ++e) {
        double lSigNZ = totSigNZ_PP - evspp[e].sigNZ;
        double lMixNZ = totMixNZ_PP - evspp[e].mixNZ;
        if (lSigNZ <= 0 || lMixNZ <= 0) continue;

        vector<double> r2d_loo(nBins);
        for (int b = 0; b < nBins; ++b) {
            double ls = totSigPP[b] - (b < (int)evspp[e].sigBins.size() ? evspp[e].sigBins[b] : 0);
            double lm = totMixPP[b] - (b < (int)evspp[e].mixBins.size() ? evspp[e].mixBins[b] : 0);
            r2d_loo[b] = ls/lSigNZ - lm/lMixNZ;
        }
        symmetrize2D(r2d_loo, nEta, nPhi);

        vector<double> etaLoo, phiLoo;
        projectEta(r2d_loo, nEta, nPhi, phiFirst, phiLast, etaScale, etaAxis, etaLoo);
        projectPhi(r2d_loo, nEta, nPhi, phiScale, phiAxis, phiLoo);

        for (int i = 0; i < nEta; ++i) for (int j = 0; j < nEta; ++j)
            C_PP_eta(i,j) += (etaLoo[i] - etaPP[i]) * (etaLoo[j] - etaPP[j]);
        for (int i = 0; i < nPhi; ++i) for (int j = 0; j < nPhi; ++j)
            C_PP_phi(i,j) += (phiLoo[i] - phiPP[i]) * (phiLoo[j] - phiPP[j]);
        ++validPP;
    }
    if (validPP > 1) { C_PP_eta *= (double)(validPP-1)/validPP; C_PP_phi *= (double)(validPP-1)/validPP; }
    cout << "pp JK: " << validPP << " valid replicas" << endl;

    // ====== Fill output histograms ======
    TH1D* hEtaHI = (TH1D*)etaAxis->Clone("hEtaHI"); hEtaHI->SetDirectory(0);
    TH1D* hPhiHI = (TH1D*)phiAxis->Clone("hPhiHI"); hPhiHI->SetDirectory(0);
    TH1D* hEtaPP = (TH1D*)etaAxis->Clone("hEtaPP"); hEtaPP->SetDirectory(0);
    TH1D* hPhiPP = (TH1D*)phiAxis->Clone("hPhiPP"); hPhiPP->SetDirectory(0);

    for (int i = 0; i < nEta; ++i) {
        hEtaHI->SetBinContent(i+1, etaHI[i]);
        hEtaHI->SetBinError(i+1, sqrt(max(0.0, C_HI_eta(i,i))));
        hEtaPP->SetBinContent(i+1, etaPP[i]);
        hEtaPP->SetBinError(i+1, sqrt(max(0.0, C_PP_eta(i,i))));
    }
    for (int i = 0; i < nPhi; ++i) {
        hPhiHI->SetBinContent(i+1, phiHI[i]);
        hPhiHI->SetBinError(i+1, sqrt(max(0.0, C_HI_phi(i,i))));
        hPhiPP->SetBinContent(i+1, phiPP[i]);
        hPhiPP->SetBinError(i+1, sqrt(max(0.0, C_PP_phi(i,i))));
    }

    // ====== Save output ROOT file ======
    string rootOutPath = outDir + "/" + tag + "-output.root";
    TFile* fout = TFile::Open(rootOutPath.c_str(), "RECREATE");
    hEtaHI->Write("hEtaCombined"); hPhiHI->Write("hPhiCombined");
    hEtaPP->Write("hEtaPP"); hPhiPP->Write("hPhiPP");
    C_HI_eta.Write("CovEtaCombined"); C_HI_phi.Write("CovPhiCombined");
    C_PP_eta.Write("CovEtaPP"); C_PP_phi.Write("CovPhiPP");
    fout->Close();
    cout << "Output ROOT: " << rootOutPath << endl;

    // ====== Plots ======
    double etaRange = sePPb->GetXaxis()->GetXmax();
    double phiMin = -M_PI/2.0, phiMax = 3.0*M_PI/2.0;
    auto path = [&](const string& suf) { return outDir+"/"+tag+"-"+suf+".pdf"; };
    string combLabel = "pPb (combined, sym.)";
    string ppLabel   = "pp (8.16 TeV, sym.)";

    makePlot(hEtaHI, hEtaPP, combLabel, ppLabel,
             "#Delta y_{ch,Z}", -etaRange, etaRange,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}",
             path("Result-DeltaEta"));
    makePlot(hPhiHI, hPhiPP, combLabel, ppLabel,
             "#Delta#phi_{ch,Z}", phiMin, phiMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}",
             path("Result-DeltaPhi"));

    // ====== P-value table ======
    // After symmetrization, only half the eta bins are independent (i and 13-i are equal).
    // DeltaEta unique bins: 1..6 (equal to 12..7); DeltaPhi unique: near-side 1..3, away-side 7..9.
    double chi2_eta_diag=0, chi2_phi_diag=0;
    int ndf_eta=0, ndf_phi=0;
    for (int i=1; i<=nEta/2; ++i) {
        double e2 = pow(hEtaHI->GetBinError(i),2)+pow(hEtaPP->GetBinError(i),2);
        if (e2<=0) continue;
        chi2_eta_diag += pow(hEtaHI->GetBinContent(i)-hEtaPP->GetBinContent(i),2)/e2;
        ++ndf_eta;
    }
    for (int i=1; i<=nPhi/4; i++) {
        double e2 = pow(hPhiHI->GetBinError(i),2)+pow(hPhiPP->GetBinError(i),2);
        if (e2<=0) continue;
        chi2_phi_diag += pow(hPhiHI->GetBinContent(i)-hPhiPP->GetBinContent(i),2)/e2;
        ++ndf_phi;
    }
    for (int i=nPhi/2+1; i<=3*nPhi/4; i++) {
        double e2 = pow(hPhiHI->GetBinError(i),2)+pow(hPhiPP->GetBinError(i),2);
        if (e2<=0) continue;
        chi2_phi_diag += pow(hPhiHI->GetBinContent(i)-hPhiPP->GetBinContent(i),2)/e2;
        ++ndf_phi;
    }
    double pval_eta_diag = (ndf_eta>0) ? TMath::Prob(chi2_eta_diag, ndf_eta) : -1;
    double pval_phi_diag = (ndf_phi>0) ? TMath::Prob(chi2_phi_diag, ndf_phi) : -1;

    cout << "Full covariance DeltaEta (pooled, sym.):" << endl;
    auto [c2eta, rketa] = fullCovChiSq(hEtaHI, hEtaPP, C_HI_eta, C_PP_eta);
    double pval_eta_cov = (rketa>0) ? TMath::Prob(c2eta, rketa) : -1;

    cout << "Full covariance DeltaPhi (pooled, sym.):" << endl;
    auto [c2phi, rkphi] = fullCovChiSq(hPhiHI, hPhiPP, C_HI_phi, C_PP_phi);
    double pval_phi_cov = (rkphi>0) ? TMath::Prob(c2phi, rkphi) : -1;

    cout << "\n--- Chi-square summary (pooled+symmetrized) ---\n";
    cout << Form("DeltaEta diagonal(JK): chi2=%.3g  ndf=%d  p=%.4g\n", chi2_eta_diag, ndf_eta, pval_eta_diag);
    cout << Form("DeltaPhi diagonal(JK): chi2=%.3g  ndf=%d  p=%.4g\n", chi2_phi_diag, ndf_phi, pval_phi_diag);
    cout << Form("DeltaEta fullCov:       chi2=%.3g  ndf=%d  p=%.4g\n", c2eta, rketa, pval_eta_cov);
    cout << Form("DeltaPhi fullCov:       chi2=%.3g  ndf=%d  p=%.4g\n", c2phi, rkphi, pval_phi_cov);

    string texPath = outDir + "/" + tag + "-pvalue.tex";
    ofstream tex(texPath);
    tex << "\\begin{tabular}{|l|r|r|r|r|}\n\\hline\n";
    tex << "Observable & $\\chi^2$ & ndf & $\\chi^2/\\mathrm{ndf}$ & $p$-value \\\\\n\\hline\n";
    auto fmtRow = [&](const string& obs, double c2, int ndf, double pv) {
        tex << Form("%s & %.3g & %d & %.3g & %.4g \\\\\n", obs.c_str(), c2, ndf, ndf>0?c2/ndf:-1.0, pv);
    };
    fmtRow("$\\Delta y_{ch,Z}$ pooled sym.\\ diagonal (JK)", chi2_eta_diag, ndf_eta, pval_eta_diag);
    fmtRow("$\\Delta\\phi_{ch,Z}$ pooled sym.\\ diagonal (JK)", chi2_phi_diag, ndf_phi, pval_phi_diag);
    fmtRow("$\\Delta y_{ch,Z}$ pooled sym.\\ full covariance", c2eta, rketa, pval_eta_cov);
    fmtRow("$\\Delta\\phi_{ch,Z}$ pooled sym.\\ full covariance", c2phi, rkphi, pval_phi_cov);
    tex << "\\hline\n\\end{tabular}\n";
    tex.close();
    cout << "P-value table: " << texPath << endl;

    fpPb->Close(); fPbP->Close(); fpp->Close();
    fPPbRaw->Close(); fPbPRaw->Close(); fppRaw->Close();
    cout << "=== Done: " << outDir << "/" << tag << "-*.pdf ===" << endl;
    return 0;
}
