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
#include "utilities.h"             // Yen-Jie's random utility functions
#include "helpMessage.h"           // Print out help message
#include "parameter.h"             // The parameters used in the analysis
#include "Messenger.h"             // Yi's Messengers for reading data files
#include "CommandLine.h"           // Yi's Commandline bundle
#include "ProgressBar.h"           // Yi's fish progress bar
#include "TrackResidualCorrector.h" // Residual correction

bool validateVZConfiguration(const Parameters& par) {
   if (par.isData) {
      if (par.useVZWeight) {
         cerr << "Error! Data stages must run with UseVZWeight=false." << endl;
         return false;
      }
      if (par.VZWeightFile != "") {
         cerr << "Error! Data stages must not receive VZWeightFile." << endl;
         return false;
      }
   }

   if (par.useVZWeight && par.VZWeightFile == "") {
      cerr << "Error! UseVZWeight=true requires an explicit external VZWeightFile for MC stages." << endl;
      return false;
   }

   if (!par.useVZWeight && par.VZWeightFile != "") {
      cerr << "Error! VZWeightFile was provided but UseVZWeight=false. Pass both explicitly for MC VZ weighting." << endl;
      return false;
   }

   return true;
}

bool eventSelection(ZHadronMessenger *b, const Parameters& par) {
   if (par.isPUReject && par.isData && b->NVertex!=1) return 0;
   if (par.useVZWindow && fabs(b->VZ) >= 15) return 0;

   if ((par.isGenZ ? b->genZMass->size() : b->zMass->size())==0) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0])<60) return 0;
   if ((par.isGenZ ? (*b->genZMass)[0] : (*b->zMass)[0])>120) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0]))<=par.MinZY) return 0;
   if (fabs((par.isGenZ ? (*b->genZY)[0] : (*b->zY)[0]))>=par.MaxZY) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0])<par.MinZPT) return 0;
   if ((par.isGenZ ? (*b->genZPt)[0] : (*b->zPt)[0])>par.MaxZPT) return 0;

   return 1;
}

pair<int, int> findClosestMuonTracks(ZHadronMessenger *b, const Parameters &par)
{
   if(par.TrackMuClosest == false)
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
    if ((*b->trackPt)[j] >= 15) return false;
    if ((*b->trackPt)[j] < 0.5) return false;
    if ((!par.includeHole)&&(*b->trackWeight)[j]<0) return false;
    if ((*b->trackEta)[j] > 2.4) return false;
    if ((*b->trackEta)[j] < -2.4) return false;
    return true;
}

