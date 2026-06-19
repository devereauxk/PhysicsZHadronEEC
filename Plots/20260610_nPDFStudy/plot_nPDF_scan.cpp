#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TROOT.h"
#include "TStyle.h"

#include "CommandLine.h"
#include "KylesPlotting.h"
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

struct BinData {
    TH1D *hC, *hE, *hN;
    string trkLabel;
};

BinData LoadBin(const string &dir, const string &tag, const string &zptTag,
                const string &trkTag, const string &obs)
{
    string trkRange = trkTag;

    auto openResult = [&](const string &pdf) -> TFile* {
        string path = dir + "/" + pdf + "_" + tag + "_" + zptTag + "_trkPT" + trkTag + "-result.root";
        TFile *f = TFile::Open(path.c_str());
        if (!f || f->IsZombie()) { cerr << "Cannot open " << path << endl; exit(1); }
        return f;
    };

    TFile *fC = openResult("nPDF_ct18anlo");
    TFile *fE = openResult("nPDF_epps21");
    TFile *fN = openResult("nPDF_nnpdf31");

    string histName = obs + "_Result" + trkRange;
    TH1D *hC = (TH1D *)fC->Get(histName.c_str());
    TH1D *hE = (TH1D *)fE->Get(histName.c_str());
    TH1D *hN = (TH1D *)fN->Get(histName.c_str());
    if (!hC || !hE || !hN) { cerr << "Missing " << histName << endl; exit(1); }

    hC = (TH1D *)hC->Clone(Form("hC_%s_%s_%s", obs.c_str(), zptTag.c_str(), trkTag.c_str()));
    hE = (TH1D *)hE->Clone(Form("hE_%s_%s_%s", obs.c_str(), zptTag.c_str(), trkTag.c_str()));
    hN = (TH1D *)hN->Clone(Form("hN_%s_%s_%s", obs.c_str(), zptTag.c_str(), trkTag.c_str()));
    hC->SetDirectory(nullptr); hE->SetDirectory(nullptr); hN->SetDirectory(nullptr);

    hC->Scale(0.5); hE->Scale(0.5); hN->Scale(0.5);

    if (obs == "DeltaEta") { Symmetrize1DEta(hC); Symmetrize1DEta(hE); Symmetrize1DEta(hN); }
    if (obs == "DeltaPhi") { Symmetrize1DPhi(hC); Symmetrize1DPhi(hE); Symmetrize1DPhi(hN); }

    string lo = trkTag.substr(0, trkTag.find('_'));
    string hi = trkTag.substr(trkTag.find('_') + 1);
    string label = lo + " < p_{T}^{ch} < " + hi + " GeV";

    return {hC, hE, hN, label};
}

