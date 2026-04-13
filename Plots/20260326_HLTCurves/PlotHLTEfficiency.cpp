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
#include "TPad.h"
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

static string NormalizeOrientation(const string &Orientation)
{
   if(Orientation == "Pbp")
      return "Pbp";
   return Orientation;
}

static string GetMCLabel(const string &Orientation)
{
   return (NormalizeOrientation(Orientation) == "pp") ? "Pythia+Madgraph" : "Powheg+EPOS";
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

static void DrawPanel(TPad *Pad, TGraphAsymmErrors &DataEfficiency, TGraphAsymmErrors &MCEfficiency,
   const string &Orientation)
{
   string NormalizedOrientation = NormalizeOrientation(Orientation);

   Pad->cd();
   Pad->SetMargin(0.14, 0.05, 0.12, 0.10);

   DataEfficiency.SetMarkerStyle(20);
   DataEfficiency.SetMarkerColor(kBlack);
   DataEfficiency.SetLineColor(kBlack);
   DataEfficiency.SetLineWidth(2);
   DataEfficiency.SetMarkerSize(1.4);

   MCEfficiency.SetMarkerStyle(21);
   MCEfficiency.SetMarkerColor(kBlue + 1);
   MCEfficiency.SetLineColor(kBlue + 1);
   MCEfficiency.SetLineWidth(2);
   MCEfficiency.SetMarkerSize(1.4);

   TH1 *HWorld = Pad->DrawFrame(0, 0.90, 120, 1.02);
   HWorld->SetTitle(";Reco Z p_{T};Trigger efficiency");
   HWorld->SetStats(0);

   static int GraphIndex = 0;
   TGraphAsymmErrors *MCGraph = (TGraphAsymmErrors *)MCEfficiency.Clone(Form("MCEfficiency_%d", GraphIndex));
   TGraphAsymmErrors *DataGraph = (TGraphAsymmErrors *)DataEfficiency.Clone(Form("DataEfficiency_%d", GraphIndex));
   GraphIndex = GraphIndex + 1;
   MCGraph->Draw("P SAME");
   DataGraph->Draw("P SAME");

   TLegend *Legend = new TLegend(0.48, 0.20, 0.88, 0.36);
   Legend->SetBorderSize(0);
   Legend->SetFillStyle(0);
   Legend->SetTextFont(42);
   Legend->SetTextSize(0.035);
   Legend->AddEntry(MCGraph, GetMCLabel(NormalizedOrientation).c_str(), "pl");
   Legend->AddEntry(DataGraph, (NormalizedOrientation + " data").c_str(), "pl");
   Legend->Draw();

   TLatex Latex;
   Latex.SetNDC();
   Latex.SetTextFont(42);
   Latex.SetTextSize(0.040);
   Latex.DrawLatex(0.17, 0.92, "HLT efficiency");
   Latex.DrawLatex(0.17, 0.86, NormalizedOrientation.c_str());

   Pad->Modified();
   Pad->Update();
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   vector<string> DataFileNames = GetInputFiles(CL.Get("Data", ""), CL.Get("DataList", ""));
   vector<string> MCFileNames = GetInputFiles(CL.Get("MC", ""), CL.Get("MCList", ""));
   string Orientation = NormalizeOrientation(CL.Get("Orientation", "pPb"));
   string OutputFileName = CL.Get("Output", "HLTEfficiency.pdf");

   if(DataFileNames.empty() == true || MCFileNames.empty() == true)
   {
      cerr << "Both data and MC inputs are required" << endl;
      return 1;
   }

   TGraphAsymmErrors DataEfficiency = BuildEfficiencyGraph(DataFileNames);
   TGraphAsymmErrors MCEfficiency = BuildEfficiencyGraph(MCFileNames);

   TCanvas Canvas("Canvas", "", 800, 800);
   DrawPanel((TPad *)Canvas.cd(), DataEfficiency, MCEfficiency, Orientation);

   Canvas.SaveAs(OutputFileName.c_str());

   return 0;
}
