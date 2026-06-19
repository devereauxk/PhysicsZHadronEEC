#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLine.h>
#include <TLegend.h>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <cmath>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"
// SetStyle.h defines an overloaded SetTDRStyle() that conflicts with MITHIG_CMSStyle.h
// (already included via KylesPlotting.h). We don't need the no-arg version, so don't include it.

// ---- utilities ----

void divByWidth(TH1D *h) {
    for (int i = 1; i <= h->GetNbinsX(); i++) {
        double w = h->GetBinWidth(i);
        if (w > 0) { h->SetBinContent(i, h->GetBinContent(i)/w); h->SetBinError(i, h->GetBinError(i)/w); }
    }
}
TH1D *projectX(TH2D *h2, const char *name, double norm) {
    TH1D *h = (TH1D*)h2->ProjectionX(name, 1, h2->GetNbinsY());
    h->SetDirectory(0); divByWidth(h); if (norm) h->Scale(norm); return h;
}
TH1D *projectY(TH2D *h2, const char *name, double norm) {
    TH1D *h = (TH1D*)h2->ProjectionY(name, 1, h2->GetNbinsX());
    h->SetDirectory(0); divByWidth(h); if (norm) h->Scale(norm); return h;
}
// Range-restricted projections: project over subset of the orthogonal bins
TH1D *projectXRange(TH2D *h2, const char *name, int ymin, int ymax, double norm) {
    TH1D *h = (TH1D*)h2->ProjectionX(name, ymin, ymax);
    h->SetDirectory(0); divByWidth(h); if (norm) h->Scale(norm); return h;
}
TH1D *projectYRange(TH2D *h2, const char *name, int xmin, int xmax, double norm) {
    TH1D *h = (TH1D*)h2->ProjectionY(name, xmin, xmax);
    h->SetDirectory(0); divByWidth(h); if (norm) h->Scale(norm); return h;
}

// ---- per-pair loaded histograms ----

struct PairData {
    TH1D *seEtaPPb, *sePhiPPb, *meEtaPPb, *mePhiPPb, *resEtaPPb, *resPhiPPb;
    TH1D *seEtaPbP, *sePhiPbP, *meEtaPbP, *mePhiPbP, *resEtaPbP, *resPhiPbP;
};

static string nosubPath(const string &p) {
    size_t pos = p.rfind("-result.root");
    return pos != string::npos ? p.substr(0, pos) + "-nosub.root" : p;
}

PairData loadPairData(const string &pPbRes, const string &PbPRes, const string &key, int idx) {
    TFile *frA = TFile::Open(pPbRes.c_str(),          "READ");
    TFile *frB = TFile::Open(PbPRes.c_str(),           "READ");
    TFile *fsA = TFile::Open(nosubPath(pPbRes).c_str(),"READ");
    TFile *fsB = TFile::Open(nosubPath(PbPRes).c_str(),"READ");
    for (auto &[p,f] : vector<pair<string,TFile*>>{{pPbRes,frA},{PbPRes,frB},{nosubPath(pPbRes),fsA},{nosubPath(PbPRes),fsB}}) {
        if (!f || f->IsZombie()) { cerr << "Cannot open: " << p << endl; exit(1); }
    }

    string s = Form("%d", idx);
    TH2D *sSamA = (TH2D*)fsA->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixA = (TH2D*)fsA->Get(Form("hMixData_%s", key.c_str()));
    TH2D *sSamB = (TH2D*)fsB->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixB = (TH2D*)fsB->Get(Form("hMixData_%s", key.c_str()));
    TH1D *rEtA  = (TH1D*)frA->Get(Form("DeltaEta_Result%s", key.c_str()));
    TH1D *rPhA  = (TH1D*)frA->Get(Form("DeltaPhi_Result%s", key.c_str()));
    TH1D *rEtB  = (TH1D*)frB->Get(Form("DeltaEta_Result%s", key.c_str()));
    TH1D *rPhB  = (TH1D*)frB->Get(Form("DeltaPhi_Result%s", key.c_str()));
    for (auto &[k,h] : vector<pair<string,void*>>{
            {"hData_A",sSamA},{"hMixData_A",sMixA},{"hData_B",sSamB},{"hMixData_B",sMixB},
            {"ResEta_A",rEtA},{"ResPhi_A",rPhA},{"ResEta_B",rEtB},{"ResPhi_B",rPhB}}) {
        if (!h) { cerr << "Missing histo: " << k << " key=" << key << endl; exit(1); }
    }

    PairData d;
    d.seEtaPPb = projectX(sSamA, Form("seEtaA%s",s.c_str()), 0.5);
    d.sePhiPPb = projectY(sSamA, Form("sePhiA%s",s.c_str()), 0.5);
    d.meEtaPPb = projectX(sMixA, Form("meEtaA%s",s.c_str()), 0.5);
    d.mePhiPPb = projectY(sMixA, Form("mePhiA%s",s.c_str()), 0.5);
    d.seEtaPbP = projectX(sSamB, Form("seEtaB%s",s.c_str()), 0.5);
    d.sePhiPbP = projectY(sSamB, Form("sePhiB%s",s.c_str()), 0.5);
    d.meEtaPbP = projectX(sMixB, Form("meEtaB%s",s.c_str()), 0.5);
    d.mePhiPbP = projectY(sMixB, Form("mePhiB%s",s.c_str()), 0.5);

    auto cloneDetach = [](TH1D *src, const char *name) {
        TH1D *h = (TH1D*)src->Clone(name); h->SetDirectory(0); return h;
    };
    d.resEtaPPb = cloneDetach(rEtA, Form("resEtaA%s",s.c_str())); d.resEtaPPb->Scale(0.5);
    d.resPhiPPb = cloneDetach(rPhA, Form("resPhiA%s",s.c_str())); d.resPhiPPb->Scale(0.5);
    d.resEtaPbP = cloneDetach(rEtB, Form("resEtaB%s",s.c_str())); d.resEtaPbP->Scale(0.5);
    d.resPhiPbP = cloneDetach(rPhB, Form("resPhiB%s",s.c_str())); d.resPhiPbP->Scale(0.5);

    frA->Close(); frB->Close(); fsA->Close(); fsB->Close();
    return d;
}

