#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

#include "TFile.h"
#include "TH1D.h"

#include "CommandLine.h"

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

TH1D *CloneHistogram(TFile &file, const string &name)
{
   constexpr double ResultNormalization = 0.5;

   TH1D *histogram = (TH1D *)file.Get(name.c_str());
   if(histogram == nullptr)
      return nullptr;

   histogram = (TH1D *)histogram->Clone();
   histogram->SetDirectory(nullptr);
   histogram->Scale(ResultNormalization);
   return histogram;
}

TH1D *BuildFamilyHistogram(const TH1D *nominal, const vector<string> &files,
   const string &histogramName, const string &nominalHistogramName, double trackingFraction = -1)
{
   TH1D *result = (TH1D *)nominal->Clone(histogramName.c_str());
   result->Reset("ICES");
   result->SetTitle(histogramName.c_str());
   result->SetDirectory(nullptr);

   for(int i = 1; i <= nominal->GetNbinsX(); i++)
   {
      double value = 0;
      if(trackingFraction >= 0)
         value = fabs(trackingFraction * nominal->GetBinContent(i));
      else
      {
         for(const string &fileName : files)
         {
            TFile file(fileName.c_str());
            TH1D *variation = CloneHistogram(file, nominalHistogramName);
            if(variation == nullptr)
               continue;
            value = max(value, fabs(nominal->GetBinContent(i) - variation->GetBinContent(i)));
            delete variation;
         }
      }

      result->SetBinContent(i, value);
      result->SetBinError(i, 0);
   }

   return result;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string nominalFileName = CL.Get("Nominal");
   string outputFileName = CL.Get("Output", "systematics.root");
   string trackTag = CL.Get("TrackTag", "2_500");
   double trackingFraction = CL.GetDouble("TrackingFraction", 0.021);
   vector<string> includeFamilies = ParseCSV(CL.Get("IncludeFamilies", "TrackSelection,TrackCorrection,MuonRejection,PUpp,PUpPb,ScaleFactor"));
   set<string> included(includeFamilies.begin(), includeFamilies.end());

   map<string, vector<string>> familyFiles;
   familyFiles["TrackSelection"] = ParseCSV(CL.Get("TrackSelectionFiles", ""));
   familyFiles["MuonRejection"] = ParseCSV(CL.Get("MuonRejectionFiles", ""));
   familyFiles["PUpp"] = ParseCSV(CL.Get("PUppFiles", ""));
   familyFiles["PUpPb"] = ParseCSV(CL.Get("PUpPbFiles", ""));
   familyFiles["ScaleFactor"] = ParseCSV(CL.Get("ScaleFactorFiles", ""));

   vector<string> families = {"TrackSelection", "TrackCorrection", "MuonRejection", "PUpp", "PUpPb", "ScaleFactor"};
   vector<string> observables = {"DeltaPhi", "DeltaEta"};

   TFile nominalFile(nominalFileName.c_str());
   TFile outputFile(outputFileName.c_str(), "RECREATE");

   for(const string &observable : observables)
   {
      string nominalHistogramName = observable + "_Result" + trackTag;
      TH1D *nominal = CloneHistogram(nominalFile, nominalHistogramName);
      if(nominal == nullptr)
      {
         cerr << "Missing histogram " << nominalHistogramName << " in " << nominalFileName << endl;
         return 1;
      }

      map<string, TH1D *> contributions;
      contributions["TrackSelection"] = BuildFamilyHistogram(nominal, familyFiles["TrackSelection"], "TrackSelection_" + observable, nominalHistogramName);
      contributions["TrackCorrection"] = BuildFamilyHistogram(nominal, {}, "TrackCorrection_" + observable, nominalHistogramName, trackingFraction);
      contributions["MuonRejection"] = BuildFamilyHistogram(nominal, familyFiles["MuonRejection"], "MuonRejection_" + observable, nominalHistogramName);
      contributions["PUpp"] = BuildFamilyHistogram(nominal, familyFiles["PUpp"], "PUpp_" + observable, nominalHistogramName);
      contributions["PUpPb"] = BuildFamilyHistogram(nominal, familyFiles["PUpPb"], "PUpPb_" + observable, nominalHistogramName);
      contributions["ScaleFactor"] = BuildFamilyHistogram(nominal, familyFiles["ScaleFactor"], "ScaleFactor_" + observable, nominalHistogramName);

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
   nominalFile.Close();

   return 0;
}
