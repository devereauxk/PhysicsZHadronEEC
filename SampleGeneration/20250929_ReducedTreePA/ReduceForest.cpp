#include <cmath>
#include <limits>
#include <iostream>
using namespace std;

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TLorentzVector.h"

#include "CommonFunctions.h"
#include "CommandLine.h"
#include "ProgressBar.h"
#include "Messenger.h"
#include "tnp_weight.h"
#include "trackingEfficiency2017pp.h"

#include "TrackEfficiencyCorrector.h"
#include "JSON_handler.h"

int main(int argc, char *argv[]);
double GetHFSum(PFTreeMessenger *M);
double GetGenHFSum(GenParticleTreeMessenger *M, int SubEvent = -1);
double GetPPDimuonTnPWeight(double Mu1Eta, double Mu2Eta);
double GetPADimuonTnPWeight(double Mu1PT, double Mu1Eta, double Mu2PT, double Mu2Eta);
bool UsePADimuonTnPWeights(double Mu1PT, double Mu1Eta, double Mu2PT, double Mu2Eta);
double GetRelativeWeight(double Numerator, double Denominator);
string GetPPTrackingDirectory(string TrackEfficiencyPath);

double GetPPDimuonTnPWeight(double Mu1Eta, double Mu2Eta)
{
   return tnp_weight_TightID_pp(Mu1Eta, 0)
      * tnp_weight_TightID_pp(Mu2Eta, 0)
      * tnp_weight_L3Mu12_double_pp(Mu1Eta, Mu2Eta, 0);
}

double GetPADimuonTnPWeight(double Mu1PT, double Mu1Eta, double Mu2PT, double Mu2Eta)
{
   return tnp_weight_TightID_pPb(Mu1PT, Mu1Eta, 0)
      * tnp_weight_TightID_pPb(Mu2PT, Mu2Eta, 0)
      * tnp_weight_L3Mu12_double_pPb(Mu1PT, Mu1Eta, Mu2PT, Mu2Eta, 0);
}

bool UsePADimuonTnPWeights(double Mu1PT, double Mu1Eta, double Mu2PT, double Mu2Eta)
{
   return tnp_weight_pPb_valid_muon(Mu1PT, Mu1Eta)
      && tnp_weight_pPb_valid_muon(Mu2PT, Mu2Eta);
}

double GetRelativeWeight(double Numerator, double Denominator)
{
   if(std::isfinite(Numerator) == false || std::isfinite(Denominator) == false || Denominator == 0)
      return numeric_limits<double>::quiet_NaN();
   return Numerator / Denominator;
}

string GetPPTrackingDirectory(string TrackEfficiencyPath)
{
   if(TrackEfficiencyPath == "")
      return "";

   if(TrackEfficiencyPath.rfind(".root") == TrackEfficiencyPath.size() - 5)
   {
      size_t Slash = TrackEfficiencyPath.find_last_of('/');
      if(Slash == string::npos)
         return "";
      return TrackEfficiencyPath.substr(0, Slash + 1);
   }

   if(TrackEfficiencyPath.back() != '/')
      TrackEfficiencyPath = TrackEfficiencyPath + "/";
   return TrackEfficiencyPath;
}

