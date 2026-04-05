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
   string rejectionFileName = CL.Get("Rejection", "");
   string rejectionPPbFileName = CL.Get("RejectionPPb", "");
   string rejectionPBPFileName = CL.Get("RejectionPBP", "");
   string outputBase = CL.Get("OutputBase", "plots/muonTrack/systematic");
   string collision = CL.Get("Collision", "pPb");
   string zptRange = CL.Get("ZPTRange", "40_350");
   string trackRange = CL.Get("TrackPTRange", "2_500");

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *rejectionFile = (rejectionFileName != "") ? TFile::Open(rejectionFileName.c_str()) : nullptr;
   TFile *nominalPPbFile = (nominalPPbFileName != "") ? TFile::Open(nominalPPbFileName.c_str()) : nullptr;
   TFile *nominalPBPFile = (nominalPBPFileName != "") ? TFile::Open(nominalPBPFileName.c_str()) : nullptr;
   TFile *rejectionPPbFile = (rejectionPPbFileName != "") ? TFile::Open(rejectionPPbFileName.c_str()) : nullptr;
   TFile *rejectionPBPFile = (rejectionPBPFileName != "") ? TFile::Open(rejectionPBPFileName.c_str()) : nullptr;

   if((nominalFile != nullptr && nominalFile->IsZombie())
      || (rejectionFile != nullptr && rejectionFile->IsZombie())
      || (nominalPPbFile != nullptr && nominalPPbFile->IsZombie())
      || (nominalPBPFile != nullptr && nominalPBPFile->IsZombie())
      || (rejectionPPbFile != nullptr && rejectionPPbFile->IsZombie())
      || (rejectionPBPFile != nullptr && rejectionPBPFile->IsZombie()))
   {
      cerr << "Unable to open muon-track comparison inputs" << endl;
      return 1;
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      TH1D *rejection = nullptr;
      if(nominalPPbFile != nullptr && nominalPBPFile != nullptr)
         nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile, observable, trackRange, observable + "_Nominal");
      else if(nominalFile != nullptr)
         nominal = LoadResultHistogram(*nominalFile, observable, trackRange, observable + "_Nominal");
      if(rejectionPPbFile != nullptr && rejectionPBPFile != nullptr)
         rejection = BuildCombinedResultHistogram(*rejectionPPbFile, *rejectionPBPFile, observable, trackRange, observable + "_MuonTrack");
      else if(rejectionFile != nullptr)
         rejection = LoadResultHistogram(*rejectionFile, observable, trackRange, observable + "_MuonTrack");

      if(nominal == nullptr || rejection == nullptr)
      {
         cerr << "Missing " << observable << " histogram for muon-track comparison" << endl;
         delete nominal;
         delete rejection;
         return 1;
      }

      vector<TH1 *> histograms = {nominal, rejection};
      pair<double, double> xRange = GetObservableRange(observable);
      pair<double, double> yRange = GetComparisonYRange(histograms, observable);
      pair<double, double> differenceRange = GetDifferenceRange(histograms);

      TCanvas canvas(("CanvasMuonTrack" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("MuonTrackComparison_" + observable).c_str(), {"Nominal", "No Rejection"},
         {cmsBlue, cmsRed}, {0, 0},
         {cmsBlue, cmsRed}, {20, 21},
         GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
         GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
         "variation - nominal", differenceRange.first, differenceRange.second,
         0,
         false, false, true,
         0.62
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), GetCollisionLabel(collision).c_str());
      DrawKinematicLabels(pad, "Muon-track rejection", zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      delete rejection;
   }

   if(nominalFile != nullptr) { nominalFile->Close(); delete nominalFile; }
   if(rejectionFile != nullptr) { rejectionFile->Close(); delete rejectionFile; }
   if(nominalPPbFile != nullptr) { nominalPPbFile->Close(); delete nominalPPbFile; }
   if(nominalPBPFile != nullptr) { nominalPBPFile->Close(); delete nominalPBPFile; }
   if(rejectionPPbFile != nullptr) { rejectionPPbFile->Close(); delete rejectionPPbFile; }
   if(rejectionPBPFile != nullptr) { rejectionPBPFile->Close(); delete rejectionPBPFile; }
   return 0;
}
