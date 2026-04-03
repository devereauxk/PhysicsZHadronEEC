#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TColor.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TLatex.h"

#include "CommandLine.h"

static vector<string> ReadFileList(const string &FileListName)
{
   ifstream Input(FileListName);
   if(Input)
   {
      vector<string> Result;
      string FileName;
      while(getline(Input, FileName))
      {
         if(FileName.empty() == false)
            Result.push_back(FileName);
      }
      return Result;
   }

   cerr << "Unable to open file list " << FileListName << endl;
   exit(1);
}

static vector<string> GetInputFiles(const string &SingleFileName, const string &FileListName)
{
   if(FileListName.empty() == false)
      return ReadFileList(FileListName);

   if(SingleFileName.empty() == false)
      return vector<string>{SingleFileName};

   return vector<string>();
}

static TGraphAsymmErrors BuildEfficiencyGraph(const vector<string> &InputFileNames)
{
   if(InputFileNames.empty() == true)
   {
      cerr << "No input files were provided" << endl;
      exit(1);
   }

   TH1D *HN = nullptr;
   TH1D *HD = nullptr;
   static int HistogramIndex = 0;

   for(const string &InputFileName : InputFileNames)
   {
      TFile InputFile(InputFileName.c_str(), "READ");
      if(InputFile.IsZombie() == true)
      {
         cerr << "Unable to open input file " << InputFileName << endl;
         exit(1);
      }

      TH1D *CurrentNumerator = (TH1D *)InputFile.Get("HltTree/TriggerTurnOn/HLTEffNumerator");
      TH1D *CurrentDenominator = (TH1D *)InputFile.Get("HltTree/TriggerTurnOn/HLTEffDenominator");

      if(CurrentNumerator == nullptr || CurrentDenominator == nullptr)
      {
         cerr << "Missing HLTEffNumerator or HLTEffDenominator in " << InputFileName << endl;
         exit(1);
      }

      if(HN == nullptr)
      {
         string NumeratorName = "HN_" + to_string(HistogramIndex);
         string DenominatorName = "HD_" + to_string(HistogramIndex);
         HistogramIndex = HistogramIndex + 1;
         HN = (TH1D *)CurrentNumerator->Clone(NumeratorName.c_str());
         HD = (TH1D *)CurrentDenominator->Clone(DenominatorName.c_str());
         HN->SetDirectory(nullptr);
         HD->SetDirectory(nullptr);
      }
      else
      {
         HN->Add(CurrentNumerator);
         HD->Add(CurrentDenominator);
      }

      InputFile.Close();
   }

   TGraphAsymmErrors Graph;
   Graph.BayesDivide(HN, HD);
   Graph.SetTitle(";Reco Z p_{T};Trigger efficiency");
   return Graph;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   vector<string> DataFileNames = GetInputFiles(CL.Get("Data", ""), CL.Get("DataList", ""));
   vector<string> MCFileNames = GetInputFiles(CL.Get("MC", ""), CL.Get("MCList", ""));
   string Orientation = CL.Get("Orientation", "pPb");
   string OutputFileName = CL.Get("Output", "HLTEfficiency.pdf");

   if(DataFileNames.empty() == true || MCFileNames.empty() == true)
   {
      cerr << "Both data and MC inputs are required" << endl;
      return 1;
   }

   TGraphAsymmErrors DataEfficiency = BuildEfficiencyGraph(DataFileNames);
   TGraphAsymmErrors MCEfficiency = BuildEfficiencyGraph(MCFileNames);

   DataEfficiency.SetMarkerStyle(20);
   DataEfficiency.SetMarkerColor(kBlack);
   DataEfficiency.SetLineColor(kBlack);
   DataEfficiency.SetLineWidth(2);

   MCEfficiency.SetMarkerStyle(21);
   MCEfficiency.SetMarkerColor(kBlue + 1);
   MCEfficiency.SetLineColor(kBlue + 1);
   MCEfficiency.SetLineWidth(2);

   TCanvas Canvas("Canvas", "", 800, 800);
   TH1D HWorld("HWorld", ";Reco Z p_{T};Trigger efficiency", 30, 0, 120);
   HWorld.SetMinimum(0);
   HWorld.SetMaximum(1.1);
   HWorld.SetStats(0);
   HWorld.Draw();

   MCEfficiency.Draw("P SAME");
   DataEfficiency.Draw("P SAME");

   TLegend Legend(0.58, 0.20, 0.86, 0.33);
   Legend.SetBorderSize(0);
   Legend.SetFillStyle(0);
   Legend.SetTextFont(42);
   Legend.SetTextSize(0.035);
   Legend.AddEntry(&MCEfficiency, "Powheg+EPOS", "pl");
   Legend.AddEntry(&DataEfficiency, (Orientation + " data").c_str(), "pl");
   Legend.Draw();

   TLatex Latex;
   Latex.SetNDC();
   Latex.SetTextFont(42);
   Latex.SetTextSize(0.035);
    Latex.DrawLatex(0.15, 0.92, ("HLT efficiency from V0.2 " + Orientation + " inputs").c_str());

   Canvas.SaveAs(OutputFileName.c_str());

   return 0;
}
