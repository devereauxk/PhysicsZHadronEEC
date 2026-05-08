#ifndef RESULT_COMBINATION_UTILITIES_H
#define RESULT_COMBINATION_UTILITIES_H

#include <cmath>
#include <string>

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

inline std::string GetCollisionLabel(const std::string &collision)
{
   if(collision == "pPbPbp")
      return "pPb";
   return collision;
}

inline void NormalizeByBinWidth(TH1D *histogram)
{
   if(histogram == nullptr)
      return;

   for(int bin = 1; bin <= histogram->GetNbinsX(); bin++)
   {
      double width = histogram->GetBinWidth(bin);
      if(width == 0)
         continue;

      histogram->SetBinContent(bin, histogram->GetBinContent(bin) / width);
      histogram->SetBinError(bin, histogram->GetBinError(bin) / width);
   }
}

inline TH1D *LoadCloned1D(TFile &file, const std::string &name, const std::string &cloneName)
{
   TH1D *histogram = (TH1D *)file.Get(name.c_str());
   if(histogram == nullptr)
      return nullptr;

   histogram = (TH1D *)histogram->Clone(cloneName.c_str());
   histogram->SetDirectory(nullptr);
   return histogram;
}

inline TH2D *LoadCloned2D(TFile &file, const std::string &name, const std::string &cloneName)
{
   TH2D *histogram = (TH2D *)file.Get(name.c_str());
   if(histogram == nullptr)
      return nullptr;

   histogram = (TH2D *)histogram->Clone(cloneName.c_str());
   histogram->SetDirectory(nullptr);
   return histogram;
}

inline TH1D *LoadSingleResultHistogram(TFile &file, const std::string &observable,
   const std::string &trackRange, const std::string &name)
{
   constexpr double ResultNormalization = 0.5;

   TH1D *histogram = (TH1D *)file.Get((observable + "_Result" + trackRange).c_str());
   if(histogram == nullptr)
      return nullptr;

   histogram = (TH1D *)histogram->Clone(name.c_str());
   histogram->SetDirectory(nullptr);
   histogram->Scale(ResultNormalization);
   return histogram;
}

inline bool AxisHasExactEdge(const TAxis *axis, double boundary)
{
   if(axis == nullptr)
      return false;

   double tolerance = std::max(1.0, std::abs(boundary)) * 1e-9;
   for(int i = 0; i <= axis->GetNbins(); i++)
   {
      double edge = axis->GetBinLowEdge(i + 1);
      if(std::abs(edge - boundary) < tolerance)
         return true;
   }
   return false;
}

inline bool ValidateShifted10x10Histogram(TH2D *histogram)
{
   if(histogram == nullptr)
      return false;
   if(histogram->GetNbinsX() != 10 || histogram->GetNbinsY() != 10)
      return false;
   if(AxisHasExactEdge(histogram->GetXaxis(), 0.0) == false)
      return false;
   if(AxisHasExactEdge(histogram->GetXaxis(), 4.0) == false)
      return false;
   if(AxisHasExactEdge(histogram->GetYaxis(), 0.0) == false)
      return false;
   if(AxisHasExactEdge(histogram->GetYaxis(), M_PI) == false)
      return false;
   return true;
}

inline bool ValidateModified12x12Histogram(TH2D *histogram)
{
   if(histogram == nullptr)
      return false;
   if(histogram->GetNbinsX() != 12 || histogram->GetNbinsY() != 12)
      return false;
   if(AxisHasExactEdge(histogram->GetXaxis(), 0.0) == false)
      return false;
   if(AxisHasExactEdge(histogram->GetXaxis(), 4.0) == false)
      return false;
   if(AxisHasExactEdge(histogram->GetYaxis(), 0.0) == false)
      return false;
   if(AxisHasExactEdge(histogram->GetYaxis(), M_PI / 2) == false)
      return false;
   if(AxisHasExactEdge(histogram->GetYaxis(), M_PI) == false)
      return false;
   return true;
}

inline TH1D *ProjectOfficial20x20ResultObservable(TH2D *histogram, const std::string &observable,
   const std::string &name)
{
   if(histogram == nullptr)
      return nullptr;

   TH1D *result = nullptr;
   if(observable == "DeltaPhi")
      result = (TH1D *)histogram->ProjectionY(name.c_str(), 0, 10);
   else
      result = (TH1D *)histogram->ProjectionX(name.c_str(), 6, 10);

   result->SetDirectory(nullptr);
   NormalizeByBinWidth(result);
   result->Scale(0.5);
   return result;
}

