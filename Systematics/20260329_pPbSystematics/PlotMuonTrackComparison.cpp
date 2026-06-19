#include <iostream>
#include <sstream>
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

vector<string> SplitCSV(const string &input)
{
   vector<string> items;
   stringstream stream(input);
   string item;
   while(getline(stream, item, ','))
   {
      size_t start = item.find_first_not_of(" \t");
      size_t end = item.find_last_not_of(" \t");
      if(start == string::npos)
         continue;
      items.push_back(item.substr(start, end - start + 1));
   }
   return items;
}

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
   string rejectionLabel = CL.Get("RejectionLabel", "No Rejection");
   vector<string> variationLabels = SplitCSV(CL.Get("VariationLabels", ""));
   vector<string> variationFileNames = SplitCSV(CL.Get("VariationFiles", ""));
   vector<string> variationPPbFileNames = SplitCSV(CL.Get("VariationPPbFiles", ""));
   vector<string> variationPBPFileNames = SplitCSV(CL.Get("VariationPBPFiles", ""));
   string outputBase = CL.Get("OutputBase", "plots/muonTrack/systematic");
   string collision = CL.Get("Collision", "pPb");
   string zptRange = CL.Get("ZPTRange", "40_350");
   string trackRange = CL.Get("TrackPTRange", "2_500");
   bool useModified12x12 = CL.GetBool("UseModified12x12", false);

   bool useCombinedInputs = (nominalPPbFileName != "" && nominalPBPFileName != "");
   if(variationLabels.empty() && (rejectionFileName != "" || rejectionPPbFileName != "" || rejectionPBPFileName != ""))
   {
      variationLabels.push_back(rejectionLabel);
      if(useCombinedInputs)
      {
         variationPPbFileNames.push_back(rejectionPPbFileName);
         variationPBPFileNames.push_back(rejectionPBPFileName);
      }
      else
         variationFileNames.push_back(rejectionFileName);
   }

   if(variationLabels.empty())
   {
      cerr << "No muon-track comparison variations were provided" << endl;
      return 1;
   }

   if(useCombinedInputs)
   {
      if(variationPPbFileNames.size() != variationLabels.size()
         || variationPBPFileNames.size() != variationLabels.size())
      {
         cerr << "Combined muon-track comparison inputs do not match the variation-label count" << endl;
         return 1;
      }
   }
   else if(variationFileNames.size() != variationLabels.size())
   {
      cerr << "Muon-track comparison inputs do not match the variation-label count" << endl;
      return 1;
   }

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *nominalPPbFile = (nominalPPbFileName != "") ? TFile::Open(nominalPPbFileName.c_str()) : nullptr;
   TFile *nominalPBPFile = (nominalPBPFileName != "") ? TFile::Open(nominalPBPFileName.c_str()) : nullptr;
   vector<TFile *> variationFiles;
   vector<TFile *> variationPPbFiles;
   vector<TFile *> variationPBPFiles;
   for(const string &fileName : variationFileNames)
      variationFiles.push_back((fileName != "") ? TFile::Open(fileName.c_str()) : nullptr);
   for(const string &fileName : variationPPbFileNames)
      variationPPbFiles.push_back((fileName != "") ? TFile::Open(fileName.c_str()) : nullptr);
   for(const string &fileName : variationPBPFileNames)
      variationPBPFiles.push_back((fileName != "") ? TFile::Open(fileName.c_str()) : nullptr);

   if((nominalFile != nullptr && nominalFile->IsZombie())
      || (nominalPPbFile != nullptr && nominalPPbFile->IsZombie())
      || (nominalPBPFile != nullptr && nominalPBPFile->IsZombie())
      )
   {
      cerr << "Unable to open muon-track comparison inputs" << endl;
      return 1;
   }
   for(TFile *file : variationFiles)
      if(file == nullptr || file->IsZombie())
      {
         cerr << "Unable to open muon-track comparison inputs" << endl;
         return 1;
      }
   for(TFile *file : variationPPbFiles)
      if(file == nullptr || file->IsZombie())
      {
         cerr << "Unable to open muon-track comparison inputs" << endl;
         return 1;
      }
   for(TFile *file : variationPBPFiles)
      if(file == nullptr || file->IsZombie())
      {
         cerr << "Unable to open muon-track comparison inputs" << endl;
         return 1;
      }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      if(nominalPPbFile != nullptr && nominalPBPFile != nullptr)
         nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile, observable, trackRange, observable + "_Nominal", false, useModified12x12);
      else if(nominalFile != nullptr)
         nominal = LoadResultHistogram(*nominalFile, observable, trackRange, observable + "_Nominal", useModified12x12);

      vector<TH1D *> variationHistograms;
      for(size_t i = 0; i < variationLabels.size(); i++)
      {
         TH1D *variation = nullptr;
         if(nominalPPbFile != nullptr && nominalPBPFile != nullptr)
            variation = BuildCombinedResultHistogram(*variationPPbFiles[i], *variationPBPFiles[i], observable,
               trackRange, observable + "_MuonTrackVariation" + to_string(i), false, useModified12x12);
         else
            variation = LoadResultHistogram(*variationFiles[i], observable, trackRange,
               observable + "_MuonTrackVariation" + to_string(i), useModified12x12);
         variationHistograms.push_back(variation);
      }

      bool missingHistogram = (nominal == nullptr);
      for(TH1D *variation : variationHistograms)
         if(variation == nullptr)
            missingHistogram = true;

      if(missingHistogram)
      {
         cerr << "Missing " << observable << " histogram for muon-track comparison" << endl;
         delete nominal;
         for(TH1D *variation : variationHistograms)
            delete variation;
         return 1;
      }

      vector<TH1 *> histograms = {nominal};
      vector<string> labels = {"Nominal"};
      for(size_t i = 0; i < variationHistograms.size(); i++)
      {
         histograms.push_back(variationHistograms[i]);
         labels.push_back(variationLabels[i]);
      }
      pair<double, double> xRange = GetObservableRange(observable, useModified12x12);
      pair<double, double> yRange = GetComparisonYRange(histograms, observable);
      if(observable == "DeltaEta" && yRange.first >= yRange.second)
      {
         double minimum = 1e30;
         double maximum = -1e30;
         for(TH1 *histogram : histograms)
         {
            if(histogram == nullptr)
               continue;
            for(int bin = 1; bin <= histogram->GetNbinsX(); bin++)
            {
               minimum = min(minimum, histogram->GetBinContent(bin));
               maximum = max(maximum, histogram->GetBinContent(bin));
            }
         }

         double range = maximum - minimum;
         if(range <= 0)
            range = max(fabs(maximum), 1.0);
         double margin = range * 0.20;
         yRange = {minimum - margin, maximum + margin * 2.60};
      }
      pair<double, double> differenceRange = GetDifferenceRange(histograms);
      vector<int> colorPalette = {cmsBlue, cmsRed, kGreen + 2, kMagenta + 1, kOrange + 7, kCyan + 2, kViolet + 1};
      vector<int> markerPalette = {20, 21, 22, 23, 33, 34, 29};
      vector<int> colors;
      vector<int> markerColors;
      vector<int> markers;
      vector<int> fillStyles;
      for(size_t i = 0; i < histograms.size(); i++)
      {
         colors.push_back(colorPalette[i % colorPalette.size()]);
         markerColors.push_back(colorPalette[i % colorPalette.size()]);
         markers.push_back(markerPalette[i % markerPalette.size()]);
         fillStyles.push_back(0);
      }
      double legendX = (observable == "DeltaPhi") ? 0.25 : ((histograms.size() > 4) ? 0.45 : 0.62);
      double legendY = (observable == "DeltaPhi") ? 0.55 : 0.70;

      TCanvas canvas(("CanvasMuonTrack" + observable).c_str(), "", 600, 600);
      TPad *pad = (TPad *)plotCMSDiff(
         histograms, ("MuonTrackComparison_" + observable).c_str(), labels,
         colors, fillStyles,
         markerColors, markers,
         GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
         GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
         "variation - nominal", differenceRange.first, differenceRange.second,
         0,
         false, false, true,
         legendX,
         legendY
      );

      AddCMSHeader(pad, "Internal", false);
      AddUPCHeader(pad, GetCollisionEnergy(collision).c_str(), GetCollisionLabel(collision).c_str());
      DrawKinematicLabels(pad,
         (variationHistograms.size() > 1) ? "Muon-track rejection variations" : "Muon-track rejection",
         zptRange, trackRange);

      canvas.Update();
      canvas.SaveAs((outputBase + "-" + observable + ".pdf").c_str());

      delete nominal;
      for(TH1D *variation : variationHistograms)
         delete variation;
   }

    if(nominalFile != nullptr) { nominalFile->Close(); delete nominalFile; }
    if(nominalPPbFile != nullptr) { nominalPPbFile->Close(); delete nominalPPbFile; }
    if(nominalPBPFile != nullptr) { nominalPBPFile->Close(); delete nominalPBPFile; }
    for(TFile *file : variationFiles)
    {
       if(file != nullptr)
       {
          file->Close();
          delete file;
       }
    }
    for(TFile *file : variationPPbFiles)
    {
       if(file != nullptr)
       {
          file->Close();
          delete file;
       }
    }
    for(TFile *file : variationPBPFiles)
    {
       if(file != nullptr)
       {
          file->Close();
          delete file;
       }
    }
    return 0;
}
