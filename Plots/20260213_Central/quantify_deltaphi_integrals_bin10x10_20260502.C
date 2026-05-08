#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

namespace {

struct Selection {
   string ZPT;
   string TrackPT;
};

struct ResultProjectionWindow {
   int DeltaPhiXFirst = 0;
   int DeltaPhiXLast = 0;
};

int findLastFullBinAtOrBelow(const TAxis *axis, double boundary)
{
   if (axis == nullptr)
      return 0;

   const double tolerance = std::max(1.0, std::abs(boundary)) * 1e-12;
   int lastBin = 0;
   for (int bin = 1; bin <= axis->GetNbins(); ++bin)
   {
      if (axis->GetBinUpEdge(bin) <= boundary + tolerance)
         lastBin = bin;
      else
         break;
   }
   return lastBin;
}

struct IntegralRow {
   string Label;
   double PPRaw = 0;
   double PPSub = 0;
   double HICombinedRaw = 0;
   double HICombinedSub = 0;
   double Difference = 0;
   double DifferencePercentOfAverageRaw = 0;
};

void divideByWidth(TH1D *histogram)
{
   if (histogram == nullptr)
      return;

   for (int i = 1; i <= histogram->GetNbinsX(); ++i)
   {
      double width = histogram->GetBinWidth(i);
      if (width == 0)
         continue;
      histogram->SetBinContent(i, histogram->GetBinContent(i) / width);
      histogram->SetBinError(i, histogram->GetBinError(i) / width);
   }
}

ResultProjectionWindow getProjectionWindow(const TH2D *histogram)
{
   ResultProjectionWindow window;
   if (histogram == nullptr)
      return window;

   window.DeltaPhiXFirst = 0;
   window.DeltaPhiXLast = findLastFullBinAtOrBelow(histogram->GetXaxis(), 0.0);

   return window;
}

TH1D *loadDetachedTH1(TFile &file, const string &name)
{
   TH1D *histogram = (TH1D *)file.Get(name.c_str());
   if (histogram == nullptr)
      return nullptr;
   histogram = (TH1D *)histogram->Clone((name + "_clone").c_str());
   histogram->SetDirectory(nullptr);
   return histogram;
}

TH2D *loadDetachedTH2(TFile &file, const string &name)
{
   TH2D *histogram = (TH2D *)file.Get(name.c_str());
   if (histogram == nullptr)
      return nullptr;
   histogram = (TH2D *)histogram->Clone((name + "_clone").c_str());
   histogram->SetDirectory(nullptr);
   return histogram;
}

double getDisplayedIntegral(TH1D *histogram)
{
   if (histogram == nullptr)
      return std::numeric_limits<double>::quiet_NaN();
   histogram->Scale(0.5);
   return histogram->Integral("width");
}

TH1D *buildCombinedDeltaPhi(const string &tag, const string &zpt, const string &trkpt, bool subtractBackground)
{
   const string base = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/";
   TFile ppbFile((base + "pPb_trkResidual_" + tag + "_ZPT" + zpt + "-nosub.root").c_str(), "READ");
   TFile pbpFile((base + "PbP_trkResidual_" + tag + "_ZPT" + zpt + "-nosub.root").c_str(), "READ");

   if (ppbFile.IsZombie() || pbpFile.IsZombie())
      return nullptr;

   TH2D *signalPPb = loadDetachedTH2(ppbFile, "hData_" + trkpt);
   TH2D *mixPPb = loadDetachedTH2(ppbFile, "hMixData_" + trkpt);
   TH1D *signalNZPPb = loadDetachedTH1(ppbFile, "hNZData_" + trkpt);
   TH1D *mixNZPPb = loadDetachedTH1(ppbFile, "hNZMixData_" + trkpt);

   TH2D *signalPbP = loadDetachedTH2(pbpFile, "hData_" + trkpt);
   TH2D *mixPbP = loadDetachedTH2(pbpFile, "hMixData_" + trkpt);
   TH1D *signalNZPbP = loadDetachedTH1(pbpFile, "hNZData_" + trkpt);
   TH1D *mixNZPbP = loadDetachedTH1(pbpFile, "hNZMixData_" + trkpt);

   if (signalPPb == nullptr || mixPPb == nullptr || signalNZPPb == nullptr || mixNZPPb == nullptr
      || signalPbP == nullptr || mixPbP == nullptr || signalNZPbP == nullptr || mixNZPbP == nullptr)
   {
      delete signalPPb;
      delete mixPPb;
      delete signalNZPPb;
      delete mixNZPPb;
      delete signalPbP;
      delete mixPbP;
      delete signalNZPbP;
      delete mixNZPbP;
      return nullptr;
   }

   signalPPb->Scale(signalNZPPb->GetBinContent(1));
   mixPPb->Scale(mixNZPPb->GetBinContent(1));
   signalPbP->Scale(signalNZPbP->GetBinContent(1));
   mixPbP->Scale(mixNZPbP->GetBinContent(1));

   TH2D *signalCombined = (TH2D *)signalPPb->Clone(("signalCombined_" + zpt + "_" + trkpt).c_str());
   signalCombined->Add(signalPbP);
   double totalSignalNZ = signalNZPPb->GetBinContent(1) + signalNZPbP->GetBinContent(1);
   signalCombined->Scale(1.0 / totalSignalNZ);

   TH2D *mixCombined = (TH2D *)mixPPb->Clone(("mixCombined_" + zpt + "_" + trkpt).c_str());
   mixCombined->Add(mixPbP);
   double totalMixNZ = mixNZPPb->GetBinContent(1) + mixNZPbP->GetBinContent(1);
   mixCombined->Scale(1.0 / totalMixNZ);

   TH2D *finalHistogram = (TH2D *)signalCombined->Clone(("finalCombined_" + zpt + "_" + trkpt).c_str());
   if (subtractBackground)
      finalHistogram->Add(mixCombined, -1);

   ResultProjectionWindow window = getProjectionWindow(finalHistogram);
   TH1D *projection = (TH1D *)finalHistogram->ProjectionY(
      ("DeltaPhi_" + zpt + "_" + trkpt + (subtractBackground ? "_sub" : "_raw")).c_str(),
      window.DeltaPhiXFirst, window.DeltaPhiXLast);
   projection->SetDirectory(nullptr);
   divideByWidth(projection);

   delete signalPPb;
   delete mixPPb;
   delete signalNZPPb;
   delete mixNZPPb;
   delete signalPbP;
   delete mixPbP;
   delete signalNZPbP;
   delete mixNZPbP;
   delete signalCombined;
   delete mixCombined;
   delete finalHistogram;

   return projection;
}

string latexEscape(const string &input)
{
   string result = input;
   size_t position = 0;
   while ((position = result.find("_", position)) != string::npos)
   {
      result.replace(position, 1, "\\_");
      position += 2;
   }
   return result;
}

} // namespace

