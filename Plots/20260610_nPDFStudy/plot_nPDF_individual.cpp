#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TPad.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"

#include "CommandLine.h"
#include "SetStyle.h"

void Symmetrize1DEta(TH1D *h) {
    int n = h->GetNbinsX();
    for (int i = 0; i < n / 2; ++i) {
        int mi = n - 1 - i;
        double v = 0.5 * (h->GetBinContent(i+1) + h->GetBinContent(mi+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(i+1),2) + pow(h->GetBinError(mi+1),2));
        h->SetBinContent(i+1, v); h->SetBinError(i+1, e);
        h->SetBinContent(mi+1, v); h->SetBinError(mi+1, e);
    }
}

void Symmetrize1DPhi(TH1D *h) {
    int n = h->GetNbinsX();
    if (n != 12) return;
    for (int j = 0; j < 3; ++j) {
        int mj = 5 - j;
        double v = 0.5 * (h->GetBinContent(j+1) + h->GetBinContent(mj+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j+1),2) + pow(h->GetBinError(mj+1),2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
    for (int j = 6; j < 9; ++j) {
        int mj = 17 - j;
        double v = 0.5 * (h->GetBinContent(j+1) + h->GetBinContent(mj+1));
        double e = 0.5 * sqrt(pow(h->GetBinError(j+1),2) + pow(h->GetBinError(mj+1),2));
        h->SetBinContent(j+1, v); h->SetBinError(j+1, e);
        h->SetBinContent(mj+1, v); h->SetBinError(mj+1, e);
    }
}

void ApplyDiagonalJackknifeErrors(TFile *file, const string &trkPtRange,
    TH1D *hEta, TH1D *hPhi)
{
    if (!file) return;
    TTree *tree = (TTree *)file->Get(Form("JackknifeProjection%s", trkPtRange.c_str()));
    if (!tree) return;

    double signalNZ = 0, mixNZ = 0;
    vector<float> *signalEta = nullptr, *mixEta = nullptr;
    vector<float> *signalPhi = nullptr, *mixPhi = nullptr;
    tree->SetBranchAddress("SignalNZ", &signalNZ);
    tree->SetBranchAddress("MixNZ", &mixNZ);
    tree->SetBranchAddress("SignalEta", &signalEta);
    tree->SetBranchAddress("MixEta", &mixEta);
    tree->SetBranchAddress("SignalPhi", &signalPhi);
    tree->SetBranchAddress("MixPhi", &mixPhi);

    Long64_t n = tree->GetEntries();
    if (n < 2) return;

    int nBins = hEta->GetNbinsX();
    double totalSNZ = 0, totalMNZ = 0;
    vector<double> fullSEta(nBins, 0), fullMEta(nBins, 0);
    vector<double> fullSPhi(nBins, 0), fullMPhi(nBins, 0);

    struct Entry { double sNZ, mNZ; vector<float> sEta, mEta, sPhi, mPhi; };
    vector<Entry> entries(n);
    for (Long64_t i = 0; i < n; i++) {
        tree->GetEntry(i);
        entries[i] = {signalNZ, mixNZ, *signalEta, *mixEta, *signalPhi, *mixPhi};
        totalSNZ += signalNZ;
        totalMNZ += mixNZ;
        for (int b = 0; b < nBins; b++) {
            fullSEta[b] += (*signalEta)[b]; fullMEta[b] += (*mixEta)[b];
            fullSPhi[b] += (*signalPhi)[b]; fullMPhi[b] += (*mixPhi)[b];
        }
    }

    int validN = 0;
    for (auto &e : entries)
        if (totalSNZ - e.sNZ > 0 && totalMNZ - e.mNZ > 0) validN++;
    if (validN < 2) return;

    for (int b = 0; b < nBins; b++) {
        double fullEta = fullSEta[b] / totalSNZ - fullMEta[b] / totalMNZ;
        double fullPhi = fullSPhi[b] / totalSNZ - fullMPhi[b] / totalMNZ;
        double varEta = 0, varPhi = 0;
        for (auto &e : entries) {
            if (totalSNZ - e.sNZ <= 0 || totalMNZ - e.mNZ <= 0) continue;
            double dEta = (fullSEta[b] - e.sEta[b]) / (totalSNZ - e.sNZ)
                        - (fullMEta[b] - e.mEta[b]) / (totalMNZ - e.mNZ) - fullEta;
            double dPhi = (fullSPhi[b] - e.sPhi[b]) / (totalSNZ - e.sNZ)
                        - (fullMPhi[b] - e.mPhi[b]) / (totalMNZ - e.mNZ) - fullPhi;
            varEta += dEta * dEta;
            varPhi += dPhi * dPhi;
        }
        hEta->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varEta));
        hPhi->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varPhi));
    }
}

