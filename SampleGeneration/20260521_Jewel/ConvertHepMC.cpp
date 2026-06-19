#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#include "TFile.h"
#include "TTree.h"

#include "CommandLine.h"
#include "Messenger.h"

#include "Helper.h"

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string InputFileName = CL.Get("Input");
   string OutputFileName = CL.Get("Output");

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");
   TTree OutputTree("Tree", "Tree for ZHadron Jewel samples");
   ZHadronMessenger MZHadron;
   MZHadron.SetBranch(&OutputTree);

   ifstream In(InputFileName.c_str());

   int EntryCount = 0;
   bool InEvent = false;

   struct Particle { int PID; double PX, PY, PZ, E, M; int Status; };
   vector<Particle> Particles;
   double EventWeight = 1;

   auto ProcessEvent = [&]()
   {
      if(Particles.empty())
         return;

      MZHadron.Clear();
      EntryCount++;

      MZHadron.Run = 1;
      MZHadron.Lumi = 1;
      MZHadron.Event = EntryCount;
      MZHadron.hiBin = 0;
      MZHadron.hiBinUp = 0;
      MZHadron.hiBinDown = 0;
      MZHadron.hiHF = 0;
      MZHadron.SignalHF = 0;
      MZHadron.BackgroundHF = 0;
      MZHadron.SubEvent0HF = 0;
      MZHadron.SubEventAllHF = 0;
      MZHadron.SignalVZ = 0;
      MZHadron.EventWeight = EventWeight;
      MZHadron.NCollWeight = 1;
      MZHadron.InterSampleZWeight = 1;
      MZHadron.ZWeight = 1;
      MZHadron.VZWeight = 1;
      for(int i = 0; i < 12; i++)
         MZHadron.ExtraZWeight[i] = 1;
      MZHadron.NVertex = 1;
      MZHadron.VX = 0;  MZHadron.VY = 0;  MZHadron.VZ = 0;
      MZHadron.VXError = 0;  MZHadron.VYError = 0;  MZHadron.VZError = 0;
      MZHadron.NPU = 0;

      vector<bool> TrackFromZ;
      double SignalHF = 0;

      using namespace LorentzHelper;
      for(auto &P : Particles)
      {
         if(P.Status != 1)
            continue;
         if(P.PID == 0)
            continue;

         Vec4 V = {P.PX, P.PY, P.PZ, P.E};
         double pt = Pt(V), eta = Eta(V), rap = Rapidity(V), phi = Phi(V);

         if(!isfinite(eta) || !isfinite(rap))
            continue;

         if(fabs(eta) >= 3 && fabs(eta) <= 5 && pt >= 0.4)
            SignalHF += pt * cosh(eta);

         int Charge = LookupCharge(P.PID);
         if(Charge == 0)
            continue;
         if(abs(Charge) > 100)
            continue;
         if(pt < 0.5)
            continue;
         if(fabs(eta) > 2.4)
            continue;

         MZHadron.trackPt->push_back(pt);
         MZHadron.trackPDFId->push_back(P.PID);
         MZHadron.trackEta->push_back(eta);
         MZHadron.trackY->push_back(rap);
         MZHadron.trackPhi->push_back(phi);
         MZHadron.trackMuTagged->push_back(false);
         MZHadron.trackMuDR->push_back(-1);
         MZHadron.trackWeight->push_back(1);
         MZHadron.trackResidualWeight->push_back(1);
         MZHadron.trackCharge->push_back(Charge);
         MZHadron.subevent->push_back(0);
         TrackFromZ.push_back(abs(P.PID) == 13);
      }

      FillAuxiliaryVariables(MZHadron, TrackFromZ, SignalHF, true, true);
      MZHadron.FillEntry();
   };

   string Line;
   while(getline(In, Line))
   {
      if(Line.empty())
         continue;

      istringstream SS(Line);
      string Type;
      SS >> Type;

      if(Type == "E")
      {
         if(InEvent)
            ProcessEvent();

         InEvent = true;
         Particles.clear();
         EventWeight = 1;

         double Dummy;
         for(int i = 0; i < 10; i++)
            SS >> Dummy;

         int NRandom = 0;
         SS >> NRandom;
         int IRand;
         for(int i = 0; i < NRandom; i++)
            SS >> IRand;

         int NWeight = 0;
         SS >> NWeight;
         if(NWeight > 0)
         {
            EventWeight = 1;
            for(int i = 0; i < NWeight; i++)
            {
               double W;
               SS >> W;
               EventWeight *= W;
            }
         }
      }
      else if(Type == "P" && InEvent)
      {
         Particle P;
         int Index;
         SS >> Index >> P.PID >> P.PX >> P.PY >> P.PZ >> P.E >> P.M >> P.Status;
         Particles.push_back(P);
      }
   }

   if(InEvent)
      ProcessEvent();

   In.close();
   OutputTree.Write();
   OutputFile.Close();

   cout << "Converted " << EntryCount << " events into " << OutputFileName << endl;
   return 0;
}
