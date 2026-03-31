#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TStyle.h"

#include "CommandLine.h"
#include "SetStyle.h"

vector<string> ParseCSV(const string &text)
{
   if(text == "")
      return {};

   vector<string> result = CommandLine::Parse(text);
   vector<string> cleaned;
   for(const string &item : result)
      if(item != "")
         cleaned.push_back(item);
   return cleaned;
}

TH1D *CloneHistogram(TFile &file, const string &name, double scale = 1)
{
   TH1D *histogram = (TH1D *)file.Get(name.c_str());
   if(histogram == nullptr)
      return nullptr;

   histogram = (TH1D *)histogram->Clone();
   histogram->SetDirectory(nullptr);
   histogram->Scale(scale);
   return histogram;
}

pair<string, string> SplitRange(const string &range)
{
   size_t split = range.find('_');
   if(split == string::npos)
      return {range, ""};
   return {range.substr(0, split), range.substr(split + 1)};
}

string GetObservableLabel(const string &observable)
{
   if(observable == "DeltaPhi")
      return "#Delta#phi_{ch,Z}";
   return "#Delta y_{ch,Z}";
}

string GetResultAxisLabel(const string &observable)
{
   return "Result d#LT#DeltaN_{ch}#GT/d" + GetObservableLabel(observable);
}

string GetLegendLabel(const string &family)
{
   if(family == "PUpp" || family == "PUpPb")
      return "PU";
   return family;
}

string FormatPTRange(const string &range, const string &symbol)
{
   pair<string, string> bounds = SplitRange(range);
   if(bounds.second == "" || bounds.second == "500")
      return symbol + " > " + bounds.first + " GeV";
   return bounds.first + " < " + symbol + " < " + bounds.second + " GeV";
}

void DrawLabels(const string &collision, const string &zptRange, const string &trackRange, const string &title)
{
   TLatex latex;
   latex.SetNDC();
   latex.SetTextFont(42);
   latex.SetTextSize(0.032);
   latex.DrawLatex(0.16, 0.84, (collision + " " + title).c_str());
   latex.DrawLatex(0.16, 0.79, FormatPTRange(trackRange, "p_{T}^{ch}").c_str());
   latex.DrawLatex(0.16, 0.74, FormatPTRange(zptRange, "p_{T}^{Z}").c_str());
}

TH1D *BuildRelativeHistogram(const TH1D *histogram, const TH1D *nominal, const string &name)
{
   TH1D *relative = (TH1D *)histogram->Clone(name.c_str());
   relative->SetDirectory(nullptr);
   for(int i = 1; i <= relative->GetNbinsX(); i++)
   {
      double denominator = fabs(nominal->GetBinContent(i));
      double value = 0;
      if(denominator > 0)
         value = histogram->GetBinContent(i) / denominator;
      relative->SetBinContent(i, value);
      relative->SetBinError(i, 0);
   }
   return relative;
}

map<string, int> GetColors()
{
   map<string, int> colors;
   colors["TrackSelection"] = kBlue + 1;
   colors["TrackCorrection"] = kMagenta + 1;
   colors["MuonRejection"] = kGreen + 2;
   colors["PUpp"] = kOrange + 7;
   colors["PUpPb"] = kAzure + 2;
   colors["ScaleFactor"] = kRed + 1;
   colors["Total"] = kBlack;
   return colors;
}

vector<pair<string, TH1D *>> LoadFamilyHistograms(TFile &file, const string &observable, const vector<string> &families)
{
   vector<pair<string, TH1D *>> histograms;
   for(const string &family : families)
   {
      if(family == "Total")
         continue;
      TH1D *histogram = CloneHistogram(file, family + "_" + observable);
      if(histogram == nullptr)
         continue;
      histograms.push_back({family, histogram});
   }

   TH1D *total = CloneHistogram(file, "Total_" + observable);
   if(total != nullptr)
      histograms.push_back({"Total", total});
   return histograms;
}

