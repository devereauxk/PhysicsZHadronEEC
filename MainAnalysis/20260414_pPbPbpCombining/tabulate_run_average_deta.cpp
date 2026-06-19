#include <TFile.h>
#include <TH1D.h>
#include <TNtuple.h>
#include <TString.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <utility>

using namespace std;

#include "parameter.h"
#include "Messenger.h"
#include "CommandLine.h"
#include "TrackResidualCorrector.h"

bool checkError(const Parameters& par)
{
   if(par.useVZWeight && par.VZWeightFile == "")
   {
      cout << "Error! UseVZWeight=true requires an explicit external VZWeightFile." << endl;
      return true;
   }
   return false;
}

pair<int, int> findClosestMuonTracks(ZHadronMessenger *b, const Parameters &par)
{
   if(!par.TrackMuClosest)
      return {-1, -1};

   int closestTrack = -1;
   int secondClosestTrack = -1;
   float closestDR = -1;
   float secondClosestDR = -1;

   for(unsigned int i = 0; i < b->trackMuDR->size(); i++)
   {
      float trackMuDR = (*b->trackMuDR)[i];
      if(trackMuDR < 0)
         continue;

      if(closestTrack < 0 || trackMuDR < closestDR)
      {
         secondClosestTrack = closestTrack;
         secondClosestDR = closestDR;
         closestTrack = i;
         closestDR = trackMuDR;
      }
      else if(secondClosestTrack < 0 || trackMuDR < secondClosestDR)
      {
         secondClosestTrack = i;
         secondClosestDR = trackMuDR;
      }
   }

   return {closestTrack, secondClosestTrack};
}

bool rejectMuonMatchedTrack(ZHadronMessenger *b, const Parameters &par, int j,
   const pair<int, int> &closestMuonTracks = {-1, -1})
{
   if(par.TrackMuDR >= 0)
      return ((*b->trackMuDR)[j] >= 0 && (*b->trackMuDR)[j] < par.TrackMuDR);
   if(par.TrackMuClosest)
      return (j == closestMuonTracks.first || j == closestMuonTracks.second);
   return (par.isMuTagged && (*b->trackMuTagged)[j]);
}

bool trackSelection(ZHadronMessenger *b, const Parameters &par, int j,
   const pair<int, int> &closestMuonTracks = {-1, -1})
{
   if(rejectMuonMatchedTrack(b, par, j, closestMuonTracks)) return false;
   if((*b->trackPt)[j] > par.MaxTrackPT) return false;
   if((*b->trackPt)[j] < par.MinTrackPT) return false;
   if((!par.includeHole) && (*b->trackWeight)[j] < 0) return false;
   if((*b->trackEta)[j] > 2.4) return false;
   if((*b->trackEta)[j] < -2.4) return false;
   return true;
}

bool eventSelection(ZHadronMessenger *b, const Parameters& par)
{
   if (b->Run < par.MinRun) return 0;
   if (b->Run >= par.MaxRun) return 0;
   if (par.isPUReject && par.isData && b->NVertex != 1) return 0;
   if (par.useVZWindow && fabs(b->VZ) >= 15) return 0;
   if ((par.isGenZ ? b->genZMass->size() : b->zMass->size()) == 0) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0]) < 60) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0]) > 120) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0])) <= par.MinZY) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0])) >= par.MaxZY) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0]) < par.MinZPT) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0]) > par.MaxZPT) return 0;
   return 1;
}

