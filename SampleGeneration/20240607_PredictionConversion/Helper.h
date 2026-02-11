#include <map>
#include <vector>
using namespace std;

#include "include/TLorentzVector.h"

#include "Messenger.h"

void FillAuxiliaryVariables(ZHadronMessenger &M, bool ChargedOnly = true);
double GetGenHFSum(ZHadronMessenger &M);
void SetCharge(ZHadronMessenger &M);
void ZeroNeutrals(ZHadronMessenger &M);
void ZeroUnstables(ZHadronMessenger &M);

void FillAuxiliaryVariables(ZHadronMessenger &M, bool ChargedOnly)
{
   int N = M.trackPt->size();

   // Global thing
   M.NPU = 0;
   M.NCollWeight = 1;
   M.InterSampleZWeight = 1;
   M.NVertex = 1;
   M.ZWeight = 1;
   M.VZWeight = 1;
   M.SignalHF = GetGenHFSum(M);
   M.SubEvent0HF = 0;
   M.SubEventAllHF = M.SignalHF;

   // Tracks message
   SetCharge(M);

   // Muons
   vector<int> Muons;
   vector<int> AntiMuons;
   for(int i = 0; i < N; i++)
   {
      if(M.trackPDFId->at(i) == 13 || M.trackPDFId->at(i) == -13)
         (*M.trackMuTagged)[i] = true;

      if(M.trackWeight->at(i) == 0)
         continue;
      if(M.trackEta->at(i) > 2.4 || M.trackEta->at(i) < -2.4)
         continue;
      if(M.trackPt->at(i) < 20)
         continue;
      
      if(M.trackPDFId->at(i) == 13)
         Muons.push_back(i);
      if(M.trackPDFId->at(i) == -13)
         AntiMuons.push_back(i);
   }

   // Z's
   for(int i1 = 0; i1 < (int)Muons.size(); i1++)
   {
      TLorentzVector Mu1;
      Mu1.SetPtEtaPhiM(M.trackPt->at(Muons[i1]),
         M.trackEta->at(Muons[i1]),
         M.trackPhi->at(Muons[i1]),
         0.105);

      for(int i2 = 0; i2 < (int)AntiMuons.size(); i2++)
      {
         TLorentzVector Mu2;
         Mu2.SetPtEtaPhiM(M.trackPt->at(AntiMuons[i2]),
            M.trackEta->at(AntiMuons[i2]),
            M.trackPhi->at(AntiMuons[i2]),
            0.105);

         TLorentzVector VZ = Mu1 + Mu2;

         if(VZ.M() < 60 || VZ.M() > 120)
            continue;
         if(fabs(VZ.Rapidity()) > 2.4)
            continue;

         M.genZMass->push_back(VZ.M());
         M.genZPt->push_back  (VZ.Pt());
         M.genZPhi->push_back (VZ.Phi());
         M.genZEta->push_back (VZ.Eta());
         M.genZY->push_back   (VZ.Rapidity());

         M.genMuPt1->push_back(Mu1.Pt());
         M.genMuPt2->push_back(Mu2.Pt());
         M.genMuEta1->push_back(Mu1.Eta());
         M.genMuEta2->push_back(Mu2.Eta());
         M.genMuPhi1->push_back(Mu1.Phi());
         M.genMuPhi2->push_back(Mu2.Phi());

         double genDeltaMuEta = Mu1.Eta() - Mu2.Eta();
         double genDeltaMuPhi = Mu1.Phi() - Mu2.Phi();
         if(genDeltaMuPhi < -M_PI)   genDeltaMuPhi = genDeltaMuPhi + 2 * M_PI;
         if(genDeltaMuPhi > +M_PI)   genDeltaMuPhi = genDeltaMuPhi - 2 * M_PI;

         M.genMuDeta->push_back(genDeltaMuEta);
         M.genMuDphi->push_back(genDeltaMuPhi);
         M.genMuDR->push_back(sqrt(genDeltaMuEta * genDeltaMuEta + genDeltaMuPhi * genDeltaMuPhi));

         double genDeltaPhiStar = tan((M_PI - genDeltaMuPhi) / 2)
            * sqrt(1 - tanh(genDeltaMuEta / 2) * tanh(genDeltaMuEta / 2));
         M.genMuDphiS->push_back(genDeltaPhiStar);
      }
   }

   // Finally zero out neutrals...what to do with hole neutrals?
   if(ChargedOnly == true)
      ZeroNeutrals(M);
}

double GetGenHFSum(ZHadronMessenger &M)
{
   double Sum = 0;
   int N = M.trackPt->size();
   for(int iGen = 0; iGen < N; iGen++)
   {
      if(fabs(M.trackEta->at(iGen)) < 3)
         continue;
      if(fabs(M.trackEta->at(iGen)) > 5)
         continue;
      if(M.trackPt->at(iGen) < 0.4)   // for now...
         continue;

      Sum = Sum + M.trackPt->at(iGen) * cosh(M.trackEta->at(iGen)) * M.trackWeight->at(iGen);
   }
   return Sum;
}

void SetCharge(ZHadronMessenger &M)
{
   static map<int, int> Charge;
   if(Charge.size() == 0)
   {
      // Stable things only
      Charge[1] = 999;
      Charge[2] = 999;
      Charge[3] = 999;
      Charge[4] = 999;
      Charge[21] = 999;
      Charge[23] = 999;
      Charge[24] = 999;
      Charge[2101] = 999;
      
      Charge[11] = -1;
      Charge[12] = 0;
      Charge[13] = -1;
      Charge[14] = 0;
      Charge[15] = -1;
      Charge[16] = 0;
      Charge[22] = 0;
      Charge[211] = 1;
      Charge[111] = 0;
      Charge[130] = 0;
      Charge[310] = 0;
      Charge[311] = 0;
      Charge[321] = 1;
      Charge[323] = 1;
      Charge[1114] = -1;
      Charge[2212] = 1;
      Charge[2112] = 0;
      Charge[2114] = 0;
      Charge[2214] = 1;
      Charge[2224] = 2;
      Charge[3214] = 0;
      Charge[3112] = -1;
      Charge[3114] = -1;
      Charge[3222] = 1;
      Charge[3212] = 0;
      Charge[3122] = 0;
      Charge[3312] = -1;
      Charge[3322] = 0;
      Charge[3224] = 1;
      Charge[3334] = -1;
      Charge[313] = 0;
   }

   int N = M.trackPt->size();
   for(int i = 0; i < N; i++)
   {
      if(M.trackWeight->at(i) == 0)   // if it's weight 0 skip
         continue;
      // if(M.trackWeight->at(i) < 0)   // if weight negative it's hole
      //    continue;
      
      int ID = M.trackPDFId->at(i);
      if(Charge.find(ID) != Charge.end())
         (*M.trackCharge)[i] = Charge[ID];
      else if(Charge.find(-ID) != Charge.end())
         (*M.trackCharge)[i] = -Charge[-ID];
      else if (ID != 0)
         cout << "Please add this ID to the list for charge " << ID << endl;
   }
}

void ZeroNeutrals(ZHadronMessenger &M)
{
   int N = M.trackPt->size();
   for(int i = 0; i < N; i++)
   {
      if((*M.trackCharge)[i] == 0)
         (*M.trackWeight)[i] = 0;
   }
}

void ZeroUnstables(ZHadronMessenger &M)
{
   int N = M.trackPt->size();
   for(int i = 0; i < N; i++)
   {
      if((*M.trackCharge)[i] > 100)
         (*M.trackWeight)[i] = 0;
   }
}





