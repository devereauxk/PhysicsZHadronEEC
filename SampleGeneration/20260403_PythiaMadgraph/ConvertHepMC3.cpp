#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <vector>
using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include "CommandLine.h"
#include "Messenger.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Reader.h"
#include "HepMC3/ReaderFactory.h"

#include "Helper.h"

namespace
{
bool HasAncestorZ(const shared_ptr<HepMC3::GenParticle> &Particle)
{
   if(Particle == nullptr)
      return false;

   vector<shared_ptr<HepMC3::GenParticle>> Pending = {Particle};
   set<const HepMC3::GenParticle *> Visited;
   while(Pending.empty() == false)
   {
      shared_ptr<HepMC3::GenParticle> Current = Pending.back();
      Pending.pop_back();
      if(Current == nullptr)
         continue;
      if(Visited.insert(Current.get()).second == false)
         continue;

      shared_ptr<HepMC3::GenVertex> Vertex = Current->production_vertex();
      if(Vertex == nullptr)
         continue;

      for(const shared_ptr<HepMC3::GenParticle> &Parent : Vertex->particles_in())
      {
         if(Parent == nullptr)
            continue;
         if(abs(Parent->pid()) == 23)
            return true;
         Pending.push_back(Parent);
      }
   }
   return false;
}

bool IsLeaf(const shared_ptr<HepMC3::GenParticle> &Particle)
{
   if(Particle == nullptr)
      return false;

   shared_ptr<HepMC3::GenVertex> Vertex = Particle->end_vertex();
   return (Vertex == nullptr || Vertex->particles_out().empty() == true);
}
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string InputFileName = CL.Get("Input");
   string OutputFileName = CL.Get("Output");
   bool ChargedOnly = CL.GetBool("ChargedOnly", true);
   bool MirrorGenToReco = CL.GetBool("MirrorGenToReco", true);

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");
   TTree OutputTree("Tree", "Tree for ZHadron Pythia+MadGraph samples");
   ZHadronMessenger MZHadron;
   MZHadron.SetBranch(&OutputTree);

   shared_ptr<HepMC3::Reader> Reader = HepMC3::deduce_reader(InputFileName);
   if(Reader == nullptr)
   {
      cerr << "Unable to deduce HepMC reader for " << InputFileName << endl;
      return -1;
   }
   HepMC3::GenEvent Event;

   int EntryCount = 0;
   while(Reader->failed() == false)
   {
      Event.clear();
      Reader->read_event(Event);
      if(Reader->failed() == true)
         break;

      MZHadron.Clear();
      EntryCount = EntryCount + 1;

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
      MZHadron.EventWeight = 1;
      if(Event.weights().empty() == false && std::isfinite(Event.weights()[0]))
         MZHadron.EventWeight = Event.weights()[0];
      MZHadron.NCollWeight = 1;
      MZHadron.InterSampleZWeight = 1;
      MZHadron.ZWeight = 1;
      MZHadron.VZWeight = 1;
      for(int i = 0; i < 12; i++)
         MZHadron.ExtraZWeight[i] = 1;
      MZHadron.NVertex = 1;
      MZHadron.VX = 0;
      MZHadron.VY = 0;
      MZHadron.VZ = 0;
      MZHadron.VXError = 0;
      MZHadron.VYError = 0;
      MZHadron.VZError = 0;
      MZHadron.NPU = 0;

      vector<bool> TrackFromZ;
      double SignalHF = 0;

      for(const shared_ptr<HepMC3::GenParticle> &Particle : Event.particles())
      {
         if(Particle == nullptr)
            continue;
         if(IsLeaf(Particle) == false)
            continue;

         HepMC3::FourVector P = Particle->momentum();
         TLorentzVector V;
         V.SetPxPyPzE(P.px(), P.py(), P.pz(), P.e());

         if(std::isfinite(V.Eta()) == false || std::isfinite(V.Rapidity()) == false)
            continue;

         if(fabs(V.Eta()) >= 3 && fabs(V.Eta()) <= 5 && V.Pt() >= 0.4)
            SignalHF = SignalHF + V.Pt() * cosh(V.Eta());

         int Charge = LookupCharge(Particle->pid());
         if(Charge == 0)
            continue;
         if(abs(Charge) > 100)
            continue;
          if(V.Pt() < 0.5)
             continue;
         if(fabs(V.Eta()) > 2.4)
            continue;

         MZHadron.trackPt->push_back(V.Pt());
         MZHadron.trackPDFId->push_back(Particle->pid());
         MZHadron.trackEta->push_back(V.Eta());
         MZHadron.trackY->push_back(V.Rapidity());
         MZHadron.trackPhi->push_back(V.Phi());
          MZHadron.trackMuTagged->push_back(false);
          MZHadron.trackMuDR->push_back(-1);
          MZHadron.trackWeight->push_back(1);
          MZHadron.trackResidualWeight->push_back(1);
          MZHadron.trackCharge->push_back(Charge);
          MZHadron.subevent->push_back(0);
          TrackFromZ.push_back(abs(Particle->pid()) == 13 && HasAncestorZ(Particle));
      }

      FillAuxiliaryVariables(MZHadron, TrackFromZ, SignalHF, ChargedOnly, MirrorGenToReco);
      MZHadron.FillEntry();
   }

    Reader->close();

   OutputTree.Write();
   OutputFile.Close();

   cout << "Converted " << EntryCount << " events into " << OutputFileName << endl;

   return 0;
}
