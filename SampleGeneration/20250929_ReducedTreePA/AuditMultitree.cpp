#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "TFile.h"
#include "TTree.h"

#include "CommandLine.h"
#include "Messenger.h"

bool CloseFloat(float A, float B, float Eps = 1e-6)
{
   return fabs(A - B) <= Eps;
}

template <class T>
bool CompareVector(const vector<T> *A, const vector<T> *B)
{
   if(A == nullptr || B == nullptr)
      return A == B;
   if(A->size() != B->size())
      return false;
   for(int i = 0; i < (int)A->size(); i++)
      if((*A)[i] != (*B)[i])
         return false;
   return true;
}

bool CompareFloatVector(const vector<float> *A, const vector<float> *B)
{
   if(A == nullptr || B == nullptr)
      return A == B;
   if(A->size() != B->size())
      return false;
   for(int i = 0; i < (int)A->size(); i++)
      if(CloseFloat((*A)[i], (*B)[i]) == false)
         return false;
   return true;
}

string SetToString(const set<int> &Values)
{
   stringstream out;
   out << "[";
   bool first = true;
   for(int value : Values)
   {
      if(first == false)
         out << ", ";
      out << value;
      first = false;
   }
   out << "]";
   return out.str();
}

bool CompareNonTrack(ZHadronMessenger &A, ZHadronMessenger &B, string &Reason, bool IgnoreHiBinSyst)
{
   if(A.Run != B.Run) { Reason = "Run"; return false; }
   if(A.Event != B.Event) { Reason = "Event"; return false; }
   if(A.Lumi != B.Lumi) { Reason = "Lumi"; return false; }
   if(A.hiBin != B.hiBin) { Reason = "hiBin"; return false; }
   if(IgnoreHiBinSyst == false)
   {
      if(A.hiBinUp != B.hiBinUp) { Reason = "hiBinUp"; return false; }
      if(A.hiBinDown != B.hiBinDown) { Reason = "hiBinDown"; return false; }
   }
   if(CloseFloat(A.hiHF, B.hiHF) == false) { Reason = "hiHF"; return false; }
   if(CloseFloat(A.SignalHF, B.SignalHF) == false) { Reason = "SignalHF"; return false; }
   if(CloseFloat(A.BackgroundHF, B.BackgroundHF) == false) { Reason = "BackgroundHF"; return false; }
   if(CloseFloat(A.SubEvent0HF, B.SubEvent0HF) == false) { Reason = "SubEvent0HF"; return false; }
   if(CloseFloat(A.SubEventAllHF, B.SubEventAllHF) == false) { Reason = "SubEventAllHF"; return false; }
   if(CloseFloat(A.SignalVZ, B.SignalVZ) == false) { Reason = "SignalVZ"; return false; }
   if(CloseFloat(A.EventWeight, B.EventWeight) == false) { Reason = "EventWeight"; return false; }
   if(CloseFloat(A.NCollWeight, B.NCollWeight) == false) { Reason = "NCollWeight"; return false; }
   if(CloseFloat(A.InterSampleZWeight, B.InterSampleZWeight) == false) { Reason = "InterSampleZWeight"; return false; }
   if(CloseFloat(A.ZWeight, B.ZWeight) == false) { Reason = "ZWeight"; return false; }
   if(CloseFloat(A.VZWeight, B.VZWeight) == false) { Reason = "VZWeight"; return false; }
   for(int i = 0; i < 12; i++)
      if(CloseFloat(A.ExtraZWeight[i], B.ExtraZWeight[i]) == false) { Reason = "ExtraZWeight"; return false; }
   if(A.NVertex != B.NVertex) { Reason = "NVertex"; return false; }
   if(CloseFloat(A.VX, B.VX) == false) { Reason = "VX"; return false; }
   if(CloseFloat(A.VY, B.VY) == false) { Reason = "VY"; return false; }
   if(CloseFloat(A.VZ, B.VZ) == false) { Reason = "VZ"; return false; }
   if(CloseFloat(A.VXError, B.VXError) == false) { Reason = "VXError"; return false; }
   if(CloseFloat(A.VYError, B.VYError) == false) { Reason = "VYError"; return false; }
   if(CloseFloat(A.VZError, B.VZError) == false) { Reason = "VZError"; return false; }
   if(A.NPU != B.NPU) { Reason = "NPU"; return false; }
   if(CompareFloatVector(A.zMass, B.zMass) == false) { Reason = "zMass"; return false; }
   if(CompareFloatVector(A.zEta, B.zEta) == false) { Reason = "zEta"; return false; }
   if(CompareFloatVector(A.zY, B.zY) == false) { Reason = "zY"; return false; }
   if(CompareFloatVector(A.zPhi, B.zPhi) == false) { Reason = "zPhi"; return false; }
   if(CompareFloatVector(A.zPt, B.zPt) == false) { Reason = "zPt"; return false; }
   if(CompareFloatVector(A.genZMass, B.genZMass) == false) { Reason = "genZMass"; return false; }
   if(CompareFloatVector(A.genZEta, B.genZEta) == false) { Reason = "genZEta"; return false; }
   if(CompareFloatVector(A.genZY, B.genZY) == false) { Reason = "genZY"; return false; }
   if(CompareFloatVector(A.genZPhi, B.genZPhi) == false) { Reason = "genZPhi"; return false; }
   if(CompareFloatVector(A.genZPt, B.genZPt) == false) { Reason = "genZPt"; return false; }
   if(CompareFloatVector(A.muEta1, B.muEta1) == false) { Reason = "muEta1"; return false; }
   if(CompareFloatVector(A.muEta2, B.muEta2) == false) { Reason = "muEta2"; return false; }
   if(CompareFloatVector(A.muPhi1, B.muPhi1) == false) { Reason = "muPhi1"; return false; }
   if(CompareFloatVector(A.muPhi2, B.muPhi2) == false) { Reason = "muPhi2"; return false; }
   if(CompareFloatVector(A.muPt1, B.muPt1) == false) { Reason = "muPt1"; return false; }
   if(CompareFloatVector(A.muPt2, B.muPt2) == false) { Reason = "muPt2"; return false; }
   if(CompareFloatVector(A.muDeta, B.muDeta) == false) { Reason = "muDeta"; return false; }
   if(CompareFloatVector(A.muDphi, B.muDphi) == false) { Reason = "muDphi"; return false; }
   if(CompareFloatVector(A.muDR, B.muDR) == false) { Reason = "muDR"; return false; }
   if(CompareFloatVector(A.muDphiS, B.muDphiS) == false) { Reason = "muDphiS"; return false; }
   if(CompareFloatVector(A.genMuPt1, B.genMuPt1) == false) { Reason = "genMuPt1"; return false; }
   if(CompareFloatVector(A.genMuPt2, B.genMuPt2) == false) { Reason = "genMuPt2"; return false; }
   if(CompareFloatVector(A.genMuEta1, B.genMuEta1) == false) { Reason = "genMuEta1"; return false; }
   if(CompareFloatVector(A.genMuEta2, B.genMuEta2) == false) { Reason = "genMuEta2"; return false; }
   if(CompareFloatVector(A.genMuPhi1, B.genMuPhi1) == false) { Reason = "genMuPhi1"; return false; }
   if(CompareFloatVector(A.genMuPhi2, B.genMuPhi2) == false) { Reason = "genMuPhi2"; return false; }
   if(CompareFloatVector(A.genMuDeta, B.genMuDeta) == false) { Reason = "genMuDeta"; return false; }
   if(CompareFloatVector(A.genMuDphi, B.genMuDphi) == false) { Reason = "genMuDphi"; return false; }
   if(CompareFloatVector(A.genMuDR, B.genMuDR) == false) { Reason = "genMuDR"; return false; }
   if(CompareFloatVector(A.genMuDphiS, B.genMuDphiS) == false) { Reason = "genMuDphiS"; return false; }
   return true;
}

