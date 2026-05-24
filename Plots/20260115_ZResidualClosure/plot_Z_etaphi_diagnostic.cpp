#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TROOT.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"

TH2D *projectEtaPhi(TH3D *h3D, const string &name)
{
   if(h3D == nullptr) return nullptr;
   TH2D *h2D = (TH2D *)h3D->Project3D("zy");
   h2D->SetName(name.c_str());
   h2D->SetDirectory(nullptr);
   return h2D;
}

TH2D *loadEtaPhi(const string &fileName, const string &histName, const string &nzName,
   const string &cloneName, bool normalize)
{
   TFile *f = TFile::Open(fileName.c_str(), "READ");
   if(f == nullptr || f->IsZombie())
   {
      cerr << "Cannot open " << fileName << endl;
      return nullptr;
   }

   TH3D *h3D = (TH3D *)f->Get(histName.c_str());
   if(h3D == nullptr)
   {
      cerr << "Missing " << histName << " in " << fileName << endl;
      return nullptr;
   }

   TH2D *h2D = projectEtaPhi(h3D, cloneName);

   if(normalize)
   {
      TH1D *hNZ = (TH1D *)f->Get(nzName.c_str());
      if(hNZ != nullptr && hNZ->GetBinContent(1) > 0)
         h2D->Scale(1.0 / hNZ->GetBinContent(1));
   }

   return h2D;
}

void drawMap(TH2D *h, const string &title, const string &outputPath,
   const string &collisionType, double zMin, double zMax)
{
   if(h == nullptr) return;

   gStyle->SetPalette(kBird);
   gStyle->SetOptStat(0);

   h->SetTitle(Form(";y_{Z};#phi_{Z};%s", title.c_str()));
   h->SetMinimum(zMin);
   h->SetMaximum(zMax);
   h->SetContour(100);

   TCanvas *c = new TCanvas("c_map", "c_map", 700, 600);
   c->SetRightMargin(0.18);
   c->SetLeftMargin(0.12);
   c->SetBottomMargin(0.12);

   h->Draw("COLZ");
   AddCMSHeader((TPad *)c, "Internal", false);
   AddUPCHeader((TPad *)c, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV",
      collisionType.c_str());

   c->Update();
   c->SaveAs(outputPath.c_str());
   delete c;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   gROOT->SetBatch(kTRUE);
   SetTDRStyle();

   string collisionType = CL.Get("collisionType", "pp");
   string inputTag = CL.Get("inputTag", "20260407_ZCorrection_V8");
   string dataTag = CL.Get("dataTag", "");
   string outputDir = CL.Get("outputDir", "plots");

   string mcPrefix = (collisionType == "pp") ? "pythia" : collisionType;
   string base = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow/output/closure_inputs";

   string genFile = Form("%s/%s_%s_zPt0-500_gen.root", base.c_str(), collisionType.c_str(), inputTag.c_str());
   string recoFile = Form("%s/%s_%s_zPt0-500_reco.root", base.c_str(), collisionType.c_str(), inputTag.c_str());
   string correctedFile = Form("%s/%s_%s_zPt0-500_corrected.root", base.c_str(), collisionType.c_str(), inputTag.c_str());

   cout << "Loading gen: " << genFile << endl;
   cout << "Loading reco: " << recoFile << endl;
   cout << "Loading corrected: " << correctedFile << endl;

   TH2D *hGen = loadEtaPhi(genFile, "h3D", "hNZ", "hZEtaPhi_gen", true);
   TH2D *hReco = loadEtaPhi(recoFile, "h3D", "hNZ", "hZEtaPhi_reco", true);
   TH2D *hCorrected = loadEtaPhi(correctedFile, "h3D", "hNZ", "hZEtaPhi_corrected", true);

   TH2D *hData = nullptr;
   if(dataTag != "")
   {
      string dataFile = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%s_nominal_%s_ZPT0_500-nosub.root",
         collisionType.c_str(), dataTag.c_str());
      cout << "Loading data: " << dataFile << endl;

      TFile *fData = TFile::Open(dataFile.c_str(), "READ");
      if(fData != nullptr && !fData->IsZombie())
      {
         TH3D *h3D = nullptr;
         TH1D *hNZ = nullptr;
         const char *trkKeys[] = {"0.5_15", "0.5_500"};
         for(const char *tk : trkKeys)
         {
            h3D = (TH3D *)fData->Get(Form("hZPtEtaPhi_%s", tk));
            hNZ = (TH1D *)fData->Get(Form("hNZData_%s", tk));
            if(h3D != nullptr && hNZ != nullptr) break;
         }
         if(h3D != nullptr)
         {
            hData = projectEtaPhi(h3D, "hZEtaPhi_data");
            if(hNZ != nullptr && hNZ->GetBinContent(1) > 0)
               hData->Scale(1.0 / hNZ->GetBinContent(1));
         }
         else
            cerr << "Missing hZPtEtaPhi in data file" << endl;
      }
   }

   string outBase = Form("%s/%s/ZPT0_500_Z_etaphi_diagnostic", outputDir.c_str(), collisionType.c_str());

   double sharedMax = 0;
   if(hGen) sharedMax = max(sharedMax, hGen->GetMaximum());
   if(hReco) sharedMax = max(sharedMax, hReco->GetMaximum());
   if(hCorrected) sharedMax = max(sharedMax, hCorrected->GetMaximum());
   if(hData) sharedMax = max(sharedMax, hData->GetMaximum());

   drawMap(hGen, "(1/N_{Z}) Z counts (MC GEN)", outBase + "_gen.pdf", collisionType, 0, sharedMax);
   drawMap(hReco, "(1/N_{Z}) Z counts (MC RECO)", outBase + "_reco.pdf", collisionType, 0, sharedMax);
   drawMap(hCorrected, "(1/N_{Z}) Z counts (MC RECO corrected)", outBase + "_corrected.pdf", collisionType, 0, sharedMax);
   if(hData)
      drawMap(hData, "(1/N_{Z}) Z counts (Data)", outBase + "_data.pdf", collisionType, 0, sharedMax);

   if(hReco != nullptr && hGen != nullptr)
   {
      TH2D *hRatioReco = (TH2D *)hReco->Clone("hRatioReco");
      hRatioReco->Divide(hGen);
      drawMap(hRatioReco, "MC RECO / GEN", outBase + "_ratio_reco_gen.pdf", collisionType, 0.8, 1.2);
   }

   if(hCorrected != nullptr && hGen != nullptr)
   {
      TH2D *hRatioCorrected = (TH2D *)hCorrected->Clone("hRatioCorrected");
      hRatioCorrected->Divide(hGen);
      drawMap(hRatioCorrected, "MC RECO corrected / GEN", outBase + "_ratio_corrected_gen.pdf", collisionType, 0.8, 1.2);
   }

   if(hData != nullptr && hGen != nullptr)
   {
      TH2D *hRatioData = (TH2D *)hData->Clone("hRatioData");
      hRatioData->Divide(hGen);
      drawMap(hRatioData, "Data / MC GEN", outBase + "_ratio_data_gen.pdf", collisionType, 0.8, 1.2);
   }

   return 0;
}
