// Study 20: pp MC Gen boost-prescription closure.
// Three variants: nominal (|eta|<2.4), pPb-boosted (-1.935<eta<2.865), Pbp-boosted (-2.865<eta<1.935).
// Both boosted variants: boost +-0.465 first, then apply |eta_cm|<2.4 in lab-frame coordinates.
// Key closure test: pPb-boosted vs Pbp-boosted (flipped) should agree.

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
    double chi2=0; int ndf=0;
    for (int i=1; i<=ha->GetNbinsX(); i++) {
        double ea=ha->GetBinError(i), eb=hb->GetBinError(i);
        double e2=ea*ea+eb*eb;
        if (e2<=0) continue;
        double diff=ha->GetBinContent(i)-hb->GetBinContent(i);
        chi2+=diff*diff/e2; ndf++;
    }
    return {chi2,ndf};
}

static int gUID=0;
void makePlot2(TH1D* h1, TH1D* h2,
               const string& lab1, const string& lab2,
               const char* xTitle, double xMin, double xMax,
               const char* yTitle, bool isRatio,
               const string& outPath) {
    SetTDRStyle();
    string uid=Form("mb%d",gUID++);
    vector<TH1*> hv={h1,h2};
    vector<string> labels={lab1,lab2};
    vector<int> cols={kRed+1,cmsBlue};
    vector<int> ls={0,0}, ms={mCircleFill,mCircleFill};

    double lo=1e30,hi=-1e30;
    for (auto h:hv) {
        int b1=max(1,h->FindBin(xMin+1e-9)),b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
        for (int b=b1;b<=b2;b++) { double v=h->GetBinContent(b),e=h->GetBinError(b); lo=min(lo,v-e); hi=max(hi,v+e); }
    }
    double rng=hi-lo,p=0.15/(1.0-2*0.15);
    double tlo=lo-p*rng,thi=hi+p*rng;

    TPad* pad=nullptr;
    TCanvas* c=new TCanvas(uid.c_str(),uid.c_str(),600,600);
    if (isRatio) {
        double rlo=0.98,rhi=1.02;
        for (int i=1;i<=h1->GetNbinsX();i++) {
            double n=h1->GetBinContent(i),e1=h1->GetBinError(i),d=h2->GetBinContent(i),e2=h2->GetBinError(i);
            if (!d) continue; double r=n/d,re=fabs(r)*hypot(n?e1/n:0,e2/d);
            rlo=min(rlo,r-re); rhi=max(rhi,r+re);
        }
        double mg=(rhi-rlo)*0.15/(1.0-2*0.15);
        pad=(TPad*)plotCMSRatio(hv,(uid+"r").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Ratio",rlo-mg,rhi+mg,0,false,false,true,0.55);
    } else {
        double dmax=1e-6;
        for (int i=1;i<=h1->GetNbinsX();i++) { double d=h1->GetBinContent(i)-h2->GetBinContent(i); dmax=max(dmax,fabs(d)+hypot(h1->GetBinError(i),h2->GetBinError(i))); }
        dmax/=0.85;
        pad=(TPad*)plotCMSDiff(hv,(uid+"d").c_str(),labels,cols,ls,cols,ms,xTitle,xMin,xMax,yTitle,tlo,thi,"Difference",-dmax,dmax,0,false,false,true,0.12);
    }
    if (pad) { AddCMSHeader(pad,"Internal",false); AddUPCHeader(pad,"8.16 TeV","PP"); }
    c->Update(); c->SaveAs(outPath.c_str()); delete c;
}

void makePlot3(TH1D* h1, TH1D* h2, TH1D* h3,
               const string& lab1, const string& lab2, const string& lab3,
               const char* xTitle, double xMin, double xMax,
               const char* yTitle,
               const string& outPath) {
    SetTDRStyle();
    string uid=Form("mb3%d",gUID++);
    vector<TH1*> hv={h1,h2,h3};
    vector<string> labels={lab1,lab2,lab3};
    vector<int> cols={kRed+1, cmsBlue, kBlack};
    vector<int> ls={0,0,0}, ms={mCircleFill,mCircleFill,mCircleFill};

    double lo=1e30,hi=-1e30;
    for (auto h:hv) {
        int b1=max(1,h->FindBin(xMin+1e-9)),b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
        for (int b=b1;b<=b2;b++) { double v=h->GetBinContent(b),e=h->GetBinError(b); lo=min(lo,v-e); hi=max(hi,v+e); }
    }
    double rng=hi-lo,p=0.15/(1.0-2*0.15);
    double tlo=lo-p*rng,thi=hi+p*rng;
    if (rng==0) { tlo-=1e-6; thi+=1e-6; }

    TCanvas* c=new TCanvas(uid.c_str(),uid.c_str(),600,600);
    TH1* frame=(TH1*)h1->Clone((uid+"fr").c_str());
    frame->SetDirectory(0);
    frame->Reset("ICES");
    frame->GetXaxis()->SetTitle(xTitle);
    frame->GetXaxis()->SetRangeUser(xMin,xMax);
    frame->GetYaxis()->SetTitle(yTitle);
    frame->GetYaxis()->SetRangeUser(tlo,thi);
    frame->Draw("AXIS");

    for (int k=0;k<(int)hv.size();k++) {
        hv[k]->SetLineColor(cols[k]); hv[k]->SetMarkerColor(cols[k]);
        hv[k]->SetMarkerStyle(ms[k]); hv[k]->SetMarkerSize(0.7);
        hv[k]->Draw("E SAME");
    }
    TLegend* leg=new TLegend(0.18,0.63,0.94,0.88);
    leg->SetBorderSize(0); leg->SetFillStyle(0);
    leg->SetTextSize(0.032);
    for (int k=0;k<(int)hv.size();k++) leg->AddEntry(hv[k],labels[k].c_str(),"lp");
    leg->Draw();
    c->Update(); c->SaveAs(outPath.c_str()); delete c;
}

int main(int argc, char* argv[]) {
    CommandLine CL(argc, argv);
    string nomFile  = CL.Get("NomFile");
    string ppbFile  = CL.Get("PPbFile");
    string pbpFile  = CL.Get("PbPFile");
    string outDir   = CL.Get("OutputDir", "plots/12x12_ppmc_boost");
    string tag      = CL.Get("Tag", "boost");
    string key      = CL.Get("TrkPtKey", "0.5_15");
    mkdir(outDir.c_str(), 0755);

    auto openFile=[](const string& path) {
        TFile* f=TFile::Open(path.c_str(),"READ");
        if (!f||f->IsZombie()) { cerr<<"Cannot open: "<<path<<endl; exit(1); }
        return f;
    };
    TFile* fNom = openFile(nomFile);
    TFile* fPPb = openFile(ppbFile);
    TFile* fPbP = openFile(pbpFile);

    auto getH2=[&](TFile* f,const string& nm) {
        TH2D* h=(TH2D*)f->Get(nm.c_str());
        if (!h) { cerr<<"Missing "<<nm<<" in "<<f->GetName()<<endl; exit(1); }
        return h;
    };

    TH2D *seNom=getH2(fNom,"hData_"+key), *meNom=getH2(fNom,"hMixData_"+key);
    TH2D *sePPb=getH2(fPPb,"hData_"+key), *mePPb=getH2(fPPb,"hMixData_"+key);
    TH2D *sePbP=getH2(fPbP,"hData_"+key), *mePbP=getH2(fPbP,"hMixData_"+key);

    TH2D* reNom=(TH2D*)seNom->Clone("reNom"); reNom->Add(meNom,-1.0);
    TH2D* rePPb=(TH2D*)sePPb->Clone("rePPb"); rePPb->Add(mePPb,-1.0);
    TH2D* rePbP=(TH2D*)sePbP->Clone("rePbP"); rePbP->Add(mePbP,-1.0);

    TH1D* reEtaNom = proj1X(reNom, "reEtaNom", 4, 6,  0.5);
    TH1D* rePhiNom = proj1Y(reNom, "rePhiNom", 1, 12, 0.5);
    TH1D* reEtaPPb = proj1X(rePPb, "reEtaPPb", 4, 6,  0.5);
    TH1D* rePhiPPb = proj1Y(rePPb, "rePhiPPb", 1, 12, 0.5);
    TH1D* reEtaPbP = proj1X(rePbP, "reEtaPbP", 4, 6,  0.5);
    TH1D* rePhiPbP = proj1Y(rePbP, "rePhiPbP", 1, 12, 0.5);

    TH1D* reEtaPbPflip = flipHist(reEtaPbP, "reEtaPbPflip");

    auto path=[&](const string& suf) { return outDir+"/"+tag+"-"+suf+".pdf"; };
    const double etaMin=-4.0,etaMax=4.0;
    const double phiMin=-M_PI/2.0,phiMax=3.0*M_PI/2.0;

    const string labNom  = "pp MC Gen (|#eta|<2.4)";
    const string labPPb  = "pp MC Gen (pPb-boosted)";
    const string labPbP  = "pp MC Gen (Pbp-boosted)";
    const string labPbPf = "pp MC Gen (Pbp-boosted, flipped)";

    // 3-curve overlays
    makePlot3(reEtaNom, reEtaPPb, reEtaPbPflip,
              labNom, labPPb, labPbPf,
              "#Delta y_{ch,Z}", etaMin, etaMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}",
              path("DeltaEta-3curve"));
    makePlot3(rePhiNom, rePhiPPb, rePhiPbP,
              labNom, labPPb, labPbP,
              "#Delta#phi_{ch,Z}", phiMin, phiMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}",
              path("DeltaPhi-3curve"));

    // Key closure: pPb-boosted vs Pbp-boosted flipped
    makePlot2(reEtaPPb, reEtaPbPflip, labPPb, labPbPf,
              "#Delta y_{ch,Z}", etaMin, etaMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", false,
              path("DeltaEta-ppb-vs-pbp-diff"));
    makePlot2(rePhiPPb, rePhiPbP, labPPb, labPbP,
              "#Delta#phi_{ch,Z}", phiMin, phiMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", false,
              path("DeltaPhi-ppb-vs-pbp-diff"));

    // Nominal vs pPb-boosted
    makePlot2(reEtaNom, reEtaPPb, labNom, labPPb,
              "#Delta y_{ch,Z}", etaMin, etaMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", false,
              path("DeltaEta-nom-vs-ppb"));
    makePlot2(rePhiNom, rePhiPPb, labNom, labPPb,
              "#Delta#phi_{ch,Z}", phiMin, phiMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", false,
              path("DeltaPhi-nom-vs-ppb"));

    // Nominal vs Pbp-boosted flipped
    makePlot2(reEtaNom, reEtaPbPflip, labNom, labPbPf,
              "#Delta y_{ch,Z}", etaMin, etaMax,
              "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", false,
              path("DeltaEta-nom-vs-pbp-flipped"));

    // Chi-square table
    auto [c2_etaPPbvsPbpf, n2_etaPPbvsPbpf] = chiSq(reEtaPPb,    reEtaPbPflip);
    auto [c2_phiPPbvsPbp,  n2_phiPPbvsPbp]  = chiSq(rePhiPPb,    rePhiPbP);
    auto [c2_etaNomvsPPb,  n2_etaNomvsPPb]  = chiSq(reEtaNom,    reEtaPPb);
    auto [c2_phiNomvsPPb,  n2_phiNomvsPPb]  = chiSq(rePhiNom,    rePhiPPb);
    auto [c2_etaNomvsPbpf, n2_etaNomvsPbpf] = chiSq(reEtaNom,    reEtaPbPflip);
    auto [c2_phiNomvsPbp,  n2_phiNomvsPbp]  = chiSq(rePhiNom,    rePhiPbP);

    struct Row { string obs; double chi2; int ndf; };
    vector<Row> rows = {
        {"pPb-boosted vs Pbp-boosted flipped DeltaEta", c2_etaPPbvsPbpf, n2_etaPPbvsPbpf},
        {"pPb-boosted vs Pbp-boosted DeltaPhi",          c2_phiPPbvsPbp,  n2_phiPPbvsPbp},
        {"Nominal vs pPb-boosted DeltaEta",              c2_etaNomvsPPb,  n2_etaNomvsPPb},
        {"Nominal vs pPb-boosted DeltaPhi",              c2_phiNomvsPPb,  n2_phiNomvsPPb},
        {"Nominal vs Pbp-boosted flipped DeltaEta",      c2_etaNomvsPbpf, n2_etaNomvsPbpf},
        {"Nominal vs Pbp-boosted DeltaPhi",              c2_phiNomvsPbp,  n2_phiNomvsPbp},
    };

    cout << "\n--- Chi-square summary (pp MC Gen boost closure) ---\n";
    cout << Form("%-50s  %8s  %4s  %10s  %8s\n","Observable","chi2","ndf","chi2/ndf","p-value");
    for (auto& r : rows) {
        double pval=(r.ndf>0)?TMath::Prob(r.chi2,r.ndf):-1;
        double ratio=(r.ndf>0)?r.chi2/r.ndf:-1;
        cout << Form("%-50s  %8.4g  %4d  %10.4g  %8.4g\n",r.obs.c_str(),r.chi2,r.ndf,ratio,pval);
    }

    string texPath=outDir+"/"+tag+"-pvalue.tex";
    ofstream tex(texPath);
    tex << "\\begin{tabular}{|l|r|r|r|r|}\n\\hline\n";
    tex << "Observable & $\\chi^2$ & ndf & $\\chi^2/\\mathrm{ndf}$ & $p$-value \\\\\n\\hline\n";
    for (auto& r : rows) {
        double pval=(r.ndf>0)?TMath::Prob(r.chi2,r.ndf):-1;
        double ratio=(r.ndf>0)?r.chi2/r.ndf:-1;
        tex << Form("%s & %.3g & %d & %.3g & %.3g \\\\\n",r.obs.c_str(),r.chi2,r.ndf,ratio,pval);
    }
    tex << "\\hline\n\\end{tabular}\n";
    tex.close();
    cout << "P-value table: " << texPath << "\n";

    fNom->Close(); fPPb->Close(); fPbP->Close();
    cout << "=== Done: " << outDir << "/" << tag << "-*.pdf ===\n";
    return 0;
}
