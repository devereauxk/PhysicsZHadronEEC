// PlotNmixComparison.cpp
// Overlay of corrected-data and EPOS gen results at nmix = 1, 5, 10, 15, 20
// to demonstrate convergence of the mixed-event background estimate.
//
// Three panels per observable (DeltaPhi / DeltaEta) x type (result / bkg):
//   pp, combined pPb, combined EPOS gen
// Each panel: top = 5-curve overlay; bottom = nmix_N - nmix_20.

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLatex.h"
#include "TPad.h"
#include "TROOT.h"
#include "TStyle.h"

#include "CommandLine.h"
#include "KylesPlotting.h"
#include "SetStyle.h"

#include "PlotComparisonUtilities.h"
#include "ResultCombinationUtilities.h"

// Build combined pPb bkg projection from two nosub files.
TH1D *BuildCombinedBkgHistogram(TFile &pPbFile, TFile &pbpFile,
   const string &observable, const string &trackRange, const string &name)
{
   TH2D *bkgPPb = LoadCloned2D(pPbFile, "hMixData_" + trackRange, name + "_bkgPPb");
   TH2D *bkgPbp = LoadCloned2D(pbpFile, "hMixData_" + trackRange, name + "_bkgPbp");
   TH1D *nzPPb  = LoadCloned1D(pPbFile, "hNZMixData_" + trackRange, name + "_nzPPb");
   TH1D *nzPbp  = LoadCloned1D(pbpFile, "hNZMixData_" + trackRange, name + "_nzPbp");

   if (!bkgPPb || !bkgPbp || !nzPPb || !nzPbp) {
      delete bkgPPb; delete bkgPbp; delete nzPPb; delete nzPbp;
      return nullptr;
   }

   double nzPPbVal = nzPPb->GetBinContent(1);
   double nzPbpVal = nzPbp->GetBinContent(1);
   bkgPPb->Scale(nzPPbVal);
   bkgPbp->Scale(nzPbpVal);
   bkgPPb->Add(bkgPbp);
   double totalNZ = nzPPbVal + nzPbpVal;
   if (totalNZ != 0) bkgPPb->Scale(1.0 / totalNZ);

   TH1D *result = ProjectOfficial20x20ResultObservable(bkgPPb, observable, name);

   delete bkgPPb; delete bkgPbp; delete nzPPb; delete nzPbp;
   return result;
}

