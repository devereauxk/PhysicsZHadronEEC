// Quantify the effect of useZWeight (Z correction from file)
// by comparing existing _nominal vs _ZResidual outputs for pPb and PbP
#include <TFile.h>
#include <TH1D.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TColor.h>
#include <TPad.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "KylesPlotting.h"
#include "SetStyle.h"

int main() {
   SetThesisStyle();
   gStyle->SetOptStat(0);
   gSystem->mkdir("plots/ZWeight_study", kTRUE);

   string basedir = "../20241102_ZhadronVsZPt/plots/";
   string tag = "ZV8_trkV26_nmix10";
   string trkBin = "0.5_15";

   int cmsBlue = TColor::GetColor("#3f90da");

   struct System { string label, nomFile, zresFile; };
   vector<System> systems = {
      {"pPb", basedir + "pPb_nominal_" + tag + "_ZPT0_500-nosub.root",
              basedir + "pPb_ZResidual_" + tag + "_ZPT0_500-nosub.root"},
      {"PbP", basedir + "PbP_nominal_" + tag + "_ZPT0_500-nosub.root",
              basedir + "PbP_ZResidual_" + tag + "_ZPT0_500-nosub.root"}
   };

   struct Obs {
      string histName, xLabel, yLabel, outName;
      double xmin, xmax;
   };
   vector<Obs> observables = {
      {"DeltaPhi_Result" + trkBin, "#Delta#phi",
       "1/N_{Z} dN/d#Delta#phi", "DeltaPhi", 0, M_PI},
      {"DeltaEta_Result" + trkBin, "#Delta#eta",
       "1/N_{Z} dN/d#Delta#eta", "DeltaEta", -4, 4}
   };

   for (auto &sys : systems) {
      TFile *fNom = TFile::Open(sys.nomFile.c_str(), "READ");
      TFile *fZR  = TFile::Open(sys.zresFile.c_str(), "READ");
      if (!fNom || !fZR) { cout << "Missing file for " << sys.label << endl; continue; }

      TH1D *hNZ_nom = (TH1D*)fNom->Get(("hNZData_" + trkBin).c_str());
      TH1D *hNZ_zr  = (TH1D*)fZR->Get(("hNZData_" + trkBin).c_str());
      double nz_nom = hNZ_nom->GetBinContent(1);
      double nz_zr  = hNZ_zr->GetBinContent(1);
      printf("%s: NZ nominal=%.1f, ZResidual=%.1f, shift=%.2f%%\n",
             sys.label.c_str(), nz_nom, nz_zr, 100.0*(nz_zr - nz_nom)/nz_nom);

      for (auto &obs : observables) {
         TH1D *hNom = (TH1D*)((TH1D*)fNom->Get(obs.histName.c_str()))->Clone("hNom");
         TH1D *hZR  = (TH1D*)((TH1D*)fZR->Get(obs.histName.c_str()))->Clone("hZR");

         hNom->Scale(0.5 / nz_nom);
         hZR->Scale(0.5 / nz_zr);
         divideByWidth(hNom);
         divideByWidth(hZR);

         vector<TH1*> overlay = {(TH1*)hNom, (TH1*)hZR};
         vector<string> labels = {"nominal (no Z corr)", "+Z correction"};
         vector<Int_t> lineColors   = {kBlack, cmsBlue};
         vector<Int_t> lineStyles   = {1, 1};
         vector<Int_t> markerColors = {kBlack, cmsBlue};
         vector<Int_t> markerStyles = {20, 21};

         TCanvas *c = new TCanvas(Form("c_%s_%s", sys.label.c_str(), obs.outName.c_str()), "", 700, 700);
         TPad *pad = plotCMSRatio(overlay,
                Form("pad_%s_%s", sys.label.c_str(), obs.outName.c_str()),
                labels, lineColors, lineStyles, markerColors, markerStyles,
                obs.xLabel.c_str(), obs.xmin, obs.xmax,
                obs.yLabel.c_str(), -1, -1,
                "+Z / nom", 0.95, 1.05,
                0, false, false, true, 0.50);

         AddUPCHeader(pad, "8.16 TeV", Form("%s Z correction effect", sys.label.c_str()));

         string outPath = Form("plots/ZWeight_study/%s_%s_ZCorr_effect.pdf",
                                sys.label.c_str(), obs.outName.c_str());
         c->SaveAs(outPath.c_str());
         delete c;
      }
      fNom->Close();
      fZR->Close();
   }

   return 0;
}