void DrawUncertaintyOverlay(TFile &systematicsFile, TH1D *nominal, const string &observable,
   const vector<string> &families, const string &collision, const string &zptRange,
   const string &trackRange, const string &outputName, bool doRelative)
{
   map<string, int> colors = GetColors();

   vector<pair<string, TH1D *>> histograms;
   vector<pair<string, TH1D *>> absoluteHistograms = LoadFamilyHistograms(systematicsFile, observable, families);
   for(const auto &entry : absoluteHistograms)
   {
      TH1D *histogram = entry.second;
      if(doRelative == true)
         histogram = BuildRelativeHistogram(entry.second, nominal, entry.first + "_Relative_" + observable);
      histograms.push_back({entry.first, histogram});
   }
   if(histograms.empty())
   {
      for(const auto &entry : absoluteHistograms)
         delete entry.second;
      return;
   }

   double maximum = 0;
   for(const auto &entry : histograms)
      if(entry.first == "Total")
         maximum = max(maximum, entry.second->GetMaximum());
   if(maximum <= 0)
      for(const auto &entry : histograms)
         maximum = max(maximum, entry.second->GetMaximum());
   if(maximum <= 0)
      maximum = 1;
   maximum = maximum * 2;

   string canvasTag = observable + (doRelative == true ? "_Relative" : "_Absolute");
   TH1D *world = (TH1D *)histograms.front().second->Clone(("World_" + canvasTag).c_str());
   world->Reset("ICES");
   world->SetStats(0);
   world->SetMinimum(0);
   world->SetMaximum(maximum);
   world->GetXaxis()->SetTitle(GetObservableLabel(observable).c_str());
   world->GetYaxis()->SetTitle(doRelative == true ? "Relative uncertainty" : "Absolute uncertainty");

   TCanvas canvas(("Canvas_" + canvasTag).c_str(), "", 900, 700);
   world->Draw("axis");

   TLegend legend(0.60, 0.58, 0.85, 0.84);
   legend.SetFillStyle(0);
   legend.SetBorderSize(0);
   legend.SetTextSize(0.028);

   for(auto &entry : histograms)
   {
      entry.second->SetStats(0);
      entry.second->SetLineColor(colors[entry.first]);
      entry.second->SetLineWidth(entry.first == "Total" ? 3 : 2);
      entry.second->Draw("hist same");
      legend.AddEntry(entry.second, GetLegendLabel(entry.first).c_str(), "l");
   }
   legend.Draw();

   DrawLabels(collision, zptRange, trackRange, doRelative == true ? "relative systematic uncertainties" : "systematic uncertainties");

   canvas.SaveAs(outputName.c_str());

   delete world;
   if(doRelative == true)
      for(auto &entry : histograms)
         delete entry.second;
   for(const auto &entry : absoluteHistograms)
      delete entry.second;
}