int main(int argc, char *argv[])
{
   string VersionString = "V0.1";
   
   CommandLine CL(argc, argv);

   vector<string> InputFileNames = CL.GetStringVector("Input");
   string OutputFileName = CL.Get("Output");

   bool DoGenLevel                    = CL.GetBool("DoGenLevel", true);
   bool IsPP                          = CL.GetBool("IsPP", false);
   bool IsData                        = CL.GetBool("IsData", false);
   bool Is8TeV                        = CL.GetBool("Is8TeV", false);
   bool IsBackground                  = CL.GetBool("IsBackground", false);
   double Fraction                    = CL.GetDouble("Fraction", 1.00);
   double MinZPT                      = CL.GetDouble("MinZPT", 20);
   double MinTrackPT                  = CL.GetDouble("MinTrackPT", 1);
   bool DoSumET                       = CL.GetBool("DoSumET", false);
   double MuonVeto                    = CL.GetDouble("MuonVeto", 0.0025);
   bool CheckZ                        = CL.GetBool("CheckZ", true);
   string TrackEfficiencyPath         = (DoGenLevel == false) ? CL.Get("TrackEfficiencyPath") : "";
   string TrackEfficiencyPathLoose    = (DoGenLevel == false) ? CL.Get("TrackEfficiencyPathLoose", TrackEfficiencyPath) : "";
   string TrackEfficiencyPathTight    = (DoGenLevel == false) ? CL.Get("TrackEfficiencyPathTight", TrackEfficiencyPath) : "";
   bool WriteAllTrackSelectionTrees   = CL.GetBool("WriteAllTrackSelectionTrees", false);
   bool IgnoreEventWeight             = CL.GetBool("IgnoreEventWeight", false);
   string JSONPath                    = IsData ? CL.Get("JSONPath", "") : "";
   string PFTreeName                  = CL.Get("PFTree", "pfcandAnalyzer/pfTree");

   if(DoGenLevel == true && WriteAllTrackSelectionTrees == true)
      cout << "Warning: WriteAllTrackSelectionTrees requested for gen-level skimming. Only Tree will be written." << endl;

   JSON_handler *JSONHandler = nullptr;
   if(JSONPath != "")
      JSONHandler = new JSON_handler(JSONPath);

   TrackEfficiencyCorrector *TrackEfficiency = nullptr;
   TrackEfficiencyCorrector *TrackEfficiencyLoose = nullptr;
   TrackEfficiencyCorrector *TrackEfficiencyTight = nullptr;
   TrkEff2017pp *TrackEfficiencyPP = nullptr;
   TrkEff2017pp *TrackEfficiencyPPLoose = nullptr;
   TrkEff2017pp *TrackEfficiencyPPTight = nullptr;
   if(DoGenLevel == false)
   {
      if(IsPP == true)
      {
         TrackEfficiencyPP = new TrkEff2017pp(false, GetPPTrackingDirectory(TrackEfficiencyPath));
         TrackEfficiencyPPLoose = new TrkEff2017pp(false, GetPPTrackingDirectory(TrackEfficiencyPathLoose));
         TrackEfficiencyPPTight = new TrkEff2017pp(false, GetPPTrackingDirectory(TrackEfficiencyPathTight));
      }
      else
      {
         TrackEfficiency = new TrackEfficiencyCorrector(TrackEfficiencyPath);
         TrackEfficiencyLoose = new TrackEfficiencyCorrector(TrackEfficiencyPathLoose);
         TrackEfficiencyTight = new TrackEfficiencyCorrector(TrackEfficiencyPathTight);
      }
   }
   // TrackResidualCentralityCorrector TrackResidual(TrackResidualPath);

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");
   TTree Tree("Tree", Form("Tree for ZHadron PA analysis (%s)", VersionString.c_str()));
   TTree TreeLoose("TreeLoose", Form("Loose track-selection tree for ZHadron PA analysis (%s)", VersionString.c_str()));
   TTree TreeTight("TreeTight", Form("Tight track-selection tree for ZHadron PA analysis (%s)", VersionString.c_str()));
   TTree InfoTree("InfoTree", "Information");
   TH1D HLTEffNumerator("HLTEffNumerator", ";Reco Z p_{T};Events", 30, 0, 120);
   TH1D HLTEffDenominator("HLTEffDenominator", ";Reco Z p_{T};Events", 30, 0, 120);
   HLTEffNumerator.Sumw2();
   HLTEffDenominator.Sumw2();

   ZHadronMessenger MZHadron;
   ZHadronMessenger MZHadronLoose;
   ZHadronMessenger MZHadronTight;
   MZHadron.SetBranch(&Tree);
   if(DoGenLevel == false && WriteAllTrackSelectionTrees == true)
   {
      MZHadronLoose.SetBranch(&TreeLoose);
      MZHadronTight.SetBranch(&TreeTight);
   }

   cout << "WriteAllTrackSelectionTrees = " << (WriteAllTrackSelectionTrees ? "true" : "false") << endl;

   for(string InputFileName : InputFileNames)
   {
      TFile InputFile(InputFileName.c_str());

      HiEventTreeMessenger     MEvent(InputFile);
      TrackTreeMessenger       MTrack(InputFile);
      GenParticleTreeMessenger MGen(InputFile);
      PFTreeMessenger          MPF(InputFile, PFTreeName);
      MuTreeMessenger          MMu(InputFile);
      SkimTreeMessenger        MSkim(InputFile);
      TriggerTreeMessenger     MTrigger(InputFile);
      
      int EntryCount = MEvent.GetEntries() * Fraction;
      ProgressBar Bar(cout, EntryCount);
      Bar.SetStyle(-1);
      
      /////////////////////////////////
      //////// Main Event Loop ////////
      /////////////////////////////////   

      for(int iE = 0; iE < EntryCount; iE++)
      {
         bool PassHLTSelection = true;

         if(EntryCount < 300 || (iE % (EntryCount / 250)) == 0)
         {
            Bar.Update(iE);
            Bar.Print();
         }
         
         MEvent.GetEntry(iE);
         MGen.GetEntry(iE);
         MTrack.GetEntry(iE);
         MPF.GetEntry(iE);
         MMu.GetEntry(iE);
         MSkim.GetEntry(iE);
         MTrigger.GetEntry(iE);

         MZHadron.Clear();
         if(DoGenLevel == false && WriteAllTrackSelectionTrees == true)
         {
            MZHadronLoose.Clear();
            MZHadronTight.Clear();
         }

         ////////////////////////////////////////
         ////////// Global event stuff //////////
         ////////////////////////////////////////
         
         MZHadron.Run   = MEvent.Run;
         MZHadron.Lumi  = MEvent.Lumi;
         MZHadron.Event = MEvent.Event;
         MZHadron.hiBin = MEvent.hiBin;
         MZHadron.hiHF  = MEvent.hiHF;
         MZHadron.NPU   = 0;
         if(MEvent.npus->size() == 9)
            MZHadron.NPU = MEvent.npus->at(5);
         else if(MEvent.npus->size() > 1)
            MZHadron.NPU = MEvent.npus->at(0);
         else
            MZHadron.NPU = 0;
         MZHadron.EventWeight = MEvent.weight;
         if(IgnoreEventWeight == true)
            MZHadron.EventWeight = 1;

         ////////////////////////////
         ////////// Vertex //////////
         ////////////////////////////

         MZHadron.NVertex = 0;
         int BestVertex = -1;
         for(int i = 0; i < MTrack.nVtx; i++)
         {
            if(BestVertex < 0 || MTrack.sumPtVtx[i] > MTrack.sumPtVtx[BestVertex])
               BestVertex = i;
            MZHadron.NVertex = MZHadron.NVertex + 1;
         }

         if(BestVertex >= 0)
         {
            MZHadron.VX      = MTrack.xVtx[BestVertex];
            MZHadron.VY      = MTrack.yVtx[BestVertex];
            MZHadron.VZ      = MTrack.zVtx[BestVertex];
            MZHadron.VXError = MTrack.xVtxErr[BestVertex];
            MZHadron.VYError = MTrack.yVtxErr[BestVertex];
            MZHadron.VZError = MTrack.zVtxErr[BestVertex];

            MZHadron.VZWeight = 1;
            // if(IsData == false)
            //    MZHadron.VZWeight = GetVZWeight(MZHadron.VZ);
         }

         /////////////////////////////////////
         ////////// Event selection //////////
         /////////////////////////////////////

         if(IsPP == true)
         {
            if(DoGenLevel == false)
            {
               // JSON selection
               if (JSONHandler != nullptr && !(JSONHandler->isGood(MEvent.Run, MEvent.Lumi)))
                  continue;
               
               // offline event selection
               int pprimaryVertexFilter = MSkim.PVFilter;
               int beamScrapingFilter = MSkim.BeamScrapingFilter;

               // Event selection criteria
               //    see https://twiki.cern.ch/twiki/bin/viewauth/CMS/HIPhotonJe5TeVpp2017PbPb2018
               if(pprimaryVertexFilter == 0 || beamScrapingFilter == 0)
                  continue;

               //HLT trigger to select dimuon events, see Kaya's note: AN2019_143_v12, p.5
               int HLT_HIL2Mu12_2018 = MTrigger.CheckTriggerStartWith("HLT_HIL2Mu12");
               int HLT_HIL3Mu12_2018 = MTrigger.CheckTriggerStartWith("HLT_HIL3Mu12");
               //int HLT_HIL3Mu12_2023 = MTrigger.CheckTriggerStartWith("HLT_HIL3SingleMu12");
               PassHLTSelection = (HLT_HIL3Mu12_2018 == 1 && HLT_HIL2Mu12_2018 == 1);

               MZHadron.NCollWeight = 1;
            }
            else
               MZHadron.NCollWeight = 1;
         }
         else
         {
            if(DoGenLevel == false)
            {
               // JSON selection
               if (JSONHandler != nullptr && !(JSONHandler->isGood(MEvent.Run, MEvent.Lumi)))
                  continue;

               // offline event selection
               int pprimaryVertexFilter = MSkim.PVFilter;
               int beamScrapingFilter = MSkim.BeamScrapingFilter;
               //int phfCoincFilter3 = MSkim.HFCoincidenceFilter; // TODO reforest

               // Event selection criteria
               //    see https://twiki.cern.ch/twiki/bin/viewauth/CMS/HIPhotonJe5TeVpp2017PbPb2018
               if(pprimaryVertexFilter == 0 || beamScrapingFilter == 0 ) // || phfCoincFilter3 == 0)
                  continue;

               int HLT_PAL2Mu12 = MTrigger.CheckTriggerStartWith("HLT_PAL2Mu12");
               int HLT_PAL3Mu12 = MTrigger.CheckTriggerStartWith("HLT_PAL3Mu12");
               PassHLTSelection = (HLT_PAL2Mu12 == 1 && HLT_PAL3Mu12 == 1);

               MZHadron.NCollWeight = 1;
            }
            else
               MZHadron.NCollWeight = 1;
         }

         ///////////////////////////
         ////////// Muons //////////
         ///////////////////////////

         TLorentzVector VGenZ, VGenMu1, VGenMu2;

         if(MMu.NGen > 1)
         {
            for(int igen1 = 0; igen1 < MMu.NGen; igen1++)
            {
               // We only want muon from Z's
               if(MMu.GenMom[igen1] != 23)
                  continue;
               if(MMu.GenPT[igen1] < 20)
                  continue;
               if(fabs(MMu.GenEta[igen1]) > 2.4)
                  continue;

               VGenMu1.SetPtEtaPhiM(MMu.GenPT[igen1],
                     MMu.GenEta[igen1],
                     MMu.GenPhi[igen1],
                     M_MU);

               for(int igen2 = igen1 + 1; igen2 < MMu.NGen; igen2++)
               {
                  // We only want muon from Z's
                  if(MMu.GenMom[igen2] != 23)
                     continue;
                  if(MMu.GenPT[igen2] < 20)
                     continue;
                  if(fabs(MMu.GenEta[igen2]) > 2.4)
                     continue;

                  VGenMu2.SetPtEtaPhiM(MMu.GenPT[igen2],
                        MMu.GenEta[igen2],
                        MMu.GenPhi[igen2],
                        M_MU);

                  VGenZ = VGenMu1 + VGenMu2;

                  if(VGenZ.M() < 60 || VGenZ.M() > 120)
                     continue;
                  if(fabs(VGenZ.Rapidity()) > 2.4)
                     continue;

                  MZHadron.genZMass->push_back(VGenZ.M());
                  MZHadron.genZPt->push_back  (VGenZ.Pt());
                  MZHadron.genZPhi->push_back (VGenZ.Phi());
                  MZHadron.genZEta->push_back (VGenZ.Eta());
                  MZHadron.genZY->push_back   (VGenZ.Rapidity());

                  MZHadron.genMuPt1->push_back(MMu.GenPT[igen1]);
                  MZHadron.genMuPt2->push_back(MMu.GenPT[igen2]);
                  MZHadron.genMuEta1->push_back(MMu.GenEta[igen1]);
                  MZHadron.genMuEta2->push_back(MMu.GenEta[igen2]);
                  MZHadron.genMuPhi1->push_back(MMu.GenPhi[igen1]);
                  MZHadron.genMuPhi2->push_back(MMu.GenPhi[igen2]);

                  double genDeltaMuEta = MMu.GenEta[igen1] - MMu.GenEta[igen2];
                  double genDeltaMuPhi = PhiRangePositive(DeltaPhi(MMu.GenPhi[igen1], MMu.GenPhi[igen2]));

                  MZHadron.genMuDeta->push_back(genDeltaMuEta);
                  MZHadron.genMuDphi->push_back(genDeltaMuPhi);
                  MZHadron.genMuDR->push_back(sqrt(genDeltaMuEta * genDeltaMuEta + genDeltaMuPhi * genDeltaMuPhi));

                  double genDeltaPhiStar = tan((M_PI - genDeltaMuPhi) / 2)
                     * sqrt(1 - tanh(genDeltaMuEta / 2) * tanh(genDeltaMuEta / 2));
                  MZHadron.genMuDphiS->push_back(genDeltaPhiStar);
               }
            }
         }

         for(int ipair = 0; ipair < MMu.NDi; ipair++)
         {
            // We want opposite-charge muons with some basic kinematic cuts
            if(MMu.DiCharge1[ipair] == MMu.DiCharge2[ipair])        continue;
            if(fabs(MMu.DiEta1[ipair]) > 2.4)                       continue;
            if(fabs(MMu.DiEta2[ipair]) > 2.4)                       continue;
            if(fabs(MMu.DiPT1[ipair]) < 20)                         continue;
            if(fabs(MMu.DiPT2[ipair]) < 20)                         continue;
            if(MMu.DimuonPassTightCut(ipair) == false)              continue;
            if(MMu.DiMass[ipair] < 60 || MMu.DiMass[ipair] > 120)   continue;

            TLorentzVector Mu1, Mu2;
            Mu1.SetPtEtaPhiM(MMu.DiPT1[ipair], MMu.DiEta1[ipair], MMu.DiPhi1[ipair], M_MU);
            Mu2.SetPtEtaPhiM(MMu.DiPT2[ipair], MMu.DiEta2[ipair], MMu.DiPhi2[ipair], M_MU);
            TLorentzVector Z = Mu1 + Mu2;
            if(fabs(Z.Rapidity()) > 2.4)
               continue;

            MZHadron.zMass->push_back(MMu.DiMass[ipair]);
            MZHadron.zEta->push_back(MMu.DiEta[ipair]);
            MZHadron.zY->push_back(Z.Rapidity());
            MZHadron.zPhi->push_back(MMu.DiPhi[ipair]);
            MZHadron.zPt->push_back(MMu.DiPT[ipair]);

            MZHadron.muEta1->push_back(MMu.DiEta1[ipair]);
            MZHadron.muEta2->push_back(MMu.DiEta2[ipair]);
            MZHadron.muPhi1->push_back(MMu.DiPhi1[ipair]);
            MZHadron.muPhi2->push_back(MMu.DiPhi2[ipair]);

            MZHadron.muPt1->push_back(MMu.DiPT1[ipair]);
            MZHadron.muPt2->push_back(MMu.DiPT2[ipair]);

            double deltaMuEta = MMu.DiEta1[ipair] - MMu.DiEta2[ipair];
            double deltaMuPhi = PhiRangePositive(DeltaPhi(MMu.DiPhi1[ipair], MMu.DiPhi2[ipair]));

            MZHadron.muDeta->push_back(deltaMuEta);
            MZHadron.muDphi->push_back(deltaMuPhi);
            MZHadron.muDR->push_back(sqrt(deltaMuEta * deltaMuEta + deltaMuPhi * deltaMuPhi));

            double deltaPhiStar = tan((M_PI - deltaMuPhi) / 2) * sqrt(1 - tanh(deltaMuEta / 2) * tanh(deltaMuEta / 2));

            MZHadron.muDphiS->push_back(deltaPhiStar);
         }

         MZHadron.SignalHF = DoGenLevel ? GetGenHFSum(&MGen) : (DoSumET ? MEvent.hiHF : GetHFSum(&MPF));
         MZHadron.SignalVZ = MEvent.vz;
         MZHadron.SubEvent0HF = GetGenHFSum(&MGen, 0);
         MZHadron.SubEventAllHF = GetGenHFSum(&MGen, -1);
         
         bool GoodGenZ = MZHadron.genZPt->size() > 0 && (MZHadron.genZPt->at(0) > MinZPT);
         bool GoodRecoZ = MZHadron.zPt->size() > 0 && (MZHadron.zPt->at(0) > MinZPT);

         if(DoGenLevel == false && GoodRecoZ == true)
         {
            HLTEffDenominator.Fill(MZHadron.zPt->at(0));
            if(PassHLTSelection == true)
               HLTEffNumerator.Fill(MZHadron.zPt->at(0));
         }
         if(DoGenLevel == false && PassHLTSelection == false) continue;
         
         if(CheckZ == true)
         {
            // Yen-Jie: Propose to save the charged hadron information
            //    when either GoodGenZ or GoodRecoZ is identified 
            if(DoGenLevel == true && (GoodGenZ == false && GoodRecoZ == false))
            {
               MZHadron.FillEntry();
               continue;
            }
            if(DoGenLevel == false && GoodRecoZ == false)
            {
               if(WriteAllTrackSelectionTrees == true)
               {
                  MZHadronLoose.CopyNonTrack(MZHadron);
                  MZHadronTight.CopyNonTrack(MZHadron);
                  MZHadron.FillEntry();
                  MZHadronLoose.FillEntry();
                  MZHadronTight.FillEntry();
               }
               else
                  MZHadron.FillEntry();
               continue;
            }
         }

         ///////////////////////////////
         ////////// Z weights //////////
         ///////////////////////////////

         MZHadron.InterSampleZWeight = 1;
         MZHadron.ZWeight = 1;
         if(DoGenLevel == false && GoodRecoZ == true)
         {
            TLorentzVector Z;
            Z.SetPtEtaPhiM(MZHadron.zPt->at(0), MZHadron.zEta->at(0), MZHadron.zPhi->at(0), MZHadron.zMass->at(0));
            
            if(IsData == false)
               //MZHadron.ZWeight = GetZWeightPA8TeVMC(Z.Pt(), Z.Rapidity());
               MZHadron.ZWeight = 1;
            else
            {
               double Mu1Eta = MZHadron.muEta1->at(0);
               double Mu1PT = MZHadron.muPt1->at(0);
               double Mu2Eta = MZHadron.muEta2->at(0);
               double Mu2PT = MZHadron.muPt2->at(0);

               for(int i = 0; i < 12; i++)
                  MZHadron.ExtraZWeight[i] = 1;

               if(IsPP == true)
               {
                  MZHadron.ZWeight = GetPPDimuonTnPWeight(Mu1Eta, Mu2Eta);
                  MZHadron.ExtraZWeight[0] =
                     GetRelativeWeight(tnp_weight_TightID_pp(Mu1Eta, 1), tnp_weight_TightID_pp(Mu1Eta, 0))
                     * GetRelativeWeight(tnp_weight_TightID_pp(Mu2Eta, 1), tnp_weight_TightID_pp(Mu2Eta, 0));
                  MZHadron.ExtraZWeight[1] =
                     GetRelativeWeight(tnp_weight_TightID_pp(Mu1Eta, -1), tnp_weight_TightID_pp(Mu1Eta, 0))
                     * GetRelativeWeight(tnp_weight_TightID_pp(Mu2Eta, -1), tnp_weight_TightID_pp(Mu2Eta, 0));
                  MZHadron.ExtraZWeight[2] =
                     GetRelativeWeight(tnp_weight_L3Mu12_double_pp(Mu1Eta, Mu2Eta, 1),
                        tnp_weight_L3Mu12_double_pp(Mu1Eta, Mu2Eta, 0));
                  MZHadron.ExtraZWeight[3] =
                     GetRelativeWeight(tnp_weight_L3Mu12_double_pp(Mu1Eta, Mu2Eta, -1),
                        tnp_weight_L3Mu12_double_pp(Mu1Eta, Mu2Eta, 0));
               }
               else if(Is8TeV == true)
               {
                  if(UsePADimuonTnPWeights(Mu1PT, Mu1Eta, Mu2PT, Mu2Eta) == true)
                  {
                     MZHadron.ZWeight = GetPADimuonTnPWeight(Mu1PT, Mu1Eta, Mu2PT, Mu2Eta);
                     MZHadron.ExtraZWeight[0] =
                        GetRelativeWeight(tnp_weight_TightID_pPb(Mu1PT, Mu1Eta, 1), tnp_weight_TightID_pPb(Mu1PT, Mu1Eta, 0))
                        * GetRelativeWeight(tnp_weight_TightID_pPb(Mu2PT, Mu2Eta, 1), tnp_weight_TightID_pPb(Mu2PT, Mu2Eta, 0));
                     MZHadron.ExtraZWeight[1] =
                        GetRelativeWeight(tnp_weight_TightID_pPb(Mu1PT, Mu1Eta, -1), tnp_weight_TightID_pPb(Mu1PT, Mu1Eta, 0))
                        * GetRelativeWeight(tnp_weight_TightID_pPb(Mu2PT, Mu2Eta, -1), tnp_weight_TightID_pPb(Mu2PT, Mu2Eta, 0));
                     MZHadron.ExtraZWeight[2] =
                        GetRelativeWeight(tnp_weight_L3Mu12_double_pPb(Mu1PT, Mu1Eta, Mu2PT, Mu2Eta, 1),
                           tnp_weight_L3Mu12_double_pPb(Mu1PT, Mu1Eta, Mu2PT, Mu2Eta, 0));
                     MZHadron.ExtraZWeight[3] =
                        GetRelativeWeight(tnp_weight_L3Mu12_double_pPb(Mu1PT, Mu1Eta, Mu2PT, Mu2Eta, -1),
                           tnp_weight_L3Mu12_double_pPb(Mu1PT, Mu1Eta, Mu2PT, Mu2Eta, 0));
                  }
               }
            }
         }

         ////////////////////////////
         ////////// Tracks //////////
         ////////////////////////////
            
         ZHadronMessenger *CurrentMessenger[3] = {&MZHadron, &MZHadronLoose, &MZHadronTight};
         TrackEfficiencyCorrector *CurrentTrackEfficiency[3] = {TrackEfficiency, TrackEfficiencyLoose, TrackEfficiencyTight};
         TrkEff2017pp *CurrentPPTrackEfficiency[3] = {TrackEfficiencyPP, TrackEfficiencyPPLoose, TrackEfficiencyPPTight};
         string CurrentTrackSelectionMode[3] = {"Nominal", "Loose", "Tight"};
         int ModeCount = 1;

         if(DoGenLevel == false && WriteAllTrackSelectionTrees == true)
         {
            MZHadronLoose.CopyNonTrack(MZHadron);
            MZHadronTight.CopyNonTrack(MZHadron);
            ModeCount = 3;
         }

         for(int iM = 0; iM < ModeCount; iM++)
         {
            ZHadronMessenger *Current = CurrentMessenger[iM];
            int NTrack = DoGenLevel ? MGen.Mult : MTrack.nTrk;
            for(int iT = 0; iT < NTrack; iT++)
            {
               if(DoGenLevel == true)
               {
                  if(MGen.DaughterCount->at(iT) > 0)
                     continue;
                  if(MGen.Charge->at(iT) == 0)
                     continue;
               }
               if(DoGenLevel == false)
               {
                  if(CurrentTrackSelectionMode[iM] == "Loose" && MTrack.PassZHadron2022CutLoose(iT) == false)
                     continue;
                  if(CurrentTrackSelectionMode[iM] == "Nominal" && MTrack.PassZHadron2022Cut(iT) == false)
                     continue;
                  if(CurrentTrackSelectionMode[iM] == "Tight" && MTrack.PassZHadron2022CutTight(iT) == false)
                     continue;
               }

               double TrackEta = DoGenLevel ? MGen.Eta->at(iT)                       : (MTrack.trkEta[iT]);
               double TrackPhi = DoGenLevel ? MGen.Phi->at(iT)                       : (MTrack.trkPhi[iT]);
               double TrackPT  = DoGenLevel ? MGen.PT->at(iT)                        : (MTrack.trkPt[iT]);
               int TrackCharge = DoGenLevel ? MGen.Charge->at(iT)                    : (MTrack.trkCharge[iT]);
               int SubEvent    = DoGenLevel ? (MGen.SubEvent->at(iT) + IsBackground) : 0;
             
               if(TrackPT < MinTrackPT)   continue;
               if(TrackEta < -2.4)        continue;
               if(TrackEta > +2.4)        continue;
             
               TLorentzVector V; 
               V.SetPtEtaPhiM(TrackPT, TrackEta, TrackPhi, 0.139570);

               if(CheckZ == true && (DoGenLevel ? (GoodGenZ == true) : (GoodRecoZ == true)))
               {
                  double Mu1Eta = DoGenLevel ? Current->genMuEta1->at(0) : Current->muEta1->at(0);
                  double Mu1Phi = DoGenLevel ? Current->genMuPhi1->at(0) : Current->muPhi1->at(0);
                  double Mu2Eta = DoGenLevel ? Current->genMuEta2->at(0) : Current->muEta2->at(0);
                  double Mu2Phi = DoGenLevel ? Current->genMuPhi2->at(0) : Current->muPhi2->at(0);

                  double DeltaEtaMu1 = TrackEta - Mu1Eta;
                  double DeltaEtaMu2 = TrackEta - Mu2Eta;
                  double DeltaPhiMu1 = DeltaPhi(TrackPhi, Mu1Phi);
                  double DeltaPhiMu2 = DeltaPhi(TrackPhi, Mu2Phi);

                  double DeltaRMu1 = sqrt(DeltaEtaMu1 * DeltaEtaMu1 + DeltaPhiMu1 * DeltaPhiMu1);
                  double DeltaRMu2 = sqrt(DeltaEtaMu2 * DeltaEtaMu2 + DeltaPhiMu2 * DeltaPhiMu2);

                  bool MuTagged = false;
                  if(DeltaRMu1 < MuonVeto)   MuTagged = true;
                  if(DeltaRMu2 < MuonVeto)   MuTagged = true;
             
                  Current->trackMuTagged->push_back(MuTagged);
                  Current->trackMuDR->push_back(min(DeltaRMu1, DeltaRMu2));
               }
               else
               {
                  Current->trackMuTagged->push_back(false);
                  Current->trackMuDR->push_back(-1);
               }

               Current->trackPhi->push_back(TrackPhi);
               Current->trackEta->push_back(TrackEta);
               Current->trackY->push_back(V.Rapidity());
               Current->trackPt->push_back(TrackPT);
               Current->subevent->push_back(SubEvent);

                double TrackCorrection = 1;
                if(DoGenLevel == false)
                {
                   if(IsPP == true && CurrentPPTrackEfficiency[iM] != nullptr)
                      TrackCorrection = CurrentPPTrackEfficiency[iM]->getCorrection(TrackPT, TrackEta);
                   else if(CurrentTrackEfficiency[iM] != nullptr)
                      TrackCorrection = CurrentTrackEfficiency[iM]->GetCorrection(TrackPT, TrackEta);
                }
               double TrackResidualCorrection = 1;
               // if(DoTrackResidual == true && DoGenLevel == false)
               //    TrackResidualCorrection = TrackResidual.GetCorrectionFactor(TrackPT, TrackEta, TrackPhi, MZHadron.hiBin);
               Current->trackWeight->push_back(TrackCorrection * TrackResidualCorrection);
               Current->trackResidualWeight->push_back(TrackResidualCorrection);
               // MZHadron.trackResidualWeight->push_back(1);
            }

            Current->FillEntry();
         }
      }
   
      Bar.Update(EntryCount);
      Bar.Print();
      Bar.PrintLine();
   
      InputFile.Close();
   }

   OutputFile.cd();
   Tree.Write();
   if(DoGenLevel == false && WriteAllTrackSelectionTrees == true)
   {
      TreeLoose.Write();
      TreeTight.Write();
   }
   InfoTree.Write();
   TDirectory *HltTreeDirectory = OutputFile.mkdir("HltTree");
   TDirectory *TriggerTurnOnDirectory = HltTreeDirectory->mkdir("TriggerTurnOn");
   TriggerTurnOnDirectory->cd();
   HLTEffNumerator.Write();
   HLTEffDenominator.Write();

   OutputFile.Close();

   delete TrackEfficiency;
   delete TrackEfficiencyLoose;
   delete TrackEfficiencyTight;
   delete TrackEfficiencyPP;
   delete TrackEfficiencyPPLoose;
   delete TrackEfficiencyPPTight;
   delete JSONHandler;

   return 0;
}

