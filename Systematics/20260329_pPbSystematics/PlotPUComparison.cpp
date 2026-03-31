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
   string variationFileName = CL.Get("Variation");
   string outputBase = CL.Get("OutputBase", "plots/pu/systematic");
   string collision = CL.Get("Collision", "pPb");
   string zptRange = CL.Get("ZPTRange", "40_350");
   string trackRange = CL.Get("TrackPTRange", "2_500");

   TFile nominalFile(nominalFileName.c_str());
   TFile variationFile(variationFileName.c_str());

   if(nominalFile.IsZombie() || variationFile.IsZombie())
   {
      cerr << "Unable to open PU comparison inputs" << endl;
      return 1;
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = LoadResultHistogram(nominalFile, observable, trackRange, observable + "_Nominal");
      TH1D *variation = LoadResultHistogram(variationFile, observable, trackRange, observable + "_PU");

      if(nominal == nullptr || variation == nullptr)
      {
         cerr << "Missing " << observable << " histogram for PU comparison" << endl;
         delete nominal;
         delete variation;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, variation};
      pair<double, double> xRange = GetObservableRange(observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasPU" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("PUComparison_" + observable).c_str(), {"Nominal", "PU=1"},
          {cmsBlue, cmsRed}, {0, 0},
          {cmsBlue, cmsRed}, {20, 21},
          GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
          GetResultAxisLabel(observable).c_str(), -1, -1,
          "PU=1 - nominal", differenceRange.first, differenceRange.second,
          0,
          false, false, true,
          0.62
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), collision.c_str());
      DrawKinematicLabels(pad, "Nominal vs PU=1", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete variation;
   }

   nominalFile.Close();
   variationFile.Close();
   return 0;
}
