#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "TCanvas.h"
#include "TEllipse.h"
#include "TFile.h"
#include "TH2D.h"
#include "TStyle.h"

#include "CommandLine.h"
#include "Messenger.h"

namespace
{
   constexpr double MuonMass = 0.1056583755;
   constexpr double MuonVeto = 0.0025;
   constexpr double CentralBox = 0.0010;

   struct FourVector
   {
      double PX = 0;
      double PY = 0;
      double PZ = 0;
      double E = 0;
   };

   struct MuonPair
   {
      bool Valid = false;
      double Eta1 = 0;
      double Phi1 = 0;
      double Pt1 = 0;
      double Eta2 = 0;
      double Phi2 = 0;
      double Pt2 = 0;
   };

   struct ScanSummary
   {
      long long ConsideredTracks = 0;
      long long UsedTracks = 0;
      long long TaggedTracks = 0;
      long long UntaggedTracks = 0;
      long long BoxAll = 0;
      long long BoxTagged = 0;
      long long BoxUntagged = 0;
      long long BoxOutsideR001All = 0;
      long long BoxOutsideR001Tagged = 0;
      long long BoxOutsideR001Untagged = 0;
      long long VetoCircleAll = 0;
      long long VetoCircleTagged = 0;
      long long VetoCircleUntagged = 0;
   };

   struct RawForestSummary
   {
      long long ScannedEvents = 0;
      long long ProcessedEvents = 0;
      long long EventsPassingTriggerAND = 0;
      long long EventsFailingTriggerAND = 0;
      long long EventsWithTracks = 0;
      long long EventsNGlbEQ2 = 0;
      long long EventsNDiEQ1 = 0;
      long long EventsNGlbEQ2NDiEQ1 = 0;
      long long TotalNGlbMuons = 0;
      long long TotalDimuonLegMuons = 0;
      long long TotalRawMuons = 0;
      long long TotalRawTracks = 0;
      long long AllPairs = 0;
      long long ClosestPairs = 0;
      long long ExcludingClosestPairs = 0;
      long long UniqueClosestTracksExcluded = 0;
      long long MuonsWithClosestTrack = 0;
      long long MuonsWithoutTrack = 0;
      long long CentralBoxAllPairs = 0;
      long long CentralBoxClosestPairs = 0;
      long long CentralBoxExcludingClosestPairs = 0;
      long long ClosestExactCoordinateMatch = 0;
      long long ClosestAbsWindow1E6 = 0;
      long long ClosestAbsWindow1E5 = 0;
      long long ClosestAbsWindow1E4 = 0;
      long long ClosestAbsWindow1E3 = 0;
      long long ClosestDRWindow1E6 = 0;
      long long ClosestDRWindow1E5 = 0;
      long long ClosestDRWindow1E4 = 0;
      long long ClosestDRWindow1E3 = 0;
      array<long long, 6> NGlbMultiplicity = {};
      array<long long, 6> NDiMultiplicity = {};
   };

   struct RawMuon
   {
      double Eta = 0;
      double Phi = 0;
      double Pt = 0;
   };

   double WrapDeltaPhi(double Phi1, double Phi2)
   {
      double Delta = Phi1 - Phi2;
      while(Delta < -M_PI)   Delta = Delta + 2 * M_PI;
      while(Delta > +M_PI)   Delta = Delta - 2 * M_PI;
      return Delta;
   }

   FourVector MakeFourVector(double PT, double Eta, double Phi, double Mass)
   {
      FourVector Result;
      Result.PX = PT * cos(Phi);
      Result.PY = PT * sin(Phi);
      Result.PZ = PT * sinh(Eta);
      Result.E = sqrt(Result.PX * Result.PX + Result.PY * Result.PY
         + Result.PZ * Result.PZ + Mass * Mass);
      return Result;
   }

   bool PassPairAcceptance(double PT1, double Eta1, double Phi1,
      double PT2, double Eta2, double Phi2)
   {
      FourVector V1 = MakeFourVector(PT1, Eta1, Phi1, MuonMass);
      FourVector V2 = MakeFourVector(PT2, Eta2, Phi2, MuonMass);

      double PX = V1.PX + V2.PX;
      double PY = V1.PY + V2.PY;
      double PZ = V1.PZ + V2.PZ;
      double E = V1.E + V2.E;

      double Mass2 = E * E - PX * PX - PY * PY - PZ * PZ;
      double Mass = (Mass2 > 0) ? sqrt(Mass2) : 0;
      if(Mass < 60 || Mass > 120)
         return false;

      if(E <= fabs(PZ))
         return false;
      double Rapidity = 0.5 * log((E + PZ) / (E - PZ));
      if(fabs(Rapidity) > 2.4)
         return false;

      return true;
   }

