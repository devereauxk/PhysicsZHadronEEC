#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
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

void PlotRatio3(vector<TH1D*> hists, vector<string> labels,
   const string &xLabel, double xLo, double xHi,
   const string &yLabel, const string &rLabel,
   double rLo, double rHi, const string &outPath,
   bool logX = false, bool logY = false)
{
   vector<TH1*> vh(hists.begin(), hists.end());
   TCanvas canvas("c", "", 600, 600);
   TPad *pad = (TPad *)plotCMSRatio(
      vh, "nPDF",
      labels,
      {cmsBlue, cmsRed, kGreen+2}, {0, 0, 0},
      {cmsBlue, cmsRed, kGreen+2}, {20, 21, 22},
      xLabel.c_str(), xLo, xHi,
      yLabel.c_str(), -1, -1,
      rLabel.c_str(), rLo, rHi,
      0,
      logX, logY, true,
      0.42
   );
   AddCMSHeader(pad, "Internal", false);
   canvas.Update();
   canvas.SaveAs(outPath.c_str());
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   gROOT->SetBatch(kTRUE);
   SetThesisStyle();
   gStyle->SetOptStat(0);

   string inputEPPS21_nosub  = CL.Get("InputEPPS21Nosub");
   string inputCT18_nosub    = CL.Get("InputCT18Nosub");
   string inputNNPDF31_nosub = CL.Get("InputNNPDF31Nosub");
   string inputEPPS21_result  = CL.Get("InputEPPS21Result");
   string inputCT18_result    = CL.Get("InputCT18Result");
   string inputNNPDF31_result = CL.Get("InputNNPDF31Result");
   string trkPtRange = CL.Get("TrackPTRange", "0.5_15");
   string outputBase = CL.Get("OutputBase", "plots/nPDF/nPDF");

   TFile *fE_nosub  = TFile::Open(inputEPPS21_nosub.c_str());
   TFile *fC_nosub  = TFile::Open(inputCT18_nosub.c_str());
   TFile *fN_nosub  = TFile::Open(inputNNPDF31_nosub.c_str());
   TFile *fE_result = TFile::Open(inputEPPS21_result.c_str());
   TFile *fC_result = TFile::Open(inputCT18_result.c_str());
   TFile *fN_result = TFile::Open(inputNNPDF31_result.c_str());

   double nzE = ((TH1D *)fE_nosub->Get(("hNZData_" + trkPtRange).c_str()))->Integral();
   double nzC = ((TH1D *)fC_nosub->Get(("hNZData_" + trkPtRange).c_str()))->Integral();
   double nzN = ((TH1D *)fN_nosub->Get(("hNZData_" + trkPtRange).c_str()))->Integral();
   cout << "N_Z  EPPS21: " << nzE << "  CT18ANLO: " << nzC << "  NNPDF31: " << nzN << endl;

   vector<string> labels = {"CT18ANLO", "EPPS21", "NNPDF31"};
   string ratioLabel = "X / CT18";

   // --- Z kinematic distributions (from nosub TH3D) ---
   TH3D *hZE = (TH3D *)fE_nosub->Get(("hZPtEtaPhi_" + trkPtRange).c_str());
   TH3D *hZC = (TH3D *)fC_nosub->Get(("hZPtEtaPhi_" + trkPtRange).c_str());
   TH3D *hZN = (TH3D *)fN_nosub->Get(("hZPtEtaPhi_" + trkPtRange).c_str());

   auto project3 = [](TH3D *h3, int axis, const string &tag) -> TH1D* {
      TH1D *h = nullptr;
      if(axis == 1) h = h3->ProjectionX(tag.c_str());
      if(axis == 2) h = h3->ProjectionY(tag.c_str());
      if(axis == 3) h = h3->ProjectionZ(tag.c_str());
      h->SetDirectory(nullptr);
      return h;
   };

   auto normAndScale = [](TH1D *h, double nz) {
      h->Scale(1.0 / nz);
      divideByWidth(h);
   };

   // ZPt
   {
      TH1D *hC = project3(hZC, 1, "hZPt_C");
      TH1D *hE = project3(hZE, 1, "hZPt_E");
      TH1D *hN = project3(hZN, 1, "hZPt_N");
      normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
      PlotRatio3({hC, hE, hN}, labels,
         "p_{T}^{Z} (GeV)", 15, 500,
         "(1/N_{Z}) dN/dp_{T}^{Z}", ratioLabel, 0.8, 1.2,
         outputBase + "-ZPt.pdf", true);
   }
   // ZY
   {
      TH1D *hC = project3(hZC, 2, "hZY_C");
      TH1D *hE = project3(hZE, 2, "hZY_E");
      TH1D *hN = project3(hZN, 2, "hZY_N");
      normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
      PlotRatio3({hC, hE, hN}, labels,
         "y_{Z}", -2.4, 2.4,
         "(1/N_{Z}) dN/dy_{Z}", ratioLabel, 0.8, 1.2,
         outputBase + "-ZEta.pdf");
   }

   // --- Track kinematic distributions ---
   TH3D *hTrkE = (TH3D *)fE_nosub->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());
   TH3D *hTrkC = (TH3D *)fC_nosub->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());
   TH3D *hTrkN = (TH3D *)fN_nosub->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());

   if(hTrkE && hTrkC && hTrkN)
   {
      // TrkPt
      {
         TH1D *hC = project3(hTrkC, 1, "hTrkPt_C");
         TH1D *hE = project3(hTrkE, 1, "hTrkPt_E");
         TH1D *hN = project3(hTrkN, 1, "hTrkPt_N");
         normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
         PlotRatio3({hC, hE, hN}, labels,
            "p_{T}^{ch} (GeV)", 0.5, 15,
            "(1/N_{Z}) dN/dp_{T}^{ch}", ratioLabel, 0.8, 1.2,
            outputBase + "-TrkPt.pdf", true);
      }
      // TrkEta
      {
         TH1D *hC = project3(hTrkC, 2, "hTrkEta_C");
         TH1D *hE = project3(hTrkE, 2, "hTrkEta_E");
         TH1D *hN = project3(hTrkN, 2, "hTrkEta_N");
         normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
         PlotRatio3({hC, hE, hN}, labels,
            "#eta^{ch}", -2.4, 2.4,
            "(1/N_{Z}) dN/d#eta^{ch}", ratioLabel, 0.8, 1.2,
            outputBase + "-TrkEta.pdf");
      }
   }

   // --- Signal-event and mixed-event 2D -> 1D projections ---
   auto project2D = [](TFile *f, const string &histName, int axis, const string &tag) -> TH1D* {
      TH2D *h2 = (TH2D *)f->Get(histName.c_str());
      if(!h2) return nullptr;
      TH1D *h = (axis == 1) ? h2->ProjectionX(tag.c_str()) : h2->ProjectionY(tag.c_str());
      h->SetDirectory(nullptr);
      return h;
   };

   string hDataName = "hData_" + trkPtRange;
   string hMixName  = "hMixData_" + trkPtRange;

   // DeltaEta SE (project along x = DeltaEta, integrate all DeltaPhi)
   {
      TH1D *hC = project2D(fC_nosub, hDataName, 1, "hSE_DEta_C");
      TH1D *hE = project2D(fE_nosub, hDataName, 1, "hSE_DEta_E");
      TH1D *hN = project2D(fN_nosub, hDataName, 1, "hSE_DEta_N");
      if(hC && hE && hN) {
         normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
         PlotRatio3({hC, hE, hN}, labels,
            "#Deltay_{ch,Z}", -3.87, 3.87,
            "(1/N_{Z}) dN/d#Deltay_{ch,Z}", ratioLabel, 0.9, 1.1,
            outputBase + "-DeltaEta-SE.pdf");
      }
   }
   // DeltaPhi SE
   {
      TH1D *hC = project2D(fC_nosub, hDataName, 2, "hSE_DPhi_C");
      TH1D *hE = project2D(fE_nosub, hDataName, 2, "hSE_DPhi_E");
      TH1D *hN = project2D(fN_nosub, hDataName, 2, "hSE_DPhi_N");
      if(hC && hE && hN) {
         normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
         PlotRatio3({hC, hE, hN}, labels,
            "#Delta#phi_{ch,Z}", -1.5708, 4.7124,
            "(1/N_{Z}) dN/d#Delta#phi_{ch,Z}", ratioLabel, 0.9, 1.1,
            outputBase + "-DeltaPhi-SE.pdf");
      }
   }
   // DeltaEta ME
   {
      TH1D *hC = project2D(fC_nosub, hMixName, 1, "hME_DEta_C");
      TH1D *hE = project2D(fE_nosub, hMixName, 1, "hME_DEta_E");
      TH1D *hN = project2D(fN_nosub, hMixName, 1, "hME_DEta_N");
      if(hC && hE && hN) {
         normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
         PlotRatio3({hC, hE, hN}, labels,
            "#Deltay_{ch,Z}", -3.87, 3.87,
            "(1/N_{Z}) dN/d#Deltay_{ch,Z} (mixed)", ratioLabel, 0.9, 1.1,
            outputBase + "-DeltaEta-ME.pdf");
      }
   }
   // DeltaPhi ME
   {
      TH1D *hC = project2D(fC_nosub, hMixName, 2, "hME_DPhi_C");
      TH1D *hE = project2D(fE_nosub, hMixName, 2, "hME_DPhi_E");
      TH1D *hN = project2D(fN_nosub, hMixName, 2, "hME_DPhi_N");
      if(hC && hE && hN) {
         normAndScale(hC, nzC); normAndScale(hE, nzE); normAndScale(hN, nzN);
         PlotRatio3({hC, hE, hN}, labels,
            "#Delta#phi_{ch,Z}", -1.5708, 4.7124,
            "(1/N_{Z}) dN/d#Delta#phi_{ch,Z} (mixed)", ratioLabel, 0.9, 1.1,
            outputBase + "-DeltaPhi-ME.pdf");
      }
   }

   // --- Background-subtracted result (difference panel) ---
   for(const string &obs : {"DeltaEta", "DeltaPhi"})
   {
      string histName = obs + "_Result" + trkPtRange;
      TH1D *hC = (TH1D *)fC_result->Get(histName.c_str());
      TH1D *hE = (TH1D *)fE_result->Get(histName.c_str());
      TH1D *hN = (TH1D *)fN_result->Get(histName.c_str());
      if(!hC || !hE || !hN) { cout << "Missing " << histName << endl; continue; }

      hC = (TH1D *)hC->Clone(("hRes_C_" + obs).c_str()); hC->SetDirectory(nullptr);
      hE = (TH1D *)hE->Clone(("hRes_E_" + obs).c_str()); hE->SetDirectory(nullptr);
      hN = (TH1D *)hN->Clone(("hRes_N_" + obs).c_str()); hN->SetDirectory(nullptr);
      hC->Scale(0.5); hE->Scale(0.5); hN->Scale(0.5);

      if(obs == "DeltaEta") { Symmetrize1DEta(hC); Symmetrize1DEta(hE); Symmetrize1DEta(hN); }
      if(obs == "DeltaPhi") { Symmetrize1DPhi(hC); Symmetrize1DPhi(hE); Symmetrize1DPhi(hN); }

      double xLo = (obs == "DeltaPhi") ? -1.5708 : -3.87;
      double xHi = (obs == "DeltaPhi") ?  4.7124 :  3.87;
      string xLabel = (obs == "DeltaPhi") ? "#Delta#phi_{ch,Z}" : "#Deltay_{ch,Z}";

      vector<TH1*> vh = {(TH1*)hC, (TH1*)hE, (TH1*)hN};
      TCanvas canvas("c", "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         vh, "nPDF_result",
         labels,
         {cmsBlue, cmsRed, kGreen+2}, {0, 0, 0},
         {cmsBlue, cmsRed, kGreen+2}, {20, 21, 22},
         xLabel.c_str(), xLo, xHi,
         ("d#LT#DeltaN_{ch}#GT/d" + xLabel).c_str(), -1, -1,
         "X #minus CT18", -0.01, 0.01,
         0,
         false, false, true,
         0.42
      );
      AddCMSHeader(pad, "Internal", false);
      canvas.Update();
      canvas.SaveAs((outputBase + "-" + obs + "-Result.pdf").c_str());
   }

   fE_nosub->Close(); fC_nosub->Close(); fN_nosub->Close();
   fE_result->Close(); fC_result->Close(); fN_result->Close();
   return 0;
}
