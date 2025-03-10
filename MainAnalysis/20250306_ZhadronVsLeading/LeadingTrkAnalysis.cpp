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

   if (par.isHiBinUp && par.isHiBinDown) {
      std::cout << "Error! Cannot do hiBinUp and hiBinDown simultaneously!" << std::endl;
      return true;  // Return true indicates an error was found
   }

   return false;    // No errors found
}

//======= trackSelection =====================================//
// Check if the track pass selection criteria
//============================================================//
bool trackSelection(ZHadronMessenger *b, Parameters par, int j) {
   if (par.isMuTagged && (*b->trackMuTagged)[j]) return false; 
   if ((*b->trackPt)[j] > par.MaxTrackPT) return false;  
   if ((*b->trackPt)[j] < par.MinTrackPT) return false;
   if ((!par.includeHole) && (*b->trackWeight)[j] < 0) return false;
   return true;
}

//======= trackSelection =====================================//
// Check if the track pass selection criteria
//============================================================//
bool trackSelectionNoPt(ZHadronMessenger *b, Parameters par, int j) {
   if (par.isMuTagged && (*b->trackMuTagged)[j]) return false; 
   if ((!par.includeHole) && (*b->trackWeight)[j] < 0) return false;
   return true;
}

//======= eventSelection =====================================//
// Check if the event mass eventSelection criteria
// MinZPT < zPt < MaxZPT
// MinHiBin , hiBin < MaxHiBin
//============================================================//
bool eventSelection(ZHadronMessenger *b, const Parameters& par) {
   int effectiveHiBin = par.isHiBinUp ? b->hiBinUp : (par.isHiBinDown ? b->hiBinDown : b->hiBin);   // The actual centrality bin used for centrality selection
   if (par.isPUReject && par.isPP && b->NVertex != 1) return 0;                                     // Only apply PU rejection (single vertex requirement) in pp analysis
   if (effectiveHiBin < par.MinHiBin) return 0;
   if (effectiveHiBin >= par.MaxHiBin) return 0;
   if ((par.isGenZ ? b->genZMass->size() : b->zMass->size()) == 0) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0]) < 60) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0]) > 120) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0])) <= par.MinZY) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0])) >= par.MaxZY) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0]) < par.MinZPT) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0]) > par.MaxZPT) return 0;

   return 1;
}

// ======= Define mixed event matching criteria
bool matching(ZHadronMessenger *a, ZHadronMessenger *b, double shift) {
   if (a->SignalHF < shift * 1.04 && b->SignalHF < shift * 1.04) return 1;
   if ((b->SignalHF / (a->SignalHF - shift)) < 1.04 && b->SignalHF / (a->SignalHF - shift) > 0.96) return 1;
   return 0;
}

