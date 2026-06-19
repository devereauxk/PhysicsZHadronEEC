#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TLine.h>
#include <TLegend.h>
#include <iostream>
#include <string>
#include <cmath>
#include <sys/stat.h>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"

// ---------------------------------------------------------------------------
// Sideband mask: pi/2 - eps < |DeltaPhi| < pi/2 + eps  (no DeltaEta cut)
// ---------------------------------------------------------------------------
static bool inSideband(double /*dEta*/, double dPhi, double eps) {
    double absPhi = fabs(dPhi);
    return (absPhi > M_PI / 2 - eps && absPhi < M_PI / 2 + eps);
}

// Zero out bins NOT in the sideband.
TH2D *applyMask(TH2D *h2, const char *name, double eps) {
    TH2D *m = (TH2D*)h2->Clone(name);
    m->SetDirectory(0);
    for (int ix=1; ix<=m->GetNbinsX(); ix++) {
        double dEta = m->GetXaxis()->GetBinCenter(ix);
        for (int iy=1; iy<=m->GetNbinsY(); iy++) {
            double dPhi = m->GetYaxis()->GetBinCenter(iy);
            if (!inSideband(dEta, dPhi, eps)) {
                m->SetBinContent(ix, iy, 0);
                m->SetBinError(ix, iy, 0);
            }
        }
    }
    return m;
}

// Project masked 2D hist onto X axis (DeltaEta), dividing by bin width.
TH1D *projEta(TH2D *m, const char *name) {
    TH1D *h = (TH1D*)m->ProjectionX(name, 1, m->GetNbinsY());
    h->SetDirectory(0);
    for (int i=1; i<=h->GetNbinsX(); i++) {
        double w = h->GetBinWidth(i);
        if (w>0) { h->SetBinContent(i, h->GetBinContent(i)/w); h->SetBinError(i, h->GetBinError(i)/w); }
    }
    return h;
}

// Project onto Y axis (DeltaPhi), dividing by bin width.
TH1D *projPhi(TH2D *m, const char *name) {
    TH1D *h = (TH1D*)m->ProjectionY(name, 1, m->GetNbinsX());
    h->SetDirectory(0);
    for (int i=1; i<=h->GetNbinsX(); i++) {
        double w = h->GetBinWidth(i);
        if (w>0) { h->SetBinContent(i, h->GetBinContent(i)/w); h->SetBinError(i, h->GetBinError(i)/w); }
    }
    return h;
}

