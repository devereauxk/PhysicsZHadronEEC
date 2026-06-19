#include <cmath>
#include <map>
#include <vector>
using namespace std;

#include "Messenger.h"

namespace LorentzHelper {
   struct Vec4 { double px, py, pz, e; };
   inline Vec4 FromPtEtaPhiM(double pt, double eta, double phi, double m) {
      double px = pt * cos(phi), py = pt * sin(phi), pz = pt * sinh(eta);
      double e = sqrt(px*px + py*py + pz*pz + m*m);
      return {px, py, pz, e};
   }
   inline double Pt(Vec4 v) { return sqrt(v.px*v.px + v.py*v.py); }
   inline double P(Vec4 v) { return sqrt(v.px*v.px + v.py*v.py + v.pz*v.pz); }
   inline double Eta(Vec4 v) { double p = P(v); return 0.5 * log((p + v.pz) / (p - v.pz)); }
   inline double Phi(Vec4 v) { return atan2(v.py, v.px); }
   inline double Rapidity(Vec4 v) { return 0.5 * log((v.e + v.pz) / (v.e - v.pz)); }
   inline double Mass(Vec4 v) { double m2 = v.e*v.e - v.px*v.px - v.py*v.py - v.pz*v.pz; return m2 > 0 ? sqrt(m2) : 0; }
   inline Vec4 Add(Vec4 a, Vec4 b) { return {a.px+b.px, a.py+b.py, a.pz+b.pz, a.e+b.e}; }
}

void FillAuxiliaryVariables(ZHadronMessenger &M, const vector<bool> &TrackFromZ,
   double SignalHF = 0, bool ChargedOnly = true, bool MirrorToReco = false);
double GetGenHFSum(ZHadronMessenger &M);
int LookupCharge(int ID);
void SetCharge(ZHadronMessenger &M);
void ZeroNeutrals(ZHadronMessenger &M);
void ZeroUnstables(ZHadronMessenger &M);
void ApplyTrackAcceptance(ZHadronMessenger &M, double MinTrackPT = 0.5,
   double MaxTrackEta = 2.4);
void FillGenZ(ZHadronMessenger &M, const vector<bool> &TrackFromZ);
void FillTrackMuonMatching(ZHadronMessenger &M, double MuonVeto = 0.0025);
void MirrorGenToReco(ZHadronMessenger &M);

void FillAuxiliaryVariables(ZHadronMessenger &M, const vector<bool> &TrackFromZ,
   double SignalHF,
   bool ChargedOnly,
   bool MirrorToReco)
{
   M.NPU = 0;
   M.NCollWeight = 1;
   M.InterSampleZWeight = 1;
   M.NVertex = 1;
   M.ZWeight = 1;
   M.VZWeight = 1;
   M.BackgroundHF = 0;
   M.SubEvent0HF = 0;
   M.SubEventAllHF = 0;
   M.SignalVZ = 0;
   for(int i = 0; i < 12; i++)
      M.ExtraZWeight[i] = 1;

   SetCharge(M);
   if(ChargedOnly == true)
      ZeroNeutrals(M);
   ZeroUnstables(M);
   FillGenZ(M, TrackFromZ);
   FillTrackMuonMatching(M);
   M.SignalHF = SignalHF;
   M.SubEvent0HF = M.SignalHF;
   M.SubEventAllHF = M.SignalHF;
   ApplyTrackAcceptance(M);

   if(MirrorToReco == true)
      MirrorGenToReco(M);
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
      if(M.trackPt->at(iGen) < 0.4)
         continue;

      Sum = Sum + M.trackPt->at(iGen) * cosh(M.trackEta->at(iGen))
         * M.trackWeight->at(iGen);
   }
   return Sum;
}

void ApplyTrackAcceptance(ZHadronMessenger &M, double MinTrackPT,
   double MaxTrackEta)
{
   int N = M.trackPt->size();
   for(int i = 0; i < N; i++)
   {
      if(M.trackWeight->at(i) == 0)
         continue;
      if(M.trackPt->at(i) < MinTrackPT)
         (*M.trackWeight)[i] = 0;
      if(fabs(M.trackEta->at(i)) > MaxTrackEta)
         (*M.trackWeight)[i] = 0;
   }
}

