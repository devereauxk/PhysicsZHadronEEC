// Standalone plotter for Z-correction factors with statistical error bars.
// Matches the style of MainAnalysis/20260115_ZCorrection/workflow/plot_corrections.C.
//
// Computes three correction iterations in-memory from saved GEN and RECO h2D
// histograms (closure-input snapshots) following correction.C logic — no
// CorrelationAnalysis rerun needed.
//
// Statistical errors: stored by TH1D::Divide() —
//   sigma_ratio = ratio * sqrt(sigma_num^2/num^2 + sigma_den^2/den^2)
//
// y-axis: pT panel auto-scaled symmetric about 1; eta fixed 0.8-1.2.
//
// Usage:
//   ./ExecuteZCorrections --GenFile <path> --RecoFile <path>
//                         --System pp --Tag V9 --OutputDir plots/pp

#include <algorithm>
#include <cmath>
#include <string>
#include <initializer_list>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TPad.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include "CommandLine.h"

using namespace std;

// Mirror of scaleTH2D from correction.C: multiply each bin of h2D by the
// 1D correction factor along the given axis ('x'=pT, 'y'=eta).
static void scaleTH2D(TH2D *h2, const TH1D *h1, char axis)
{
   int nx = h2->GetNbinsX(), ny = h2->GetNbinsY();
   for(int ix = 1; ix <= nx; ix++)
   for(int iy = 1; iy <= ny; iy++)
   {
      double sf = (axis == 'x') ? h1->GetBinContent(ix) : h1->GetBinContent(iy);
      int bin = h2->GetBin(ix, iy);
      h2->SetBinContent(bin, h2->GetBinContent(bin) * sf);
      h2->SetBinError  (bin, h2->GetBinError(bin)   * sf);
   }
}

// Compute one iteration of correction from hGen2D and the current hReco2D.
// Mirrors correction.C: project, divide, then sequentially scale hReco2D
// so it is ready for the next iteration.  Returns the two ratio histograms.
static void computeIteration(TH2D *hGen2D, TH2D *hReco2D,
                              const char *sfxPt, const char *sfxEta,
                              TH1D *&hPtCorr, TH1D *&hEtaCorr)
{
   TH1D *hPtGen  = hGen2D->ProjectionX(Form("hPtGen_%s",  sfxPt));
   TH1D *hEtaGen = hGen2D->ProjectionY(Form("hEtaGen_%s", sfxEta));

   // pT ratio
   TH1D *hPtReco = hReco2D->ProjectionX(Form("hPtReco_%s", sfxPt));
   hPtCorr = (TH1D*)hPtGen->Clone(Form("hPtCorr_%s", sfxPt));
   hPtCorr->Divide(hPtReco);
   scaleTH2D(hReco2D, hPtCorr, 'x');

   // eta ratio (after pT scaling)
   TH1D *hEtaReco = hReco2D->ProjectionY(Form("hEtaReco_%s", sfxEta));
   hEtaCorr = (TH1D*)hEtaGen->Clone(Form("hEtaCorr_%s", sfxEta));
   hEtaCorr->Divide(hEtaReco);
   scaleTH2D(hReco2D, hEtaCorr, 'y');
}

