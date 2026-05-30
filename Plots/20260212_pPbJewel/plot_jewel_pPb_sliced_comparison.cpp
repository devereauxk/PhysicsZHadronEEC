#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include "CommandLine.h"
#include "KylesPlotting.h"
#include "SetStyle.h"

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   gROOT->SetBatch(kTRUE);
   SetThesisStyle();
   gStyle->SetOptStat(0);

   string inputOriginal = CL.Get("InputOriginal");
   string inputSliced   = CL.Get("InputSliced");
   string trkPtRange    = CL.Get("TrackPTRange", "0.5_15");
   string outputDir     = CL.Get("OutputDir", "plots/pPb_sliced");

   gSystem->mkdir(outputDir.c_str(), true);

   string labelOriginal = "JEWEL pPb 8.16 TeV (original)";
   string labelSliced   = "JEWEL pPb 8.16 TeV (p_{T}-hat sliced)";
   string ratioLabel    = "sliced / original";

   TFile *fOrig   = TFile::Open(inputOriginal.c_str());
   TFile *fSliced = TFile::Open(inputSliced.c_str());

   TH1D *hNZOrig   = (TH1D *)fOrig->Get(("hNZData_" + trkPtRange).c_str());
   TH1D *hNZSliced = (TH1D *)fSliced->Get(("hNZData_" + trkPtRange).c_str());
   double nzOrig   = hNZOrig->Integral();
   double nzSliced = hNZSliced->Integral();
   cout << "N_Z original: " << nzOrig << endl;
   cout << "N_Z sliced:   " << nzSliced << endl;

   // --- Z kinematic diagnostics ---
   struct Obs { string name; int axis; double lo; double hi; string label; };
   vector<Obs> zObs = {
      {"ZPt",  1, 15, 500, "p_{T}^{Z} (GeV)"},
      {"ZY",   2, -2.4, 2.4, "y_{Z}"},
      {"ZPhi", 3, -3.14159, 3.14159, "#phi_{Z}"},
   };

   TH3D *hZOrig   = (TH3D *)fOrig->Get(("hZPtEtaPhi_" + trkPtRange).c_str());
   TH3D *hZSliced = (TH3D *)fSliced->Get(("hZPtEtaPhi_" + trkPtRange).c_str());

   for(auto &obs : zObs)
   {
      TH1D *hO = nullptr, *hS = nullptr;
      if(obs.axis == 1) { hO = hZOrig->ProjectionX(("hOrig_" + obs.name).c_str()); hS = hZSliced->ProjectionX(("hSliced_" + obs.name).c_str()); }
      if(obs.axis == 2) { hO = hZOrig->ProjectionY(("hOrig_" + obs.name).c_str()); hS = hZSliced->ProjectionY(("hSliced_" + obs.name).c_str()); }
      if(obs.axis == 3) { hO = hZOrig->ProjectionZ(("hOrig_" + obs.name).c_str()); hS = hZSliced->ProjectionZ(("hSliced_" + obs.name).c_str()); }

      hO->Scale(1.0 / nzOrig);   divideByWidth(hO);
      hS->Scale(1.0 / nzSliced); divideByWidth(hS);

      TCanvas c(("c_" + obs.name).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSRatio(
         {(TH1 *)hO, (TH1 *)hS}, ("ZDiag_" + obs.name).c_str(),
         {labelOriginal.c_str(), labelSliced.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         obs.label.c_str(), obs.lo, obs.hi,
         ("(1/N_{Z}) dN/d" + obs.label).c_str(), -1, -1,
         ratioLabel.c_str(), 0.5, 1.5,
         0, (obs.name == "ZPt"), false, false
      );
      AddCMSHeader(pad, "Internal", false);
      c.Update();
      c.SaveAs((outputDir + "/pPb_sliced-" + obs.name + ".pdf").c_str());
   }

   // --- Track diagnostics ---
   TH3D *hTrkOrig   = (TH3D *)fOrig->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());
   TH3D *hTrkSliced = (TH3D *)fSliced->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());

   if(hTrkOrig && hTrkSliced)
   {
      TH1D *hTrkPtO = hTrkOrig->ProjectionX("hTrkPtOrig");
      TH1D *hTrkPtS = hTrkSliced->ProjectionX("hTrkPtSliced");
      hTrkPtO->Scale(1.0 / nzOrig);   divideByWidth(hTrkPtO);
      hTrkPtS->Scale(1.0 / nzSliced); divideByWidth(hTrkPtS);

      TCanvas cPt("cTrkPt", "", 600, 600);
      TPad *pad = (TPad *)plotCMSRatio(
         {(TH1 *)hTrkPtO, (TH1 *)hTrkPtS}, "TrkPt",
         {labelOriginal.c_str(), labelSliced.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         "p_{T}^{ch} (GeV)", 0.5, 15,
         "(1/N_{Z}) dN/dp_{T}^{ch}", -1, -1,
         ratioLabel.c_str(), 0.5, 1.5,
         0, true, false, false
      );
      AddCMSHeader(pad, "Internal", false);
      cPt.Update();
      cPt.SaveAs((outputDir + "/pPb_sliced-TrkPt.pdf").c_str());

      TH1D *hTrkEtaO = hTrkOrig->ProjectionY("hTrkEtaOrig");
      TH1D *hTrkEtaS = hTrkSliced->ProjectionY("hTrkEtaSliced");
      hTrkEtaO->Scale(1.0 / nzOrig);   divideByWidth(hTrkEtaO);
      hTrkEtaS->Scale(1.0 / nzSliced); divideByWidth(hTrkEtaS);

      TCanvas cEta("cTrkEta", "", 600, 600);
      TPad *padEta = (TPad *)plotCMSRatio(
         {(TH1 *)hTrkEtaO, (TH1 *)hTrkEtaS}, "TrkEta",
         {labelOriginal.c_str(), labelSliced.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         "#eta^{ch}", -2.4, 2.4,
         "(1/N_{Z}) dN/d#eta^{ch}", -1, -1,
         ratioLabel.c_str(), 0.5, 1.5,
         0, false, false, false
      );
      AddCMSHeader(padEta, "Internal", false);
      cEta.Update();
      cEta.SaveAs((outputDir + "/pPb_sliced-TrkEta.pdf").c_str());
   }

   // --- DeltaEta / DeltaPhi results ---
   for(const string &obs : {"DeltaEta", "DeltaPhi"})
   {
      string histName = obs + "_Result" + trkPtRange;
      TH1D *hO = (TH1D *)fOrig->Get(histName.c_str());
      TH1D *hS = (TH1D *)fSliced->Get(histName.c_str());
      hO = (TH1D *)hO->Clone(("hOrig_" + obs).c_str()); hO->SetDirectory(nullptr); hO->Scale(0.5);
      hS = (TH1D *)hS->Clone(("hSlic_" + obs).c_str()); hS->SetDirectory(nullptr); hS->Scale(0.5);

      double xLo    = (obs == "DeltaPhi") ? -1.5707 : -4.0;
      double xHi    = (obs == "DeltaPhi") ?  4.7123 :  4.0;
      string xLabel = (obs == "DeltaPhi") ? "#Delta#phi_{ch,Z}" : "#Deltay_{ch,Z}";

      TCanvas c(("c_" + obs).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         {(TH1 *)hO, (TH1 *)hS}, ("PPbSliced_" + obs).c_str(),
         {labelOriginal.c_str(), labelSliced.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         xLabel.c_str(), xLo, xHi,
         ("d#LT#DeltaN_{ch}#GT/d" + xLabel).c_str(), -1, -1,
         "sliced - original", -0.2, 0.2,
         0, false, false, true, 0.56
      );
      AddCMSHeader(pad, "Internal", false);
      c.Update();
      c.SaveAs((outputDir + "/pPb_sliced-" + obs + ".pdf").c_str());

      delete hO; delete hS;
   }

   fOrig->Close();
   fSliced->Close();
   return 0;
}
