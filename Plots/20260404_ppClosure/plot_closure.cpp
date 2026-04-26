#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TROOT.h"
#include "TSystem.h"

#include "../../CommonCode/include/KylesPlotting.h"

#include "CommandLine.h"

namespace
{
TH1D *CloneHistogram1D(TFile &InputFile, const string &Name, const string &CloneName)
{
   TH1D *Histogram = (TH1D *)InputFile.Get(Name.c_str());
   if(Histogram == nullptr)
      return nullptr;

   TH1D *Clone = (TH1D *)Histogram->Clone(CloneName.c_str());
   Clone->SetDirectory(nullptr);
   return Clone;
}

TH2D *CloneHistogram2D(TFile &InputFile, const string &Name, const string &CloneName)
{
   TH2D *Histogram = (TH2D *)InputFile.Get(Name.c_str());
   if(Histogram == nullptr)
      return nullptr;

   TH2D *Clone = (TH2D *)Histogram->Clone(CloneName.c_str());
   Clone->SetDirectory(nullptr);
   return Clone;
}

void ScaleClosureHistogram(TH1D *Histogram)
{
   if(Histogram != nullptr)
      Histogram->Scale(0.5);
}

void LoadInputs(const vector<string> &InputBases, const string &TrkPtRange,
   vector<TH1*> &AllEta, vector<TH1*> &AllPhi,
   vector<TH1*> &MixEta, vector<TH1*> &MixPhi,
   vector<TH1*> &ResultEta, vector<TH1*> &ResultPhi)
{
   for(int i = 0; i < (int)InputBases.size(); i++)
   {
      TFile NoSubFile(Form("%s-nosub.root", InputBases[i].c_str()));
      if(NoSubFile.IsZombie() == true)
         continue;

      TH1D *ThisAllEta = CloneHistogram1D(NoSubFile,
         Form("DeltaEta_Result%s", TrkPtRange.c_str()),
         Form("AllEta_%d", i));
      TH1D *ThisAllPhi = CloneHistogram1D(NoSubFile,
         Form("DeltaPhi_Result%s", TrkPtRange.c_str()),
         Form("AllPhi_%d", i));
      TH2D *ThisMix2D = CloneHistogram2D(NoSubFile,
         Form("hMixData_%s", TrkPtRange.c_str()),
         Form("Mix2D_%d", i));

      if(ThisAllEta == nullptr || ThisAllPhi == nullptr || ThisMix2D == nullptr)
         continue;

      TH1D *ThisMixPhi = ThisMix2D->ProjectionY(Form("MixPhi_%d", i), 0, 10);
      TH1D *ThisMixEta = ThisMix2D->ProjectionX(Form("MixEta_%d", i), 6, 10);
      ThisMixPhi->SetDirectory(nullptr);
      ThisMixEta->SetDirectory(nullptr);
      divideByWidth(ThisMixPhi);
      divideByWidth(ThisMixEta);

      ScaleClosureHistogram(ThisAllEta);
      ScaleClosureHistogram(ThisAllPhi);
      ScaleClosureHistogram(ThisMixEta);
      ScaleClosureHistogram(ThisMixPhi);

      AllEta.push_back(ThisAllEta);
      AllPhi.push_back(ThisAllPhi);
      MixEta.push_back(ThisMixEta);
      MixPhi.push_back(ThisMixPhi);

      TFile ResultFile(Form("%s-result.root", InputBases[i].c_str()));
      if(ResultFile.IsZombie() == true)
         continue;

      TH1D *ThisResultEta = CloneHistogram1D(ResultFile,
         Form("DeltaEta_Result%s", TrkPtRange.c_str()),
         Form("ResultEta_%d", i));
      TH1D *ThisResultPhi = CloneHistogram1D(ResultFile,
         Form("DeltaPhi_Result%s", TrkPtRange.c_str()),
         Form("ResultPhi_%d", i));

      if(ThisResultEta == nullptr || ThisResultPhi == nullptr)
         continue;

      ScaleClosureHistogram(ThisResultEta);
      ScaleClosureHistogram(ThisResultPhi);
      ResultEta.push_back(ThisResultEta);
      ResultPhi.push_back(ThisResultPhi);
   }
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

   string ZPtRange = CL.Get("zPtRange", "40_500");
   string TrkPtRange = CL.Get("trkPtRange", "0.5_500");
   string Tag = CL.Get("tag", "madgraphClosure_nmix10");

   vector<string> InputBases = {
      Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_Gen_nominal_%s_ZPT%s", Tag.c_str(), ZPtRange.c_str()),
      Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/madgraphMC_Gen_nominal_%s_ZPT%s", Tag.c_str(), ZPtRange.c_str())
   };
   vector<string> Labels = {"Official pp MC", "Private MadGraph+Pythia"};
   vector<int> MarkerColors = {cmsBlue, cmsRed};
   vector<int> MarkerStyles = {mCircleFill, mSquareFill};
   vector<int> LineColors = {cmsBlue, cmsRed};
   vector<int> LineStyles = {0, 2};

   vector<TH1*> AllEta;
   vector<TH1*> AllPhi;
   vector<TH1*> MixEta;
   vector<TH1*> MixPhi;
   vector<TH1*> ResultEta;
   vector<TH1*> ResultPhi;

   LoadInputs(InputBases, TrkPtRange, AllEta, AllPhi, MixEta, MixPhi, ResultEta, ResultPhi);
   if(AllEta.size() != 2 || AllPhi.size() != 2 || MixEta.size() != 2 || MixPhi.size() != 2
      || ResultEta.size() != 2 || ResultPhi.size() != 2)
   {
      cerr << "Unable to load both official and private closure inputs for tag " << Tag << endl;
      return -1;
   }

   gSystem->mkdir("plots/pp", true);
   string OutputBase = Form("plots/pp/pp_ZPT%s_trkPT%s_%s-closure",
      ZPtRange.c_str(), TrkPtRange.c_str(), Tag.c_str());

   TCanvas CAllEta("CAllEta", "", 600, 600);
   TPad *PAllEta = (TPad *)plotCMSRatio(AllEta, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#Delta y_{ch,Z}", -4, 4,
      "Signal d#DeltaN_{ch}/d#Delta y_{ch,Z}", -1, -1,
      "Private / official", 0.7, 1.3,
      0, false, false, true, 0.55);
   AddCommonHeader(PAllEta);
   CAllEta.SaveAs(Form("%s-DeltaEta-all.pdf", OutputBase.c_str()));

   TCanvas CAllPhi("CAllPhi", "", 600, 600);
   TPad *PAllPhi = (TPad *)plotCMSRatio(AllPhi, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
      "Signal d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", -1, -1,
      "Private / official", 0.7, 1.3,
      0, false, false, true, 0.55);
   AddCommonHeader(PAllPhi);
   CAllPhi.SaveAs(Form("%s-DeltaPhi-all.pdf", OutputBase.c_str()));

   TCanvas CMixEta("CMixEta", "", 600, 600);
   TPad *PMixEta = (TPad *)plotCMSRatio(MixEta, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#Delta y_{ch,Z}", -4, 4,
      "Mixed d#DeltaN_{ch}/d#Delta y_{ch,Z}", -1, -1,
      "Private / official", 0.7, 1.3,
      0, false, false, true, 0.55);
   AddCommonHeader(PMixEta);
   CMixEta.SaveAs(Form("%s-DeltaEta-bkg.pdf", OutputBase.c_str()));

   TCanvas CMixPhi("CMixPhi", "", 600, 600);
   TPad *PMixPhi = (TPad *)plotCMSRatio(MixPhi, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
      "Mixed d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", -1, -1,
      "Private / official", 0.7, 1.3,
      0, false, false, true, 0.55);
   AddCommonHeader(PMixPhi);
   CMixPhi.SaveAs(Form("%s-DeltaPhi-bkg.pdf", OutputBase.c_str()));

   TCanvas CResultEta("CResultEta", "", 600, 600);
   TPad *PResultEta = (TPad *)plotCMSDiff(ResultEta, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#Delta y_{ch,Z}", -4, 4,
      "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
      "Private - official", -0.05, 0.05,
      0, false, false, true, 0.55);
   AddCommonHeader(PResultEta);
   CResultEta.SaveAs(Form("%s-DeltaEta-result.pdf", OutputBase.c_str()));

   TCanvas CResultPhi("CResultPhi", "", 600, 600);
   TPad *PResultPhi = (TPad *)plotCMSDiff(ResultPhi, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
      "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, -1,
      "Private - official", -0.1, 0.1,
      0, false, false, true, 0.55);
   AddCommonHeader(PResultPhi);
   CResultPhi.SaveAs(Form("%s-DeltaPhi-result.pdf", OutputBase.c_str()));

   return 0;
}
