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

#include "CommandLine.h"
#include "KylesPlotting.h"
#include "SetStyle.h"

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   gROOT->SetBatch(kTRUE);
   SetThesisStyle();
   gStyle->SetOptStat(0);

   string input8160 = CL.Get("Input8160");
   string input5020 = CL.Get("Input5020");
   string trkPtRange = CL.Get("TrackPTRange", "0.5_15");
   string outputBase = CL.Get("OutputBase", "plots/pp_energy/jewel_pp_diag");

   TFile *f8160 = TFile::Open(input8160.c_str());
   TFile *f5020 = TFile::Open(input5020.c_str());

   TH1D *hNZ8160 = (TH1D *)f8160->Get(("hNZData_" + trkPtRange).c_str());
   TH1D *hNZ5020 = (TH1D *)f5020->Get(("hNZData_" + trkPtRange).c_str());
   double nz8160 = hNZ8160->Integral();
   double nz5020 = hNZ5020->Integral();
   cout << "N_Z (8.16 TeV): " << nz8160 << endl;
   cout << "N_Z (5.02 TeV): " << nz5020 << endl;

   struct Obs { string name; int axis; double lo; double hi; string label; };
   vector<Obs> observables = {
      {"ZPt",  1, 15, 500, "p_{T}^{Z} (GeV)"},
      {"ZY",   2, -2.4, 2.4, "y_{Z}"},
      {"ZPhi", 3, -3.14159, 3.14159, "#phi_{Z}"},
   };

   TH3D *hZPtEtaPhi_8160 = (TH3D *)f8160->Get(("hZPtEtaPhi_" + trkPtRange).c_str());
   TH3D *hZPtEtaPhi_5020 = (TH3D *)f5020->Get(("hZPtEtaPhi_" + trkPtRange).c_str());

   for(auto &obs : observables)
   {
      TH1D *h8160 = nullptr, *h5020 = nullptr;
      if(obs.axis == 1) h8160 = hZPtEtaPhi_8160->ProjectionX(("h8160_" + obs.name).c_str());
      if(obs.axis == 2) h8160 = hZPtEtaPhi_8160->ProjectionY(("h8160_" + obs.name).c_str());
      if(obs.axis == 3) h8160 = hZPtEtaPhi_8160->ProjectionZ(("h8160_" + obs.name).c_str());
      if(obs.axis == 1) h5020 = hZPtEtaPhi_5020->ProjectionX(("h5020_" + obs.name).c_str());
      if(obs.axis == 2) h5020 = hZPtEtaPhi_5020->ProjectionY(("h5020_" + obs.name).c_str());
      if(obs.axis == 3) h5020 = hZPtEtaPhi_5020->ProjectionZ(("h5020_" + obs.name).c_str());

      h8160->Scale(1.0 / nz8160);
      h5020->Scale(1.0 / nz5020);
      divideByWidth(h8160);
      divideByWidth(h5020);

      vector<TH1 *> histograms = {h8160, h5020};

      TCanvas canvas(("c_" + obs.name).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSRatio(
         histograms, ("ZDiag_" + obs.name).c_str(),
         {"JEWEL pp 8.16 TeV", "JEWEL pp 5.02 TeV"},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         obs.label.c_str(), obs.lo, obs.hi,
         ("(1/N_{Z}) dN/d" + obs.label).c_str(), -1, -1,
         "5.02 / 8.16", 0.5, 1.5,
         0,
         (obs.name == "ZPt"), false, false
      );
      AddCMSHeader(pad, "Internal", false);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + obs.name + ".pdf").c_str());
   }

   TH3D *hTrkPtEtaPhi_8160 = (TH3D *)f8160->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());
   TH3D *hTrkPtEtaPhi_5020 = (TH3D *)f5020->Get(("hTrkPtEtaPhiData_" + trkPtRange).c_str());

   if(hTrkPtEtaPhi_8160 && hTrkPtEtaPhi_5020)
   {
      TH1D *hTrkPt_8160 = hTrkPtEtaPhi_8160->ProjectionX("hTrkPt8160");
      TH1D *hTrkPt_5020 = hTrkPtEtaPhi_5020->ProjectionX("hTrkPt5020");
      hTrkPt_8160->Scale(1.0 / nz8160);
      hTrkPt_5020->Scale(1.0 / nz5020);
      divideByWidth(hTrkPt_8160);
      divideByWidth(hTrkPt_5020);

      TCanvas c("cTrkPt", "", 600, 600);
      TPad *pad = (TPad *)plotCMSRatio(
         {(TH1 *)hTrkPt_8160, (TH1 *)hTrkPt_5020}, "TrkPt",
         {"JEWEL pp 8.16 TeV", "JEWEL pp 5.02 TeV"},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         "p_{T}^{ch} (GeV)", 0.5, 15,
         "(1/N_{Z}) dN/dp_{T}^{ch}", -1, -1,
         "5.02 / 8.16", 0.5, 1.5,
         0,
         true, false, false
      );
      AddCMSHeader(pad, "Internal", false);
      c.Update();
      c.SaveAs((outputBase + "-TrkPt.pdf").c_str());

      TH1D *hTrkEta_8160 = hTrkPtEtaPhi_8160->ProjectionY("hTrkEta8160");
      TH1D *hTrkEta_5020 = hTrkPtEtaPhi_5020->ProjectionY("hTrkEta5020");
      hTrkEta_8160->Scale(1.0 / nz8160);
      hTrkEta_5020->Scale(1.0 / nz5020);
      divideByWidth(hTrkEta_8160);
      divideByWidth(hTrkEta_5020);

      TCanvas cEta("cTrkEta", "", 600, 600);
      TPad *padEta = (TPad *)plotCMSRatio(
         {(TH1 *)hTrkEta_8160, (TH1 *)hTrkEta_5020}, "TrkEta",
         {"JEWEL pp 8.16 TeV", "JEWEL pp 5.02 TeV"},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         "#eta^{ch}", -2.4, 2.4,
         "(1/N_{Z}) dN/d#eta^{ch}", -1, -1,
         "5.02 / 8.16", 0.5, 1.5,
         0,
         false, false, false
      );
      AddCMSHeader(padEta, "Internal", false);
      cEta.Update();
      cEta.SaveAs((outputBase + "-TrkEta.pdf").c_str());
   }

   f8160->Close();
   f5020->Close();
   return 0;
}