bool CompareTracks(ZHadronMessenger &A, ZHadronMessenger &B, string &Reason)
{
   if(CompareFloatVector(A.trackPt, B.trackPt) == false) { Reason = "trackPt"; return false; }
   if(CompareFloatVector(A.trackEta, B.trackEta) == false) { Reason = "trackEta"; return false; }
   if(CompareFloatVector(A.trackY, B.trackY) == false) { Reason = "trackY"; return false; }
   if(CompareFloatVector(A.trackPhi, B.trackPhi) == false) { Reason = "trackPhi"; return false; }
   if(CompareVector(A.trackMuTagged, B.trackMuTagged) == false) { Reason = "trackMuTagged"; return false; }
   if(CompareFloatVector(A.trackMuDR, B.trackMuDR) == false) { Reason = "trackMuDR"; return false; }
   if(CompareFloatVector(A.trackWeight, B.trackWeight) == false) { Reason = "trackWeight"; return false; }
   if(CompareFloatVector(A.trackResidualWeight, B.trackResidualWeight) == false) { Reason = "trackResidualWeight"; return false; }
   if(CompareVector(A.trackCharge, B.trackCharge) == false) { Reason = "trackCharge"; return false; }
   if(CompareVector(A.subevent, B.subevent) == false) { Reason = "subevent"; return false; }
   return true;
}

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string Label = CL.Get("Label", "Audit");
   string OriginalFileName = CL.Get("Original");
   string TestFileName = CL.Get("Test");

   TFile OriginalFile(OriginalFileName.c_str(), "READ");
   TFile TestFile(TestFileName.c_str(), "READ");
   TTree *OriginalTree = (TTree *)OriginalFile.Get("Tree");
   TTree *TestTree = (TTree *)TestFile.Get("Tree");
   TTree *LooseTree = (TTree *)TestFile.Get("TreeLoose");
   TTree *TightTree = (TTree *)TestFile.Get("TreeTight");

   if(OriginalTree == nullptr || TestTree == nullptr)
   {
      cerr << "Missing Tree in one of the files for label " << Label << endl;
      return 1;
   }

   ZHadronMessenger Original(OriginalTree);
   ZHadronMessenger Test(TestTree);

   cout << "LABEL " << Label << endl;
   cout << "original_entries " << Original.GetEntries() << endl;
   cout << "test_entries " << Test.GetEntries() << endl;

   set<int> OriginalHiBinUp;
   set<int> TestHiBinUp;
   bool CompareOK = (Original.GetEntries() == Test.GetEntries());
   string FirstMismatch = "";
   int FirstMismatchEntry = -1;

   int NEntry = min(Original.GetEntries(), Test.GetEntries());
   for(int i = 0; i < NEntry; i++)
   {
      Original.GetEntry(i);
      Test.GetEntry(i);
      OriginalHiBinUp.insert(Original.hiBinUp);
      TestHiBinUp.insert(Test.hiBinUp);
      if(CompareOK == true)
      {
         string Reason = "";
         if(CompareNonTrack(Original, Test, Reason, true) == false || CompareTracks(Original, Test, Reason) == false)
         {
            CompareOK = false;
            FirstMismatch = Reason;
            FirstMismatchEntry = i;
         }
      }
   }

   cout << "original_hiBinUp_values " << SetToString(OriginalHiBinUp) << endl;
   cout << "test_hiBinUp_values " << SetToString(TestHiBinUp) << endl;
   cout << "nominal_equal_except_hiBinUpDown " << (CompareOK ? "yes" : "no") << endl;
   if(CompareOK == false)
      cout << "first_nominal_mismatch entry=" << FirstMismatchEntry << " branch=" << FirstMismatch << endl;

   if(LooseTree != nullptr && TightTree != nullptr)
   {
      ZHadronMessenger Loose(LooseTree);
      ZHadronMessenger Tight(TightTree);
      bool EventOK = true;
      bool OrderOK = true;
      string EventReason = "";
      int EventMismatchEntry = -1;
      int OrderMismatchEntry = -1;
      int OrderLoose = -1, OrderNominal = -1, OrderTight = -1;

      for(int i = 0; i < Test.GetEntries(); i++)
      {
         Test.GetEntry(i);
         Loose.GetEntry(i);
         Tight.GetEntry(i);

         if(EventOK == true)
         {
            string Reason = "";
            if(CompareNonTrack(Test, Loose, Reason, false) == false)
            {
               EventOK = false;
               EventReason = string("Tree vs TreeLoose: ") + Reason;
               EventMismatchEntry = i;
            }
            else if(CompareNonTrack(Test, Tight, Reason, false) == false)
            {
               EventOK = false;
               EventReason = string("Tree vs TreeTight: ") + Reason;
               EventMismatchEntry = i;
            }
         }

         int NNominal = (Test.trackPt == nullptr ? -1 : (int)Test.trackPt->size());
         int NLoose = (Loose.trackPt == nullptr ? -1 : (int)Loose.trackPt->size());
         int NTight = (Tight.trackPt == nullptr ? -1 : (int)Tight.trackPt->size());
         if(OrderOK == true && !(NLoose >= NNominal && NNominal >= NTight))
         {
            OrderOK = false;
            OrderMismatchEntry = i;
            OrderLoose = NLoose;
            OrderNominal = NNominal;
            OrderTight = NTight;
         }
      }

      cout << "event_info_equal_across_trees " << (EventOK ? "yes" : "no") << endl;
      if(EventOK == false)
         cout << "first_event_mismatch entry=" << EventMismatchEntry << " detail=" << EventReason << endl;
      cout << "track_order_loose_ge_nominal_ge_tight " << (OrderOK ? "yes" : "no") << endl;
      if(OrderOK == false)
         cout << "first_track_order_mismatch entry=" << OrderMismatchEntry
            << " loose=" << OrderLoose << " nominal=" << OrderNominal << " tight=" << OrderTight << endl;
   }
   else
   {
      cout << "event_info_equal_across_trees not_applicable" << endl;
      cout << "track_order_loose_ge_nominal_ge_tight not_applicable" << endl;
   }

   return 0;
}
