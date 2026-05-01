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

   if (par.useVZWeight && par.VZWeightFile == "") {
      std::cout << "Error! UseVZWeight=true requires an explicit external VZWeightFile." << std::endl;
      return true;
   }

   if (!par.useVZWeight && par.VZWeightFile != "") {
      std::cout << "Error! VZWeightFile was provided but UseVZWeight=false. Pass both explicitly for VZ weighting." << std::endl;
      return true;
   }

   return false;    // No errors found
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

//======= eventSelection =====================================//
// Check if the event mass eventSelection criteria
// MinZPT < zPt < MaxZPT
//============================================================//
bool eventSelection(ZHadronMessenger *b, const Parameters& par) {
   if (par.isPUReject && par.isData && b->NVertex != 1) return 0;
   if (par.useVZWindow && fabs(b->VZ) >= 15) return 0;

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
      hard->trackMuTagged->push_back(ue->trackMuTagged->at(i));
      hard->trackMuDR->push_back(ue->trackMuDR->at(i));
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

int getZpTBin(float zPt) {
   if (zPt < 10) return 0;
   if (zPt < 20) return 1;
   if (zPt < 40) return 2;
   return 3;
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
 
struct JackknifeEventContribution {
   double SignalNZ = 0;
   double MixNZ = 0;
   vector<float> SignalBins;
   vector<float> MixBins;

   JackknifeEventContribution() = default;
   JackknifeEventContribution(int nBins) : SignalBins(nBins, 0), MixBins(nBins, 0) {}
};

int getRegularBinIndex(const TH2D *h, double x, double y)
{
   if(h == nullptr)
      return -1;

   int xBin = h->GetXaxis()->FindBin(x);
   int yBin = h->GetYaxis()->FindBin(y);

   if(xBin < 1 || xBin > h->GetNbinsX())
      return -1;
   if(yBin < 1 || yBin > h->GetNbinsY())
      return -1;

   return (xBin - 1) * h->GetNbinsY() + (yBin - 1);
}

void accumulateEventContribution(const TH2D *h, vector<float> &bins, double x, double y, double weight)
{
   int index = getRegularBinIndex(h, x, y);
   if(index < 0 || index >= (int)bins.size())
      return;

   bins[index] += weight;
}

void accumulateReflectedEventContribution(const TH2D *h, vector<float> &bins,
   double deta, double dphi, double dphi2, double weight)
{
   accumulateEventContribution(h, bins, deta, dphi, weight);
   accumulateEventContribution(h, bins, -deta, dphi, weight);
   accumulateEventContribution(h, bins, deta, dphi2, weight);
   accumulateEventContribution(h, bins, -deta, dphi2, weight);
}


//============================================================//
// Z hadron dphi calculation
//============================================================//
double getDphi(ZHadronMessenger *MZSignal, ZHadronMessenger *MMix,
               ZHadronMessenger *MZUE,
               TH2D *h, TH2D *hSub0, TH3D *hTrkPtEtaPhi, TH3D* hZPtEtaPhi,
               TH1D* hVZ, TH1D* hZmass, TH3D* hTrkResidualCorrectionPtEtaPhi,
               const Parameters& par, TNtuple *nt = 0,
               vector<JackknifeEventContribution> *jackknifeEvents = nullptr) {
   double nZ = 0;
   h->Sumw2();
   if (hTrkPtEtaPhi != 0) hTrkPtEtaPhi->Sumw2();
   if (hZPtEtaPhi != 0) hZPtEtaPhi->Sumw2();
   if (hZmass != 0) hZmass->Sumw2();
   if (hTrkResidualCorrectionPtEtaPhi != 0) hTrkResidualCorrectionPtEtaPhi->Sumw2();
   par.printParameters();
   unsigned long nEntry = MZSignal->GetEntries() * par.scaleFactor;
   unsigned long iStart = nEntry * (par.nChunk - 1) / par.nThread;
   unsigned long iEnd = nEntry * par.nChunk / par.nThread;
   unsigned int targetMix = ((par.nMix - 1) * par.mix + 1);
   targetMix = (targetMix < 1) ? 1 : targetMix; // ensure targetMix is at least 1

   ProgressBar Bar(cout, iEnd - iStart);
   Bar.SetStyle(1);
   unsigned long mix_i = iStart;
   unsigned long mixstart_i = mix_i;
   int deltaI = (iEnd - iStart) / 100 + 1;
   float dPhi_threshold = M_PI / 2;
   size_t jackknifeIndex = 0;
   int jackknifeBinCount = h->GetNbinsX() * h->GetNbinsY();

   // open pp energy extrapolation file if needed
   TrackResidualCorrector *EnergyCorrector;
   if (par.EnergyExtraFile != "") {
      EnergyCorrector = new TrackResidualCorrector(par.EnergyExtraFile.c_str());
   }

   // open Z correction if needed
   TrackResidualCorrector *Zcorrector;
   if (par.useZWeight && par.ZWeightFile != "") {
      Zcorrector = new TrackResidualCorrector(par.ZWeightFile.c_str());
   }

   // open VZ correction if needed
   VZCorrector *vzCorrector = nullptr;
   if (par.useVZWeight) {
      vzCorrector = new VZCorrector(par.VZWeightFile.c_str());
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

   // Optional fast-mixing metadata cache:
   // keep event-matching logic the same but avoid repeated eventSelection scans in the inner loop.
   vector<char> mixEventPass;
   vector<char> mixEventZPtBin;
   bool useFastMixingCache = (par.mix && par.useFastMixing && !par.useEPOSFile);
   if (useFastMixingCache) {
      unsigned long nMixEntry = MMix->GetEntries();
      mixEventPass.assign(nMixEntry, 0);
      mixEventZPtBin.assign(nMixEntry, -1);
      for (unsigned long m = 0; m < nMixEntry; m++) {
         MMix->GetEntry(m);
         if (!eventSelection(MMix, par)) continue;
         float mixZPt = (par.isGenZ ? (*MMix->genZPt)[0] : (*MMix->zPt)[0]);
         mixEventPass[m] = 1;
         mixEventZPtBin[m] = getZpTBin(mixZPt);
      }
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

      JackknifeEventContribution *jackknifeEvent = nullptr;
      if (jackknifeEvents != nullptr) {
         if (par.mix == false) {
            jackknifeEvents->emplace_back(jackknifeBinCount);
            jackknifeEvent = &jackknifeEvents->back();
         }
         else if (jackknifeIndex < jackknifeEvents->size()) {
            jackknifeEvent = &((*jackknifeEvents)[jackknifeIndex]);
         }
      }

      float zY = (par.isGenZ ? (*MZSignal->genZY)[0] : (*MZSignal->zY)[0]);
      float zPhi = (par.isGenZ ? (*MZSignal->genZPhi)[0] : (*MZSignal->zPhi)[0]);
      if (zPhi < 0) zPhi += 2 * M_PI;
      float zPt = (par.isGenZ ? (*MZSignal->genZPt)[0] : (*MZSignal->zPt)[0]);
      float zMass = (par.isGenZ ? (*MZSignal->genZMass)[0] : (*MZSignal->zMass)[0]);
      int zPtBin = getZpTBin(zPt);

      if (zPt < 10) corrector = corrector_0_10;
      else if (zPt >= 10 && zPt < 20) corrector = corrector_10_20;
      else if (zPt >= 20 && zPt < 40) corrector = corrector_20_40;
      else corrector = corrector_40_500;


      //==================================================//
      // calculate event weights
      //==================================================//
      float ZWeight = (par.ZWeightFile != "") ? Zcorrector->GetCorrectionFactor(zPt, zY, zPhi) : 1;
      if (par.ExtraZWeight >= 0) ZWeight *= MZSignal->ExtraZWeight[par.ExtraZWeight];
      else if (par.isData) ZWeight *= MZSignal->ZWeight;

      float eventWeightSignal = 1;
      if (par.useEventWeight) eventWeightSignal *= MZSignal->EventWeight;
      if (par.useVZWeight)
         eventWeightSignal *= vzCorrector->GetCorrectionFactor(MZSignal->VZ);
      if (par.useZWeight) eventWeightSignal *= ZWeight;

      // energy extrapolation
      //eta, phi dummy variables since the correction is only a function of Z pT
      if (par.EnergyExtraFile != "" && par.isPP) {
         float energyExtrapolationWeight = EnergyCorrector->GetCorrectionFactor(zPt, 1, 1);
         eventWeightSignal *= energyExtrapolationWeight;
      }

      if (jackknifeEvent != nullptr && par.mix == false)
         jackknifeEvent->SignalNZ = eventWeightSignal;

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
      pair<int, int> signalClosestMuonTracks = findClosestMuonTracks(MZSignal, par);
      for (unsigned long j = 0; j < MZSignal->trackPhi->size(); j++) {
         if (!trackSelection(MZSignal, par, j, signalClosestMuonTracks)) continue;
         float trackPhi  = (*MZSignal->trackPhi)[j];
         if (trackPhi<0) trackPhi+= 2 * M_PI;
         float trackEta  = (*MZSignal->trackEta)[j];
         float trackPt   = (*MZSignal->trackPt)[j];
         float residualCorrection = ((par.residualWeightFile=="")||par.isGenZ==1)? 1 : corrector->GetCorrectionFactor(trackPt, trackEta, trackPhi);
          float weight = eventWeightSignal;
         weight*= (*MZSignal->trackWeight)[j];
         if (par.useResidualWeight) weight*= residualCorrection;  
         if (hTrkPtEtaPhi != 0) hTrkPtEtaPhi->Fill(trackPt, trackEta, trackPhi, weight);
      }

      //==================================================//
      // loop over mixed events
      //==================================================//
      bool foundMix = false;
      for (unsigned int nMix = 0; nMix < targetMix; nMix++) {

         // get mix event
         foundMix = false;
         mixstart_i = mix_i;
         vector<int>* UEmaskMix = nullptr;
         if (par.mix) {
            while (!foundMix) {
               mix_i = (mix_i + 1);
               if (mix_i >= MMix->GetEntries()) mix_i = 0;
               if (mixstart_i == mix_i) break;

                if (useFastMixingCache) {
                   if (mixEventPass[mix_i] == 0) continue;
                   if (mixEventZPtBin[mix_i] != zPtBin) continue;
                   if (i != mix_i) foundMix = true;
                   continue;
               }

               MMix->GetEntry(mix_i);

               // add epos particles if needed
               if (par.useEPOSFile) {
                  MZUE->GetEntry(mix_i);
                  UEmaskMix = addUEParticles(MMix, MZUE);
               }

               // event selection
               if (!eventSelection(MMix, par)) continue;

               // only mix with Z events of similar Z pT
               float mix_zPt = (par.isGenZ ? (*MMix->genZPt)[0] : (*MMix->zPt)[0]); 
               if (!isSameZpTBin(zPt, mix_zPt)) continue;

               if (i != mix_i) foundMix = true;
            }
         }
         
         MMix->GetEntry(mix_i);

         if (par.mix && par.useEPOSFile) {
            MZUE->GetEntry(mix_i);
            UEmaskMix = addUEParticles(MMix, MZUE);
         }
         
         //==================================================//
         // calculate mix event weight
         //==================================================//
         float eventWeightMix = eventWeightSignal;
         if (par.mix) {
            float zPtMix = (par.isGenZ ? (*MMix->genZPt)[0] : (*MMix->zPt)[0]);
            float zYMix = (par.isGenZ ? (*MMix->genZY)[0] : (*MMix->zY)[0]);
            float zPhiMix = (par.isGenZ ? (*MMix->genZPhi)[0] : (*MMix->zPhi)[0]);
            if (zPhiMix < 0) zPhiMix += 2 * M_PI;
            float ZWeightMix = (par.ZWeightFile != "") ? Zcorrector->GetCorrectionFactor(zPtMix, zYMix, zPhiMix) : 1;
            if (par.ExtraZWeight >= 0) ZWeightMix *= MMix->ExtraZWeight[par.ExtraZWeight];
            else if (par.isData) ZWeightMix *= MMix->ZWeight;
            
            float computedMixWeight = 1;
            if (par.useEventWeight) computedMixWeight *= MMix->EventWeight;
            if (par.useVZWeight)
               computedMixWeight *= vzCorrector->GetCorrectionFactor(MMix->VZ);
            if (par.useZWeight) computedMixWeight *= ZWeightMix;

            // energy extrapolation
            if (par.EnergyExtraFile != "" && par.isPP) {
               float energyExtrapolationWeightMix = EnergyCorrector->GetCorrectionFactor(zPtMix, 1, 1);
               computedMixWeight *= energyExtrapolationWeightMix;
            }
            eventWeightMix = computedMixWeight;

         }

          nZ += (par.mix) ? eventWeightMix : eventWeightSignal;
          if (jackknifeEvent != nullptr && par.mix)
             jackknifeEvent->MixNZ += eventWeightMix;
          
          //==================================================//
          // loop over tracks
         //==================================================//
         ZHadronMessenger *currentEvent = (par.mix ? MMix : MZSignal);
         pair<int, int> currentClosestMuonTracks = findClosestMuonTracks(currentEvent, par);
         for (unsigned long j = 0; j < (par.mix ? MMix->trackPhi->size() : MZSignal->trackPhi->size()); j++) {

            // Check if the event passes the selection criteria
            if (!trackSelection(currentEvent, par, j, currentClosestMuonTracks)) continue;
            
            float trackDphi  = par.mix ? DeltaPhi((*MMix->trackPhi)[j], zPhi)
                                       : DeltaPhi((*MZSignal->trackPhi)[j], zPhi);
            float trackDphi2 = par.mix ? DeltaPhi(zPhi, (*MMix->trackPhi)[j])
                                       : DeltaPhi(zPhi, (*MZSignal->trackPhi)[j]);
            float trackDeta  = par.mix ? fabs((*MMix->trackEta)[j] - zY)
                                       : fabs((*MZSignal->trackEta)[j] - zY);

            float trackPhi  = par.mix ? (*MMix->trackPhi)[j] : (*MZSignal->trackPhi)[j];
            float trackEta  = par.mix ? (*MMix->trackEta)[j] : (*MZSignal->trackEta)[j];
            float trackPt   = par.mix ? (*MMix->trackPt)[j] : (*MZSignal->trackPt)[j];

            // boost will not affect trackDeta as track and Z boosted by same amount
            // boost nonetheless applied for Z and track-specific histograms
            if (par.yBoost != 0 && !par.isPP) {
               if (par.isPPb) trackEta = trackEta - par.yBoost;
               else trackEta = -(trackEta + par.yBoost);
            }

            //==================================================//
            // calculate total weight
            //==================================================//
            // event + VZ + Z residual weight
            float this_eventWeight = 1;
            if (par.mix) {
               this_eventWeight = eventWeightMix;
            } else {
               this_eventWeight = eventWeightSignal;
            }

            // track weight
            float this_trackWeight = 1;
            if(par.useTrackWeight) {
               if (par.mix) {
                  this_trackWeight *= (*MMix->trackWeight)[j];
               } else {
                  this_trackWeight *= (*MZSignal->trackWeight)[j];
               }
               this_trackWeight *= par.TrackExtraWeight;
            }
            
            // track residual weight
            float this_residualWeight = 1;
            if(par.useResidualWeight) {
               /*
               if (par.mix) {
                  this_residualWeight /= (*MMix->trackResidualWeight)[j];
               } else {
                  this_residualWeight /= (*MZSignal->trackResidualWeight)[j];
               }
               */
               if (par.residualWeightFile != "")
               {
                  this_residualWeight *= corrector->GetCorrectionFactor(trackPt, trackEta, trackPhi);
               }
            }
            if (hTrkResidualCorrectionPtEtaPhi != 0) hTrkResidualCorrectionPtEtaPhi->Fill(trackPt, trackEta, trackPhi, this_residualWeight);

            float weight = this_eventWeight * this_trackWeight * this_residualWeight;

            //==================================================//
            // fill central values
            //==================================================//
            h->Fill(trackDeta, trackDphi, weight);
            h->Fill(-trackDeta, trackDphi, weight);
            h->Fill(trackDeta, trackDphi2, weight);
            h->Fill(-trackDeta, trackDphi2, weight);

            if (jackknifeEvent != nullptr) {
               if (par.mix)
                  accumulateReflectedEventContribution(h, jackknifeEvent->MixBins, trackDeta, trackDphi, trackDphi2, weight);
               else
                  accumulateReflectedEventContribution(h, jackknifeEvent->SignalBins, trackDeta, trackDphi, trackDphi2, weight);
            }

          
         } // end track loop

      } // end mix event loop

      if (jackknifeEvents != nullptr)
         jackknifeIndex = jackknifeIndex + 1;

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
   ZHadronMessenger *MZHadron, *MMix, *MZHadronUE;
   string title;
   TH3D* hTrkResidualCorrectionPtEtaPhi;
   TTree *tJackknife2D = nullptr;
   vector<JackknifeEventContribution> jackknifeEvents;

   DataAnalyzer(const char* filename, const char* mixFilename, const char* outFilename, const char* filenameUE, const char* mytitle = "Data", bool useEPOSFile = false, const char* treeName = "Tree") :
      inf(new TFile(filename)),
      MZHadron(new ZHadronMessenger(*inf, string(treeName))),
      mixFile(new TFile(mixFilename)),
      mixFileClone(new TFile(mixFilename)),
      MMix(new ZHadronMessenger(*mixFile, string(treeName))),
      ntDiagnose(nullptr),
      title(mytitle),
      outf(new TFile(outFilename, "recreate")) {
      if (useEPOSFile) {
         infUE = new TFile(filenameUE);
         MZHadronUE = new ZHadronMessenger(*infUE, string(treeName));
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
      
      h = new TH2D(Form("h%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hSub0 = new TH2D(Form("hSub0%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hNZ = new TH1D(Form("hNZ%s", title.c_str()), "", 1, 0, 1);
      bool doJackknife = (par.useJackknife && par.isData);
      jackknifeEvents.clear();
      hNZ->SetBinContent(1, getDphi(MZHadron, MMix, MZHadronUE, h, hSub0, hTrkPtEtaPhi, hZPtEtaPhi, hVZ, hZmass, hTrkResidualCorrectionPtEtaPhi, par,
         0, doJackknife ? &jackknifeEvents : nullptr)); // Dphi analysis
      
      //==================================================//
      // Second histogram with mix=true
      //==================================================//
      
      par.mix = true;
      hMix = new TH2D(Form("hMix%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hNZMix = new TH1D(Form("hNZMix%s", title.c_str()), "", 1, 0, 1);

      if(par.nMix<1) return; // skip if nMix=0 (turned off)
      hNZMix->SetBinContent(1, getDphi(MZHadron, MMix, MZHadronUE, hMix, 0,0, 0, 0, 0, 0, par,
         ntDiagnose, doJackknife ? &jackknifeEvents : nullptr)); // Dphi analysis with mixing

      if (doJackknife && jackknifeEvents.size() > 0) {
         outf->cd();
         double signalNZ = 0;
         double mixNZ = 0;
         vector<float> signalBins;
         vector<float> mixBins;

         tJackknife2D = new TTree(Form("Jackknife2D%s", title.c_str()), "Per-event jackknife contributions");
         tJackknife2D->Branch("SignalNZ", &signalNZ);
         tJackknife2D->Branch("MixNZ", &mixNZ);
         tJackknife2D->Branch("SignalBins", &signalBins);
         tJackknife2D->Branch("MixBins", &mixBins);

         for (const JackknifeEventContribution &event : jackknifeEvents) {
            signalNZ = event.SignalNZ;
            mixNZ = event.MixNZ;
            signalBins = event.SignalBins;
            mixBins = event.MixBins;
            tJackknife2D->Fill();
         }
      }
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
      smartWrite(tJackknife2D);
   }

private:
   void deleteHistograms() {
      delete h;
      delete hSub0;
      delete hMix;
      delete hNZ;
      delete hNZMix;
      delete hTrkPtEtaPhi;
      delete hZPtEtaPhi;
      delete hVZ;
      delete hZmass;
      delete hTrkResidualCorrectionPtEtaPhi;
      delete tJackknife2D;
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
   par.TrackExtraWeight = CL.GetDouble("TrackExtraWeight", 1.0);
   par.useEventWeight   = CL.GetBool  ("UseEventWeight", false);     // Default is false
   par.useZWeight       = CL.GetBool  ("UseZWeight", false);         // Default is false
   par.ZWeightFile     = CL.Get      ("ZWeightFile", "");           // Z weight file
   par.useResidualWeight = CL.GetBool  ("UseResidualWeight", false); // Default is false
   par.residualWeightFile = CL.Get      ("ResidualWeightFile", "");       // Residual weight file
   par.EnergyExtraFile = CL.Get      ("EnergyExtraFile", "");
   par.VZWeightFile     = CL.Get      ("VZWeightFile", "");           // VZ weight file
   par.useVZWeight       = CL.GetBool  ("UseVZWeight", false);
   par.useVZWindow       = CL.GetBool  ("UseVZWindow", true);
   par.useFastMixing   = CL.GetBool  ("UseFastMixing", false);
   par.useJackknife    = CL.GetBool  ("UseJackknife", false);
   par.TrackSelectionMode = CL.Get      ("TrackSelectionMode", "Nominal");
   par.TrackTreeName   = "Tree";
   if (par.TrackSelectionMode == "Loose")   par.TrackTreeName = "TreeLoose";
   if (par.TrackSelectionMode == "Tight")   par.TrackTreeName = "TreeTight";
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
   DataAnalyzer analyzer(par.input.c_str(), par.mixFile.c_str(), par.output.c_str(), par.EPOSFile.c_str(), "Data", par.useEPOSFile, par.TrackTreeName.c_str());
   analyzer.analyze(par);
   analyzer.writeHistograms(analyzer.outf);
   saveParametersToHistograms(par, analyzer.outf);
   cout << "done!" << analyzer.outf->GetName() << endl;
}
