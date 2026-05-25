// PlotNmixSigma.cpp
// Average jackknife statistical uncertainty vs N_mix for pp and combined pPb.
// Two output PDFs: DeltaEta and DeltaPhi.

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TAxis.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"

#include "CommandLine.h"
#include "KylesPlotting.h"
#include "SetStyle.h"

struct JackknifeProjectionContribution {
   double SignalNZ = 0, MixNZ = 0;
   vector<float> SignalEta, MixEta, SignalPhi, MixPhi;
};

void AppendJackknifeProjectionContributions(TFile *file, const string &treeName,
   vector<JackknifeProjectionContribution> &events)
{
   if (!file) return;
   TTree *tree = (TTree *)file->Get(treeName.c_str());
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
   for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
      tree->GetEntry(entry);
      JackknifeProjectionContribution c;
      c.SignalNZ = signalNZ; c.MixNZ = mixNZ;
      c.SignalEta = *signalEta; c.MixEta = *mixEta;
      c.SignalPhi = *signalPhi; c.MixPhi = *mixPhi;
      events.push_back(c);
   }
}

vector<double> ComputeProjectedJackknifeSigma(const vector<JackknifeProjectionContribution> &events,
   const TH1D *fullHistogram, bool useEta)
{
   int nBins = fullHistogram->GetNbinsX();
   vector<double> sigma(nBins, 0);
   if (events.size() < 2) return sigma;
   double totalSignalNZ = 0, totalMixNZ = 0;
   vector<double> fullSignal(nBins, 0), fullMix(nBins, 0);
   for (const auto &e : events) {
      totalSignalNZ += e.SignalNZ;
      totalMixNZ += e.MixNZ;
      for (int i = 0; i < nBins; ++i) {
         fullSignal[i] += (useEta ? e.SignalEta[i] : e.SignalPhi[i]);
         fullMix[i]    += (useEta ? e.MixEta[i]    : e.MixPhi[i]);
      }
   }
   int validEvents = 0;
   for (const auto &e : events)
      if (totalSignalNZ - e.SignalNZ > 0 && totalMixNZ - e.MixNZ > 0) validEvents++;
   if (validEvents < 2) return sigma;
   for (int i = 1; i <= nBins; ++i) {
      double fullValue = fullSignal[i-1] / totalSignalNZ - fullMix[i-1] / totalMixNZ;
      double varianceSum = 0;
      for (const auto &e : events) {
         if (totalSignalNZ - e.SignalNZ <= 0 || totalMixNZ - e.MixNZ <= 0) continue;
         double sigWO = (fullSignal[i-1] - (useEta ? e.SignalEta[i-1] : e.SignalPhi[i-1])) / (totalSignalNZ - e.SignalNZ);
         double mixWO = (fullMix[i-1]    - (useEta ? e.MixEta[i-1]    : e.MixPhi[i-1]))    / (totalMixNZ    - e.MixNZ);
         double delta = (sigWO - mixWO) - fullValue;
         varianceSum += delta * delta;
      }
      sigma[i-1] = sqrt((validEvents - 1.0) / validEvents * varianceSum);
   }
   return sigma;
}

// Average bin errors over bins whose center lies in (xMin, xMax).
double AverageBinErrors(TH1D *h, double xMin, double xMax)
{
   double sum = 0; int count = 0;
   for (int i = 1; i <= h->GetNbinsX(); i++) {
      double c = h->GetBinCenter(i);
      if (c > xMin && c < xMax) { sum += h->GetBinError(i); count++; }
   }
   return count > 0 ? sum / count : 0;
}