//============================================================//
// Z hadron dphi calculation
//============================================================//
float getDphi(ZHadronMessenger *MZSignal, ZHadronMessenger *MMix, ZHadronMessenger *MMixEvt, TH2D *h, TH2D *hSub0, const Parameters& par, TNtuple *nt = 0) {
   float nZ = 0;
   h->Sumw2();
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
   float dPhi_threshold = 2 * M_PI / 3;

   TrackResidualCorrector *corrector;
   TrackResidualCorrector *corrector_0_20;
   TrackResidualCorrector *corrector_20_60;
   TrackResidualCorrector *corrector_60_100;
   TrackResidualCorrector *corrector_100_200;
   if (par.useResidualCor) {
      corrector_0_20    = new TrackResidualCorrector(Form("%s_0_20.root",    par.residualCor.c_str()));              
      corrector_20_60   = new TrackResidualCorrector(Form("%s_20_60.root",   par.residualCor.c_str()));              
      corrector_60_100  = new TrackResidualCorrector(Form("%s_60_100.root",  par.residualCor.c_str()));              
      corrector_100_200 = new TrackResidualCorrector(Form("%s_100_200.root", par.residualCor.c_str()));              
   }

   for (unsigned long i = iStart; i < iEnd; i++) {
      MZSignal->GetEntry(i);
      if (i % deltaI == 0) {
         Bar.Update(i - iStart);
         Bar.Print();
      }
      // Check if the event passes the selection criteria
      if (eventSelection(MZSignal, par)) {
         // Find a mixed akeevent
         float zY = (par.isGenZ ? (*MZSignal->genZY)[0] : (*MZSignal->zY)[0]);
         float zPhi = (par.isGenZ ? (*MZSignal->genZPhi)[0] : (*MZSignal->zPhi)[0]);
         float zPt = (par.isGenZ ? (*MZSignal->genZPt)[0] : (*MZSignal->zPt)[0]);

         if (par.useLeadingTrk) {
            float maxTrkPt = -1;
            long maxTrkIdx = -1;
            for (unsigned long j = 0; j < MZSignal->trackPhi->size(); j++) {
               if (!trackSelectionNoPt(MZSignal, par, j)) continue;
               if (DeltaPhi((*MZSignal->trackPhi)[j], zPhi) < dPhi_threshold) continue;
               if ((*MZSignal->trackPt)[j] > maxTrkPt) {
                  maxTrkPt = (*MZSignal->trackPt)[j];
                  maxTrkIdx = j;
               }
            }
            if (maxTrkIdx > -1 && maxTrkPt > 10) {
               // replace the Z position by leading track direction
               zY = (*MZSignal->trackEta)[maxTrkIdx];
               zPhi = (*MZSignal->trackPhi)[maxTrkIdx] + M_PI;
            }
         }   
         for (unsigned int nMix = 0; nMix < targetMix; nMix++) {
            bool foundMix = false;
            mixstart_i = mix_i;
            if (par.mix) {
               while (!foundMix) {
                  mix_i = (mix_i + 1);
                  if (mix_i >= MMixEvt->GetEntries()) mix_i = 0;
                  if (mixstart_i == mix_i) break;
                  MMixEvt->GetEntry(mix_i);
                  if (par.isSelfMixing) {
                     if (eventSelection(MMixEvt, par) && par.isSelfMixing && i != mix_i) foundMix = true;
                  } else {
                     if (matching(MZSignal, MMixEvt, par.shift) && !par.isSelfMixing) foundMix = true;
                  }
               }
            }
            if (!foundMix && par.mix) {
               cout << "Cannot find a mixed event!!! Event = " << i << " " << MZSignal->SignalHF << endl;
               break;
            }
            MMix->GetEntry(mix_i);
            nZ += ((par.mix && par.isSelfMixing) ? (MMix->ZWeight * MMix->EventWeight) * (MZSignal->ZWeight * MZSignal->EventWeight) : (MZSignal->ZWeight * MZSignal->EventWeight)) * (
                     (par.ExtraZWeight == -1) ? 1 : ((par.mix && par.isSelfMixing) ? MMix->ExtraZWeight[par.ExtraZWeight] * MZSignal->ExtraZWeight[par.ExtraZWeight] : MZSignal->ExtraZWeight[par.ExtraZWeight]));
            for (unsigned long j = 0; j < (par.mix ? MMix->trackPhi->size() : MZSignal->trackPhi->size()); j++) {
               if (!trackSelection((par.mix ? MMix : MZSignal), par, j)) continue;
               float trackDphi  = par.mix ? DeltaPhi((*MMix->trackPhi)[j], zPhi) : DeltaPhi((*MZSignal->trackPhi)[j], zPhi);
               float trackDphi2 = par.mix ? DeltaPhi(zPhi, (*MMix->trackPhi)[j]) : DeltaPhi(zPhi, (*MZSignal->trackPhi)[j]);
               float trackDeta  = par.mix ? fabs((*MMix->trackEta)[j] - zY) : fabs((*MZSignal->trackEta)[j] - zY);
               float weight = (par.mix && par.isSelfMixing) ? (MMix->ZWeight * MMix->EventWeight) * (MZSignal->ZWeight * MZSignal->EventWeight) : (MZSignal->ZWeight * MZSignal->EventWeight);
               weight *= (par.ExtraZWeight == -1) ? 1 : ((par.mix && par.isSelfMixing) ? MMix->ExtraZWeight[par.ExtraZWeight] * MZSignal->ExtraZWeight[par.ExtraZWeight] : MZSignal->ExtraZWeight[par.ExtraZWeight]);
               if (par.useResidualCor) {
                  float trackPhi  = par.mix ? (*MMix->trackPhi)[j] : (*MZSignal->trackPhi)[j];
                  float trackEta  = par.mix ? (*MMix->trackEta)[j] : (*MZSignal->trackEta)[j];
                  float trackPt   = par.mix ? (*MMix->trackPt)[j] : (*MZSignal->trackPt)[j];
                  float hiBin = par.mix ? MMix->hiBin : MZSignal->hiBin;
                  if (hiBin < 20) corrector = corrector_0_20;
                  else if (hiBin < 60) corrector = corrector_20_60;
                  else if (hiBin < 100) corrector = corrector_60_100;
                  else corrector = corrector_100_200;
                  float residualCorrection = corrector->GetCorrectionFactor(trackPt, trackEta, trackPhi);

                  weight *= (par.mix ? ((*MMix->trackWeight)[j] / (*MMix->trackResidualWeight)[j] * (1 - 0.33 * par.isJewel * ((*MMix->trackWeight)[j] < 0))) : ((*MZSignal->trackWeight)[j] / (*MZSignal->trackResidualWeight)[j] * (1 - 0.33 * par.isJewel * ((*MZSignal->trackWeight)[j] < 0))));
                  weight *= residualCorrection;
               } else {
                  weight *= (par.mix ? ((*MMix->trackWeight)[j] * (1 - 0.33 * par.isJewel * ((*MMix->trackWeight)[j] < 0))) : ((*MZSignal->trackWeight)[j] * (1 - 0.33 * par.isJewel * ((*MZSignal->trackWeight)[j] < 0))));
               }
               h->Fill(trackDeta, trackDphi, weight);
               h->Fill(-trackDeta, trackDphi, weight);
               h->Fill(trackDeta, trackDphi2, weight);
               h->Fill(-trackDeta, trackDphi2, weight);

               if (!par.mix && (*MZSignal->subevent)[j] == 0) {
                  hSub0->Fill(trackDeta, trackDphi, weight);
                  hSub0->Fill(-trackDeta, trackDphi, weight);
                  hSub0->Fill(trackDeta, trackDphi2, weight);
                  hSub0->Fill(-trackDeta, trackDphi2, weight);
               }
            }
         }
      }
   }
   return nZ;
}