// Upper y-bound for a symmetric-about-1 range: find max |val ± err - 1|
// across all histograms, add padding, then range = [1-half, 1+half].
static double symHalf(initializer_list<TH1D*> hs, double pad)
{
   double maxdev = 0;
   for(auto *h : hs)
      for(int i = 1; i <= h->GetNbinsX(); i++)
      {
         double v = h->GetBinContent(i), e = h->GetBinError(i);
         if(v <= 0) continue;
         maxdev = max(maxdev, fabs(v + e - 1.0));
         maxdev = max(maxdev, fabs(v - e - 1.0));
      }
   return maxdev * (1.0 + pad);
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);
   string genFile   = CL.Get("GenFile");
   string recoFile  = CL.Get("RecoFile");
   string system    = CL.Get("System",    "pp");
   string tag       = CL.Get("Tag",       "V9");
   string outputDir = CL.Get("OutputDir", "plots/pp");

   gROOT->SetBatch(kTRUE);
   gStyle->SetOptStat(0);
   gStyle->SetTitleY(1.01);
   gStyle->SetTitleSize(0.9);
   gSystem->mkdir(outputDir.c_str(), true);

   // ---- load source 2D histograms ------------------------------------------
   TFile *fGen  = TFile::Open(genFile.c_str());
   TFile *fReco = TFile::Open(recoFile.c_str());
   if(!fGen || !fReco) { printf("Error opening source files\n"); return 1; }

   TH2D *hGen2D  = (TH2D*)fGen->Get("h2D");
   TH2D *hReco2D = (TH2D*)fReco->Get("h2D");
   if(!hGen2D || !hReco2D) { printf("h2D not found in source files\n"); return 1; }

   // ---- compute three iterations in-memory ---------------------------------
   TH1D *hPtCorr1,  *hEtaCorr1;
   TH1D *hPtCorr2,  *hEtaCorr2;
   TH1D *hPtCorr3,  *hEtaCorr3;

   TH2D *hReco = (TH2D*)hReco2D->Clone("hRecoWork");
   computeIteration(hGen2D, hReco, "1","1", hPtCorr1, hEtaCorr1);
   computeIteration(hGen2D, hReco, "2","2", hPtCorr2, hEtaCorr2);
   computeIteration(hGen2D, hReco, "3","3", hPtCorr3, hEtaCorr3);

   // ---- styling — match plot_corrections.C ---------------------------------
   hPtCorr1->SetLineColor(kRed);      hPtCorr2->SetLineColor(kBlue);      hPtCorr3->SetLineColor(kGreen + 2);
   hEtaCorr1->SetLineColor(kRed);     hEtaCorr2->SetLineColor(kBlue);     hEtaCorr3->SetLineColor(kGreen + 2);
   for(TH1D *h : {hPtCorr1, hPtCorr2, hPtCorr3, hEtaCorr1, hEtaCorr2, hEtaCorr3})
   {
      h->SetMarkerColor(h->GetLineColor());
      h->SetMarkerSize(0);
   }

   // ---- y-ranges: pT symmetric about 1, eta fixed 0.8-1.2 -----------------
   double ptHalf = symHalf({hPtCorr1, hPtCorr2, hPtCorr3}, 0.05);

   // ---- canvas: 3-panel (pT | eta | compact legend) ------------------------
   TCanvas *c = new TCanvas("c", "Corrections", 1800, 600);

   TPad *p1 = new TPad("p1", "", 0.00, 0.00, 0.40, 1.00); p1->Draw();
   TPad *p2 = new TPad("p2", "", 0.40, 0.00, 0.80, 1.00); p2->Draw();
   TPad *p3 = new TPad("p3", "", 0.80, 0.60, 1.00, 1.00); p3->Draw();

   // Pad 1: pT (log x), x range 0.5-500 to suppress the 0-0.5 stub bin
   p1->cd(); p1->SetLogx();
   hPtCorr1->SetTitle("Z p_{T} Dependent Correction");
   hPtCorr1->GetYaxis()->SetRangeUser(1.0 - ptHalf, 1.0 + ptHalf);
   hPtCorr1->GetXaxis()->SetRangeUser(0.5, 500.0);
   hPtCorr1->GetXaxis()->CenterTitle();
   hPtCorr1->SetTitleOffset(1.1, "X");
   hPtCorr1->SetTitleSize(0.055, "X");
   hPtCorr1->SetYTitle("Correction");
   hPtCorr1->GetYaxis()->CenterTitle();
   hPtCorr1->Draw("HIST E1");
   hPtCorr2->Draw("HIST E1 SAME");
   hPtCorr3->Draw("HIST E1 SAME");

   // Pad 2: eta
   p2->cd();
   hEtaCorr1->SetTitle("Z #eta Dependent Correction");
   hEtaCorr1->GetYaxis()->SetRangeUser(0.8, 1.2);
   hEtaCorr1->GetXaxis()->CenterTitle();
   hEtaCorr1->SetTitleOffset(1.1, "X");
   hEtaCorr1->SetTitleSize(0.055, "X");
   hEtaCorr1->SetYTitle("Correction");
   hEtaCorr1->GetYaxis()->CenterTitle();
   hEtaCorr1->Draw("HIST E1");
   hEtaCorr2->Draw("HIST E1 SAME");
   hEtaCorr3->Draw("HIST E1 SAME");

   // Pad 3: compact legend (top-right only, no white space below)
   p3->cd();
   TLegend *legend = new TLegend(0.05, 0.05, 0.95, 0.95);
   legend->AddEntry(hPtCorr1, "Iteration 1", "le");
   legend->AddEntry(hPtCorr2, "Iteration 2", "le");
   legend->AddEntry(hPtCorr3, "Iteration 3", "le");
   legend->Draw();

   // ---- save ---------------------------------------------------------------
   string outPath = outputDir + "/corrections_" + system + "_withErr_" + tag + ".pdf";
   c->SaveAs(outPath.c_str());
   printf("Saved: %s\n", outPath.c_str());

   return 0;
}