// Average combined-pPb jackknife sigma over bins whose center lies in (xMin, xMax).
double AverageCombinedPPbSigma(TFile *fPPb, TFile *fPbP,
   const string &trackRange, bool useEta, double xMin, double xMax)
{
   string treeName = "JackknifeProjection" + trackRange;
   vector<JackknifeProjectionContribution> events;
   AppendJackknifeProjectionContributions(fPPb, treeName, events);
   AppendJackknifeProjectionContributions(fPbP, treeName, events);
   if (events.size() < 2) return 0;

   string histName = (useEta ? "DeltaEta" : "DeltaPhi");
   histName += "_Result" + trackRange;
   TH1D *hRef = nullptr;
   if (fPPb) hRef = (TH1D *)fPPb->Get(histName.c_str());
   if (!hRef && fPbP) hRef = (TH1D *)fPbP->Get(histName.c_str());
   if (!hRef) return 0;

   vector<double> sigma = ComputeProjectedJackknifeSigma(events, hRef, useEta);
   double sum = 0; int count = 0;
   for (int i = 0; i < (int)sigma.size(); i++) {
      double center = hRef->GetBinCenter(i + 1);
      if (center > xMin && center < xMax) {
         sum += sigma[i] / hRef->GetBinWidth(i + 1) * 0.5;
         count++;
      }
   }
   return count > 0 ? sum / count : 0;
}