// ---- Study 9: ME-corrected Pbp — load with bin-by-bin double-ratio correction ----
// Correction: replace Pbp ME bin-by-bin with pPb ME (C[i,j] = ME_pPb[i,j]/ME_Pbp[i,j]).
// After correction corrME_Pbp/ME_pPb = 1 in every 2D bin, so ME ratio is flat in both projections.
// Corrected Pbp result = SE_Pbp - ME_pPb; remaining disagreement with pPb is pure SE mismatch.
PairData loadMECorrPairData(const string &pPbRes, const string &PbPRes, const string &key, int idx) {
    TFile *frA = TFile::Open(pPbRes.c_str(),           "READ");
    TFile *frB = TFile::Open(PbPRes.c_str(),           "READ");
    TFile *fsA = TFile::Open(nosubPath(pPbRes).c_str(),"READ");
    TFile *fsB = TFile::Open(nosubPath(PbPRes).c_str(),"READ");
    for (auto &[p,f] : vector<pair<string,TFile*>>{{pPbRes,frA},{PbPRes,frB},{nosubPath(pPbRes),fsA},{nosubPath(PbPRes),fsB}}) {
        if (!f || f->IsZombie()) { cerr << "Cannot open: " << p << endl; exit(1); }
    }
    string s = Form("%d", idx);
    TH2D *sSamA = (TH2D*)fsA->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixA = (TH2D*)fsA->Get(Form("hMixData_%s", key.c_str()));
    TH2D *sSamB = (TH2D*)fsB->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixB = (TH2D*)fsB->Get(Form("hMixData_%s", key.c_str()));
    TH1D *rEtA  = (TH1D*)frA->Get(Form("DeltaEta_Result%s", key.c_str()));
    TH1D *rPhA  = (TH1D*)frA->Get(Form("DeltaPhi_Result%s", key.c_str()));
    for (auto &[k,h] : vector<pair<string,void*>>{
            {"hData_A",sSamA},{"hMixData_A",sMixA},{"hData_B",sSamB},{"hMixData_B",sMixB},
            {"ResEta_A",rEtA},{"ResPhi_A",rPhA}}) {
        if (!h) { cerr << "Missing: " << k << " key=" << key << endl; exit(1); }
    }

    // Corrected ME for Pbp: replace each bin with ME_pPb so corrME_Pbp/ME_pPb = 1 everywhere.
    // Fallback to ME_Pbp only when ME_pPb is unavailable (zero/negative).
    TH2D *hCorrME = (TH2D*)sMixB->Clone(Form("corrME%s",s.c_str())); hCorrME->SetDirectory(0);
    int nX = sMixB->GetNbinsX(), nY = sMixB->GetNbinsY();
    for (int ix=1; ix<=nX; ix++) for (int iy=1; iy<=nY; iy++) {
        double me_pPb = sMixA->GetBinContent(ix,iy);
        double me_Pbp = sMixB->GetBinContent(ix,iy);
        hCorrME->SetBinContent(ix, iy, (me_pPb > 0) ? me_pPb : me_Pbp);
    }
    // corrected result 2D: SE_Pbp - corrME_Pbp
    TH2D *hCorrRes = (TH2D*)sSamB->Clone(Form("corrRes%s",s.c_str())); hCorrRes->SetDirectory(0);
    hCorrRes->Add(hCorrME, -1.0);

    PairData d;
    d.seEtaPPb = projectX(sSamA, Form("seEtaA%s",s.c_str()), 0.5);
    d.sePhiPPb = projectY(sSamA, Form("sePhiA%s",s.c_str()), 0.5);
    d.meEtaPPb = projectX(sMixA, Form("meEtaA%s",s.c_str()), 0.5);
    d.mePhiPPb = projectY(sMixA, Form("mePhiA%s",s.c_str()), 0.5);
    d.seEtaPbP = projectX(sSamB, Form("seEtaB%s",s.c_str()), 0.5);
    d.sePhiPbP = projectY(sSamB, Form("sePhiB%s",s.c_str()), 0.5);
    d.meEtaPbP = projectX(hCorrME,  Form("meEtaB%s",s.c_str()), 0.5);
    d.mePhiPbP = projectY(hCorrME,  Form("mePhiB%s",s.c_str()), 0.5);
    // Result: DeltaEta over phi bins 4-6 ([0,pi/2]), DeltaPhi over eta bins 7-12 ([0,4])
    d.resEtaPbP = projectXRange(hCorrRes, Form("resEtaB%s",s.c_str()), 4, 6, 0.5);
    d.resPhiPbP = projectYRange(hCorrRes, Form("resPhiB%s",s.c_str()), 7, 12, 0.5);
    auto cloneDetach = [](TH1D *src, const char *name) {
        TH1D *h = (TH1D*)src->Clone(name); h->SetDirectory(0); return h;
    };
    d.resEtaPPb = cloneDetach(rEtA, Form("resEtaA%s",s.c_str())); d.resEtaPPb->Scale(0.5);
    d.resPhiPPb = cloneDetach(rPhA, Form("resPhiA%s",s.c_str())); d.resPhiPPb->Scale(0.5);
    delete hCorrME; delete hCorrRes;
    frA->Close(); frB->Close(); fsA->Close(); fsB->Close();
    return d;
}

