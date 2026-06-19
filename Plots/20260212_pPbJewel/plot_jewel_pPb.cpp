#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
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

   string inputPPb = CL.Get("InputPPb");
   string inputPP = CL.Get("InputPP");
   string trkPtRange = CL.Get("TrackPTRange", "0.5_15");
   string outputBase = CL.Get("OutputBase", "plots/pPb/jewel_pPb");

   TFile *fPPb = TFile::Open(inputPPb.c_str());
   TFile *fPP = TFile::Open(inputPP.c_str());

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &obs : observables)
   {
      string histName = obs + "_Result" + trkPtRange;

      TH1D *hPPb = (TH1D *)fPPb->Get(histName.c_str());
      TH1D *hPP = (TH1D *)fPP->Get(histName.c_str());

      hPPb = (TH1D *)hPPb->Clone(("hPPb_" + obs).c_str());
      hPP = (TH1D *)hPP->Clone(("hPP_" + obs).c_str());
      hPPb->SetDirectory(nullptr);
      hPP->SetDirectory(nullptr);
      hPPb->Scale(0.5);
      hPP->Scale(0.5);

      double xLo = (obs == "DeltaPhi") ? -1.5707 : -4.0;
      double xHi = (obs == "DeltaPhi") ?  4.7123 :  4.0;
      string xLabel = (obs == "DeltaPhi") ? "#Delta#phi_{ch,Z}" : "#Deltay_{ch,Z}";

      vector<TH1 *> histograms = {hPPb, hPP};

      TCanvas canvas(("c_" + obs).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("JewelPPb_" + obs).c_str(),
         {"JEWEL pPb 8.16 TeV", "JEWEL pp 8.16 TeV"},
         {cmsRed, cmsBlue}, {0, 0},
         {cmsRed, cmsBlue}, {21, 20},
         xLabel.c_str(), xLo, xHi,
         ("d#LT#DeltaN_{ch}#GT/d" + xLabel).c_str(), -1, -1,
         "pPb - pp", -0.2, 0.2,
         0,
         false, false, true,
         0.56
      );

      AddCMSHeader(pad, "Internal", false);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + obs + ".pdf").c_str());

      delete hPPb;
      delete hPP;
   }

   fPPb->Close();
   fPP->Close();
   return 0;
}