   MuonPair FindRecoMuonPair(MuTreeMessenger &MMu)
   {
      MuonPair Result;

      for(int i = 0; i < MMu.NDi; i++)
      {
         if(MMu.DiCharge1[i] == MMu.DiCharge2[i])        continue;
         if(fabs(MMu.DiEta1[i]) > 2.4)                   continue;
         if(fabs(MMu.DiEta2[i]) > 2.4)                   continue;
         if(fabs(MMu.DiPT1[i]) < 20)                     continue;
         if(fabs(MMu.DiPT2[i]) < 20)                     continue;
         if(MMu.DimuonPassTightCut(i) == false)          continue;
         if(MMu.DiMass[i] < 60 || MMu.DiMass[i] > 120)   continue;

         if(PassPairAcceptance(MMu.DiPT1[i], MMu.DiEta1[i], MMu.DiPhi1[i],
            MMu.DiPT2[i], MMu.DiEta2[i], MMu.DiPhi2[i]) == false)
            continue;

         Result.Valid = true;
         Result.Eta1 = MMu.DiEta1[i];
         Result.Phi1 = MMu.DiPhi1[i];
         Result.Pt1 = MMu.DiPT1[i];
         Result.Eta2 = MMu.DiEta2[i];
         Result.Phi2 = MMu.DiPhi2[i];
         Result.Pt2 = MMu.DiPT2[i];
         return Result;
      }

      return Result;
   }

   MuonPair FindGenMuonPair(MuTreeMessenger &MMu)
   {
      MuonPair Result;

      for(int i = 0; i < MMu.NGen; i++)
      {
         if(MMu.GenMom[i] != 23)              continue;
         if(MMu.GenPT[i] < 20)                continue;
         if(fabs(MMu.GenEta[i]) > 2.4)        continue;

         for(int j = i + 1; j < MMu.NGen; j++)
         {
            if(MMu.GenMom[j] != 23)              continue;
            if(MMu.GenPT[j] < 20)                continue;
            if(fabs(MMu.GenEta[j]) > 2.4)        continue;
            if(PassPairAcceptance(MMu.GenPT[i], MMu.GenEta[i], MMu.GenPhi[i],
               MMu.GenPT[j], MMu.GenEta[j], MMu.GenPhi[j]) == false)
               continue;

            Result.Valid = true;
            Result.Eta1 = MMu.GenEta[i];
            Result.Phi1 = MMu.GenPhi[i];
            Result.Pt1 = MMu.GenPT[i];
            Result.Eta2 = MMu.GenEta[j];
            Result.Phi2 = MMu.GenPhi[j];
            Result.Pt2 = MMu.GenPT[j];
            return Result;
         }
      }

      return Result;
   }

   MuonPair FindSkimRecoMuonPair(ZHadronMessenger &M)
   {
      MuonPair Result;
      if(M.muEta1->size() == 0 || M.muEta2->size() == 0)
         return Result;

      Result.Valid = true;
      Result.Eta1 = M.muEta1->at(0);
      Result.Phi1 = M.muPhi1->at(0);
      Result.Pt1 = M.muPt1->at(0);
      Result.Eta2 = M.muEta2->at(0);
      Result.Phi2 = M.muPhi2->at(0);
      Result.Pt2 = M.muPt2->at(0);
      return Result;
   }

   MuonPair FindSkimGenMuonPair(ZHadronMessenger &M)
   {
      MuonPair Result;
      if(M.genMuEta1->size() == 0 || M.genMuEta2->size() == 0)
         return Result;

      Result.Valid = true;
      Result.Eta1 = M.genMuEta1->at(0);
      Result.Phi1 = M.genMuPhi1->at(0);
      Result.Pt1 = M.genMuPt1->at(0);
      Result.Eta2 = M.genMuEta2->at(0);
      Result.Phi2 = M.genMuPhi2->at(0);
      Result.Pt2 = M.genMuPt2->at(0);
      return Result;
   }

   void RecordTrack(ScanSummary &Summary, TH2D &HAll, TH2D &HTagged, TH2D &HUntagged,
      double TrackEta, double TrackPhi, const MuonPair &Pair, bool Tagged)
   {
      double DeltaEta1 = TrackEta - Pair.Eta1;
      double DeltaPhi1 = WrapDeltaPhi(TrackPhi, Pair.Phi1);
      double DeltaR1 = sqrt(DeltaEta1 * DeltaEta1 + DeltaPhi1 * DeltaPhi1);

      double DeltaEta2 = TrackEta - Pair.Eta2;
      double DeltaPhi2 = WrapDeltaPhi(TrackPhi, Pair.Phi2);
      double DeltaR2 = sqrt(DeltaEta2 * DeltaEta2 + DeltaPhi2 * DeltaPhi2);

      double DeltaEta = DeltaEta1;
      double DeltaPhi = DeltaPhi1;
      double DeltaR = DeltaR1;

      if(DeltaR2 < DeltaR1)
      {
         DeltaEta = DeltaEta2;
         DeltaPhi = DeltaPhi2;
         DeltaR = DeltaR2;
      }

      Summary.UsedTracks++;
      if(Tagged == true)   Summary.TaggedTracks++;
      else                 Summary.UntaggedTracks++;

      HAll.Fill(DeltaEta, DeltaPhi);
      if(Tagged == true)   HTagged.Fill(DeltaEta, DeltaPhi);
      else                 HUntagged.Fill(DeltaEta, DeltaPhi);

      bool InCentralBox = (fabs(DeltaEta) <= CentralBox && fabs(DeltaPhi) <= CentralBox);
      bool OutsideR001 = (DeltaR > CentralBox);
      bool InVetoCircle = (DeltaR < MuonVeto);

      if(InCentralBox == true)
      {
         Summary.BoxAll++;
         if(Tagged == true)   Summary.BoxTagged++;
         else                 Summary.BoxUntagged++;
      }
      if(InCentralBox == true && OutsideR001 == true)
      {
         Summary.BoxOutsideR001All++;
         if(Tagged == true)   Summary.BoxOutsideR001Tagged++;
         else                 Summary.BoxOutsideR001Untagged++;
      }
      if(InVetoCircle == true)
      {
         Summary.VetoCircleAll++;
         if(Tagged == true)   Summary.VetoCircleTagged++;
         else                 Summary.VetoCircleUntagged++;
      }
   }

