#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

#include "TFile.h"
#include "TH1D.h"

#include "CommandLine.h"

#include "ResultCombinationUtilities.h"

vector<string> ParseCSV(const string &text)
{
   if(text == "")
      return {};

   vector<string> result = CommandLine::Parse(text);
   vector<string> cleaned;
   for(const string &item : result)
      if(item != "")
         cleaned.push_back(item);
   return cleaned;
}

vector<pair<string, string>> ZipFiles(const vector<string> &first, const vector<string> &second)
{
   vector<pair<string, string>> result;
   size_t count = min(first.size(), second.size());
   for(size_t i = 0; i < count; i++)
      result.push_back({first[i], second[i]});
   return result;
}

TH1D *BuildFamilyHistogramSingle(const TH1D *nominal, const vector<string> &files,
   const string &observable, const string &trackTag, const string &histogramName)
{
   TH1D *result = (TH1D *)nominal->Clone(histogramName.c_str());
   result->Reset("ICES");
   result->SetTitle(histogramName.c_str());
   result->SetDirectory(nullptr);

   for(int i = 1; i <= nominal->GetNbinsX(); i++)
   {
      double value = 0;
      for(const string &fileName : files)
      {
         TFile file(fileName.c_str());
         TH1D *variation = LoadSingleResultHistogram(file, observable, trackTag,
            histogramName + "_Variation");
         if(variation == nullptr)
            continue;

         value = max(value, fabs(nominal->GetBinContent(i) - variation->GetBinContent(i)));
         delete variation;
      }

      result->SetBinContent(i, value);
      result->SetBinError(i, 0);
   }

   return result;
}

