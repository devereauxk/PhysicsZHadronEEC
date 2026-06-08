#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TPad.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "CommandLine.h"

using namespace std;

void NormByWidth(TH1D *h)
{
   for (int b = 1; b <= h->GetNbinsX(); b++) {
      double w = h->GetBinWidth(b);
      if (w > 0) {
         h->SetBinContent(b, h->GetBinContent(b) / w);
         h->SetBinError(b, h->GetBinError(b) / w);
      }
   }
}

TH1D *ProjectAndNormalize(TH2D *h2d, const string &observable, const string &name)
{
   TH1D *h = nullptr;
   if (observable == "DeltaPhi")
      h = (TH1D *)h2d->ProjectionY(name.c_str(), 7, 12);
   else
      h = (TH1D *)h2d->ProjectionX(name.c_str(), 4, 6);

   h->SetDirectory(nullptr);
   NormByWidth(h);
   h->Scale(0.5);
   return h;
}

struct CellPads {
   TPad *main;
   TPad *ratio;
};

CellPads MakeCell(TCanvas *c, const char *prefix, int col, int row)
{
   double xlo = col * 0.5;
   double xhi = xlo + 0.5;
   double yhi = 1.0 - row * 0.5;
   double ylo = yhi - 0.5;
   double splitFrac = 0.30;
   double ySplit = ylo + (yhi - ylo) * splitFrac;

   CellPads cell;
   c->cd();
   cell.main  = new TPad(Form("%s_main_%d_%d", prefix, col, row), "",
       xlo, ySplit, xhi, yhi);
   cell.ratio = new TPad(Form("%s_rat_%d_%d", prefix, col, row), "",
       xlo, ylo, xhi, ySplit);

   cell.main->SetBottomMargin(0.015);
   cell.main->SetTopMargin(0.07);
   cell.main->SetLeftMargin(0.16);
   cell.main->SetRightMargin(0.04);

   cell.ratio->SetBottomMargin(0.35);
   cell.ratio->SetTopMargin(0.01);
   cell.ratio->SetLeftMargin(0.16);
   cell.ratio->SetRightMargin(0.04);

   cell.main->Draw();
   cell.ratio->Draw();
   return cell;
}

void DrawMainPanel(TPad *pad, TH1D *hpPb, TH1D *hPbP,
                   const string &yTitle, const string &panelLabel,
                   double xmin, double xmax, bool showLegend)
{
   pad->cd();

   double ymin = 1e30, ymax = -1e30;
   for (int b = 1; b <= hpPb->GetNbinsX(); b++) {
      ymin = min(ymin, min(hpPb->GetBinContent(b) - hpPb->GetBinError(b),
                           hPbP->GetBinContent(b) - hPbP->GetBinError(b)));
      ymax = max(ymax, max(hpPb->GetBinContent(b) + hpPb->GetBinError(b),
                           hPbP->GetBinContent(b) + hPbP->GetBinError(b)));
   }
   double range = ymax - ymin;
   ymin -= 0.10 * range;
   ymax += 0.22 * range;

   hpPb->SetMarkerStyle(20); hpPb->SetMarkerSize(0.7);
   hpPb->SetMarkerColor(kAzure - 2); hpPb->SetLineColor(kAzure - 2);
   hPbP->SetMarkerStyle(24); hPbP->SetMarkerSize(0.7);
   hPbP->SetMarkerColor(kOrange + 7); hPbP->SetLineColor(kOrange + 7);

   hpPb->SetMinimum(ymin); hpPb->SetMaximum(ymax);
   hpPb->GetXaxis()->SetRangeUser(xmin, xmax);
   hpPb->GetXaxis()->SetLabelSize(0);
   hpPb->GetXaxis()->SetTickLength(0.03);
   hpPb->GetYaxis()->SetTitle(yTitle.c_str());
   hpPb->GetYaxis()->SetTitleSize(0.060);
   hpPb->GetYaxis()->SetTitleOffset(1.20);
   hpPb->GetYaxis()->SetLabelSize(0.050);
   hpPb->SetStats(0); hpPb->SetTitle("");

   hpPb->Draw("PE");
   hPbP->Draw("PE SAME");

   if (showLegend) {
      TLegend *leg = new TLegend(0.58, 0.68, 0.93, 0.90);
      leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.050);
      leg->AddEntry(hpPb, "pPb 8.16 TeV", "pe");
      leg->AddEntry(hPbP, "Pbp 8.16 TeV", "pe");
      leg->Draw();
   }

   TLatex lat;
   lat.SetNDC(); lat.SetTextSize(0.055); lat.SetTextFont(42);
   lat.DrawLatex(0.19, 0.85, panelLabel.c_str());
}

