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

   string inputPPb  = CL.Get("InputPPb");   // our pp 5020 (for pPb study)
   string inputPPbB = CL.Get("InputPPbB");  // v9 pp (for PbPb study)
   string trkPtRange = CL.Get("TrackPTRange", "0.5_15");
   string outputDir  = CL.Get("OutputDir", "plots/pp_skims");
   double dEtaRange  = CL.GetDouble("DEtaRange", 4.0);
   string labelPPb   = CL.Get("LabelA", "JEWEL pp 5.02 TeV (pPb study)");
   string labelPPbB  = CL.Get("LabelB", "JEWEL pp 5.02 TeV (PbPb study)");

   gSystem->mkdir(outputDir.c_str(), true);

   string ratioLabel = "PbPb study / pPb study";

   TFile *fPPb  = TFile::Open(inputPPb.c_str());
   TFile *fPPbB = TFile::Open(inputPPbB.c_str());

   TH1D *hNZPPb  = (TH1D *)fPPb->Get(("hNZData_" + trkPtRange).c_str());
   TH1D *hNZPPbB = (TH1D *)fPPbB->Get(("hNZData_" + trkPtRange).c_str());
   double nzPPb  = hNZPPb->Integral();
   double nzPPbB = hNZPPbB->Integral();
   cout << "N_Z (pPb study pp 5020): " << nzPPb << endl;
   cout << "N_Z (PbPb study pp 5020): " << nzPPbB << endl;

   // --- Z kinematic diagnostics ---
   struct Obs { string name; int axis; double lo; double hi; string label; };
   vector<Obs> zObs = {
      {"ZPt",  1, 15, 500, "p_{T}^{Z} (GeV)"},
      {"ZY",   2, -2.4, 2.4, "y_{Z}"},
      {"ZPhi", 3, -3.14159, 3.14159, "#phi_{Z}"},
   };

   TH3D *hZPPb  = (TH3D *)fPPb->Get(("hZPtEtaPhi_" + trkPtRange).c_str());
   TH3D *hZPPbB = (TH3D *)fPPbB->Get(("hZPtEtaPhi_" + trkPtRange).c_str());

   for(auto &obs : zObs)
   {
      TH1D *hA = nullptr, *hB = nullptr;
      if(obs.axis == 1) { hA = hZPPb->ProjectionX(("hPPb_" + obs.name).c_str()); hB = hZPPbB->ProjectionX(("hPPbB_" + obs.name).c_str()); }
      if(obs.axis == 2) { hA = hZPPb->ProjectionY(("hPPb_" + obs.name).c_str()); hB = hZPPbB->ProjectionY(("hPPbB_" + obs.name).c_str()); }
      if(obs.axis == 3) { hA = hZPPb->ProjectionZ(("hPPb_" + obs.name).c_str()); hB = hZPPbB->ProjectionZ(("hPPbB_" + obs.name).c_str()); }

      hA->Scale(1.0 / nzPPb);  divideByWidth(hA);
      hB->Scale(1.0 / nzPPbB); divideByWidth(hB);

      TCanvas c(("c_" + obs.name).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSRatio(
         {(TH1 *)hA, (TH1 *)hB}, ("ZDiag_" + obs.name).c_str(),
         {labelPPb.c_str(), labelPPbB.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         obs.label.c_str(), obs.lo, obs.hi,
         ("(1/N_{Z}) dN/d" + obs.label).c_str(), -1, -1,
         ratioLabel.c_str(), 0.5, 1.5,
         0, (obs.name == "ZPt"), false, false
      );
      AddCMSHeader(pad, "Internal", false);
      c.Update();
      c.SaveAs((outputDir + "/pp_skims-" + obs.name + ".pdf").c_str());
   }

   // --- Track diagnostics ---
   TH3D *hTrkPPb  = (TH3D *)fPPb->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());
   TH3D *hTrkPPbB = (TH3D *)fPPbB->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());

   if(hTrkPPb && hTrkPPbB)
   {
      TH1D *hTrkPtA = hTrkPPb->ProjectionX("hTrkPtPPb");
      TH1D *hTrkPtB = hTrkPPbB->ProjectionX("hTrkPtPPbB");
      hTrkPtA->Scale(1.0 / nzPPb);  divideByWidth(hTrkPtA);
      hTrkPtB->Scale(1.0 / nzPPbB); divideByWidth(hTrkPtB);

      TCanvas cPt("cTrkPt", "", 600, 600);
      TPad *pad = (TPad *)plotCMSRatio(
         {(TH1 *)hTrkPtA, (TH1 *)hTrkPtB}, "TrkPt",
         {labelPPb.c_str(), labelPPbB.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         "p_{T}^{ch} (GeV)", 0.5, 15,
         "(1/N_{Z}) dN/dp_{T}^{ch}", -1, -1,
         ratioLabel.c_str(), 0.5, 1.5,
         0, true, false, false
      );
      AddCMSHeader(pad, "Internal", false);
      cPt.Update();
      cPt.SaveAs((outputDir + "/pp_skims-TrkPt.pdf").c_str());

      TH1D *hTrkEtaA = hTrkPPb->ProjectionY("hTrkEtaPPb");
      TH1D *hTrkEtaB = hTrkPPbB->ProjectionY("hTrkEtaPPbB");
      hTrkEtaA->Scale(1.0 / nzPPb);  divideByWidth(hTrkEtaA);
      hTrkEtaB->Scale(1.0 / nzPPbB); divideByWidth(hTrkEtaB);

      TCanvas cEta("cTrkEta", "", 600, 600);
      TPad *padEta = (TPad *)plotCMSRatio(
         {(TH1 *)hTrkEtaA, (TH1 *)hTrkEtaB}, "TrkEta",
         {labelPPb.c_str(), labelPPbB.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         "#eta^{ch}", -2.4, 2.4,
         "(1/N_{Z}) dN/d#eta^{ch}", -1, -1,
         ratioLabel.c_str(), 0.5, 1.5,
         0, false, false, false
      );
      AddCMSHeader(padEta, "Internal", false);
      cEta.Update();
      cEta.SaveAs((outputDir + "/pp_skims-TrkEta.pdf").c_str());
   }

   // --- DeltaEta / DeltaPhi results ---
   for(const string &obs : {"DeltaEta", "DeltaPhi"})
   {
      string histName = obs + "_Result" + trkPtRange;
      TH1D *hA = (TH1D *)fPPb->Get(histName.c_str());
      TH1D *hB = (TH1D *)fPPbB->Get(histName.c_str());
      hA = (TH1D *)hA->Clone(("hPPb_" + obs).c_str()); hA->SetDirectory(nullptr); hA->Scale(0.5);
      hB = (TH1D *)hB->Clone(("hPPbB_" + obs).c_str()); hB->SetDirectory(nullptr); hB->Scale(0.5);

      double xLo    = (obs == "DeltaPhi") ? -1.5707 : -dEtaRange;
      double xHi    = (obs == "DeltaPhi") ?  4.7123 :  dEtaRange;
      string xLabel = (obs == "DeltaPhi") ? "#Delta#phi_{ch,Z}" : "#Deltay_{ch,Z}";

      TCanvas c(("c_" + obs).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         {(TH1 *)hA, (TH1 *)hB}, ("PPSkims_" + obs).c_str(),
         {labelPPb.c_str(), labelPPbB.c_str()},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         xLabel.c_str(), xLo, xHi,
         ("d#LT#DeltaN_{ch}#GT/d" + xLabel).c_str(), -1, -1,
         "PbPb study - pPb study", -0.2, 0.2,
         0, false, false, true, 0.56
      );
      AddCMSHeader(pad, "Internal", false);
      c.Update();
      c.SaveAs((outputDir + "/pp_skims-" + obs + ".pdf").c_str());

      delete hA; delete hB;
   }

   fPPb->Close();
   fPPbB->Close();
   return 0;
}