//============================================================//
// leading track angular distribution wrt Z calculation
//============================================================//
void getLeadingVsZ(ZHadronMessenger *MZSignal, ZHadronMessenger *MMix, ZHadronMessenger *MMixEvt, vector<TH3D*> hLeadingVsZ, vector<TH1D*> hNZ, TH1D *hTrkPt, TH1D *hLeadingPt, TH1D *hZPt, TH1D *hZMass, float *ptbinlo, const Parameters& par, TNtuple *nt = 0) {

   float nZ = 0;
   hTrkPt->Sumw2();
   hLeadingPt->Sumw2();
   hZPt->Sumw2();
   hZMass->Sumw2();

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

   // event loop
   for (unsigned long i = iStart; i < iEnd; i++) {
      MZSignal->GetEntry(i);
      if (i % deltaI == 0) {
         Bar.Update(i - iStart);
         Bar.Print();
      }

      // Check if the event passes the selection criteria
      if (!eventSelection(MZSignal, par)) continue;

      // get the z info
      float zY = (par.isGenZ ? (*MZSignal->genZY)[0] : (*MZSignal->zY)[0]);
      float zPhi = (par.isGenZ ? (*MZSignal->genZPhi)[0] : (*MZSignal->zPhi)[0]);
      float zPt = (par.isGenZ ? (*MZSignal->genZPt)[0] : (*MZSignal->zPt)[0]);
      float zMass = (par.isGenZ ? (*MZSignal->genZMass)[0] : (*MZSignal->zMass)[0]);

      hZPt->Fill(zPt);
      hZMass->Fill(zMass);

      // sum over parts in the mixed (resp signal) events
      float maxTrkPt = -1;
      int maxTrkIdx = -1;
      for (unsigned long j = 0; j < MZSignal->trackPt->size(); j++) {

         // Check if the track passes the selection criteria
         if (!trackSelection(MZSignal, par, j)) continue;

         float trackPt = (*MZSignal->trackPt)[j];
         hTrkPt->Fill(trackPt);

         if (trackPt > maxTrkPt) {
            maxTrkPt = trackPt;
            maxTrkIdx = j;
         }

      }

      // do calculations with the leading track and Z
      hLeadingPt->Fill(maxTrkPt);

      float trackDphi  = par.mix ? DeltaPhi((*MMix->trackPhi)[maxTrkIdx], zPhi) : DeltaPhi((*MZSignal->trackPhi)[maxTrkIdx], zPhi);
      float trackDphi2 = par.mix ? DeltaPhi(zPhi, (*MMix->trackPhi)[maxTrkIdx]) : DeltaPhi(zPhi, (*MZSignal->trackPhi)[maxTrkIdx]);
      float trackDeta  = par.mix ? fabs((*MMix->trackEta)[maxTrkIdx] - zY) : fabs((*MZSignal->trackEta)[maxTrkIdx] - zY);
      float trackDr = sqrt(trackDeta * trackDeta + (trackDphi - M_PI) * (trackDphi - M_PI));

      for (int i = 0; i<hLeadingVsZ.size(); i++) {
         if (maxTrkPt > ptbinlo[i]) {
            hLeadingVsZ[i]->Fill(trackDeta, trackDphi, trackDr);
            hLeadingVsZ[i]->Fill(-trackDeta, trackDphi, trackDr);
            hLeadingVsZ[i]->Fill(trackDeta, trackDphi2, trackDr);
            hLeadingVsZ[i]->Fill(-trackDeta, trackDphi2, trackDr);

            hNZ[i]->Fill(0.5);
         }
      }

   }
   
}