void DrawRatioPanel(TPad *pad, TH1D *hpPb, TH1D *hPbP,
                    const string &xTitle, double xmin, double xmax)
{
   pad->cd();

   TH1D *ratio = (TH1D *)hpPb->Clone("ratio_tmp");
   ratio->SetDirectory(nullptr);
   ratio->Divide(hPbP);

   double maxDev = 0;
   for (int b = 1; b <= ratio->GetNbinsX(); b++) {
      double dev = fabs(ratio->GetBinContent(b) - 1.0) + ratio->GetBinError(b);
      maxDev = max(maxDev, dev);
   }
   if (maxDev < 0.01) maxDev = 0.01;
   double pad_factor = 1.5;

   ratio->SetMinimum(1.0 - maxDev * pad_factor);
   ratio->SetMaximum(1.0 + maxDev * pad_factor);
   ratio->GetXaxis()->SetRangeUser(xmin, xmax);
   ratio->GetXaxis()->SetTitle(xTitle.c_str());
   ratio->GetXaxis()->SetTitleSize(0.14);
   ratio->GetXaxis()->SetLabelSize(0.12);
   ratio->GetXaxis()->SetTickLength(0.08);
   ratio->GetYaxis()->SetTitle("pPb/Pbp");
   ratio->GetYaxis()->SetTitleSize(0.12);
   ratio->GetYaxis()->SetTitleOffset(0.55);
   ratio->GetYaxis()->SetLabelSize(0.10);
   ratio->GetYaxis()->SetNdivisions(505);
   ratio->GetYaxis()->CenterTitle();
   ratio->SetMarkerStyle(20); ratio->SetMarkerSize(0.6);
   ratio->SetMarkerColor(kBlack); ratio->SetLineColor(kBlack);
   ratio->SetStats(0); ratio->SetTitle("");

   ratio->Draw("PE");

   TLine *unity = new TLine(xmin, 1.0, xmax, 1.0);
   unity->SetLineStyle(2); unity->SetLineColor(kGray + 1);
   unity->Draw();
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string baseDir = CL.Get("BaseDir",
       "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
   string tag = CL.Get("pPbtag", "ZV10_trkV29_nmix10");
   string zPtRange = CL.Get("zPtRange", "0_500");
   string trkPtRange = CL.Get("trkPtRange", "0.5_15");
   string outputDir = CL.Get("outputDir", "plots/central_overlay_PPbPbP");

   string pPbPath = Form("%s/pPb_trkResidual_%s_ZPT%s-nosub.root",
       baseDir.c_str(), tag.c_str(), zPtRange.c_str());
   string PbPPath = Form("%s/PbP_trkResidual_%s_ZPT%s-nosub.root",
       baseDir.c_str(), tag.c_str(), zPtRange.c_str());

   cout << "pPb: " << pPbPath << endl;
   cout << "PbP: " << PbPPath << endl;

   TFile fpPb(pPbPath.c_str(), "READ");
   TFile fPbP(PbPPath.c_str(), "READ");
   if (fpPb.IsZombie() || fPbP.IsZombie()) {
      cerr << "Failed to open input files" << endl;
      return 1;
   }

   string dataKey = "hData_" + trkPtRange;
   string mixKey = "hMixData_" + trkPtRange;

   TH2D *pPb_SE = (TH2D *)((TH2D *)fpPb.Get(dataKey.c_str()))->Clone("pPb_SE");
   TH2D *pPb_ME = (TH2D *)((TH2D *)fpPb.Get(mixKey.c_str()))->Clone("pPb_ME");
   TH2D *PbP_SE = (TH2D *)((TH2D *)fPbP.Get(dataKey.c_str()))->Clone("PbP_SE");
   TH2D *PbP_ME = (TH2D *)((TH2D *)fPbP.Get(mixKey.c_str()))->Clone("PbP_ME");
   pPb_SE->SetDirectory(nullptr);
   pPb_ME->SetDirectory(nullptr);
   PbP_SE->SetDirectory(nullptr);
   PbP_ME->SetDirectory(nullptr);

   TH1D *pPb_SE_eta = ProjectAndNormalize(pPb_SE, "DeltaEta", "pPb_SE_eta");
   TH1D *pPb_ME_eta = ProjectAndNormalize(pPb_ME, "DeltaEta", "pPb_ME_eta");
   TH1D *PbP_SE_eta = ProjectAndNormalize(PbP_SE, "DeltaEta", "PbP_SE_eta");
   TH1D *PbP_ME_eta = ProjectAndNormalize(PbP_ME, "DeltaEta", "PbP_ME_eta");

   TH1D *pPb_SE_phi = ProjectAndNormalize(pPb_SE, "DeltaPhi", "pPb_SE_phi");
   TH1D *pPb_ME_phi = ProjectAndNormalize(pPb_ME, "DeltaPhi", "pPb_ME_phi");
   TH1D *PbP_SE_phi = ProjectAndNormalize(PbP_SE, "DeltaPhi", "PbP_SE_phi");
   TH1D *PbP_ME_phi = ProjectAndNormalize(PbP_ME, "DeltaPhi", "PbP_ME_phi");

   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   gStyle->SetPadTickX(1);
   gStyle->SetPadTickY(1);

   gSystem->mkdir(outputDir.c_str(), true);

   TCanvas *c = new TCanvas("c", "c", 1000, 1000);

   CellPads c1 = MakeCell(c, "se_eta", 0, 0);
   CellPads c2 = MakeCell(c, "me_eta", 1, 0);
   CellPads c3 = MakeCell(c, "se_phi", 0, 1);
   CellPads c4 = MakeCell(c, "me_phi", 1, 1);

   string yLabel = "1/N_{Z} d^{2}N/d#Deltay d#Delta#varphi";

   DrawMainPanel(c1.main, pPb_SE_eta, PbP_SE_eta, yLabel, "Same-event", -3.87, 3.87, true);
   DrawRatioPanel(c1.ratio, pPb_SE_eta, PbP_SE_eta, "#Delta y_{ch,Z}", -3.87, 3.87);

   DrawMainPanel(c2.main, pPb_ME_eta, PbP_ME_eta, yLabel, "Mixed-event", -3.87, 3.87, false);
   DrawRatioPanel(c2.ratio, pPb_ME_eta, PbP_ME_eta, "#Delta y_{ch,Z}", -3.87, 3.87);

   DrawMainPanel(c3.main, pPb_SE_phi, PbP_SE_phi, yLabel, "Same-event", -M_PI/2, 3*M_PI/2, false);
   DrawRatioPanel(c3.ratio, pPb_SE_phi, PbP_SE_phi, "#Delta#varphi_{ch,Z}", -M_PI/2, 3*M_PI/2);

   DrawMainPanel(c4.main, pPb_ME_phi, PbP_ME_phi, yLabel, "Mixed-event", -M_PI/2, 3*M_PI/2, false);
   DrawRatioPanel(c4.ratio, pPb_ME_phi, PbP_ME_phi, "#Delta#varphi_{ch,Z}", -M_PI/2, 3*M_PI/2);

   string outPath = Form("%s/%s_ZPT%s_trkPT%s-SE-ME-comparison.pdf",
       outputDir.c_str(), tag.c_str(), zPtRange.c_str(), trkPtRange.c_str());
   c->SaveAs(outPath.c_str());
   cout << "Saved: " << outPath << endl;

   return 0;
}
