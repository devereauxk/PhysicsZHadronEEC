#include <TCanvas.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TCut.h>
#include <TLegend.h>
#include <TNtuple.h>
#include <TFile.h>

#include <cmath>
#include <iostream>
#include <utility>

using namespace std;
#include "utilities.h"              // Yen-Jie's random utility functions
#include "helpMessage.h"            // Print out help message
#include "parameter.h"              // The parameters used in the analysis
#include "Messenger.h"              // Yi's Messengers for reading data files
#include "CommandLine.h"            // Yi's Commandline bundle
#include "ProgressBar.h"            // Yi's fish progress bar
#include "TrackResidualCorrector.h" // Residual correction

//============================================================//
// Function to check for configuration errors
//============================================================//
bool checkError(const Parameters& par) {
   if (par.isSelfMixing && par.input != par.mixFile) {
      std::cout << "Error! Self-mixing mode but assigned different input and mix files. Please check the macro." << std::endl;
      return true;  // Return true indicates an error was found
   }

   return false;    // No errors found
}

bool hasTrackMuDRBranch(ZHadronMessenger *b) {
   return (b != nullptr && b->Tree != nullptr && b->Tree->GetBranch("trackMuDR") != nullptr
      && b->trackMuDR != nullptr);
}

double GetRawDeltaPhi(double phi1, double phi2) {
   double deltaPhi = phi1 - phi2;
   while (deltaPhi < -M_PI) deltaPhi += 2 * M_PI;
   while (deltaPhi > M_PI) deltaPhi -= 2 * M_PI;
   return deltaPhi;
}

float getTrackMuonDR(ZHadronMessenger *b, const Parameters &par, int trackIndex) {
   if (hasTrackMuDRBranch(b) && trackIndex < (int)b->trackMuDR->size())
      return (*b->trackMuDR)[trackIndex];

   if (trackIndex >= (int)b->trackEta->size() || trackIndex >= (int)b->trackPhi->size())
      return -1;

   vector<float> muEta;
   vector<float> muPhi;

   if (par.isGenZ) {
      if (b->genMuEta1 != nullptr && b->genMuEta1->size() > 0 && b->genMuPhi1 != nullptr && b->genMuPhi1->size() > 0) {
         muEta.push_back((*b->genMuEta1)[0]);
         muPhi.push_back((*b->genMuPhi1)[0]);
      }
      if (b->genMuEta2 != nullptr && b->genMuEta2->size() > 0 && b->genMuPhi2 != nullptr && b->genMuPhi2->size() > 0) {
         muEta.push_back((*b->genMuEta2)[0]);
         muPhi.push_back((*b->genMuPhi2)[0]);
      }
   }
   else {
      if (b->muEta1 != nullptr && b->muEta1->size() > 0 && b->muPhi1 != nullptr && b->muPhi1->size() > 0) {
         muEta.push_back((*b->muEta1)[0]);
         muPhi.push_back((*b->muPhi1)[0]);
      }
      if (b->muEta2 != nullptr && b->muEta2->size() > 0 && b->muPhi2 != nullptr && b->muPhi2->size() > 0) {
         muEta.push_back((*b->muEta2)[0]);
         muPhi.push_back((*b->muPhi2)[0]);
      }
   }

   if (muEta.size() == 0)
      return -1;

   float trackEta = (*b->trackEta)[trackIndex];
   float trackPhi = (*b->trackPhi)[trackIndex];
   float minDR = -1;

   for (int i = 0; i < (int)muEta.size(); i++) {
      double deltaEta = trackEta - muEta[i];
      double deltaPhi = GetRawDeltaPhi(trackPhi, muPhi[i]);
      float deltaR = sqrt(deltaEta * deltaEta + deltaPhi * deltaPhi);
      if (minDR < 0 || deltaR < minDR)
         minDR = deltaR;
   }

   return minDR;
}

