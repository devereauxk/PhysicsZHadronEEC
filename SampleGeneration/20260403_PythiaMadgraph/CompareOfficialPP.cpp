#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TSystem.h"

#include "../../CommonCode/include/KylesPlotting.h"

#include "CommandLine.h"
#include "Messenger.h"

namespace
{
struct SampleHistograms
{
   TH1D HZPtLog;
   TH1D HZPtLinear;
   TH1D HZEta;
   TH1D HTrackEta;
   double SelectedWeight;

   SampleHistograms(const string &Prefix, const double *LogBins, int LogBinCount)
      : HZPtLog(Form("%s_ZPtLog", Prefix.c_str()), "", LogBinCount, LogBins),
        HZPtLinear(Form("%s_ZPtLinear", Prefix.c_str()), "", 100, 0, 50),
        HZEta(Form("%s_ZEta", Prefix.c_str()), "", 48, -2.4, 2.4),
        HTrackEta(Form("%s_TrackEta", Prefix.c_str()), "", 48, -2.4, 2.4),
        SelectedWeight(0)
   {
      HZPtLog.Sumw2();
      HZPtLinear.Sumw2();
      HZEta.Sumw2();
      HTrackEta.Sumw2();
   }
};

bool GetLeadingZ(const ZHadronMessenger &M, bool PreferReco, double &ZPt, double &ZEta)
{
   bool HasReco = (M.zPt != nullptr && M.zEta != nullptr && M.zPt->size() > 0 && M.zEta->size() > 0);
   bool HasGen = (M.genZPt != nullptr && M.genZEta != nullptr && M.genZPt->size() > 0 && M.genZEta->size() > 0);

   if(PreferReco == true && HasReco == true)
   {
      ZPt = M.zPt->at(0);
      ZEta = M.zEta->at(0);
      return true;
   }
   if(HasGen == true)
   {
      ZPt = M.genZPt->at(0);
      ZEta = M.genZEta->at(0);
      return true;
   }
   if(HasReco == true)
   {
      ZPt = M.zPt->at(0);
      ZEta = M.zEta->at(0);
      return true;
   }

   return false;
}

void NormalizeHistogram(TH1D &H, double Weight)
{
   if(Weight > 0)
      H.Scale(1 / Weight, "width");
}

void FillHistograms(const string &InputFileName, SampleHistograms &Histograms, bool PreferReco)
{
   TFile InputFile(InputFileName.c_str());
   if(InputFile.IsZombie() == true)
      throw runtime_error("Unable to open input file " + InputFileName);

   ZHadronMessenger M(InputFile, "Tree");
   int EntryCount = M.GetEntries();

   for(int iE = 0; iE < EntryCount; iE++)
   {
      M.GetEntry(iE);

      double EventWeight = (std::isfinite(M.EventWeight) == true) ? M.EventWeight : 1;
      double ZPt = 0;
      double ZEta = 0;
      if(GetLeadingZ(M, PreferReco, ZPt, ZEta) == false)
         continue;

      Histograms.SelectedWeight = Histograms.SelectedWeight + EventWeight;
      Histograms.HZPtLog.Fill(max(ZPt, 0.010001), EventWeight);
      Histograms.HZPtLinear.Fill(ZPt, EventWeight);
      Histograms.HZEta.Fill(ZEta, EventWeight);

      if(M.trackEta == nullptr || M.trackWeight == nullptr)
         continue;

      int TrackCount = min(M.trackEta->size(), M.trackWeight->size());
      for(int iT = 0; iT < TrackCount; iT++)
      {
         double Weight = EventWeight * M.trackWeight->at(iT);
         if(Weight == 0)
            continue;
         Histograms.HTrackEta.Fill(M.trackEta->at(iT), Weight);
      }
   }

   NormalizeHistogram(Histograms.HZPtLog, Histograms.SelectedWeight);
   NormalizeHistogram(Histograms.HZPtLinear, Histograms.SelectedWeight);
   NormalizeHistogram(Histograms.HZEta, Histograms.SelectedWeight);
   NormalizeHistogram(Histograms.HTrackEta, Histograms.SelectedWeight);
}

void WriteHistograms(TFile &OutputFile, SampleHistograms &Histograms)
{
   OutputFile.cd();
   Histograms.HZPtLog.Write();
   Histograms.HZPtLinear.Write();
   Histograms.HZEta.Write();
   Histograms.HTrackEta.Write();
}

void AddCommonHeader(TPad *Pad)
{
   AddCMSHeader(Pad, "Internal", false);
   AddUPCHeader(Pad, "5.02 TeV", "pp");
}
}

