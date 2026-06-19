// Cross-check plotter for Option A (CM frame) vs Option B (pPb-oriented frame).
// Inputs: nosub.root files for pPb, Pbp, pp from each option.
// For each option:
//   1. Combine signed pPb + Pbp by N_Z weighting
//   2. Compute result = SE_combined - ME_combined
//   3. Project to 1D DeltaEta and DeltaPhi
//   4. Symmetrize the signed 12-bin projection to |DeltaEta|, |DeltaPhi|
//   5. Compare symmetrized combined HI with symmetrized pp
// Cross-check: Option A symmetrized vs Option B symmetrized (should be equivalent).

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMath.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <cmath>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"

// Project 2D → 1D, divide by bin width, apply scale
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

// Combine two per-N_Z-normalized TH2D by N_Z weighting
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

// Symmetrize a signed 1D histogram to |x| by averaging positive and negative bins.
// The signed histogram has 12 bins symmetric about 0.
// Bin i corresponds to negative side; bin 13-i to positive side (for 12 bins: center at bin 6.5).
// Result: 6 bins on positive side, averaged with their mirror.
TH1D* symmetrize1D(TH1D* h, const char* name) {
    int n = h->GetNbinsX();
    int half = n / 2;
    // Create output: use positive half of bin edges
    vector<double> edges;
    for (int i = half+1; i <= n+1; i++) edges.push_back(h->GetBinLowEdge(i));
    TH1D* out = new TH1D(name, name, half, edges.data());
    out->SetDirectory(0);
    for (int i = 1; i <= half; i++) {
        int iPos = half + i;      // positive-side bin
        int iNeg = half + 1 - i;  // mirror negative-side bin
        double vPos = h->GetBinContent(iPos), ePos = h->GetBinError(iPos);
        double vNeg = h->GetBinContent(iNeg), eNeg = h->GetBinError(iNeg);
        double avg = 0.5 * (vPos + vNeg);
        double err = 0.5 * hypot(ePos, eNeg);
        out->SetBinContent(i, avg);
        out->SetBinError(i, err);
    }
    return out;
}

// Diagonal chi-squared and p-value between two 1D histograms
pair<double,int> chiSq(TH1D* ha, TH1D* hb) {
    double chi2 = 0; int ndf = 0;
    for (int i = 1; i <= ha->GetNbinsX(); i++) {
        double ea = ha->GetBinError(i), eb = hb->GetBinError(i);
        double e2 = ea*ea + eb*eb;
        if (e2 <= 0) continue;
        double diff = ha->GetBinContent(i) - hb->GetBinContent(i);
        chi2 += diff*diff / e2; ndf++;
    }
    return {chi2, ndf};
}

static int gUID = 0;

