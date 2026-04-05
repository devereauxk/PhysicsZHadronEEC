#ifndef PLOT_COMPARISON_UTILITIES_H
#define PLOT_COMPARISON_UTILITIES_H

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLatex.h"
#include "TPad.h"

#include "ResultCombinationUtilities.h"

inline std::pair<std::string, std::string> SplitRange(const std::string &range)
{
   size_t split = range.find('_');
   if(split == std::string::npos)
      return {range, ""};
   return {range.substr(0, split), range.substr(split + 1)};
}

inline std::string FormatPTRange(const std::string &range, const std::string &symbol)
{
   std::pair<std::string, std::string> bounds = SplitRange(range);
   if(bounds.second == "" || bounds.second == "500")
      return symbol + " > " + bounds.first + " GeV";
   return bounds.first + " < " + symbol + " < " + bounds.second + " GeV";
}

inline std::string GetObservableLabel(const std::string &observable)
{
   if(observable == "DeltaPhi")
      return "#Delta#phi_{ch,Z}";
   return "#Delta y_{ch,Z}";
}

inline std::string GetResultAxisLabel(const std::string &observable)
{
   return "d#LT#DeltaN_{ch}#GT/d" + GetObservableLabel(observable);
}

inline std::pair<double, double> GetObservableRange(const std::string &observable)
{
   if(observable == "DeltaPhi")
      return {-1.5707, 4.7123};
   return {-4.0, 4.0};
}

inline std::string GetCollisionEnergy(const std::string &collision)
{
   return "8.16 TeV";
}

inline TH1D *LoadResultHistogram(TFile &file, const std::string &observable,
   const std::string &trackRange, const std::string &name)
{
   return LoadSingleResultHistogram(file, observable, trackRange, name);
}

inline TH1D *BuildDifferenceHistogram(TH1D *variation, TH1D *reference,
   const std::string &name)
{
   if(variation == nullptr || reference == nullptr)
      return nullptr;

   TH1D *histogram = (TH1D *)variation->Clone(name.c_str());
   histogram->SetDirectory(nullptr);
   histogram->Add(reference, -1);
   return histogram;
}

inline std::pair<double, double> GetRatioRange(const std::vector<TH1 *> &histograms, int baseline = 0)
{
   double maxDeviation = 0;

   for(size_t i = 0; i < histograms.size(); i++)
   {
      if((int)i == baseline)
         continue;

      TH1 *histogram = histograms[i];
      TH1 *reference = histograms[baseline];
      for(int bin = 1; bin <= histogram->GetNbinsX(); bin++)
      {
         double denominator = reference->GetBinContent(bin);
         if(denominator == 0)
            continue;
         double ratio = histogram->GetBinContent(bin) / denominator;
         maxDeviation = std::max(maxDeviation, std::fabs(ratio - 1));
      }
   }

   double margin = std::max(0.05, maxDeviation * 1.25);
   return {std::max(0.0, 1 - margin), 1 + margin};
}

inline std::pair<double, double> GetDifferenceRange(const std::vector<TH1 *> &histograms, int baseline = 0)
{
   return {-0.2, 0.2};
}

inline std::pair<double, double> GetComparisonYRange(const std::vector<TH1 *> &histograms,
   const std::string &observable)
{
   if(observable != "DeltaPhi")
      return {-1, -1};

   double minimum = 1e30;
   double maximum = -1e30;

   for(TH1 *histogram : histograms)
   {
      if(histogram == nullptr)
         continue;

      for(int bin = 1; bin <= histogram->GetNbinsX(); bin++)
      {
         minimum = std::min(minimum, histogram->GetBinContent(bin));
         maximum = std::max(maximum, histogram->GetBinContent(bin));
      }
   }

   if(maximum < minimum)
      return {-1, -1};

   double range = maximum - minimum;
   if(range <= 0)
      range = std::max(std::fabs(maximum), 1.0);

   double ymin = minimum - range * 0.10;
   if(minimum >= 0)
      ymin = std::max(0.0, minimum - range * 0.15);
   double ymax = maximum + range * 0.40;

   return {ymin, ymax};
}

inline void DrawKinematicLabels(TPad *pad, const std::string &title,
   const std::string &zptRange, const std::string &trackRange)
{
   pad->cd();

   TLatex latex;
   latex.SetNDC();
   latex.SetTextFont(42);
   latex.SetTextSize(0.032);
   latex.DrawLatex(0.18, 0.84, title.c_str());
   latex.DrawLatex(0.18, 0.79, FormatPTRange(trackRange, "p_{T}^{ch}").c_str());
   latex.DrawLatex(0.18, 0.74, FormatPTRange(zptRange, "p_{T}^{Z}").c_str());
}

#endif
