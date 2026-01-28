#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "CommandLine.h"
#include "Messenger.h"

#include "Helper.h"

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string InputFileName  = CL.Get("Input");
   string OutputFileName = CL.Get("Output");
   int NegativeID        = CL.GetInteger("NegativeID", -999999);
   bool ChargedOnly      = CL.GetBool("ChargedOnly", true);
   bool ZeroOutNegative  = CL.GetBool("ZeroOutNegative", false);

   TFile OutputFile(OutputFileName.c_str(), "RECREATE");

   TTree OutputTree("Tree", "");
   ZHadronMessenger MZHadron;
   MZHadron.SetBranch(&OutputTree);

   ifstream in(InputFileName);

   while(in)
   {
      char ch[10240];
      ch[0] = '\0';
      in.getline(ch, 10239, '\n');
      if(ch[0] == '\0')
         continue;

      stringstream str(ch);

      vector<string> list;
      while(str)
      {
         string Temp = "";
         str >> Temp;
         if(Temp != "")
            list.push_back(Temp);
      }

      if(list.size() == 0)
         continue;
      if(list[0][0] == '#')
         continue;
      if(list[0].size() > 1)
         continue;
      if(list[0] == "E")
      {
         if(MZHadron.trackPt->size() > 0)
         {
            FillAuxiliaryVariables(MZHadron, ChargedOnly);
            MZHadron.FillEntry();
         }
         MZHadron.Clear();
         MZHadron.EventWeight = stof(list[list.size()-1]);
         continue;
      }
      if(list[0] == "P")
      {
         int ID     = stoi(list[2]);
         double px  = stof(list[3]);
         double py  = stof(list[4]);
         double pz  = stof(list[5]);
         double e   = stof(list[6]);
         double m   = stof(list[7]);
         int Status = stoi(list[8]);

         double p   = sqrt(px * px + py * py + pz * pz);
         double eta = 0.5 * log((p + pz) / (p - pz));
         double y   = 0.5 * log((e + pz) / (e - pz));

         double phi = 0;
         if(px == 0 && py > 0)        phi = M_PI / 2;
         else if(px == 0 && py < 0)   phi = -M_PI / 2;
         else                         phi = atan(py / px);
         if(px < 0)                   phi = phi + M_PI;
         if(phi > M_PI)               phi = phi - 2 * M_PI;

         double Weight = 0;
         if(Status == 1)
            Weight = 1;
         if(Status == NegativeID)
            Weight = -1;
         if(ZeroOutNegative == true && Weight < 0)
            Weight = 0;

         MZHadron.trackPt->push_back(sqrt(px * px + py * py));
         MZHadron.trackPDFId->push_back(ID);
         MZHadron.trackEta->push_back(eta);
         MZHadron.trackY->push_back(y);
         MZHadron.trackPhi->push_back(phi);
         MZHadron.trackMuTagged->push_back(false);
         MZHadron.trackMuDR->push_back(false);
         MZHadron.trackWeight->push_back(Weight);
         MZHadron.trackResidualWeight->push_back(1);
         MZHadron.trackCharge->push_back(999);
         MZHadron.subevent->push_back(0);
      }
   }
   if(MZHadron.trackPt->size() > 0)
   {
      FillAuxiliaryVariables(MZHadron, ChargedOnly);
      MZHadron.FillEntry();
   }

   in.close();

   OutputTree.Write();
   OutputFile.Close();

   return 0;
}





