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
#include "utilities.h"             // Yen-Jie's random utility functions
#include "helpMessage.h"           // Print out help message
#include "parameter.h"             // The parameters used in the analysis
#include "Messenger.h"             // Yi's Messengers for reading data files
#include "CommandLine.h"           // Yi's Commandline bundle
#include "ProgressBar.h"           // Yi's fish progress bar
#include "TrackResidualCorrector.h" // Residual correction


//======= eventSelection =====================================//
// Check if the event mass eventSelection criteria
// MinZPT < zPt < MaxZPT
//============================================================//
bool eventSelection(ZHadronMessenger *b, const Parameters& par) {
   if (par.isPUReject && par.isPP && b->NVertex!=1) return 0;    // Only apply PU rejection (single vertex requirement) in pp analysis

   if ((par.isGenZ ? b->genZMass->size() : b->zMass->size())==0) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0])<60) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0])>120) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0]))<=par.MinZY) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0]))>=par.MaxZY) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0])<par.MinZPT) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0])>par.MaxZPT) return 0;
   
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

//======= trackSelection =====================================//
// Check if the track pass selection criteria
//============================================================//
bool trackSelection(ZHadronMessenger *b, Parameters par, int j) {
    if (par.isMuTagged && (*b->trackMuTagged)[j]) return false; 
    if ((*b->trackPt)[j]>par.MaxTrackPT) return false;  
    if ((*b->trackPt)[j]<par.MinTrackPT) return false;
    if ((!par.includeHole)&&(*b->trackWeight)[j]<0) return false;
    if ((*b->trackEta)[j] > 2.4) return false;
    if ((*b->trackEta)[j] < -2.4) return false;
    return true;
}

// ======= Define mixed event matching criteria
bool matching(ZHadronMessenger *a, ZHadronMessenger *b, double shift) {
    if (a->SignalHF<shift*1.04&&b->SignalHF<shift*1.04) return 1;
    if ((b->SignalHF/(a->SignalHF-shift))<1.04&&b->SignalHF/(a->SignalHF-shift)>0.96) return 1;
    return 0;
}

float getMultiplicity(ZHadronMessenger *b, const Parameters& par) {
    float mult = 0;
    for (unsigned long j = 0; j < b->trackPt->size(); j++) {
        if (!trackSelection(b, par, j)) continue;
        float trackWeight = (*b->trackWeight)[j];
        mult += trackWeight;
    }
    return mult;
}

//============================================================//
// Z hadron dphi calculation
//============================================================//
double get3D(ZHadronMessenger *MZSignal, ZHadronMessenger *MZUE, TH3D *h, TH1D *hEventWeight, const Parameters& par) {
   if (par.isAddUE) {
      if (MZUE->GetEntries()<MZSignal->GetEntries()) {
         cout <<"Error! Smaller number of UE events than Z events"<<endl;
         return -1;
      }   
   }
   double nZ = 0;
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
   int deltaI = (iEnd-iStart)/100+1;
   TrackResidualCorrector corrector(par.residualFile.c_str());              
   TrackResidualCorrector energy_extrapolator(par.EnergyExtraFile.c_str());

   // open VZ correction if needed
   VZCorrector *vzCorrector;
   if (par.VZWeightFile != "") {
      vzCorrector = new VZCorrector(par.VZWeightFile.c_str());
   }

   int i_EPOS = iStart;
   for (unsigned long i = iStart; i < iEnd; i++) {
      MZSignal->GetEntry(i);
   
      if (i % deltaI == 0) {
         Bar.Update(i - iStart);
         Bar.Print();
      }

      // Check if the event passes the selection criteria
      if (!eventSelection(MZSignal, par)) continue;

      float zY = (par.isGenZ ? (*MZSignal->genZY)[0] : (*MZSignal->zY)[0]);
      float zPhi = (par.isGenZ ? (*MZSignal->genZPhi)[0] : (*MZSignal->zPhi)[0]);
      if (zPhi < 0) zPhi += 2 * M_PI;
      float zPt = (par.isGenZ ? (*MZSignal->genZPt)[0] : (*MZSignal->zPt)[0]);
      float residualCorrection = ((par.residualFile=="")||par.isGen==1)? 1 : corrector.GetCorrectionFactor(zPt, zY, zPhi);

      // fill histograms
      // hard coded since SIM should have event weights of 1, for pp pythia+MADGRAPH for some reason they aren't
      float this_eventWeight = 1; // MZSignal->EventWeight;
      if (par.VZWeightFile != "") {
         float vzCorrectionFactor = vzCorrector->GetCorrectionFactor(MZSignal->VZ);
         this_eventWeight *= vzCorrectionFactor;
      } else {
         this_eventWeight *= MZSignal->VZWeight;
      }
      this_eventWeight *= residualCorrection;

      // energy extrapolation weight, parasitically uses track residual weight, dummy args for eta and phi
      float energyWeight = (par.EnergyExtraFile=="") ? 1 : energy_extrapolator.GetCorrectionFactor(zPt, 1, 1);
      this_eventWeight *= energyWeight;

      h->Fill(zPt, zY, zPhi, this_eventWeight);

      nZ += this_eventWeight; //1;

   }
   cout<<"Total number of Zs: "<<nZ<<endl;
   return nZ;

}