void FillGenZ(ZHadronMessenger &M, const vector<bool> &TrackFromZ)
{
   int N = M.trackPt->size();

   vector<int> Muons;
   vector<int> AntiMuons;
   for(int i = 0; i < N; i++)
   {
      if(M.trackWeight->at(i) == 0)
         continue;
      if(M.trackEta->at(i) > 2.4 || M.trackEta->at(i) < -2.4)
         continue;
      if(M.trackPt->at(i) < 20)
         continue;
      if(i >= (int)TrackFromZ.size() || TrackFromZ[i] == false)
         continue;

      if(M.trackPDFId->at(i) == 13)
         Muons.push_back(i);
      if(M.trackPDFId->at(i) == -13)
         AntiMuons.push_back(i);
   }

   using namespace LorentzHelper;
   for(int i1 = 0; i1 < (int)Muons.size(); i1++)
   {
      Vec4 Mu1 = FromPtEtaPhiM(M.trackPt->at(Muons[i1]),
         M.trackEta->at(Muons[i1]),
         M.trackPhi->at(Muons[i1]), 0.105658);

      for(int i2 = 0; i2 < (int)AntiMuons.size(); i2++)
      {
         Vec4 Mu2 = FromPtEtaPhiM(M.trackPt->at(AntiMuons[i2]),
            M.trackEta->at(AntiMuons[i2]),
            M.trackPhi->at(AntiMuons[i2]), 0.105658);

         Vec4 VZ = Add(Mu1, Mu2);

         if(Mass(VZ) < 60 || Mass(VZ) > 120)
            continue;
         if(fabs(Rapidity(VZ)) > 2.4)
            continue;

         M.genZMass->push_back(Mass(VZ));
         M.genZPt->push_back(Pt(VZ));
         M.genZPhi->push_back(Phi(VZ));
         M.genZEta->push_back(Eta(VZ));
         M.genZY->push_back(Rapidity(VZ));

         M.genMuPt1->push_back(Pt(Mu1));
         M.genMuPt2->push_back(Pt(Mu2));
         M.genMuEta1->push_back(Eta(Mu1));
         M.genMuEta2->push_back(Eta(Mu2));
         M.genMuPhi1->push_back(Phi(Mu1));
         M.genMuPhi2->push_back(Phi(Mu2));

         double dEta = Eta(Mu1) - Eta(Mu2);
         double dPhi = Phi(Mu1) - Phi(Mu2);
         if(dPhi < -M_PI)   dPhi = dPhi + 2 * M_PI;
         if(dPhi > +M_PI)   dPhi = dPhi - 2 * M_PI;

         M.genMuDeta->push_back(dEta);
         M.genMuDphi->push_back(dPhi);
         M.genMuDR->push_back(sqrt(dEta * dEta + dPhi * dPhi));

         double DeltaPhiStar = tan((M_PI - dPhi) / 2)
            * sqrt(1 - tanh(dEta / 2) * tanh(dEta / 2));
         M.genMuDphiS->push_back(DeltaPhiStar);
      }
   }
}

