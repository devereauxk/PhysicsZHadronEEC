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
   string nominalPPbFileName = CL.Get("NominalPPb", "");
   string nominalPBPFileName = CL.Get("NominalPBP", "");
   string variationFileName = CL.Get("Variation", "");
   string variationPPbFileName = CL.Get("VariationPPb", "");
   string variationPBPFileName = CL.Get("VariationPBP", "");
   string outputBase = CL.Get("OutputBase", "plots/pu/systematic");
   string collision = CL.Get("Collision", "pPb");
   string zptRange = CL.Get("ZPTRange", "40_350");
   string trackRange = CL.Get("TrackPTRange", "2_500");
   bool useModified12x12 = CL.GetBool("UseModified12x12", false);

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *variationFile = (variationFileName != "") ? TFile::Open(variationFileName.c_str()) : nullptr;
   TFile *nominalPPbFile = (nominalPPbFileName != "") ? TFile::Open(nominalPPbFileName.c_str()) : nullptr;
   TFile *nominalPBPFile = (nominalPBPFileName != "") ? TFile::Open(nominalPBPFileName.c_str()) : nullptr;
   TFile *variationPPbFile = (variationPPbFileName != "") ? TFile::Open(variationPPbFileName.c_str()) : nullptr;
   TFile *variationPBPFile = (variationPBPFileName != "") ? TFile::Open(variationPBPFileName.c_str()) : nullptr;

   if((nominalFile != nullptr && nominalFile->IsZombie())
      || (variationFile != nullptr && variationFile->IsZombie())
      || (nominalPPbFile != nullptr && nominalPPbFile->IsZombie())
      || (nominalPBPFile != nullptr && nominalPBPFile->IsZombie())
      || (variationPPbFile != nullptr && variationPPbFile->IsZombie())
      || (variationPBPFile != nullptr && variationPBPFile->IsZombie()))
   {
      cerr << "Unable to open PU comparison inputs" << endl;
      return 1;
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      TH1D *variation = nullptr;
      if(nominalPPbFile != nullptr && nominalPBPFile != nullptr)
         nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile, observable, trackRange, observable + "_Nominal", false, useModified12x12);
      else if(nominalFile != nullptr)
         nominal = LoadResultHistogram(*nominalFile, observable, trackRange, observable + "_Nominal", useModified12x12);
      if(variationPPbFile != nullptr && variationPBPFile != nullptr)
         variation = BuildCombinedResultHistogram(*variationPPbFile, *variationPBPFile, observable, trackRange, observable + "_PU", false, useModified12x12);
      else if(variationFile != nullptr)
         variation = LoadResultHistogram(*variationFile, observable, trackRange, observable + "_PU", useModified12x12);

      if(nominal == nullptr || variation == nullptr)
      {
         cerr << "Missing " << observable << " histogram for PU comparison" << endl;
         delete nominal;
         delete variation;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, variation};
      pair<double, double> xRange = GetObservableRange(observable, useModified12x12);
      pair<double, double> yRange = GetComparisonYRange(histograms, observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasPU" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
          histograms, ("PUComparison_" + observable).c_str(), {"Nominal", "PU reject"},
           {cmsBlue, cmsRed}, {0, 0},
           {cmsBlue, cmsRed}, {20, 21},
           GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
           GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
           "PU reject - nominal", differenceRange.first, differenceRange.second,
          0,
          false, false, true,
          (observable == "DeltaPhi") ? 0.25f : 0.62f,
          (observable == "DeltaPhi") ? 0.55f : 0.70f
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), GetCollisionLabel(collision).c_str());
        DrawKinematicLabels(pad, "Nominal vs PU reject", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete variation;
   }

   if(nominalFile != nullptr) { nominalFile->Close(); delete nominalFile; }
   if(variationFile != nullptr) { variationFile->Close(); delete variationFile; }
   if(nominalPPbFile != nullptr) { nominalPPbFile->Close(); delete nominalPPbFile; }
   if(nominalPBPFile != nullptr) { nominalPBPFile->Close(); delete nominalPBPFile; }
   if(variationPPbFile != nullptr) { variationPPbFile->Close(); delete variationPPbFile; }
   if(variationPBPFile != nullptr) { variationPBPFile->Close(); delete variationPBPFile; }
   return 0;
}