class DataAnalyzer {
public:
  DataAnalyzer(const char* filename, const char* filenameUE, const char* residualFilename, const char* outFilename, const char *mytitle = "Data", bool doUE = false) :
     inf(new TFile(filename)),  
     MZHadron(new ZHadronMessenger(*inf,string("Tree"))), 
     title(mytitle), outf(new TFile(outFilename, "recreate"))  {
     if (doUE) {
        infUE = new TFile(filenameUE);
        MZHadronUE = new ZHadronMessenger(*infUE,string("Tree"));  
     }
     outf->cd();
  }

  ~DataAnalyzer() {
    deleteHistograms();
    inf->Close();
    outf->Close();
    delete MZHadron;
  }

  void analyze(Parameters& par) {
    // First histogram with mix=false
    par.mix = false;
    const int nbinsX = 50;
    const double xMin = 0.5;
    const double xMax = 100;
    std::vector<double> binEdgesX(nbinsX + 1);
    
    for (int i = 0; i <= nbinsX; ++i) binEdgesX[i] = xMin * std::pow(xMax / xMin, double(i) / nbinsX);
    binEdgesX[nbinsX]=1000;

    const int nbinsY = 25;
    const double yMin = -2.4;
    const double yMax = 2.4;
    std::vector<double> binEdgesY(nbinsY + 1);

    for (int i = 0; i <= nbinsY; ++i) binEdgesY[i] = yMin + (yMax - yMin) * i / nbinsY;

    const int nbinsZ = 50;
    const double zMin = 0;
    const double zMax = 2 * M_PI;
    std::vector<double> binEdgesZ(nbinsZ + 1);

    for (int i = 0; i <= nbinsZ; ++i) binEdgesZ[i] = zMin + (zMax - zMin) * i / nbinsZ;

    h = new TH3D("h3D", "Histogram Title; p_{T} (GeV/c); #eta; #phi",
                     nbinsX, &binEdgesX[0],
                     nbinsY, &binEdgesY[0],
                     nbinsZ, &binEdgesZ[0]);
    //    h = new TH3D("h3D", "Histogram Title; p_{T} (GeV/c); #eta; #phi", 50,0,10,50,-2.4,2.4,50, 0,2*M_PI);

    hEventWeight = new TH1D("hEventWeight", "", 500, 0, 3);
    hEventWeight->Sumw2();

    hNZ = new TH1D("hNZ","",1,0,100);
    hNZ->Fill(1, get3D(MZHadron, MZHadronUE, h, hEventWeight, par));
  }
  
  void writeHistograms(TFile* outf) {
    outf->cd();
    smartWrite(h);
    smartWrite(hNZ);
    smartWrite(hEventWeight);
  }

  TFile *inf, *infUE, *residualFile, *residualFileClone, *outf;
  TH3D *h=0;
  TH1D *hNZ=0;
  TH1D *hEventWeight=0;
  ZHadronMessenger *MZHadron, *MZHadronUE;
  string title;
  