class DataAnalyzer {
public:
   TFile *inf, *mixFile, *mixFileClone, *outf;
   TNtuple *ntDiagnose;
   TH1D *hTrkPt = 0, *hLeadingPt = 0, *hZPt = 0, *hZMass = 0;
   vector<TH3D*> hLeadingVsZ;
   vector<TH1D*> hNZ;
   ZHadronMessenger *MZHadron, *MMix, *MMixEvt;
   string title;

   DataAnalyzer(const char* filename, const char* mixFilename, const char* outFilename, const char* mytitle = "Data") :
      inf(new TFile(filename)), MZHadron(new ZHadronMessenger(*inf, string("Tree"))), mixFile(new TFile(mixFilename)), mixFileClone(new TFile(mixFilename)), MMix(new ZHadronMessenger(*mixFile, string("Tree"))), MMixEvt(new ZHadronMessenger(*mixFileClone, string("Tree"), true)), title(mytitle), outf(new TFile(outFilename, "recreate")) {
      outf->cd();
      ntDiagnose = new TNtuple("ntDiagnose", "", "zPt:nTrk:hiBin:SignalHF:nTrkMix:hiBinMix:SignalHFMix:nMix");     
   }

   ~DataAnalyzer() {
      deleteHistograms();
      inf->Close();
      mixFile->Close();
      mixFileClone->Close();
      outf->Close(); 
      delete MZHadron;
      delete MMix;
      delete MMixEvt;
   }

   void analyze(Parameters& par) {
      // First histogram with mix=false
      outf->cd();
      par.mix = false;

      int nptbins = 4;
      float ptbinlo[nptbins] = {4, 5, 6, 10};
      for (int i = 0; i < nptbins; i++) {
         TH3D* this_hLeadingVsZ = new TH3D(Form("hLeadingVsZ%s_%i", title.c_str(), static_cast<int>(ptbinlo[i])), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2, 20, -4, 4); // 2D: (deta, dphi, dr)
         hLeadingVsZ.push_back(this_hLeadingVsZ);

         TH1D* this_hNZ = new TH1D(Form("hNZ%s_%i", title.c_str(), static_cast<int>(ptbinlo[i])), "", 1, 0, 1);
         hNZ.push_back(this_hNZ);
      }

      hTrkPt = new TH1D(Form("hTrkPt%s", title.c_str()), "", 40, 0, 20);
      hLeadingPt = new TH1D(Form("hLeadingPt%s", title.c_str()), "", 40, 0, 20);
      hZPt = new TH1D(Form("hZPt%s", title.c_str()), "", 40, 40, 200);
      hZMass = new TH1D(Form("hZMass%s", title.c_str()), "", 40, 60, 120);
      
      getLeadingVsZ(MZHadron, MMix, MMixEvt, hLeadingVsZ, hNZ, hTrkPt, hLeadingPt, hZPt, hZMass, ptbinlo, par); // analysis

      // Second histogram with mix=true
      /*
      par.mix = true;
      hMix = new TH2D(Form("hMix%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hNZMix = new TH1D(Form("hNZMix%s", title.c_str()), "", 1, 0, 1);
      hNZMix->SetBinContent(1, getDphi(MZHadron, MMix, MMixEvt, hMix, 0, par, ntDiagnose)); // Dphi analysis with mixing
      */
   }