// 2-curve comparison plot (top + diff panel)
void makePlot(TH1D* h1, TH1D* h2,
              const string& lab1, const string& lab2,
              const char* xTitle, double xMin, double xMax,
              const char* yTitle,
              const string& outPath,
              bool isRatio = false) {
    SetTDRStyle();
    string uid = Form("fx%d", gUID++);
    vector<TH1*> hv = {h1, h2};
    vector<string> labels = {lab1, lab2};
    vector<int> cols = {cmsBlue, kRed+1};
    vector<int> ls = {0, 0}, ms = {mCircleFill, mCircleFill};

    double lo = 1e30, hi = -1e30;
    for (auto h : hv) {
        int b1 = max(1, h->FindBin(xMin+1e-9)), b2 = min(h->GetNbinsX(), h->FindBin(xMax-1e-9));
        for (int b = b1; b <= b2; b++) { double v=h->GetBinContent(b),e=h->GetBinError(b); lo=min(lo,v-e); hi=max(hi,v+e); }
    }
    double rng = hi - lo, p = 0.15/(1.0-2*0.15);
    double tlo = lo - p*rng, thi = hi + p*rng;

    TPad* pad = nullptr;
    TCanvas* c = new TCanvas(uid.c_str(), uid.c_str(), 600, 600);
    if (isRatio) {
        double rlo = 0.98, rhi = 1.02;
        for (int i = 1; i <= h1->GetNbinsX(); i++) {
            double n=h1->GetBinContent(i),e1=h1->GetBinError(i),d=h2->GetBinContent(i),e2=h2->GetBinError(i);
            if (!d) continue; double r=n/d, re=fabs(r)*hypot(n?e1/n:0,e2/d);
            rlo=min(rlo,r-re); rhi=max(rhi,r+re);
        }
        double mg=(rhi-rlo)*0.15/(1.0-2*0.15);
        pad = (TPad*)plotCMSRatio(hv,(uid+"r").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Ratio",rlo-mg,rhi+mg,0,false,false,true,0.55);
    } else {
        double dmax=1e-6;
        for (int i=1;i<=h1->GetNbinsX();i++) { double d=h1->GetBinContent(i)-h2->GetBinContent(i); dmax=max(dmax,fabs(d)+hypot(h1->GetBinError(i),h2->GetBinError(i))); }
        dmax/=0.85;
        pad = (TPad*)plotCMSDiff(hv,(uid+"d").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Diff 1#minus2",-dmax,dmax,0,false,false,true,0.12);
    }
    if (pad) { AddCMSHeader(pad,"Internal",false); AddUPCHeader(pad,"8.16 TeV","PPb"); }
    c->Update(); c->SaveAs(outPath.c_str()); delete c;
}

// Load nosub histograms for one system from one option
struct SysData {
    TH2D *se, *me;
    double nz;
};

SysData loadSys(TFile* f, const string& key) {
    SysData d;
    d.se = (TH2D*)f->Get(Form("hData_%s",    key.c_str()));
    d.me = (TH2D*)f->Get(Form("hMixData_%s", key.c_str()));
    TH1D* hn = (TH1D*)f->Get(Form("hNZData_%s", key.c_str()));
    if (!d.se||!d.me||!hn) { cerr<<"Missing histograms for key="<<key<<" in "<<f->GetName()<<endl; exit(1); }
    d.nz = hn->GetBinContent(1);
    return d;
}

// Build result 1D projections (DeltaEta and DeltaPhi) from one option's nosub files
// Returns: {seEta, mEta, reEta, sePhi, mePhi, rePhi} — all heap-allocated
struct Option1D {
    TH1D *seEtaHI, *meEtaHI, *reEtaHI;
    TH1D *sePhiHI, *mePhiHI, *rePhiHI;
    TH1D *seEtaPP, *meEtaPP, *reEtaPP;
    TH1D *sePhiPP, *mePhiPP, *rePhiPP;
    // symmetrized versions
    TH1D *symReEtaHI, *symRePhiHI, *symReEtaPP, *symRePhiPP;
    double NZ_pPb, NZ_PbP;
};

Option1D buildOption(TFile* fpPb, TFile* fPbP, TFile* fpp,
                     const string& key, const string& pfx,
                     int etaPhiMin=4, int etaPhiMax=6,
                     int etaRangeMin=1, int etaRangeMax=12) {
    auto pPb = loadSys(fpPb, key);
    auto PbP = loadSys(fPbP, key);
    auto pp  = loadSys(fpp,  key);

    TH2D* seComb = combineNZ(pPb.se, pPb.nz, PbP.se, PbP.nz, (pfx+"seComb").c_str());
    TH2D* meComb = combineNZ(pPb.me, pPb.nz, PbP.me, PbP.nz, (pfx+"meComb").c_str());
    TH2D* reComb = (TH2D*)seComb->Clone((pfx+"reComb").c_str()); reComb->Add(meComb,-1.0);
    TH2D* repp   = (TH2D*)pp.se->Clone((pfx+"repp").c_str());   repp->Add(pp.me,   -1.0);

    // DeltaEta: project phi bins etaPhiMin..etaPhiMax ([0,pi/2]), scale 0.5
    // DeltaPhi: project full eta range (bins 1-12), scale 0.5
    Option1D o;
    o.NZ_pPb = pPb.nz; o.NZ_PbP = PbP.nz;
    o.seEtaHI = proj1X(seComb, (pfx+"seEtaHI").c_str(), etaPhiMin, etaPhiMax, 0.5);
    o.meEtaHI = proj1X(meComb, (pfx+"meEtaHI").c_str(), etaPhiMin, etaPhiMax, 0.5);
    o.reEtaHI = proj1X(reComb, (pfx+"reEtaHI").c_str(), etaPhiMin, etaPhiMax, 0.5);
    o.sePhiHI = proj1Y(seComb, (pfx+"sePhiHI").c_str(), etaRangeMin, etaRangeMax, 0.5);
    o.mePhiHI = proj1Y(meComb, (pfx+"mePhiHI").c_str(), etaRangeMin, etaRangeMax, 0.5);
    o.rePhiHI = proj1Y(reComb, (pfx+"rePhiHI").c_str(), etaRangeMin, etaRangeMax, 0.5);

    o.seEtaPP = proj1X(pp.se, (pfx+"seEtaPP").c_str(), etaPhiMin, etaPhiMax, 0.5);
    o.meEtaPP = proj1X(pp.me, (pfx+"meEtaPP").c_str(), etaPhiMin, etaPhiMax, 0.5);
    o.reEtaPP = proj1X(repp,  (pfx+"reEtaPP").c_str(), etaPhiMin, etaPhiMax, 0.5);
    o.sePhiPP = proj1Y(pp.se, (pfx+"sePhiPP").c_str(), etaRangeMin, etaRangeMax, 0.5);
    o.mePhiPP = proj1Y(pp.me, (pfx+"mePhiPP").c_str(), etaRangeMin, etaRangeMax, 0.5);
    o.rePhiPP = proj1Y(repp,  (pfx+"rePhiPP").c_str(), etaRangeMin, etaRangeMax, 0.5);

    o.symReEtaHI = symmetrize1D(o.reEtaHI, (pfx+"symReEtaHI").c_str());
    o.symReEtaPP = symmetrize1D(o.reEtaPP, (pfx+"symReEtaPP").c_str());
    o.symRePhiHI = symmetrize1D(o.rePhiHI, (pfx+"symRePhiHI").c_str());
    o.symRePhiPP = symmetrize1D(o.rePhiPP, (pfx+"symRePhiPP").c_str());

    return o;
}

int main(int argc, char* argv[]) {
    CommandLine CL(argc, argv);
    string pPbFileA = CL.Get("pPbFileA");
    string PbPFileA = CL.Get("PbPFileA");
    string ppFileA  = CL.Get("ppFileA");
    string pPbFileB = CL.Get("pPbFileB");
    string PbPFileB = CL.Get("PbPFileB");
    string ppFileB  = CL.Get("ppFileB");
    string outDir   = CL.Get("OutputDir", "plots/12x12_frames_xcheck");
    string tag      = CL.Get("Tag", "xcheck");
    string key      = CL.Get("TrkPtKey", "0.5_15");
    mkdir(outDir.c_str(), 0755);

    // Open files
    auto openFile = [](const string& path) {
        TFile* f = TFile::Open(path.c_str(), "READ");
        if (!f||f->IsZombie()) { cerr<<"Cannot open: "<<path<<endl; exit(1); }
        return f;
    };
    TFile* fpPbA = openFile(pPbFileA);
    TFile* fPbPA = openFile(PbPFileA);
    TFile* fppA  = openFile(ppFileA);
    TFile* fpPbB = openFile(pPbFileB);
    TFile* fPbPB = openFile(PbPFileB);
    TFile* fppB  = openFile(ppFileB);

    Option1D A = buildOption(fpPbA, fPbPA, fppA, key, "A_");
    Option1D B = buildOption(fpPbB, fPbPB, fppB, key, "B_");

    cout << "Option A: N_Z pPb=" << A.NZ_pPb << "  Pbp=" << A.NZ_PbP << endl;
    cout << "Option B: N_Z pPb=" << B.NZ_pPb << "  Pbp=" << B.NZ_PbP << endl;

    auto path = [&](const string& suf) { return outDir+"/"+tag+"-"+suf+".pdf"; };

    // ---- Signed result plots: Option A HI vs pp ----
    const double etaMin = -4.0, etaMax = 4.0;
    const double phiMin = -M_PI/2.0, phiMax = 3.0*M_PI/2.0;
    makePlot(A.reEtaHI, A.reEtaPP, "pPb+Pbp (CM frame)", "pp (8.16 TeV)",
             "#Delta y_{ch,Z}", etaMin, etaMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", path("A-Result-DeltaEta-signed"));
    makePlot(A.rePhiHI, A.rePhiPP, "pPb+Pbp (CM frame)", "pp (8.16 TeV)",
             "#Delta#phi_{ch,Z}", phiMin, phiMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", path("A-Result-DeltaPhi-signed"));

    // ---- Signed result plots: Option B HI vs pp ----
    makePlot(B.reEtaHI, B.reEtaPP, "pPb+Pbp (pPb-oriented)", "pp (8.16 TeV)",
             "#Delta y_{ch,Z}", etaMin, etaMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", path("B-Result-DeltaEta-signed"));
    makePlot(B.rePhiHI, B.rePhiPP, "pPb+Pbp (pPb-oriented)", "pp (8.16 TeV)",
             "#Delta#phi_{ch,Z}", phiMin, phiMax,
             "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", path("B-Result-DeltaPhi-signed"));

    // ---- Symmetrized result: Option A HI vs pp ----
    const double setaMin = 0.0, setaMax = 4.0;
    const double sphiMin = 0.0, sphiMax = M_PI/2.0;
    makePlot(A.symReEtaHI, A.symReEtaPP, "pPb+Pbp (CM frame)", "pp (8.16 TeV)",
             "|#Delta y_{ch,Z}|", setaMin, setaMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta y_{ch,Z}|", path("A-Result-DeltaEta-sym"));
    makePlot(A.symRePhiHI, A.symRePhiPP, "pPb+Pbp (CM frame)", "pp (8.16 TeV)",
             "|#Delta#phi_{ch,Z}|", sphiMin, sphiMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta#phi_{ch,Z}|", path("A-Result-DeltaPhi-sym"));

    // ---- Symmetrized result: Option B HI vs pp ----
    makePlot(B.symReEtaHI, B.symReEtaPP, "pPb+Pbp (pPb-oriented)", "pp (8.16 TeV)",
             "|#Delta y_{ch,Z}|", setaMin, setaMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta y_{ch,Z}|", path("B-Result-DeltaEta-sym"));
    makePlot(B.symRePhiHI, B.symRePhiPP, "pPb+Pbp (pPb-oriented)", "pp (8.16 TeV)",
             "|#Delta#phi_{ch,Z}|", sphiMin, sphiMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta#phi_{ch,Z}|", path("B-Result-DeltaPhi-sym"));

    // ---- Cross-check: Option A symmetrized vs Option B symmetrized ----
    // Note: suffix must NOT include "xcheck-" — tag already prepends it via path().
    makePlot(A.symReEtaHI, B.symReEtaHI, "HI (CM frame)", "HI (pPb-oriented)",
             "|#Delta y_{ch,Z}|", setaMin, setaMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta y_{ch,Z}|", path("HI-DeltaEta"), true);
    makePlot(A.symRePhiHI, B.symRePhiHI, "HI (CM frame)", "HI (pPb-oriented)",
             "|#Delta#phi_{ch,Z}|", sphiMin, sphiMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta#phi_{ch,Z}|", path("HI-DeltaPhi"), true);
    makePlot(A.symReEtaPP, B.symReEtaPP, "pp (CM frame)", "pp (pPb-oriented)",
             "|#Delta y_{ch,Z}|", setaMin, setaMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta y_{ch,Z}|", path("pp-DeltaEta"), true);
    makePlot(A.symRePhiPP, B.symRePhiPP, "pp (CM frame)", "pp (pPb-oriented)",
             "|#Delta#phi_{ch,Z}|", sphiMin, sphiMax,
             "Result d#LT#DeltaN_{ch}#GT/d|#Delta#phi_{ch,Z}|", path("pp-DeltaPhi"), true);

    // ---- Chi-square table ----
    auto [chi2_AHIeta, ndf_AHIeta] = chiSq(A.symReEtaHI, A.symReEtaPP);
    auto [chi2_AHIphi, ndf_AHIphi] = chiSq(A.symRePhiHI, A.symRePhiPP);
    auto [chi2_BHIeta, ndf_BHIeta] = chiSq(B.symReEtaHI, B.symReEtaPP);
    auto [chi2_BHIphi, ndf_BHIphi] = chiSq(B.symRePhiHI, B.symRePhiPP);
    auto [chi2_xcHIeta, ndf_xcHIeta] = chiSq(A.symReEtaHI, B.symReEtaHI);
    auto [chi2_xcHIphi, ndf_xcHIphi] = chiSq(A.symRePhiHI, B.symRePhiHI);
    auto [chi2_xcPPeta, ndf_xcPPeta] = chiSq(A.symReEtaPP, B.symReEtaPP);
    auto [chi2_xcPPphi, ndf_xcPPphi] = chiSq(A.symRePhiPP, B.symRePhiPP);

    struct Row { string obs; double chi2; int ndf; };
    vector<Row> rows = {
        {"Option A: HI vs pp DeltaEta", chi2_AHIeta, ndf_AHIeta},
        {"Option A: HI vs pp DeltaPhi", chi2_AHIphi, ndf_AHIphi},
        {"Option B: HI vs pp DeltaEta", chi2_BHIeta, ndf_BHIeta},
        {"Option B: HI vs pp DeltaPhi", chi2_BHIphi, ndf_BHIphi},
        {"X-check: A vs B HI DeltaEta", chi2_xcHIeta, ndf_xcHIeta},
        {"X-check: A vs B HI DeltaPhi", chi2_xcHIphi, ndf_xcHIphi},
        {"X-check: A vs B pp DeltaEta", chi2_xcPPeta, ndf_xcPPeta},
        {"X-check: A vs B pp DeltaPhi", chi2_xcPPphi, ndf_xcPPphi},
    };

    cout << "\n--- Chi-square summary (symmetrized |DeltaEta|/|DeltaPhi|) ---\n";
    cout << Form("%-42s  %8s  %4s  %10s  %8s\n", "Observable","chi2","ndf","chi2/ndf","p-value");
    for (auto& r : rows) {
        double pval = (r.ndf > 0) ? TMath::Prob(r.chi2, r.ndf) : -1;
        double ratio = (r.ndf > 0) ? r.chi2/r.ndf : -1;
        cout << Form("%-42s  %8.4g  %4d  %10.4g  %8.4g\n", r.obs.c_str(), r.chi2, r.ndf, ratio, pval);
    }

    string texPath = outDir + "/" + tag + "-pvalue.tex";
    ofstream tex(texPath);
    tex << "\\begin{tabular}{|l|r|r|r|r|}\n\\hline\n";
    tex << "Observable & $\\chi^2$ & ndf & $\\chi^2/\\mathrm{ndf}$ & $p$-value \\\\\n\\hline\n";
    for (auto& r : rows) {
        double pval = (r.ndf > 0) ? TMath::Prob(r.chi2, r.ndf) : -1;
        double ratio = (r.ndf > 0) ? r.chi2/r.ndf : -1;
        tex << Form("%s & %.3g & %d & %.3g & %.3g \\\\\n",
                    r.obs.c_str(), r.chi2, r.ndf, ratio, pval);
    }
    tex << "\\hline\n\\end{tabular}\n";
    tex.close();
    cout << "P-value table: " << texPath << "\n";

    fpPbA->Close(); fPbPA->Close(); fppA->Close();
    fpPbB->Close(); fPbPB->Close(); fppB->Close();
    cout << "=== Done: " << outDir << "/" << tag << "-*.pdf ===\n";
    return 0;
}
