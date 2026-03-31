#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"

#include "CommandLine.h"
#include "KylesPlotting.h"
#include "SetStyle.h"

#include "PlotComparisonUtilities.h"

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   gROOT->SetBatch(kTRUE);
   SetThesisStyle();
   gStyle->SetOptStat(0);

   string nominalFileName = CL.Get("Nominal");
   string looseFileName = CL.Get("Loose");
   string tightFileName = CL.Get("Tight");
   string outputBase = CL.Get("OutputBase", "plots/trackSelection/systematic");
   string collision = CL.Get("Collision", "pPb");
   string zptRange = CL.Get("ZPTRange", "40_350");
   string trackRange = CL.Get("TrackPTRange", "2_500");

   TFile nominalFile(nominalFileName.c_str());
   TFile looseFile(looseFileName.c_str());
   TFile tightFile(tightFileName.c_str());

   if(nominalFile.IsZombie() || looseFile.IsZombie() || tightFile.IsZombie())
   {
      cerr << "Unable to open track-selection comparison inputs" << endl;
      return 1;
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = LoadResultHistogram(nominalFile, observable, trackRange, observable + "_Nominal");
      TH1D *loose = LoadResultHistogram(looseFile, observable, trackRange, observable + "_Loose");
      TH1D *tight = LoadResultHistogram(tightFile, observable, trackRange, observable + "_Tight");

      if(nominal == nullptr || loose == nullptr || tight == nullptr)
      {
         cerr << "Missing " << observable << " histogram for track-selection comparison" << endl;
         delete nominal;
         delete loose;
         delete tight;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, loose, tight};
      pair<double, double> xRange = GetObservableRange(observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasTrackSelection" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("TrackSelection_" + observable).c_str(),
         {"Nominal", "Loose", "Tight"},
         {cmsBlue, cmsRed, kGreen + 2}, {0, 0, 0},
         {cmsBlue, cmsRed, kGreen + 2}, {20, 21, 22},
         GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
         GetResultAxisLabel(observable).c_str(), -1, -1,
         "variation - nominal", differenceRange.first, differenceRange.second,
         0,
         false, false, true,
         0.58
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), collision.c_str());
      DrawKinematicLabels(pad, "Track-selection variations", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete loose;
      delete tight;
   }

   nominalFile.Close();
   looseFile.Close();
   tightFile.Close();
   return 0;
}