// ---------------------------------------------------------------------------
// Four-curve 1D comparison plot.
// A1/B1 = pPb/Pbp for eps1, A2/B2 = pPb/Pbp for eps2.
// Bottom panel: ratio A/B per epsilon (isRatio=true) or difference A-B (isRatio=false).
// showLegend: draw legend in top panel.
// ---------------------------------------------------------------------------
static int gUID2 = 0;
void save1DPlot(
    TH1D *hA1, TH1D *hB1, const string &labelA1, const string &labelB1,
    TH1D *hA2, TH1D *hB2, const string &labelA2, const string &labelB2,
    const string &xTitle, const string &yTitle,
    double xMin, double xMax,
    const string &path,
    bool isRatio = true,
    bool showLegend = true
) {
    SetTDRStyle();
    string uid = Form("sb%d", gUID2++);
    TCanvas *c = new TCanvas(uid.c_str(), uid.c_str(), 600, 600);
    const double border = 0.06;
    TPad *p1 = new TPad((uid+"a").c_str(),"",border,0.28+border,1-border,1-border);
    p1->SetBottomMargin(0); p1->Draw();
    TPad *p2 = new TPad((uid+"b").c_str(),"",border,border,1-border,0.28+border);
    p2->SetTopMargin(0); p2->SetBottomMargin(0.28); p2->Draw();

    const int colA = cmsBlue, colB = kSpring-6;
    const int lsE1 = 1, lsE2 = 7;  // solid for eps1, long-dash for eps2

    // Top panel y range
    double glo=1e30, ghi=-1e30;
    for (auto h : {hA1, hB1, hA2, hB2}) {
        int b1=max(1,h->FindBin(xMin+1e-9)), b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
        for (int b=b1;b<=b2;b++){double v=h->GetBinContent(b),e=h->GetBinError(b); glo=min(glo,v-e); ghi=max(ghi,v+e);}
    }
    double mg=(0.15/(1.0-2*0.15))*(ghi-glo);

    p1->cd();
    auto setup = [&](TH1D *h, int col, int ls) {
        h->SetLineColor(col); h->SetLineStyle(ls); h->SetLineWidth(2); h->SetMarkerSize(0);
        h->GetXaxis()->SetRangeUser(xMin,xMax); h->GetXaxis()->SetTitle(xTitle.c_str());
        h->GetYaxis()->SetTitle(yTitle.c_str()); h->GetYaxis()->SetTitleSize(0.05); h->GetYaxis()->SetTitleOffset(0.7);
        h->GetYaxis()->SetRangeUser(glo-mg, ghi+mg);
    };
    setup(hA1, colA, lsE1); hA1->Draw("HIST E");
    setup(hB1, colB, lsE1); hB1->Draw("HIST E SAME");
    setup(hA2, colA, lsE2); hA2->Draw("HIST E SAME");
    setup(hB2, colB, lsE2); hB2->Draw("HIST E SAME");

    if (showLegend) {
        TLegend *leg = new TLegend(0.40,0.60,0.88,0.84);
        leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextFont(42); leg->SetTextSize(0.032);
        leg->AddEntry(hA1, labelA1.c_str(),"l");
        leg->AddEntry(hB1, labelB1.c_str(),"l");
        leg->AddEntry(hA2, labelA2.c_str(),"l");
        leg->AddEntry(hB2, labelB2.c_str(),"l");
        leg->Draw("SAME");
    }
    AddCMSHeader(p1,"Internal",false); AddUPCHeader(p1,"8.16 TeV","PPb");

    p2->cd();
    if (isRatio) {
        TH1D *hr1 = (TH1D*)hA1->Clone((uid+"r1").c_str()); hr1->SetDirectory(0); hr1->Divide(hB1);
        TH1D *hr2 = (TH1D*)hA2->Clone((uid+"r2").c_str()); hr2->SetDirectory(0); hr2->Divide(hB2);
        auto setupR = [&](TH1D *hr, int col, int ls, bool first) {
            hr->SetLineColor(col); hr->SetLineStyle(ls); hr->SetLineWidth(2);
            hr->GetXaxis()->SetRangeUser(xMin,xMax); hr->GetXaxis()->SetTitle(xTitle.c_str());
            hr->GetXaxis()->SetTitleSize(0.11); hr->GetXaxis()->SetLabelSize(0.09); hr->GetXaxis()->SetTitleOffset(1.0);
            hr->GetYaxis()->SetTitle("Ratio pPb/Pbp"); hr->GetYaxis()->SetRangeUser(0.95, 1.05);
            hr->GetYaxis()->SetTitleSize(0.10); hr->GetYaxis()->SetLabelSize(0.08); hr->GetYaxis()->SetTitleOffset(0.45);
            hr->GetYaxis()->SetNdivisions(505);
            hr->Draw(first ? "HIST E" : "HIST E SAME");
        };
        setupR(hr1, colA, lsE1, true);
        setupR(hr2, colA, lsE2, false);
        TLine *l=new TLine(xMin,1.0,xMax,1.0); l->SetLineColor(kGray+2); l->SetLineStyle(2); l->Draw("SAME");
    } else {
        // Difference panel: auto-scaled
        TH1D *hd1 = (TH1D*)hA1->Clone((uid+"d1").c_str()); hd1->SetDirectory(0); hd1->Add(hB1,-1.0);
        TH1D *hd2 = (TH1D*)hA2->Clone((uid+"d2").c_str()); hd2->SetDirectory(0); hd2->Add(hB2,-1.0);
        for (int i=1; i<=hd1->GetNbinsX(); i++)
            hd1->SetBinError(i, hypot(hA1->GetBinError(i), hB1->GetBinError(i)));
        for (int i=1; i<=hd2->GetNbinsX(); i++)
            hd2->SetBinError(i, hypot(hA2->GetBinError(i), hB2->GetBinError(i)));
        double dlo=1e30, dhi=-1e30;
        for (auto h : {hd1, hd2}) {
            int b1=max(1,h->FindBin(xMin+1e-9)), b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
            for (int b=b1;b<=b2;b++){double v=h->GetBinContent(b),e=h->GetBinError(b); dlo=min(dlo,v-e); dhi=max(dhi,v+e);}
        }
        double dmg=(0.15/(1.0-2*0.15))*(dhi-dlo);
        auto setupD = [&](TH1D *hd, int col, int ls, bool first) {
            hd->SetLineColor(col); hd->SetLineStyle(ls); hd->SetLineWidth(2);
            hd->GetXaxis()->SetRangeUser(xMin,xMax); hd->GetXaxis()->SetTitle(xTitle.c_str());
            hd->GetXaxis()->SetTitleSize(0.11); hd->GetXaxis()->SetLabelSize(0.09); hd->GetXaxis()->SetTitleOffset(1.0);
            hd->GetYaxis()->SetTitle("Diff pPb#minusPbp"); hd->GetYaxis()->SetRangeUser(dlo-dmg, dhi+dmg);
            hd->GetYaxis()->SetTitleSize(0.10); hd->GetYaxis()->SetLabelSize(0.08); hd->GetYaxis()->SetTitleOffset(0.45);
            hd->GetYaxis()->SetNdivisions(505);
            hd->Draw(first ? "HIST E" : "HIST E SAME");
        };
        setupD(hd1, colA, lsE1, true);
        setupD(hd2, colA, lsE2, false);
        TLine *l=new TLine(xMin,0.0,xMax,0.0); l->SetLineColor(kGray+2); l->SetLineStyle(2); l->Draw("SAME");
    }

    c->Update(); c->SaveAs(path.c_str()); delete c;
}

