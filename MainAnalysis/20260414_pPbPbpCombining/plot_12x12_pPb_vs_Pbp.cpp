// Study 19: pPb vs Pbp closure under matched CM acceptance.
// Loads the CMA (CM-frame) nosub files produced by runner-12x12-cmframe.sh.
// Both pPb and Pbp have |eta_cm| < 1.935 acceptance (FillSigned=true, FlipDeltaEta=false).
// For DeltaEta comparison: flips Pbp's signed histogram so both are in "proton-forward" orientation.
// For DeltaPhi comparison: no flip (azimuthally symmetric).

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

// Flip a signed histogram: bin i ↔ bin N+1-i (negates the observable axis).
TH1D* flipHist(TH1D* h, const char* name) {
    int n = h->GetNbinsX();
    TH1D* out = (TH1D*)h->Clone(name);
    out->SetDirectory(0);
    for (int i = 1; i <= n; i++) {
        out->SetBinContent(i, h->GetBinContent(n+1-i));
        out->SetBinError(i,   h->GetBinError(n+1-i));
    }
    return out;
}

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
void makePlot2(TH1D* h1, TH1D* h2,
               const string& lab1, const string& lab2,
               const char* xTitle, double xMin, double xMax,
               const char* yTitle, bool isRatio,
               const string& outPath) {
    SetTDRStyle();
    string uid = Form("pv%d", gUID++);
    vector<TH1*> hv = {h1, h2};
    vector<string> labels = {lab1, lab2};
    vector<int> cols = {kRed+1, cmsBlue};
    vector<int> ls = {0,0}, ms = {mCircleFill, mCircleFill};

    double lo=1e30, hi=-1e30;
    for (auto h : hv) {
        int b1=max(1,h->FindBin(xMin+1e-9)), b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
        for (int b=b1; b<=b2; b++) { double v=h->GetBinContent(b),e=h->GetBinError(b); lo=min(lo,v-e); hi=max(hi,v+e); }
    }
    double rng=hi-lo, p=0.15/(1.0-2*0.15);
    double tlo=lo-p*rng, thi=hi+p*rng;

    TPad* pad=nullptr;
    TCanvas* c=new TCanvas(uid.c_str(),uid.c_str(),600,600);
    if (isRatio) {
        double rDev=0.02;
        for (int i=1;i<=h1->GetNbinsX();i++) {
            double num=h1->GetBinContent(i),e1=h1->GetBinError(i),den=h2->GetBinContent(i),e2=h2->GetBinError(i);
            if (!den) continue; double r=num/den, re=fabs(r)*hypot(num?e1/num:0,e2/den);
            rDev=max(rDev, max(fabs(r-1.0)+re, re));
        }
        double mg=rDev*0.15/(1.0-2*0.15);
        double rlo=1.0-rDev-mg, rhi=1.0+rDev+mg;
        pad=(TPad*)plotCMSRatio(hv,(uid+"r").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Ratio pPb/Pbp",rlo,rhi,0,false,false,true,0.55);
    } else {
        double dmax=1e-6;
        for (int i=1;i<=h1->GetNbinsX();i++) { double d=h1->GetBinContent(i)-h2->GetBinContent(i); dmax=max(dmax,fabs(d)+hypot(h1->GetBinError(i),h2->GetBinError(i))); }
        dmax/=0.85;
        pad=(TPad*)plotCMSDiff(hv,(uid+"d").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Diff pPb#minusPbp",-dmax,dmax,0,false,false,true,0.12);
    }
    if (pad) { AddCMSHeader(pad,"Internal",false); AddUPCHeader(pad,"8.16 TeV","PPb"); }
    c->Update(); c->SaveAs(outPath.c_str()); delete c;
}

int main(int argc, char* argv[]) {
    CommandLine CL(argc, argv);
    string pPbFile = CL.Get("pPbFile");
    string PbPFile = CL.Get("PbPFile");
    string outDir  = CL.Get("OutputDir", "plots/12x12_pPb_vs_Pbp");
    string tag     = CL.Get("Tag", "pPbvsPbp");
    string key     = CL.Get("TrkPtKey", "0.5_15");
    mkdir(outDir.c_str(), 0755);

    auto openFile = [](const string& path) {
        TFile* f = TFile::Open(path.c_str(), "READ");
        if (!f||f->IsZombie()) { cerr<<"Cannot open: "<<path<<endl; exit(1); }
        return f;
    };
    TFile* fpPb = openFile(pPbFile);
    TFile* fPbP = openFile(PbPFile);

    auto getH2 = [&](TFile* f, const string& nm) {
        TH2D* h = (TH2D*)f->Get(nm.c_str());
        if (!h) { cerr<<"Missing "<<nm<<" in "<<f->GetName()<<endl; exit(1); }
        return h;
    };
    auto getH1 = [&](TFile* f, const string& nm) {
        TH1D* h = (TH1D*)f->Get(nm.c_str());
        if (!h) { cerr<<"Missing "<<nm<<" in "<<f->GetName()<<endl; exit(1); }
        return h;
    };

    TH2D *sePPb = getH2(fpPb, "hData_"+key);
    TH2D *mePPb = getH2(fpPb, "hMixData_"+key);
    TH1D *nzPPb = getH1(fpPb, "hNZData_"+key);
    TH2D *sePbP = getH2(fPbP, "hData_"+key);
    TH2D *mePbP = getH2(fPbP, "hMixData_"+key);
    TH1D *nzPbP = getH1(fPbP, "hNZData_"+key);

    double NZ_pPb = nzPPb->GetBinContent(1);
    double NZ_PbP = nzPbP->GetBinContent(1);
    cout << "N_Z: pPb=" << NZ_pPb << "  Pbp=" << NZ_PbP << endl;

    TH2D* rePPb = (TH2D*)sePPb->Clone("rePPb"); rePPb->Add(mePPb,-1.0);
    TH2D* rePbP = (TH2D*)sePbP->Clone("rePbP"); rePbP->Add(mePbP,-1.0);

    // Project: DeltaEta over phi bins 4-6 ([0,pi/2]), scale 0.5.
    // DeltaPhi: full DeltaEta range (bins 1-12), scale 0.5.
    TH1D* reEtaPPb = proj1X(rePPb, "reEtaPPb", 4, 6,  0.5);
    TH1D* rePhiPPb = proj1Y(rePPb, "rePhiPPb", 1, 12, 0.5);
    TH1D* reEtaPbP = proj1X(rePbP, "reEtaPbP", 4, 6,  0.5);
    TH1D* rePhiPbP = proj1Y(rePbP, "rePhiPbP", 1, 12, 0.5);

    // Flip Pbp DeltaEta to align proton-beam orientation
    TH1D* reEtaPbPflip = flipHist(reEtaPbP, "reEtaPbPflip");

    auto path = [&](const string& suf) { return outDir+"/"+tag+"-"+suf+".pdf"; };
    const double etaMin=-4.0, etaMax=4.0;
    const double phiMin=-M_PI/2.0, phiMax=3.0*M_PI/2.0;
    const string pPbLabel = "pPb (|#eta_{cm}|<1.935)";
    const string PbPLabel = "Pbp flipped (|#eta_{cm}|<1.935)";
    const string PbPLabelRaw = "Pbp (|#eta_{cm}|<1.935)";

    // Signed DeltaEta: pPb vs Pbp (unflipped) — should look like mirror images
    makePlot2(reEtaPPb, reEtaPbP, pPbLabel, PbPLabelRaw,
              "#Delta y_{ch,Z}", etaMin, etaMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", false,
              path("DeltaEta-signed-raw"));

    // Signed DeltaEta: pPb vs Pbp flipped — key closure test
    makePlot2(reEtaPPb, reEtaPbPflip, pPbLabel, PbPLabel,
              "#Delta y_{ch,Z}", etaMin, etaMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", false,
              path("DeltaEta-signed-flipped"));

    // DeltaPhi: full DeltaEta range (bins 1-12)
    makePlot2(rePhiPPb, rePhiPbP, pPbLabel, PbPLabelRaw,
              "#Delta#phi_{ch,Z}", phiMin, phiMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", false,
              path("DeltaPhi-full"));

    // Chi-square table
    auto [chi2_etaRaw,  ndf_etaRaw]  = chiSq(reEtaPPb, reEtaPbP);
    auto [chi2_etaFlip, ndf_etaFlip] = chiSq(reEtaPPb, reEtaPbPflip);
    auto [chi2_phi,     ndf_phi]     = chiSq(rePhiPPb, rePhiPbP);

    struct Row { string obs; double chi2; int ndf; };
    vector<Row> rows = {
        {"pPb vs Pbp DeltaEta (no flip)",  chi2_etaRaw,  ndf_etaRaw},
        {"pPb vs Pbp flipped DeltaEta",    chi2_etaFlip, ndf_etaFlip},
        {"pPb vs Pbp DeltaPhi (full eta)", chi2_phi,     ndf_phi},
    };

    cout << "\n--- Chi-square summary (pPb vs Pbp, signed 12-bin) ---\n";
    cout << Form("%-42s  %8s  %4s  %10s  %8s\n","Observable","chi2","ndf","chi2/ndf","p-value");
    for (auto& r : rows) {
        double pval = (r.ndf>0) ? TMath::Prob(r.chi2,r.ndf) : -1;
        double ratio = (r.ndf>0) ? r.chi2/r.ndf : -1;
        cout << Form("%-42s  %8.4g  %4d  %10.4g  %8.4g\n",r.obs.c_str(),r.chi2,r.ndf,ratio,pval);
    }

    string texPath = outDir+"/"+tag+"-pvalue.tex";
    ofstream tex(texPath);
    tex << "\\begin{tabular}{|l|r|r|r|r|}\n\\hline\n";
    tex << "Observable & $\\chi^2$ & ndf & $\\chi^2/\\mathrm{ndf}$ & $p$-value \\\\\n\\hline\n";
    for (auto& r : rows) {
        double pval = (r.ndf>0) ? TMath::Prob(r.chi2,r.ndf) : -1;
        double ratio = (r.ndf>0) ? r.chi2/r.ndf : -1;
        tex << Form("%s & %.3g & %d & %.3g & %.3g \\\\\n",r.obs.c_str(),r.chi2,r.ndf,ratio,pval);
    }
    tex << "\\hline\n\\end{tabular}\n";
    tex.close();
    cout << "P-value table: " << texPath << "\n";

    fpPb->Close(); fPbP->Close();
    cout << "=== Done: " << outDir << "/" << tag << "-*.pdf ===\n";
    return 0;
}
