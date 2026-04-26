#include <algorithm>
#include <iostream>
using namespace std;

#include "TFile.h"
#include "TTree.h"

int main(int argc, char *argv[])
{
   if(argc < 2)
      return 1;

   TFile File(argv[1], "READ");
   if(File.IsZombie() == true)
      return 2;

   TTree *TrackTree = (TTree *)File.Get("ppTrack/trackTree");
   if(TrackTree == nullptr)
      TrackTree = (TTree *)File.Get("PbPbTracks/trackTree");
   TTree *MuonTree = (TTree *)File.Get("hltMuTree/HLTMuTree");
   if(TrackTree == nullptr || MuonTree == nullptr)
      return 3;

   cout << min(TrackTree->GetEntries(), MuonTree->GetEntries()) << endl;
   return 0;
}
