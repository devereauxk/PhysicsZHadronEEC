#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"

#include "CommandLine.h"
#include "MetColors.h"
#include "SetStyle.h"

// Assign Austria palette colors to each systematic family.
// Total stays kBlack so it always stands out as the envelope.
map<string, Int_t> BuildAustriaColorMap()
{
   vector<Int_t> cols = MetColors("Austria");
   // Austria discrete order: deep red, navy, forest green, yellow, wine, mauve, teal
   map<string, Int_t> m;
   m["TrackSelection"]      = cols[0]; // deep red
   m["TrackCorrection"]     = cols[1]; // navy
   m["MuonRejection"]       = cols[2]; // forest green
   m["PUpp"]                = cols[3]; // yellow
   m["PUpPb"]               = cols[3]; // same slot for PA
   m["ScaleFactor"]         = cols[4]; // wine
   m["EnergyExtrapolation"] = cols[5]; // mauve
   m["Total"]               = kBlack;
   return m;
}

TH1D *CloneHisto(TFile &f, const string &name)
{
   TH1D *h = (TH1D *)f.Get(name.c_str());
   if(!h) return nullptr;
   h = (TH1D *)h->Clone();
   h->SetDirectory(nullptr);
   return h;
}

// Draw one pad: all families + Total for a given observable.
void DrawObservable(TFile &sysFile, const string &observable,
   const vector<string> &families, const map<string, Int_t> &colors,
   bool drawYAxis, bool drawLegend,
   const string &collisionLabel, const string &zptRange, const string &trkRange)
{
   string xLabel = (observable == "DeltaPhi") ? "#Delta#phi_{ch,Z}" : "#Delta y_{ch,Z}";

   // Load histograms in family order, Total last
   vector<pair<string, TH1D *>> histos;
   for(const string &fam : families)
   {
      if(fam == "Total") continue;
      TH1D *h = CloneHisto(sysFile, fam + "_" + observable);
      if(h) histos.push_back({fam, h});
   }
   TH1D *total = CloneHisto(sysFile, "Total_" + observable);
   if(total) histos.push_back({"Total", total});

   if(histos.empty()) return;

   // Y range: 2× the Total maximum
   double ymax = 0;
   for(const auto &entry : histos)
      if(entry.first == "Total")
         ymax = max(ymax, entry.second->GetMaximum());
   if(ymax <= 0)
      for(const auto &entry : histos)
         ymax = max(ymax, entry.second->GetMaximum());
   ymax = max(ymax, 1e-6) * 2.2;

   TH1D *world = (TH1D *)histos.front().second->Clone("world_tmp");
   world->Reset("ICES");
   world->SetStats(0);
   world->SetMinimum(0);
   world->SetMaximum(ymax);
   world->GetXaxis()->SetTitle(xLabel.c_str());
   world->GetXaxis()->SetTitleSize(0.055);
   world->GetXaxis()->SetLabelSize(0.048);
   if(drawYAxis)
   {
      world->GetYaxis()->SetTitle("Absolute uncertainty");
      world->GetYaxis()->SetTitleSize(0.05);
      world->GetYaxis()->SetLabelSize(0.044);
      world->GetYaxis()->SetTitleOffset(1.25);
   }
   else
   {
      world->GetYaxis()->SetLabelSize(0);
      world->GetYaxis()->SetTickLength(0);
   }
   world->Draw("axis");

   for(auto &entry : histos)
   {
      auto it = colors.find(entry.first);
      Int_t col = (it != colors.end()) ? it->second : kBlack;
      entry.second->SetStats(0);
      entry.second->SetLineColor(col);
      entry.second->SetLineWidth(entry.first == "Total" ? 3 : 2);
      entry.second->Draw("hist same");
   }

   if(drawLegend)
   {
      TLegend *leg = new TLegend(0.55, 0.50, 0.88, 0.86);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      leg->SetTextSize(0.040);
      for(auto &entry : histos)
      {
         string label = entry.first;
         if(label == "PUpp" || label == "PUpPb") label = "PU";
         if(label == "EnergyExtrapolation")       label = "Energy extrap.";
         leg->AddEntry(entry.second, label.c_str(), "l");
      }
      leg->Draw();
   }

   // Labels top-left
   TLatex tex;
   tex.SetNDC();
   tex.SetTextFont(42);
   tex.SetTextSize(0.043);
   tex.DrawLatex(0.14, 0.86, (collisionLabel + "  systematic uncertainties").c_str());
   tex.SetTextSize(0.038);

   // Format ranges
   auto fmt = [](const string &r, const string &sym) -> string {
      size_t p = r.find('_');
      if(p == string::npos) return sym + " > " + r + " GeV";
      string lo = r.substr(0, p), hi = r.substr(p + 1);
      if(hi == "500" || hi == "15") return lo + " < " + sym + " < " + hi + " GeV";
      return lo + " < " + sym + " < " + hi + " GeV";
   };
   tex.DrawLatex(0.14, 0.80, fmt(trkRange, "p_{T}^{ch}").c_str());
   tex.DrawLatex(0.14, 0.75, fmt(zptRange, "p_{T}^{Z}").c_str());

   // Intentionally not deleting drawn histos — canvas repaints on SaveAs
}

int main(int argc, char *argv[])
{
   CommandLine cl(argc, argv);
   string inputFile  = cl.Get("Input");
   string outputFile = cl.Get("Output", "plots/metcolors_austria_example.pdf");
   string collision  = cl.Get("Collision", "pp");
   string zptRange   = cl.Get("ZPT",  "0_500");
   string trkRange   = cl.Get("Trk",  "0.5_15");

   string collisionLabel;
   if(collision == "pp")   collisionLabel = "pp (8.16 TeV)";
   else if(collision == "pPb") collisionLabel = "pPb (8.16 TeV)";
   else if(collision == "PbP") collisionLabel = "Pbp (8.16 TeV)";
   else                    collisionLabel = collision;

   vector<string> families = {"TrackSelection", "TrackCorrection", "MuonRejection",
                               "PUpp", "ScaleFactor", "EnergyExtrapolation", "Total"};

   TFile sysFile(inputFile.c_str(), "READ");
   if(sysFile.IsZombie())
   {
      cerr << "Cannot open " << inputFile << endl;
      return 1;
   }

   SetThesisStyle();
   gStyle->SetOptStat(0);

   map<string, Int_t> colors = BuildAustriaColorMap();

   // Two-panel canvas: left DeltaEta, right DeltaPhi
   TCanvas canvas("canvas", "", 1400, 640);
   canvas.Divide(2, 1, 0.005, 0.0);

   canvas.cd(1);
   gPad->SetLeftMargin(0.14);
   gPad->SetRightMargin(0.02);
   gPad->SetBottomMargin(0.13);
   gPad->SetTopMargin(0.05);
   DrawObservable(sysFile, "DeltaEta", families, colors,
      true, false, collisionLabel, zptRange, trkRange);

   canvas.cd(2);
   gPad->SetLeftMargin(0.05);
   gPad->SetRightMargin(0.03);
   gPad->SetBottomMargin(0.13);
   gPad->SetTopMargin(0.05);
   DrawObservable(sysFile, "DeltaPhi", families, colors,
      false, true, collisionLabel, zptRange, trkRange);

   canvas.SaveAs(outputFile.c_str());
   cout << "Saved " << outputFile << endl;

   return 0;
}