// Build bkg projection for a single-system nosub file (pp).
TH1D *BuildSingleBkgHistogram(TFile &file,
   const string &observable, const string &trackRange, const string &name)
{
   TH2D *bkg = LoadCloned2D(file, "hMixData_" + trackRange, name + "_bkg2D");
   if (!bkg) return nullptr;

   TH1D *result = ProjectOfficial20x20ResultObservable(bkg, observable, name);
   delete bkg;
   return result;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   gROOT->SetBatch(kTRUE);
   SetThesisStyle();
   gStyle->SetOptStat(0);

   string studyDir   = CL.Get("StudyDir",   "plots");
   string centralDir = CL.Get("CentralDir", "plots");
   string outputDir  = CL.Get("OutputDir",  "plots/nmixStudy");
   string zptRange   = CL.Get("ZPTRange",   "0_500");
   string trackRange = CL.Get("TrackPTRange", "0.5_15");
   string ppTagBase  = CL.Get("PPTagBase",  "EEV5_ZV9_trkV28_nmix");
   string ppbTagBase = CL.Get("PPBTagBase", "ZV9_trkV28_nmix");

   vector<int>    nmixValues = {1, 5, 10, 15, 20};
   vector<string> nmixLabels = {"N_{mix}=1", "N_{mix}=5", "N_{mix}=10",
                                "N_{mix}=15", "N_{mix}=20"};
   vector<int>    nmixColors = {kRed+1, kOrange+1, kGreen+2, kBlue+1, kBlack};
   vector<int>    nmixMarkers = {20, 21, 22, 23, 29};

   // Precompute file paths for each nmix value.
   struct NmixFiles {
      string ppResult, ppNosub;
      string pPbNosub, pbpNosub;
      string pPbGenNosub, pbpGenNosub;
   };

   vector<NmixFiles> files(nmixValues.size());
   for (size_t i = 0; i < nmixValues.size(); i++) {
      int N = nmixValues[i];
      string ppTag  = ppTagBase  + to_string(N);
      string ppbTag = ppbTagBase + to_string(N);
      string dir    = (N == 10) ? centralDir : studyDir;

      files[i].ppResult     = dir + "/pp_trkResidual_"          + ppTag  + "_ZPT" + zptRange + "-result.root";
      files[i].ppNosub      = dir + "/pp_trkResidual_"          + ppTag  + "_ZPT" + zptRange + "-nosub.root";
      files[i].pPbNosub     = dir + "/pPb_trkResidual_"         + ppbTag + "_ZPT" + zptRange + "-nosub.root";
      files[i].pbpNosub     = dir + "/PbP_trkResidual_"         + ppbTag + "_ZPT" + zptRange + "-nosub.root";
      files[i].pPbGenNosub  = dir + "/pPbMC_Gen_nominal_"       + ppbTag + "_ZPT" + zptRange + "-nosub.root";
      files[i].pbpGenNosub  = dir + "/PbPMC_Gen_nominal_"       + ppbTag + "_ZPT" + zptRange + "-nosub.root";
   }

   vector<string> observables = {"DeltaEta", "DeltaPhi"};
   vector<string> types       = {"result", "bkg"};

   // collision id, label, energy label
   struct CollisionSpec { string id, label; };
   vector<CollisionSpec> collisions = {
      {"pp",      "pp"},
      {"pPbPbp",  "pPb"},
      {"EPOSgen", "pPb (EPOS gen)"}
   };

   for (const string &observable : observables) {
      for (const string &type : types) {
         for (const CollisionSpec &col : collisions) {

            // Load histograms for all nmix values.
            vector<TH1 *> histos;
            for (size_t i = 0; i < nmixValues.size(); i++) {
               const NmixFiles &F = files[i];
               TH1D *h = nullptr;
               string suffix = "_" + col.id + "_" + observable + "_" + type
                               + "_nmix" + to_string(nmixValues[i]);

               if (type == "result") {
                  if (col.id == "pp") {
                     TFile *f = TFile::Open(F.ppResult.c_str());
                     if (!f || f->IsZombie()) {
                        cerr << "Cannot open " << F.ppResult << endl;
                        histos.push_back(nullptr);
                        if (f) f->Close();
                        continue;
                     }
                     h = LoadSingleResultHistogram(*f, observable, trackRange, suffix);
                     f->Close(); delete f;
                  } else if (col.id == "pPbPbp") {
                     TFile *fPPb = TFile::Open(F.pPbNosub.c_str());
                     TFile *fPbp = TFile::Open(F.pbpNosub.c_str());
                     if (!fPPb || fPPb->IsZombie() || !fPbp || fPbp->IsZombie()) {
                        cerr << "Cannot open pPb/PbP nosub files for nmix=" << nmixValues[i] << endl;
                        histos.push_back(nullptr);
                        if (fPPb) { fPPb->Close(); delete fPPb; }
                        if (fPbp) { fPbp->Close(); delete fPbp; }
                        continue;
                     }
                     h = BuildCombinedResultHistogram(*fPPb, *fPbp, observable, trackRange, suffix);
                     fPPb->Close(); delete fPPb;
                     fPbp->Close(); delete fPbp;
                  } else { // EPOSgen
                     TFile *fPPb = TFile::Open(F.pPbGenNosub.c_str());
                     TFile *fPbp = TFile::Open(F.pbpGenNosub.c_str());
                     if (!fPPb || fPPb->IsZombie() || !fPbp || fPbp->IsZombie()) {
                        cerr << "Cannot open EPOS gen nosub files for nmix=" << nmixValues[i] << endl;
                        histos.push_back(nullptr);
                        if (fPPb) { fPPb->Close(); delete fPPb; }
                        if (fPbp) { fPbp->Close(); delete fPbp; }
                        continue;
                     }
                     h = BuildCombinedResultHistogram(*fPPb, *fPbp, observable, trackRange, suffix);
                     fPPb->Close(); delete fPPb;
                     fPbp->Close(); delete fPbp;
                  }
               } else { // bkg
                  if (col.id == "pp") {
                     TFile *f = TFile::Open(F.ppNosub.c_str());
                     if (!f || f->IsZombie()) {
                        cerr << "Cannot open " << F.ppNosub << endl;
                        histos.push_back(nullptr);
                        if (f) f->Close();
                        continue;
                     }
                     h = BuildSingleBkgHistogram(*f, observable, trackRange, suffix);
                     f->Close(); delete f;
                  } else if (col.id == "pPbPbp") {
                     TFile *fPPb = TFile::Open(F.pPbNosub.c_str());
                     TFile *fPbp = TFile::Open(F.pbpNosub.c_str());
                     if (!fPPb || fPPb->IsZombie() || !fPbp || fPbp->IsZombie()) {
                        cerr << "Cannot open pPb/PbP nosub files for bkg nmix=" << nmixValues[i] << endl;
                        histos.push_back(nullptr);
                        if (fPPb) { fPPb->Close(); delete fPPb; }
                        if (fPbp) { fPbp->Close(); delete fPbp; }
                        continue;
                     }
                     h = BuildCombinedBkgHistogram(*fPPb, *fPbp, observable, trackRange, suffix);
                     fPPb->Close(); delete fPPb;
                     fPbp->Close(); delete fPbp;
                  } else { // EPOSgen
                     TFile *fPPb = TFile::Open(F.pPbGenNosub.c_str());
                     TFile *fPbp = TFile::Open(F.pbpGenNosub.c_str());
                     if (!fPPb || fPPb->IsZombie() || !fPbp || fPbp->IsZombie()) {
                        cerr << "Cannot open EPOS gen nosub files for bkg nmix=" << nmixValues[i] << endl;
                        histos.push_back(nullptr);
                        if (fPPb) { fPPb->Close(); delete fPPb; }
                        if (fPbp) { fPbp->Close(); delete fPbp; }
                        continue;
                     }
                     h = BuildCombinedBkgHistogram(*fPPb, *fPbp, observable, trackRange, suffix);
                     fPPb->Close(); delete fPPb;
                     fPbp->Close(); delete fPbp;
                  }
               }

               histos.push_back(h);
            }

            // Check we have the reference (nmix=20, last entry).
            TH1D *reference = (TH1D *)histos.back();
            if (!reference) {
               cerr << "Missing nmix=20 reference for " << col.id << "/" << observable << "/" << type << endl;
               for (TH1 *h : histos) delete h;
               continue;
            }

            // Style histograms.
            for (size_t i = 0; i < histos.size(); i++) {
               if (!histos[i]) continue;
               histos[i]->SetLineColor(nmixColors[i]);
               histos[i]->SetMarkerColor(nmixColors[i]);
               histos[i]->SetMarkerStyle(nmixMarkers[i]);
               histos[i]->SetMarkerSize(0.7);
               histos[i]->SetLineWidth(1);
            }

            // Determine y-range.
            pair<double,double> xRange = GetObservableRange(observable);
            pair<double,double> yRange;
            if (type == "result")
               yRange = GetComparisonYRange(histos, observable);
            else {
               // For bkg, auto-scale around content range.
               double mn = 1e30, mx = -1e30;
               for (TH1 *h : histos) {
                  if (!h) continue;
                  for (int b = 1; b <= h->GetNbinsX(); b++) {
                     double v = h->GetBinContent(b);
                     if (v < mn) mn = v;
                     if (v > mx) mx = v;
                  }
               }
               if (mx < mn) { mn = 0; mx = 1; }
               double range = mx - mn;
               if (range <= 0) range = max(abs(mx), 1.0);
               yRange = {max(0.0, mn - range*0.10), mx + range*0.40};
            }

            // Difference range.
            pair<double,double> diffRange = {-0.15, 0.15};

            // Build the labels vector (skip nulls).
            vector<TH1 *> validHistos;
            vector<string> validLabels;
            vector<int> validColors, validMarkers;
            for (size_t i = 0; i < histos.size(); i++) {
               if (!histos[i]) continue;
               validHistos.push_back(histos[i]);
               validLabels.push_back(nmixLabels[i]);
               validColors.push_back(nmixColors[i]);
               validMarkers.push_back(nmixMarkers[i]);
            }

            if (validHistos.size() < 2) {
               cerr << "Not enough histograms for " << col.id << "/" << observable << "/" << type << endl;
               for (TH1 *h : histos) delete h;
               continue;
            }

            // Reference is the last valid histogram (nmix=20).
            int refIdx = (int)validHistos.size() - 1;

            TCanvas canvas(("Canvas_" + col.id + "_" + observable + "_" + type).c_str(), "", 600, 600);
            TPad *pad = (TPad *)plotCMSDiff(
               validHistos,
               ("NmixStudy_" + col.id + "_" + observable + "_" + type).c_str(),
               validLabels, validColors, vector<int>(validHistos.size(), 0),
               validColors, validMarkers,
               GetObservableLabel(observable).c_str(), xRange.first, xRange.second,
               GetResultAxisLabel(observable).c_str(), yRange.first, yRange.second,
               "N_{mix}=N - N_{mix}=20", diffRange.first, diffRange.second,
               refIdx,
               false, false, true,
               0.56
            );

            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "8.16 TeV", GetCollisionLabel(col.id == "EPOSgen" ? "EPOSgen" : col.id).c_str());

            // Draw additional label for EPOS gen.
            if (col.id == "EPOSgen") {
               pad->cd();
               TLatex latex;
               latex.SetNDC();
               latex.SetTextFont(42);
               latex.SetTextSize(0.030);
               latex.DrawLatex(0.18, 0.89, "Powheg+EPOS gen");
            }

            DrawKinematicLabels(pad,
               (string("N_{mix} convergence") + (type == "bkg" ? " (bkg)" : " (result)")).c_str(),
               zptRange, trackRange);

            canvas.Update();
            string outName = outputDir + "/NmixStudy_" + col.id + "_ZPT" + zptRange
                             + "_trkPT" + trackRange + "-" + observable + "-" + type + ".pdf";
            canvas.SaveAs(outName.c_str());
            cout << "Saved: " << outName << endl;

            for (TH1 *h : histos) delete h;
         } // type
      } // observable
   } // collision

   return 0;
}
