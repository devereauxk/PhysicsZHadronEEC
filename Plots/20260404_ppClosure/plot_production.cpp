#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TROOT.h"
#include "TSystem.h"

#include "../../CommonCode/include/KylesPlotting.h"

#include "CommandLine.h"

namespace
{
pair<double, double> ParseRange(const string &Range)
{
   size_t split = Range.find('_');
   if(split == string::npos)
      return {0, 500};

   return {stod(Range.substr(0, split)), stod(Range.substr(split + 1))};
}

void AddCommonHeader(TPad *Pad)
{
   AddCMSHeader(Pad, "Internal", false);
   AddUPCHeader(Pad, "5.02 TeV", "pp");
}

TH1D *LoadProjection(TFile &InputFile, const string &TrkPtRange, int Axis, const string &Name)
{
   TH3D *Histogram3D = (TH3D *)InputFile.Get(Form("hZPtEtaPhi_%s", TrkPtRange.c_str()));
   TH1D *NZ = (TH1D *)InputFile.Get(Form("hNZData_%s", TrkPtRange.c_str()));
   if(Histogram3D == nullptr || NZ == nullptr || NZ->GetBinContent(1) <= 0)
      return nullptr;

   TH1D *Projection = nullptr;
   if(Axis == 1)
      Projection = Histogram3D->ProjectionX(Name.c_str());
   else if(Axis == 2)
      Projection = Histogram3D->ProjectionY(Name.c_str());
   else
      Projection = Histogram3D->ProjectionZ(Name.c_str());

   Projection->SetDirectory(nullptr);
   Projection->Scale(1.0 / NZ->GetBinContent(1));
   divideByWidth(Projection);
   return Projection;
}

bool LoadHistograms(const vector<string> &InputBases, const string &TrkPtRange,
   vector<TH1*> &ZPt, vector<TH1*> &ZEta, vector<TH1*> &ZPhi)
{
   for(int i = 0; i < (int)InputBases.size(); i++)
   {
      TFile InputFile(Form("%s-nosub.root", InputBases[i].c_str()));
      if(InputFile.IsZombie() == true)
         return false;

      TH1D *ThisZPt = LoadProjection(InputFile, TrkPtRange, 1, Form("ZPt_%d", i));
      TH1D *ThisZEta = LoadProjection(InputFile, TrkPtRange, 2, Form("ZEta_%d", i));
      TH1D *ThisZPhi = LoadProjection(InputFile, TrkPtRange, 3, Form("ZPhi_%d", i));
      if(ThisZPt == nullptr || ThisZEta == nullptr || ThisZPhi == nullptr)
         return false;

      ZPt.push_back(ThisZPt);
      ZEta.push_back(ThisZEta);
      ZPhi.push_back(ThisZPhi);
   }

   return true;
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
   vector<string> Labels = {"Official pp gen MC", "Private Pythia+MadGraph"};
   vector<int> MarkerColors = {cmsBlue, cmsRed};
   vector<int> MarkerStyles = {mCircleFill, mSquareFill};
   vector<int> LineColors = {cmsBlue, cmsRed};
   vector<int> LineStyles = {0, 2};

   vector<TH1*> ZPt;
   vector<TH1*> ZEta;
   vector<TH1*> ZPhi;
   if(LoadHistograms(InputBases, TrkPtRange, ZPt, ZEta, ZPhi) == false)
   {
      cerr << "Unable to load official/private production histograms for tag " << Tag << endl;
      return -1;
   }

   gSystem->mkdir("plots/production", true);
   string OutputBase = Form("plots/production/pp_ZPT%s_trkPT%s_%s-production",
      ZPtRange.c_str(), TrkPtRange.c_str(), Tag.c_str());

   pair<double, double> ZPtBounds = ParseRange(ZPtRange);

   TCanvas CZPt("CZPt", "", 600, 600);
   TPad *PZPt = (TPad *)plotCMSRatio(ZPt, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "p_{T}^{Z}", ZPtBounds.first, ZPtBounds.second,
      "(1/N_{Z}) dN_{Z}/dp_{T}^{Z}", -1, -1,
      "Private / official", 0.7, 1.3,
      0, false, false, true, 0.55);
   AddCommonHeader(PZPt);
   CZPt.SaveAs(Form("%s-pt.pdf", OutputBase.c_str()));

   TCanvas CZEta("CZEta", "", 600, 600);
   TPad *PZEta = (TPad *)plotCMSRatio(ZEta, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "y_{Z}", -4, 4,
      "(1/N_{Z}) dN_{Z}/dy_{Z}", -1, -1,
      "Private / official", 0.7, 1.3,
      0, false, false, true, 0.55);
   AddCommonHeader(PZEta);
   CZEta.SaveAs(Form("%s-eta.pdf", OutputBase.c_str()));

   TCanvas CZPhi("CZPhi", "", 600, 600);
   TPad *PZPhi = (TPad *)plotCMSRatio(ZPhi, "", Labels,
      LineColors, LineStyles, MarkerColors, MarkerStyles,
      "#phi_{Z}", 0, 2 * M_PI,
      "(1/N_{Z}) dN_{Z}/d#phi_{Z}", -1, -1,
      "Private / official", 0.7, 1.3,
      0, false, false, true, 0.55);
   AddCommonHeader(PZPhi);
   CZPhi.SaveAs(Form("%s-phi.pdf", OutputBase.c_str()));

   return 0;
}