double GetHFSum(PFTreeMessenger *M)
{
   if(M == nullptr)
      return -1;
   if(M->Tree == nullptr)
      return -1;

   double Sum = 0;
   for(int iPF = 0; iPF < M->ID->size(); iPF++)
   {
      if(fabs(M->Eta->at(iPF)) < 3)
         continue;
      if(fabs(M->Eta->at(iPF)) > 5)
         continue;
      Sum = Sum + M->E->at(iPF);
   }

   // cout << Sum << endl;

   return Sum;
}

double GetGenHFSum(GenParticleTreeMessenger *M, int SubEvent)
{
   if(M == nullptr)
      return -1;
   if(M->Tree == nullptr)
      return -1;

   double Sum = 0;
   for(int iGen = 0; iGen < M->Mult; iGen++)
   {
      if(fabs(M->Eta->at(iGen)) < 3)
         continue;
      if(fabs(M->Eta->at(iGen)) > 5)
         continue;
      if(M->DaughterCount->at(iGen) > 0)
         continue;
      if(M->PT->at(iGen) < 0.4)   // for now...
         continue;

      if(SubEvent >= 0)   // if SubEvent >= 0, check subevent
      {
         if(M->SubEvent->at(iGen) != SubEvent)
            continue;
      }

      Sum = Sum + M->PT->at(iGen) * cosh(M->Eta->at(iGen));
   }

   return Sum;
}
