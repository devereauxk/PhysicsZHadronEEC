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
   string downFileName = CL.Get("Down", "");
   string downPPbFileName = CL.Get("DownPPb", "");
   string downPBPFileName = CL.Get("DownPBP", "");
   string upFileName = CL.Get("Up", "");
   string upPPbFileName = CL.Get("UpPPb", "");
   string upPBPFileName = CL.Get("UpPBP", "");
   string outputBase = CL.Get("OutputBase", "plots/trackCorrection/systematic");
   string collision = CL.Get("Collision", "pp");
   string zptRange = CL.Get("ZPTRange", "5_500");
   string trackRange = CL.Get("TrackPTRange", "0.5_500");
   bool useModified12x12 = CL.GetBool("UseModified12x12", false);

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *downFile = (downFileName != "") ? TFile::Open(downFileName.c_str()) : nullptr;
   TFile *upFile = (upFileName != "") ? TFile::Open(upFileName.c_str()) : nullptr;
   TFile *nominalPPbFile = (nominalPPbFileName != "") ? TFile::Open(nominalPPbFileName.c_str()) : nullptr;
   TFile *nominalPBPFile = (nominalPBPFileName != "") ? TFile::Open(nominalPBPFileName.c_str()) : nullptr;
   TFile *downPPbFile = (downPPbFileName != "") ? TFile::Open(downPPbFileName.c_str()) : nullptr;
   TFile *downPBPFile = (downPBPFileName != "") ? TFile::Open(downPBPFileName.c_str()) : nullptr;
   TFile *upPPbFile = (upPPbFileName != "") ? TFile::Open(upPPbFileName.c_str()) : nullptr;
   TFile *upPBPFile = (upPBPFileName != "") ? TFile::Open(upPBPFileName.c_str()) : nullptr;

   if((nominalFile != nullptr && nominalFile->IsZombie())
      || (downFile != nullptr && downFile->IsZombie())
      || (upFile != nullptr && upFile->IsZombie())
      || (nominalPPbFile != nullptr && nominalPPbFile->IsZombie())
      || (nominalPBPFile != nullptr && nominalPBPFile->IsZombie())
      || (downPPbFile != nullptr && downPPbFile->IsZombie())
      || (downPBPFile != nullptr && downPBPFile->IsZombie())
      || (upPPbFile != nullptr && upPPbFile->IsZombie())
      || (upPBPFile != nullptr && upPBPFile->IsZombie()))
   {
      cerr << "Unable to open track-correction comparison inputs" << endl;
      return 1;
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      TH1D *down = nullptr;
      TH1D *up = nullptr;
      if(nominalPPbFile != nullptr && nominalPBPFile != nullptr)
         nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile, observable, trackRange, observable + "_Nominal", false, useModified12x12);
      else if(nominalFile != nullptr)
         nominal = LoadResultHistogram(*nominalFile, observable, trackRange, observable + "_Nominal", useModified12x12);
      if(downPPbFile != nullptr && downPBPFile != nullptr)
         down = BuildCombinedResultHistogram(*downPPbFile, *downPBPFile, observable, trackRange, observable + "_TrackCorrection0p976", false, useModified12x12);
      else if(downFile != nullptr)
         down = LoadResultHistogram(*downFile, observable, trackRange, observable + "_TrackCorrection0p976", useModified12x12);
      if(upPPbFile != nullptr && upPBPFile != nullptr)
         up = BuildCombinedResultHistogram(*upPPbFile, *upPBPFile, observable, trackRange, observable + "_TrackCorrection1p024", false, useModified12x12);
      else if(upFile != nullptr)
         up = LoadResultHistogram(*upFile, observable, trackRange, observable + "_TrackCorrection1p024", useModified12x12);

      if(nominal == nullptr || down == nullptr || up == nullptr)
      {
         cerr << "Missing " << observable << " histogram for track-correction comparison" << endl;
         delete nominal;
         delete down;
         delete up;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, down, up};
      pair<double, double> xRange = GetObservableRange(observable, useModified12x12);
      pair<double, double> yRange = GetComparisonYRange(histograms, observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasTrackCorrection" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("TrackCorrection_" + observable).c_str(),
          {"Nominal", "0.976", "1.024"},
          {cmsBlue, cmsRed, kGreen + 2}, {0, 0, 0},
          {cmsBlue, cmsRed, kGreen + 2}, {20, 21, 22},
          GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
          GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
          "variation - nominal", differenceRange.first, differenceRange.second,
          0,
          false, false, true,
         (observable == "DeltaPhi") ? 0.25f : 0.58f,
         (observable == "DeltaPhi") ? 0.55f : 0.70f
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), GetCollisionLabel(collision).c_str());
      DrawKinematicLabels(pad, "Track-correction variations", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete down;
      delete up;
   }

   if(nominalFile != nullptr) { nominalFile->Close(); delete nominalFile; }
   if(downFile != nullptr) { downFile->Close(); delete downFile; }
   if(upFile != nullptr) { upFile->Close(); delete upFile; }
   if(nominalPPbFile != nullptr) { nominalPPbFile->Close(); delete nominalPPbFile; }
   if(nominalPBPFile != nullptr) { nominalPBPFile->Close(); delete nominalPBPFile; }
   if(downPPbFile != nullptr) { downPPbFile->Close(); delete downPPbFile; }
   if(downPBPFile != nullptr) { downPBPFile->Close(); delete downPBPFile; }
   if(upPPbFile != nullptr) { upPPbFile->Close(); delete upPPbFile; }
   if(upPBPFile != nullptr) { upPBPFile->Close(); delete upPBPFile; }
   return 0;
}