void quantify_deltaphi_integrals_bin10x10_20260502(
   const char *ppTag = "EEV5_ZV9_trkV27_nmix10_bin10x10_20260502",
   const char *ppbTag = "ZV9_trkV27_nmix10_bin10x10_20260502",
   const char *texOutput = "/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/presentations/central_combined_bin10x10_20260502_integrals.tex",
   const char *tsvOutput = "/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/presentations/central_combined_bin10x10_20260502_integrals.tsv")
{
   const string base = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/";
   const vector<Selection> selections = {
      {"0_500", "0.5_15"},
      {"0_30", "0.5_2"},
      {"0_30", "2_4"},
      {"0_30", "4_15"},
      {"30_500", "0.5_2"},
      {"30_500", "2_4"},
      {"30_500", "4_15"},
      {"5_350", "0.5_15"},
      {"20_40", "0.5_15"},
      {"5_30", "0.5_2"},
      {"5_30", "2_4"},
      {"5_30", "4_15"},
      {"30_350", "0.5_2"},
      {"30_350", "2_4"},
      {"30_350", "4_15"}
   };

   vector<IntegralRow> rows;
   rows.reserve(selections.size());

   double maxAbsDifferencePercent = -1;
   string maxAbsDifferenceLabel;
   double maxAbsPPResidualPercent = -1;
   string maxAbsPPResidualLabel;
   double maxAbsHIResidualPercent = -1;
   string maxAbsHIResidualLabel;

   for (const Selection &selection : selections)
   {
      const string ppNoSubFileName = base + "pp_trkResidual_" + string(ppTag) + "_ZPT" + selection.ZPT + "-nosub.root";
      const string ppSubFileName = base + "pp_trkResidual_" + string(ppTag) + "_ZPT" + selection.ZPT + "-result.root";

      TFile ppNoSubFile(ppNoSubFileName.c_str(), "READ");
      TFile ppSubFile(ppSubFileName.c_str(), "READ");
      if (ppNoSubFile.IsZombie() || ppSubFile.IsZombie())
      {
         cerr << "Failed to open pp files for ZPT" << selection.ZPT << " trkPT" << selection.TrackPT << endl;
         return;
      }

      TH1D *ppRaw = loadDetachedTH1(ppNoSubFile, "DeltaPhi_Result" + selection.TrackPT);
      TH1D *ppSub = loadDetachedTH1(ppSubFile, "DeltaPhi_Result" + selection.TrackPT);
      TH1D *hiRaw = buildCombinedDeltaPhi(ppbTag, selection.ZPT, selection.TrackPT, false);
      TH1D *hiSub = buildCombinedDeltaPhi(ppbTag, selection.ZPT, selection.TrackPT, true);

      if (ppRaw == nullptr || ppSub == nullptr || hiRaw == nullptr || hiSub == nullptr)
      {
         cerr << "Missing histogram for ZPT" << selection.ZPT << " trkPT" << selection.TrackPT << endl;
         delete ppRaw;
         delete ppSub;
         delete hiRaw;
         delete hiSub;
         return;
      }

      IntegralRow row;
      row.Label = "ZPT" + selection.ZPT + ", trkPT" + selection.TrackPT;
      row.PPRaw = getDisplayedIntegral(ppRaw);
      row.PPSub = getDisplayedIntegral(ppSub);
      row.HICombinedRaw = getDisplayedIntegral(hiRaw);
      row.HICombinedSub = getDisplayedIntegral(hiSub);
      row.Difference = row.HICombinedSub - row.PPSub;

      const double averageRawScale = 0.5 * (fabs(row.PPRaw) + fabs(row.HICombinedRaw));
      if (averageRawScale > 0)
         row.DifferencePercentOfAverageRaw = fabs(row.Difference) / averageRawScale * 100.0;

      const double ppResidualPercent = (fabs(row.PPRaw) > 0) ? fabs(row.PPSub) / fabs(row.PPRaw) * 100.0 : 0.0;
      const double hiResidualPercent = (fabs(row.HICombinedRaw) > 0) ? fabs(row.HICombinedSub) / fabs(row.HICombinedRaw) * 100.0 : 0.0;

      if (row.DifferencePercentOfAverageRaw > maxAbsDifferencePercent)
      {
         maxAbsDifferencePercent = row.DifferencePercentOfAverageRaw;
         maxAbsDifferenceLabel = row.Label;
      }
      if (ppResidualPercent > maxAbsPPResidualPercent)
      {
         maxAbsPPResidualPercent = ppResidualPercent;
         maxAbsPPResidualLabel = row.Label;
      }
      if (hiResidualPercent > maxAbsHIResidualPercent)
      {
         maxAbsHIResidualPercent = hiResidualPercent;
         maxAbsHIResidualLabel = row.Label;
      }

      rows.push_back(row);

      delete ppRaw;
      delete ppSub;
      delete hiRaw;
      delete hiSub;
   }

   ofstream tsv(tsvOutput);
   tsv << "Selection\tPP raw\tPP sub\tpPb raw\tpPb sub\tpPb-pp sub\t|pPb-pp|/<raw> [%]\n";
   tsv << fixed << setprecision(6);
   for (const IntegralRow &row : rows)
      tsv << row.Label << '\t' << row.PPRaw << '\t' << row.PPSub << '\t'
         << row.HICombinedRaw << '\t' << row.HICombinedSub << '\t'
         << row.Difference << '\t' << row.DifferencePercentOfAverageRaw << '\n';
   tsv.close();

   ofstream tex(texOutput);
   tex << "\\begin{frame}[plain]{Delta$\\varphi$ integral audit}\n";
   tex << "\\scriptsize\n";
   tex << "\\centering\n";
   tex << "\\resizebox{\\textwidth}{!}{%\n";
   tex << "\\begin{tabular}{lrrrrrr}\n";
   tex << "\\hline\n";
   tex << "Selection & $I_{\\mathrm{pp}}^{\\mathrm{raw}}$ & $I_{\\mathrm{pp}}^{\\mathrm{sub}}$ & $I_{\\mathrm{pPb}}^{\\mathrm{raw}}$ & $I_{\\mathrm{pPb}}^{\\mathrm{sub}}$ & $I_{\\mathrm{pPb-pp}}$ & $|I_{\\mathrm{pPb-pp}}|/\\langle I_{\\mathrm{raw}}\\rangle$ [\\%]\\\\\n";
   tex << "\\hline\n";
   tex << fixed << setprecision(4);
   for (const IntegralRow &row : rows)
      tex << latexEscape(row.Label) << " & " << row.PPRaw << " & " << row.PPSub
         << " & " << row.HICombinedRaw << " & " << row.HICombinedSub
         << " & " << row.Difference << " & " << row.DifferencePercentOfAverageRaw << "\\\\\n";
   tex << "\\hline\n";
   tex << "\\end{tabular}}\n";
   tex << "\\vspace{0.4em}\n";
   tex << "\\tiny Worst $|I_{\\mathrm{pPb-pp}}|/\\langle I_{\\mathrm{raw}}\\rangle$: "
      << latexEscape(maxAbsDifferenceLabel) << " (" << fixed << setprecision(2) << maxAbsDifferencePercent << "\\%). "
      << "Largest residual fractions: pp " << latexEscape(maxAbsPPResidualLabel) << " (" << maxAbsPPResidualPercent
      << "\\%), pPb " << latexEscape(maxAbsHIResidualLabel) << " (" << maxAbsHIResidualPercent << "\\%).\n";
   tex << "\\end{frame}\n";
   tex.close();
}
