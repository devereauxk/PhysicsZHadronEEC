#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TSystem.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

#include "KylesPlotting.h"
#include "CommandLine.h"

// ── Utility functions (same as plot_central_combined.cpp) ──────────────

struct ResultProjectionWindow {
    int DeltaPhiXFirst = 0, DeltaPhiXLast = 0;
    int DeltaEtaYFirst = 1, DeltaEtaYLast = 1;
};
void SetModified12x12ProjectionWindow(ResultProjectionWindow &w) {
    w.DeltaPhiXFirst = 7; w.DeltaPhiXLast = 12;
    w.DeltaEtaYFirst = 4; w.DeltaEtaYLast = 6;
}

void Symmetrize2DFourfold(TH2D *h) {
    if (!h || h->GetNbinsX() != 12 || h->GetNbinsY() != 12) return;
    TH2D *c = (TH2D*)h->Clone("_s2d"); c->SetDirectory(nullptr);
    for (int i = 1; i <= 12; ++i) {
        int mi = 13 - i;
        for (int j = 1; j <= 12; ++j) {
            int mj = (j <= 6) ? (7 - j) : (19 - j);
            h->SetBinContent(i, j, 0.25*(c->GetBinContent(i,j)+c->GetBinContent(mi,j)
                +c->GetBinContent(i,mj)+c->GetBinContent(mi,mj)));
            h->SetBinError(i, j, 0.25*sqrt(pow(c->GetBinError(i,j),2)+pow(c->GetBinError(mi,j),2)
                +pow(c->GetBinError(i,mj),2)+pow(c->GetBinError(mi,mj),2)));
        }
    }
    delete c;
}
void Symmetrize1DEta(TH1D *h) {
    if (!h) return;
    int n = h->GetNbinsX();
    for (int i = 0; i < n/2; ++i) {
        int mi = n-1-i;
        double v = 0.5*(h->GetBinContent(i+1)+h->GetBinContent(mi+1));
        double e = 0.5*sqrt(pow(h->GetBinError(i+1),2)+pow(h->GetBinError(mi+1),2));
        h->SetBinContent(i+1, v); h->SetBinError(i+1, e);
        h->SetBinContent(mi+1, v); h->SetBinError(mi+1, e);
    }
}
void Symmetrize1DPhi(TH1D *h) {
    if (!h || h->GetNbinsX() != 12) return;
    for (int j = 0; j < 3; ++j) {
        int mj = 5-j;
        double v = 0.5*(h->GetBinContent(j+1)+h->GetBinContent(mj+1));
        double e = 0.5*sqrt(pow(h->GetBinError(j+1),2)+pow(h->GetBinError(mj+1),2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
    for (int j = 6; j < 9; ++j) {
        int mj = 17-j;
        double v = 0.5*(h->GetBinContent(j+1)+h->GetBinContent(mj+1));
        double e = 0.5*sqrt(pow(h->GetBinError(j+1),2)+pow(h->GetBinError(mj+1),2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
}

TH1D *LoadSystHist(const string &fn, const string &hn, const string &cn) {
    TFile f(fn.c_str(), "READ");
    if (f.IsZombie()) return nullptr;
    TH1D *h = (TH1D*)f.Get(hn.c_str());
    if (!h) return nullptr;
    h = (TH1D*)h->Clone(cn.c_str());
    h->SetDirectory(nullptr);
    return h;
}

struct JKContrib {
    double SignalNZ = 0, MixNZ = 0;
    vector<float> SignalEta, MixEta, SignalPhi, MixPhi;
};

void AppendJK(TFile *f, const string &tree, vector<JKContrib> &out) {
    if (!f) return;
    TTree *t = (TTree*)f->Get(tree.c_str());
    if (!t) return;
    double sNZ = 0, mNZ = 0;
    vector<float> *sE = nullptr, *mE = nullptr, *sP = nullptr, *mP = nullptr;
    t->SetBranchAddress("SignalNZ", &sNZ); t->SetBranchAddress("MixNZ", &mNZ);
    t->SetBranchAddress("SignalEta", &sE); t->SetBranchAddress("MixEta", &mE);
    t->SetBranchAddress("SignalPhi", &sP); t->SetBranchAddress("MixPhi", &mP);
    for (Long64_t i = 0; i < t->GetEntries(); ++i) {
        t->GetEntry(i);
        out.push_back({sNZ, mNZ, *sE, *mE, *sP, *mP});
    }
}

vector<double> ComputeJKSigma(const vector<JKContrib> &ev, const TH1D *h, bool useEta) {
    int nb = h->GetNbinsX();
    vector<double> sigma(nb, 0);
    if (ev.size() < 2) return sigma;
    double tSNZ = 0, tMNZ = 0;
    vector<double> fS(nb, 0), fM(nb, 0);
    for (auto &e : ev) {
        tSNZ += e.SignalNZ; tMNZ += e.MixNZ;
        for (int i = 0; i < nb; ++i) {
            fS[i] += (useEta ? e.SignalEta[i] : e.SignalPhi[i]);
            fM[i] += (useEta ? e.MixEta[i] : e.MixPhi[i]);
        }
    }
    int valid = 0;
    for (auto &e : ev)
        if (tSNZ - e.SignalNZ > 0 && tMNZ - e.MixNZ > 0) valid++;
    if (valid < 2) return sigma;
    for (int i = 0; i < nb; ++i) {
        double full = fS[i]/tSNZ - fM[i]/tMNZ;
        double var = 0;
        for (auto &e : ev) {
            if (tSNZ - e.SignalNZ <= 0 || tMNZ - e.MixNZ <= 0) continue;
            double sv = (fS[i] - (useEta ? e.SignalEta[i] : e.SignalPhi[i])) / (tSNZ - e.SignalNZ);
            double mv = (fM[i] - (useEta ? e.MixEta[i] : e.MixPhi[i])) / (tMNZ - e.MixNZ);
            double d = sv - mv - full;
            var += d*d;
        }
        sigma[i] = sqrt((valid-1.0)/valid * var);
    }
    return sigma;
}

void ApplyJKErrors(TH1D *hEta, TH1D *hPhi, const vector<JKContrib> &ev) {
    if (ev.size() < 2) return;
    auto sE = ComputeJKSigma(ev, hEta, true);
    auto sP = ComputeJKSigma(ev, hPhi, false);
    for (int i = 1; i <= hEta->GetNbinsX(); ++i) hEta->SetBinError(i, sE[i-1]);
    for (int i = 1; i <= hPhi->GetNbinsX(); ++i) hPhi->SetBinError(i, sP[i-1]);
}

void ApplyCombinedJKErrors(TH1D *hEta, TH1D *hPhi,
    const vector<JKContrib> &evPPb, const vector<JKContrib> &evPbP,
    double NZ_pPb, double NZ_PbP) {
    double tot = NZ_pPb + NZ_PbP;
    double w1 = NZ_pPb/tot, w2 = NZ_PbP/tot;
    auto eE1 = ComputeJKSigma(evPPb, hEta, true);
    auto eP1 = ComputeJKSigma(evPPb, hPhi, false);
    auto eE2 = ComputeJKSigma(evPbP, hEta, true);
    auto eP2 = ComputeJKSigma(evPbP, hPhi, false);
    for (int i = 1; i <= hEta->GetNbinsX(); ++i)
        hEta->SetBinError(i, sqrt(w1*w1*eE1[i-1]*eE1[i-1] + w2*w2*eE2[i-1]*eE2[i-1]));
    for (int i = 1; i <= hPhi->GetNbinsX(); ++i)
        hPhi->SetBinError(i, sqrt(w1*w1*eP1[i-1]*eP1[i-1] + w2*w2*eP2[i-1]*eP2[i-1]));
}

// ── Data loading per track-pT bin ──────────────────────────────────────

struct PanelData {
    TH1D *hPhi_pp = nullptr, *hPhi_pPb = nullptr;
    TH1D *hEta_pp = nullptr, *hEta_pPb = nullptr;
    TH1D *hPhi_ppMC = nullptr, *hPhi_pPbMC = nullptr;
    TH1D *hEta_ppMC = nullptr, *hEta_pPbMC = nullptr;
    TH1D *topSystPhi_pp = nullptr, *topSystPhi_pPb = nullptr;
    TH1D *topSystEta_pp = nullptr, *topSystEta_pPb = nullptr;
    TH1D *diffSystPhi = nullptr, *diffSystEta = nullptr;
    string trkLabel;
};

PanelData LoadPanel(const string &trkPt, const string &zPt,
    const string &baseDir, const string &systDir,
    const string &ppTag, const string &pPbTag,
    const string &ppSystTag, const string &pPbSystTag,
    const string &ppMCPrefix = "", const string &ppMCTag = "",
    const string &pPbMCTag = "") {

    PanelData pd;
    size_t p = trkPt.find('_');
    string lo = trkPt.substr(0, p), hi = trkPt.substr(p+1);
    pd.trkLabel = Form("%s < p_{T}^{ch} < %s GeV", lo.c_str(), hi.c_str());

    // pp
    string ppFile = Form("%s/pp_trkResidual_%s_ZPT%s", baseDir.c_str(), ppTag.c_str(), zPt.c_str());
    TFile *fpp = TFile::Open(Form("%s-result.root", ppFile.c_str()), "READ");
    if (!fpp || fpp->IsZombie()) { cerr << "Cannot open pp " << ppFile << endl; return pd; }

    TH1D *phiPP = (TH1D*)fpp->Get(Form("DeltaPhi_Result%s", trkPt.c_str()));
    TH1D *etaPP = (TH1D*)fpp->Get(Form("DeltaEta_Result%s", trkPt.c_str()));
    if (!phiPP || !etaPP) { cerr << "Missing pp hists for " << trkPt << endl; return pd; }
    phiPP = (TH1D*)phiPP->Clone(Form("pp_phi_%s", trkPt.c_str())); phiPP->SetDirectory(nullptr);
    etaPP = (TH1D*)etaPP->Clone(Form("pp_eta_%s", trkPt.c_str())); etaPP->SetDirectory(nullptr);

    vector<JKContrib> jkPP;
    AppendJK(fpp, Form("JackknifeProjection%s", trkPt.c_str()), jkPP);
    if (jkPP.size() >= 2) {
        ApplyJKErrors(etaPP, phiPP, jkPP);
        for (int i = 1; i <= etaPP->GetNbinsX(); ++i) etaPP->SetBinError(i, etaPP->GetBinError(i)/etaPP->GetBinWidth(i));
        for (int i = 1; i <= phiPP->GetNbinsX(); ++i) phiPP->SetBinError(i, phiPP->GetBinError(i)/phiPP->GetBinWidth(i));
    }
    phiPP->Scale(0.5); etaPP->Scale(0.5);
    if (etaPP->GetNbinsX() == 12) { Symmetrize1DEta(etaPP); Symmetrize1DPhi(phiPP); }

    // pPb + PbP combine
    string pPbFile = Form("%s/pPb_trkResidual_%s_ZPT%s", baseDir.c_str(), pPbTag.c_str(), zPt.c_str());
    string PbPFile = Form("%s/PbP_trkResidual_%s_ZPT%s", baseDir.c_str(), pPbTag.c_str(), zPt.c_str());

    TFile *fpPb = TFile::Open(Form("%s-nosub.root", pPbFile.c_str()), "READ");
    TFile *fPbP = TFile::Open(Form("%s-nosub.root", PbPFile.c_str()), "READ");
    if (!fpPb || fpPb->IsZombie() || !fPbP || fPbP->IsZombie()) {
        cerr << "Cannot open pPb/PbP nosub for " << trkPt << endl; return pd;
    }

    auto load2D = [&](TFile *f, const string &prefix) {
        TH2D *hD = (TH2D*)((TH2D*)f->Get(Form("hData_%s", trkPt.c_str())))->Clone(Form("%s_D_%s", prefix.c_str(), trkPt.c_str()));
        TH2D *hM = (TH2D*)((TH2D*)f->Get(Form("hMixData_%s", trkPt.c_str())))->Clone(Form("%s_M_%s", prefix.c_str(), trkPt.c_str()));
        TH1D *nD = (TH1D*)((TH1D*)f->Get(Form("hNZData_%s", trkPt.c_str())))->Clone(Form("%s_nD_%s", prefix.c_str(), trkPt.c_str()));
        TH1D *nM = (TH1D*)((TH1D*)f->Get(Form("hNZMixData_%s", trkPt.c_str())))->Clone(Form("%s_nM_%s", prefix.c_str(), trkPt.c_str()));
        hD->SetDirectory(nullptr); hM->SetDirectory(nullptr);
        nD->SetDirectory(nullptr); nM->SetDirectory(nullptr);
        hD->Scale(nD->GetBinContent(1));
        hM->Scale(nM->GetBinContent(1));
        return make_tuple(hD, hM, nD, nM);
    };

    auto [dPPb, mPPb, nPPb, nmPPb] = load2D(fpPb, "pPb");
    auto [dPbP, mPbP, nPbP, nmPbP] = load2D(fPbP, "PbP");

    vector<JKContrib> jkPPb, jkPbP;
    AppendJK(fpPb, Form("JackknifeProjection%s", trkPt.c_str()), jkPPb);
    AppendJK(fPbP, Form("JackknifeProjection%s", trkPt.c_str()), jkPbP);

    TH2D *S = (TH2D*)dPPb->Clone(Form("S_%s", trkPt.c_str())); S->Add(dPbP);
    double SNZ = nPPb->GetBinContent(1) + nPbP->GetBinContent(1);
    S->Scale(1.0/SNZ);
    TH2D *B = (TH2D*)mPPb->Clone(Form("B_%s", trkPt.c_str())); B->Add(mPbP);
    double BNZ = nmPPb->GetBinContent(1) + nmPbP->GetBinContent(1);
    B->Scale(1.0/BNZ);
    S->Add(B, -1);
    Symmetrize2DFourfold(S);

    ResultProjectionWindow pw;
    SetModified12x12ProjectionWindow(pw);
    TH1D *projPhi = (TH1D*)S->ProjectionY(Form("projPhi_%s", trkPt.c_str()), pw.DeltaPhiXFirst, pw.DeltaPhiXLast);
    TH1D *projEta = (TH1D*)S->ProjectionX(Form("projEta_%s", trkPt.c_str()), pw.DeltaEtaYFirst, pw.DeltaEtaYLast);
    divideByWidth(projPhi); projPhi->Scale(0.5);
    divideByWidth(projEta); projEta->Scale(0.5);
    Symmetrize1DPhi(projPhi); Symmetrize1DEta(projEta);

    ApplyCombinedJKErrors(projEta, projPhi, jkPPb, jkPbP,
        nPPb->GetBinContent(1), nPbP->GetBinContent(1));

    pd.hPhi_pp = phiPP; pd.hPhi_pPb = projPhi;
    pd.hEta_pp = etaPP; pd.hEta_pPb = projEta;

    // Systematics
    string ppSystFile = Form("%s/pp_%s_ZPT%s_trkPT%s-systematics.root",
        systDir.c_str(), ppSystTag.c_str(), zPt.c_str(), trkPt.c_str());
    string pPbSystFile = Form("%s/pPbPbp_%s_ZPT%s_trkPT%s-systematics.root",
        systDir.c_str(), pPbSystTag.c_str(), zPt.c_str(), trkPt.c_str());

    pd.topSystPhi_pp = LoadSystHist(ppSystFile, "Total_DeltaPhi", Form("sPhi_pp_%s", trkPt.c_str()));
    pd.topSystEta_pp = LoadSystHist(ppSystFile, "Total_DeltaEta", Form("sEta_pp_%s", trkPt.c_str()));
    pd.topSystPhi_pPb = LoadSystHist(pPbSystFile, "Total_DeltaPhi", Form("sPhi_pPb_%s", trkPt.c_str()));
    pd.topSystEta_pPb = LoadSystHist(pPbSystFile, "Total_DeltaEta", Form("sEta_pPb_%s", trkPt.c_str()));
    pd.diffSystPhi = LoadSystHist(pPbSystFile, "DifferenceTotal_DeltaPhi", Form("dsPhi_%s", trkPt.c_str()));
    pd.diffSystEta = LoadSystHist(pPbSystFile, "DifferenceTotal_DeltaEta", Form("dsEta_%s", trkPt.c_str()));

    // pp MC (Pythia8+MadGraph EE-extrapolated)
    if (!ppMCPrefix.empty()) {
        string ppMCFile = Form("%s/%s_%s_ZPT%s-result.root",
            baseDir.c_str(), ppMCPrefix.c_str(), ppMCTag.c_str(), zPt.c_str());
        TFile *fppMC = TFile::Open(ppMCFile.c_str(), "READ");
        if (fppMC && !fppMC->IsZombie()) {
            TH1D *phiMC = (TH1D*)fppMC->Get(Form("DeltaPhi_Result%s", trkPt.c_str()));
            TH1D *etaMC = (TH1D*)fppMC->Get(Form("DeltaEta_Result%s", trkPt.c_str()));
            if (phiMC && etaMC) {
                phiMC = (TH1D*)phiMC->Clone(Form("ppMC_phi_%s_%s", zPt.c_str(), trkPt.c_str()));
                etaMC = (TH1D*)etaMC->Clone(Form("ppMC_eta_%s_%s", zPt.c_str(), trkPt.c_str()));
                phiMC->SetDirectory(nullptr); etaMC->SetDirectory(nullptr);
                phiMC->Scale(0.5); etaMC->Scale(0.5);
                if (etaMC->GetNbinsX() == 12) { Symmetrize1DEta(etaMC); Symmetrize1DPhi(phiMC); }
                pd.hPhi_ppMC = phiMC; pd.hEta_ppMC = etaMC;
            }
        }
    }

    // pPb+PbP MC (Powheg+EPOS combined)
    if (!pPbMCTag.empty()) {
        string pPbMCFile = Form("%s/pPbMC_Gen_nominal_%s_ZPT%s-nosub.root",
            baseDir.c_str(), pPbMCTag.c_str(), zPt.c_str());
        string PbPMCFile = Form("%s/PbPMC_Gen_nominal_%s_ZPT%s-nosub.root",
            baseDir.c_str(), pPbMCTag.c_str(), zPt.c_str());
        TFile *fMCpPb = TFile::Open(pPbMCFile.c_str(), "READ");
        TFile *fMCPbP = TFile::Open(PbPMCFile.c_str(), "READ");
        if (fMCpPb && !fMCpPb->IsZombie() && fMCPbP && !fMCPbP->IsZombie()) {
            auto loadMC2D = [&](TFile *f, const string &pre) {
                TH2D *hD = (TH2D*)((TH2D*)f->Get(Form("hData_%s", trkPt.c_str())))->Clone(Form("%s_mcD_%s_%s", pre.c_str(), zPt.c_str(), trkPt.c_str()));
                TH2D *hM = (TH2D*)((TH2D*)f->Get(Form("hMixData_%s", trkPt.c_str())))->Clone(Form("%s_mcM_%s_%s", pre.c_str(), zPt.c_str(), trkPt.c_str()));
                TH1D *nD = (TH1D*)((TH1D*)f->Get(Form("hNZData_%s", trkPt.c_str())))->Clone(Form("%s_mcnD_%s_%s", pre.c_str(), zPt.c_str(), trkPt.c_str()));
                TH1D *nM = (TH1D*)((TH1D*)f->Get(Form("hNZMixData_%s", trkPt.c_str())))->Clone(Form("%s_mcnM_%s_%s", pre.c_str(), zPt.c_str(), trkPt.c_str()));
                hD->SetDirectory(nullptr); hM->SetDirectory(nullptr);
                nD->SetDirectory(nullptr); nM->SetDirectory(nullptr);
                hD->Scale(nD->GetBinContent(1));
                hM->Scale(nM->GetBinContent(1));
                return make_tuple(hD, hM, nD, nM);
            };
            auto [mcDpPb, mcMpPb, mcNpPb, mcNMpPb] = loadMC2D(fMCpPb, "pPbMC");
            auto [mcDPbP, mcMPbP, mcNPbP, mcNMPbP] = loadMC2D(fMCPbP, "PbPMC");

            TH2D *mcS = (TH2D*)mcDpPb->Clone(Form("mcS_%s_%s", zPt.c_str(), trkPt.c_str()));
            mcS->Add(mcDPbP);
            double mcSNZ = mcNpPb->GetBinContent(1) + mcNPbP->GetBinContent(1);
            mcS->Scale(1.0/mcSNZ);
            TH2D *mcB = (TH2D*)mcMpPb->Clone(Form("mcB_%s_%s", zPt.c_str(), trkPt.c_str()));
            mcB->Add(mcMPbP);
            double mcBNZ = mcNMpPb->GetBinContent(1) + mcNMPbP->GetBinContent(1);
            mcB->Scale(1.0/mcBNZ);
            mcS->Add(mcB, -1);
            Symmetrize2DFourfold(mcS);

            ResultProjectionWindow mcpw;
            SetModified12x12ProjectionWindow(mcpw);
            TH1D *mcProjPhi = (TH1D*)mcS->ProjectionY(Form("mcProjPhi_%s_%s", zPt.c_str(), trkPt.c_str()), mcpw.DeltaPhiXFirst, mcpw.DeltaPhiXLast);
            TH1D *mcProjEta = (TH1D*)mcS->ProjectionX(Form("mcProjEta_%s_%s", zPt.c_str(), trkPt.c_str()), mcpw.DeltaEtaYFirst, mcpw.DeltaEtaYLast);
            divideByWidth(mcProjPhi); mcProjPhi->Scale(0.5);
            divideByWidth(mcProjEta); mcProjEta->Scale(0.5);
            Symmetrize1DPhi(mcProjPhi); Symmetrize1DEta(mcProjEta);
            pd.hPhi_pPbMC = mcProjPhi; pd.hEta_pPbMC = mcProjEta;
        }
    }

    return pd;
}

// ── Main ───────────────────────────────────────────────────────────────

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);
    string ppTag   = CL.Get("pptag");
    string pPbTag  = CL.Get("pPbtag");
    string ppSystTag  = CL.Get("ppSystematicsTag", ppTag);
    string pPbSystTag = CL.Get("pPbSystematicsTag", pPbTag);
    string baseDir = CL.Get("BaseDir");
    string systDir = CL.Get("systematicsDir",
        "/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output");
    string outputFile = CL.Get("output", "plots/composite/composite_deltaphi.pdf");
    bool doEta = CL.GetBool("doEta", false);
    bool includeMC = CL.GetBool("includeMC", true);
    string ppMCPrefix = CL.Get("ppMCPrefix", "pythiaMC_Gen_EExtrapolation");
    string ppMCTag = CL.Get("ppMCTag", ppTag);
    string pPbMCTag = CL.Get("pPbMCTag", pPbTag);

    gSystem->mkdir(gSystem->DirName(outputFile.c_str()), true);

    string trkBins[] = {"0.5_2", "2_4", "4_15"};
    string zPtBins[] = {"0_500", "0_30", "30_500"};
    string zPtLabels[] = {"inclusive p_{T}^{Z}", "p_{T}^{Z} < 30 GeV", "p_{T}^{Z} > 30 GeV"};
    int nRows = 3, nCols = 3;

    PanelData panels[3][3]; // [row][col]
    for (int r = 0; r < nRows; r++) {
        for (int col = 0; col < nCols; col++) {
            cout << "Loading ZPT " << zPtBins[r] << " trkPT " << trkBins[col] << "..." << endl;
            panels[r][col] = LoadPanel(trkBins[col], zPtBins[r], baseDir, systDir,
                ppTag, pPbTag, ppSystTag, pPbSystTag,
                includeMC ? ppMCPrefix : "",
                includeMC ? ppMCTag : "",
                includeMC ? pPbMCTag : "");
        }
    }

    const char *xTitle, *yTitle;
    double xmin, xmax, sigLo, sigHi;
    double yHeadroom;
    if (doEta) {
        xTitle = "#Delta y_{ch,Z}";
        yTitle = "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}";
        xmin = -3.87; xmax = 3.87; sigLo = 0; sigHi = 4;
        yHeadroom = 3.0;
    } else {
        xTitle = "#Delta#varphi_{ch,Z}";
        yTitle = "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}";
        xmin = -M_PI/2; xmax = 3*M_PI/2; sigLo = 0; sigHi = M_PI;
        yHeadroom = 1.0;
    }

    bool hasPPMC = includeMC && panels[0][0].hPhi_ppMC;
    bool hasPPbMC = includeMC && panels[0][0].hPhi_pPbMC;

    string scPythia = "P#scale[0.8]{YTHIA}8";
    string scMadGraph = "M#scale[0.8]{AD}G#scale[0.8]{RAPH}";
    string scPowheg = "P#scale[0.8]{OWHEG}";
    string scEpos = "E#scale[0.8]{POS}";
    string ppMCLabel = "pp " + scPythia + "+" + scMadGraph;
    string pPbMCLabel = "pPb " + scPowheg + "+" + scEpos;

    vector<string> labels = {"pp (extrap. 8.16 TeV)", "pPb (8.16 TeV)"};
    vector<int> markerColors = {cmsBlue, cmsRed};
    vector<int> markerStyles = {mSquareFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed};
    vector<int> lineStyles = {0, 0};
    if (hasPPMC) {
        labels.push_back(ppMCLabel);
        markerColors.push_back(kGray+3); markerStyles.push_back(0);
        lineColors.push_back(kGray+3); lineStyles.push_back(12);
    }
    if (hasPPbMC) {
        labels.push_back(pPbMCLabel);
        markerColors.push_back(cmsYellow); markerStyles.push_back(0);
        lineColors.push_back(cmsYellow); lineStyles.push_back(11);
    }

    float resultTextScale = 1.3;
    int panelModes[] = {1, 2, 3};
    float centerLeftMargin = 0.13;

    double areaLeft = 0.98 * (1 - 0.195 - 0.05);
    double areaCenter = 0.98 * (1 - centerLeftMargin - 0.05);
    double overlap = 0.04;
    double wLeft = (1.0 + 2*overlap) / (1.0 + 2.0 * areaLeft / areaCenter);
    double wCenter = wLeft * areaLeft / areaCenter;

    gStyle->SetLineScalePS(1);

    double outerLeftFrac = 0.04;
    double outerTopFrac  = 0.03;

    double topRowBorderT = 0.02;
    double otherRowBorderT = 0.003;
    double singleRowH = 600.0;
    double topRowPx = singleRowH;
    double otherRowPx = singleRowH * (1.0 - topRowBorderT + otherRowBorderT);
    int gridH = (int)(topRowPx + (nRows-1) * otherRowPx);
    int canvasH = (int)(gridH / (1.0 - outerTopFrac));
    int canvasW = (int)(1440 / (1.0 - outerLeftFrac));
    TCanvas *c = new TCanvas("c", "c", canvasW, canvasH);

    double totalUsed = wLeft + 2*wCenter - 2*overlap;
    double scale = 1.0 / totalUsed;
    double sLeft = wLeft * scale;
    double sCenter = wCenter * scale;
    double sOverlap = overlap * scale;

    double gx0[] = {0, sLeft - sOverlap, sLeft + sCenter - 2*sOverlap};
    double gx1[] = {sLeft, sLeft + sCenter - sOverlap, sLeft + 2*sCenter - 2*sOverlap};
    for (int i = 0; i < 3; ++i) {
        gx0[i] = outerLeftFrac + gx0[i] * (1.0 - outerLeftFrac);
        gx1[i] = outerLeftFrac + gx1[i] * (1.0 - outerLeftFrac);
    }

    double topFrac = topRowPx / canvasH;
    double otherFrac = otherRowPx / canvasH;
    double gridTop = 1.0 - outerTopFrac;
    TPad *subPads[3][3];
    TPad *plotPads[3][3];

    for (int r = 0; r < nRows; r++) {
        double yhi = (r == 0) ? gridTop : gridTop - topFrac - (r-1)*otherFrac;
        double ylo = max(0.0, yhi - ((r == 0) ? topFrac : otherFrac));

        for (int col = 0; col < nCols; col++) {
            c->cd();
            subPads[r][col] = new TPad(Form("sub%d_%d", r, col), "",
                gx0[col], ylo, gx1[col], yhi);
            subPads[r][col]->SetFillColor(kWhite);
            subPads[r][col]->Draw();
            subPads[r][col]->cd();

            PanelData &pd = panels[r][col];
            vector<TH1*> hists, topSyst, diffSyst;
            if (doEta) {
                hists = {(TH1*)pd.hEta_pp, (TH1*)pd.hEta_pPb};
                topSyst = {(TH1*)pd.topSystEta_pp, (TH1*)pd.topSystEta_pPb};
                diffSyst = {nullptr, (TH1*)pd.diffSystEta};
                if (hasPPMC) { hists.push_back((TH1*)pd.hEta_ppMC); topSyst.push_back(nullptr); diffSyst.push_back(nullptr); }
                if (hasPPbMC) { hists.push_back((TH1*)pd.hEta_pPbMC); topSyst.push_back(nullptr); diffSyst.push_back(nullptr); }
            } else {
                hists = {(TH1*)pd.hPhi_pp, (TH1*)pd.hPhi_pPb};
                topSyst = {(TH1*)pd.topSystPhi_pp, (TH1*)pd.topSystPhi_pPb};
                diffSyst = {nullptr, (TH1*)pd.diffSystPhi};
                if (hasPPMC) { hists.push_back((TH1*)pd.hPhi_ppMC); topSyst.push_back(nullptr); diffSyst.push_back(nullptr); }
                if (hasPPbMC) { hists.push_back((TH1*)pd.hPhi_pPbMC); topSyst.push_back(nullptr); diffSyst.push_back(nullptr); }
            }

            int pm = panelModes[col];

            double bTop = (r == 0) ? -1 : otherRowBorderT;
            bool legend = (r == 0 && col == 0 && !doEta);
            plotPads[r][col] = (TPad*)PlotCMSDiffResultRegion(
                hists, topSyst, diffSyst, Form("p%d_%d", r, col),
                labels,
                lineColors, lineStyles,
                markerColors, markerStyles,
                xTitle, xmin, xmax,
                yTitle, -1, -1,
                "pPb #minus pp", -1, -1,
                sigLo, sigHi,
                0,
                false, false, true,
                0.23, resultTextScale, 0.40, yHeadroom,
                pm, 0.02, bTop, legend
            );

            if (col > 0) {
                plotPads[r][col]->SetLeftMargin(centerLeftMargin);
                plotPads[r][col]->Modified();
                TIter nextSub(subPads[r][col]->GetListOfPrimitives());
                TObject *obj;
                while ((obj = nextSub())) {
                    if (obj->InheritsFrom("TPad") && obj != plotPads[r][col]) {
                        ((TPad*)obj)->SetLeftMargin(centerLeftMargin);
                        ((TPad*)obj)->Modified();
                        break;
                    }
                }
            }

            plotPads[r][col]->cd();

            double rowScale = (r == 0) ? 1.0 : topRowPx / otherRowPx;
            {
                size_t pos = trkBins[col].find('_');
                string trkLo = trkBins[col].substr(0, pos), trkHi = trkBins[col].substr(pos+1);
                TLatex latexR;
                latexR.SetNDC();
                latexR.SetTextFont(42);
                latexR.SetTextAlign(31);
                latexR.SetTextSize(0.045 * resultTextScale * rowScale);
                float rMargin = plotPads[r][col]->GetRightMargin();
                latexR.DrawLatex(1 - rMargin - 0.06, 0.80,
                    Form("%s < p_{T}^{ch} < %s GeV", trkLo.c_str(), trkHi.c_str()));
            }
        }
    }

    float headerSize = plotTextSize * 1.4;

    // CMS header on top-left panel only
    plotPads[0][0]->cd();
    float t = plotPads[0][0]->GetTopMargin();
    float l = plotPads[0][0]->GetLeftMargin();
    TLatex cmsLatex;
    cmsLatex.SetNDC();
    cmsLatex.SetTextFont(42);
    cmsLatex.SetTextAlign(11);
    cmsLatex.SetTextSize(headerSize);
    cmsLatex.DrawLatex(l, 1 - t + 0.015,
        "#font[61]{#scale[1.25]{CMS}} #font[52]{Preliminary}");

    // Lumi header on top-right panel only
    plotPads[0][2]->cd();
    float t2 = plotPads[0][2]->GetTopMargin();
    float r2 = plotPads[0][2]->GetRightMargin();
    TLatex lumiLatex;
    lumiLatex.SetNDC();
    lumiLatex.SetTextFont(42);
    lumiLatex.SetTextAlign(31);
    lumiLatex.SetTextSize(headerSize);
    lumiLatex.DrawLatex(1 - r2, 1 - t2 + 0.015,
        "pPb (pp) 8.16 TeV  174 nb^{-1} (301 pb^{-1})");

    // Reflected marker legend helpers
    auto makeReflLegend = [&](TPad *pad, float x1, float y1, float x2, float y2) {
        pad->cd();
        TLegend *leg = new TLegend(x1, y1, x2, y2);
        leg->SetBorderSize(0); leg->SetFillStyle(0);
        leg->SetTextFont(42); leg->SetTextSize(0.034 * resultTextScale);
        TGraph *gPP = new TGraph(1);
        gPP->SetMarkerColor(cmsBlue); gPP->SetMarkerStyle(25);
        gPP->SetMarkerSize(resultTextScale); gPP->SetLineColor(cmsBlue);
        leg->AddEntry(gPP, "pp reflected", "p");
        TGraph *gPPb = new TGraph(1);
        gPPb->SetMarkerColor(cmsRed); gPPb->SetMarkerStyle(24);
        gPPb->SetMarkerSize(resultTextScale); gPPb->SetLineColor(cmsRed);
        leg->AddEntry(gPPb, "pPb reflected", "p");
        leg->Draw("SAME");
    };

    if (doEta) {
        // Data + reflected legend on top-row middle, two columns
        float legLM = plotPads[0][1]->GetLeftMargin();
        float legX = legLM + 0.02;
        float legY = 0.74;
        plotPads[0][1]->cd();
        TLegend *legData = new TLegend(legX, legY - 0.14, legX + 0.42, legY);
        legData->SetBorderSize(0); legData->SetFillStyle(0);
        legData->SetTextFont(42); legData->SetTextSize(0.034 * resultTextScale);
        legData->SetMargin(0.20);
        TGraph *gPPd = new TGraph(1);
        gPPd->SetMarkerColor(cmsBlue); gPPd->SetMarkerStyle(mSquareFill);
        gPPd->SetMarkerSize(resultTextScale); gPPd->SetLineColor(cmsBlue);
        legData->AddEntry(gPPd, "pp (extrap. 8.16 TeV)", "p");
        TGraph *gPPbd = new TGraph(1);
        gPPbd->SetMarkerColor(cmsRed); gPPbd->SetMarkerStyle(mCircleFill);
        gPPbd->SetMarkerSize(resultTextScale); gPPbd->SetLineColor(cmsRed);
        legData->AddEntry(gPPbd, "pPb (8.16 TeV)", "p");
        legData->Draw("SAME");

        // Reflected legend on top-row middle, right column beside data
        TLegend *legRefl = new TLegend(legX + 0.43, legY - 0.14, legX + 0.80, legY);
        legRefl->SetBorderSize(0); legRefl->SetFillStyle(0);
        legRefl->SetTextFont(42); legRefl->SetTextSize(0.034 * resultTextScale);
        legRefl->SetMargin(0.20);
        TGraph *gPPr = new TGraph(1);
        gPPr->SetMarkerColor(cmsBlue); gPPr->SetMarkerStyle(25);
        gPPr->SetMarkerSize(resultTextScale); gPPr->SetLineColor(cmsBlue);
        legRefl->AddEntry(gPPr, "pp reflected", "p");
        TGraph *gPPbr = new TGraph(1);
        gPPbr->SetMarkerColor(cmsRed); gPPbr->SetMarkerStyle(24);
        gPPbr->SetMarkerSize(resultTextScale); gPPbr->SetLineColor(cmsRed);
        legRefl->AddEntry(gPPbr, "pPb reflected", "p");
        legRefl->Draw("SAME");

        // MC legend on top-row right, same vertical position, short line icons
        float legLM2 = plotPads[0][2]->GetLeftMargin();
        float legX2 = legLM2 + 0.05;
        plotPads[0][2]->cd();
        TLegend *legMC = new TLegend(legX2, legY - 0.14, legX2 + 0.55, legY);
        legMC->SetBorderSize(0); legMC->SetFillStyle(0);
        legMC->SetTextFont(42); legMC->SetTextSize(0.034 * resultTextScale);
        legMC->SetMargin(0.15);
        if (hasPPMC) {
            TGraph *gMC1 = new TGraph(1);
            gMC1->SetLineColor(kGray+3); gMC1->SetLineStyle(2); gMC1->SetLineWidth(2);
            legMC->AddEntry(gMC1, ppMCLabel.c_str(), "l");
        }
        if (hasPPbMC) {
            TGraph *gMC2 = new TGraph(1);
            gMC2->SetLineColor(cmsYellow); gMC2->SetLineStyle(1); gMC2->SetLineWidth(2);
            legMC->AddEntry(gMC2, pPbMCLabel.c_str(), "l");
        }
        legMC->Draw("SAME");
    } else {
        // DeltaPhi: reflected legend on top-row middle, same position as main legend
        makeReflLegend(plotPads[0][1], 0.18, 0.44, 0.50, 0.59);
    }

    // pTZ label on all panels per row; yCM line only on left column of top row
    for (int r = 0; r < nRows; r++) {
        for (int col = 0; col < nCols; col++) {
            plotPads[r][col]->cd();
            TLatex latex;
            latex.SetNDC();
            latex.SetTextFont(42);
            latex.SetTextAlign(11);
            double kinRowScale = (r == 0) ? 1.0 : topRowPx / otherRowPx;
            latex.SetTextSize(0.045 * resultTextScale * kinRowScale);
            float lm = plotPads[r][col]->GetLeftMargin();
            float labelY = 0.80;
            latex.DrawLatex(lm + 0.05, labelY, zPtLabels[r].c_str());
            if (r == 0 && col == 0) {
                labelY -= 0.08;
                if (doEta) {
                    latex.DrawLatex(lm + 0.05, labelY, "|y_{CM}| < 1.935");
                    labelY -= 0.08;
                    latex.DrawLatex(lm + 0.05, labelY, "|#Delta#varphi_{ch,Z}| < #frac{#pi}{2}");
                } else
                    latex.DrawLatex(lm + 0.05, labelY, "|y_{CM}| < 1.935");
            }
        }
    }

    // Outer column labels (track pT) above the grid
    {
        string colLabels[] = {
            "0.5 < p_{T}^{ch} < 2 GeV",
            "2 < p_{T}^{ch} < 4 GeV",
            "4 < p_{T}^{ch} < 15 GeV"
        };
        c->cd();
        TLatex colTex;
        colTex.SetNDC();
        colTex.SetTextFont(42);
        colTex.SetTextAlign(21);
        colTex.SetTextSize(0.022);
        for (int col = 0; col < nCols; col++) {
            double cx = 0.5 * (gx0[col] + gx1[col]);
            colTex.DrawLatex(cx, gridTop + 0.008, colLabels[col].c_str());
        }
    }

    // Outer row labels (Z pT) to the left of the grid, rotated 90 degrees
    {
        string rowLabels[] = {
            "inclusive p_{T}^{Z}",
            "p_{T}^{Z} < 30 GeV",
            "p_{T}^{Z} > 30 GeV"
        };
        c->cd();
        TLatex rowTex;
        rowTex.SetNDC();
        rowTex.SetTextFont(42);
        rowTex.SetTextAlign(22);
        rowTex.SetTextAngle(90);
        rowTex.SetTextSize(0.022);
        for (int r = 0; r < nRows; r++) {
            double ryhi = (r == 0) ? gridTop : gridTop - topFrac - (r-1)*otherFrac;
            double rylo = max(0.0, ryhi - ((r == 0) ? topFrac : otherFrac));
            double cy = 0.5 * (rylo + ryhi);
            rowTex.DrawLatex(outerLeftFrac * 0.4, cy, rowLabels[r].c_str());
        }
    }

    c->Update();
    c->SaveAs(outputFile.c_str());
    cout << "Saved: " << outputFile << endl;
    return 0;
}
