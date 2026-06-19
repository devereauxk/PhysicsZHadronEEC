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

vector<string> NormalizeDifferenceFamilies(const vector<string> &families)
{
   vector<string> normalized;
   set<string> seen;

   for(string family : families)
   {
      if(family == "PUpp" || family == "PUpPb")
         family = "PU";
      if(seen.insert(family).second == true)
         normalized.push_back(family);
   }

   return normalized;
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
   const string &observable, const string &trackTag, const string &histogramName,
   bool symmetrize = false)
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
            histogramName + "_Variation", symmetrize);
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
   const string &trackTag, const string &histogramName, bool useShifted10x10 = false,
   bool useModified12x12 = false)
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
            trackTag, histogramName + "_Variation", useShifted10x10, useModified12x12);
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

TH1D *BuildDifferenceFamilyHistogram(const TH1D *nominalDifference,
   const TH1D *combinedContribution, const TH1D *ppContribution,
   const string &histogramName)
{
   TH1D *result = (TH1D *)nominalDifference->Clone(histogramName.c_str());
   result->Reset("ICES");
   result->SetTitle(histogramName.c_str());
   result->SetDirectory(nullptr);

   for(int i = 1; i <= nominalDifference->GetNbinsX(); i++)
   {
      double combinedValue = (combinedContribution != nullptr) ? combinedContribution->GetBinContent(i) : 0;
      double ppValue = (ppContribution != nullptr) ? ppContribution->GetBinContent(i) : 0;
      double value = sqrt(combinedValue * combinedValue + ppValue * ppValue);
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
    string nominalPPFileName = CL.Get("NominalPP", "");
    bool useShifted10x10 = CL.GetBool("UseShifted10x10", false);
    bool useModified12x12 = CL.GetBool("UseModified12x12", false);
      string outputFileName = CL.Get("Output", "systematics.root");
    if(useShifted10x10 == true && useModified12x12 == true)
    {
       cerr << "UseShifted10x10 and UseModified12x12 cannot both be true" << endl;
       return 1;
    }
    string trackTag = CL.Get("TrackTag", "2_500");
    vector<string> includeFamilies = ParseCSV(CL.Get("IncludeFamilies",
       "TrackSelection,TrackCorrection,MuonRejection,PUpp,PUpPb,ScaleFactor,EnergyExtrapolation"));
    vector<string> includeDifferenceFamilies = NormalizeDifferenceFamilies(ParseCSV(CL.Get("IncludeDifferenceFamilies",
      "TrackSelection,TrackCorrection,MuonRejection,PU,ScaleFactor,EnergyExtrapolation")));
   set<string> included(includeFamilies.begin(), includeFamilies.end());
   set<string> includedDifference(includeDifferenceFamilies.begin(), includeDifferenceFamilies.end());

   bool doCombined = (nominalPPbFileName != "" && nominalPBPFileName != "");
   bool doDifference = (doCombined == true && nominalPPFileName != "");

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
    familyFilePairs["PU"] = ZipFiles(
       ParseCSV(CL.Get("PUpPbFilesPPb", "")),
       ParseCSV(CL.Get("PUpPbFilesPBP", "")));
    familyFilePairs["ScaleFactor"] = ZipFiles(
       ParseCSV(CL.Get("ScaleFactorFilesPPb", "")),
       ParseCSV(CL.Get("ScaleFactorFilesPBP", "")));
    familyFilePairs["EnergyExtrapolation"] = ZipFiles(
       ParseCSV(CL.Get("EnergyExtrapolationFilesPPb", "")),
       ParseCSV(CL.Get("EnergyExtrapolationFilesPBP", "")));

   string meWeightFile = CL.Get("MEWeightFile", "");

   map<string, vector<string>> familyFilesPP;
   familyFilesPP["TrackSelection"] = ParseCSV(CL.Get("TrackSelectionFilesPP", ""));
   familyFilesPP["TrackCorrection"] = ParseCSV(CL.Get("TrackCorrectionFilesPP", ""));
   familyFilesPP["MuonRejection"] = ParseCSV(CL.Get("MuonRejectionFilesPP", ""));
   familyFilesPP["PUpp"] = ParseCSV(CL.Get("PUppFilesPP", ""));
   familyFilesPP["PUpPb"] = ParseCSV(CL.Get("PUpPbFilesPP", ""));
   familyFilesPP["PU"] = ParseCSV(CL.Get("PUppFilesPP", ""));
   familyFilesPP["ScaleFactor"] = ParseCSV(CL.Get("ScaleFactorFilesPP", ""));
   familyFilesPP["EnergyExtrapolation"] = ParseCSV(CL.Get("EnergyExtrapolationFilesPP", ""));

    vector<string> families = {"TrackSelection", "TrackCorrection", "MuonRejection", "PUpp", "PUpPb", "ScaleFactor", "EnergyExtrapolation", "EventMixing"};
   vector<string> differenceFamilies = {"TrackSelection", "TrackCorrection", "MuonRejection", "PU", "ScaleFactor", "EnergyExtrapolation", "EventMixing"};
   vector<string> observables = {"DeltaPhi", "DeltaEta"};

   TFile *nominalFile = (nominalFileName != "") ? TFile::Open(nominalFileName.c_str()) : nullptr;
   TFile *nominalPPbFile = (nominalPPbFileName != "") ? TFile::Open(nominalPPbFileName.c_str()) : nullptr;
   TFile *nominalPBPFile = (nominalPBPFileName != "") ? TFile::Open(nominalPBPFileName.c_str()) : nullptr;
   TFile *nominalPPFile = (nominalPPFileName != "") ? TFile::Open(nominalPPFileName.c_str()) : nullptr;
   TFile outputFile(outputFileName.c_str(), "RECREATE");

   for(const string &observable : observables)
   {
      TH1D *nominal = nullptr;
      if(doCombined == true)
         nominal = BuildCombinedResultHistogram(*nominalPPbFile, *nominalPBPFile, observable,
            trackTag, "Nominal_" + observable, useShifted10x10, useModified12x12);
      else
         nominal = LoadSingleResultHistogram(*nominalFile, observable, trackTag,
            "Nominal_" + observable, useModified12x12);

      if(nominal == nullptr)
      {
         cerr << "Missing nominal " << observable << " histogram" << endl;
         return 1;
      }

      TH1D *nominalPP = nullptr;
      TH1D *nominalDifference = nullptr;
      if(doDifference == true)
      {
         nominalPP = LoadSingleResultHistogram(*nominalPPFile, observable, trackTag,
            "NominalPP_" + observable, useModified12x12);
         if(nominalPP == nullptr)
         {
            cerr << "Missing pp nominal " << observable << " histogram for difference systematics" << endl;
            return 1;
         }

         nominalDifference = (TH1D *)nominal->Clone(("DifferenceNominal_" + observable).c_str());
         nominalDifference->SetDirectory(nullptr);
         nominalDifference->Add(nominalPP, -1);
      }

      map<string, TH1D *> contributions;
      for(const string &family : families)
      {
         string histogramName = family + "_" + observable;
         if(family == "EventMixing")
         {
            TH1D *emResult = (TH1D *)nominal->Clone(histogramName.c_str());
            emResult->Reset("ICES");
            emResult->SetTitle(histogramName.c_str());
            emResult->SetDirectory(nullptr);

            if(doCombined == true && meWeightFile != "")
            {
               TFile meFile(meWeightFile.c_str());
               TH2D *meWeight = (TH2D *)meFile.Get("hMEWeight");
               if(meWeight != nullptr)
               {
                  meWeight->SetDirectory(nullptr);
                  TH1D *variation = BuildCombinedResultHistogramWithMEWeight(
                     *nominalPPbFile, *nominalPBPFile, observable, trackTag,
                     histogramName + "_Variation", meWeight, useModified12x12);
                  if(variation != nullptr)
                  {
                     for(int i = 1; i <= nominal->GetNbinsX(); i++)
                     {
                        double value = fabs(nominal->GetBinContent(i) - variation->GetBinContent(i));
                        emResult->SetBinContent(i, value);
                     }
                     delete variation;
                  }
                  delete meWeight;
               }
            }
            contributions[family] = emResult;
         }
         else if(doCombined == true)
             contributions[family] = BuildFamilyHistogramCombined(nominal,
               familyFilePairs[family], observable, trackTag, histogramName, useShifted10x10, useModified12x12);
         else
            contributions[family] = BuildFamilyHistogramSingle(nominal,
               familyFiles[family], observable, trackTag, histogramName, useModified12x12);
      }

       map<string, TH1D *> differenceContributions;
       if(doDifference == true)
       {
          for(const string &family : differenceFamilies)
          {
             string histogramName = "Difference" + family + "_" + observable;
             const TH1D *combinedContribution = (family == "PU") ? contributions["PUpPb"] : contributions[family];
             TH1D *ppContribution = BuildFamilyHistogramSingle(nominalPP,
                familyFilesPP[family], observable, trackTag, histogramName + "_PP", useModified12x12);
             differenceContributions[family] = BuildDifferenceFamilyHistogram(nominalDifference,
                combinedContribution, ppContribution, histogramName);
             delete ppContribution;
          }
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

      TH1D *differenceTotal = nullptr;
      if(doDifference == true)
      {
         differenceTotal = (TH1D *)nominalDifference->Clone(("DifferenceTotal_" + observable).c_str());
         differenceTotal->Reset("ICES");
         differenceTotal->SetDirectory(nullptr);
         differenceTotal->SetTitle(("DifferenceTotal_" + observable).c_str());
         for(int i = 1; i <= nominalDifference->GetNbinsX(); i++)
         {
            double sum2 = 0;
            for(const string &family : differenceFamilies)
            {
               if(includedDifference.find(family) == includedDifference.end())
                  continue;
               double value = differenceContributions[family]->GetBinContent(i);
               sum2 = sum2 + value * value;
            }
            differenceTotal->SetBinContent(i, sqrt(sum2));
            differenceTotal->SetBinError(i, 0);
         }
      }

      outputFile.cd();
      for(const string &family : families)
         contributions[family]->Write();
      total->Write();
       if(doDifference == true)
       {
          for(const string &family : differenceFamilies)
            differenceContributions[family]->Write();
          differenceTotal->Write();
       }

       for(const string &family : families)
          delete contributions[family];
      if(doDifference == true)
         for(const string &family : differenceFamilies)
            delete differenceContributions[family];
      delete total;
      delete differenceTotal;
      delete nominalDifference;
      delete nominalPP;
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
    if(nominalPPFile != nullptr)
    {
       nominalPPFile->Close();
       delete nominalPPFile;
    }

    return 0;
}
