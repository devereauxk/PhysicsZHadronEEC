#include <iostream>
#include <string>
#include <vector>
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"
#include "CommandLine.h"

using namespace std;

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

int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);
    vector<string> files = CL.GetStringVector("files");
    vector<string> labels = CL.GetStringVector("labels");
    string refLabel = CL.Get("refLabel", "");
    string output = CL.Get("output", "plots/jewel_comparison.pdf");
    string trkPt = CL.Get("trkPtRange", "0.5_15");
    bool doEta = CL.GetBool("doEta", true);
    string obs = doEta ? "DeltaEta" : "DeltaPhi";

    if (files.size() < 2 || files.size() != labels.size()) {
        cerr << "Need >=2 files with matching labels" << endl;
        return 1;
    }

    bool showLegend = CL.GetBool("showLegend", true);
    int colors[] = {kRed+1, kBlue+1, kGreen+2, kMagenta+1, kOrange+1, kViolet+1, kCyan+2, kBlack};
    int markers[] = {20, 24, 21, 25, 22, 26, 32, 23};

    vector<TH1D*> histos;
    for (size_t i = 0; i < files.size(); ++i) {
        TFile *f = TFile::Open(Form("%s-result.root", files[i].c_str()));
        if (!f || f->IsZombie()) { cerr << "Cannot open " << files[i] << endl; return 1; }
        TH1D *h = (TH1D*)f->Get(Form("%s_Result%s", obs.c_str(), trkPt.c_str()));
        if (!h) { cerr << "Missing " << obs << "_Result" << trkPt << " in " << files[i] << endl; return 1; }
        h = (TH1D*)h->Clone(Form("h_%zu", i));
        h->SetDirectory(nullptr);
        h->Scale(1./2);
        if (h->GetNbinsX() == 12) { doEta ? Symmetrize1DEta(h) : Symmetrize1DPhi(h); }
        cout << labels[i] << " integral: " << h->Integral() << endl;
        histos.push_back(h);
    }

    gStyle->SetOptStat(0);
    TCanvas c("c", "", 700, 800);
    TPad *pTop = new TPad("pTop", "", 0, 0.35, 1, 1);
    TPad *pBot = new TPad("pBot", "", 0, 0, 1, 0.35);
    pTop->SetBottomMargin(0.02);
    pTop->SetLeftMargin(0.15);
    pBot->SetTopMargin(0.02);
    pBot->SetBottomMargin(0.3);
    pBot->SetLeftMargin(0.15);
    pTop->Draw(); pBot->Draw();

    pTop->cd();
    double ymin = 1e30, ymax = -1e30;
    for (auto *h : histos) {
        ymin = min(ymin, h->GetMinimum());
        ymax = max(ymax, h->GetMaximum());
    }
    double margin = 0.15 * (ymax - ymin);

    for (size_t i = 0; i < histos.size(); ++i) {
        histos[i]->SetLineColor(colors[i % 8]);
        histos[i]->SetLineWidth(2);
        histos[i]->SetMarkerColor(colors[i % 8]);
        histos[i]->SetMarkerStyle(markers[i % 8]);
        histos[i]->SetMarkerSize(0.9);
    }
    histos[0]->GetYaxis()->SetRangeUser(ymin - margin, ymax + 2.5*margin);
    histos[0]->GetXaxis()->SetLabelSize(0);
    histos[0]->GetXaxis()->SetTickLength(0.03);
    histos[0]->GetYaxis()->SetTitle(doEta ? "#frac{1}{N_{Z}} #frac{dN_{ch}}{d#Deltay}"
                                          : "#frac{1}{N_{Z}} #frac{dN_{ch}}{d#Delta#phi}");
    histos[0]->GetYaxis()->SetTitleSize(0.05);
    histos[0]->GetYaxis()->SetTitleOffset(1.2);
    histos[0]->Draw("EP");
    for (size_t i = 1; i < histos.size(); ++i) histos[i]->Draw("EP SAME");

    if (showLegend) {
        TLegend *leg = new TLegend(0.15, 0.88 - 0.06*histos.size(), 0.60, 0.88);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextSize(0.035);
        for (size_t i = 0; i < histos.size(); ++i)
            leg->AddEntry(histos[i], labels[i].c_str(), "lep");
        leg->Draw();
    }

    TLatex tex;
    tex.SetNDC();
    tex.SetTextSize(0.035);
    tex.DrawLatex(0.15, 0.88 - 0.06*histos.size() - 0.04,
        Form("trkPT %s, ZPT 0-500 GeV", trkPt.c_str()));

    // ratio panel: all / first
    pBot->cd();
    string ratioTitle = refLabel.empty() ? Form("ratio to %s", labels[0].c_str()) : Form("ratio to %s", refLabel.c_str());
    vector<TH1D*> ratios;
    for (size_t i = 1; i < histos.size(); ++i) {
        TH1D *hRatio = (TH1D*)histos[i]->Clone(Form("hRatio_%zu", i));
        hRatio->Divide(histos[0]);
        hRatio->SetLineColor(colors[i % 8]);
        hRatio->SetMarkerColor(colors[i % 8]);
        hRatio->SetMarkerStyle(markers[i % 8]);
        hRatio->SetMarkerSize(0.8);
        ratios.push_back(hRatio);
    }
    double rmin = 1e30, rmax = -1e30;
    for (auto *r : ratios) {
        for (int b = 1; b <= r->GetNbinsX(); ++b) {
            double v = r->GetBinContent(b);
            if (v != 0) { rmin = min(rmin, v); rmax = max(rmax, v); }
        }
    }
    double rmargin = 0.15 * (rmax - rmin);
    for (size_t i = 0; i < ratios.size(); ++i) {
        if (i == 0) {
            ratios[i]->GetYaxis()->SetTitle(ratioTitle.c_str());
            ratios[i]->GetYaxis()->SetTitleSize(0.09);
            ratios[i]->GetYaxis()->SetTitleOffset(0.6);
            ratios[i]->GetYaxis()->SetLabelSize(0.08);
            ratios[i]->GetYaxis()->SetNdivisions(505);
            ratios[i]->GetXaxis()->SetTitle(doEta ? "#Deltay_{ch,Z}" : "#Delta#phi_{ch,Z}");
            ratios[i]->GetXaxis()->SetTitleSize(0.1);
            ratios[i]->GetXaxis()->SetTitleOffset(1.0);
            ratios[i]->GetXaxis()->SetLabelSize(0.08);
            ratios[i]->GetYaxis()->SetRangeUser(rmin - rmargin, rmax + rmargin);
            ratios[i]->Draw("EP");
        } else {
            ratios[i]->Draw("EP SAME");
        }
    }

    TLine line(histos[0]->GetXaxis()->GetXmin(), 1, histos[0]->GetXaxis()->GetXmax(), 1);
    line.SetLineStyle(2);
    line.Draw();

    c.SaveAs(output.c_str());
    cout << "Saved: " << output << endl;
    return 0;
}
