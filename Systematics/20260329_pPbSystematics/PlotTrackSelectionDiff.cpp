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
   string looseFileName = CL.Get("Loose", "");
   string loosePPbFileName = CL.Get("LoosePPb", "");
   string loosePBPFileName = CL.Get("LoosePBP", "");
   string tightFileName = CL.Get("Tight", "");
   string tightPPbFileName = CL.Get("TightPPb", "");
   string tightPBPFileName = CL.Get("TightPBP", "");
   string outputBase = CL.Get("OutputBase", "plots/trackSelection/systematic");
   string collision = CL.Get("Collision", "pPb");
   string zptRange = CL.Get("ZPTRange", "40_350");
   string trackRange = CL.Get("TrackPTRange", "2_500");
   bool useModified12x12 = CL.GetBool("UseModified12x12", false);

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *looseFile = (looseFileName != "") ? TFile::Open(looseFileName.c_str()) : nullptr;
   TFile *tightFile = (tightFileName != "") ? TFile::Open(tightFileName.c_str()) : nullptr;
   TFile *nominalPPbFile = (nominalPPbFileName != "") ? TFile::Open(nominalPPbFileName.c_str()) : nullptr;
   TFile *nominalPBPFile = (nominalPBPFileName != "") ? TFile::Open(nominalPBPFileName.c_str()) : nullptr;
   TFile *loosePPbFile = (loosePPbFileName != "") ? TFile::Open(loosePPbFileName.c_str()) : nullptr;
   TFile *loosePBPFile = (loosePBPFileName != "") ? TFile::Open(loosePBPFileName.c_str()) : nullptr;
   TFile *tightPPbFile = (tightPPbFileName != "") ? TFile::Open(tightPPbFileName.c_str()) : nullptr;
   TFile *tightPBPFile = (tightPBPFileName != "") ? TFile::Open(tightPBPFileName.c_str()) : nullptr;

   if((nominalFile != nullptr && nominalFile->IsZombie())
      || (looseFile != nullptr && looseFile->IsZombie())
      || (tightFile != nullptr && tightFile->IsZombie())
      || (nominalPPbFile != nullptr && nominalPPbFile->IsZombie())
      || (nominalPBPFile != nullptr && nominalPBPFile->IsZombie())
      || (loosePPbFile != nullptr && loosePPbFile->IsZombie())
      || (loosePBPFile != nullptr && loosePBPFile->IsZombie())
      || (tightPPbFile != nullptr && tightPPbFile->IsZombie())
      || (tightPBPFile != nullptr && tightPBPFile->IsZombie()))
   {
      cerr << "Unable to open track-selection comparison inputs" << endl;
      return 1;
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      TH1D *loose = nullptr;
      TH1D *tight = nullptr;
      if(nominalPPbFile != nullptr && nominalPBPFile != nullptr)
         nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile, observable, trackRange, observable + "_Nominal", false, useModified12x12);
      else if(nominalFile != nullptr)
         nominal = LoadResultHistogram(*nominalFile, observable, trackRange, observable + "_Nominal", useModified12x12);
      if(loosePPbFile != nullptr && loosePBPFile != nullptr)
         loose = BuildCombinedResultHistogram(*loosePPbFile, *loosePBPFile, observable, trackRange, observable + "_Loose", false, useModified12x12);
      else if(looseFile != nullptr)
         loose = LoadResultHistogram(*looseFile, observable, trackRange, observable + "_Loose", useModified12x12);
      if(tightPPbFile != nullptr && tightPBPFile != nullptr)
         tight = BuildCombinedResultHistogram(*tightPPbFile, *tightPBPFile, observable, trackRange, observable + "_Tight", false, useModified12x12);
      else if(tightFile != nullptr)
         tight = LoadResultHistogram(*tightFile, observable, trackRange, observable + "_Tight", useModified12x12);

      if(nominal == nullptr || loose == nullptr || tight == nullptr)
      {
         cerr << "Missing " << observable << " histogram for track-selection comparison" << endl;
         delete nominal;
         delete loose;
         delete tight;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, loose, tight};
      pair<double, double> xRange = GetObservableRange(observable, useModified12x12);
      pair<double, double> yRange = GetComparisonYRange(histograms, observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasTrackSelection" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("TrackSelection_" + observable).c_str(),
          {"Nominal", "Loose", "Tight"},
          {cmsBlue, cmsRed, kGreen + 2}, {0, 0, 0},
          {cmsBlue, cmsRed, kGreen + 2}, {20, 21, 22},
          GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
          GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
          "variation - nominal", differenceRange.first, differenceRange.second,
          0,
          false, false, true,
         0.58
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), GetCollisionLabel(collision).c_str());
      DrawKinematicLabels(pad, "Track-selection variations", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete loose;
      delete tight;
   }

   if(nominalFile != nullptr) { nominalFile->Close(); delete nominalFile; }
   if(looseFile != nullptr) { looseFile->Close(); delete looseFile; }
   if(tightFile != nullptr) { tightFile->Close(); delete tightFile; }
   if(nominalPPbFile != nullptr) { nominalPPbFile->Close(); delete nominalPPbFile; }
   if(nominalPBPFile != nullptr) { nominalPBPFile->Close(); delete nominalPBPFile; }
   if(loosePPbFile != nullptr) { loosePPbFile->Close(); delete loosePPbFile; }
   if(loosePBPFile != nullptr) { loosePBPFile->Close(); delete loosePBPFile; }
   if(tightPPbFile != nullptr) { tightPPbFile->Close(); delete tightPPbFile; }
   if(tightPBPFile != nullptr) { tightPBPFile->Close(); delete tightPBPFile; }
   return 0;
}