   void DrawCircle(double Radius)
   {
      TEllipse Circle(0, 0, Radius, Radius);
      Circle.SetLineColor(kBlack);
      Circle.SetLineWidth(2);
      Circle.SetFillStyle(0);
      Circle.DrawClone();
   }

   void SavePlots(const string &Output, const string &Title, TH2D &HAll, TH2D &HTagged, TH2D &HUntagged)
   {
      TFile File((Output + ".root").c_str(), "RECREATE");
      HAll.Write();
      HTagged.Write();
      HUntagged.Write();
      File.Close();

      gStyle->SetOptStat(0);
      gStyle->SetPalette(kBird);

      TCanvas Canvas("Canvas", "Canvas", 1800, 600);
      Canvas.Divide(3, 1);

      vector<pair<TH2D *, string>> Histograms = {
         {&HAll, "all"},
         {&HTagged, "tagged"},
         {&HUntagged, "untagged"}
      };

      for(int i = 0; i < 3; i++)
      {
         Canvas.cd(i + 1);
         gPad->SetLeftMargin(0.13);
         gPad->SetRightMargin(0.14);
         gPad->SetBottomMargin(0.12);
         gPad->SetLogz();

         TH2D *H = Histograms[i].first;
         string PanelTitle = Title + " (" + Histograms[i].second + ")";
         H->SetTitle(PanelTitle.c_str());
         H->GetXaxis()->SetTitle("#Delta#eta_{nearest #mu,ch}");
         H->GetYaxis()->SetTitle("#Delta#phi_{nearest #mu,ch}");
         H->GetXaxis()->SetRangeUser(-0.01, 0.01);
         H->GetYaxis()->SetRangeUser(-0.01, 0.01);
         H->Draw("colz");
         DrawCircle(0.001);
         DrawCircle(MuonVeto);
      }

      Canvas.SaveAs((Output + ".pdf").c_str());
   }

   void DrawRawHistogram(TH2D &H, const string &Title)
   {
      gPad->SetLeftMargin(0.13);
      gPad->SetRightMargin(0.14);
      gPad->SetBottomMargin(0.12);
      gPad->SetLogz();

      H.SetTitle(Title.c_str());
      H.GetXaxis()->SetTitle("#Delta#eta_{#mu,ch}");
      H.GetYaxis()->SetTitle("#Delta#phi_{#mu,ch}");
      H.GetXaxis()->SetRangeUser(-0.01, 0.01);
      H.GetYaxis()->SetRangeUser(-0.01, 0.01);
      H.Draw("colz");
      DrawCircle(0.001);
      DrawCircle(MuonVeto);
   }

   void SaveRawForestPlots(const string &Output, const string &Title,
      TH2D &HAllPairs, TH2D &HClosestOnly, TH2D &HExcludingClosest)
   {
      TFile File((Output + ".root").c_str(), "RECREATE");
      HAllPairs.Write();
      HClosestOnly.Write();
      HExcludingClosest.Write();
      File.Close();

      gStyle->SetOptStat(0);
      gStyle->SetPalette(kBird);

      TCanvas Canvas("CanvasRaw", "CanvasRaw", 1800, 600);
      Canvas.Divide(3, 1);

      Canvas.cd(1);
      DrawRawHistogram(HAllPairs, Title + " (all muon-track pairs)");
      Canvas.cd(2);
      DrawRawHistogram(HClosestOnly, Title + " (closest track to each muon)");
      Canvas.cd(3);
      DrawRawHistogram(HExcludingClosest, Title + " (all pairs excluding closest tracks)");

      Canvas.SaveAs((Output + ".pdf").c_str());
   }

   bool InCentralBox(double DeltaEta, double DeltaPhi)
   {
      return (fabs(DeltaEta) <= CentralBox && fabs(DeltaPhi) <= CentralBox);
   }

   void CountMultiplicity(array<long long, 6> &Count, int Value)
   {
      if(Value < 0)   return;
      if(Value < 5)   Count[Value]++;
      else            Count[5]++;
   }

   void RecordClosestWindows(RawForestSummary &Summary, double DeltaEta, double DeltaPhi, double DeltaR)
   {
      if(DeltaEta == 0 && DeltaPhi == 0)
         Summary.ClosestExactCoordinateMatch++;

      if(fabs(DeltaEta) <= 1e-6 && fabs(DeltaPhi) <= 1e-6)   Summary.ClosestAbsWindow1E6++;
      if(fabs(DeltaEta) <= 1e-5 && fabs(DeltaPhi) <= 1e-5)   Summary.ClosestAbsWindow1E5++;
      if(fabs(DeltaEta) <= 1e-4 && fabs(DeltaPhi) <= 1e-4)   Summary.ClosestAbsWindow1E4++;
      if(fabs(DeltaEta) <= 1e-3 && fabs(DeltaPhi) <= 1e-3)   Summary.ClosestAbsWindow1E3++;

      if(DeltaR <= 1e-6)   Summary.ClosestDRWindow1E6++;
      if(DeltaR <= 1e-5)   Summary.ClosestDRWindow1E5++;
      if(DeltaR <= 1e-4)   Summary.ClosestDRWindow1E4++;
      if(DeltaR <= 1e-3)   Summary.ClosestDRWindow1E3++;
   }

