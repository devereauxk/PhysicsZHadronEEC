#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"
#include "TSystem.h"
#include "CommandLine.h"

using namespace std;

void Symmetrize1DPhi(TH1D *h) {
    int n = h->GetNbinsX();
    int half = n / 2;
    for (int j = 0; j < half / 2; ++j) {
        int jNear = j + 1, mNear = half - j;
        double avg = 0.5 * (h->GetBinContent(jNear) + h->GetBinContent(mNear));
        double err = 0.5 * sqrt(pow(h->GetBinError(jNear), 2) + pow(h->GetBinError(mNear), 2));
        h->SetBinContent(jNear, avg); h->SetBinContent(mNear, avg);
        h->SetBinError(jNear, err);   h->SetBinError(mNear, err);

        int jAway = half + j + 1;
        int mAway = n + half + 1 - jAway;
        avg = 0.5 * (h->GetBinContent(jAway) + h->GetBinContent(mAway));
        err = 0.5 * sqrt(pow(h->GetBinError(jAway), 2) + pow(h->GetBinError(mAway), 2));
        h->SetBinContent(jAway, avg); h->SetBinContent(mAway, avg);
        h->SetBinError(jAway, err);   h->SetBinError(mAway, err);
    }
}

void Symmetrize1DEta(TH1D *h) {
    int n = h->GetNbinsX();
    for (int i = 0; i < n / 2; ++i) {
        int iL = i + 1, iR = n - i;
        double avg = 0.5 * (h->GetBinContent(iL) + h->GetBinContent(iR));
        double err = 0.5 * sqrt(pow(h->GetBinError(iL), 2) + pow(h->GetBinError(iR), 2));
        h->SetBinContent(iL, avg); h->SetBinContent(iR, avg);
        h->SetBinError(iL, err);   h->SetBinError(iR, err);
    }
}

TH1D* LoadHist(const string &filePrefix, const string &zpt, const string &trkPt, bool doEta) {
    string path = filePrefix + "_ZPT" + zpt + "-result.root";
    TFile *f = TFile::Open(path.c_str(), "READ");
    if (!f || f->IsZombie()) { cerr << "Cannot open " << path << endl; return nullptr; }
    string obs = doEta ? "DeltaEta" : "DeltaPhi";
    string hname = obs + "_Result" + trkPt;
    TH1D *h = (TH1D*)f->Get(hname.c_str());
    if (!h) { cerr << "Missing " << hname << " in " << path << endl; return nullptr; }
    h = (TH1D*)h->Clone();
    h->SetDirectory(nullptr);
    h->Scale(0.5);
    if (h->GetNbinsX() == 12) { doEta ? Symmetrize1DEta(h) : Symmetrize1DPhi(h); }
    return h;
}

int OpenMarkerStyle(int filled) {
    switch (filled) {
        case 20: return 24; case 21: return 25; case 22: return 26;
        case 23: return 32; case 33: return 27; case 34: return 28;
        default: return filled + 4;
    }
}

string ZPtRowLabel(const string &z) {
    size_t p = z.find('_');
    int lo = atoi(z.substr(0, p).c_str()), hi = atoi(z.substr(p + 1).c_str());
    if (lo == 0 && hi >= 500) return "inclusive p_{T}^{Z}";
    if (lo == 0) return Form("p_{T}^{Z} < %d GeV", hi);
    return Form("p_{T}^{Z} > %d GeV", lo);
}

