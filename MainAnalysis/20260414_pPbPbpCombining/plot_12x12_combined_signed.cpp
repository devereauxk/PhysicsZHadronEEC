// Studies 17 and 18: signed combined pPb+Pbp vs pp comparison.
// Jackknife uncertainties and full covariance chi-square for p-value.

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
        double vc = (va*nza + vb*nzb) / nztot;
        double ec = sqrt(pow(ea*nza/nztot,2) + pow(eb*nzb/nztot,2));
        out->SetBinContent(i,j,vc);
        out->SetBinError(i,j,ec);
    }
    return out;
}

// ---- Jackknife: per-event TTree contribution ----
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

// Build jackknife covariance matrix for a single system (leave-one-out over Z events).
// Result units: divided by binwidth (matches result-histogram units).
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

// Override error bars of a 1D histogram with jackknife diagonal.
void applyJKErrors(TH1D* h, const TMatrixDSym& C) {
    int n = min(h->GetNbinsX(), C.GetNrows());
    for (int i=0; i<n; ++i) h->SetBinError(i+1, sqrt(max(0.0, C(i,i))));
}

// Full covariance chi-square via SVD pseudoinverse. C_tot = C_HI + C_pp.
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
              const char* yTitle, bool isRatio,
              const string& outPath) {
    SetTDRStyle();
    string uid = Form("cs%d", gUID++);
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

    TPad* pad = nullptr;
    TCanvas* c = new TCanvas(uid.c_str(), uid.c_str(), 600, 600);
    if (isRatio) {
        // Symmetric ratio range about 1.0
        double rDev = 0.02;
        for (int i=1; i<=hComb->GetNbinsX(); i++) {
            double num=hComb->GetBinContent(i), e1=hComb->GetBinError(i);
            double den=hPP->GetBinContent(i),   e2=hPP->GetBinError(i);
            if (!den) continue;
            double r=num/den, re=fabs(r)*hypot(num?e1/num:0, e2/den);
            rDev = max(rDev, max(fabs(r-1.0)+re, re));
        }
        double mg = rDev*0.15/(1.0-2*0.15);
        double rlo=1.0-rDev-mg, rhi=1.0+rDev+mg;
        pad=(TPad*)plotCMSRatio(hv,(uid+"r").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Ratio comb./pp",rlo,rhi,0,false,false,true,0.55);
    } else {
        double dmax=1e-6;
        for (int i=1;i<=hComb->GetNbinsX();i++) { double d=hComb->GetBinContent(i)-hPP->GetBinContent(i); dmax=max(dmax,fabs(d)+hypot(hComb->GetBinError(i),hPP->GetBinError(i))); }
        dmax/=0.85;
        pad=(TPad*)plotCMSDiff(hv,(uid+"d").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Diff comb.#minuspp",-dmax,dmax,0,false,false,true,0.12);
    }
    if (pad) { AddCMSHeader(pad,"Internal",false); AddUPCHeader(pad,"8.16 TeV","PPb"); }
    c->Update(); c->SaveAs(outPath.c_str()); delete c;
}

int main(int argc, char* argv[]) {
    CommandLine CL(argc, argv);
    string pPbFile     = CL.Get("pPbFile");
    string PbPFile     = CL.Get("PbPFile");
    string ppFile      = CL.Get("ppFile");
    // Optional result files for jackknife errors and full-covariance p-value
    string pPbResFile  = CL.Get("pPbResultFile", "");
    string PbPResFile  = CL.Get("PbPResultFile", "");
    string ppResFile   = CL.Get("ppResultFile",  "");
    string outDir      = CL.Get("OutputDir", "plots/12x12_combined_signed");
    string tag         = CL.Get("Tag", "combined");
    string key         = CL.Get("TrkPtKey", "0.5_15");
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

    // ---- Combine pPb + Pbp (N_Z weighted) in signed 12x12 space ----
    TH2D* seCombo = combineNZ(sePPb, NZ_pPb, sePbP, NZ_PbP, "seCombo");
    TH2D* meCombo = combineNZ(mePPb, NZ_pPb, mePbP, NZ_PbP, "meCombo");

    // ---- Result = SE - ME ----
    TH2D* resCombo = (TH2D*)seCombo->Clone("resCombo"); resCombo->Add(meCombo, -1.0);
    TH2D* respp    = (TH2D*)sepp   ->Clone("respp");    respp   ->Add(mepp,    -1.0);

    // ---- 1D projections ----
    TH1D* seEtaComb = proj1X(seCombo,  "seEtaComb", 4, 6,  0.5);
    TH1D* sePhiComb = proj1Y(seCombo,  "sePhiComb", 1, 12, 0.5);
    TH1D* meEtaComb = proj1X(meCombo,  "meEtaComb", 4, 6,  0.5);
    TH1D* mePhiComb = proj1Y(meCombo,  "mePhiComb", 1, 12, 0.5);
    TH1D* reEtaComb = proj1X(resCombo, "reEtaComb", 4, 6,  0.5);
    TH1D* rePhiComb = proj1Y(resCombo, "rePhiComb", 1, 12, 0.5);

    TH1D* seEtapp = proj1X(sepp,  "seEtapp", 4, 6,  0.5);
    TH1D* sePhipp = proj1Y(sepp,  "sePhipp", 1, 12, 0.5);
    TH1D* meEtapp = proj1X(mepp,  "meEtapp", 4, 6,  0.5);
    TH1D* mePhipp = proj1Y(mepp,  "mePhipp", 1, 12, 0.5);
    TH1D* reEtapp = proj1X(respp, "reEtapp", 4, 6,  0.5);
    TH1D* rePhipp = proj1Y(respp, "rePhipp", 1, 12, 0.5);

    // ---- Jackknife: build covariance matrices and override error bars ----
    bool haveJK = (!pPbResFile.empty() && !PbPResFile.empty() && !ppResFile.empty());
    TMatrixDSym C_combEta(12), C_combPhi(12), C_ppEta(12), C_ppPhi(12);

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

            // Per-system covariance matrices
            TMatrixDSym C_pPbEta = buildCovMatrix(evsPPb, reEtaComb, true);
            TMatrixDSym C_PbPEta = buildCovMatrix(evsPbP, reEtaComb, true);
            TMatrixDSym C_pPbPhi = buildCovMatrix(evsPPb, rePhiComb, false);
            TMatrixDSym C_PbPPhi = buildCovMatrix(evsPbP, rePhiComb, false);
            C_ppEta = buildCovMatrix(evspp, reEtapp, true);
            C_ppPhi = buildCovMatrix(evspp, rePhipp, false);

            // Combined HI covariance (NZ-weighted sum of per-system matrices)
            double wPPb = NZ_pPb / NZ_tot, wPbP = NZ_PbP / NZ_tot;
            int nEta = reEtaComb->GetNbinsX(), nPhi = rePhiComb->GetNbinsX();
            C_combEta.ResizeTo(nEta, nEta);
            C_combPhi.ResizeTo(nPhi, nPhi);
            for (int i=0; i<nEta; ++i) for (int j=0; j<nEta; ++j)
                C_combEta(i,j) = wPPb*wPPb*C_pPbEta(i,j) + wPbP*wPbP*C_PbPEta(i,j);
            for (int i=0; i<nPhi; ++i) for (int j=0; j<nPhi; ++j)
                C_combPhi(i,j) = wPPb*wPPb*C_pPbPhi(i,j) + wPbP*wPbP*C_PbPPhi(i,j);

            // Override error bars on result histograms
            applyJKErrors(reEtaComb, C_combEta);
            applyJKErrors(rePhiComb, C_combPhi);
            applyJKErrors(reEtapp,   C_ppEta);
            applyJKErrors(rePhipp,   C_ppPhi);

            fPPbR->Close(); fPbPR->Close(); fppR->Close();
            cout << "Jackknife errors applied to result histograms." << endl;
        }
    }

    // ---- Plots ----
    auto path = [&](const string& suf) { return outDir+"/"+tag+"-"+suf+".pdf"; };
    string combLabel = "pPb+Pbp combined";
    string ppLabel   = "pp (8.16 TeV)";
    const double etaMin=-4.0, etaMax=4.0;
    const double phiMin=-M_PI/2.0, phiMax=3.0*M_PI/2.0;

    // Diagnostic: SE and ME ratio (same-event, mixed-event acceptance comparison)
    makePlot(seEtaComb, seEtapp, combLabel, ppLabel,
             "#Delta y_{ch,Z}", etaMin, etaMax,
             "Same-event d#LTN_{ch}#GT/d#Delta y_{ch,Z}", true, path("SameEvent-DeltaEta"));
    makePlot(sePhiComb, sePhipp, combLabel, ppLabel,
             "#Delta#phi_{ch,Z}", phiMin, phiMax,
             "Same-event d#LTN_{ch}#GT/d#Delta#phi_{ch,Z}", true, path("SameEvent-DeltaPhi"));
    makePlot(meEtaComb, meEtapp, combLabel, ppLabel,
             "#Delta y_{ch,Z}", etaMin, etaMax,
             "Mixed-event d#LTN_{ch}#GT/d#Delta y_{ch,Z}", true, path("MixedEvent-DeltaEta"));
    makePlot(mePhiComb, mePhipp, combLabel, ppLabel,
             "#Delta#phi_{ch,Z}", phiMin, phiMax,
             "Mixed-event d#LTN_{ch}#GT/d#Delta#phi_{ch,Z}", true, path("MixedEvent-DeltaPhi"));

    // Physics result: background-subtracted difference
    makePlot(reEtaComb, reEtapp, combLabel, ppLabel,
             "#Delta y_{ch,Z}", etaMin, etaMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", false, path("Result-DeltaEta"));
    makePlot(rePhiComb, rePhipp, combLabel, ppLabel,
             "#Delta#phi_{ch,Z}", phiMin, phiMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", false, path("Result-DeltaPhi"));

    // ---- P-value table ----
    double pval_eta_diag=-1, pval_phi_diag=-1;
    double pval_eta_cov=-1,  pval_phi_cov=-1;
    int ndf_eta_diag=0, ndf_phi_diag=0;
    double chi2_eta_diag=0, chi2_phi_diag=0;
    int rank_eta=0, rank_phi=0;
    double chi2_eta_cov=0, chi2_phi_cov=0;

    // Diagonal chi-square (using current errors, which are JK if haveJK)
    for (int i=1; i<=reEtaComb->GetNbinsX(); i++) {
        double e2 = pow(reEtaComb->GetBinError(i),2)+pow(reEtapp->GetBinError(i),2);
        if (e2<=0) continue;
        chi2_eta_diag += pow(reEtaComb->GetBinContent(i)-reEtapp->GetBinContent(i),2)/e2;
        ++ndf_eta_diag;
    }
    for (int i=1; i<=rePhiComb->GetNbinsX(); i++) {
        double e2 = pow(rePhiComb->GetBinError(i),2)+pow(rePhipp->GetBinError(i),2);
        if (e2<=0) continue;
        chi2_phi_diag += pow(rePhiComb->GetBinContent(i)-rePhipp->GetBinContent(i),2)/e2;
        ++ndf_phi_diag;
    }
    pval_eta_diag = (ndf_eta_diag>0) ? TMath::Prob(chi2_eta_diag, ndf_eta_diag) : -1;
    pval_phi_diag = (ndf_phi_diag>0) ? TMath::Prob(chi2_phi_diag, ndf_phi_diag) : -1;

    // Full covariance chi-square (if jackknife available)
    if (haveJK) {
        cout << "Full covariance DeltaEta:" << endl;
        auto [c2eta, rketa] = fullCovChiSq(reEtaComb, reEtapp, C_combEta, C_ppEta);
        chi2_eta_cov=c2eta; rank_eta=rketa;
        pval_eta_cov = (rketa>0) ? TMath::Prob(c2eta, rketa) : -1;

        cout << "Full covariance DeltaPhi:" << endl;
        auto [c2phi, rkphi] = fullCovChiSq(rePhiComb, rePhipp, C_combPhi, C_ppPhi);
        chi2_phi_cov=c2phi; rank_phi=rkphi;
        pval_phi_cov = (rkphi>0) ? TMath::Prob(c2phi, rkphi) : -1;
    }

    cout << "\n--- Chi-square summary ---\n";
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
    fmtRow("$\\Delta y_{ch,Z}$ diagonal (JK)", chi2_eta_diag, ndf_eta_diag, pval_eta_diag);
    fmtRow("$\\Delta\\phi_{ch,Z}$ diagonal (JK)", chi2_phi_diag, ndf_phi_diag, pval_phi_diag);
    if (haveJK) {
        fmtRow("$\\Delta y_{ch,Z}$ full covariance", chi2_eta_cov, rank_eta, pval_eta_cov);
        fmtRow("$\\Delta\\phi_{ch,Z}$ full covariance", chi2_phi_cov, rank_phi, pval_phi_cov);
    }
    tex << "\\hline\n\\end{tabular}\n";
    tex.close();
    cout << "P-value table: " << texPath << endl;

    fpPb->Close(); fPbP->Close(); fpp->Close();
    cout << "=== Done: " << outDir << "/" << tag << "-*.pdf ===" << endl;
    return 0;
}
