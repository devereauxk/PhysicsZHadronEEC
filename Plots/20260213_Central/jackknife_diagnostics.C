#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TLine.h>
#include <TSystem.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void styleCentral(TH1D *h, int color, int marker)
{
   h->SetLineColor(color);
   h->SetMarkerColor(color);
   h->SetMarkerStyle(marker);
}

void styleValue(TH1D *h, int color, int marker)
{
   h->SetLineColor(color);
   h->SetMarkerColor(color);
   h->SetMarkerStyle(marker);
}

double median(vector<double> values)
{
   if(values.empty())
      return 0;
   sort(values.begin(), values.end());
   if(values.size() % 2 == 1)
      return values[values.size() / 2];
   return 0.5 * (values[values.size() / 2 - 1] + values[values.size() / 2]);
}

void makeOneDiagnostic(const string &system, const string &observable,
   TH1D *nominal, TH1D *jackknife, TH1D *sumw2, const string &pdfPath,
   ofstream &report)
{
   double maxNominalDiff = 0;
   double maxSameFileDiff = 0;
   vector<double> ratios;

   TH1D *errorSumw2 = (TH1D *)sumw2->Clone(Form("%s_%s_errorSumw2", system.c_str(), observable.c_str()));
   TH1D *errorJackknife = (TH1D *)jackknife->Clone(Form("%s_%s_errorJackknife", system.c_str(), observable.c_str()));
   TH1D *ratio = (TH1D *)jackknife->Clone(Form("%s_%s_ratio", system.c_str(), observable.c_str()));

   for(int i = 1; i <= jackknife->GetNbinsX(); i++)
   {
      maxNominalDiff = max(maxNominalDiff, fabs(nominal->GetBinContent(i) - jackknife->GetBinContent(i)));
      maxSameFileDiff = max(maxSameFileDiff, fabs(sumw2->GetBinContent(i) - jackknife->GetBinContent(i)));
      double sumw2Error = sumw2->GetBinError(i);
      double jackknifeError = jackknife->GetBinError(i);
      errorSumw2->SetBinContent(i, sumw2Error);
      errorSumw2->SetBinError(i, 0);
      errorJackknife->SetBinContent(i, jackknifeError);
      errorJackknife->SetBinError(i, 0);
      double thisRatio = (sumw2Error > 0) ? jackknifeError / sumw2Error : 0;
      ratio->SetBinContent(i, thisRatio);
      ratio->SetBinError(i, 0);
      if(sumw2Error > 0)
         ratios.push_back(thisRatio);
   }

   double ratioMin = ratios.empty() ? 0 : *min_element(ratios.begin(), ratios.end());
   double ratioMax = ratios.empty() ? 0 : *max_element(ratios.begin(), ratios.end());
   double ratioMedian = median(ratios);

   report << observable << ": max |central_jk - central_nominal| = " << maxNominalDiff
      << "; max |central_jk - central_sumw2_samefile| = " << maxSameFileDiff
      << "; sigma_JK/sigma_Sumw2 median = " << ratioMedian
      << ", range = [" << ratioMin << ", " << ratioMax << "]" << endl;

   TCanvas canvas(Form("c_%s_%s", system.c_str(), observable.c_str()), "", 800, 900);
   canvas.Divide(1, 3);

   canvas.cd(1);
   gPad->SetBottomMargin(0.02);
   styleCentral(nominal, kBlack, 20);
   styleCentral(jackknife, kRed + 1, 24);
   double ymax = max(nominal->GetMaximum(), jackknife->GetMaximum());
   double ymin = min(nominal->GetMinimum(), jackknife->GetMinimum());
   if(ymax <= ymin)
   {
      ymax = ymax + 1;
      ymin = ymin - 1;
   }
   nominal->SetTitle(Form("%s %s diagnostics;%s;value", system.c_str(), observable.c_str(), nominal->GetXaxis()->GetTitle()));
   nominal->SetMinimum(ymin - 0.15 * (ymax - ymin));
   nominal->SetMaximum(ymax + 0.25 * (ymax - ymin));
   nominal->Draw("E1");
   jackknife->Draw("E1 SAME");
   TLegend legend1(0.52, 0.75, 0.88, 0.88);
   legend1.AddEntry(nominal, "Nominal full rerun", "lp");
   legend1.AddEntry(jackknife, "Jackknife full rerun", "lp");
   legend1.Draw();

   canvas.cd(2);
   gPad->SetBottomMargin(0.02);
   styleValue(errorSumw2, kBlue + 1, 20);
   styleValue(errorJackknife, kRed + 1, 24);
   double emax = max(errorSumw2->GetMaximum(), errorJackknife->GetMaximum());
   errorSumw2->SetTitle(Form(";%s;stat. uncertainty", errorSumw2->GetXaxis()->GetTitle()));
   errorSumw2->SetMinimum(0);
   errorSumw2->SetMaximum((emax > 0) ? emax * 1.35 : 1);
   errorSumw2->Draw("HIST P");
   errorJackknife->Draw("HIST P SAME");
   TLegend legend2(0.55, 0.75, 0.88, 0.88);
   legend2.AddEntry(errorSumw2, "Same-file Sumw2 error", "lp");
   legend2.AddEntry(errorJackknife, "Jackknife error", "lp");
   legend2.Draw();

   canvas.cd(3);
   gPad->SetTopMargin(0.05);
   styleValue(ratio, kMagenta + 2, 21);
   ratio->SetTitle(Form(";%s;#sigma_{JK} / #sigma_{Sumw2}", ratio->GetXaxis()->GetTitle()));
   ratio->SetMinimum(0);
   ratio->SetMaximum(max(1.2, ratio->GetMaximum() * 1.25));
   ratio->Draw("HIST P");
   TLine line(ratio->GetXaxis()->GetXmin(), 1.0, ratio->GetXaxis()->GetXmax(), 1.0);
   line.SetLineStyle(2);
   line.Draw();

   canvas.SaveAs(pdfPath.c_str());
}