   void PrintMultiplicityLine(const string &Label, const array<long long, 6> &Count)
   {
      cout << Label
         << " 0=" << Count[0]
         << " 1=" << Count[1]
         << " 2=" << Count[2]
         << " 3=" << Count[3]
         << " 4=" << Count[4]
         << " 5plus=" << Count[5]
         << endl;
   }

   vector<RawMuon> GetRawMuons(const MuTreeMessenger &MMu, const string &RawMuonSource)
   {
      vector<RawMuon> Result;

      if(RawMuonSource == "single")
      {
         Result.reserve(max(MMu.NGlb, 0));
         for(int i = 0; i < MMu.NGlb; i++)
            Result.push_back({MMu.GlbEta[i], MMu.GlbPhi[i], MMu.GlbPT[i]});
      }
      else
      {
         Result.reserve(max(MMu.NDi * 2, 0));
         for(int i = 0; i < MMu.NDi; i++)
         {
            Result.push_back({MMu.DiEta1[i], MMu.DiPhi1[i], MMu.DiPT1[i]});
            Result.push_back({MMu.DiEta2[i], MMu.DiPhi2[i], MMu.DiPT2[i]});
         }
      }

      return Result;
   }

   int GetPbPbTrackCount(const PbPbTrackTreeMessenger &MTrack)
   {
      if(MTrack.TrackPT == nullptr || MTrack.TrackEta == nullptr || MTrack.TrackPhi == nullptr)
         return 0;

      return min({(int)MTrack.TrackPT->size(), (int)MTrack.TrackEta->size(), (int)MTrack.TrackPhi->size()});
   }

   bool HasTriggerPrefix(const TriggerTreeMessenger &MTrigger, const string &TriggerPrefix)
   {
      for(int i = 0; i < (int)MTrigger.Name.size(); i++)
      {
         if(MTrigger.Exist[i] == false || MTrigger.Name[i].find(TriggerPrefix) != 0)
            continue;

         return true;
      }

      return false;
   }

   void PrintRawForestSummary(const string &Label, const string &RawMuonSource,
      const vector<string> &RawTriggerAND, const vector<long long> &TriggerPresentEvents,
      const vector<long long> &TriggerNonZeroEvents, const RawForestSummary &Summary)
   {
      cout << "============================================" << endl;
      cout << Label << endl;
      cout << "mode=forest reco/raw muon-track study" << endl;
      cout << "raw_muon_source=" << RawMuonSource << endl;
      cout << "scanned_events=" << Summary.ScannedEvents << endl;
      cout << "processed_events=" << Summary.ProcessedEvents << endl;
      cout << "raw_trigger_and_count=" << RawTriggerAND.size() << endl;
      if(RawTriggerAND.size() > 0)
      {
         cout << "events_passing_trigger_and=" << Summary.EventsPassingTriggerAND << endl;
         cout << "events_failing_trigger_and=" << Summary.EventsFailingTriggerAND << endl;
      }
      for(int i = 0; i < (int)RawTriggerAND.size(); i++)
      {
         cout << "raw_trigger_and_" << i << "=" << RawTriggerAND[i] << endl;
         cout << "trigger_prefix_present_events_" << i << "=" << TriggerPresentEvents[i] << endl;
         cout << "trigger_prefix_nonzero_events_" << i << "=" << TriggerNonZeroEvents[i] << endl;
      }
      cout << "events_with_tracks=" << Summary.EventsWithTracks << endl;
      cout << "events_nglb_eq_2=" << Summary.EventsNGlbEQ2 << endl;
      cout << "events_ndi_eq_1=" << Summary.EventsNDiEQ1 << endl;
      cout << "events_nglb_eq_2_and_ndi_eq_1=" << Summary.EventsNGlbEQ2NDiEQ1 << endl;
      cout << "total_nglb_muons=" << Summary.TotalNGlbMuons << endl;
      cout << "total_dimuon_leg_muons=" << Summary.TotalDimuonLegMuons << endl;
      cout << "total_raw_muons=" << Summary.TotalRawMuons << endl;
      cout << "total_raw_tracks=" << Summary.TotalRawTracks << endl;
      cout << "all_pairs=" << Summary.AllPairs << endl;
      cout << "closest_pairs=" << Summary.ClosestPairs << endl;
      cout << "excluding_closest_pairs=" << Summary.ExcludingClosestPairs << endl;
      cout << "unique_closest_tracks_excluded=" << Summary.UniqueClosestTracksExcluded << endl;
      cout << "muons_with_closest_track=" << Summary.MuonsWithClosestTrack << endl;
      cout << "muons_without_track=" << Summary.MuonsWithoutTrack << endl;
      cout << "central_box_all_pairs=" << Summary.CentralBoxAllPairs << endl;
      cout << "central_box_closest_pairs=" << Summary.CentralBoxClosestPairs << endl;
      cout << "central_box_excluding_closest_pairs=" << Summary.CentralBoxExcludingClosestPairs << endl;
      cout << "closest_exact_coordinate_match=" << Summary.ClosestExactCoordinateMatch << endl;
      cout << "closest_abs_window_1e-6=" << Summary.ClosestAbsWindow1E6 << endl;
      cout << "closest_abs_window_1e-5=" << Summary.ClosestAbsWindow1E5 << endl;
      cout << "closest_abs_window_1e-4=" << Summary.ClosestAbsWindow1E4 << endl;
      cout << "closest_abs_window_1e-3=" << Summary.ClosestAbsWindow1E3 << endl;
      cout << "closest_deltar_window_1e-6=" << Summary.ClosestDRWindow1E6 << endl;
      cout << "closest_deltar_window_1e-5=" << Summary.ClosestDRWindow1E5 << endl;
      cout << "closest_deltar_window_1e-4=" << Summary.ClosestDRWindow1E4 << endl;
      cout << "closest_deltar_window_1e-3=" << Summary.ClosestDRWindow1E3 << endl;
      PrintMultiplicityLine("nglb_multiplicity", Summary.NGlbMultiplicity);
      PrintMultiplicityLine("ndi_multiplicity", Summary.NDiMultiplicity);
   }