pair<int, int> findClosestMuonTracks(ZHadronMessenger *b, const Parameters &par) {
   if (!par.TrackMuClosest)
      return {-1, -1};

   int closestTrack = -1;
   int secondClosestTrack = -1;
   float closestDR = -1;
   float secondClosestDR = -1;

   for (unsigned int i = 0; i < b->trackPt->size(); i++) {
      float trackMuDR = getTrackMuonDR(b, par, i);
      if (trackMuDR < 0)
         continue;

      if (closestTrack < 0 || trackMuDR < closestDR) {
         secondClosestTrack = closestTrack;
         secondClosestDR = closestDR;
         closestTrack = i;
         closestDR = trackMuDR;
      }
      else if (secondClosestTrack < 0 || trackMuDR < secondClosestDR) {
         secondClosestTrack = i;
         secondClosestDR = trackMuDR;
      }
   }

   return {closestTrack, secondClosestTrack};
}

bool rejectMuonMatchedTrack(ZHadronMessenger *b, const Parameters &par, int j,
   const pair<int, int> &closestMuonTracks = {-1, -1}) {
   if (par.TrackMuDR >= 0) {
      float trackMuDR = getTrackMuonDR(b, par, j);
      return (trackMuDR >= 0 && trackMuDR < par.TrackMuDR);
   }

   if (par.TrackMuClosest)
      return (j == closestMuonTracks.first || j == closestMuonTracks.second);

   return (par.isMuTagged && (*b->trackMuTagged)[j]);
}

//======= trackSelection =====================================//
// Check if the track pass selection criteria
//============================================================//
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

//======= trackSelection =====================================//
// Check if the track pass selection criteria
//============================================================//
bool trackSelectionNoPt(ZHadronMessenger *b, const Parameters &par, int j,
   const pair<int, int> &closestMuonTracks = {-1, -1}) {
   if (rejectMuonMatchedTrack(b, par, j, closestMuonTracks)) return false;
   if ((!par.includeHole) && (*b->trackWeight)[j] < 0) return false;
   if ((*b->trackEta)[j] > 2.4) return false;
   if ((*b->trackEta)[j] < -2.4) return false;
   return true;
}

bool trackStudySelection(ZHadronMessenger *b, const Parameters &par, int j,
   const pair<int, int> &closestMuonTracks = {-1, -1}) {
   if (rejectMuonMatchedTrack(b, par, j, closestMuonTracks)) return false;
   if ((*b->trackPt)[j] > par.MaxTrackPT) return false;
   if ((*b->trackPt)[j] < par.MinTrackPT) return false;
   if ((*b->trackEta)[j] > 2.4) return false;
   if ((*b->trackEta)[j] < -2.4) return false;
   return true;
}

//======= eventSelection =====================================//
// Check if the event mass eventSelection criteria
// MinZPT < zPt < MaxZPT
//============================================================//
bool eventSelection(ZHadronMessenger *b, const Parameters& par) {
   if (par.isPUReject && par.isPP && b->NVertex!=1) return 0;    // Only apply PU rejection (single vertex requirement) in pp analysis

   if ((par.isGenZ ? b->genZMass->size() : b->zMass->size()) == 0) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0]) < 60) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0]) > 120) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0])) <= par.MinZY) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0])) >= par.MaxZY) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0]) < par.MinZPT) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0]) > par.MaxZPT) return 0;

   /*
   if (b->genZMass->size()==0   || b->zMass->size()==0) return 0;
   if ((*b->genZMass)[0]<60 || (*b->zMass)[0]<60) return 0;
   if ((*b->genZMass)[0]>120 || (*b->zMass)[0]>120) return 0;
   if (fabs((*b->genZY)[0])<=par.MinZY || fabs((*b->zY)[0])<=par.MinZY) return 0;
   if (fabs((*b->genZY)[0])>=par.MaxZY || fabs((*b->zY)[0])>=par.MaxZY) return 0;
   if ((*b->genZPt)[0]<par.MinZPT || (*b->zPt)[0]<par.MinZPT) return 0;
   if ((*b->genZPt)[0]>par.MaxZPT || (*b->zPt)[0]>par.MaxZPT) return 0;
   */

   return 1;
}

// ======= Define mixed event matching criteria
bool matching(ZHadronMessenger *a, ZHadronMessenger *b, double shift) {
   if (a->SignalHF < shift * 1.04 && b->SignalHF < shift * 1.04) return 1;
   if ((b->SignalHF / (a->SignalHF - shift)) < 1.04 && b->SignalHF / (a->SignalHF - shift) > 0.96) return 1;
   return 0;
}


