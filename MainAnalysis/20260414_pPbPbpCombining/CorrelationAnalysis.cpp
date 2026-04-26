#include <TCanvas.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TCut.h>
#include <TLegend.h>
#include <TNtuple.h>
#include <TFile.h>

#include <iostream>
#include <cmath>
#include <limits>
#include <memory>
#include <utility>

using namespace std;
#include "utilities.h"
#include "helpMessage.h"
#include "parameter.h"
#include "Messenger.h"
#include "CommandLine.h"
#include "ProgressBar.h"
#include "TrackResidualCorrector.h"

bool checkError(const Parameters& par) {
   if (par.useVZWeight && par.VZWeightFile == "") {
      cout << "Error! UseVZWeight=true requires an explicit external VZWeightFile." << endl;
      return true;
   }
   return false;
}

class ZYDirectCorrector
{
public:
   ZYDirectCorrector(const string &filename)
   {
      File = new TFile(filename.c_str());
      Weight = (TH2D *)File->Get("hWeightToApply");
   }

   ~ZYDirectCorrector()
   {
      if(File != nullptr)
      {
         File->Close();
         delete File;
      }
   }

   double GetCorrectionFactor(double yCM, double phi) const
   {
      if(Weight == nullptr)
         return 1;

      while(phi < 0)          phi += 2 * M_PI;
      while(phi >= 2 * M_PI)  phi -= 2 * M_PI;

      const TAxis *yAxis = Weight->GetXaxis();
      const TAxis *phiAxis = Weight->GetYaxis();

      int yBin = yAxis->FindBin(yCM);
      int phiBin = phiAxis->FindBin(phi);

      if(yCM <= yAxis->GetXmin()) yBin = 1;
      if(yCM >= yAxis->GetXmax()) yBin = yAxis->GetNbins();
      if(phi <= phiAxis->GetXmin()) phiBin = 1;
      if(phi >= phiAxis->GetXmax()) phiBin = phiAxis->GetNbins();

      double correction = Weight->GetBinContent(yBin, phiBin);
      if(!std::isfinite(correction) || correction <= 0)
         correction = 1;

      return correction;
   }

private:
   TFile *File = nullptr;
   TH2D *Weight = nullptr;
};

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
   const pair<int, int> &closestMuonTracks = {-1, -1}) {
   if (rejectMuonMatchedTrack(b, par, j, closestMuonTracks)) return false;
   if ((*b->trackPt)[j] > par.MaxTrackPT) return false;
   if ((*b->trackPt)[j] < par.MinTrackPT) return false;
   if ((!par.includeHole) && (*b->trackWeight)[j] < 0) return false;
   if ((*b->trackEta)[j] > 2.4) return false;
   if ((*b->trackEta)[j] < -2.4) return false;
   return true;
}

