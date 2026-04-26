#include <cmath>
#include <iostream>
using namespace std;

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"

#include "CommandLine.h"
#include "Messenger.h"

int main(int argc, char *argv[])
{
   gROOT->SetBatch(kTRUE);

   CommandLine CL(argc, argv);

   string InputFileName = CL.Get("Input");
   string OutputFileName = CL.Get("Output");
   string TrackPlotFileName = CL.Get("TrackPlot", "");
   string ZPlotFileName = CL.Get("ZPlot", "");
   double EtaMax = CL.GetDouble("EtaMax", 5.0);
   int EtaBin = CL.GetInteger("EtaBin", 50);

   TFile InputFile(InputFileName.c_str());
   ZHadronMessenger M(InputFile, "Tree");

   TH1D HTrackEta("HTrackEta", ";#eta;1/N_{evt} dN_{ch}/d#eta", EtaBin, -EtaMax, EtaMax);
   TH1D HZEta("HZEta", ";#eta_{Z};1/N_{evt} dN_{Z}/d#eta", EtaBin, -EtaMax, EtaMax);
   HTrackEta.Sumw2();
   HZEta.Sumw2();

   double EventWeightSum = 0;
   int EntryCount = M.GetEntries();
   for(int iE = 0; iE < EntryCount; iE++)
   {
      M.GetEntry(iE);

      double Weight = 1;
      if(std::isfinite(M.EventWeight))
         Weight = M.EventWeight;
      EventWeightSum = EventWeightSum + Weight;

      for(int iT = 0; iT < (int)M.trackEta->size(); iT++)
      {
         if(M.trackWeight->at(iT) == 0)
            continue;
         HTrackEta.Fill(M.trackEta->at(iT), Weight * M.trackWeight->at(iT));
      }

      for(int iZ = 0; iZ < (int)M.genZEta->size(); iZ++)
         HZEta.Fill(M.genZEta->at(iZ), Weight);
   }

   if(EventWeightSum != 0)
   {
      HTrackEta.Scale(1 / EventWeightSum, "width");
      HZEta.Scale(1 / EventWeightSum, "width");
   }

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");
   HTrackEta.Write();
   HZEta.Write();
   OutputFile.Close();

   TCanvas CTrack("CTrack", "", 800, 600);
   HTrackEta.SetStats(0);
   HTrackEta.SetLineWidth(2);
   HTrackEta.Draw("hist");
   if(TrackPlotFileName != "")
      CTrack.SaveAs(TrackPlotFileName.c_str());

   TCanvas CZ("CZ", "", 800, 600);
   HZEta.SetStats(0);
   HZEta.SetLineWidth(2);
   HZEta.Draw("hist");
   if(ZPlotFileName != "")
      CZ.SaveAs(ZPlotFileName.c_str());

   cout << "Processed " << EntryCount << " events from " << InputFileName << endl;

   return 0;
}
