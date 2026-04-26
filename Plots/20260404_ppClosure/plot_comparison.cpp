#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"

#include "../../CommonCode/include/KylesPlotting.h"

#include "CommandLine.h"

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

bool GetLeadingZ(const TTreeReaderArray<float> &RecoPt,
   const TTreeReaderArray<float> &RecoEta,
   const TTreeReaderArray<float> &GenPt,
   const TTreeReaderArray<float> &GenEta,
   bool PreferReco,
   double &ZPt,
   double &ZEta)
{
   bool HasReco = (RecoPt.GetSize() > 0 && RecoEta.GetSize() > 0);
   bool HasGen = (GenPt.GetSize() > 0 && GenEta.GetSize() > 0);

   if(PreferReco == true && HasReco == true)
   {
      ZPt = RecoPt.At(0);
      ZEta = RecoEta.At(0);
      return true;
   }
   if(HasGen == true)
   {
      ZPt = GenPt.At(0);
      ZEta = GenEta.At(0);
      return true;
   }
   if(HasReco == true)
   {
      ZPt = RecoPt.At(0);
      ZEta = RecoEta.At(0);
      return true;
   }

   return false;
}

void NormalizeHistogram(TH1D &H, double Weight)
{
   if(Weight > 0)
      H.Scale(1 / Weight, "width");
}

void FillHistograms(const string &InputFileName, SampleHistograms &Histograms,
   bool PreferReco, long long MaxEvents)
{
   TFile InputFile(InputFileName.c_str());
   if(InputFile.IsZombie() == true)
      throw runtime_error("Unable to open input file " + InputFileName);

   TTree *Tree = (TTree *)InputFile.Get("Tree");
   if(Tree == nullptr)
      throw runtime_error("Unable to find Tree in " + InputFileName);

   TTreeReader Reader(Tree);
   TTreeReaderValue<float> EventWeight(Reader, "EventWeight");
   TTreeReaderArray<float> RecoPt(Reader, "zPt");
   TTreeReaderArray<float> RecoEta(Reader, "zEta");
   TTreeReaderArray<float> GenPt(Reader, "genZPt");
   TTreeReaderArray<float> GenEta(Reader, "genZEta");
   TTreeReaderArray<float> TrackEta(Reader, "trackEta");
   TTreeReaderArray<float> TrackWeight(Reader, "trackWeight");

   long long Entry = 0;
   while(Reader.Next() == true)
   {
      if(MaxEvents >= 0 && Entry >= MaxEvents)
         break;
      Entry = Entry + 1;

      double ThisEventWeight = (std::isfinite(*EventWeight) == true) ? *EventWeight : 1;
      double ZPt = 0;
      double ZEta = 0;
      if(GetLeadingZ(RecoPt, RecoEta, GenPt, GenEta, PreferReco, ZPt, ZEta) == false)
         continue;

      Histograms.SelectedWeight = Histograms.SelectedWeight + ThisEventWeight;
      Histograms.HZPtLog.Fill(max(ZPt, 0.010001), ThisEventWeight);
      Histograms.HZPtLinear.Fill(ZPt, ThisEventWeight);
      Histograms.HZEta.Fill(ZEta, ThisEventWeight);

      int NTrack = min((int)TrackEta.GetSize(), (int)TrackWeight.GetSize());
      for(int iT = 0; iT < NTrack; iT++)
      {
         double Weight = ThisEventWeight * TrackWeight.At(iT);
         if(Weight == 0)
            continue;
         Histograms.HTrackEta.Fill(TrackEta.At(iT), Weight);
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

   const char *DefaultOfficialInput = getenv("OFFICIAL_MCGENINPUT_PP");
   if(DefaultOfficialInput == nullptr || DefaultOfficialInput[0] == '\0')
      DefaultOfficialInput = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-gen-v11-Zpt0.root";
   const char *DefaultPrivateInput = getenv("PRIVATE_PP_5020_INPUT");
   if(DefaultPrivateInput == nullptr || DefaultPrivateInput[0] == '\0')
      DefaultPrivateInput = "/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20260403_PythiaMadgraph/output/ZMuMu_5020_validation.root";

   string OfficialInputFileName = CL.Get("OfficialInput", DefaultOfficialInput);
   string PrivateInputFileName = CL.Get("PrivateInput", DefaultPrivateInput);
   string OutputDirectory = CL.Get("OutputDir", "plots/compare");
   string OfficialLabel = CL.Get("OfficialLabel", "Official pp gen MC");
   string PrivateLabel = CL.Get("PrivateLabel", "Private MadGraph+Pythia");
   bool PreferReco = CL.GetBool("PreferReco", false);
   long long OfficialMaxEvents = stoll(CL.Get("OfficialMaxEvents", "-1"));
   long long PrivateMaxEvents = stoll(CL.Get("PrivateMaxEvents", "-1"));

   gSystem->mkdir(OutputDirectory.c_str(), true);

   const int LogBinCount = 40;
   vector<double> LogBins(LogBinCount + 1);
   double LogMin = log10(0.01);
   double LogMax = log10(500);
   for(int i = 0; i <= LogBinCount; i++)
      LogBins[i] = pow(10, LogMin + (LogMax - LogMin) * i / LogBinCount);

   SampleHistograms Official("Official", LogBins.data(), LogBinCount);
   SampleHistograms Private("Private", LogBins.data(), LogBinCount);

   FillHistograms(OfficialInputFileName, Official, PreferReco, OfficialMaxEvents);
   FillHistograms(PrivateInputFileName, Private, PreferReco, PrivateMaxEvents);

   if(Official.SelectedWeight <= 0 || Private.SelectedWeight <= 0)
   {
      cerr << "Unable to fill comparison histograms from the provided samples." << endl;
      return -1;
   }

   TFile OutputFile(Form("%s/pp-generator-comparison.root", OutputDirectory.c_str()), "RECREATE");
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