// ---- Study 16: ME-corrected via inverse SE-derived scale factor ----
// Derive bin-by-bin sf[ix,iy] = SE_Pbp[ix,iy] / SE_pPb[ix,iy] (inverse of SE ratio).
// Apply to ME_Pbp: corrME_Pbp = ME_Pbp * sf.
// Corrected result = SE_Pbp - corrME_Pbp (original SE unchanged).
// No closure expected in SE, ME, or result.
PairData loadSECorrPairData(const string &pPbRes, const string &PbPRes, const string &key, int idx) {
    TFile *frA = TFile::Open(pPbRes.c_str(),           "READ");
    TFile *frB = TFile::Open(PbPRes.c_str(),           "READ");
    TFile *fsA = TFile::Open(nosubPath(pPbRes).c_str(),"READ");
    TFile *fsB = TFile::Open(nosubPath(PbPRes).c_str(),"READ");
    for (auto &[p,f] : vector<pair<string,TFile*>>{{pPbRes,frA},{PbPRes,frB},{nosubPath(pPbRes),fsA},{nosubPath(PbPRes),fsB}}) {
        if (!f || f->IsZombie()) { cerr << "Cannot open: " << p << endl; exit(1); }
    }
    string s = Form("%d", idx);
    TH2D *sSamA = (TH2D*)fsA->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixA = (TH2D*)fsA->Get(Form("hMixData_%s", key.c_str()));
    TH2D *sSamB = (TH2D*)fsB->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixB = (TH2D*)fsB->Get(Form("hMixData_%s", key.c_str()));
    TH1D *rEtA  = (TH1D*)frA->Get(Form("DeltaEta_Result%s", key.c_str()));
    TH1D *rPhA  = (TH1D*)frA->Get(Form("DeltaPhi_Result%s", key.c_str()));
    for (auto &[k,h] : vector<pair<string,void*>>{
            {"hData_A",sSamA},{"hMixData_A",sMixA},{"hData_B",sSamB},{"hMixData_B",sMixB},
            {"ResEta_A",rEtA},{"ResPhi_A",rPhA}}) {
        if (!h) { cerr << "Missing: " << k << " key=" << key << endl; exit(1); }
    }

    // Inverse scale factor from SE ratio: sf[ix,iy] = SE_Pbp[ix,iy] / SE_pPb[ix,iy].
    // Apply to ME_Pbp: corrME_Pbp = ME_Pbp * sf. Fall back to sf=1 where SE_pPb <= 0.
    TH2D *hCorrME = (TH2D*)sMixB->Clone(Form("corrME%s",s.c_str())); hCorrME->SetDirectory(0);
    int nX = sMixB->GetNbinsX(), nY = sMixB->GetNbinsY();
    for (int ix=1; ix<=nX; ix++) for (int iy=1; iy<=nY; iy++) {
        double se_pPb = sSamA->GetBinContent(ix,iy);
        double se_Pbp = sSamB->GetBinContent(ix,iy);
        double sf = (se_pPb > 0) ? se_Pbp / se_pPb : 1.0;
        hCorrME->SetBinContent(ix, iy, sMixB->GetBinContent(ix,iy) * sf);
    }
    // Corrected result: original SE_Pbp - SE-scaled ME_Pbp
    TH2D *hCorrRes = (TH2D*)sSamB->Clone(Form("seRes%s",s.c_str())); hCorrRes->SetDirectory(0);
    hCorrRes->Add(hCorrME, -1.0);

    PairData d;
    d.seEtaPPb = projectX(sSamA,   Form("seEtaA%s",s.c_str()), 0.5);
    d.sePhiPPb = projectY(sSamA,   Form("sePhiA%s",s.c_str()), 0.5);
    d.meEtaPPb = projectX(sMixA,   Form("meEtaA%s",s.c_str()), 0.5);
    d.mePhiPPb = projectY(sMixA,   Form("mePhiA%s",s.c_str()), 0.5);
    d.seEtaPbP = projectX(sSamB,   Form("seEtaB%s",s.c_str()), 0.5);  // original Pbp SE
    d.sePhiPbP = projectY(sSamB,   Form("sePhiB%s",s.c_str()), 0.5);
    d.meEtaPbP = projectX(hCorrME, Form("meEtaB%s",s.c_str()), 0.5);  // SE-scaled ME_Pbp
    d.mePhiPbP = projectY(hCorrME, Form("mePhiB%s",s.c_str()), 0.5);
    // Result: DeltaEta over phi bins 4-6 ([0,pi/2]), DeltaPhi full eta range (bins 1-12)
    d.resEtaPbP = projectXRange(hCorrRes, Form("resEtaB%s",s.c_str()), 4, 6,  0.5);
    d.resPhiPbP = projectYRange(hCorrRes, Form("resPhiB%s",s.c_str()), 1, 12, 0.5);
    auto cloneDetach = [](TH1D *src, const char *name) {
        TH1D *h = (TH1D*)src->Clone(name); h->SetDirectory(0); return h;
    };
    d.resEtaPPb = cloneDetach(rEtA, Form("resEtaA%s",s.c_str())); d.resEtaPPb->Scale(0.5);
    d.resPhiPPb = cloneDetach(rPhA, Form("resPhiA%s",s.c_str())); d.resPhiPPb->Scale(0.5);
    delete hCorrME; delete hCorrRes;
    frA->Close(); frB->Close(); fsA->Close(); fsB->Close();
    return d;
}

