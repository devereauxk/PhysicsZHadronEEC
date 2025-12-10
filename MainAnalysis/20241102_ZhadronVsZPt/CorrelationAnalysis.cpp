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
   if ((*b->trackEta)[j] > 2.4) return false;
   if ((*b->trackEta)[j] < -2.4) return false;
   return true;
}

//======= trackSelection =====================================//
// Check if the track pass selection criteria
//============================================================//
bool trackSelectionNoPt(ZHadronMessenger *b, Parameters par, int j) {
   if (par.isMuTagged && (*b->trackMuTagged)[j]) return false; 
   if ((!par.includeHole) && (*b->trackWeight)[j] < 0) return false;
   if ((*b->trackEta)[j] > 2.4) return false;
   if ((*b->trackEta)[j] < -2.4) return false;
   return true;
}

//======= eventSelection =====================================//
// Check if the event mass eventSelection criteria
// MinZPT < zPt < MaxZPT
// MinHiBin , hiBin < MaxHiBin
//============================================================//
bool eventSelection(ZHadronMessenger *b, const Parameters& par) {
   if (par.isPUReject && par.isPP && b->NVertex != 1) return 0;                                     // Only apply PU rejection (single vertex requirement) in pp analysis
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


// ======= Check if PPb event is PPb or PbP, returns true if PPb
bool isPPbEvent(ZHadronMessenger *b) {
   if (b->Run < 285922) return true;
   return false;
}

// ====== add UE event from EPOS to hard event
void addUEParticles(ZHadronMessenger *hard, ZHadronMessenger *ue) {
   for (unsigned int i = 0; i < ue->trackPt->size(); i++) {
      hard->trackPt->push_back(ue->trackPt->at(i));
      hard->trackEta->push_back(ue->trackEta->at(i));
      hard->trackY->push_back(ue->trackY->at(i));
      hard->trackPhi->push_back(ue->trackPhi->at(i));
      hard->trackMuTagged->push_back(ue->trackMuTagged->at(i));
      hard->trackWeight->push_back(ue->trackWeight->at(i));
      hard->trackResidualWeight->push_back(ue->trackResidualWeight->at(i));
   }
}

//============================================================//
// Z hadron dphi calculation
//============================================================//
float getDphi(ZHadronMessenger *MZSignal, ZHadronMessenger *MMix, ZHadronMessenger *MMixEvt, TH2D *h, TH2D *hSub0, TH3D *hTrkPtEtaPhi, TH3D* hZPtEtaMult, TH1D* hVZ, const Parameters& par, TNtuple *nt = 0) {
   float nZ = 0;
   h->Sumw2();
   if (hTrkPtEtaPhi != 0) hTrkPtEtaPhi->Sumw2();
   if (hZPtEtaMult != 0) hZPtEtaMult->Sumw2();
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

   // open EPOS file if needed
   ZHadronMessenger *MEPOS = nullptr;
   if (par.useEPOSFile) {
      TFile *fEPOS = TFile::Open(par.EPOSFile.c_str());
      if (!fEPOS || fEPOS->IsZombie()) {
         return -1;
      }
      MEPOS = new ZHadronMessenger(*fEPOS, string("Tree"));
   }

   // open track residual correctors if needed
   TrackResidualCorrector *corrector;
   TrackResidualCorrector *corrector_0_10;
   TrackResidualCorrector *corrector_10_20;
   TrackResidualCorrector *corrector_20_40;
   TrackResidualCorrector *corrector_40_500;
   if (par.useResidualWeight) {
      corrector_0_10   = new TrackResidualCorrector(Form("%s0-10.root",   par.residualWeightFile.c_str()));              
      corrector_10_20  = new TrackResidualCorrector(Form("%s10-20.root",  par.residualWeightFile.c_str()));
      corrector_20_40  = new TrackResidualCorrector(Form("%s20-40.root",  par.residualWeightFile.c_str()));
      corrector_40_500 = new TrackResidualCorrector(Form("%s40-500.root", par.residualWeightFile.c_str()));              
   }


   //==================================================//
   // loop over events
   //==================================================//
   int mix_EPOS_i = 0;
   for (unsigned long i = iStart; i < iEnd; i++) {
      MZSignal->GetEntry(i);

      if (i % deltaI == 0) {
         Bar.Update(i - iStart);
         Bar.Print();
      }

      // Check if the event passes the selection criteria
      if (!eventSelection(MZSignal, par)) continue;
      //if (MZSignal->trackPt->size() < 1) continue;

      float zY = (par.isGenZ ? (*MZSignal->genZY)[0] : (*MZSignal->zY)[0]);
      float zPhi = (par.isGenZ ? (*MZSignal->genZPhi)[0] : (*MZSignal->zPhi)[0]);
      float zPt = (par.isGenZ ? (*MZSignal->genZPt)[0] : (*MZSignal->zPt)[0]);

      if (par.yBoost != 0) {
         if (par.isPPb) zY = zY - par.yBoost;
         else zY = -(zY + par.yBoost);
      }

      // add UE to signal file particles from EPOS if needed
      if (par.useEPOSFile) {
         MEPOS->GetEntry(i % MEPOS->GetEntries());
         addUEParticles(MZSignal, MEPOS);
      }

      //==================================================//
      // loop over mixed events
      //==================================================//
      bool foundMix = false;
      for (unsigned int nMix = 0; nMix < targetMix; nMix++) {
         foundMix = false;
         mixstart_i = mix_i;
         if (par.mix) {
            while (!foundMix) {
               mix_i = (mix_i + 1);
               if (mix_i >= MMixEvt->GetEntries()) mix_i = 0;
               if (mixstart_i == mix_i) break;
               MMixEvt->GetEntry(mix_i);
               // only mix PPb with PPb and PbP with PbP
               if (!par.isPP && par.isData && isPPbEvent(MZSignal) != isPPbEvent(MMixEvt)) continue;
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

         // add UE to mix file particles from EPOS if needed
         if (par.useEPOSFile) {
            MEPOS->GetEntry(mix_EPOS_i % MEPOS->GetEntries());
            addUEParticles(MMix, MEPOS);
            mix_EPOS_i++;
         }

         //==================================================//
         // calculate event+Z weight
         //==================================================//
         float this_eventZWeight = 1;
         if (par.useEventWeight) {
            if (par.mix && par.isSelfMixing) {
               this_eventZWeight *= (MZSignal->EventWeight) * (MMix->EventWeight) * (MZSignal->ZWeight) * (MMix->ZWeight) * (MZSignal->VZWeight) * (MMix->VZWeight);
            } else {
               this_eventZWeight *= (MZSignal->EventWeight) * (MZSignal->ZWeight) * (MZSignal->VZWeight);
            }
         }
         // check event indeed has the right orientation
         //if (!par.isPP && par.isData && isPPbEvent(MZSignal) != par.isPPb)  this_eventZWeight = 0;
         //cout<<par.isPPb << " " << isPPbEvent(MZSignal) << " " << MZSignal->Run << " ::: " << this_eventZWeight << endl;

         nZ += this_eventZWeight;
         
         //==================================================//
         // loop over tracks
         //==================================================//
         int mult = 0;
         for (unsigned long j = 0; j < (par.mix ? MMix->trackPhi->size() : MZSignal->trackPhi->size()); j++) {

            // Check if the event passes the selection criteria
            if (!trackSelection((par.mix ? MMix : MZSignal), par, j)) continue;
            mult++;
            
            float trackDphi  = par.mix ? DeltaPhi((*MMix->trackPhi)[j], zPhi) : DeltaPhi((*MZSignal->trackPhi)[j], zPhi);
            float trackDphi2 = par.mix ? DeltaPhi(zPhi, (*MMix->trackPhi)[j]) : DeltaPhi(zPhi, (*MZSignal->trackPhi)[j]);
            float trackDeta  = par.mix ? fabs((*MMix->trackEta)[j] - zY) : fabs((*MZSignal->trackEta)[j] - zY);

            float trackPhi  = par.mix ? (*MMix->trackPhi)[j] : (*MZSignal->trackPhi)[j];
            float trackEta  = par.mix ? (*MMix->trackEta)[j] : (*MZSignal->trackEta)[j];
            float trackPt   = par.mix ? (*MMix->trackPt)[j] : (*MZSignal->trackPt)[j];

            if (par.yBoost != 0 && !par.isPP) {
               if (par.isPPb) trackEta = trackEta - par.yBoost;
               else trackEta = -(trackEta + par.yBoost);
            }
            // boost will not affect trackDeta as track and Z boosted by same amount
            // boost nonetheless applied for Z and track-specific histograms

            //==================================================//
            // calculate track weight
            //==================================================//
            float this_trackWeight = 1;
            if(par.useTrackWeight) {
               if (par.mix) {
                  this_trackWeight *= (*MMix->trackWeight)[j];
               } else {
                  this_trackWeight *= (*MZSignal->trackWeight)[j];
               }
            }

            float this_residualWeight = 1;
            if(par.useResidualWeight) {

               if (zPt < 10) corrector = corrector_0_10;
               else if (zPt >= 10 && zPt < 20) corrector = corrector_10_20;
               else if (zPt >= 20 && zPt < 40) corrector = corrector_20_40;
               else corrector = corrector_40_500;

               float residualCorrection = corrector->GetCorrectionFactor(trackPt, trackEta, trackPhi);
               //this_residualWeight *= residualCorrection;
               
               if (par.mix) {
                  this_residualWeight *= (*MMix->trackResidualWeight)[j];
               } else {
                  this_residualWeight *= (*MZSignal->trackResidualWeight)[j];
               }
               
            }

            float weight = this_eventZWeight * this_trackWeight * this_residualWeight;

            /*
            if (trackPt > 8) {
               cout << "(" << trackPt << ", " << trackEta << ") "
                   << "(" << this_eventZWeight << ", " << this_trackWeight << ", " << this_residualWeight << ") = "
                   << weight << endl;
            }
                   */
            
            //==================================================//
            // fill central values
            //==================================================//
            h->Fill(trackDeta, trackDphi, weight);
            h->Fill(-trackDeta, trackDphi, weight);
            h->Fill(trackDeta, trackDphi2, weight);
            h->Fill(-trackDeta, trackDphi2, weight);

            if (hTrkPtEtaPhi != 0) hTrkPtEtaPhi->Fill(trackPt, trackEta, trackPhi, weight);
         
         } // end track loop

         if (hZPtEtaMult != 0) hZPtEtaMult->Fill(zPt, zY, mult, this_eventZWeight);
         if (hVZ != 0) hVZ->Fill(MZSignal->VZ, this_eventZWeight);

      } // end mix event loop

   } // end event loop

   return nZ;
}

class DataAnalyzer {
public:
   TFile *inf, *mixFile, *mixFileClone, *outf;
   TNtuple *ntDiagnose;
   TH1D *hNZ, *hNZMix, *hVZ;
   TH3D *hTrkPtEtaPhi;
   TH3D *hZPtEtaMult;
   TH2D *h = 0, *hSub0 = 0, *hMix = 0;
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
      h = new TH2D(Form("h%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hTrkPtEtaPhi = new TH3D(Form("hTrkPtEtaPhi%s", title.c_str()), "", 100, 0, 100, 48, -2.4, 2.4, 48, -M_PI, M_PI);
      hZPtEtaMult = new TH3D(Form("hZPtEtaMult%s", title.c_str()), "", 70, 0, 200, 30, -3, 3, 300, 0, 300);
      hVZ = new TH1D(Form("hVZ%s", title.c_str()), "", 40, -20, 20);
      hSub0 = new TH2D(Form("hSub0%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hNZ = new TH1D(Form("hNZ%s", title.c_str()), "", 1, 0, 1);
      hNZ->SetBinContent(1, getDphi(MZHadron, MMix, MMixEvt, h, hSub0, hTrkPtEtaPhi, hZPtEtaMult, hVZ, par)); // Dphi analysis

      // Second histogram with mix=true
      par.mix = true;
      hMix = new TH2D(Form("hMix%s", title.c_str()), "", 20, -4, 4, 20, -M_PI / 2, 3 * M_PI / 2);
      hNZMix = new TH1D(Form("hNZMix%s", title.c_str()), "", 1, 0, 1);
      hNZMix->SetBinContent(1, getDphi(MZHadron, MMix, MMixEvt, hMix, 0, 0, 0, 0, par, ntDiagnose)); // Dphi analysis with mixing
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
      smartWrite(hZPtEtaMult);
      smartWrite(hVZ);
   }

private:
   void deleteHistograms() {
      delete h, hSub0, hMix, hNZ, hNZMix, hTrkPtEtaPhi, hZPtEtaMult, hVZ;
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
   int   MinHiBin    = CL.GetInt   ("MinHiBin", 0);          // Minimum hiBin value for event selection.
   int   MaxHiBin    = CL.GetInt   ("MaxHiBin", 200);        // Maximum hiBin value for event selection.
   bool  IsData      = CL.GetBool  ("IsData", false);        // Determines whether the analysis is being run on actual data.
   bool  IsPP        = CL.GetBool  ("IsPP", false);          // Flag to indicate if the analysis is for Proton-Proton collisions.
   bool  IsJewel     = CL.GetBool  ("IsJewel", false);       // Flag to indicate if the analysis is for Jewel since the hole for Jewel is not hadronized

   if (IsPP) {                                                                         
      MinHiBin=-2;
      MaxHiBin=1000000;
   }

   Parameters par(MinZPT, MaxZPT, MinTrackPT, MaxTrackPT, MinHiBin, MaxHiBin);
   par.input         = CL.Get      ("Input",   "mergedSample/HISingleMuon-v5.root");         // Input file
   par.mixFile       = CL.Get      ("MixFile", "mergedSample/HISingleMuon-v5.root");         // Input Mix file
   par.EPOSFile      = CL.Get      ("EPOSFile", "");                                         // EPOS sample, to add UE/background to GenMC hard processes, leave blank if no embeding needed
   par.useEPOSFile   = !par.EPOSFile.empty();
   par.output        = CL.Get      ("Output",  "output.root");                               // Output file
   par.isSelfMixing  = CL.GetBool  ("IsSelfMixing", true);   // Determine if the analysis is self-mixing
   par.isGenZ        = CL.GetBool  ("IsGenZ", false);        // Determine if the analysis is using Gen level Z     
   par.isPUReject    = CL.GetBool  ("IsPUReject", true);     // Flag to reject PU sample for systemaitcs.
   par.isMuTagged    = CL.GetBool  ("IsMuTagged", true);     // Default is true
   par.useLeadingTrk = CL.GetBool  ("UseLeadingTrk", false); // Default is false
   par.useTrackWeight   = CL.GetBool  ("UseTrackWeight", true);     // Default is true
   par.useEventWeight   = CL.GetBool  ("UseEventWeight", true);     // Default is false
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
   DataAnalyzer analyzer(par.input.c_str(), par.mixFile.c_str(), par.output.c_str(), "Data");
   analyzer.analyze(par);
   analyzer.writeHistograms(analyzer.outf);
   saveParametersToHistograms(par, analyzer.outf);
   cout << "done!" << analyzer.outf->GetName() << endl;
}
