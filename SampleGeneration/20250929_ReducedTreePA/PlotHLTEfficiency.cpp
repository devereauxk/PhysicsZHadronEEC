#include <iostream>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TLatex.h"

#include "CommandLine.h"

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string InputFileName = CL.Get("Input");
   string OutputFileName = CL.Get("Output", "HLTEfficiency.pdf");

   TFile InputFile(InputFileName.c_str(), "READ");
   TH1D *HN = (TH1D *)InputFile.Get("HltTree/TriggerTurnOn/HLTEffNumerator");
   TH1D *HD = (TH1D *)InputFile.Get("HltTree/TriggerTurnOn/HLTEffDenominator");

   if(HN == nullptr || HD == nullptr)
   {
      cerr << "Missing HLTEffNumerator or HLTEffDenominator in " << InputFileName << endl;
      return 1;
   }

   TGraphAsymmErrors GEfficiency;
   GEfficiency.BayesDivide(HN, HD);
   GEfficiency.SetTitle(";Reco Z p_{T};Trigger efficiency");
   GEfficiency.SetMarkerStyle(20);
   GEfficiency.SetLineWidth(2);

   TCanvas Canvas("Canvas", "", 800, 600);
   TH1D HWorld("HWorld", ";Reco Z p_{T};Trigger efficiency", 20, 20, 120);
   HWorld.SetMinimum(0);
   HWorld.SetMaximum(1.1);
   HWorld.SetStats(0);
   HWorld.Draw();
   GEfficiency.Draw("P SAME");

   TLatex Latex;
   Latex.SetNDC();
   Latex.SetTextFont(42);
   Latex.SetTextSize(0.035);
   Latex.DrawLatex(0.15, 0.92, "HLT efficiency from skim histograms");

   Canvas.SaveAs(OutputFileName.c_str());

   InputFile.Close();
   return 0;
}