// ---- Study 23: Double-ratio ME correction applied to Pbp ----
// Scale factor: sf[ix,iy] = (SE_Pbp[ix,iy] / SE_pPb[ix,iy]) / (ME_Pbp[ix,iy] / ME_pPb[ix,iy])
//             = (SE_Pbp[ix,iy] * ME_pPb[ix,iy]) / (SE_pPb[ix,iy] * ME_Pbp[ix,iy])
// Applied to Pbp ME: corrME_Pbp = ME_Pbp * sf. pPb SE and ME unchanged.
// By construction: corrME_Pbp / ME_pPb = SE_Pbp / SE_pPb, i.e. ME ratio pPb/Pbp matches SE ratio.
// Corrected Pbp result = SE_Pbp - corrME_Pbp. pPb result loaded from precomputed file.
PairData loadDoubleRatioCorrPairData(const string &pPbRes, const string &PbPRes, const string &key, int idx) {
    TFile *frA = TFile::Open(pPbRes.c_str(),           "READ");
    TFile *frB = TFile::Open(PbPRes.c_str(),           "READ");
    TFile *fsA = TFile::Open(nosubPath(pPbRes).c_str(),"READ");
    TFile *fsB = TFile::Open(nosubPath(PbPRes).c_str(),"READ");
    for (auto &[p,f] : vector<pair<string,TFile*>>{{pPbRes,frA},{PbPRes,frB},{nosubPath(pPbRes),fsA},{nosubPath(PbPRes),fsB}}) {
        if (!f || f->IsZombie()) { cerr << "Cannot open: " << p << endl; exit(1); }
    }
    string s = Form("%d", idx);
    TH2D *sSamA = (TH2D*)fsA->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixA = (TH2D*)fsA->Get(Form("hMixData_%s", key.c_str()));
    TH2D *sSamB = (TH2D*)fsB->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixB = (TH2D*)fsB->Get(Form("hMixData_%s", key.c_str()));
    TH1D *rEtA  = (TH1D*)frA->Get(Form("DeltaEta_Result%s", key.c_str()));
    TH1D *rPhA  = (TH1D*)frA->Get(Form("DeltaPhi_Result%s", key.c_str()));
    for (auto &[k,h] : vector<pair<string,void*>>{
            {"hData_A",sSamA},{"hMixData_A",sMixA},{"hData_B",sSamB},{"hMixData_B",sMixB},
            {"ResEta_A",rEtA},{"ResPhi_A",rPhA}}) {
        if (!h) { cerr << "Missing: " << k << " key=" << key << endl; exit(1); }
    }

    // Double-ratio scale factor applied to Pbp ME.
    // sf = (SE_Pbp * ME_pPb) / (SE_pPb * ME_Pbp). Fallback to sf=1 if denominator <= 0.
    TH2D *hCorrME = (TH2D*)sMixB->Clone(Form("drCorrME%s",s.c_str())); hCorrME->SetDirectory(0);
    int nX = sMixB->GetNbinsX(), nY = sMixB->GetNbinsY();
    for (int ix=1; ix<=nX; ix++) for (int iy=1; iy<=nY; iy++) {
        double se_pPb = sSamA->GetBinContent(ix,iy);
        double se_Pbp = sSamB->GetBinContent(ix,iy);
        double me_pPb = sMixA->GetBinContent(ix,iy);
        double me_Pbp = sMixB->GetBinContent(ix,iy);
        double denom  = se_pPb * me_Pbp;
        double sf     = (denom > 0) ? (se_Pbp * me_pPb) / denom : 1.0;
        hCorrME->SetBinContent(ix, iy, me_Pbp * sf);
    }
    // Corrected Pbp result: SE_Pbp - corrME_Pbp
    TH2D *hCorrRes = (TH2D*)sSamB->Clone(Form("drCorrRes%s",s.c_str())); hCorrRes->SetDirectory(0);
    hCorrRes->Add(hCorrME, -1.0);

    auto cloneDetach = [](TH1D *src, const char *name) {
        TH1D *h = (TH1D*)src->Clone(name); h->SetDirectory(0); return h;
    };
    PairData d;
    d.seEtaPPb = projectX(sSamA,   Form("seEtaA%s",s.c_str()), 0.5);  // pPb SE unchanged
    d.sePhiPPb = projectY(sSamA,   Form("sePhiA%s",s.c_str()), 0.5);
    d.meEtaPPb = projectX(sMixA,   Form("meEtaA%s",s.c_str()), 0.5);  // pPb ME unchanged
    d.mePhiPPb = projectY(sMixA,   Form("mePhiA%s",s.c_str()), 0.5);
    d.seEtaPbP = projectX(sSamB,   Form("seEtaB%s",s.c_str()), 0.5);  // Pbp SE unchanged
    d.sePhiPbP = projectY(sSamB,   Form("sePhiB%s",s.c_str()), 0.5);
    d.meEtaPbP = projectX(hCorrME, Form("meEtaB%s",s.c_str()), 0.5);  // double-ratio corrected Pbp ME
    d.mePhiPbP = projectY(hCorrME, Form("mePhiB%s",s.c_str()), 0.5);
    // Pbp corrected result: DeltaEta over phi bins 4-6 ([0,pi/2]), DeltaPhi full eta range
    d.resEtaPbP = projectXRange(hCorrRes, Form("resEtaB%s",s.c_str()), 4, 6,  0.5);
    d.resPhiPbP = projectYRange(hCorrRes, Form("resPhiB%s",s.c_str()), 1, 12, 0.5);
    // pPb result from precomputed file (unchanged)
    d.resEtaPPb = cloneDetach(rEtA, Form("resEtaA%s",s.c_str())); d.resEtaPPb->Scale(0.5);
    d.resPhiPPb = cloneDetach(rPhA, Form("resPhiA%s",s.c_str())); d.resPhiPPb->Scale(0.5);
    delete hCorrME; delete hCorrRes;
    frA->Close(); frB->Close(); fsA->Close(); fsB->Close();
    return d;
}

// ---- Study 10: ME-scaled Pbp — scale Pbp ME by meScale, recompute result ----
PairData loadMEScalePairData(const string &pPbRes, const string &PbPRes, const string &key, int idx, double meScale) {
    TFile *frA = TFile::Open(pPbRes.c_str(),           "READ");
    TFile *frB = TFile::Open(PbPRes.c_str(),           "READ");
    TFile *fsA = TFile::Open(nosubPath(pPbRes).c_str(),"READ");
    TFile *fsB = TFile::Open(nosubPath(PbPRes).c_str(),"READ");
    for (auto &[p,f] : vector<pair<string,TFile*>>{{pPbRes,frA},{PbPRes,frB},{nosubPath(pPbRes),fsA},{nosubPath(PbPRes),fsB}}) {
        if (!f || f->IsZombie()) { cerr << "Cannot open: " << p << endl; exit(1); }
    }
    string s = Form("%d", idx);
    TH2D *sSamA = (TH2D*)fsA->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixA = (TH2D*)fsA->Get(Form("hMixData_%s", key.c_str()));
    TH2D *sSamB = (TH2D*)fsB->Get(Form("hData_%s",    key.c_str()));
    TH2D *sMixB = (TH2D*)fsB->Get(Form("hMixData_%s", key.c_str()));
    TH1D *rEtA  = (TH1D*)frA->Get(Form("DeltaEta_Result%s", key.c_str()));
    TH1D *rPhA  = (TH1D*)frA->Get(Form("DeltaPhi_Result%s", key.c_str()));
    for (auto &[k,h] : vector<pair<string,void*>>{
            {"hData_A",sSamA},{"hMixData_A",sMixA},{"hData_B",sSamB},{"hMixData_B",sMixB},
            {"ResEta_A",rEtA},{"ResPhi_A",rPhA}}) {
        if (!h) { cerr << "Missing: " << k << " key=" << key << endl; exit(1); }
    }
    // Scaled Pbp ME
    TH2D *hScalME = (TH2D*)sMixB->Clone(Form("scME%s",s.c_str())); hScalME->SetDirectory(0);
    hScalME->Scale(meScale);
    // corrected result: SE_Pbp - meScale * ME_Pbp
    TH2D *hCorrRes = (TH2D*)sSamB->Clone(Form("scRes%s",s.c_str())); hCorrRes->SetDirectory(0);
    hCorrRes->Add(hScalME, -1.0);

    PairData d;
    d.seEtaPPb = projectX(sSamA, Form("seEtaA%s",s.c_str()), 0.5);
    d.sePhiPPb = projectY(sSamA, Form("sePhiA%s",s.c_str()), 0.5);
    d.meEtaPPb = projectX(sMixA, Form("meEtaA%s",s.c_str()), 0.5);
    d.mePhiPPb = projectY(sMixA, Form("mePhiA%s",s.c_str()), 0.5);
    d.seEtaPbP = projectX(sSamB, Form("seEtaB%s",s.c_str()), 0.5);
    d.sePhiPbP = projectY(sSamB, Form("sePhiB%s",s.c_str()), 0.5);
    d.meEtaPbP = projectX(hScalME,  Form("meEtaB%s",s.c_str()), 0.5);
    d.mePhiPbP = projectY(hScalME,  Form("mePhiB%s",s.c_str()), 0.5);
    // Result: DeltaEta over phi bins 4-6 ([0,pi/2]), DeltaPhi over eta bins 7-12 ([0,4])
    d.resEtaPbP = projectXRange(hCorrRes, Form("resEtaB%s",s.c_str()), 4, 6, 0.5);
    d.resPhiPbP = projectYRange(hCorrRes, Form("resPhiB%s",s.c_str()), 7, 12, 0.5);
    auto cloneDetach = [](TH1D *src, const char *name) {
        TH1D *h = (TH1D*)src->Clone(name); h->SetDirectory(0); return h;
    };
    d.resEtaPPb = cloneDetach(rEtA, Form("resEtaA%s",s.c_str())); d.resEtaPPb->Scale(0.5);
    d.resPhiPPb = cloneDetach(rPhA, Form("resPhiA%s",s.c_str())); d.resPhiPPb->Scale(0.5);
    delete hScalME; delete hCorrRes;
    frA->Close(); frB->Close(); fsA->Close(); fsB->Close();
    return d;
}

