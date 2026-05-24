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

   string input8160 = CL.Get("Input8160");
   string input5020 = CL.Get("Input5020");
   string trkPtRange = CL.Get("TrackPTRange", "0.5_15");
   string outputBase = CL.Get("OutputBase", "plots/jewel_pp_energy");

   TFile *f8160 = TFile::Open(input8160.c_str());
   TFile *f5020 = TFile::Open(input5020.c_str());

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &obs : observables)
   {
      string histName = obs + "_Result" + trkPtRange;

      TH1D *h8160 = (TH1D *)f8160->Get(histName.c_str());
      TH1D *h5020 = (TH1D *)f5020->Get(histName.c_str());

      h8160 = (TH1D *)h8160->Clone(("h8160_" + obs).c_str());
      h5020 = (TH1D *)h5020->Clone(("h5020_" + obs).c_str());
      h8160->SetDirectory(nullptr);
      h5020->SetDirectory(nullptr);
      h8160->Scale(0.5);
      h5020->Scale(0.5);

      double xLo = (obs == "DeltaPhi") ? -1.5707 : -4.0;
      double xHi = (obs == "DeltaPhi") ?  4.7123 :  4.0;
      string xLabel = (obs == "DeltaPhi") ? "#Delta#phi_{ch,Z}" : "#Deltay_{ch,Z}";

      vector<TH1 *> histograms = {h8160, h5020};

      TCanvas canvas(("c_" + obs).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("JewelPP_" + obs).c_str(),
         {"JEWEL pp 8.16 TeV", "JEWEL pp 5.02 TeV"},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         xLabel.c_str(), xLo, xHi,
         ("d#LT#DeltaN_{ch}#GT/d" + xLabel).c_str(), -1, -1,
         "5.02 - 8.16", -0.2, 0.2,
         0,
         false, false, true,
         0.56
      );

      AddCMSHeader(pad, "Internal", false);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + obs + ".pdf").c_str());

      delete h8160;
      delete h5020;
   }

   f8160->Close();
   f5020->Close();
   return 0;
}