// ====== add UE event from EPOS to hard event
vector<int>* addUEParticles(ZHadronMessenger *hard, ZHadronMessenger *ue) {
   // returns a mask vector hard event = 0, UE event = 1

   vector<int>* UEmask = new vector<int>(hard->trackPt->size(), 0);

   for (unsigned int i = 0; i < ue->trackPt->size(); i++) {
      hard->trackPt->push_back(ue->trackPt->at(i));
      hard->trackEta->push_back(ue->trackEta->at(i));
      hard->trackY->push_back(ue->trackY->at(i));
      hard->trackPhi->push_back(ue->trackPhi->at(i));
      if (hard->trackMuTagged != nullptr) {
         bool muTagged = (ue->trackMuTagged != nullptr && i < ue->trackMuTagged->size())
            ? ue->trackMuTagged->at(i) : false;
         hard->trackMuTagged->push_back(muTagged);
      }
      if (hard->trackMuDR != nullptr) {
         float trackMuDR = (ue->trackMuDR != nullptr && i < ue->trackMuDR->size())
            ? ue->trackMuDR->at(i) : -1;
         hard->trackMuDR->push_back(trackMuDR);
      }
      hard->trackWeight->push_back(ue->trackWeight->at(i));
      hard->trackResidualWeight->push_back(ue->trackResidualWeight->at(i));
      UEmask->push_back(1);
   }
   return UEmask;
}

bool isSameZpTBin(float zPt_sig, float zPt_bkg) {
   if (zPt_sig < 10 && zPt_bkg < 10)         return true;
   if (zPt_sig >= 10 && zPt_sig < 20 &&
         zPt_bkg >= 10 && zPt_bkg < 20)      return true;
   if (zPt_sig >= 20 && zPt_sig < 40 &&
         zPt_bkg >= 20 && zPt_bkg < 40)      return true;
   if (zPt_sig >= 40 && zPt_bkg >= 40)       return true;

   return false;
}

float getMultiplicity(ZHadronMessenger *b, const Parameters& par, TrackResidualCorrector *corrector) {
   float mult = 0;
   pair<int, int> closestMuonTracks = findClosestMuonTracks(b, par);
   for (unsigned long j = 0; j < b->trackPt->size(); j++) {
      if (!trackSelection(b, par, j, closestMuonTracks)) continue;
      float trackWeight = (*b->trackWeight)[j];
      mult += trackWeight;
   }
   return mult;
}

double* getLogBins(float min, float max, int nBins) {
   double* bins = new double[nBins + 1];
   double logMin = log10(min);
   double logMax = log10(max);
   double logBinWidth = (logMax - logMin) / nBins;

   for (int i = 0; i <= nBins; i++) {
      bins[i] = pow(10, logMin + i * logBinWidth);
   }

   return bins;
}

double* getLinBins(float min, float max, int nBins) {
   double* bins = new double[nBins + 1];
   double binWidth = (max - min) / nBins;

   for (int i = 0; i <= nBins; i++) {
      bins[i] = min + i * binWidth;
   }

   return bins;
}
 