// ---- multi-pair canvas renderer ----
// Draws up to 3 {pPb, Pbp} pairs in top panel with per-pair line styles.
// Bottom panel: one ratio or diff curve per pair (pPb/Pbp or pPb-Pbp).

static int gUID = 0;

void makePairPlot(
    const vector<pair<TH1D*,TH1D*>> &pairs,
    const vector<string> &pPbLabels, const vector<string> &PbPLabels,
    const vector<int> &lineStyles,
    int pPbColor, int PbPColor,
    const char *xTitle, double xMin, double xMax,
    const char *yTitle, const char *rTitle,
    bool isRatio,
    const string &outPath,
    bool showLegend = true,
    double legendX = 0.55
) {
    SetTDRStyle();
    string uid = Form("mc%d", gUID++);
    TCanvas *c = new TCanvas(uid.c_str(), uid.c_str(), 600, 600);
    c->cd();

    const double border = 0.06;
    TPad *pad1 = new TPad((uid+"p1").c_str(), "", border, 0.25+border, 1.0-border, 1.0-border);
    pad1->SetBottomMargin(0.0); pad1->Draw();
    TPad *pad2 = new TPad((uid+"p2").c_str(), "", border, border, 1.0-border, 0.25+border);
    pad2->SetTopMargin(0.0); pad2->SetBottomMargin(0.25); pad2->Draw();

    // ---- top panel ----
    pad1->cd();
    double gmin=1e30, gmax=-1e30;
    for (auto &[h1,h2] : pairs) {
        for (auto h : {h1,h2}) {
            int b1=max(1,h->FindBin(xMin+1e-9)), b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
            for (int b=b1; b<=b2; b++) { double v=h->GetBinContent(b), e=h->GetBinError(b); gmin=min(gmin,v-e); gmax=max(gmax,v+e); }
        }
    }
    double mg = (0.15/(1.0-2*0.15))*(gmax-gmin);

    int nLeg = 2*(int)pairs.size();
    TLegend *leg = new TLegend(legendX, max(0.35, 0.85-0.07*nLeg), legendX+0.23, 0.85);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextFont(42); leg->SetTextSize(0.032);

    bool first = true;
    for (int pi = 0; pi < (int)pairs.size(); pi++) {
        for (auto &[h, col, lbl] : vector<tuple<TH1D*,int,string>>{
                {pairs[pi].first, pPbColor, pPbLabels[pi]},
                {pairs[pi].second, PbPColor, PbPLabels[pi]}}) {
            h->SetLineColor(col); h->SetLineStyle(lineStyles[pi]); h->SetLineWidth(2);
            h->SetMarkerSize(0);
            h->GetXaxis()->SetRangeUser(xMin,xMax); h->GetXaxis()->SetTitle(xTitle);
            h->GetYaxis()->SetTitle(yTitle); h->GetYaxis()->SetTitleSize(0.05); h->GetYaxis()->SetTitleOffset(0.7);
            h->GetYaxis()->SetRangeUser(gmin-mg, gmax+mg);
            h->Draw(first ? "HIST E" : "HIST E SAME"); first = false;
            leg->AddEntry(h, lbl.c_str(), "l");
        }
    }
    if (showLegend) leg->Draw("SAME");
    AddCMSHeader(pad1, "Internal", false);
    AddUPCHeader(pad1, "8.16 TeV", "PPb");

    // ---- bottom panel ----
    pad2->cd();
    double maxDev = isRatio ? 0.02 : 1e-9;
    for (auto &[h1,h2] : pairs) {
        int b1=max(1,h1->FindBin(xMin+1e-9)), b2=min(h1->GetNbinsX(),h1->FindBin(xMax-1e-9));
        for (int b=b1; b<=b2; b++) {
            double v1=h1->GetBinContent(b), e1=h1->GetBinError(b), v2=h2->GetBinContent(b), e2=h2->GetBinError(b);
            if (isRatio) { if (!v2) continue; double rat=v1/v2; maxDev=max(maxDev, fabs(rat-1.0)+fabs(rat)*hypot(v1?e1/v1:0,e2/v2)); }
            else maxDev=max(maxDev, fabs(v1-v2)+hypot(e1,e2));
        }
    }
    maxDev /= 0.85;
    double rMin = isRatio ? 1.0-maxDev : -maxDev;
    double rMax = isRatio ? 1.0+maxDev :  maxDev;

    bool firstR = true;
    for (int pi = 0; pi < (int)pairs.size(); pi++) {
        TH1D *hr = (TH1D*)pairs[pi].first->Clone(Form("%sr%d",uid.c_str(),pi));
        hr->SetDirectory(0);
        if (isRatio) hr->Divide(pairs[pi].second);
        else hr->Add(pairs[pi].second, -1.0);
        hr->SetLineColor(pPbColor); hr->SetLineStyle(lineStyles[pi]); hr->SetLineWidth(2);
        hr->GetXaxis()->SetRangeUser(xMin,xMax); hr->GetXaxis()->SetTitle(xTitle);
        hr->GetXaxis()->SetTitleSize(0.10); hr->GetXaxis()->SetLabelSize(0.08); hr->GetXaxis()->SetTitleOffset(1.0);
        hr->GetYaxis()->SetTitle(rTitle); hr->GetYaxis()->SetRangeUser(rMin,rMax);
        hr->GetYaxis()->SetTitleSize(0.09); hr->GetYaxis()->SetLabelSize(0.07); hr->GetYaxis()->SetTitleOffset(0.45);
        hr->GetYaxis()->SetNdivisions(505);
        hr->Draw(firstR ? "HIST E" : "HIST E SAME"); firstR = false;
    }
    double ref = isRatio ? 1.0 : 0.0;
    TLine *line = new TLine(xMin, ref, xMax, ref);
    line->SetLineColor(kGray+2); line->SetLineStyle(2); line->Draw("SAME");

    c->Update(); c->SaveAs(outPath.c_str()); delete c;
}