   int RunRawForestRecoStudy(const vector<string> &InputFileNames, const string &Output,
      const string &Label, int MaxEvents, const string &RawMuonSource,
      const vector<string> &RawTriggerAND)
   {
      TH2D HAllPairs("hAllRawPairs", "", 200, -0.01, 0.01, 200, -0.01, 0.01);
      TH2D HClosestOnly("hClosestRawPairs", "", 200, -0.01, 0.01, 200, -0.01, 0.01);
      TH2D HExcludingClosest("hExcludingClosestRawPairs", "", 200, -0.01, 0.01, 200, -0.01, 0.01);

      RawForestSummary Summary;
      vector<long long> TriggerPresentEvents(RawTriggerAND.size(), 0);
      vector<long long> TriggerNonZeroEvents(RawTriggerAND.size(), 0);

      for(string InputFileName : InputFileNames)
      {
         TFile InputFile(InputFileName.c_str());
         if(InputFile.IsZombie() == true)
         {
            cerr << "Unable to open " << InputFileName << endl;
            return 1;
         }

         TrackTreeMessenger MTrack(&InputFile, "ppTrack/trackTree");
         PbPbTrackTreeMessenger MPbPbTrack(&InputFile, "PbPbTracks/trackTree");
         TriggerTreeMessenger MTrigger(&InputFile);
         MuTreeMessenger MMu(&InputFile);
         bool UsePPTrackTree = (MTrack.Tree != nullptr);
         bool UsePbPbTrackTree = (UsePPTrackTree == false && MPbPbTrack.Tree != nullptr);

         if((UsePPTrackTree == false && UsePbPbTrackTree == false) || MMu.Tree == nullptr)
         {
            cerr << "Missing required forest tree in " << InputFileName << endl;
            return 1;
         }
         if(RawTriggerAND.size() > 0 && MTrigger.Tree == nullptr)
         {
            cerr << "Missing required trigger tree in " << InputFileName << endl;
            return 1;
         }

         int EntryCount = min(UsePPTrackTree ? (int)MTrack.Tree->GetEntries() : (int)MPbPbTrack.Tree->GetEntries(),
            (int)MMu.Tree->GetEntries());
         if(RawTriggerAND.size() > 0)
            EntryCount = min(EntryCount, (int)MTrigger.Tree->GetEntries());
         for(int iE = 0; iE < EntryCount; iE++)
         {
            if(MaxEvents >= 0 && Summary.ProcessedEvents >= MaxEvents)
               break;

            Summary.ScannedEvents++;

            if(RawTriggerAND.size() > 0)
            {
               MTrigger.GetEntry(iE);

               bool PassTriggerAND = true;
               for(int i = 0; i < (int)RawTriggerAND.size(); i++)
               {
                  if(HasTriggerPrefix(MTrigger, RawTriggerAND[i]) == false)
                  {
                     cerr << "Missing required trigger prefix " << RawTriggerAND[i]
                        << " in " << InputFileName << endl;
                     return 1;
                  }

                  int Decision = MTrigger.CheckTriggerStartWith(RawTriggerAND[i]);
                  TriggerPresentEvents[i]++;
                  if(Decision != 0)
                     TriggerNonZeroEvents[i]++;
                  if(Decision == 0)
                     PassTriggerAND = false;
               }

               if(PassTriggerAND == false)
               {
                  Summary.EventsFailingTriggerAND++;
                  continue;
               }

               Summary.EventsPassingTriggerAND++;
            }

            if(UsePPTrackTree == true)
               MTrack.GetEntry(iE);
            else
               MPbPbTrack.GetEntry(iE);
            MMu.GetEntry(iE);
            Summary.ProcessedEvents++;

            CountMultiplicity(Summary.NGlbMultiplicity, MMu.NGlb);
            CountMultiplicity(Summary.NDiMultiplicity, MMu.NDi);

            if(MMu.NGlb == 2)   Summary.EventsNGlbEQ2++;
            if(MMu.NDi == 1)    Summary.EventsNDiEQ1++;
            if(MMu.NGlb == 2 && MMu.NDi == 1)
               Summary.EventsNGlbEQ2NDiEQ1++;

            Summary.TotalNGlbMuons += MMu.NGlb;
            Summary.TotalDimuonLegMuons += MMu.NDi * 2;
            int TrackCount = (UsePPTrackTree == true) ? MTrack.nTrk : GetPbPbTrackCount(MPbPbTrack);
            Summary.TotalRawTracks += TrackCount;
            if(TrackCount > 0)
               Summary.EventsWithTracks++;

            vector<RawMuon> Muons = GetRawMuons(MMu, RawMuonSource);
            Summary.TotalRawMuons += Muons.size();

            vector<int> ClosestTrackIndex(Muons.size(), -1);
            vector<double> ClosestDeltaEta(Muons.size(), 0);
            vector<double> ClosestDeltaPhi(Muons.size(), 0);
            vector<double> ClosestDeltaR(Muons.size(), numeric_limits<double>::infinity());
            vector<bool> TrackExcluded(max(TrackCount, 0), false);

            for(int iM = 0; iM < (int)Muons.size(); iM++)
            {
               double MuEta = Muons[iM].Eta;
               double MuPhi = Muons[iM].Phi;

               for(int iT = 0; iT < TrackCount; iT++)
               {
                  double TrackEta = (UsePPTrackTree == true) ? MTrack.trkEta[iT] : MPbPbTrack.TrackEta->at(iT);
                  double TrackPhi = (UsePPTrackTree == true) ? MTrack.trkPhi[iT] : MPbPbTrack.TrackPhi->at(iT);
                  double DeltaEta = TrackEta - MuEta;
                  double DeltaPhi = WrapDeltaPhi(TrackPhi, MuPhi);
                  double DeltaR = sqrt(DeltaEta * DeltaEta + DeltaPhi * DeltaPhi);

                  HAllPairs.Fill(DeltaEta, DeltaPhi);
                  Summary.AllPairs++;
                  if(InCentralBox(DeltaEta, DeltaPhi) == true)
                     Summary.CentralBoxAllPairs++;

                  if(DeltaR < ClosestDeltaR[iM])
                  {
                     ClosestDeltaR[iM] = DeltaR;
                     ClosestDeltaEta[iM] = DeltaEta;
                     ClosestDeltaPhi[iM] = DeltaPhi;
                     ClosestTrackIndex[iM] = iT;
                  }
               }

               if(ClosestTrackIndex[iM] >= 0)
               {
                  Summary.ClosestPairs++;
                  Summary.MuonsWithClosestTrack++;
                  TrackExcluded[ClosestTrackIndex[iM]] = true;
                  HClosestOnly.Fill(ClosestDeltaEta[iM], ClosestDeltaPhi[iM]);
                  if(InCentralBox(ClosestDeltaEta[iM], ClosestDeltaPhi[iM]) == true)
                     Summary.CentralBoxClosestPairs++;
                  RecordClosestWindows(Summary, ClosestDeltaEta[iM], ClosestDeltaPhi[iM], ClosestDeltaR[iM]);
               }
               else
                  Summary.MuonsWithoutTrack++;
            }

            Summary.UniqueClosestTracksExcluded += count(TrackExcluded.begin(), TrackExcluded.end(), true);

            for(int iT = 0; iT < TrackCount; iT++)
            {
               if(TrackExcluded[iT] == true)
                  continue;

               for(int iM = 0; iM < (int)Muons.size(); iM++)
               {
                  double TrackEta = (UsePPTrackTree == true) ? MTrack.trkEta[iT] : MPbPbTrack.TrackEta->at(iT);
                  double TrackPhi = (UsePPTrackTree == true) ? MTrack.trkPhi[iT] : MPbPbTrack.TrackPhi->at(iT);
                  double DeltaEta = TrackEta - Muons[iM].Eta;
                  double DeltaPhi = WrapDeltaPhi(TrackPhi, Muons[iM].Phi);

                  HExcludingClosest.Fill(DeltaEta, DeltaPhi);
                  Summary.ExcludingClosestPairs++;
                  if(InCentralBox(DeltaEta, DeltaPhi) == true)
                     Summary.CentralBoxExcludingClosestPairs++;
               }
            }
         }

         if(MaxEvents >= 0 && Summary.ProcessedEvents >= MaxEvents)
            break;
      }

      PrintRawForestSummary(Label, RawMuonSource, RawTriggerAND,
         TriggerPresentEvents, TriggerNonZeroEvents, Summary);

      if(Output != "")
         SaveRawForestPlots(Output, Label, HAllPairs, HClosestOnly, HExcludingClosest);

      return 0;
   }