void MakeSigmaVsNmixPlot(
   const vector<int>    &nmixValues,
   const vector<double> &ppSigmas,
   const vector<double> &pPbSigmas,
   const string &observable,
   const string &outputPath)
{
   gStyle->SetOptStat(0);
   TCanvas *c = new TCanvas("c", "", 600, 500);
   c->SetLeftMargin(0.14);
   c->SetBottomMargin(0.13);
   c->SetRightMargin(0.04);
   c->SetTopMargin(0.06);

   int N = nmixValues.size();
   vector<double> x(N);
   for (int i = 0; i < N; i++) x[i] = nmixValues[i];

   TGraph *gPP  = new TGraph(N, x.data(), ppSigmas.data());
   TGraph *gPPb = new TGraph(N, x.data(), pPbSigmas.data());

   // pp: blue filled circle
   gPP->SetMarkerStyle(20); gPP->SetMarkerSize(1.3);
   gPP->SetMarkerColor(kBlue+1); gPP->SetLineColor(kBlue+1); gPP->SetLineWidth(2);

   // pPb: red filled square
   gPPb->SetMarkerStyle(21); gPPb->SetMarkerSize(1.3);
   gPPb->SetMarkerColor(kRed+1); gPPb->SetLineColor(kRed+1); gPPb->SetLineWidth(2);

   // draw frame
   double yMax = 0;
   for (auto v : ppSigmas)  if (v > yMax) yMax = v;
   for (auto v : pPbSigmas) if (v > yMax) yMax = v;
   yMax *= 1.35;

   TH1F *frame = c->DrawFrame(0, 0, 22, yMax);
   frame->GetXaxis()->SetTitle("N_{Mix}");
   frame->GetYaxis()->SetTitle("<#sigma>");
   frame->GetXaxis()->SetTitleSize(0.05);
   frame->GetYaxis()->SetTitleSize(0.05);
   frame->GetXaxis()->SetLabelSize(0.044);
   frame->GetYaxis()->SetLabelSize(0.044);
   frame->GetXaxis()->SetTitleOffset(1.1);
   frame->GetYaxis()->SetTitleOffset(1.25);

   // draw x ticks only at actual nmix values
   frame->GetXaxis()->SetNdivisions(505);

   gPP->Draw("LP same");
   gPPb->Draw("LP same");

   TLegend *leg = new TLegend(0.55, 0.75, 0.93, 0.92);
   leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.042);
   leg->AddEntry(gPP,  "pp (8.16 TeV)",  "lp");
   leg->AddEntry(gPPb, "pPb (8.16 TeV)", "lp");
   leg->Draw();

   TLatex tex; tex.SetNDC(); tex.SetTextSize(0.042);
   string label = (observable == "DeltaEta") ? "#Delta#eta_{ch,Z}" : "#Delta#phi_{ch,Z}";
   tex.DrawLatex(0.17, 0.88, label.c_str());

   c->SaveAs(outputPath.c_str());
   printf("Saved: %s\n", outputPath.c_str());

   delete gPP; delete gPPb; delete leg; delete c;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);
   gROOT->SetBatch(kTRUE);
   SetThesisStyle();
   gStyle->SetOptStat(0);

   string studyDir   = CL.Get("StudyDir",     "plots");
   string centralDir = CL.Get("CentralDir",   "plots");
   string outputDir  = CL.Get("OutputDir",    "plots/nmixStudy");
   string trackRange = CL.Get("TrackPTRange", "0.5_15");
   string zptRange   = CL.Get("ZPTRange",     "0_500");
   string ppTagBase  = CL.Get("PPTagBase",    "EEV5_ZV9_trkV28_nmix");
   string ppbTagBase = CL.Get("PPBTagBase",   "ZV9_trkV28_nmix");

   vector<int> nmixValues = {1, 5, 10, 15, 20};

   // Build file path: nmix10 lives in centralDir, others in studyDir.
   auto resultPath = [&](const string &prefix, const string &tagBase, int nmix) {
      string dir = (nmix == 10) ? centralDir : studyDir;
      return dir + "/" + prefix + "_trkResidual_" + tagBase + to_string(nmix)
             + "_ZPT" + zptRange + "-result.root";
   };

   vector<double> ppEtaSigma, ppPhiSigma, pPbEtaSigma, pPbPhiSigma;

   for (int nmix : nmixValues) {
      // --- pp ---
      string ppFile = resultPath("pp", ppTagBase, nmix);
      TFile *fPP = TFile::Open(ppFile.c_str());
      if (!fPP || fPP->IsZombie()) {
         cerr << "Cannot open " << ppFile << endl;
         ppEtaSigma.push_back(0); ppPhiSigma.push_back(0);
      } else {
         TH1D *hEta = (TH1D *)fPP->Get(("DeltaEta_Result" + trackRange).c_str());
         TH1D *hPhi = (TH1D *)fPP->Get(("DeltaPhi_Result" + trackRange).c_str());
         ppEtaSigma.push_back(hEta ? AverageBinErrors(hEta, 0.0, 4.0)  : 0);
         ppPhiSigma.push_back(hPhi ? AverageBinErrors(hPhi, 0.0, M_PI) : 0);
         fPP->Close(); delete fPP;
      }

      // --- combined pPb ---
      string pPbFile = resultPath("pPb", ppbTagBase, nmix);
      string pbpFile = resultPath("PbP", ppbTagBase, nmix);
      TFile *fPPb = TFile::Open(pPbFile.c_str());
      TFile *fPbP = TFile::Open(pbpFile.c_str());
      if ((!fPPb || fPPb->IsZombie()) && (!fPbP || fPbP->IsZombie())) {
         cerr << "Cannot open pPb/PbP files for nmix=" << nmix << endl;
         pPbEtaSigma.push_back(0); pPbPhiSigma.push_back(0);
      } else {
         pPbEtaSigma.push_back(AverageCombinedPPbSigma(fPPb, fPbP, trackRange, true,  0.0, 4.0));
         pPbPhiSigma.push_back(AverageCombinedPPbSigma(fPPb, fPbP, trackRange, false, 0.0, M_PI));
      }
      if (fPPb) { fPPb->Close(); delete fPPb; }
      if (fPbP) { fPbP->Close(); delete fPbP; }
   }

   MakeSigmaVsNmixPlot(nmixValues, ppEtaSigma, pPbEtaSigma, "DeltaEta",
      outputDir + "/NmixSigma_ZPT" + zptRange + "_trkPT" + trackRange + "-DeltaEta.pdf");
   MakeSigmaVsNmixPlot(nmixValues, ppPhiSigma, pPbPhiSigma, "DeltaPhi",
      outputDir + "/NmixSigma_ZPT" + zptRange + "_trkPT" + trackRange + "-DeltaPhi.pdf");

   return 0;
}
