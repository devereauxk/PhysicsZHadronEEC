#include <cmath>
#include <iostream>
#include <string>
using namespace std;

#include "TFile.h"
#include "TH2D.h"

#include "CommandLine.h"

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string pPbFile = CL.Get("PPb");
   string pbpFile = CL.Get("PBP");
   string trackRange = CL.Get("TrackRange", "0.5_15");
   string outputFile = CL.Get("Output", "output/MEWeight.root");

   TFile fPPb(pPbFile.c_str());
   TFile fPbP(pbpFile.c_str());

   TH2D *sePPb = (TH2D *)fPPb.Get(("hData_" + trackRange).c_str());
   TH2D *mePPb = (TH2D *)fPPb.Get(("hMixData_" + trackRange).c_str());
   TH2D *sePbP = (TH2D *)fPbP.Get(("hData_" + trackRange).c_str());
   TH2D *mePbP = (TH2D *)fPbP.Get(("hMixData_" + trackRange).c_str());

   if(!sePPb || !mePPb || !sePbP || !mePbP)
   {
      cerr << "Missing histograms in input files" << endl;
      return 1;
   }

   int nx = sePPb->GetNbinsX();
   int ny = sePPb->GetNbinsY();

   TH2D *hSF = (TH2D *)sePPb->Clone("hScaleFactor");
   hSF->SetDirectory(nullptr);
   hSF->Reset("ICES");
   hSF->SetTitle("ME scale factor sf = (SE_Pbp * ME_pPb) / (SE_pPb * ME_Pbp)");

   TH2D *hMEWeight = (TH2D *)sePPb->Clone("hMEWeight");
   hMEWeight->SetDirectory(nullptr);
   hMEWeight->Reset("ICES");
   hMEWeight->SetTitle("ME weight C = 0.5 * (1 + sf)");

   for(int i = 1; i <= nx; i++)
   {
      for(int j = 1; j <= ny; j++)
      {
         double se_ppb = sePPb->GetBinContent(i, j);
         double me_ppb = mePPb->GetBinContent(i, j);
         double se_pbp = sePbP->GetBinContent(i, j);
         double me_pbp = mePbP->GetBinContent(i, j);

         double sf = 1.0;
         if(se_ppb > 0 && me_pbp > 0)
            sf = (se_pbp * me_ppb) / (se_ppb * me_pbp);

         double c = 0.5 * (1.0 + sf);

         hSF->SetBinContent(i, j, sf);
         hMEWeight->SetBinContent(i, j, c);
      }
   }

   cout << "ME weight summary:" << endl;
   cout << "  Bins: " << nx << " x " << ny << endl;
   double sfMean = 0, sfMin = 1e30, sfMax = -1e30;
   for(int i = 1; i <= nx; i++)
      for(int j = 1; j <= ny; j++)
      {
         double sf = hSF->GetBinContent(i, j);
         sfMean += sf;
         sfMin = min(sfMin, sf);
         sfMax = max(sfMax, sf);
      }
   sfMean /= (nx * ny);
   cout << "  sf: mean=" << sfMean << " min=" << sfMin << " max=" << sfMax << endl;
   cout << "  C: mean=" << 0.5 * (1 + sfMean) << " range=[" << 0.5 * (1 + sfMin) << ", " << 0.5 * (1 + sfMax) << "]" << endl;

   TFile fOut(outputFile.c_str(), "RECREATE");
   hSF->Write();
   hMEWeight->Write();
   fOut.Close();

   cout << "Written: " << outputFile << endl;

   return 0;
}
