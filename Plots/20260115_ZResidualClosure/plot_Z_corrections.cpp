// Standalone plotter for Z-correction factors with statistical error bars.
// Matches the style of MainAnalysis/20260115_ZCorrection/workflow/plot_corrections.C.
//
// Computes three correction iterations in-memory from saved GEN and RECO h3D
// histograms (closure-input snapshots) following correction.C logic — no
// CorrelationAnalysis rerun needed.
//
// Statistical errors: stored by TH1D::Divide() —
//   sigma_ratio = ratio * sqrt(sigma_num^2/num^2 + sigma_den^2/den^2)
//
// y-axis: pT panel auto-scaled symmetric about 1; eta/phi fixed 0.8-1.2.
//
// Usage:
//   ./ExecuteZCorrections --GenFile <path> --RecoFile <path>
//                         --System pp --Tag V8 --OutputDir plots/pp

#include <algorithm>
#include <cmath>
#include <string>
#include <initializer_list>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include "CommandLine.h"

using namespace std;

// Mirror of scaleTH3D from correction.C: multiply each bin of h3 by the
// 1D correction factor along the given axis ('x'=pT, 'y'=eta, 'z'=phi).
static void scaleTH3D(TH3D *h3, const TH1D *h1, char axis)
{
   int nx = h3->GetNbinsX(), ny = h3->GetNbinsY(), nz = h3->GetNbinsZ();
   for(int ix = 1; ix <= nx; ix++)
   for(int iy = 1; iy <= ny; iy++)
   for(int iz = 1; iz <= nz; iz++)
   {
      double sf = 1.0;
      if(axis == 'x') sf = h1->GetBinContent(ix);
      if(axis == 'y') sf = h1->GetBinContent(iy);
      if(axis == 'z') sf = h1->GetBinContent(iz);
      int bin = h3->GetBin(ix, iy, iz);
      h3->SetBinContent(bin, h3->GetBinContent(bin) * sf);
      h3->SetBinError  (bin, h3->GetBinError(bin)   * sf);
   }
}