int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);
    string baselinePrefix = CL.Get("baselinePrefix");
    vector<string> jewelPrefixes = CL.GetStringVector("jewelPrefixes", vector<string>());
    vector<string> labels = CL.GetStringVector("labels", vector<string>());
    string output = CL.Get("output", "plots/jewel_composite_deltaphi.pdf");
    bool doEta = CL.GetBool("doEta", false);

    // Per-row mode: different JEWEL curves for the 0_30 (Low) and 30_500 (High) rows.
    // Color indices select from the palette so curve colors stay consistent across plots.
    vector<string> jpLow = CL.GetStringVector("jewelPrefixesLow", vector<string>());
    vector<string> jpHigh = CL.GetStringVector("jewelPrefixesHigh", vector<string>());
    vector<string> labLow = CL.GetStringVector("labelsLow", vector<string>());
    vector<string> labHigh = CL.GetStringVector("labelsHigh", vector<string>());
    vector<string> colLow = CL.GetStringVector("jewelColorsLow", vector<string>());
    vector<string> colHigh = CL.GetStringVector("jewelColorsHigh", vector<string>());
    vector<string> colGlobal = CL.GetStringVector("jewelColors", vector<string>());
    string baselineLabel = CL.Get("baselineLabel", labels.empty() ? "baseline" : labels[0].c_str());
    double legendY = CL.GetDouble("legendY", 0.40);
    double legendScale = CL.GetDouble("legendScale", 1.0);
    bool perRow = !jpLow.empty() || !jpHigh.empty();

    if (!perRow && labels.size() != jewelPrefixes.size() + 1) {
        cerr << "Need labels.size() == jewelPrefixes.size() + 1 (baseline + jewel curves)" << endl;
        return 1;
    }

    gSystem->mkdir(gSystem->DirName(output.c_str()), true);

    string trkBins[] = {"0.5_2", "2_4", "4_15"};
    vector<string> zPtBins = CL.GetStringVector("zPtBins", vector<string>{"0_30", "30_500"});
    vector<string> zPtLabels;
    for (auto &z : zPtBins) zPtLabels.push_back(ZPtRowLabel(z));
    int nRows = zPtBins.size(), nCols = 3;
    if (perRow && nRows != 2) {
        cerr << "Per-row mode (jewelPrefixesLow/High) requires exactly 2 zPtBins" << endl;
        return 1;
    }

    int colors[] = {kRed+1, kBlue+1, kGreen+2, kMagenta+1, kOrange+1, kViolet+1, kCyan+2, kBlack};
    int markers[] = {20, 24, 21, 25, 22, 26, 32, 23};

    // Per-row curve configuration: rowPrefixes[r] excludes baseline; rowLabels[r] includes it first
    vector<vector<string>> rowPrefixes(nRows), rowLabels(nRows);
    vector<vector<int>> rowColorIdx(nRows);
    for (int r = 0; r < nRows; r++) {
        vector<string> &jp = perRow ? (r == 0 ? jpLow : jpHigh) : jewelPrefixes;
        vector<string> &lab = perRow ? (r == 0 ? labLow : labHigh) : labels;
        rowPrefixes[r] = jp;
        if (perRow) {
            rowLabels[r].push_back(baselineLabel);
            for (auto &l : lab) rowLabels[r].push_back(l);
            vector<string> &ci = (r == 0 ? colLow : colHigh);
            for (size_t j = 0; j < jp.size(); j++)
                rowColorIdx[r].push_back(j < ci.size() ? atoi(ci[j].c_str()) : (int)j + 1);
        } else {
            rowLabels[r] = labels;
            for (size_t j = 0; j < jp.size(); j++)
                rowColorIdx[r].push_back(j < colGlobal.size() ? atoi(colGlobal[j].c_str()) : (int)j + 1);
        }
        if (rowLabels[r].size() != rowPrefixes[r].size() + 1) {
            cerr << "Row " << r << ": labels must be jewel curves + 1 (baseline)" << endl;
            return 1;
        }
    }

    // Load all histograms: hists[row][col][curve]
    vector<vector<vector<TH1D*>>> hists(nRows, vector<vector<TH1D*>>(nCols));
    for (int r = 0; r < nRows; r++) {
        for (int c = 0; c < nCols; c++) {
            TH1D *hBase = LoadHist(baselinePrefix, zPtBins[r], trkBins[c], doEta);
            if (!hBase) { cerr << "Failed to load baseline for ZPT " << zPtBins[r] << " trkPT " << trkBins[c] << endl; return 1; }
            hists[r][c].push_back(hBase);
            for (size_t j = 0; j < rowPrefixes[r].size(); j++) {
                TH1D *hJ = LoadHist(rowPrefixes[r][j], zPtBins[r], trkBins[c], doEta);
                if (!hJ) { cerr << "Failed to load JEWEL " << j << " for ZPT " << zPtBins[r] << " trkPT " << trkBins[c] << endl; return 1; }
                hists[r][c].push_back(hJ);
            }
        }
    }

    // Canvas layout matching paper composite style (scaled to 2 rows)
    gStyle->SetOptStat(0);
    gStyle->SetLineScalePS(1);

    float textScale = 1.3;
    float centerLeftMargin = 0.13;
    double areaLeft = 0.98 * (1 - 0.195 - 0.05);
    double areaCenter = 0.98 * (1 - centerLeftMargin - 0.05);
    double overlap = 0.04;
    double wLeft = (1.0 + 2*overlap) / (1.0 + 2.0 * areaLeft / areaCenter);
    double wCenter = wLeft * areaLeft / areaCenter;

    double singleRowH = 600.0;
    double topRowBorderT = 0.02;
    double otherRowBorderT = 0.003;
    double topRowPx = singleRowH;
    double otherRowPx = singleRowH * (1.0 - topRowBorderT + otherRowBorderT);
    int canvasH = (int)(topRowPx + (nRows-1) * otherRowPx);
    TCanvas *canvas = new TCanvas("c", "c", 1440, canvasH);

    double totalUsed = wLeft + 2*wCenter - 2*overlap;
    double scale = 1.0 / totalUsed;
    double sLeft = wLeft * scale;
    double sCenter = wCenter * scale;
    double sOverlap = overlap * scale;

    double x0[] = {0, sLeft - sOverlap, sLeft + sCenter - 2*sOverlap};
    double x1[] = {sLeft, sLeft + sCenter - sOverlap, sLeft + 2*sCenter - 2*sOverlap};

    double topFrac = topRowPx / canvasH;
    double otherFrac = otherRowPx / canvasH;

    const char *xTitle = doEta ? "#Deltay_{ch,Z}" : "#Delta#varphi_{ch,Z}";
    const char *yTitle = doEta ? "d#LT#DeltaN_{ch}#GT/d#Deltay_{ch,Z}"
                               : "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}";
    double xmin = doEta ? -3.87 : -M_PI/2;
    double xmax = doEta ? 3.87 : 3*M_PI/2;
    double sigLo = 0;
    double sigHi = doEta ? 4.0 : M_PI;

    for (int r = 0; r < nRows; r++) {
        double yhi = (r == 0) ? 1.0 : 1.0 - topFrac - (r-1)*otherFrac;
        double ylo = max(0.0, yhi - ((r == 0) ? topFrac : otherFrac));

        for (int col = 0; col < nCols; col++) {
            canvas->cd();
            TPad *subPad = new TPad(Form("sub%d_%d", r, col), "", x0[col], ylo, x1[col], yhi);
            subPad->SetFillColor(kWhite);
            subPad->Draw();
            subPad->cd();

            bool isLeft = (col == 0);
            double borderR = 0.02;
            double borderT = (r == 0) ? -1 : otherRowBorderT;
            if (borderT < 0) borderT = 0.02;
            float leftMargin = isLeft ? 0.195 : centerLeftMargin;
            double splitGap = 0.0075;

            // Top pad (signal)
            TPad *pad1 = new TPad(Form("top%d_%d", r, col), "", 0, 0.25 + 0.06 + splitGap, 1.0 - borderR, 1.0 - borderT);
            pad1->SetTickx(1); pad1->SetTicky(1);
            pad1->SetLeftMargin(leftMargin);
            pad1->SetBottomMargin(0);
            pad1->Draw();

            // Bottom pad (ratio)
            TPad *pad2 = new TPad(Form("bot%d_%d", r, col), "", 0, 0, 1.0 - borderR, 0.25 + 0.06 - splitGap);
            pad2->SetTickx(1); pad2->SetTicky(1);
            pad2->SetLeftMargin(leftMargin);
            pad2->SetTopMargin(0);
            pad2->SetBottomMargin(0.25 * textScale);
            pad2->Draw();

            // Style histograms (curve 0 = baseline; others use per-row palette indices)
            auto &panelHists = hists[r][col];
            auto curveIdx = [&](size_t i) { return (i == 0) ? 0 : rowColorIdx[r][i-1]; };
            for (size_t i = 0; i < panelHists.size(); i++) {
                int ci = curveIdx(i);
                panelHists[i]->SetLineColor(colors[ci % 8]);
                panelHists[i]->SetLineWidth(2);
                panelHists[i]->SetMarkerColor(colors[ci % 8]);
                panelHists[i]->SetMarkerStyle(markers[ci % 8]);
                panelHists[i]->SetMarkerSize(0.9);
            }

            // Top pad: draw signal
            pad1->cd();
            double gmin = 1e30, gmax = -1e30;
            for (auto *h : panelHists) {
                for (int b = 1; b <= h->GetNbinsX(); b++) {
                    double v = h->GetBinContent(b), e = h->GetBinError(b);
                    gmin = min(gmin, v - e);
                    gmax = max(gmax, v + e);
                }
            }
            double margin = 0.2 * (gmax - gmin);
            double marginTop = margin * 1.5;
            double marginBot = margin * 0.65;

            TH1D *frame1 = (TH1D*)panelHists[0]->Clone(Form("frame1_%d_%d", r, col));
            frame1->Reset();
            frame1->GetYaxis()->SetRangeUser(gmin - marginBot, gmax + marginTop);
            frame1->GetXaxis()->SetRangeUser(xmin, xmax);
            frame1->GetXaxis()->SetLabelSize(0);
            frame1->GetXaxis()->SetTickLength(0.03);
            if (isLeft) {
                frame1->GetYaxis()->SetTitle(yTitle);
                frame1->GetYaxis()->SetTitleSize(0.055 * textScale);
                frame1->GetYaxis()->SetTitleOffset(1.3);
            } else {
                frame1->GetYaxis()->SetTitle("");
            }
            frame1->GetYaxis()->SetLabelSize(0.045 * textScale);
            frame1->GetYaxis()->SetNdivisions(505);
            frame1->Draw("AXIS");

            // Draw signal/reflected with open/filled markers
            for (size_t i = 0; i < panelHists.size(); i++) {
                TH1D *hSig = (TH1D*)panelHists[i]->Clone(Form("sig_%d_%d_%zu", r, col, i));
                TH1D *hRef = (TH1D*)panelHists[i]->Clone(Form("ref_%d_%d_%zu", r, col, i));
                for (int b = 1; b <= hSig->GetNbinsX(); b++) {
                    double center = hSig->GetXaxis()->GetBinCenter(b);
                    if (center < sigLo || center > sigHi) {
                        hSig->SetBinContent(b, 0); hSig->SetBinError(b, 0);
                    } else {
                        hRef->SetBinContent(b, 0); hRef->SetBinError(b, 0);
                    }
                }
                hRef->SetMarkerStyle(OpenMarkerStyle(markers[i % 8]));
                hSig->Draw("EP SAME");
                hRef->Draw("EP SAME");
            }

            // Zero line
            TLine *zeroTop = new TLine(xmin, 0, xmax, 0);
            zeroTop->SetLineColor(kGray+1);
            zeroTop->Draw();

            // Ratio pad
            pad2->cd();
            double rmin_v = 1e30, rmax_v = -1e30;
            vector<TH1D*> ratios;
            for (size_t i = 1; i < panelHists.size(); i++) {
                TH1D *hRatio = (TH1D*)panelHists[i]->Clone(Form("ratio_%d_%d_%zu", r, col, i));
                hRatio->Divide(panelHists[0]);
                ratios.push_back(hRatio);
                for (int b = 1; b <= hRatio->GetNbinsX(); b++) {
                    double v = hRatio->GetBinContent(b);
                    if (v != 0) { rmin_v = min(rmin_v, v); rmax_v = max(rmax_v, v); }
                }
            }
            // Symmetrize ratio range about 1
            double maxdev = max(fabs(rmin_v - 1), fabs(rmax_v - 1));
            double rmargin = 0.15 * 2 * maxdev;
            double rlo = 1 - maxdev - rmargin;
            double rhi = 1 + maxdev + rmargin;

            TH1D *frameR = (TH1D*)panelHists[0]->Clone(Form("frameR_%d_%d", r, col));
            frameR->Reset();
            frameR->GetYaxis()->SetRangeUser(rlo, rhi);
            frameR->GetXaxis()->SetRangeUser(xmin, xmax);
            frameR->GetXaxis()->SetTitle(xTitle);
            frameR->GetXaxis()->SetTitleSize(0.11 * textScale);
            frameR->GetXaxis()->SetTitleOffset(1.0);
            frameR->GetXaxis()->SetLabelSize(0.095 * textScale);
            if (isLeft) {
                frameR->GetYaxis()->SetTitle("ratio to baseline");
                frameR->GetYaxis()->SetTitleSize(0.08 * textScale);
                frameR->GetYaxis()->SetTitleOffset(0.7);
            } else {
                frameR->GetYaxis()->SetTitle("");
            }
            frameR->GetYaxis()->SetLabelSize(0.08 * textScale);
            frameR->GetYaxis()->SetNdivisions(505);
            frameR->Draw("AXIS");

            TLine *unity = new TLine(xmin, 1, xmax, 1);
            unity->SetLineStyle(2);
            unity->Draw();

            for (size_t i = 0; i < ratios.size(); i++) {
                int ci = curveIdx(i + 1);
                // Signal/reflected split for ratio
                TH1D *rSig = (TH1D*)ratios[i]->Clone(Form("rsig_%d_%d_%zu", r, col, i));
                TH1D *rRef = (TH1D*)ratios[i]->Clone(Form("rref_%d_%d_%zu", r, col, i));
                for (int b = 1; b <= rSig->GetNbinsX(); b++) {
                    double center = rSig->GetXaxis()->GetBinCenter(b);
                    if (center < sigLo || center > sigHi) {
                        rSig->SetBinContent(b, 0); rSig->SetBinError(b, 0);
                    } else {
                        rRef->SetBinContent(b, 0); rRef->SetBinError(b, 0);
                    }
                }
                rSig->SetLineColor(colors[ci % 8]);
                rSig->SetMarkerColor(colors[ci % 8]);
                rSig->SetMarkerStyle(markers[ci % 8]);
                rSig->SetMarkerSize(0.8);
                rRef->SetLineColor(colors[ci % 8]);
                rRef->SetMarkerColor(colors[ci % 8]);
                rRef->SetMarkerStyle(OpenMarkerStyle(markers[ci % 8]));
                rRef->SetMarkerSize(0.8);
                rSig->Draw("EP SAME");
                rRef->Draw("EP SAME");
            }

            // Track pT label (top-right of top pad)
            pad1->cd();
            double rowScale = (r == 0) ? 1.0 : topRowPx / otherRowPx;
            {
                size_t pos = trkBins[col].find('_');
                string trkLo = trkBins[col].substr(0, pos), trkHi = trkBins[col].substr(pos+1);
                TLatex latexR;
                latexR.SetNDC();
                latexR.SetTextFont(42);
                latexR.SetTextAlign(31);
                latexR.SetTextSize(0.045 * textScale * rowScale);
                float rM = pad1->GetRightMargin();
                latexR.DrawLatex(1 - rM - 0.06, 0.80,
                    Form("%s < p_{T}^{ch} < %s GeV", trkLo.c_str(), trkHi.c_str()));
            }

            // ZPT label (top-left of left column only)
            if (isLeft) {
                TLatex latexZ;
                latexZ.SetNDC();
                latexZ.SetTextFont(42);
                latexZ.SetTextAlign(11);
                latexZ.SetTextSize(0.045 * textScale * rowScale);
                latexZ.DrawLatex(leftMargin + 0.04, 0.80, zPtLabels[r].c_str());
            }

            // Legend: top-left panel in global mode; left panel of every row in per-row mode
            if (col == 0 && (perRow || r == 0)) {
                float legY = legendY;
                float legH = 0.06 * legendScale * panelHists.size();
                TLegend *leg = new TLegend(leftMargin + 0.03, legY, leftMargin + 0.03 + 0.52 * legendScale, legY + legH);
                leg->SetBorderSize(0);
                leg->SetFillStyle(0);
                leg->SetTextFont(42);
                leg->SetTextSize(0.038 * legendScale * textScale * rowScale);
                for (size_t i = 0; i < panelHists.size(); i++)
                    leg->AddEntry(panelHists[i], rowLabels[r][i].c_str(), "lep");
                leg->Draw();
            }
        }
    }

    canvas->SaveAs(output.c_str());
    cout << "Saved: " << output << endl;
    return 0;
}