  private:
  void deleteHistograms() {
    delete h;
  }
};

//============================================================//
// Main analysis
//============================================================//
int main(int argc, char *argv[])
{
   if (printHelpMessage(argc, argv)) return 0;

   CommandLine CL(argc, argv);
   float MinZPT      = CL.GetDouble("MinZPT", 40);         // Minimum Z particle transverse momentum threshold for event selection.
   float MaxZPT      = CL.GetDouble("MaxZPT", 200);        // Maximum Z particle transverse momentum threshold for event selection.
   float MinTrackPT  = CL.GetDouble("MinTrackPT", 1);      // Minimum track transverse momentum threshold for track selection.
   float MaxTrackPT  = CL.GetDouble("MaxTrackPT", 2);      // Maximum track transverse momentum threshold for track selection.
   bool  IsData      = CL.GetBool  ("IsData", false);      // Determines whether the analysis is being run on actual data.
   bool  IsPP        = CL.GetBool  ("IsPP", false);        // Flag to indicate if the analysis is for Proton-Proton collisions.
   bool  IsJewel     = CL.GetBool  ("IsJewel", false);     // Flag to indicate if the analysis is for Jewel since the hole for Jewel is not hadronized

   Parameters par(MinZPT, MaxZPT, MinTrackPT, MaxTrackPT);
   par.input         = CL.Get      ("Input",   "mergedSample/HISingleMuon-v5.root");            // Input file
   par.inputUE       = CL.Get      ("InputUE", "");                                             // Input file for UE
   par.residualFile  = CL.Get      ("residualFile", "");            // Input Mix file
   par.EnergyExtraFile =  CL.Get      ("EnergyExtraFile", "");            // Input file for energy extrapolation
   par.VZWeightFile     = CL.Get      ("VZWeightFile", "");           // VZ weight file
   par.output        = CL.Get      ("Output",  "output.root");                             	// Output file
   par.isGen         = CL.GetBool  ("IsGen", false); // Determine if the analysis is gen level
   par.isGenZ        = CL.GetBool  ("IsGenZ", true);      // Determine if the analysis is using Gen level Z     
   par.isPUReject    = CL.GetBool  ("IsPUReject", true);  // Flag to reject PU sample for systemaitcs.
   par.isMuTagged    = CL.GetBool  ("IsMuTagged", true);   // Default is true
   par.scaleFactor   = CL.GetDouble("Fraction", 1.00);     // Fraction of event processed in the sample
   par.nThread       = CL.GetInt   ("nThread", 1);         // The number of threads to be used for parallel processing.
   par.nChunk        = CL.GetInt   ("nChunk", 1);          // Specifies which chunk (segment) of the data to process, used in parallel processing.
   par.nMix          = CL.GetInt   ("nMix", 10);           // Number of mixed events to be considered in the analysis.
   par.shift         = CL.GetDouble("Shift", 971.74);       // Shift of sumHF in MB matching
   par.MinZY         = CL.GetDouble("MinZY", 0);           // Minimum Z particle rapidity threshold for event selection.
   par.MaxZY         = CL.GetDouble("MaxZY", 200);         // Maximum Z particle rapidity threshold for event selection.
   par.ExtraZWeight  = CL.GetInt   ("ExtraZWeight",-1);    // Do Muon systematics, -1 means no extraweight.
   par.includeHole   = CL.GetBool  ("includeHole",true);   // Include hole particle or not
   par.mix = 0;
   par.isPP = IsPP;
   par.isJewel = IsJewel;
   
   if (par.inputUE=="") par.isAddUE = false; else par.isAddUE = true;
          
   // Analyze Data
   DataAnalyzer analyzer(par.input.c_str(), par.inputUE.c_str(), par.residualFile.c_str(), par.output.c_str(), "Data", par.isAddUE);
   analyzer.analyze(par);
   analyzer.writeHistograms(analyzer.outf);
   saveParametersToHistograms(par, analyzer.outf);
   cout << "done!" << analyzer.outf->GetName() << endl;
}