double get3D(ZHadronMessenger *MZSignal, ZHadronMessenger *MZUE, TH3D *h, const Parameters& par) {
   double nZ = 0;
   h->Sumw2();
   par.printParameters();
   unsigned long nEntry = MZSignal->GetEntries() * par.scaleFactor;
   unsigned long iStart = nEntry * (par.nChunk - 1) / par.nThread;
   unsigned long iEnd = nEntry * par.nChunk / par.nThread;

   ProgressBar Bar(cout, iEnd - iStart);
   Bar.SetStyle(1);
   int deltaI = (iEnd-iStart)/100+1;
   TrackResidualCorrector2D corrector(par.residualFile.c_str());

   ZCorrector *Zcorrector = nullptr;
   if (par.ZWeightFile != "") {
      Zcorrector = new ZCorrector(par.ZWeightFile.c_str());
   }

   VZCorrector *vzCorrector = nullptr;
   if (par.useVZWeight) {
      vzCorrector = new VZCorrector(par.VZWeightFile.c_str());
   }

   for (unsigned long i = iStart; i < iEnd; i++) {
      MZSignal->GetEntry(i);

      if (i % deltaI == 0) {
         Bar.Update(i - iStart);
         Bar.Print();
      }

      if (par.isAddUE) {
         MZUE->GetEntry(i % MZUE->GetEntries());
      }

      if (!eventSelection(MZSignal, par)) continue;

      float zPt = (par.isGenZ ? (*MZSignal->genZPt)[0] : (*MZSignal->zPt)[0]);
      float zY  = (par.isGenZ ? (*MZSignal->genZY)[0] : (*MZSignal->zY)[0]);
      float zPhi = (par.isGenZ ? (*MZSignal->genZPhi)[0] : (*MZSignal->zPhi)[0]);
      if (zPhi < 0) zPhi += 2 * M_PI;
      float ZWeight = (par.ZWeightFile != "") ? Zcorrector->GetCorrectionFactor(zPt, zY) : 1;
      ZWeight *= (par.isOO) ? MZSignal->ZWeight : 1;

      float this_eventWeight = ZWeight * MZSignal->EventWeight;
      if (par.useVZWeight)
         this_eventWeight *= vzCorrector->GetCorrectionFactor(MZSignal->VZ);
      pair<int, int> signalClosestMuonTracks = findClosestMuonTracks(MZSignal, par);

      for (unsigned long j = 0; j < MZSignal->trackPhi->size(); j++) {
         if (!trackSelection(MZSignal, par, j, signalClosestMuonTracks)) continue;
         float trackPhi  = (*MZSignal->trackPhi)[j];
         if (trackPhi<0) trackPhi+= 2 * M_PI;
         float trackEta  = (*MZSignal->trackEta)[j];
         float trackPt   = (*MZSignal->trackPt)[j];
         float residualCorrection = par.isGen ? 1 : corrector.GetCorrectionFactor(trackPt, trackEta, trackPhi);
         float weight = this_eventWeight;
         weight*= (*MZSignal->trackWeight)[j];
         weight*= par.TrackExtraWeight;
         weight*= residualCorrection;
         h->Fill( trackPt, trackEta, trackPhi, weight);
      }
      if (par.isAddUE) {
         pair<int, int> ueClosestMuonTracks = findClosestMuonTracks(MZUE, par);
         for (unsigned long j = 0; j < MZUE->trackPhi->size(); j++) {
            if (!trackSelection(MZUE, par, j, ueClosestMuonTracks)) continue;
            float trackPhi  = (*MZUE->trackPhi)[j];
            if (trackPhi<0) trackPhi+= 2 * M_PI;
            float trackEta  = (*MZUE->trackEta)[j];
            float trackPt   = (*MZUE->trackPt)[j];
            float residualCorrection = par.isGen ? 1 : corrector.GetCorrectionFactor(trackPt, trackEta, trackPhi);
            float weight = this_eventWeight;
            weight*= (*MZUE->trackWeight)[j];
            weight*= par.TrackExtraWeight;
            weight*= residualCorrection;
            h->Fill( trackPt, trackEta, trackPhi, weight);
         }
      }
      nZ += this_eventWeight;

    }
    cout<<"Total number of Zs: "<<nZ<<endl;

    if (Zcorrector) delete Zcorrector;
    if (vzCorrector) delete vzCorrector;

    return nZ;
}

class DataAnalyzer {
public:
  DataAnalyzer(const char* filename, const char* filenameUE, const char* outFilename, const char *mytitle = "Data", bool doUE = false, const char *trackTreeName = "Tree") :
     inf(new TFile(filename)),
     MZHadron(new ZHadronMessenger(*inf,string(trackTreeName))),
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
    par.mix = false;
    const int nbinsX = 26;
    const double xMin = 0.5;
    const double xMax = 15;
    std::vector<double> binEdgesX(nbinsX + 1);

    for (int i = 0; i <= nbinsX; ++i) binEdgesX[i] = xMin * std::pow(xMax / xMin, double(i) / nbinsX);

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