void PlotScanRow(const string &obs, const string &zptTag,
                 const vector<string> &trkTags, const string &inputDir,
                 const string &tag, const string &outPath)
{
    vector<BinData> bins;
    for (auto &t : trkTags)
        bins.push_back(LoadBin(inputDir, tag, zptTag, t, obs));

    bool isEta = (obs == "DeltaEta");
    double xLo = isEta ? -3.87 : -1.5708;
    double xHi = isEta ?  3.87 :  4.7124;
    string xLabel = isEta ? "#Deltay_{ch,Z}" : "#Delta#phi_{ch,Z}";
    string yLabel = "d#LT#DeltaN_{ch}#GT/d" + xLabel;

    int nCol = bins.size();
    double canW = 400 * nCol;
    TCanvas canvas("cScan", "", canW, 500);

    double colW = 1.0 / nCol;
    double topFrac = 0.70;

    vector<Int_t> colors = {cmsBlue, cmsRed, kGreen+2};
    vector<Int_t> markers = {20, 21, 22};
    vector<string> labels = {"CT18ANLO", "EPPS21", "NNPDF31"};

    for (int ic = 0; ic < nCol; ++ic) {
        double x1 = ic * colW;
        double x2 = (ic + 1) * colW;

        // Top pad
        TPad *pTop = new TPad(Form("top_%d", ic), "", x1, 1.0 - topFrac, x2, 1.0);
        pTop->SetBottomMargin(0.0);
        pTop->SetLeftMargin(ic == 0 ? 0.18 : 0.04);
        pTop->SetRightMargin(ic == nCol - 1 ? 0.04 : 0.0);
        pTop->SetTopMargin(0.08);
        canvas.cd();
        pTop->Draw();
        pTop->cd();

        vector<TH1D*> hists = {bins[ic].hC, bins[ic].hE, bins[ic].hN};

        double gmin = 1e30, gmax = -1e30;
        for (auto *h : hists) {
            for (int b = 1; b <= h->GetNbinsX(); ++b) {
                double v = h->GetBinContent(b);
                if (v < gmin) gmin = v;
                if (v > gmax) gmax = v;
            }
        }
        double margin = 0.25 * (gmax - gmin);

        for (int ih = 0; ih < 3; ++ih) {
            hists[ih]->SetLineColor(colors[ih]);
            hists[ih]->SetMarkerColor(colors[ih]);
            hists[ih]->SetMarkerStyle(markers[ih]);
            hists[ih]->SetMarkerSize(0.8);
            hists[ih]->GetXaxis()->SetRangeUser(xLo, xHi);
            hists[ih]->GetYaxis()->SetRangeUser(gmin - margin, gmax + margin);
            hists[ih]->GetXaxis()->SetLabelSize(0);
            hists[ih]->GetXaxis()->SetTickLength(0.03);
            if (ic == 0) {
                hists[ih]->GetYaxis()->SetTitle(yLabel.c_str());
                hists[ih]->GetYaxis()->SetTitleSize(0.06);
                hists[ih]->GetYaxis()->SetTitleOffset(1.3);
                hists[ih]->GetYaxis()->SetLabelSize(0.05);
            } else {
                hists[ih]->GetYaxis()->SetLabelSize(0);
                hists[ih]->GetYaxis()->SetTickLength(0.01);
            }
            hists[ih]->Draw(ih == 0 ? "P" : "P SAME");
        }

        TLatex lx;
        lx.SetNDC();
        lx.SetTextFont(42);
        lx.SetTextSize(0.06);
        lx.DrawLatex(ic == 0 ? 0.22 : 0.08, 0.85, bins[ic].trkLabel.c_str());

        if (ic == 0) {
            string zLo = zptTag.substr(3, zptTag.find('_') - 3);
            string zHi = zptTag.substr(zptTag.find('_') + 1);
            lx.DrawLatex(0.22, 0.77, Form("%s < p_{T}^{Z} < %s GeV", zLo.c_str(), zHi.c_str()));

            TLegend *leg = new TLegend(0.22, 0.50, 0.80, 0.72);
            leg->SetBorderSize(0);
            leg->SetFillStyle(0);
            leg->SetTextFont(42);
            leg->SetTextSize(0.055);
            for (int ih = 0; ih < 3; ++ih)
                leg->AddEntry(hists[ih], labels[ih].c_str(), "p");
            leg->Draw();

            lx.SetTextFont(62);
            lx.SetTextSize(0.07);
            lx.DrawLatex(0.22, 0.93, "CMS");
            lx.SetTextFont(52);
            lx.SetTextSize(0.055);
            lx.DrawLatex(0.42, 0.93, "Internal");
        }

        // Bottom pad (difference)
        TPad *pBot = new TPad(Form("bot_%d", ic), "", x1, 0.0, x2, 1.0 - topFrac);
        pBot->SetTopMargin(0.0);
        pBot->SetBottomMargin(0.30);
        pBot->SetLeftMargin(ic == 0 ? 0.18 : 0.04);
        pBot->SetRightMargin(ic == nCol - 1 ? 0.04 : 0.0);
        canvas.cd();
        pBot->Draw();
        pBot->cd();

        double dLo = -0.01, dHi = 0.01;

        for (int ih = 1; ih < 3; ++ih) {
            TH1D *hDiff = (TH1D *)hists[ih]->Clone(Form("diff_%d_%d", ic, ih));
            hDiff->Add(hists[0], -1.0);
            setDifferenceErrors(hDiff, hists[ih], hists[0]);

            hDiff->GetXaxis()->SetRangeUser(xLo, xHi);
            hDiff->GetYaxis()->SetRangeUser(dLo, dHi);
            hDiff->GetXaxis()->SetTitle(xLabel.c_str());
            hDiff->GetXaxis()->SetTitleSize(0.12);
            hDiff->GetXaxis()->SetLabelSize(0.10);
            hDiff->GetXaxis()->SetTitleOffset(1.0);
            if (ic == 0) {
                hDiff->GetYaxis()->SetTitle("X #minus CT18");
                hDiff->GetYaxis()->SetTitleSize(0.10);
                hDiff->GetYaxis()->SetTitleOffset(0.75);
                hDiff->GetYaxis()->SetLabelSize(0.08);
                hDiff->GetYaxis()->SetNdivisions(505);
            } else {
                hDiff->GetYaxis()->SetLabelSize(0);
                hDiff->GetYaxis()->SetTickLength(0.01);
            }
            hDiff->SetMarkerSize(0.8);
            hDiff->Draw(ih == 1 ? "P" : "P SAME");
        }

        TLine *line = new TLine(xLo, 0, xHi, 0);
        line->SetLineColor(kGray + 2);
        line->SetLineStyle(2);
        line->Draw("SAME");
    }

    canvas.Update();
    canvas.SaveAs(outPath.c_str());
}

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    gROOT->SetBatch(kTRUE);
    SetThesisStyle();
    gStyle->SetOptStat(0);

    string inputDir  = CL.Get("InputDir");
    string tag       = CL.Get("Tag", "nPDF_study");
    string outputBase = CL.Get("OutputBase", "plots/nPDF/nPDF_scan");

    vector<string> zptTags  = {"ZPT0_30", "ZPT30_500"};
    vector<string> trkTags  = {"0.5_2", "2_4", "4_15"};

    for (auto &zpt : zptTags) {
        PlotScanRow("DeltaEta", zpt, trkTags, inputDir, tag, outputBase + "-" + zpt + "-DeltaEta.pdf");
        PlotScanRow("DeltaPhi", zpt, trkTags, inputDir, tag, outputBase + "-" + zpt + "-DeltaPhi.pdf");
    }

    return 0;
}