string buildResultFile(const string &base, const string &system, const string &tag)
{
   return base + "/MainAnalysis/20241102_ZhadronVsZPt/plots/" + system + "_trkResidual_" + tag + "_ZPT0_500-result.root";
}

void jackknife_diagnostics(const char *nominalPPTag = "EEV5_ZV9_trkV27_nmix10",
   const char *nominalPPBTag = "ZV9_trkV27_nmix10",
   const char *jackknifePPTag = "EEV5_ZV9_trkV27_nmix10_jackknife",
   const char *jackknifePPBTag = "ZV9_trkV27_nmix10_jackknife",
   const char *outputDirectory = "/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/jackknife_diagnostics")
{
   string base = "/home/kdeverea/PhysicsZHadronEEC";
   string outdir = outputDirectory;
   gSystem->mkdir(outdir.c_str(), true);

   vector<string> systems = {"pp", "pPb", "PbP"};
   vector<string> observables = {"DeltaEta", "DeltaPhi"};

   ofstream report(outdir + "/ZPT0_500_trkPT0.5_15_jackknife_diagnostics.txt");
   report << "jackknife diagnostics for full rerun comparison, ZPT0_500 trkPT0.5_15" << endl << endl;
   report << "nominal pp tag: " << nominalPPTag << endl;
   report << "nominal pPb/PbP tag: " << nominalPPBTag << endl;
   report << "jackknife pp tag: " << jackknifePPTag << endl;
   report << "jackknife pPb/PbP tag: " << jackknifePPBTag << endl << endl;

   for(const string &system : systems)
   {
      string nominalTag = (system == "pp") ? nominalPPTag : nominalPPBTag;
      string jackknifeTag = (system == "pp") ? jackknifePPTag : jackknifePPBTag;
      string nominalFile = buildResultFile(base, system, nominalTag);
      string jackknifeFile = buildResultFile(base, system, jackknifeTag);

      TFile nominal(nominalFile.c_str(), "READ");
      TFile jackknife(jackknifeFile.c_str(), "READ");
      if(nominal.IsZombie() || jackknife.IsZombie())
      {
         cerr << "Unable to open diagnostic inputs for " << system << endl;
         return;
      }

      report << "[" << system << "]" << endl;
      for(const string &observable : observables)
      {
         string histName = observable + "_Result0.5_15";
         string sumw2Name = observable + "_ResultSumw20.5_15";
         TH1D *hNominal = (TH1D *)nominal.Get(histName.c_str());
         TH1D *hJackknife = (TH1D *)jackknife.Get(histName.c_str());
         TH1D *hSumw2 = (TH1D *)jackknife.Get(sumw2Name.c_str());
         if(hNominal == nullptr || hJackknife == nullptr || hSumw2 == nullptr)
         {
            cerr << "Missing histogram for " << system << " " << observable << endl;
            return;
         }

         hNominal = (TH1D *)hNominal->Clone(Form("%s_%s_nominal", system.c_str(), observable.c_str()));
         hJackknife = (TH1D *)hJackknife->Clone(Form("%s_%s_jackknife", system.c_str(), observable.c_str()));
         hSumw2 = (TH1D *)hSumw2->Clone(Form("%s_%s_sumw2", system.c_str(), observable.c_str()));
         hNominal->SetDirectory(nullptr);
         hJackknife->SetDirectory(nullptr);
         hSumw2->SetDirectory(nullptr);

         makeOneDiagnostic(system, observable, hNominal, hJackknife, hSumw2,
            outdir + "/" + system + "_ZPT0_500_trkPT0.5_15_" + observable + "_diagnostic_jackknife.pdf", report);
      }
      report << endl;
   }
}