void DrawCentralValue(TFile &nominalFile, TFile &systematicsFile, const string &observable,
   const string &collision, const string &zptRange, const string &trackRange, const string &outputName)
{
   string histogramName = observable + "_Result" + trackRange;
   TH1D *nominal = CloneHistogram(nominalFile, histogramName, 0.5);
   TH1D *total = CloneHistogram(systematicsFile, "Total_" + observable);
   if(nominal == nullptr || total == nullptr)
   {
      delete nominal;
      delete total;
      return;
   }

   nominal->SetStats(0);
   for(int i = 1; i <= nominal->GetNbinsX(); i++)
      nominal->SetBinError(i, total->GetBinContent(i));

   double minimum = nominal->GetBinContent(1) - nominal->GetBinError(1);
   double maximum = nominal->GetBinContent(1) + nominal->GetBinError(1);
   for(int i = 1; i <= nominal->GetNbinsX(); i++)
   {
      minimum = min(minimum, nominal->GetBinContent(i) - nominal->GetBinError(i));
      maximum = max(maximum, nominal->GetBinContent(i) + nominal->GetBinError(i));
   }
   double range = maximum - minimum;
   if(range <= 0)
      range = max(fabs(maximum), 1.0);
   double margin = range * 0.12;

   TH1D *world = (TH1D *)nominal->Clone(("CentralWorld_" + observable).c_str());
   world->Reset("ICES");
   world->SetStats(0);
   world->SetMinimum(minimum - margin);
   world->SetMaximum(maximum + margin);
   world->GetXaxis()->SetTitle(GetObservableLabel(observable).c_str());
   world->GetYaxis()->SetTitle(GetResultAxisLabel(observable).c_str());

   nominal->SetLineColor(kBlack);
   nominal->SetMarkerColor(kBlack);
   nominal->SetMarkerStyle(20);
   nominal->SetMarkerSize(1.0);
   nominal->SetLineWidth(2);

   TCanvas canvas(("CentralCanvas_" + observable).c_str(), "", 900, 700);
   world->Draw("axis");
   nominal->Draw("ep same");

   TLegend legend(0.57, 0.74, 0.88, 0.88);
   legend.SetFillStyle(0);
   legend.SetBorderSize(0);
   legend.AddEntry(nominal, "Central value #pm syst.", "lep");
   legend.Draw();

   DrawLabels(collision, zptRange, trackRange, "central value");

   canvas.SaveAs(outputName.c_str());

   delete world;
   delete nominal;
   delete total;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   gROOT->SetBatch(kTRUE);
   SetThesisStyle();
   gStyle->SetOptStat(0);

   string inputFileName = CL.Get("Input");
   string nominalFileName = CL.Get("Nominal", "");
   string outputBase = CL.Get("OutputBase", "plots/systematics");
   string collision = CL.Get("Collision", "pPb");
   string zptRange = CL.Get("ZPTRange", "40_350");
   string trackRange = CL.Get("TrackPTRange", "2_500");
   vector<string> families = ParseCSV(CL.Get("Families", "TrackSelection,TrackCorrection,MuonRejection,PUpp,PUpPb,ScaleFactor"));

   TFile systematicsFile(inputFileName.c_str());
   TFile nominalFile(nominalFileName.c_str());

   TH1D *nominalDeltaPhi = CloneHistogram(nominalFile, "DeltaPhi_Result" + trackRange, 0.5);
   TH1D *nominalDeltaEta = CloneHistogram(nominalFile, "DeltaEta_Result" + trackRange, 0.5);
   if(nominalDeltaPhi != nullptr)
   {
      DrawUncertaintyOverlay(systematicsFile, nominalDeltaPhi, "DeltaPhi", families, collision, zptRange, trackRange, outputBase + "-DeltaPhi-absolute.pdf", false);
      DrawUncertaintyOverlay(systematicsFile, nominalDeltaPhi, "DeltaPhi", families, collision, zptRange, trackRange, outputBase + "-DeltaPhi-relative.pdf", true);
   }
   if(nominalDeltaEta != nullptr)
   {
      DrawUncertaintyOverlay(systematicsFile, nominalDeltaEta, "DeltaEta", families, collision, zptRange, trackRange, outputBase + "-DeltaEta-absolute.pdf", false);
      DrawUncertaintyOverlay(systematicsFile, nominalDeltaEta, "DeltaEta", families, collision, zptRange, trackRange, outputBase + "-DeltaEta-relative.pdf", true);
   }

   DrawCentralValue(nominalFile, systematicsFile, "DeltaPhi", collision, zptRange, trackRange, outputBase + "-DeltaPhi-central.pdf");
   DrawCentralValue(nominalFile, systematicsFile, "DeltaEta", collision, zptRange, trackRange, outputBase + "-DeltaEta-central.pdf");

   delete nominalDeltaPhi;
   delete nominalDeltaEta;
   nominalFile.Close();
   systematicsFile.Close();

   return 0;
}