//============================================================//
// Z hadron dphi calculation
//============================================================//
double getDphi(ZHadronMessenger *MZSignal, ZHadronMessenger *MMix,
               ZHadronMessenger *MZUE,
               TH2D *h, TH2D *hSub0, TH3D *hTrkPtEtaPhi, TH3D* hZPtEtaPhi,
               TH1D* hVZ, TH1D* hZmass, TH3D* hTrkResidualCorrectionPtEtaPhi,
               TH2D* hDeltaRMuTrk, TH2D* hDeltaRZGenReco,
               const Parameters& par, TNtuple *nt = 0) {
   double nZ = 0;
   h->Sumw2();
   if (hTrkPtEtaPhi != 0) hTrkPtEtaPhi->Sumw2();
   if (hZPtEtaPhi != 0) hZPtEtaPhi->Sumw2();
   if (hZmass != 0) hZmass->Sumw2();
   if (hTrkResidualCorrectionPtEtaPhi != 0) hTrkResidualCorrectionPtEtaPhi->Sumw2();
   if (hDeltaRMuTrk != 0) hDeltaRMuTrk->Sumw2();
   if (hDeltaRZGenReco != 0) hDeltaRZGenReco->Sumw2();
   par.printParameters();
   unsigned long nEntry = MZSignal->GetEntries() * par.scaleFactor;
   unsigned long iStart = nEntry * (par.nChunk - 1) / par.nThread;
   unsigned long iEnd = nEntry * par.nChunk / par.nThread;
   unsigned int targetMix = ((par.nMix - 1) * par.mix + 1);

   ProgressBar Bar(cout, iEnd - iStart);
   Bar.SetStyle(1);
   unsigned long mix_i = iStart;
   unsigned long mixstart_i = mix_i;
   int deltaI = (iEnd - iStart) / 100 + 1;
   float dPhi_threshold = M_PI / 2;

   // open Z correction if needed
   TrackResidualCorrector *Zcorrector;
   if (par.useZWeight && par.ZWeightFile != "") {
      Zcorrector = new TrackResidualCorrector(par.ZWeightFile.c_str());
   }

   // open track residual correctors if needed
   TrackResidualCorrector *corrector;
   TrackResidualCorrector *corrector_0_10;
   TrackResidualCorrector *corrector_10_20;
   TrackResidualCorrector *corrector_20_40;
   TrackResidualCorrector *corrector_40_500;
   if (par.useResidualWeight && par.residualWeightFile != "") {
      corrector_0_10   = new TrackResidualCorrector(Form("%s0-10.root",   par.residualWeightFile.c_str()));              
      corrector_10_20  = new TrackResidualCorrector(Form("%s10-20.root",  par.residualWeightFile.c_str()));
      corrector_20_40  = new TrackResidualCorrector(Form("%s20-40.root",  par.residualWeightFile.c_str()));
      corrector_40_500 = new TrackResidualCorrector(Form("%s40-500.root", par.residualWeightFile.c_str()));              
   }


   //==================================================//
   // loop over events
   //==================================================//
   for (unsigned long i = iStart; i < iEnd; i++) {
      MZSignal->GetEntry(i);

      if (i % deltaI == 0) {
         Bar.Update(i - iStart);
         Bar.Print();
      }

      // get UE from EPOS file if needed
      vector<int>* UEmaskSignal = nullptr;
      if (par.useEPOSFile) {
         MZUE->GetEntry(i);
         UEmaskSignal = addUEParticles(MZSignal, MZUE);
      }

      // Check if the event passes the selection criteria
      if (!eventSelection(MZSignal, par)) continue;
      //if (MZSignal->trackPt->size() < 1) continue;

      float zY = (par.isGenZ ? (*MZSignal->genZY)[0] : (*MZSignal->zY)[0]);
      float zPhi = (par.isGenZ ? (*MZSignal->genZPhi)[0] : (*MZSignal->zPhi)[0]);
      if (zPhi < 0) zPhi += 2 * M_PI;
      float zPt = (par.isGenZ ? (*MZSignal->genZPt)[0] : (*MZSignal->zPt)[0]);
      float zMass = (par.isGenZ ? (*MZSignal->genZMass)[0] : (*MZSignal->zMass)[0]);

      if (zPt < 10) corrector = corrector_0_10;
      else if (zPt >= 10 && zPt < 20) corrector = corrector_10_20;
      else if (zPt >= 20 && zPt < 40) corrector = corrector_20_40;
      else corrector = corrector_40_500;

      if (par.isGenZ) {
         if (!(MZSignal->genZPt->size() > 0)) continue;

         float zY_reco = (*MZSignal->zY)[0];
         float zPhi_reco = (*MZSignal->zPhi)[0];

         float deltaY = zY - zY_reco;
         float deltaPhi = zPhi - zPhi_reco;
         
         hDeltaRZGenReco->Fill(deltaY, deltaPhi);
      }

      //==================================================//
      // calculate event weights
      //==================================================//
      float ZWeight = (par.ZWeightFile != "") ? Zcorrector->GetCorrectionFactor(zPt, zY, zPhi) : 1;

      float eventWeightSignal = (par.useEventWeight ? MZSignal->EventWeight * MZSignal->VZWeight : 1);
      if (par.useZWeight) eventWeightSignal *= ZWeight;

      float eventWeightSignalUE = (par.useEventWeight ? MZSignal->EventWeight * MZSignal->VZWeight : 1);
      if (par.useEPOSFile) eventWeightSignalUE *= 1; //MZUE->VZWeight; // * MZUE->EventWeight;
      if (par.useZWeight) eventWeightSignalUE *= ZWeight;

      if (hZPtEtaPhi != 0) hZPtEtaPhi->Fill(zPt, zY, zPhi, eventWeightSignal);
      if (hVZ != 0) hVZ->Fill(MZSignal->VZ, eventWeightSignal);
      if (hZmass != 0) hZmass->Fill(zMass);

      //==================================================//
      // boost to CM frame
      //==================================================//
      if (par.yBoost != 0) {
         if (par.isPPb) zY = zY - par.yBoost;
         else zY = -(zY + par.yBoost);
      }

      //==================================================//
      // track residual check
      //==================================================//
      // get track residual corrector
      // correcting MMix or MSignal particles here, we force both Zs in mixed events to be in the same track residual correction Z bin though, so we just use the sig Z here
      pair<int, int> closestMuonTracks = findClosestMuonTracks(MZSignal, par);
      for (unsigned long j = 0; j < MZSignal->trackPhi->size(); j++) {
         if (!trackSelection(MZSignal, par, j, closestMuonTracks)) continue;
         float trackPhi  = (*MZSignal->trackPhi)[j];
         if (trackPhi<0) trackPhi+= 2 * M_PI;
         float trackEta  = (*MZSignal->trackEta)[j];
         float trackPt   = (*MZSignal->trackPt)[j];
         float residualCorrection = ((par.residualWeightFile=="")||par.isGenZ==1)? 1 : corrector->GetCorrectionFactor(trackPt, trackEta, trackPhi);
         float weight = (par.useEPOSFile && UEmaskSignal->at(j)==1) ? eventWeightSignalUE : eventWeightSignal;
         //weight*= MZSignal->ExtraZWeight[par.ExtraZWeight];
         weight*= (*MZSignal->trackWeight)[j];
         if (par.useResidualWeight) weight*= residualCorrection;  
         if (hTrkPtEtaPhi != 0) hTrkPtEtaPhi->Fill(trackPt, trackEta, trackPhi, weight);
      }

      nZ += eventWeightSignal;

      if ((par.isGenZ ? (*MZSignal->genMuEta1).size() : (*MZSignal->muEta1).size()) == 0) continue;

      float muEta1 = (par.isGenZ ? (*MZSignal->genMuEta1)[0] : (*MZSignal->muEta1)[0]);
      float muPhi1 = (par.isGenZ ? (*MZSignal->genMuPhi1)[0] : (*MZSignal->muPhi1)[0]);
      float muPt1 = (par.isGenZ ? (*MZSignal->genMuPt1)[0] : (*MZSignal->muPt1)[0]);

      float muEta2 = (par.isGenZ ? (*MZSignal->genMuEta2)[0] : (*MZSignal->muEta2)[0]);
      float muPhi2 = (par.isGenZ ? (*MZSignal->genMuPhi2)[0] : (*MZSignal->muPhi2)[0]);
      float muPt2 = (par.isGenZ ? (*MZSignal->genMuPt2)[0] : (*MZSignal->muPt2)[0]);

      //==================================================//
      // loop over tracks
      //==================================================//
      for (unsigned long k = 0; k < MZSignal->trackPhi->size(); k++) {
         if (!trackStudySelection(MZSignal, par, k, closestMuonTracks)) continue;

         float trackEta = (*MZSignal->trackEta)[k];
         float trackPhi = (*MZSignal->trackPhi)[k];

         if (muPt1 > 20 && abs(muEta1) < 2.4) {
            float deltaEta1 = trackEta - muEta1;
            float deltaPhi1 = trackPhi - muPhi1;
            hDeltaRMuTrk->Fill(deltaEta1, deltaPhi1);
         }

         if (muPt2 > 20 && abs(muEta2) < 2.4) {
            float deltaEta2 = trackEta - muEta2;
            float deltaPhi2 = trackPhi - muPhi2;
            hDeltaRMuTrk->Fill(deltaEta2, deltaPhi2);
         }
      
      } // end track loop

   } // end event loop

   cout<<endl;
   cout<<"Total number of Zs: "<<nZ<<endl;
   return nZ;

}