// ---- main ----

int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);

    // Required pair 1
    string pPbFile1  = CL.Get("pPbFile");
    string PbPFile1  = CL.Get("PbPFile");
    string pPbLabel1 = CL.Get("pPbLabel",  "pPb DATA");
    string PbPLabel1 = CL.Get("PbPLabel",  "Pbp DATA");

    // Optional pair 2
    string pPbFile2  = CL.Get("pPbFile2",  "");
    string PbPFile2  = CL.Get("PbPFile2",  "");
    string pPbLabel2 = CL.Get("pPbLabel2", "pPb 2");
    string PbPLabel2 = CL.Get("PbPLabel2", "Pbp 2");

    // Optional pair 3
    string pPbFile3  = CL.Get("pPbFile3",  "");
    string PbPFile3  = CL.Get("PbPFile3",  "");
    string pPbLabel3 = CL.Get("pPbLabel3", "pPb 3");
    string PbPLabel3 = CL.Get("PbPLabel3", "Pbp 3");

    // Optional standalone pp curve (single file, no pair)
    string ppFile   = CL.Get("ppFile",  "");
    string ppLabel  = CL.Get("ppLabel", "pp 8.16 TeV");

    string outputDir    = CL.Get("OutputDir",    "plots/12x12_ppbpbp");
    string tag          = CL.Get("Tag",          "study");
    bool showSignedEta  = CL.GetBool("ShowSignedEta", false);
    bool showFullPhi    = CL.GetBool("ShowFullPhi",   false);
    string trkPtKey     = CL.Get("TrkPtKey",     "0.5_15");

    // Study 9: ME correction mode
    bool meCorrPbP       = CL.GetBool("MECorrPbP", false);
    // Study 16: SE correction mode
    bool seCorrPbP       = CL.GetBool("SECorrPbP", false);
    // Study 9/16 overlay: add nominal Pbp (no correction) as dotted 3rd curve
    bool showNominalPbP  = CL.GetBool("ShowNominalPbP", false);
    string nominalPbPLabel = CL.Get("NominalPbPLabel", "Pbp nominal");
    // Study 23: double-ratio ME correction to Pbp
    bool doubleRatioCorrPbP = CL.GetBool("DoubleRatioCorrPbP", false);
    // Study 10: ME scale for pairs 2 and 3 (applied to Pbp ME only)
    double meScalePair2 = CL.GetDouble("MEScalePair2", 1.003);
    double meScalePair3 = CL.GetDouble("MEScalePair3", 0.997);

    mkdir(outputDir.c_str(), 0755);

    int nPairs = 1;
    if (!pPbFile2.empty() && !PbPFile2.empty()) nPairs = 2;
    if (nPairs == 2 && !pPbFile3.empty() && !PbPFile3.empty()) nPairs = 3;

    vector<PairData> pd;
    if (meCorrPbP)
        pd.push_back(loadMECorrPairData(pPbFile1, PbPFile1, trkPtKey, 0));
    else if (seCorrPbP)
        pd.push_back(loadSECorrPairData(pPbFile1, PbPFile1, trkPtKey, 0));
    else if (doubleRatioCorrPbP)
        pd.push_back(loadDoubleRatioCorrPairData(pPbFile1, PbPFile1, trkPtKey, 0));
    else
        pd.push_back(loadPairData(pPbFile1, PbPFile1, trkPtKey, 0));

    // Load nominal Pbp (no correction) for overlay when showNominalPbP is set.
    // Valid when meCorrPbP, seCorrPbP, or doubleRatioCorrPbP is true (otherwise nominal IS pair 0).
    PairData pdNom;
    if ((meCorrPbP || seCorrPbP || doubleRatioCorrPbP) && showNominalPbP)
        pdNom = loadPairData(pPbFile1, PbPFile1, trkPtKey, 10);
    if (nPairs >= 2) {
        bool useMEScale2 = (meScalePair2 != 1.0);
        if (useMEScale2) pd.push_back(loadMEScalePairData(pPbFile2, PbPFile2, trkPtKey, 1, meScalePair2));
        else pd.push_back(loadPairData(pPbFile2, PbPFile2, trkPtKey, 1));
    }
    if (nPairs >= 3) {
        bool useMEScale3 = (meScalePair3 != 1.0);
        if (useMEScale3) pd.push_back(loadMEScalePairData(pPbFile3, PbPFile3, trkPtKey, 2, meScalePair3));
        else pd.push_back(loadPairData(pPbFile3, PbPFile3, trkPtKey, 2));
    }

    const int pPbColor = cmsBlue, PbPColor = kSpring-6, ppColor = kRed+1;
    // Per-pair line styles: solid / long-dash / dotted
    const vector<int> kLineStyles = {1, 7, 3};

    double etaMin = showSignedEta ? -4.0 : 0.0, etaMax = 4.0;
    double phiMin = showFullPhi   ? -M_PI : 0.0, phiMax = M_PI;

    auto savePath = [&](const string &suf) { return outputDir+"/"+tag+"-"+suf+".pdf"; };

    // Load pp standalone data if requested
    bool hasPP = !ppFile.empty();
    PairData ppd;
    if (hasPP) ppd = loadPairData(ppFile, ppFile, trkPtKey, 99);

    if (nPairs == 1) {
        // ---- single-pair: use plotCMSRatio / plotCMSDiff ----
        // lower-panel ranges: include ±1σ error bars; pad so extrema are at 85% of half-range
        auto autoRatio = [](TH1 *n, TH1 *d) -> pair<double,double> {
            double m=0.02;
            for (int i=1; i<=n->GetNbinsX(); i++) {
                double nv=n->GetBinContent(i), ne=n->GetBinError(i), dv=d->GetBinContent(i), de=d->GetBinError(i);
                if (!dv) continue;
                double rat=nv/dv, ratErr=fabs(rat)*hypot(nv?ne/nv:0, de/dv);
                m=max(m, fabs(rat-1.0)+ratErr);
            }
            m/=0.85; return {1.0-m, 1.0+m};
        };
        auto autoDiff = [](TH1 *a, TH1 *b) -> pair<double,double> {
            double m=1e-6;
            for (int i=1; i<=a->GetNbinsX(); i++) {
                double diff=a->GetBinContent(i)-b->GetBinContent(i);
                m=max(m, fabs(diff)+hypot(a->GetBinError(i),b->GetBinError(i)));
            }
            m/=0.85; return {-m, m};
        };
        // top-panel range: include ±1σ; 15% white space as fraction of total axis range
        auto autoTopRange = [](const vector<TH1*> &hv, double xMin, double xMax) -> pair<double,double> {
            double lo=1e30, hi=-1e30;
            for (auto h : hv) {
                int b1=max(1,h->FindBin(xMin+1e-9)), b2=min(h->GetNbinsX(),h->FindBin(xMax-1e-9));
                for (int b=b1; b<=b2; b++) { double v=h->GetBinContent(b), e=h->GetBinError(b); lo=min(lo,v-e); hi=max(hi,v+e); }
            }
            if (lo>hi) return {-1,-1};
            double rng=hi-lo, p=0.15/(1.0-2*0.15);
            return {lo-p*rng, hi+p*rng};
        };

        auto &d0 = pd[0];
        bool hasNomPbP = ((meCorrPbP || seCorrPbP || doubleRatioCorrPbP) && showNominalPbP);
        vector<string> labs = {pPbLabel1, PbPLabel1};
        vector<int> cols = {pPbColor, PbPColor}, ls = {0, 0}, ms = {mCircleFill, mCircleFill};
        if (hasPP) { labs.push_back(ppLabel); cols.push_back(ppColor); ls.push_back(0); ms.push_back(mCircleFill); }
        if (hasNomPbP) { labs.push_back(nominalPbPLabel); cols.push_back(PbPColor); ls.push_back(3); ms.push_back(mCircleFill); }

        auto mkC = [](const char *n){ return new TCanvas(n,n,600,600); };
        auto save = [&](TPad *p, TCanvas *c, const char *suf) {
            if (!p) return;
            AddCMSHeader(p, "Internal", false); AddUPCHeader(p, "8.16 TeV", "PPb");
            c->Update(); c->SaveAs(savePath(suf).c_str());
        };

        // Helper: widen a ratio [lo,hi] to also cover an optional additional comparison histogram
        auto extRatio = [&](pair<double,double> r, TH1D *ref, TH1D *var) -> pair<double,double> {
            if (!hasNomPbP || !var) return r;
            auto [lo,hi] = autoRatio(ref, var);
            return {min(r.first,lo), max(r.second,hi)};
        };
        auto extDiff = [&](pair<double,double> r, TH1D *a, TH1D *b) -> pair<double,double> {
            if (!hasNomPbP || !b) return r;
            auto [lo,hi] = autoDiff(a, b);
            return {min(r.first,lo), max(r.second,hi)};
        };

        { auto *c=mkC("cSEE"); vector<TH1*> hv={d0.seEtaPPb,d0.seEtaPbP}; if(hasPP)hv.push_back(ppd.seEtaPPb); if(hasNomPbP)hv.push_back(pdNom.seEtaPbP);
          auto [rl,rh]=extRatio(autoRatio(d0.seEtaPPb,d0.seEtaPbP),d0.seEtaPPb,pdNom.seEtaPbP); auto [tl,th]=autoTopRange(hv,etaMin,etaMax);
          save((TPad*)plotCMSRatio(hv,"SE_Eta",labs,cols,ls,cols,ms,"#Delta y_{ch,Z}",etaMin,etaMax,"Same-event d#LTN_{ch}#GT/d#Delta y_{ch,Z}",tl,th,"Ratio pPb/Pbp",rl,rh,0,false,false,true,0.55),c,"SameEvent-DeltaEta"); }
        { auto *c=mkC("cSEP"); vector<TH1*> hv={d0.sePhiPPb,d0.sePhiPbP}; if(hasPP)hv.push_back(ppd.sePhiPPb); if(hasNomPbP)hv.push_back(pdNom.sePhiPbP);
          auto [rl,rh]=extRatio(autoRatio(d0.sePhiPPb,d0.sePhiPbP),d0.sePhiPPb,pdNom.sePhiPbP); auto [tl,th]=autoTopRange(hv,phiMin,phiMax);
          save((TPad*)plotCMSRatio(hv,"SE_Phi",labs,cols,ls,cols,ms,"#Delta#phi_{ch,Z}",phiMin,phiMax,"Same-event d#LTN_{ch}#GT/d#Delta#phi_{ch,Z}",tl,th,"Ratio pPb/Pbp",rl,rh,0,false,false,true,0.55),c,"SameEvent-DeltaPhi"); }
        { auto *c=mkC("cMEE"); vector<TH1*> hv={d0.meEtaPPb,d0.meEtaPbP}; if(hasPP)hv.push_back(ppd.meEtaPPb); if(hasNomPbP)hv.push_back(pdNom.meEtaPbP);
          auto [rl,rh]=extRatio(autoRatio(d0.meEtaPPb,d0.meEtaPbP),d0.meEtaPPb,pdNom.meEtaPbP); auto [tl,th]=autoTopRange(hv,etaMin,etaMax);
          save((TPad*)plotCMSRatio(hv,"ME_Eta",labs,cols,ls,cols,ms,"#Delta y_{ch,Z}",etaMin,etaMax,"Mixed-event d#LTN_{ch}#GT/d#Delta y_{ch,Z}",tl,th,"Ratio pPb/Pbp",rl,rh,0,false,false,true,2.0),c,"MixedEvent-DeltaEta"); }
        { auto *c=mkC("cMEP"); vector<TH1*> hv={d0.mePhiPPb,d0.mePhiPbP}; if(hasPP)hv.push_back(ppd.mePhiPPb); if(hasNomPbP)hv.push_back(pdNom.mePhiPbP);
          auto [rl,rh]=extRatio(autoRatio(d0.mePhiPPb,d0.mePhiPbP),d0.mePhiPPb,pdNom.mePhiPbP); auto [tl,th]=autoTopRange(hv,phiMin,phiMax);
          save((TPad*)plotCMSRatio(hv,"ME_Phi",labs,cols,ls,cols,ms,"#Delta#phi_{ch,Z}",phiMin,phiMax,"Mixed-event d#LTN_{ch}#GT/d#Delta#phi_{ch,Z}",tl,th,"Ratio pPb/Pbp",rl,rh,0,false,false,true,2.0),c,"MixedEvent-DeltaPhi"); }
        { auto *c=mkC("cREE"); vector<TH1*> hv={d0.resEtaPPb,d0.resEtaPbP}; if(hasPP)hv.push_back(ppd.resEtaPPb); if(hasNomPbP)hv.push_back(pdNom.resEtaPbP);
          auto [dl,dh]=extDiff(autoDiff(d0.resEtaPPb,d0.resEtaPbP),d0.resEtaPPb,pdNom.resEtaPbP); auto [tl,th]=autoTopRange(hv,etaMin,etaMax);
          save((TPad*)plotCMSDiff(hv,"R_Eta",labs,cols,ls,cols,ms,"#Delta y_{ch,Z}",etaMin,etaMax,"Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}",tl,th,"Diff pPb#minusPbp",dl,dh,0,false,false,true,0.12),c,"Result-DeltaEta"); }
        { auto *c=mkC("cREP"); vector<TH1*> hv={d0.resPhiPPb,d0.resPhiPbP}; if(hasPP)hv.push_back(ppd.resPhiPPb); if(hasNomPbP)hv.push_back(pdNom.resPhiPbP);
          auto [dl,dh]=extDiff(autoDiff(d0.resPhiPPb,d0.resPhiPbP),d0.resPhiPPb,pdNom.resPhiPbP); auto [tl,th]=autoTopRange(hv,phiMin,phiMax);
          save((TPad*)plotCMSDiff(hv,"R_Phi",labs,cols,ls,cols,ms,"#Delta#phi_{ch,Z}",phiMin,phiMax,"Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}",tl,th,"Diff pPb#minusPbp",dl,dh,0,false,false,true,0.55),c,"Result-DeltaPhi"); }

    } else {
        // ---- multi-pair mode ----
        vector<pair<TH1D*,TH1D*>> seEta, sePhi, meEta, mePhi, resEta, resPhi;
        for (int i=0; i<nPairs; i++) {
            seEta.push_back({pd[i].seEtaPPb, pd[i].seEtaPbP});
            sePhi.push_back({pd[i].sePhiPPb, pd[i].sePhiPbP});
            meEta.push_back({pd[i].meEtaPPb, pd[i].meEtaPbP});
            mePhi.push_back({pd[i].mePhiPPb, pd[i].mePhiPbP});
            resEta.push_back({pd[i].resEtaPPb, pd[i].resEtaPbP});
            resPhi.push_back({pd[i].resPhiPPb, pd[i].resPhiPbP});
        }
        vector<string> ppbL = {pPbLabel1, pPbLabel2, pPbLabel3};
        vector<string> pbpL = {PbPLabel1, PbPLabel2, PbPLabel3};
        ppbL.resize(nPairs); pbpL.resize(nPairs);
        vector<int> ls(kLineStyles.begin(), kLineStyles.begin()+nPairs);

        makePairPlot(seEta,  ppbL, pbpL, ls, pPbColor, PbPColor,
            "#Delta y_{ch,Z}",    etaMin, etaMax, "Same-event d#LTN_{ch}#GT/d#Delta y_{ch,Z}",   "Ratio pPb/Pbp",        true,  savePath("SameEvent-DeltaEta"));
        makePairPlot(sePhi,  ppbL, pbpL, ls, pPbColor, PbPColor,
            "#Delta#phi_{ch,Z}",  phiMin, phiMax, "Same-event d#LTN_{ch}#GT/d#Delta#phi_{ch,Z}", "Ratio pPb/Pbp",        true,  savePath("SameEvent-DeltaPhi"));
        makePairPlot(meEta,  ppbL, pbpL, ls, pPbColor, PbPColor,
            "#Delta y_{ch,Z}",    etaMin, etaMax, "Mixed-event d#LTN_{ch}#GT/d#Delta y_{ch,Z}",  "Ratio pPb/Pbp",        true,  savePath("MixedEvent-DeltaEta"), false);
        makePairPlot(mePhi,  ppbL, pbpL, ls, pPbColor, PbPColor,
            "#Delta#phi_{ch,Z}",  phiMin, phiMax, "Mixed-event d#LTN_{ch}#GT/d#Delta#phi_{ch,Z}","Ratio pPb/Pbp",        true,  savePath("MixedEvent-DeltaPhi"), false);
        makePairPlot(resEta, ppbL, pbpL, ls, pPbColor, PbPColor,
            "#Delta y_{ch,Z}",    etaMin, etaMax, "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", "Diff pPb#minusPbp",    false, savePath("Result-DeltaEta"), true, 0.12);
        makePairPlot(resPhi, ppbL, pbpL, ls, pPbColor, PbPColor,
            "#Delta#phi_{ch,Z}",  phiMin, phiMax, "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}","Diff pPb#minusPbp",  false, savePath("Result-DeltaPhi"));
    }

    cout << "=== Done: " << outputDir << "/" << tag << "-*.pdf ===" << endl;
    return 0;
}