// Draw a 2D heatmap with COLZ on a plain canvas.
void save2DPlot(TH2D *h2, const string &zTitle, const string &path) {
    SetTDRStyle();
    gStyle->SetPalette(kBird);
    string uid = Form("sb2d%d", gUID2++);
    TCanvas *c = new TCanvas(uid.c_str(), uid.c_str(), 700, 600);
    c->SetRightMargin(0.15);
    h2->GetXaxis()->SetTitle("#Delta#eta_{ch,Z}");
    h2->GetYaxis()->SetTitle("#Delta#phi_{ch,Z}");
    h2->GetZaxis()->SetTitle(zTitle.c_str());
    h2->SetStats(0);
    h2->GetXaxis()->SetRangeUser(0.0, 4.0);
    h2->GetYaxis()->SetRangeUser(0.0, M_PI);
    h2->Draw("COLZ");
    AddCMSHeader((TPad*)c->cd(),"Internal",false); AddUPCHeader((TPad*)c->cd(),"8.16 TeV","PPb");
    c->Update(); c->SaveAs(path.c_str()); delete c;
}

int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);
    string pPbFile = CL.Get("pPbFile");
    string PbPFile = CL.Get("PbPFile");
    string outDir  = CL.Get("OutputDir", "plots/12x12_sideband");
    string key     = CL.Get("TrkPtKey", "0.5_15");
    mkdir(outDir.c_str(), 0755);

    TFile *fA = TFile::Open(pPbFile.c_str(), "READ");
    TFile *fB = TFile::Open(PbPFile.c_str(), "READ");
    if (!fA || fA->IsZombie()) { cerr << "Cannot open: " << pPbFile << endl; return 1; }
    if (!fB || fB->IsZombie()) { cerr << "Cannot open: " << PbPFile << endl; return 1; }

    TH2D *seA = (TH2D*)fA->Get(Form("hData_%s",    key.c_str()));
    TH2D *meA = (TH2D*)fA->Get(Form("hMixData_%s", key.c_str()));
    TH2D *seB = (TH2D*)fB->Get(Form("hData_%s",    key.c_str()));
    TH2D *meB = (TH2D*)fB->Get(Form("hMixData_%s", key.c_str()));
    for (auto &[n,h] : vector<pair<string,TH2D*>>{{"hData_pPb",seA},{"hMixData_pPb",meA},{"hData_Pbp",seB},{"hMixData_Pbp",meB}}) {
        if (!h) { cerr << "Missing histogram: " << n << endl; return 1; }
        h->SetDirectory(0);
    }
    fA->Close(); fB->Close();

    // Sideband masks for two epsilon values
    const double eps1 = 0.2, eps2 = 0.5;

    TH2D *mSeA1 = applyMask(seA, "mSeA1", eps1);
    TH2D *mMeA1 = applyMask(meA, "mMeA1", eps1);
    TH2D *mSeB1 = applyMask(seB, "mSeB1", eps1);
    TH2D *mMeB1 = applyMask(meB, "mMeB1", eps1);

    TH2D *mSeA2 = applyMask(seA, "mSeA2", eps2);
    TH2D *mMeA2 = applyMask(meA, "mMeA2", eps2);
    TH2D *mSeB2 = applyMask(seB, "mSeB2", eps2);
    TH2D *mMeB2 = applyMask(meB, "mMeB2", eps2);

    // Background-subtracted 2D results for each epsilon
    TH2D *mResA1 = (TH2D*)mSeA1->Clone("mResA1"); mResA1->Add(mMeA1, -1.0); mResA1->SetDirectory(0);
    TH2D *mResB1 = (TH2D*)mSeB1->Clone("mResB1"); mResB1->Add(mMeB1, -1.0); mResB1->SetDirectory(0);
    TH2D *mResA2 = (TH2D*)mSeA2->Clone("mResA2"); mResA2->Add(mMeA2, -1.0); mResA2->SetDirectory(0);
    TH2D *mResB2 = (TH2D*)mSeB2->Clone("mResB2"); mResB2->Add(mMeB2, -1.0); mResB2->SetDirectory(0);

    auto savePath = [&](const string &suf) { return outDir + "/sideband-" + suf + ".pdf"; };

    // Labels
    const string lA1 = "pPb (#varepsilon=0.2)", lB1 = "Pbp (#varepsilon=0.2)";
    const string lA2 = "pPb (#varepsilon=0.5)", lB2 = "Pbp (#varepsilon=0.5)";

    // SE projections (scaled by 0.5 for symmetry)
    TH1D *seEtA1 = projEta(mSeA1,"seEtA1"); seEtA1->Scale(0.5);
    TH1D *seEtB1 = projEta(mSeB1,"seEtB1"); seEtB1->Scale(0.5);
    TH1D *sePhA1 = projPhi(mSeA1,"sePhA1"); sePhA1->Scale(0.5);
    TH1D *sePhB1 = projPhi(mSeB1,"sePhB1"); sePhB1->Scale(0.5);
    TH1D *seEtA2 = projEta(mSeA2,"seEtA2"); seEtA2->Scale(0.5);
    TH1D *seEtB2 = projEta(mSeB2,"seEtB2"); seEtB2->Scale(0.5);
    TH1D *sePhA2 = projPhi(mSeA2,"sePhA2"); sePhA2->Scale(0.5);
    TH1D *sePhB2 = projPhi(mSeB2,"sePhB2"); sePhB2->Scale(0.5);

    // ME projections
    TH1D *meEtA1 = projEta(mMeA1,"meEtA1"); meEtA1->Scale(0.5);
    TH1D *meEtB1 = projEta(mMeB1,"meEtB1"); meEtB1->Scale(0.5);
    TH1D *mePhA1 = projPhi(mMeA1,"mePhA1"); mePhA1->Scale(0.5);
    TH1D *mePhB1 = projPhi(mMeB1,"mePhB1"); mePhB1->Scale(0.5);
    TH1D *meEtA2 = projEta(mMeA2,"meEtA2"); meEtA2->Scale(0.5);
    TH1D *meEtB2 = projEta(mMeB2,"meEtB2"); meEtB2->Scale(0.5);
    TH1D *mePhA2 = projPhi(mMeA2,"mePhA2"); mePhA2->Scale(0.5);
    TH1D *mePhB2 = projPhi(mMeB2,"mePhB2"); mePhB2->Scale(0.5);

    // Result projections
    TH1D *resEtA1 = projEta(mResA1,"resEtA1"); resEtA1->Scale(0.5);
    TH1D *resEtB1 = projEta(mResB1,"resEtB1"); resEtB1->Scale(0.5);
    TH1D *resPhA1 = projPhi(mResA1,"resPhA1"); resPhA1->Scale(0.5);
    TH1D *resPhB1 = projPhi(mResB1,"resPhB1"); resPhB1->Scale(0.5);
    TH1D *resEtA2 = projEta(mResA2,"resEtA2"); resEtA2->Scale(0.5);
    TH1D *resEtB2 = projEta(mResB2,"resEtB2"); resEtB2->Scale(0.5);
    TH1D *resPhA2 = projPhi(mResA2,"resPhA2"); resPhA2->Scale(0.5);
    TH1D *resPhB2 = projPhi(mResB2,"resPhB2"); resPhB2->Scale(0.5);

    // ---- 1D plots ----
    // SE: ratio subplot, legend shown
    save1DPlot(seEtA1,seEtB1,lA1,lB1, seEtA2,seEtB2,lA2,lB2,
               "#Delta#eta_{ch,Z}", "Same-event (sideband)", 0, 4,
               savePath("SameEvent-DeltaEta"), true, true);
    save1DPlot(sePhA1,sePhB1,lA1,lB1, sePhA2,sePhB2,lA2,lB2,
               "#Delta#phi_{ch,Z}", "Same-event (sideband)", 0, M_PI,
               savePath("SameEvent-DeltaPhi"), true, true);

    // ME: ratio subplot, legend shown
    save1DPlot(meEtA1,meEtB1,lA1,lB1, meEtA2,meEtB2,lA2,lB2,
               "#Delta#eta_{ch,Z}", "Mixed-event (sideband)", 0, 4,
               savePath("MixedEvent-DeltaEta"), true, true);
    save1DPlot(mePhA1,mePhB1,lA1,lB1, mePhA2,mePhB2,lA2,lB2,
               "#Delta#phi_{ch,Z}", "Mixed-event (sideband)", 0, M_PI,
               savePath("MixedEvent-DeltaPhi"), true, true);

    // Result: difference subplot; no legend for DeltaEta, legend for DeltaPhi
    save1DPlot(resEtA1,resEtB1,lA1,lB1, resEtA2,resEtB2,lA2,lB2,
               "#Delta#eta_{ch,Z}", "Result (sideband)", 0, 4,
               savePath("Result-DeltaEta"), false, false);
    save1DPlot(resPhA1,resPhB1,lA1,lB1, resPhA2,resPhB2,lA2,lB2,
               "#Delta#phi_{ch,Z}", "Result (sideband)", 0, M_PI,
               savePath("Result-DeltaPhi"), false, true);

    // ---- 2D heatmaps: full (unmasked) ME distributions ----
    TH2D *meAplot = (TH2D*)meA->Clone("meAplot"); meAplot->SetDirectory(0);
    TH2D *meBplot = (TH2D*)meB->Clone("meBplot"); meBplot->SetDirectory(0);
    TH2D *meRat   = (TH2D*)meA->Clone("meRat");   meRat->SetDirectory(0);
    meRat->Divide(meB);

    meAplot->SetTitle("pPb mixed-event");
    save2DPlot(meAplot, "d#LTN_{ch}#GT/d#Delta#etad#Delta#phi", savePath("2D-ME-pPb"));
    meBplot->SetTitle("Pbp mixed-event");
    save2DPlot(meBplot, "d#LTN_{ch}#GT/d#Delta#etad#Delta#phi", savePath("2D-ME-Pbp"));
    meRat->SetTitle("pPb / Pbp mixed-event");
    save2DPlot(meRat, "Ratio pPb/Pbp", savePath("2D-ME-Ratio"));

    cout << "=== Done: " << outDir << "/sideband-*.pdf ===" << endl;
    return 0;
}