   void PrintSummary(const string &Label, const string &Mode, const string &TrackType,
      const string &MuonType, const ScanSummary &Summary)
   {
      cout << "============================================" << endl;
      cout << Label << endl;
      cout << "mode=" << Mode << " track=" << TrackType << " muon=" << MuonType << endl;
      cout << "considered_tracks=" << Summary.ConsideredTracks << endl;
      cout << "used_tracks=" << Summary.UsedTracks << endl;
      cout << "tagged_tracks=" << Summary.TaggedTracks << endl;
      cout << "untagged_tracks=" << Summary.UntaggedTracks << endl;
      cout << "central_box_all=" << Summary.BoxAll << endl;
      cout << "central_box_tagged=" << Summary.BoxTagged << endl;
      cout << "central_box_untagged=" << Summary.BoxUntagged << endl;
      cout << "central_box_outside_r0p001_all=" << Summary.BoxOutsideR001All << endl;
      cout << "central_box_outside_r0p001_tagged=" << Summary.BoxOutsideR001Tagged << endl;
      cout << "central_box_outside_r0p001_untagged=" << Summary.BoxOutsideR001Untagged << endl;
      cout << "veto_circle_all=" << Summary.VetoCircleAll << endl;
      cout << "veto_circle_tagged=" << Summary.VetoCircleTagged << endl;
      cout << "veto_circle_untagged=" << Summary.VetoCircleUntagged << endl;
   }
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string Mode = CL.Get("Mode", "skim");
   string TrackType = CL.Get("TrackType", "reco");
   string MuonType = CL.Get("MuonType", "reco");
   string TreeName = CL.Get("TreeName", "Tree");
   string Output = CL.Get("Output", "");
   string Label = CL.Get("Label", "muon-track rectangle scan");
   string ForestRecoMode = CL.Get("ForestRecoMode", "selected");
   string RawMuonSource = CL.Get("RawMuonSource", "single");
   vector<string> RawTriggerAND = CL.GetStringVector("RawTriggerAND", vector<string>{});
   double MinTrackPT = CL.GetDouble("MinTrackPT", 0.5);
   int MaxEvents = CL.GetInt("MaxEvents", -1);
   vector<string> InputFileNames = CL.GetStringVector("Input");