struct RunSummary
{
   double totalWeight = 0;
   double totalWeightSquared = 0;
   double totalWeightEventSquared = 0;
   double weightedAbsDEta = 0;
   double totalWeightTimesWeightedAbsDEtaByEvent = 0;
   double weightedAbsDEtaByEventSquared = 0;
   long long selectedEvents = 0;
   long long eventsWithTracks = 0;
   long long selectedTracks = 0;
};

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   float MinZPT      = CL.GetDouble("MinZPT", 0);
   float MaxZPT      = CL.GetDouble("MaxZPT", 500);
   float MinTrackPT  = CL.GetDouble("MinTrackPT", 0.5);
   float MaxTrackPT  = CL.GetDouble("MaxTrackPT", 15);
   bool  IsData      = CL.GetBool("IsData", true);
   bool  IsPP        = CL.GetBool("IsPP", false);
   string MinRunString = CL.Get("MinRun", "");
   string MaxRunString = CL.Get("MaxRun", "");

   Parameters par(MinZPT, MaxZPT, MinTrackPT, MaxTrackPT);
   par.input            = CL.Get("Input", "");
   par.isGenZ           = CL.GetBool("IsGenZ", false);
   par.isPUReject       = CL.GetBool("IsPUReject", false);
   par.isMuTagged       = CL.GetBool("IsMuTagged", true);
   par.TrackMuDR        = CL.GetDouble("TrackMuDR", -1);
   par.TrackMuClosest   = CL.GetBool("TrackMuClosest", false);
   par.useTrackWeight   = CL.GetBool("UseTrackWeight", true);
   par.useEventWeight   = CL.GetBool("UseEventWeight", true);
   par.useZWeight       = CL.GetBool("UseZWeight", true);
   par.ZWeightFile      = CL.Get("ZWeightFile", "");
   par.useResidualWeight = CL.GetBool("UseResidualWeight", true);
   par.residualWeightFile = CL.Get("ResidualWeightFile", "");
   par.VZWeightFile     = CL.Get("VZWeightFile", "");
   par.useVZWeight      = CL.GetBool("UseVZWeight", true);
   par.useVZWindow      = CL.GetBool("UseVZWindow", true);
   par.TrackSelectionMode = CL.Get("TrackSelectionMode", "Nominal");
   par.TrackTreeName    = "Tree";
   if(par.TrackSelectionMode == "Loose") par.TrackTreeName = "TreeLoose";
   if(par.TrackSelectionMode == "Tight") par.TrackTreeName = "TreeTight";
   par.MinZY            = CL.GetDouble("MinZY", 0);
   par.MaxZY            = CL.GetDouble("MaxZY", 200);
   par.MinRun           = (MinRunString == "") ? numeric_limits<long long>::min() : stoll(MinRunString);
   par.MaxRun           = (MaxRunString == "") ? numeric_limits<long long>::max() : stoll(MaxRunString);
   par.useZScaleFactor  = CL.GetBool("UseZScaleFactor", true);
   par.includeHole      = true;
   par.isPPb            = CL.GetBool("IsPPb", false);
   par.yBoost           = CL.GetDouble("yBoost", 0);
   par.isPP             = IsPP;
   par.isData           = IsData;
   par.isJewel          = false;

   if(checkError(par)) return -1;

   const string output = CL.Get("Output", "output/run_average_abs_deta_no_zcorr.tsv");
   const string label = CL.Get("Label", par.isPPb ? "pPb" : "PbP");
   const bool append = CL.GetBool("Append", false);

   TFile inputFile(par.input.c_str());
   ZHadronMessenger M(&inputFile, par.TrackTreeName);

   TrackResidualCorrector *Zcorrector = nullptr;
   if(par.useZWeight && par.ZWeightFile != "")
      Zcorrector = new TrackResidualCorrector(par.ZWeightFile.c_str());

   VZCorrector *vzCorrector = nullptr;
   if(par.useVZWeight)
      vzCorrector = new VZCorrector(par.VZWeightFile.c_str());

   TrackResidualCorrector *corrector_0_10 = nullptr, *corrector_10_20 = nullptr,
                          *corrector_20_40 = nullptr, *corrector_40_500 = nullptr;
   if(par.useResidualWeight && par.residualWeightFile != "")
   {
      corrector_0_10   = new TrackResidualCorrector(Form("%s0-10.root",   par.residualWeightFile.c_str()));
      corrector_10_20  = new TrackResidualCorrector(Form("%s10-20.root",  par.residualWeightFile.c_str()));
      corrector_20_40  = new TrackResidualCorrector(Form("%s20-40.root",  par.residualWeightFile.c_str()));
      corrector_40_500 = new TrackResidualCorrector(Form("%s40-500.root", par.residualWeightFile.c_str()));
   }

   map<int, RunSummary> summaries;

   for(int iEntry = 0; iEntry < M.GetEntries(); iEntry++)
   {
      M.GetEntry(iEntry);
      if(!eventSelection(&M, par))
         continue;

      float zPt = (par.isGenZ ? (*M.genZPt)[0] : (*M.zPt)[0]);
      float zY  = (par.isGenZ ? (*M.genZY)[0] : (*M.zY)[0]);
      float zPhi = (par.isGenZ ? (*M.genZPhi)[0] : (*M.zPhi)[0]);
      if(zPhi < 0) zPhi += 2 * M_PI;

      float ZWeight = (Zcorrector != nullptr) ? Zcorrector->GetCorrectionFactor(zPt, zY, zPhi) : 1;
      if(par.useZScaleFactor) ZWeight *= M.ZWeight;

      float eventWeight = 1;
      if(par.useEventWeight) eventWeight *= M.EventWeight;
      if(par.useVZWeight) eventWeight *= vzCorrector->GetCorrectionFactor(M.VZ);
      if(par.useZWeight) eventWeight *= ZWeight;

      TrackResidualCorrector *corrector = nullptr;
      if(par.useResidualWeight && par.residualWeightFile != "")
      {
         if(zPt < 10) corrector = corrector_0_10;
         else if(zPt < 20) corrector = corrector_10_20;
         else if(zPt < 40) corrector = corrector_20_40;
         else corrector = corrector_40_500;
      }

      pair<int, int> closestMuonTracks = findClosestMuonTracks(&M, par);
      RunSummary &summary = summaries[M.Run];
      summary.selectedEvents++;

      double eventWeightSum = 0;
      double eventWeightedAbsDEtaSum = 0;

      for(unsigned long j = 0; j < M.trackPt->size(); j++)
      {
         if(!trackSelection(&M, par, j, closestMuonTracks))
            continue;

         float weight = eventWeight;
         if(par.useTrackWeight) weight *= (*M.trackWeight)[j];
         if(par.useResidualWeight && corrector != nullptr)
            weight *= corrector->GetCorrectionFactor((*M.trackPt)[j], (*M.trackEta)[j], (*M.trackPhi)[j]);

          float dEta = fabs((*M.trackEta)[j] - (*M.zEta)[0]);
          eventWeightSum += weight;
          eventWeightedAbsDEtaSum += weight * dEta;
          summary.totalWeight += weight;
          summary.totalWeightSquared += weight * weight;
          summary.weightedAbsDEta += weight * dEta;
          summary.selectedTracks++;
       }

      if(eventWeightSum > 0)
      {
         summary.eventsWithTracks++;
         summary.totalWeightEventSquared += eventWeightSum * eventWeightSum;
         summary.totalWeightTimesWeightedAbsDEtaByEvent += eventWeightSum * eventWeightedAbsDEtaSum;
         summary.weightedAbsDEtaByEventSquared += eventWeightedAbsDEtaSum * eventWeightedAbsDEtaSum;
      }
    }

   ofstream out;
   if(append)
      out.open(output, ios::app);
   else
      out.open(output);

   if(!append)
      out << "system\trun\taverage_abs_deta\ttotal_track_weight\tselected_tracks\tselected_events\taverage_abs_deta_stat_uncertainty\teffective_tracks\tevents_with_tracks\n";

   out << fixed << setprecision(8);
    for(const auto &item : summaries)
    {
        double average = (item.second.totalWeight != 0) ? item.second.weightedAbsDEta / item.second.totalWeight : 0;
        double effectiveTracks = (item.second.totalWeightSquared > 0)
           ? item.second.totalWeight * item.second.totalWeight / item.second.totalWeightSquared
           : 0;
         double centeredWeightSquaredSum = item.second.weightedAbsDEtaByEventSquared
            - 2 * average * item.second.totalWeightTimesWeightedAbsDEtaByEvent
            + average * average * item.second.totalWeightEventSquared;
         if(centeredWeightSquaredSum < 0)
            centeredWeightSquaredSum = 0;
         double statUncertainty = (item.second.totalWeight > 0 && item.second.eventsWithTracks >= 2)
            ? sqrt(centeredWeightSquaredSum) / item.second.totalWeight
            : 0;
         out << label << "\t" << item.first << "\t" << average << "\t"
             << item.second.totalWeight << "\t" << item.second.selectedTracks << "\t"
            << item.second.selectedEvents << "\t" << statUncertainty << "\t"
            << effectiveTracks << "\t" << item.second.eventsWithTracks << "\n";
     }

   return 0;
}
