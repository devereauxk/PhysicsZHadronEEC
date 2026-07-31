#include <iostream>
#include <string>
#include "TFile.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"
#include "CommandLine.h"

using namespace std;

int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);
    string jewelFile = CL.Get("jewelFile");
    string dataFile = CL.Get("dataFile");
    string jewelLabel = CL.Get("jewelLabel", "JEWEL pp 8.16 TeV");
    string dataLabel = CL.Get("dataLabel", "pp data 8.16 TeV");
    string trkPt = CL.Get("trkPtRange", "0.5_15");
    string output = CL.Get("output", "plots/jewel_zpt_comparison.pdf");

    TFile *fJewel = TFile::Open(jewelFile.c_str());
    TFile *fData = TFile::Open(dataFile.c_str());
    if (!fJewel || fJewel->IsZombie() || !fData || fData->IsZombie()) {
        cerr << "Cannot open input files" << endl;
        return 1;
    }

    string hname = "hZPtEtaPhi_" + trkPt;
    TH3D *h3Jewel = (TH3D*)fJewel->Get(hname.c_str());
    TH3D *h3Data = (TH3D*)fData->Get(hname.c_str());
    if (!h3Jewel || !h3Data) {
        cerr << "Missing " << hname << endl;
        return 1;
    }

    TH1D *hJewel = h3Jewel->ProjectionX("hZPt_jewel");
    TH1D *hData = h3Data->ProjectionX("hZPt_data");
    hJewel->SetDirectory(nullptr);
    hData->SetDirectory(nullptr);

    // already 1/N_Z; divide by bin width for dN/dpT shape
    for (int i = 1; i <= hJewel->GetNbinsX(); ++i) {
        double w = hJewel->GetBinWidth(i);
        hJewel->SetBinContent(i, hJewel->GetBinContent(i) / w);
        hJewel->SetBinError(i, hJewel->GetBinError(i) / w);
    }
    for (int i = 1; i <= hData->GetNbinsX(); ++i) {
        double w = hData->GetBinWidth(i);
        hData->SetBinContent(i, hData->GetBinContent(i) / w);
        hData->SetBinError(i, hData->GetBinError(i) / w);
    }

    gStyle->SetOptStat(0);
    TCanvas c("c", "", 700, 800);
    TPad *pTop = new TPad("pTop", "", 0, 0.35, 1, 1);
    TPad *pBot = new TPad("pBot", "", 0, 0, 1, 0.35);
    pTop->SetBottomMargin(0.02);
    pTop->SetLeftMargin(0.14);
    pBot->SetTopMargin(0.02);
    pBot->SetBottomMargin(0.3);
    pBot->SetLeftMargin(0.14);
    pTop->Draw();
    pBot->Draw();

    pTop->cd();
    pTop->SetLogx();
    pTop->SetLogy();

    hData->SetLineColor(kBlack);
    hData->SetMarkerColor(kBlack);
    hData->SetMarkerStyle(20);
    hData->SetMarkerSize(0.8);
    hData->SetLineWidth(2);

    hJewel->SetLineColor(kRed + 1);
    hJewel->SetMarkerColor(kRed + 1);
    hJewel->SetMarkerStyle(24);
    hJewel->SetMarkerSize(0.8);
    hJewel->SetLineWidth(2);

    double ymax = max(hData->GetMaximum(), hJewel->GetMaximum());
    double ymin_pos = 1e30;
    for (int i = 1; i <= hData->GetNbinsX(); ++i)
        if (hData->GetBinContent(i) > 0) ymin_pos = min(ymin_pos, hData->GetBinContent(i));
    for (int i = 1; i <= hJewel->GetNbinsX(); ++i)
        if (hJewel->GetBinContent(i) > 0) ymin_pos = min(ymin_pos, hJewel->GetBinContent(i));

    hData->GetYaxis()->SetRangeUser(ymin_pos * 0.3, ymax * 5);
    hData->GetXaxis()->SetRangeUser(0.5, 350);
    hData->GetXaxis()->SetLabelSize(0);
    hData->GetXaxis()->SetTickLength(0.03);
    hData->GetYaxis()->SetTitle("#frac{1}{N_{Z}} #frac{dN_{Z}}{dp_{T}^{Z}} [GeV^{-1}]");
    hData->GetYaxis()->SetTitleSize(0.05);
    hData->GetYaxis()->SetTitleOffset(1.3);
    hData->Draw("EP");
    hJewel->Draw("EP SAME");

    TLegend leg(0.50, 0.72, 0.88, 0.88);
    leg.SetBorderSize(0);
    leg.SetFillStyle(0);
    leg.SetTextSize(0.04);
    leg.AddEntry(hData, dataLabel.c_str(), "lep");
    leg.AddEntry(hJewel, jewelLabel.c_str(), "lep");
    leg.Draw();

    TLatex tex;
    tex.SetNDC();
    tex.SetTextSize(0.035);
    tex.DrawLatex(0.50, 0.67, "Z #rightarrow #mu#mu, |y_{Z}| < 1.935");

    // ratio
    pBot->cd();
    pBot->SetLogx();
    TH1D *hRatio = (TH1D*)hJewel->Clone("hRatio_zpt");
    hRatio->Divide(hData);
    hRatio->SetLineColor(kRed + 1);
    hRatio->SetMarkerColor(kRed + 1);
    hRatio->SetMarkerStyle(24);
    hRatio->SetMarkerSize(0.8);
    string ratioLabel = "JEWEL / " + dataLabel;
    hRatio->GetYaxis()->SetTitle(ratioLabel.c_str());
    hRatio->GetYaxis()->SetTitleSize(0.09);
    hRatio->GetYaxis()->SetTitleOffset(0.65);
    hRatio->GetYaxis()->SetLabelSize(0.08);
    hRatio->GetYaxis()->SetNdivisions(505);
    hRatio->GetXaxis()->SetTitle("p_{T}^{Z} [GeV]");
    hRatio->GetXaxis()->SetTitleSize(0.1);
    hRatio->GetXaxis()->SetTitleOffset(1.0);
    hRatio->GetXaxis()->SetLabelSize(0.08);
    hRatio->GetXaxis()->SetRangeUser(0.5, 350);
    hRatio->GetYaxis()->SetRangeUser(0.0, 2.0);
    hRatio->Draw("EP");

    TLine line(0.5, 1, 350, 1);
    line.SetLineStyle(2);
    line.Draw();

    c.SaveAs(output.c_str());
    cout << "Saved: " << output << endl;
    return 0;
}