   if(InputFileNames.size() == 0)
   {
      cerr << "Please provide --Input" << endl;
      return 1;
   }

   if(Mode != "skim" && Mode != "forest")
   {
      cerr << "Mode must be skim or forest" << endl;
      return 1;
   }
   if(TrackType != "reco" && TrackType != "gen")
   {
      cerr << "TrackType must be reco or gen" << endl;
      return 1;
   }
   if(MuonType != "reco" && MuonType != "gen")
   {
      cerr << "MuonType must be reco or gen" << endl;
      return 1;
   }
   if(Mode == "skim" && TrackType != "reco")
   {
      cerr << "Skim mode only supports TrackType=reco" << endl;
      return 1;
   }
   if(ForestRecoMode != "selected" && ForestRecoMode != "raw")
   {
      cerr << "ForestRecoMode must be selected or raw" << endl;
      return 1;
   }
   if(RawMuonSource != "single" && RawMuonSource != "dimuonleg")
   {
      cerr << "RawMuonSource must be single or dimuonleg" << endl;
      return 1;
   }
   if(Mode == "forest" && TrackType == "reco" && MuonType == "reco" && ForestRecoMode == "raw")
      return RunRawForestRecoStudy(InputFileNames, Output, Label, MaxEvents, RawMuonSource, RawTriggerAND);

   TH2D HAll("hAll", "", 200, -0.01, 0.01, 200, -0.01, 0.01);
   TH2D HTagged("hTagged", "", 200, -0.01, 0.01, 200, -0.01, 0.01);
   TH2D HUntagged("hUntagged", "", 200, -0.01, 0.01, 200, -0.01, 0.01);

   ScanSummary Summary;

   int ProcessedEvents = 0;