inline TH1D *ProjectShifted10x10ResultObservable(TH2D *histogram, const std::string &observable,
   const std::string &name)
{
   if(histogram == nullptr)
      return nullptr;
   if(ValidateShifted10x10Histogram(histogram) == false)
      return nullptr;

   TH1D *result = nullptr;
   if(observable == "DeltaPhi")
      result = (TH1D *)histogram->ProjectionY(name.c_str(), 6, 10);
   else
      result = (TH1D *)histogram->ProjectionX(name.c_str(), 4, 8);

   result->SetDirectory(nullptr);
   NormalizeByBinWidth(result);
   result->Scale(0.5);
   return result;
}

inline TH1D *ProjectModified12x12ResultObservable(TH2D *histogram, const std::string &observable,
   const std::string &name)
{
   if(histogram == nullptr)
      return nullptr;
   if(ValidateModified12x12Histogram(histogram) == false)
      return nullptr;

   TH1D *result = nullptr;
   if(observable == "DeltaPhi")
      result = (TH1D *)histogram->ProjectionY(name.c_str(), 7, 12);
   else
      result = (TH1D *)histogram->ProjectionX(name.c_str(), 4, 6);

   result->SetDirectory(nullptr);
   NormalizeByBinWidth(result);
   result->Scale(0.5);
   return result;
}

inline TH1D *BuildCombinedResultHistogram(TFile &pPbFile, TFile &pbpFile,
   const std::string &observable, const std::string &trackRange, const std::string &name,
   bool useShifted10x10 = false, bool useModified12x12 = false)
{
   TH2D *signalPPb = LoadCloned2D(pPbFile, "hData_" + trackRange, name + "_SignalPPb");
   TH2D *signalPbp = LoadCloned2D(pbpFile, "hData_" + trackRange, name + "_SignalPbp");
   TH2D *backgroundPPb = LoadCloned2D(pPbFile, "hMixData_" + trackRange, name + "_BackgroundPPb");
   TH2D *backgroundPbp = LoadCloned2D(pbpFile, "hMixData_" + trackRange, name + "_BackgroundPbp");
   TH1D *signalNZPPb = LoadCloned1D(pPbFile, "hNZData_" + trackRange, name + "_SignalNZPPb");
   TH1D *signalNZPbp = LoadCloned1D(pbpFile, "hNZData_" + trackRange, name + "_SignalNZPbp");
   TH1D *backgroundNZPPb = LoadCloned1D(pPbFile, "hNZMixData_" + trackRange, name + "_BackgroundNZPPb");
   TH1D *backgroundNZPbp = LoadCloned1D(pbpFile, "hNZMixData_" + trackRange, name + "_BackgroundNZPbp");

   if(signalPPb == nullptr || signalPbp == nullptr
      || backgroundPPb == nullptr || backgroundPbp == nullptr
      || signalNZPPb == nullptr || signalNZPbp == nullptr
      || backgroundNZPPb == nullptr || backgroundNZPbp == nullptr)
   {
      delete signalPPb;
      delete signalPbp;
      delete backgroundPPb;
      delete backgroundPbp;
      delete signalNZPPb;
      delete signalNZPbp;
      delete backgroundNZPPb;
      delete backgroundNZPbp;
      return nullptr;
   }

   signalPPb->Scale(signalNZPPb->GetBinContent(1));
   signalPbp->Scale(signalNZPbp->GetBinContent(1));
   signalPPb->Add(signalPbp);
   double totalSignalNZ = signalNZPPb->GetBinContent(1) + signalNZPbp->GetBinContent(1);
   if(totalSignalNZ != 0)
      signalPPb->Scale(1.0 / totalSignalNZ);

   backgroundPPb->Scale(backgroundNZPPb->GetBinContent(1));
   backgroundPbp->Scale(backgroundNZPbp->GetBinContent(1));
   backgroundPPb->Add(backgroundPbp);
   double totalBackgroundNZ = backgroundNZPPb->GetBinContent(1) + backgroundNZPbp->GetBinContent(1);
   if(totalBackgroundNZ != 0)
      backgroundPPb->Scale(1.0 / totalBackgroundNZ);

   signalPPb->Add(backgroundPPb, -1);
   TH1D *result = nullptr;
   if(useModified12x12 == true)
      result = ProjectModified12x12ResultObservable(signalPPb, observable, name);
   else if(useShifted10x10 == true)
      result = ProjectShifted10x10ResultObservable(signalPPb, observable, name);
   else
      result = ProjectOfficial20x20ResultObservable(signalPPb, observable, name);

   delete signalPPb;
   delete signalPbp;
   delete backgroundPPb;
   delete backgroundPbp;
   delete signalNZPPb;
   delete signalNZPbp;
   delete backgroundNZPPb;
   delete backgroundNZPbp;

   return result;
}

#endif