   void writeHistograms(TFile* outf) {
      outf->cd();
      smartWrite(hTrkPt);
      smartWrite(hLeadingPt);
      smartWrite(hZPt);
      smartWrite(hZMass);

      for (int i=0; i<hLeadingVsZ.size(); i++) {
         smartWrite(hLeadingVsZ[i]);
         smartWrite(hNZ[i]);
      }
      
   }

private:
   void deleteHistograms() {
      delete hTrkPt, hLeadingPt, hZPt, hZMass, hLeadingVsZ, hNZ;
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
   float MaxTrackPT  = CL.GetDouble("MaxTrackPT", 20);        // Maximum track transverse momentum threshold for track selection.
   int   MinHiBin    = CL.GetInt   ("MinHiBin", 0);          // Minimum hiBin value for event selection.
   int   MaxHiBin    = CL.GetInt   ("MaxHiBin", 200);        // Maximum hiBin value for event selection.
   bool  IsData      = CL.GetBool  ("IsData", false);        // Determines whether the analysis is being run on actual data.
   bool  IsPP        = CL.GetBool  ("IsPP", false);          // Flag to indicate if the analysis is for Proton-Proton collisions.
   bool  IsJewel     = CL.GetBool  ("IsJewel", false);       // Flag to indicate if the analysis is for Jewel since the hole for Jewel is not hadronized

   if (IsPP) {                                                                         
      MinHiBin=-2;
      MaxHiBin=0;
   }

   Parameters par(MinZPT, MaxZPT, MinTrackPT, MaxTrackPT, MinHiBin, MaxHiBin);
   par.input         = CL.Get      ("Input",   "/home/kdeverea/zdata/HISingleMuon-v11-Zpt0.root");            // Input file
   par.mixFile       = CL.Get      ("MixFile", "/home/kdeverea/zdata/HISingleMuon-v11-Zpt0.root");            // Input Mix file
   par.output        = CL.Get      ("Output",  "output.root");                                 	// Output file
   par.residualCor   = CL.Get      ("ResidualCor",  "residualCor");                      	// Output file
   par.isSelfMixing  = CL.GetBool  ("IsSelfMixing", true);   // Determine if the analysis is self-mixing
   par.isGenZ        = CL.GetBool  ("IsGenZ", false);        // Determine if the analysis is using Gen level Z     
   par.isPUReject    = CL.GetBool  ("IsPUReject", true);     // Flag to reject PU sample for systemaitcs.
   par.isMuTagged    = CL.GetBool  ("IsMuTagged", true);     // Default is true
   par.isHiBinUp     = CL.GetBool  ("IsHiBinUp", false);     // Default is false
   par.isHiBinDown   = CL.GetBool  ("IsHiBinDown", false);   // Default is false
   par.useLeadingTrk = CL.GetBool  ("UseLeadingTrk", false); // Default is false
   par.useResidualCor= CL.GetBool  ("UseResidualCor", false);// Default is false
   par.scaleFactor   = CL.GetDouble("Fraction", 1.00);       // Fraction of event processed in the sample
   par.nThread       = CL.GetInt   ("nThread", 1);           // The number of threads to be used for parallel processing.
   par.nChunk        = CL.GetInt   ("nChunk", 1);            // Specifies which chunk (segment) of the data to process, used in parallel processing.
   par.nMix          = CL.GetInt   ("nMix", 10);             // Number of mixed events to be considered in the analysis.
   par.shift         = CL.GetDouble("Shift", 971.74);        // Shift of sumHF in MB matching
   par.MinZY         = CL.GetDouble("MinZY", 0);             // Minimum Z particle rapidity threshold for event selection.
   par.MaxZY         = CL.GetDouble("MaxZY", 200);           // Maximum Z particle rapidity threshold for event selection.
   par.ExtraZWeight  = CL.GetInt   ("ExtraZWeight",-1);      // Do Muon systematics, -1 means no extraweight.
   par.includeHole   = CL.GetBool  ("includeHole",true);     // Include hole particle or not
   par.mix = 0;
   par.isPP = IsPP;
   par.isJewel = IsJewel;
   
   if (checkError(par)) return -1;
          
   // Analyze Data
   DataAnalyzer analyzer(par.input.c_str(), par.mixFile.c_str(), par.output.c_str(), "Data");
   analyzer.analyze(par);
   analyzer.writeHistograms(analyzer.outf);
   saveParametersToHistograms(par, analyzer.outf);
   cout << "done!" << analyzer.outf->GetName() << endl;
}