class DataAnalyzer {
public:
   TFile *inf, *mixFile, *mixFileClone, *outf, *infUE;
   TNtuple *ntDiagnose;
   TH1D *hNZ, *hNZMix, *hVZ;
   TH3D *hTrkPtEtaPhi;
   TH3D *hZPtEtaPhi;
   TH1D *hZmass;
   TH2D *h = 0, *hSub0 = 0, *hMix = 0;
   TH2D *hDeltaRMuTrk = 0;
   TH2D *hDeltaRZGenReco = 0;
   ZHadronMessenger *MZHadron, *MMix, *MZHadronUE;
   string title;
   TH3D* hTrkResidualCorrectionPtEtaPhi;

   DataAnalyzer(const char* filename, const char* mixFilename, const char* outFilename, const char* filenameUE, const char* mytitle = "Data", bool useEPOSFile = false) :
      inf(new TFile(filename)),
      MZHadron(new ZHadronMessenger(*inf, string("Tree"))),
      mixFile(new TFile(mixFilename)),
      mixFileClone(new TFile(mixFilename)),
      MMix(new ZHadronMessenger(*mixFile, string("Tree"))),
      title(mytitle),
      outf(new TFile(outFilename, "recreate")) {
      if (useEPOSFile) {
         infUE = new TFile(filenameUE);
         MZHadronUE = new ZHadronMessenger(*infUE, string("Tree"));
      }
      outf->cd();
   }