    h = new TH3D("hTrkPtEtaPhiData", "; p_{T} (GeV/c); #eta; #phi",
                     nbinsX, &binEdgesX[0],
                     nbinsY, &binEdgesY[0],
                     nbinsZ, &binEdgesZ[0]);
    hNZ = new TH1D("hNZData","",1,0,100);
    hNZ->Fill(1, get3D(MZHadron, MZHadronUE, h, par));
  }

  void writeHistograms(TFile* outf) {
    outf->cd();
    smartWrite(h);
    smartWrite(hNZ);
  }

  TFile *inf, *infUE, *outf;
  TH3D *h=0;
  TH1D *hNZ=0;
  ZHadronMessenger *MZHadron, *MZHadronUE;
  string title;

  private:
  void deleteHistograms() {
    delete h;
  }
};

int main(int argc, char *argv[])
{
   if (printHelpMessage(argc, argv)) return 0;

   CommandLine CL(argc, argv);
   float MinZPT      = CL.GetDouble("MinZPT", 40);
   float MaxZPT      = CL.GetDouble("MaxZPT", 200);
   bool  IsData      = CL.GetBool  ("IsData", false);
   bool  IsPP        = CL.GetBool  ("IsPP", false);
   bool  IsJewel     = CL.GetBool  ("IsJewel", false);

   Parameters par(MinZPT, MaxZPT, 0.5, 15);
   par.input         = CL.Get      ("Input",   "mergedSample/HISingleMuon-v5.root");
   par.inputUE       = CL.Get      ("InputUE", "");
   par.residualFile  = CL.Get      ("residualFile", "");
    par.VZWeightFile   = CL.Get      ("VZWeightFile", "");
    par.useVZWeight    = CL.GetBool  ("UseVZWeight", false);
   par.useVZWindow    = CL.GetBool  ("UseVZWindow", true);
   par.output        = CL.Get      ("Output",  "output.root");
   par.isGen         = CL.GetBool  ("IsGen", false);
   par.isGenZ        = CL.GetBool  ("IsGenZ", true);
   par.isData        = IsData;
   par.isOO          = CL.GetBool  ("IsOO", false);
   par.isPUReject    = CL.GetBool  ("IsPUReject", false);
   par.isMuTagged    = CL.GetBool  ("IsMuTagged", true);
   par.TrackMuDR     = CL.GetDouble("TrackMuDR", -1);
   par.TrackMuClosest = CL.GetBool ("TrackMuClosest", false);
   par.ZWeightFile   = CL.Get      ("ZWeightFile", "");
   par.scaleFactor   = CL.GetDouble("Fraction", 1.00);
   par.nThread       = CL.GetInt   ("nThread", 1);
   par.nChunk        = CL.GetInt   ("nChunk", 1);
   par.nMix          = CL.GetInt   ("nMix", 10);
   par.shift         = CL.GetDouble("Shift", 971.74);
   par.MinZY         = CL.GetDouble("MinZY", 0);
   par.MaxZY         = CL.GetDouble("MaxZY", 200);
   par.ExtraZWeight  = CL.GetInt   ("ExtraZWeight",-1);
    par.TrackExtraWeight = CL.GetDouble("TrackExtraWeight", 1.0);
    par.TrackSelectionMode = CL.Get      ("TrackSelectionMode", "Nominal");
   par.TrackTreeName = "Tree";
   if (par.TrackSelectionMode == "Loose")   par.TrackTreeName = "TreeLoose";
   if (par.TrackSelectionMode == "Tight")   par.TrackTreeName = "TreeTight";
   if (par.isGen) par.TrackTreeName = "Tree";
   par.includeHole   = CL.GetBool  ("includeHole",true);
   par.mix = 0;
   par.isPP = IsPP;
   par.isJewel = IsJewel;

    if (par.inputUE=="") par.isAddUE = false; else par.isAddUE = true;
    if (!validateVZConfiguration(par)) return -1;

   DataAnalyzer analyzer(par.input.c_str(), par.inputUE.c_str(), par.output.c_str(), "Data", par.isAddUE, par.TrackTreeName.c_str());
   analyzer.analyze(par);
   analyzer.writeHistograms(analyzer.outf);
   saveParametersToHistograms(par, analyzer.outf);
   cout << "done!" << analyzer.outf->GetName() << endl;
}