// Compute one iteration of correction from hGen3D and the current hReco3D.
// Mirrors correction.C: project, divide, then sequentially scale hReco3D
// so it is ready for the next iteration.  Returns the three ratio histograms.
static void computeIteration(TH3D *hGen3D, TH3D *hReco3D,
                              const char *sfxPt, const char *sfxEta, const char *sfxPhi,
                              TH1D *&hPtCorr, TH1D *&hEtaCorr, TH1D *&hPhiCorr)
{
   TH1D *hPtGen  = hGen3D->ProjectionX(Form("hPtGen_%s",  sfxPt));
   TH1D *hEtaGen = hGen3D->ProjectionY(Form("hEtaGen_%s", sfxEta));
   TH1D *hPhiGen = hGen3D->ProjectionZ(Form("hPhiGen_%s", sfxPhi));

   // pT ratio
   TH1D *hPtReco = hReco3D->ProjectionX(Form("hPtReco_%s", sfxPt));
   hPtCorr = (TH1D*)hPtGen->Clone(Form("hPtCorr_%s", sfxPt));
   hPtCorr->Divide(hPtReco);
   scaleTH3D(hReco3D, hPtCorr, 'x');

   // eta ratio (after pT scaling)
   TH1D *hEtaReco = hReco3D->ProjectionY(Form("hEtaReco_%s", sfxEta));
   hEtaCorr = (TH1D*)hEtaGen->Clone(Form("hEtaCorr_%s", sfxEta));
   hEtaCorr->Divide(hEtaReco);
   scaleTH3D(hReco3D, hEtaCorr, 'y');

   // phi ratio (after pT+eta scaling)
   TH1D *hPhiReco = hReco3D->ProjectionZ(Form("hPhiReco_%s", sfxPhi));
   hPhiCorr = (TH1D*)hPhiGen->Clone(Form("hPhiCorr_%s", sfxPhi));
   hPhiCorr->Divide(hPhiReco);
   scaleTH3D(hReco3D, hPhiCorr, 'z');
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
   string tag       = CL.Get("Tag",       "V8");
   string outputDir = CL.Get("OutputDir", "plots/pp");

   gROOT->SetBatch(kTRUE);
   gStyle->SetOptStat(0);
   gStyle->SetTitleY(1.01);
   gStyle->SetTitleSize(0.9);
   gSystem->mkdir(outputDir.c_str(), true);

   // ---- load source 3D histograms ------------------------------------------
   TFile *fGen  = TFile::Open(genFile.c_str());
   TFile *fReco = TFile::Open(recoFile.c_str());
   if(!fGen || !fReco) { printf("Error opening source files\n"); return 1; }

   TH3D *hGen3D  = (TH3D*)fGen->Get("h3D");
   TH3D *hReco3D = (TH3D*)fReco->Get("h3D");
   if(!hGen3D || !hReco3D) { printf("h3D not found in source files\n"); return 1; }

   // ---- compute three iterations in-memory ---------------------------------
   // hReco3D is modified in-place by each call (scaled by accumulated correction)
   TH1D *hPtCorr1,  *hEtaCorr1, *hPhiCorr1;
   TH1D *hPtCorr2,  *hEtaCorr2, *hPhiCorr2;
   TH1D *hPtCorr3,  *hEtaCorr3, *hPhiCorr3;

   TH3D *hReco = (TH3D*)hReco3D->Clone("hRecoWork");
   computeIteration(hGen3D, hReco, "1","1","1", hPtCorr1, hEtaCorr1, hPhiCorr1);
   computeIteration(hGen3D, hReco, "2","2","2", hPtCorr2, hEtaCorr2, hPhiCorr2);
   computeIteration(hGen3D, hReco, "3","3","3", hPtCorr3, hEtaCorr3, hPhiCorr3);

   // ---- styling — match plot_corrections.C ---------------------------------
   hPtCorr1->SetLineColor(kRed);      hPtCorr2->SetLineColor(kBlue);      hPtCorr3->SetLineColor(kGreen + 2);
   hEtaCorr1->SetLineColor(kRed);     hEtaCorr2->SetLineColor(kBlue);     hEtaCorr3->SetLineColor(kGreen + 2);
   hPhiCorr1->SetLineColor(kRed);     hPhiCorr2->SetLineColor(kBlue);     hPhiCorr3->SetLineColor(kGreen + 2);
   for(TH1D *h : {hPtCorr1, hPtCorr2, hPtCorr3,
                  hEtaCorr1, hEtaCorr2, hEtaCorr3,
                  hPhiCorr1, hPhiCorr2, hPhiCorr3})
   {
      h->SetMarkerColor(h->GetLineColor());
      h->SetMarkerSize(0);
   }

   // ---- y-ranges: pT symmetric about 1, eta/phi fixed 0.8-1.2 -------------
   double ptHalf = symHalf({hPtCorr1, hPtCorr2, hPtCorr3}, 0.05);

   // ---- canvas: 2x2 pads ---------------------------------------------------
   TCanvas *c = new TCanvas("c", "Corrections", 1200, 1200);
   c->Divide(2, 2);

   // Pad 1: pT (log x)
   c->cd(1)->SetLogx();
   hPtCorr1->SetTitle("Z p_{T} Dependent Correction");
   hPtCorr1->GetYaxis()->SetRangeUser(1.0 - ptHalf, 1.0 + ptHalf);
   hPtCorr1->GetXaxis()->CenterTitle();
   hPtCorr1->SetTitleOffset(1.1, "X");
   hPtCorr1->SetTitleSize(0.055, "X");
   hPtCorr1->SetYTitle("Correction");
   hPtCorr1->GetYaxis()->CenterTitle();
   hPtCorr1->Draw("HIST E1");
   hPtCorr2->Draw("HIST E1 SAME");
   hPtCorr3->Draw("HIST E1 SAME");

   // Pad 2: eta
   c->cd(2);
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

   // Pad 3: phi
   c->cd(3);
   hPhiCorr1->SetTitle("Z #phi Dependent Correction");
   hPhiCorr1->GetYaxis()->SetRangeUser(0.8, 1.2);
   hPhiCorr1->GetXaxis()->CenterTitle();
   hPhiCorr1->SetTitleOffset(1.1, "X");
   hPhiCorr1->SetTitleSize(0.055, "X");
   hPhiCorr1->SetYTitle("Correction");
   hPhiCorr1->GetYaxis()->CenterTitle();
   hPhiCorr1->Draw("HIST E1");
   hPhiCorr2->Draw("HIST E1 SAME");
   hPhiCorr3->Draw("HIST E1 SAME");

   // Pad 4: legend
   c->cd(4);
   TLegend *legend = new TLegend(0.1, 0.7, 0.9, 0.9);
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