TH1D* LoadResultHist(const string &path, const string &obs, const string &trkRange, const string &tag) {
    TFile *f = TFile::Open(path.c_str());
    if (!f || f->IsZombie()) { cerr << "Cannot open " << path << endl; return nullptr; }
    string histName = obs + "_Result" + trkRange;
    TH1D *h = (TH1D *)f->Get(histName.c_str());
    if (!h) { cerr << "Missing " << histName << " in " << path << endl; return nullptr; }
    h = (TH1D *)h->Clone(tag.c_str());
    h->SetDirectory(nullptr);
    return h;
}

double GetNZ(TFile *f, const string &trkRange) {
    TH1D *h = (TH1D *)f->Get(Form("hNZData_%s", trkRange.c_str()));
    if (!h) return 0;
    return h->GetBinContent(1);
}

string FormatPTRange(const string &range, const string &symbol) {
    size_t sep = range.find('_');
    string lo = range.substr(0, sep);
    string hi = range.substr(sep + 1);
    return lo + " < " + symbol + " < " + hi + " GeV";
}

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    gROOT->SetBatch(kTRUE);
    SetThesisStyle();
    gStyle->SetOptStat(0);

    string inputDir    = CL.Get("InputDir");
    string tag         = CL.Get("Tag", "nPDF_study");
    string outputBase  = CL.Get("OutputBase", "plots/nPDF/nPDF");
    string zptRange    = CL.Get("ZPTRange", "0_500");
    string trkRange    = CL.Get("TrackPTRange", "0.5_15");
    string ppFile      = CL.Get("PPFile", "");
    string pPbFile     = CL.Get("PPbFile", "");
    string PbPFile     = CL.Get("PbPFile", "");

    string zptTag = "ZPT" + zptRange;
    bool hasTrkInName = (zptRange != "0_500" || trkRange != "0.5_15");

    auto buildPath = [&](const string &pdf) -> string {
        if (hasTrkInName)
            return inputDir + "/" + pdf + "_" + tag + "_" + zptTag + "_trkPT" + trkRange + "-result.root";
        return inputDir + "/" + pdf + "_" + tag + "_" + zptTag + "-result.root";
    };

    bool hasData = !ppFile.empty() && !pPbFile.empty() && !PbPFile.empty();

    for (const string &obs : {"DeltaEta", "DeltaPhi"}) {
        // Load MC nPDF histograms
        TH1D *hC = LoadResultHist(buildPath("nPDF_ct18anlo"), obs, trkRange, "hC_" + obs);
        TH1D *hE = LoadResultHist(buildPath("nPDF_epps21"),   obs, trkRange, "hE_" + obs);
        TH1D *hN = LoadResultHist(buildPath("nPDF_nnpdf31"),  obs, trkRange, "hN_" + obs);
        if (!hC || !hE || !hN) continue;

        hC->Scale(0.5); hE->Scale(0.5); hN->Scale(0.5);
        if (obs == "DeltaEta") { Symmetrize1DEta(hC); Symmetrize1DEta(hE); Symmetrize1DEta(hN); }
        if (obs == "DeltaPhi") { Symmetrize1DPhi(hC); Symmetrize1DPhi(hE); Symmetrize1DPhi(hN); }

        // Load data histograms if provided
        TH1D *hPP = nullptr, *hHI = nullptr;
        TH1D *hDiff = nullptr; // pPb - pp
        if (hasData) {
            TFile *fPP  = TFile::Open(ppFile.c_str());
            TFile *fPPb = TFile::Open(pPbFile.c_str());
            TFile *fPbP = TFile::Open(PbPFile.c_str());

            string histName = obs + "_Result" + trkRange;

            // pp
            TH1D *hPPraw = (TH1D *)fPP->Get(histName.c_str());
            hPP = (TH1D *)hPPraw->Clone(("hPP_" + obs).c_str());
            hPP->SetDirectory(nullptr);
            hPP->Scale(0.5);

            // pp JK errors
            TH1D *hPP_eta_dummy = (obs == "DeltaEta") ? hPP : (TH1D*)hPP->Clone("dum_eta_pp");
            TH1D *hPP_phi_dummy = (obs == "DeltaPhi") ? hPP : (TH1D*)hPP->Clone("dum_phi_pp");
            hPP_eta_dummy->SetDirectory(nullptr);
            hPP_phi_dummy->SetDirectory(nullptr);
            ApplyDiagonalJackknifeErrors(fPP, trkRange, hPP_eta_dummy, hPP_phi_dummy);
            for (int b = 1; b <= hPP->GetNbinsX(); b++)
                hPP->SetBinError(b, hPP->GetBinError(b) / hPP->GetBinWidth(b) * 0.5);

            if (obs == "DeltaEta") Symmetrize1DEta(hPP);
            if (obs == "DeltaPhi") Symmetrize1DPhi(hPP);

            // pPb + Pbp 2D combination (matches note pipeline)
            // PPbFile/PbPFile point to nosub files with 2D hData/hMixData
            string hDataName = "hData_" + trkRange;
            string hMixName  = "hMixData_" + trkRange;
            string hNZName   = "hNZData_" + trkRange;
            string hNZMName  = "hNZMixData_" + trkRange;

            TH2D *sppb = (TH2D *)fPPb->Get(hDataName.c_str());
            TH2D *mppb = (TH2D *)fPPb->Get(hMixName.c_str());
            TH1D *nzs_ppb = (TH1D *)fPPb->Get(hNZName.c_str());
            TH1D *nzm_ppb = (TH1D *)fPPb->Get(hNZMName.c_str());
            TH2D *spbp = (TH2D *)fPbP->Get(hDataName.c_str());
            TH2D *mpbp = (TH2D *)fPbP->Get(hMixName.c_str());
            TH1D *nzs_pbp = (TH1D *)fPbP->Get(hNZName.c_str());
            TH1D *nzm_pbp = (TH1D *)fPbP->Get(hNZMName.c_str());

            double nzPPb = nzs_ppb->GetBinContent(1);
            double nzPbP = nzs_pbp->GetBinContent(1);
            double nzTotal = nzPPb + nzPbP;
            double wPPb = nzPPb / nzTotal;
            double wPbP = nzPbP / nzTotal;

            // Undo per-system NZ, sum, renormalize
            TH2D *S2D = (TH2D *)sppb->Clone("S2D");
            S2D->Scale(nzPPb);
            TH2D *stmp = (TH2D *)spbp->Clone("stmp");
            stmp->Scale(nzPbP);
            S2D->Add(stmp);
            S2D->Scale(1.0 / nzTotal);

            double nzmPPb = nzm_ppb->GetBinContent(1);
            double nzmPbP = nzm_pbp->GetBinContent(1);
            TH2D *M2D = (TH2D *)mppb->Clone("M2D");
            M2D->Scale(nzmPPb);
            TH2D *mtmp = (TH2D *)mpbp->Clone("mtmp");
            mtmp->Scale(nzmPbP);
            M2D->Add(mtmp);
            M2D->Scale(1.0 / (nzmPPb + nzmPbP));

            S2D->Add(M2D, -1);

            // 2D fourfold symmetrization
            TH2D *sym = (TH2D *)S2D->Clone("sym");
            for (int i = 1; i <= 12; i++) {
                int mi = 13 - i;
                for (int j = 1; j <= 12; j++) {
                    int mj = (j <= 6) ? (7 - j) : (19 - j);
                    double v = 0.25 * (sym->GetBinContent(i,j) + sym->GetBinContent(mi,j)
                                     + sym->GetBinContent(i,mj) + sym->GetBinContent(mi,mj));
                    S2D->SetBinContent(i,j, v);
                }
            }

            // Project to 1D
            if (obs == "DeltaEta") {
                hHI = S2D->ProjectionX("hHI_DeltaEta", 4, 6); // DeltaPhi in [0, pi/2]
            } else {
                hHI = (TH1D *)S2D->ProjectionY("hHI_DeltaPhi", 7, 12); // DeltaEta >= 0
            }
            hHI->SetDirectory(nullptr);
            for (int b = 1; b <= hHI->GetNbinsX(); b++)
                hHI->SetBinContent(b, hHI->GetBinContent(b) / hHI->GetBinWidth(b));
            hHI->Scale(0.5);

            // JK errors: compute from individual system trees, combine with NZ weights
            TH1D *hPPb_eta = (TH1D *)hHI->Clone("ppb_eta_jk"); hPPb_eta->SetDirectory(nullptr);
            TH1D *hPPb_phi = (TH1D *)hHI->Clone("ppb_phi_jk"); hPPb_phi->SetDirectory(nullptr);
            TH1D *hPbP_eta = (TH1D *)hHI->Clone("pbp_eta_jk"); hPbP_eta->SetDirectory(nullptr);
            TH1D *hPbP_phi = (TH1D *)hHI->Clone("pbp_phi_jk"); hPbP_phi->SetDirectory(nullptr);
            ApplyDiagonalJackknifeErrors(fPPb, trkRange, hPPb_eta, hPPb_phi);
            ApplyDiagonalJackknifeErrors(fPbP, trkRange, hPbP_eta, hPbP_phi);

            TH1D *hSrc_ppb = (obs == "DeltaEta") ? hPPb_eta : hPPb_phi;
            TH1D *hSrc_pbp = (obs == "DeltaEta") ? hPbP_eta : hPbP_phi;
            for (int b = 1; b <= hHI->GetNbinsX(); b++) {
                double bw = hHI->GetBinWidth(b);
                double ePPb = hSrc_ppb->GetBinError(b) / bw;
                double ePbP = hSrc_pbp->GetBinError(b) / bw;
                double eComb = sqrt(wPPb*wPPb * ePPb*ePPb + wPbP*wPbP * ePbP*ePbP);
                hHI->SetBinError(b, eComb * 0.5);
            }

            delete stmp; delete mtmp; delete sym; delete M2D;

            // pPb - pp difference
            hDiff = (TH1D *)hHI->Clone(("hDiff_" + obs).c_str());
            hDiff->SetDirectory(nullptr);
            for (int b = 1; b <= hDiff->GetNbinsX(); b++) {
                hDiff->SetBinContent(b, hHI->GetBinContent(b) - hPP->GetBinContent(b));
                hDiff->SetBinError(b, sqrt(pow(hHI->GetBinError(b),2) + pow(hPP->GetBinError(b),2)));
            }

            delete hPPb_eta; delete hPPb_phi; delete hPbP_eta; delete hPbP_phi;
            if (obs == "DeltaEta") { delete hPP_phi_dummy; }
            else { delete hPP_eta_dummy; }
        }

        // Determine axis ranges
        double xMin, xMax;
        if (obs == "DeltaPhi") { xMin = -1.5707; xMax = 4.7123; }
        else { xMin = -3.87; xMax = 3.87; }

        // Y range from all histograms
        double yMin = 1e30, yMax = -1e30;
        for (auto *h : {(TH1D*)hC, (TH1D*)hE, (TH1D*)hN}) {
            for (int b = 1; b <= h->GetNbinsX(); b++) {
                yMin = min(yMin, h->GetBinContent(b));
                yMax = max(yMax, h->GetBinContent(b));
            }
        }
        if (hPP) for (int b = 1; b <= hPP->GetNbinsX(); b++) {
            yMin = min(yMin, hPP->GetBinContent(b));
            yMax = max(yMax, hPP->GetBinContent(b));
        }
        if (hHI) for (int b = 1; b <= hHI->GetNbinsX(); b++) {
            yMin = min(yMin, hHI->GetBinContent(b));
            yMax = max(yMax, hHI->GetBinContent(b));
        }
        double range = yMax - yMin;
        yMin -= range * 0.10;
        yMax += range * 0.50;

        // Difference range
        double diffMax = 0;
        for (auto *h : {(TH1D*)hE, (TH1D*)hN}) {
            for (int b = 1; b <= h->GetNbinsX(); b++) {
                double d = fabs(h->GetBinContent(b) - hC->GetBinContent(b));
                double e = sqrt(pow(h->GetBinError(b),2) + pow(hC->GetBinError(b),2));
                diffMax = max(diffMax, d + e);
            }
        }
        if (hDiff) {
            for (int b = 1; b <= hDiff->GetNbinsX(); b++) {
                double d = fabs(hDiff->GetBinContent(b));
                double e = hDiff->GetBinError(b);
                diffMax = max(diffMax, d + e);
            }
        }
        if (diffMax < 0.001) diffMax = 0.001;
        double diffMargin = diffMax * 1.35;

        // Canvas with split pads
        double splitFrac = 0.56;
        TCanvas canvas(("c_" + obs).c_str(), "", 600, 750);

        // Top pad
        TPad *padTop = new TPad("padTop", "", 0, 1 - splitFrac, 1, 1);
        padTop->SetBottomMargin(0.02);
        padTop->SetTopMargin(0.08);
        padTop->SetLeftMargin(0.15);
        padTop->SetRightMargin(0.05);
        padTop->Draw();

        // Bottom pad
        TPad *padBot = new TPad("padBot", "", 0, 0, 1, 1 - splitFrac);
        padBot->SetTopMargin(0.02);
        padBot->SetBottomMargin(0.22);
        padBot->SetLeftMargin(0.15);
        padBot->SetRightMargin(0.05);
        padBot->Draw();

        // --- Top pad ---
        padTop->cd();

        TH1D *frame = new TH1D(("frame_" + obs).c_str(), "", 1, xMin, xMax);
        frame->SetMinimum(yMin);
        frame->SetMaximum(yMax);
        frame->GetYaxis()->SetTitle(("d#LT#DeltaN_{ch}#GT/d" + string(obs == "DeltaPhi" ? "#Delta#phi_{ch,Z}" : "#Deltay_{ch,Z}")).c_str());
        frame->GetYaxis()->SetTitleSize(0.06);
        frame->GetYaxis()->SetLabelSize(0.05);
        frame->GetXaxis()->SetLabelSize(0);
        frame->GetXaxis()->SetTickLength(0.03);
        frame->Draw();

        // MC curves — solid markers
        int cmsBlue = TColor::GetColor("#3B68F9");
        int cmsRed  = TColor::GetColor("#F23B3B");

        hC->SetLineColor(cmsBlue);   hC->SetMarkerColor(cmsBlue);   hC->SetMarkerStyle(20); hC->SetMarkerSize(0.8);
        hE->SetLineColor(cmsRed);    hE->SetMarkerColor(cmsRed);    hE->SetMarkerStyle(21); hE->SetMarkerSize(0.8);
        hN->SetLineColor(kGreen+2);  hN->SetMarkerColor(kGreen+2);  hN->SetMarkerStyle(22); hN->SetMarkerSize(0.9);

        hC->Draw("EP SAME");
        hE->Draw("EP SAME");
        hN->Draw("EP SAME");

        if (hPP) {
            hPP->SetLineColor(kBlue);
            hPP->SetMarkerColor(kBlue);
            hPP->SetMarkerStyle(24);
            hPP->SetMarkerSize(0.7);
            hPP->SetLineStyle(2);
            hPP->Draw("EP SAME");
        }
        if (hHI) {
            hHI->SetLineColor(kRed);
            hHI->SetMarkerColor(kRed);
            hHI->SetMarkerStyle(25);
            hHI->SetMarkerSize(0.7);
            hHI->SetLineStyle(2);
            hHI->Draw("EP SAME");
        }

        // Legend — only on DeltaPhi top panel (left figure in note)
        if (obs == "DeltaPhi") {
            TLegend *leg = new TLegend(0.18, 0.38, 0.55, 0.70);
            leg->SetBorderSize(0);
            leg->SetFillStyle(0);
            leg->SetTextFont(42);
            leg->SetTextSize(0.042);
            leg->AddEntry(hC, "CT18ANLO", "lep");
            leg->AddEntry(hE, "EPPS21", "lep");
            leg->AddEntry(hN, "NNPDF31", "lep");
            if (hPP) leg->AddEntry(hPP, "pp data (8.16 TeV)", "lep");
            if (hHI) leg->AddEntry(hHI, "pPb data (8.16 TeV)", "lep");
            leg->Draw();
        }

        // Labels
        TLatex latex;
        latex.SetNDC();
        latex.SetTextFont(42);
        latex.SetTextSize(0.042);
        latex.DrawLatex(0.22, 0.85, "nPDF comparison (gen-level pp #sqrt{s} = 8.16 TeV)");
        latex.SetTextSize(0.038);
        latex.DrawLatex(0.22, 0.79, FormatPTRange(trkRange, "p_{T}^{ch}").c_str());
        latex.DrawLatex(0.22, 0.73, FormatPTRange(zptRange, "p_{T}^{Z}").c_str());

        // --- Bottom pad ---
        padBot->cd();

        string diffLabel = obs == "DeltaPhi" ? "#Delta#phi_{ch,Z}" : "#Deltay_{ch,Z}";

        TH1D *frameBot = new TH1D(("frameBot_" + obs).c_str(), "", 1, xMin, xMax);
        frameBot->SetMinimum(-diffMargin);
        frameBot->SetMaximum(diffMargin);
        frameBot->GetYaxis()->SetTitle("Difference");
        frameBot->GetYaxis()->SetTitleSize(0.08);
        frameBot->GetYaxis()->SetTitleOffset(0.7);
        frameBot->GetYaxis()->SetLabelSize(0.07);
        frameBot->GetYaxis()->SetNdivisions(505);
        frameBot->GetXaxis()->SetTitle(diffLabel.c_str());
        frameBot->GetXaxis()->SetTitleSize(0.10);
        frameBot->GetXaxis()->SetLabelSize(0.08);
        frameBot->Draw();

        TLine *zeroLine = new TLine(xMin, 0, xMax, 0);
        zeroLine->SetLineStyle(2);
        zeroLine->SetLineColor(kGray+2);
        zeroLine->Draw();

        // EPPS21 - CT18ANLO
        TH1D *hDiffE = (TH1D *)hE->Clone(("hDiffE_" + obs).c_str());
        hDiffE->Add(hC, -1);
        hDiffE->SetLineColor(cmsRed); hDiffE->SetMarkerColor(cmsRed); hDiffE->SetMarkerStyle(21); hDiffE->SetMarkerSize(0.8);
        hDiffE->SetLineStyle(1);
        hDiffE->Draw("EP SAME");

        // NNPDF31 - CT18ANLO
        TH1D *hDiffN = (TH1D *)hN->Clone(("hDiffN_" + obs).c_str());
        hDiffN->Add(hC, -1);
        hDiffN->SetLineColor(kGreen+2); hDiffN->SetMarkerColor(kGreen+2); hDiffN->SetMarkerStyle(22); hDiffN->SetMarkerSize(0.9);
        hDiffN->SetLineStyle(1);
        hDiffN->Draw("EP SAME");

        // pPb - pp
        if (hDiff) {
            hDiff->SetLineColor(kRed);
            hDiff->SetMarkerColor(kRed);
            hDiff->SetMarkerStyle(25);
            hDiff->SetMarkerSize(0.7);
            hDiff->SetLineStyle(2);
            hDiff->Draw("EP SAME");
        }

        // Legend for difference panel
        TLegend *legBot = new TLegend(0.18, 0.72, 0.60, 0.95);
        legBot->SetBorderSize(0);
        legBot->SetFillStyle(0);
        legBot->SetTextFont(42);
        legBot->SetTextSize(0.055);
        legBot->AddEntry(hDiffE, "EPPS21 #minus CT18", "lep");
        legBot->AddEntry(hDiffN, "NNPDF31 #minus CT18", "lep");
        if (hDiff) legBot->AddEntry(hDiff, "pPb #minus pp (data)", "lep");
        legBot->Draw();

        canvas.Update();
        canvas.SaveAs((outputBase + "-" + obs + ".pdf").c_str());
    }

    return 0;
}
