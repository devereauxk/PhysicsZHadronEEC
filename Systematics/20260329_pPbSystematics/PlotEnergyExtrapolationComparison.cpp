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

   string nominalFileName = CL.Get("Nominal", "");
   string variationFileName = CL.Get("Variation", "");
   string outputBase = CL.Get("OutputBase", "plots/energyExtrapolation/systematic");
   string collision = CL.Get("Collision", "pp");
   string zptRange = CL.Get("ZPTRange", "5_500");
   string trackRange = CL.Get("TrackPTRange", "0.5_500");
   bool useModified12x12 = CL.GetBool("UseModified12x12", false);

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *variationFile = (variationFileName != "") ? TFile::Open(variationFileName.c_str()) : nullptr;

   if((nominalFile != nullptr && nominalFile->IsZombie())
      || (variationFile != nullptr && variationFile->IsZombie()))
   {
      cerr << "Unable to open energy-extrapolation comparison inputs" << endl;
      return 1;
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      TH1D *variation = nullptr;
      if(nominalFile != nullptr)
         nominal = LoadResultHistogram(*nominalFile, observable, trackRange, observable + "_Nominal", useModified12x12);
      if(variationFile != nullptr)
         variation = LoadResultHistogram(*variationFile, observable, trackRange, observable + "_EnergyExtrapolation", useModified12x12);

      if(nominal == nullptr || variation == nullptr)
      {
         cerr << "Missing " << observable << " histogram for energy-extrapolation comparison" << endl;
         delete nominal;
         delete variation;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, variation};
      pair<double, double> xRange = GetObservableRange(observable, useModified12x12);
      pair<double, double> yRange = GetComparisonYRange(histograms, observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasEnergyExtrapolation" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("EnergyExtrapolationComparison_" + observable).c_str(),
         {"Nominal", "MC-driven"},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
         GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
         "MC-driven - nominal", differenceRange.first, differenceRange.second,
         0,
         false, false, true,
         (observable == "DeltaPhi") ? 0.25f : 0.62f,
         (observable == "DeltaPhi") ? 0.55f : 0.70f
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), GetCollisionLabel(collision).c_str());
      DrawKinematicLabels(pad, "Nominal vs MC-driven", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete variation;
   }

   if(nominalFile != nullptr) { nominalFile->Close(); delete nominalFile; }
   if(variationFile != nullptr) { variationFile->Close(); delete variationFile; }
   return 0;
}
