#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH2D.h"
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

   string nominalPPbFileName = CL.Get("NominalPPb", "");
   string nominalPBPFileName = CL.Get("NominalPBP", "");
   string meWeightFileName = CL.Get("MEWeightFile", "");
   string outputBase = CL.Get("OutputBase", "plots/eventMixing/systematic");
   string collision = CL.Get("Collision", "pPbPbp");
   string zptRange = CL.Get("ZPTRange", "0_500");
   string trackRange = CL.Get("TrackPTRange", "0.5_15");

   TFile *nominalPPbFile = TFile::Open(nominalPPbFileName.c_str());
   TFile *nominalPBPFile = TFile::Open(nominalPBPFileName.c_str());
   TFile meWeightFile(meWeightFileName.c_str());
   TH2D *meWeight = (TH2D *)meWeightFile.Get("hMEWeight");

   if(nominalPPbFile == nullptr || nominalPPbFile->IsZombie()
      || nominalPBPFile == nullptr || nominalPBPFile->IsZombie()
      || meWeight == nullptr)
   {
      cerr << "Unable to open event mixing comparison inputs" << endl;
      return 1;
   }
   meWeight->SetDirectory(nullptr);

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile,
         observable, trackRange, observable + "_Nominal", false, true);
      TH1D *variation = BuildCombinedResultHistogramWithMEWeight(*nominalPPbFile, *nominalPBPFile,
         observable, trackRange, observable + "_MEWeighted", meWeight, true);

      if(nominal == nullptr || variation == nullptr)
      {
         cerr << "Missing " << observable << " histogram for event mixing comparison" << endl;
         delete nominal;
         delete variation;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, variation};
      pair<double, double> xRange = GetObservableRange(observable, true);
      pair<double, double> yRange = GetComparisonYRange(histograms, observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasEM" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
          histograms, ("EMComparison_" + observable).c_str(), {"Nominal", "ME weighted"},
           {cmsBlue, cmsRed}, {0, 0},
           {cmsBlue, cmsRed}, {20, 21},
           GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
           GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
           "ME weighted - nominal", differenceRange.first, differenceRange.second,
          0,
          false, false, true,
          0.62
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), GetCollisionLabel(collision).c_str());
      DrawKinematicLabels(pad, "Nominal vs ME weighted", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete variation;
   }

   delete meWeight;
   nominalPPbFile->Close(); delete nominalPPbFile;
   nominalPBPFile->Close(); delete nominalPBPFile;
   return 0;
}