   for(string InputFileName : InputFileNames)
   {
      TFile InputFile(InputFileName.c_str());
      if(InputFile.IsZombie() == true)
      {
         cerr << "Unable to open " << InputFileName << endl;
         return 1;
      }

      if(Mode == "skim")
      {
         ZHadronMessenger M(&InputFile, TreeName);
         int EntryCount = M.GetEntries();
         for(int iE = 0; iE < EntryCount; iE++)
         {
            if(MaxEvents >= 0 && ProcessedEvents >= MaxEvents)
               break;

            M.GetEntry(iE);
            ProcessedEvents++;

            MuonPair Pair = (MuonType == "reco") ? FindSkimRecoMuonPair(M) : FindSkimGenMuonPair(M);
            if(Pair.Valid == false)
               continue;

            for(int iT = 0; iT < (int)M.trackPt->size(); iT++)
            {
               Summary.ConsideredTracks++;

               double TrackPT = M.trackPt->at(iT);
               double TrackEta = M.trackEta->at(iT);
               double TrackPhi = M.trackPhi->at(iT);

               if(TrackPT < MinTrackPT)   continue;
               if(TrackEta < -2.4)        continue;
               if(TrackEta > +2.4)        continue;

               bool Tagged = false;
               if(MuonType == "reco" && M.trackMuTagged != nullptr && iT < (int)M.trackMuTagged->size())
                  Tagged = M.trackMuTagged->at(iT);
               else
               {
                  double DeltaEta1 = TrackEta - Pair.Eta1;
                  double DeltaPhi1 = WrapDeltaPhi(TrackPhi, Pair.Phi1);
                  double DeltaR1 = sqrt(DeltaEta1 * DeltaEta1 + DeltaPhi1 * DeltaPhi1);
                  double DeltaEta2 = TrackEta - Pair.Eta2;
                  double DeltaPhi2 = WrapDeltaPhi(TrackPhi, Pair.Phi2);
                  double DeltaR2 = sqrt(DeltaEta2 * DeltaEta2 + DeltaPhi2 * DeltaPhi2);
                  Tagged = (min(DeltaR1, DeltaR2) < MuonVeto);
               }

               RecordTrack(Summary, HAll, HTagged, HUntagged, TrackEta, TrackPhi, Pair, Tagged);
            }
         }
      }
      else
      {
         TrackTreeMessenger MTrack(&InputFile, "ppTrack/trackTree");
         MuTreeMessenger MMu(&InputFile);
         GenParticleTreeMessenger MGen(&InputFile);

         int EntryCount = 0;
         if(TrackType == "reco" && MTrack.Tree != nullptr)
            EntryCount = MTrack.Tree->GetEntries();
         else if(TrackType == "gen" && MGen.Tree != nullptr)
            EntryCount = MGen.Tree->GetEntries();
         else if(MMu.Tree != nullptr)
            EntryCount = MMu.Tree->GetEntries();

         for(int iE = 0; iE < EntryCount; iE++)
         {
            if(MaxEvents >= 0 && ProcessedEvents >= MaxEvents)
               break;

            if(MMu.Tree != nullptr)     MMu.GetEntry(iE);
            if(MTrack.Tree != nullptr)  MTrack.GetEntry(iE);
            if(MGen.Tree != nullptr)    MGen.GetEntry(iE);
            ProcessedEvents++;

            MuonPair Pair = (MuonType == "reco") ? FindRecoMuonPair(MMu) : FindGenMuonPair(MMu);
            if(Pair.Valid == false)
               continue;

            if(TrackType == "reco")
            {
               for(int iT = 0; iT < MTrack.nTrk; iT++)
               {
                  Summary.ConsideredTracks++;

                  if(MTrack.PassZHadron2022Cut(iT) == false)
                     continue;
                  if(MTrack.trkPt[iT] < MinTrackPT)   continue;
                  if(MTrack.trkEta[iT] < -2.4)        continue;
                  if(MTrack.trkEta[iT] > +2.4)        continue;

                  double DeltaEta1 = MTrack.trkEta[iT] - Pair.Eta1;
                  double DeltaPhi1 = WrapDeltaPhi(MTrack.trkPhi[iT], Pair.Phi1);
                  double DeltaR1 = sqrt(DeltaEta1 * DeltaEta1 + DeltaPhi1 * DeltaPhi1);
                  double DeltaEta2 = MTrack.trkEta[iT] - Pair.Eta2;
                  double DeltaPhi2 = WrapDeltaPhi(MTrack.trkPhi[iT], Pair.Phi2);
                  double DeltaR2 = sqrt(DeltaEta2 * DeltaEta2 + DeltaPhi2 * DeltaPhi2);
                  bool Tagged = (min(DeltaR1, DeltaR2) < MuonVeto);

                  RecordTrack(Summary, HAll, HTagged, HUntagged, MTrack.trkEta[iT], MTrack.trkPhi[iT], Pair, Tagged);
               }
            }
            else
            {
               if(MGen.PT == nullptr || MGen.Eta == nullptr || MGen.Phi == nullptr
                  || MGen.Charge == nullptr || MGen.DaughterCount == nullptr)
                  continue;

               for(int iT = 0; iT < MGen.Mult; iT++)
               {
                  Summary.ConsideredTracks++;

                  if(MGen.DaughterCount->at(iT) > 0)   continue;
                  if(MGen.Charge->at(iT) == 0)         continue;
                  if(MGen.PT->at(iT) < MinTrackPT)     continue;
                  if(MGen.Eta->at(iT) < -2.4)          continue;
                  if(MGen.Eta->at(iT) > +2.4)          continue;

                  double DeltaEta1 = MGen.Eta->at(iT) - Pair.Eta1;
                  double DeltaPhi1 = WrapDeltaPhi(MGen.Phi->at(iT), Pair.Phi1);
                  double DeltaR1 = sqrt(DeltaEta1 * DeltaEta1 + DeltaPhi1 * DeltaPhi1);
                  double DeltaEta2 = MGen.Eta->at(iT) - Pair.Eta2;
                  double DeltaPhi2 = WrapDeltaPhi(MGen.Phi->at(iT), Pair.Phi2);
                  double DeltaR2 = sqrt(DeltaEta2 * DeltaEta2 + DeltaPhi2 * DeltaPhi2);
                  bool Tagged = (min(DeltaR1, DeltaR2) < MuonVeto);

                  RecordTrack(Summary, HAll, HTagged, HUntagged, MGen.Eta->at(iT), MGen.Phi->at(iT), Pair, Tagged);
               }
            }
         }
      }

      if(MaxEvents >= 0 && ProcessedEvents >= MaxEvents)
         break;
   }

   PrintSummary(Label, Mode, TrackType, MuonType, Summary);

   if(Output != "")
      SavePlots(Output, Label, HAll, HTagged, HUntagged);

   return 0;
}