   ~DataAnalyzer() {
      deleteHistograms();
      inf->Close();
      mixFile->Close();
      mixFileClone->Close();
      outf->Close(); 
      delete MZHadron;
      delete MMix;
   }

   void analyze(Parameters& par) {

      //==================================================//
      // First histogram with mix=false
      //==================================================//
      outf->cd();
      par.mix = false;

      // Z residual correction histogram =============================
      const int nbinsX_z = 50;
      const double xMin_z = 0.5;
      const double xMax_z = 100;
      std::vector<double> binEdgesX_z(nbinsX_z + 1);
      
      for (int i = 0; i <= nbinsX_z; ++i) binEdgesX_z[i] = xMin_z * std::pow(xMax_z / xMin_z, double(i) / nbinsX_z);
      binEdgesX_z[nbinsX_z]=350;

      const int nbinsY_z = 25;
      const double yMin_z = -2.4;
      const double yMax_z = 2.4;
      std::vector<double> binEdgesY_z(nbinsY_z + 1);

      for (int i = 0; i <= nbinsY_z; ++i) binEdgesY_z[i] = yMin_z + (yMax_z - yMin_z) * i / nbinsY_z;

      const int nbinsZ_z = 50;
      const double zMin_z = 0;
      const double zMax_z = 2 * M_PI;
      std::vector<double> binEdgesZ_z(nbinsZ_z + 1);

      for (int i = 0; i <= nbinsZ_z; ++i) binEdgesZ_z[i] = zMin_z + (zMax_z - zMin_z) * i / nbinsZ_z;

      hZPtEtaPhi = new TH3D("hZPtEtaPhiData", "Histogram Title; p_{T} (GeV/c); #eta; #phi",
                     nbinsX_z, &binEdgesX_z[0],
                     nbinsY_z, &binEdgesY_z[0],
                     nbinsZ_z, &binEdgesZ_z[0]);

      // track residual correction histogram =============================
      const int nbinsX = 25;
      const double xMin = 0.5;
      const double xMax = 10;
      std::vector<double> binEdgesX(nbinsX + 1);
      

      for (int i = 0; i <= nbinsX; ++i) binEdgesX[i] = xMin * std::pow(xMax / xMin, double(i) / nbinsX);
      binEdgesX[nbinsX]=200;

      const int nbinsY = 50;
      const double yMin = -2.4;
      const double yMax = 2.4;
      std::vector<double> binEdgesY(nbinsY + 1);

      for (int i = 0; i <= nbinsY; ++i) binEdgesY[i] = yMin + (yMax - yMin) * i / nbinsY;

      const int nbinsZ = 50;
      const double zMin = 0;
      const double zMax = 2 * M_PI;
      std::vector<double> binEdgesZ(nbinsZ + 1);

      for (int i = 0; i <= nbinsZ; ++i) binEdgesZ[i] = zMin + (zMax - zMin) * i / nbinsZ;

      hTrkPtEtaPhi = new TH3D("hTrkPtEtaPhiData", "Histogram Title; p_{T} (GeV/c); #eta; #phi",
                     nbinsX, &binEdgesX[0],
                     nbinsY, &binEdgesY[0],
                     nbinsZ, &binEdgesZ[0]);

      // =============================

      hTrkResidualCorrectionPtEtaPhi = new TH3D(Form("hTrkResidualCorrectionPtEtaPhi%s", title.c_str()), "", nbinsX, &binEdgesX[0], nbinsY, &binEdgesY[0], nbinsZ, &binEdgesZ[0]);
      
      hVZ = new TH1D(Form("hVZ%s", title.c_str()), "", 40, -20, 20);
      hZmass = new TH1D(Form("hZmass%s", title.c_str()), "", 40, 60, 120);
      hDeltaRMuTrk = new TH2D(Form("hDeltaRMuTrk%s", title.c_str()), "", 100, -0.01, 0.01, 100, -0.01, 0.01);
      hDeltaRZGenReco = new TH2D(Form("hDeltaRZGenReco%s", title.c_str()), "", 100, -0.5, 0.5, 100, -1, 1);
      
      h = new TH2D(Form("h%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hSub0 = new TH2D(Form("hSub0%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hNZ = new TH1D(Form("hNZ%s", title.c_str()), "", 1, 0, 1);
      hNZ->SetBinContent(1, getDphi(MZHadron, MMix, MZHadronUE, h, hSub0, hTrkPtEtaPhi, hZPtEtaPhi, hVZ, hZmass, hTrkResidualCorrectionPtEtaPhi, hDeltaRMuTrk, hDeltaRZGenReco, par)); // Dphi analysis
      
   }

   void writeHistograms(TFile* outf) {
      outf->cd();
      smartWrite(h);
      //smartWrite(hSub0);
      smartWrite(hMix);
      smartWrite(hNZ);
      smartWrite(hNZMix);
      smartWrite(ntDiagnose);
      smartWrite(hTrkPtEtaPhi);
      smartWrite(hZPtEtaPhi);
      smartWrite(hVZ);
      smartWrite(hZmass);
      smartWrite(hTrkResidualCorrectionPtEtaPhi);
      smartWrite(hDeltaRMuTrk);
      smartWrite(hDeltaRZGenReco);
   }

private:
   void deleteHistograms() {
      delete h, hSub0, hMix, hNZ, hNZMix, hTrkPtEtaPhi, hZPtEtaPhi, hVZ, hZmass, hTrkResidualCorrectionPtEtaPhi, hDeltaRMuTrk, hDeltaRZGenReco;
   }
};

//============================================================//
// Main analysis
//============================================================//
int main(int argc, char *argv[])
{
   if (printHelpMessage(argc, argv)) return 0;

   CommandLine CL(argc, argv);
   float MinZPT      = CL.GetDouble("MinZPT", 40);           // Minimum Z particle transverse momentum threshold for event selection.
   float MaxZPT      = CL.GetDouble("MaxZPT", 200);          // Maximum Z particle transverse momentum threshold for event selection.
   float MinTrackPT  = CL.GetDouble("MinTrackPT", 1);        // Minimum track transverse momentum threshold for track selection.
   float MaxTrackPT  = CL.GetDouble("MaxTrackPT", 2);        // Maximum track transverse momentum threshold for track selection.
   bool  IsData      = CL.GetBool  ("IsData", false);        // Determines whether the analysis is being run on actual data.
   bool  IsPP        = CL.GetBool  ("IsPP", true);          // Flag to indicate if the analysis is for Proton-Proton collisions.
   bool  IsJewel     = CL.GetBool  ("IsJewel", false);       // Flag to indicate if the analysis is for Jewel since the hole for Jewel is not hadronized

   Parameters par(MinZPT, MaxZPT, MinTrackPT, MaxTrackPT);
   par.input         = CL.Get      ("Input",   "mergedSample/HISingleMuon-v5.root");         // Input file
   par.mixFile       = CL.Get      ("MixFile", "mergedSample/HISingleMuon-v5.root");         // Input Mix file
   par.EPOSFile      = CL.Get      ("EPOSFile", "");                                         // EPOS sample, to add UE/background to GenMC hard processes, leave blank if no embeding needed
   par.useEPOSFile   = !par.EPOSFile.empty();
   par.output        = CL.Get      ("Output",  "output.root");                               // Output file
   par.isSelfMixing  = CL.GetBool  ("IsSelfMixing", true);   // Determine if the analysis is self-mixing
   par.isGenZ        = CL.GetBool  ("IsGenZ", false);        // Determine if the analysis is using Gen level Z     
   par.isPUReject    = CL.GetBool  ("IsPUReject", false);    // Flag to reject PU sample for systemaitcs.
   par.isMuTagged    = CL.GetBool  ("IsMuTagged", true);     // Default is true
   par.TrackMuDR     = CL.GetDouble("TrackMuDR", -1);
   par.TrackMuClosest = CL.GetBool ("TrackMuClosest", false);
   par.useTrackWeight   = CL.GetBool  ("UseTrackWeight", true);     // Default is true
   par.useEventWeight   = CL.GetBool  ("UseEventWeight", false);     // Default is false
   par.useZWeight       = CL.GetBool  ("UseZWeight", false);         // Default is false
   par.ZWeightFile     = CL.Get      ("ZWeightFile", "");           // Z weight file
   par.useResidualWeight = CL.GetBool  ("UseResidualWeight", false); // Default is false
   par.residualWeightFile = CL.Get      ("ResidualWeightFile", "");       // Residual weight file
   par.scaleFactor   = CL.GetDouble("Fraction", 1.00);       // Fraction of event processed in the sample
   par.nThread       = CL.GetInt   ("nThread", 1);           // The number of threads to be used for parallel processing.
   par.nChunk        = CL.GetInt   ("nChunk", 1);            // Specifies which chunk (segment) of the data to process, used in parallel processing.
   par.nMix          = CL.GetInt   ("nMix", 10);             // Number of mixed events to be considered in the analysis.
   par.shift         = CL.GetDouble("Shift", 971.74);        // Shift of sumHF in MB matching
   par.MinZY         = CL.GetDouble("MinZY", 0);             // Minimum Z particle rapidity threshold for event selection.
   par.MaxZY         = CL.GetDouble("MaxZY", 200);           // Maximum Z particle rapidity threshold for event selection.
   par.ExtraZWeight  = CL.GetInt   ("ExtraZWeight",-1);      // Do Muon systematics, -1 means no extraweight.
   par.includeHole   = CL.GetBool  ("includeHole",true);     // Include hole particle or not
   par.isPPb         = CL.GetBool  ("IsPPb", false);         // Flag to indicate if p is going in the positive eta direction.
   par.yBoost       = CL.GetDouble("yBoost", 0);         // Rapidity boost for pPb analysis
   par.mix = 0;
   par.isPP = IsPP;
   par.isData = IsData;
   par.isJewel = IsJewel;
   
   if (checkError(par)) return -1;
          
   // Analyze Data
   DataAnalyzer analyzer(par.input.c_str(), par.mixFile.c_str(), par.output.c_str(), par.EPOSFile.c_str(), "Data", par.useEPOSFile);
   analyzer.analyze(par);
   analyzer.writeHistograms(analyzer.outf);
   saveParametersToHistograms(par, analyzer.outf);
   cout << "done!" << analyzer.outf->GetName() << endl;
}