TH1D *BuildFamilyHistogramCombined(const TH1D *nominal,
   const vector<pair<string, string>> &files, const string &observable,
   const string &trackTag, const string &histogramName)
{
   TH1D *result = (TH1D *)nominal->Clone(histogramName.c_str());
   result->Reset("ICES");
   result->SetTitle(histogramName.c_str());
   result->SetDirectory(nullptr);

   for(int i = 1; i <= nominal->GetNbinsX(); i++)
   {
      double value = 0;
      for(const auto &filePair : files)
      {
         TFile ppbFile(filePair.first.c_str());
         TFile pbpFile(filePair.second.c_str());
         TH1D *variation = BuildCombinedResultHistogram(ppbFile, pbpFile, observable,
            trackTag, histogramName + "_Variation");
         if(variation == nullptr)
            continue;

         value = max(value, fabs(nominal->GetBinContent(i) - variation->GetBinContent(i)));
         delete variation;
      }

      result->SetBinContent(i, value);
      result->SetBinError(i, 0);
   }

   return result;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string nominalFileName = CL.Get("Nominal", "");
   string nominalPPbFileName = CL.Get("NominalPPb", "");
   string nominalPBPFileName = CL.Get("NominalPBP", "");
    string outputFileName = CL.Get("Output", "systematics.root");
    string trackTag = CL.Get("TrackTag", "2_500");
    vector<string> includeFamilies = ParseCSV(CL.Get("IncludeFamilies",
       "TrackSelection,TrackCorrection,MuonRejection,PUpp,PUpPb,ScaleFactor,EnergyExtrapolation"));
   set<string> included(includeFamilies.begin(), includeFamilies.end());

   bool doCombined = (nominalPPbFileName != "" && nominalPBPFileName != "");

   map<string, vector<string>> familyFiles;
   familyFiles["TrackSelection"] = ParseCSV(CL.Get("TrackSelectionFiles", ""));
   familyFiles["TrackCorrection"] = ParseCSV(CL.Get("TrackCorrectionFiles", ""));
   familyFiles["MuonRejection"] = ParseCSV(CL.Get("MuonRejectionFiles", ""));
    familyFiles["PUpp"] = ParseCSV(CL.Get("PUppFiles", ""));
    familyFiles["PUpPb"] = ParseCSV(CL.Get("PUpPbFiles", ""));
    familyFiles["ScaleFactor"] = ParseCSV(CL.Get("ScaleFactorFiles", ""));
    familyFiles["EnergyExtrapolation"] = ParseCSV(CL.Get("EnergyExtrapolationFiles", ""));

   map<string, vector<pair<string, string>>> familyFilePairs;
   familyFilePairs["TrackSelection"] = ZipFiles(
      ParseCSV(CL.Get("TrackSelectionFilesPPb", "")),
      ParseCSV(CL.Get("TrackSelectionFilesPBP", "")));
   familyFilePairs["TrackCorrection"] = ZipFiles(
      ParseCSV(CL.Get("TrackCorrectionFilesPPb", "")),
      ParseCSV(CL.Get("TrackCorrectionFilesPBP", "")));
   familyFilePairs["MuonRejection"] = ZipFiles(
      ParseCSV(CL.Get("MuonRejectionFilesPPb", "")),
      ParseCSV(CL.Get("MuonRejectionFilesPBP", "")));
    familyFilePairs["PUpp"] = ZipFiles(
       ParseCSV(CL.Get("PUppFilesPPb", "")),
       ParseCSV(CL.Get("PUppFilesPBP", "")));
    familyFilePairs["PUpPb"] = ZipFiles(
       ParseCSV(CL.Get("PUpPbFilesPPb", "")),
       ParseCSV(CL.Get("PUpPbFilesPBP", "")));
    familyFilePairs["ScaleFactor"] = ZipFiles(
       ParseCSV(CL.Get("ScaleFactorFilesPPb", "")),
       ParseCSV(CL.Get("ScaleFactorFilesPBP", "")));
    familyFilePairs["EnergyExtrapolation"] = ZipFiles(
       ParseCSV(CL.Get("EnergyExtrapolationFilesPPb", "")),
       ParseCSV(CL.Get("EnergyExtrapolationFilesPBP", "")));

    vector<string> families = {"TrackSelection", "TrackCorrection", "MuonRejection", "PUpp", "PUpPb", "ScaleFactor", "EnergyExtrapolation"};
   vector<string> observables = {"DeltaPhi", "DeltaEta"};

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *nominalPPbFile = (nominalPPbFileName != "") ? TFile::Open(nominalPPbFileName.c_str()) : nullptr;
   TFile *nominalPBPFile = (nominalPBPFileName != "") ? TFile::Open(nominalPBPFileName.c_str()) : nullptr;
   TFile outputFile(outputFileName.c_str(), "RECREATE");

   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      if(doCombined == true)
         nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile, observable,
            trackTag, "Nominal_" + observable);
      else
         nominal = LoadSingleResultHistogram(*nominalFile, observable, trackTag,
            "Nominal_" + observable);

      if(nominal == nullptr)
      {
         cerr << "Missing nominal " << observable << " histogram" << endl;
         return 1;
      }

      map<string, TH1D *> contributions;
      for(const string &family : families)
      {
         string histogramName = family + "_" + observable;
         if(doCombined == true)
            contributions[family] = BuildFamilyHistogramCombined(nominal,
               familyFilePairs[family], observable, trackTag, histogramName);
         else
            contributions[family] = BuildFamilyHistogramSingle(nominal,
               familyFiles[family], observable, trackTag, histogramName);
      }

      TH1D *total = (TH1D *)nominal->Clone(("Total_" + observable).c_str());
      total->Reset("ICES");
      total->SetDirectory(nullptr);
      total->SetTitle(("Total_" + observable).c_str());
      for(int i = 1; i <= nominal->GetNbinsX(); i++)
      {
         double sum2 = 0;
         for(const string &family : families)
         {
            if(included.find(family) == included.end())
               continue;
            double value = contributions[family]->GetBinContent(i);
            sum2 = sum2 + value * value;
         }
         total->SetBinContent(i, sqrt(sum2));
         total->SetBinError(i, 0);
      }

      outputFile.cd();
      for(const string &family : families)
         contributions[family]->Write();
      total->Write();

      for(const string &family : families)
         delete contributions[family];
      delete total;
      delete nominal;
   }

   outputFile.Close();
   if(nominalFile != nullptr)
   {
      nominalFile->Close();
      delete nominalFile;
   }
   if(nominalPPbFile != nullptr)
   {
      nominalPPbFile->Close();
      delete nominalPPbFile;
   }
   if(nominalPBPFile != nullptr)
   {
      nominalPBPFile->Close();
      delete nominalPBPFile;
   }

   return 0;
}