bool eventSelection(ZHadronMessenger *b, const Parameters& par) {
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

int main(int argc, char *argv[])
{
   if (printHelpMessage(argc, argv)) return 0;

   CommandLine CL(argc, argv);
   float MinZPT      = CL.GetDouble("MinZPT", 0);
   float MaxZPT      = CL.GetDouble("MaxZPT", 500);
   float MinTrackPT  = CL.GetDouble("MinTrackPT", 0.5);
   float MaxTrackPT  = CL.GetDouble("MaxTrackPT", 15);
   bool  IsData      = CL.GetBool  ("IsData", true);
   bool  IsPP        = CL.GetBool  ("IsPP", false);
   string MinRunString = CL.Get("MinRun", "");
   string MaxRunString = CL.Get("MaxRun", "");

   Parameters par(MinZPT, MaxZPT, MinTrackPT, MaxTrackPT);
   par.input         = CL.Get      ("Input",   "");
   par.mixFile       = par.input;
   par.output        = CL.Get      ("Output",  "output.root");
   par.isSelfMixing  = true;
   par.isGenZ        = CL.GetBool  ("IsGenZ", false);
   par.isPUReject    = CL.GetBool  ("IsPUReject", false);
   par.isMuTagged    = CL.GetBool  ("IsMuTagged", true);
   par.TrackMuDR     = CL.GetDouble("TrackMuDR", -1);
   par.TrackMuClosest = false;
   par.useTrackWeight   = CL.GetBool  ("UseTrackWeight", true);
   par.TrackExtraWeight = 1.0;
   par.useEventWeight   = CL.GetBool  ("UseEventWeight", true);
   par.useZWeight       = CL.GetBool  ("UseZWeight", false);
   par.ZWeightFile      = CL.Get      ("ZWeightFile", "");
   par.useResidualWeight = CL.GetBool  ("UseResidualWeight", false);
   par.residualWeightFile = CL.Get      ("ResidualWeightFile", "");
   par.EnergyExtraFile = "";
   par.VZWeightFile     = CL.Get      ("VZWeightFile", "");
   par.useVZWeight       = CL.GetBool  ("UseVZWeight", false);
   par.useVZWindow       = CL.GetBool  ("UseVZWindow", true);
   par.useFastMixing     = false;
   par.TrackSelectionMode = CL.Get      ("TrackSelectionMode", "Nominal");
   par.TrackTreeName   = "Tree";
   if (par.TrackSelectionMode == "Loose")   par.TrackTreeName = "TreeLoose";
   if (par.TrackSelectionMode == "Tight")   par.TrackTreeName = "TreeTight";
   par.scaleFactor   = CL.GetDouble("Fraction", 1.00);
   par.nThread       = CL.GetInt   ("nThread", 1);
   par.nChunk        = CL.GetInt   ("nChunk", 1);
   par.nMix          = 0;
   par.shift         = 0;
   par.MinZY         = CL.GetDouble("MinZY", 0);
   par.MaxZY         = CL.GetDouble("MaxZY", 200);
   par.MinRun        = (MinRunString == "") ? numeric_limits<long long>::min() : stoll(MinRunString);
   par.MaxRun        = (MaxRunString == "") ? numeric_limits<long long>::max() : stoll(MaxRunString);
   par.useZScaleFactor = CL.GetBool("UseZScaleFactor", false);
   par.includeHole   = true;
   par.isPPb         = CL.GetBool  ("IsPPb", false);
   par.yBoost        = CL.GetDouble("yBoost", 0);
   par.ZCorrectionFile = CL.Get    ("ZCorrectionFile", "");
   par.mix = false;
   par.isPP = IsPP;
   par.isData = IsData;
   par.isJewel = false;

   if (checkError(par)) return -1;
   par.printParameters();

   // Open files
   TFile *inf = new TFile(par.input.c_str());
   ZHadronMessenger *M = new ZHadronMessenger(*inf, string(par.TrackTreeName));
   TFile *outf = new TFile(par.output.c_str(), "recreate");

   // Track pT histogram (log binning)
   const int nPtBins = 25;
   double ptBins[nPtBins + 1];
   for (int i = 0; i <= nPtBins; i++)
      ptBins[i] = 0.5 * pow(15.0/0.5, double(i)/nPtBins);

   // Track eta histogram (linear, in CM frame)
   const int nEtaBins = 24;
   double etaMin = -3.5, etaMax = 2.5;

   // Z pT histogram (log binning)
   const int nZPtBins = 25;
   double zptBins[nZPtBins + 1];
   for (int i = 0; i <= nZPtBins; i++)
      zptBins[i] = 0.5 * pow(500.0/0.5, double(i)/nZPtBins);

   // Z y histogram (linear, in CM frame)
   const int nZYBins = 24;
   double zyMin = -3.5, zyMax = 2.5;

   TH1D *hTrkPt  = new TH1D("hTrkPtData",  ";Track p_{T} [GeV];dN/dp_{T}/N_{Z}", nPtBins, ptBins);
   TH1D *hTrkEta = new TH1D("hTrkEtaData", ";Track #eta_{CM};dN/d#eta/N_{Z}", nEtaBins, etaMin, etaMax);
   TH1D *hZPt    = new TH1D("hZPtData",    ";Z p_{T} [GeV];dN/dp_{T}/N_{Z}", nZPtBins, zptBins);
   TH1D *hZY     = new TH1D("hZYData",     ";Z y_{CM};dN/dy/N_{Z}", nZYBins, zyMin, zyMax);
   TH1D *hZPhi   = new TH1D("hZPhiData",   ";Z #phi;dN/d#phi/N_{Z}", 12, 0, 2 * M_PI);
   TH1D *hNZ     = new TH1D("hNZData",     "", 1, 0, 1);

   hTrkPt->Sumw2();
   hTrkEta->Sumw2();
   hZPt->Sumw2();
   hZY->Sumw2();
   hZPhi->Sumw2();

   // Z-track correlation histograms
    const int nDEtaBins = 12;
    double dEtaMin = 0, dEtaMax = 4.8;
    const int nDPhiBins = 12;
    double dPhiMin = 0, dPhiMax = M_PI;
    const int nDEtaDPhiBins = 120;

    TH1D *hDEta  = new TH1D("hDEtaData",  ";|#Delta#eta|;1/N_{Z} dN/d|#Delta#eta|", nDEtaBins, dEtaMin, dEtaMax);
    TH1D *hDPhi  = new TH1D("hDPhiData",  ";|#Delta#phi|;1/N_{Z} dN/d|#Delta#phi|", nDPhiBins, dPhiMin, dPhiMax);
    TH2D *hDEtaDPhi = new TH2D("hDEtaDPhiData", ";|#Delta#eta|;|#Delta#phi|",
       nDEtaDPhiBins, dEtaMin, dEtaMax, nDEtaDPhiBins, dPhiMin, dPhiMax);
   TH2D *hZEtaPhi = new TH2D("hZEtaPhiData", ";Z #eta;Z #phi",
      24, -2.4, 2.4, 12, 0, 2 * M_PI);
   TH2D *hZYPhi = new TH2D("hZYPhiData", ";Z y_{CM};Z #phi",
      nZYBins, zyMin, zyMax, 12, 0, 2 * M_PI);

   hDEta->Sumw2();
   hDPhi->Sumw2();
   hDEtaDPhi->Sumw2();
   hZEtaPhi->Sumw2();
   hZYPhi->Sumw2();

   // Event multiplicity (track count per event)
   TH1D *hMult = new TH1D("hMultData", ";N_{trk};Events", 20, 0, 150);
   hMult->Sumw2();

   // 3D Z histogram for correction derivation (pT, y_CM, phi)
   // Use log-spaced pT bins from 0.5–100 GeV; events outside land in overflow (correction=1)
   const int nZ3dPtBins = 50;
   double z3dPtBins[nZ3dPtBins + 1];
   double z3dLogMin = log(0.5), z3dLogMax = log(100.0);
   for (int i = 0; i <= nZ3dPtBins; i++)
       z3dPtBins[i] = exp(z3dLogMin + (z3dLogMax - z3dLogMin) * i / nZ3dPtBins);
   const int nZ3dYBins = 24;
   double z3dYBins[nZ3dYBins + 1];
   for (int i = 0; i <= nZ3dYBins; i++)
       z3dYBins[i] = -3.5 + (2.5 - (-3.5)) * i / nZ3dYBins;
   const int nZ3dPhiBins = 12;
   double z3dPhiBins[nZ3dPhiBins + 1];
   for (int i = 0; i <= nZ3dPhiBins; i++)
       z3dPhiBins[i] = 2 * M_PI * i / nZ3dPhiBins;
   TH3D *hZ3D = new TH3D("hZ3D", ";Z p_{T};Z y_{CM};Z #phi",
       nZ3dPtBins, z3dPtBins, nZ3dYBins, z3dYBins, nZ3dPhiBins, z3dPhiBins);
   hZ3D->Sumw2();

   // Open correctors if needed
   TrackResidualCorrector *Zcorrector = nullptr;
   if (par.useZWeight && par.ZWeightFile != "")
      Zcorrector = new TrackResidualCorrector(par.ZWeightFile.c_str());

   VZCorrector *vzCorrector = nullptr;
   if (par.useVZWeight)
      vzCorrector = new VZCorrector(par.VZWeightFile.c_str());

   unique_ptr<ZYDirectCorrector> ZCorrectionCorrector;
   if (par.ZCorrectionFile != "")
      ZCorrectionCorrector = make_unique<ZYDirectCorrector>(par.ZCorrectionFile);

   TrackResidualCorrector *corrector_0_10 = nullptr, *corrector_10_20 = nullptr,
                          *corrector_20_40 = nullptr, *corrector_40_500 = nullptr;
   if (par.useResidualWeight && par.residualWeightFile != "") {
      corrector_0_10   = new TrackResidualCorrector(Form("%s0-10.root",   par.residualWeightFile.c_str()));
      corrector_10_20  = new TrackResidualCorrector(Form("%s10-20.root",  par.residualWeightFile.c_str()));
      corrector_20_40  = new TrackResidualCorrector(Form("%s20-40.root",  par.residualWeightFile.c_str()));
      corrector_40_500 = new TrackResidualCorrector(Form("%s40-500.root", par.residualWeightFile.c_str()));
   }

   unsigned long nEntry = M->GetEntries() * par.scaleFactor;
   unsigned long iStart = nEntry * (par.nChunk - 1) / par.nThread;
   unsigned long iEnd = nEntry * par.nChunk / par.nThread;

   ProgressBar Bar(cout, iEnd - iStart);
   Bar.SetStyle(1);
   int deltaI = (iEnd - iStart) / 100 + 1;
   double nZ = 0;

   for (unsigned long i = iStart; i < iEnd; i++) {
      M->GetEntry(i);

      if (i % deltaI == 0) { Bar.Update(i - iStart); Bar.Print(); }

      if (!eventSelection(M, par)) continue;

      float zPt = (par.isGenZ ? (*M->genZPt)[0] : (*M->zPt)[0]);
      float zY  = (par.isGenZ ? (*M->genZY)[0] : (*M->zY)[0]);
      float zEta = (par.isGenZ ? (*M->genZEta)[0] : (*M->zEta)[0]);
      float zPhi = (par.isGenZ ? (*M->genZPhi)[0] : (*M->zPhi)[0]);
      if (zPhi < 0) zPhi += 2 * M_PI;

      // Boost Z y to CM frame (needed before Z correction lookup)
      float zY_CM = zY;
      if (!par.isPP) {
         if (par.isPPb) zY_CM = zY - par.yBoost;
         else           zY_CM = -(zY + par.yBoost);
      }

      // Z correction weight (from file) and skim TnP scale factor
      float ZWeight = (Zcorrector != nullptr) ? Zcorrector->GetCorrectionFactor(zPt, zY, zPhi) : 1;
      if (par.useZScaleFactor) ZWeight *= M->ZWeight;

      // Event weight
      float eventWeight = 1;
      if (par.useEventWeight) eventWeight *= M->EventWeight;
      if (par.useVZWeight) eventWeight *= vzCorrector->GetCorrectionFactor(M->VZ);
      if (par.useZWeight) eventWeight *= ZWeight;
       if (ZCorrectionCorrector != nullptr)
          eventWeight *= ZCorrectionCorrector->GetCorrectionFactor(zY_CM, zPhi);

      nZ += eventWeight;

      hZPt->Fill(zPt, eventWeight);
      hZY->Fill(zY_CM, eventWeight);
      hZPhi->Fill(zPhi, eventWeight);
      hZEtaPhi->Fill(zEta, zPhi, eventWeight);
      hZYPhi->Fill(zY_CM, zPhi, eventWeight);
      hZ3D->Fill(zPt, zY_CM, zPhi, eventWeight);

      // Select track residual corrector by Z pT bin
      TrackResidualCorrector *corrector = nullptr;
      if (par.useResidualWeight && par.residualWeightFile != "") {
         if (zPt < 10) corrector = corrector_0_10;
         else if (zPt < 20) corrector = corrector_10_20;
         else if (zPt < 40) corrector = corrector_20_40;
         else corrector = corrector_40_500;
      }

      // Track loop
      int nTracks = 0;
      pair<int, int> closestMuonTracks = findClosestMuonTracks(M, par);
      for (unsigned long j = 0; j < M->trackPt->size(); j++) {
         if (!trackSelection(M, par, j, closestMuonTracks)) continue;

         nTracks++;
         float trackPt  = (*M->trackPt)[j];
         float trackEta = (*M->trackEta)[j];
         float trackPhi = (*M->trackPhi)[j];

         float weight = eventWeight;
         if (par.useTrackWeight) weight *= (*M->trackWeight)[j];
         if (par.useResidualWeight && corrector)
            weight *= corrector->GetCorrectionFactor(trackPt, trackEta, trackPhi);

         // Boost track eta to CM frame
         float trackEta_CM = trackEta;
         if (!par.isPP) {
            if (par.isPPb) trackEta_CM = trackEta - par.yBoost;
            else           trackEta_CM = -(trackEta + par.yBoost);
         }

         hTrkPt->Fill(trackPt, weight);
         hTrkEta->Fill(trackEta_CM, weight);

         // Z-track correlations (lab frame, unsigned)
         float dEta = fabs(trackEta - zEta);
         float rawDPhi = trackPhi - zPhi;
         while (rawDPhi >  M_PI) rawDPhi -= 2*M_PI;
         while (rawDPhi < -M_PI) rawDPhi += 2*M_PI;
         float dPhi = fabs(rawDPhi);
         hDEta->Fill(dEta, weight);
         hDPhi->Fill(dPhi, weight);
         hDEtaDPhi->Fill(dEta, dPhi, weight);
      }
      hMult->Fill(nTracks, eventWeight);
   }
   cout << endl << "Total NZ: " << nZ << endl;

   hNZ->SetBinContent(1, nZ);

   outf->cd();
   hTrkPt->Write();
   hTrkEta->Write();
   hZPt->Write();
   hZY->Write();
   hZPhi->Write();
   hNZ->Write();
   hDEta->Write();
   hDPhi->Write();
   hDEtaDPhi->Write();
   hMult->Write();
   hZEtaPhi->Write();
   hZYPhi->Write();
   hZ3D->Write();
   outf->Close();
   cout << "done! " << par.output << endl;
}