int main(int argc, char *argv[])
{
   gROOT->SetBatch(kTRUE);

   CommandLine CL(argc, argv);

   string OfficialInputFileName = CL.Get("OfficialInput",
      "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pp-v11-Zpt0.root");
   string PrivateInputFileName = CL.Get("PrivateInput");
   string OutputDirectory = CL.Get("OutputDir", "plots/officialPP");
   string OfficialLabel = CL.Get("OfficialLabel", "Official pp MC");
   string PrivateLabel = CL.Get("PrivateLabel", "Private MadGraph+Pythia");
   bool PreferReco = CL.GetBool("PreferReco", true);

   if(PrivateInputFileName == "")
   {
      cerr << "Please provide --PrivateInput" << endl;
      return -1;
   }

   gSystem->mkdir(OutputDirectory.c_str(), true);

   const int LogBinCount = 40;
   vector<double> LogBins(LogBinCount + 1);
   double LogMin = log10(0.01);
   double LogMax = log10(500);
   for(int i = 0; i <= LogBinCount; i++)
      LogBins[i] = pow(10, LogMin + (LogMax - LogMin) * i / LogBinCount);

   SampleHistograms Official("Official", LogBins.data(), LogBinCount);
   SampleHistograms Private("Private", LogBins.data(), LogBinCount);

   FillHistograms(OfficialInputFileName, Official, PreferReco);
   FillHistograms(PrivateInputFileName, Private, PreferReco);

   if(Official.SelectedWeight <= 0 || Private.SelectedWeight <= 0)
   {
      cerr << "Unable to fill comparison histograms from the provided samples." << endl;
      return -1;
   }

   TFile OutputFile(Form("%s/official-private-comparison.root", OutputDirectory.c_str()), "RECREATE");
   WriteHistograms(OutputFile, Official);
   WriteHistograms(OutputFile, Private);
   OutputFile.Close();

   vector<TH1*> ZPtLogHistograms = {&Official.HZPtLog, &Private.HZPtLog};
   vector<TH1*> ZPtLinearHistograms = {&Official.HZPtLinear, &Private.HZPtLinear};
   vector<TH1*> ZEtaHistograms = {&Official.HZEta, &Private.HZEta};
   vector<TH1*> TrackEtaHistograms = {&Official.HTrackEta, &Private.HTrackEta};
   vector<string> Labels = {OfficialLabel, PrivateLabel};
   vector<int> MarkerColors = {cmsBlue, cmsRed};
   vector<int> MarkerStyles = {mCircleFill, mSquareFill};
   vector<int> LineColors = {cmsBlue, cmsRed};
   vector<int> LineStyles = {0, 2};

   TCanvas CZPtLog("CZPtLog", "", 600, 600);
   TPad *PZPtLog = (TPad *)plotCMSRatio(ZPtLogHistograms, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "p_{T}^{Z} (GeV)", 0.01, 500,
      "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", -1, -1,
      "Private / official", 0.5, 1.5,
      0, true, true, true, 0.55);
   AddCommonHeader(PZPtLog);
   CZPtLog.SaveAs(Form("%s/pp-zpt-log-ratio.pdf", OutputDirectory.c_str()));

   TCanvas CZPtLinear("CZPtLinear", "", 600, 600);
   TPad *PZPtLinear = (TPad *)plotCMSRatio(ZPtLinearHistograms, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "p_{T}^{Z} (GeV)", 0, 50,
      "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", -1, -1,
      "Private / official", 0.5, 1.5,
      0, false, false, true, 0.55);
   AddCommonHeader(PZPtLinear);
   CZPtLinear.SaveAs(Form("%s/pp-zpt-linear-ratio.pdf", OutputDirectory.c_str()));

   TCanvas CZEta("CZEta", "", 600, 600);
   TPad *PZEta = (TPad *)plotCMSRatio(ZEtaHistograms, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#eta_{Z}", -2.4, 2.4,
      "(1/N_{Z}) dN_{Z}/d#eta_{Z}", -1, -1,
      "Private / official", 0.5, 1.5,
      0, false, false, true, 0.55);
   AddCommonHeader(PZEta);
   CZEta.SaveAs(Form("%s/pp-zeta-ratio.pdf", OutputDirectory.c_str()));

   TCanvas CTrackEta("CTrackEta", "", 600, 600);
   TPad *PTrackEta = (TPad *)plotCMSRatio(TrackEtaHistograms, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#eta_{track}", -2.4, 2.4,
      "(1/N_{Z}) dN_{ch}/d#eta_{track}", -1, -1,
      "Private / official", 0.5, 1.5,
      0, false, false, true, 0.55);
   AddCommonHeader(PTrackEta);
   CTrackEta.SaveAs(Form("%s/pp-tracketa-ratio.pdf", OutputDirectory.c_str()));

   cout << "Official selected weight: " << Official.SelectedWeight << endl;
   cout << "Private selected weight: " << Private.SelectedWeight << endl;

   return 0;
}