void FillTrackMuonMatching(ZHadronMessenger &M, double MuonVeto)
{
   int N = M.trackPt->size();
   for(int i = 0; i < N; i++)
   {
      (*M.trackMuTagged)[i] = false;
      (*M.trackMuDR)[i] = -1;
   }

   if(M.genMuEta1->size() == 0 || M.genMuEta2->size() == 0
      || M.genMuPhi1->size() == 0 || M.genMuPhi2->size() == 0)
      return;

   double Mu1Eta = M.genMuEta1->at(0);
   double Mu1Phi = M.genMuPhi1->at(0);
   double Mu2Eta = M.genMuEta2->at(0);
   double Mu2Phi = M.genMuPhi2->at(0);

   for(int i = 0; i < N; i++)
   {
      double DeltaEtaMu1 = M.trackEta->at(i) - Mu1Eta;
      double DeltaEtaMu2 = M.trackEta->at(i) - Mu2Eta;
      double DeltaPhiMu1 = M.trackPhi->at(i) - Mu1Phi;
      double DeltaPhiMu2 = M.trackPhi->at(i) - Mu2Phi;

      if(DeltaPhiMu1 < -M_PI)   DeltaPhiMu1 = DeltaPhiMu1 + 2 * M_PI;
      if(DeltaPhiMu1 > +M_PI)   DeltaPhiMu1 = DeltaPhiMu1 - 2 * M_PI;
      if(DeltaPhiMu2 < -M_PI)   DeltaPhiMu2 = DeltaPhiMu2 + 2 * M_PI;
      if(DeltaPhiMu2 > +M_PI)   DeltaPhiMu2 = DeltaPhiMu2 - 2 * M_PI;

      double DeltaRMu1 = sqrt(DeltaEtaMu1 * DeltaEtaMu1 + DeltaPhiMu1 * DeltaPhiMu1);
      double DeltaRMu2 = sqrt(DeltaEtaMu2 * DeltaEtaMu2 + DeltaPhiMu2 * DeltaPhiMu2);
      double MinDeltaR = min(DeltaRMu1, DeltaRMu2);

      (*M.trackMuTagged)[i] = (DeltaRMu1 < MuonVeto || DeltaRMu2 < MuonVeto);
      (*M.trackMuDR)[i] = MinDeltaR;
   }
}

void MirrorGenToReco(ZHadronMessenger &M)
{
   *(M.zMass) = *(M.genZMass);
   *(M.zEta) = *(M.genZEta);
   *(M.zY) = *(M.genZY);
   *(M.zPhi) = *(M.genZPhi);
   *(M.zPt) = *(M.genZPt);

   *(M.muPt1) = *(M.genMuPt1);
   *(M.muPt2) = *(M.genMuPt2);
   *(M.muEta1) = *(M.genMuEta1);
   *(M.muEta2) = *(M.genMuEta2);
   *(M.muPhi1) = *(M.genMuPhi1);
   *(M.muPhi2) = *(M.genMuPhi2);
   *(M.muDeta) = *(M.genMuDeta);
   *(M.muDphi) = *(M.genMuDphi);
   *(M.muDR) = *(M.genMuDR);
   *(M.muDphiS) = *(M.genMuDphiS);
}

int LookupCharge(int ID)
{
   static map<int, int> Charge;
   if(Charge.size() == 0)
   {
      Charge[1] = 999;
      Charge[2] = 999;
      Charge[3] = 999;
      Charge[4] = 999;
      Charge[5] = 999;
      Charge[6] = 999;
      Charge[11] = -1;
      Charge[12] = 0;
      Charge[13] = -1;
      Charge[14] = 0;
      Charge[15] = -1;
      Charge[16] = 0;
      Charge[21] = 999;
      Charge[22] = 0;
      Charge[23] = 999;
      Charge[24] = 1;
      Charge[111] = 0;
      Charge[130] = 0;
      Charge[211] = 1;
      Charge[310] = 0;
      Charge[311] = 0;
      Charge[321] = 1;
      Charge[2112] = 0;
      Charge[2212] = 1;
      Charge[3122] = 0;
      Charge[3112] = -1;
      Charge[3222] = 1;
      Charge[3312] = -1;
       Charge[3334] = -1;
    }

   if(Charge.find(ID) != Charge.end())
      return Charge[ID];
   if(Charge.find(-ID) != Charge.end())
      return -Charge[-ID];

   return 999;
}

void SetCharge(ZHadronMessenger &M)
{
   int N = M.trackPt->size();
   for(int i = 0; i < N; i++)
   {
      if(M.trackWeight->at(i) == 0)
         continue;

      int ID = (int)M.trackPDFId->at(i);
      (*M.trackCharge)[i] = LookupCharge(ID);
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
