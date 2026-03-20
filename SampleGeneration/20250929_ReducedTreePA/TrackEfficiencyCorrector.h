#ifndef TRKEFF
#define TRKEFF

#include "TFile.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TMath.h"
#include <iostream>
#include <string>

class TrackEfficiencyCorrector
{
public:
   TrackEfficiencyCorrector(std::string FilePath = "");
   ~TrackEfficiencyCorrector();
   double GetCorrection(double pt, double eta);
   bool CheckBounds(double pt, double eta);
private:
   TFile *File;
   TH2D *HEfficiency;
};

TrackEfficiencyCorrector::TrackEfficiencyCorrector(std::string FilePath)
   : File(nullptr), HEfficiency(nullptr)
{
   File = TFile::Open(FilePath.c_str());
   if(File->IsOpen() == true)
      HEfficiency = (TH2D *)File->Get("rTotalEff3D_0");
}

TrackEfficiencyCorrector::~TrackEfficiencyCorrector()
{
   if(File != nullptr)
      File->Close();
}

double TrackEfficiencyCorrector::GetCorrection(double pt, double eta)
{
   if(HEfficiency == nullptr)
      return 0;
   if(CheckBounds(pt, eta) == false)
      return 0;
   return 1 / HEfficiency->GetBinContent(HEfficiency->FindBin(eta, pt));
}

bool TrackEfficiencyCorrector::CheckBounds(double pt, double eta)
{
   if(eta < -2.4 || eta > 2.4)
      return false;
   if(pt < 0 || pt > 500)
      return false;
   return true;
}

#endif
