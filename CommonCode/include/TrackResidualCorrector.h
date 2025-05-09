#include <iostream>

#include "TFile.h"
#include "TH1D.h"
#include "TF2.h"

class TrackResidualCorrector
{
public:
   TrackResidualCorrector(std::string filename = "ResidualCorrector_20230508/totalCorrection.root")
   {
      f = new TFile(filename.c_str());
      hPtCorrTotal  = (TH1D *)f->Get("hPtCorrTotal");
      hEtaCorrTotal = (TH1D *)f->Get("hEtaCorrTotal");
      hPhiCorrTotal = (TH1D *)f->Get("hPhiCorrTotal");
   }

   ~TrackResidualCorrector()
   {
      f->Close();
      delete f;
   }

   double GetCorrectionFactor(double pt, double eta, double phi)
   {
      if(phi < 0)
         phi += 2 * M_PI;

      double PTMax = hPtCorrTotal->GetXaxis()->GetBinUpEdge(hPtCorrTotal->GetNbinsX());

      int bin_pt = hPtCorrTotal->GetXaxis()->FindBin(pt);
      int bin_eta = hEtaCorrTotal->GetXaxis()->FindBin(eta);
      int bin_phi = hPhiCorrTotal->GetXaxis()->FindBin(phi);
     
      // last bin in PT is overflow
      if(pt >= PTMax)
         bin_pt = hPtCorrTotal->GetNbinsX();
      
      double corr = hPtCorrTotal->GetBinContent(bin_pt) *
         hEtaCorrTotal->GetBinContent(bin_eta) *
         hPhiCorrTotal->GetBinContent(bin_phi);

      if(isnan(corr))
      {
         std::cerr << "Error!  nan efficiency! " << bin_pt << " " << bin_eta << " " << bin_phi << std::endl;
         corr = 1;
      }

      return corr;
   }

private:
   TFile* f;
   TH1D *hPtCorrTotal;
   TH1D *hEtaCorrTotal;
   TH1D *hPhiCorrTotal;
};

class TrackResidualCentralityCorrector
{
public:
   TrackResidualCentralityCorrector(std::string F1, std::string F2, std::string F3, std::string F4)
   {
      TRC1 = new TrackResidualCorrector(F1);
      TRC2 = new TrackResidualCorrector(F2);
      TRC3 = new TrackResidualCorrector(F3);
      TRC4 = new TrackResidualCorrector(F4);
   }
   
   TrackResidualCentralityCorrector(std::vector<std::string> F)
   {
      // cout << F.size() << " " << F[0] << endl;
      if(F.size() == 4)
      {
         TRC1 = new TrackResidualCorrector(F[0]);
         TRC2 = new TrackResidualCorrector(F[1]);
         TRC3 = new TrackResidualCorrector(F[2]);
         TRC4 = new TrackResidualCorrector(F[3]);
      }
      else if(F.size() == 1)
      {
         TRC1 = new TrackResidualCorrector(F[0]);
         TRC2 = new TrackResidualCorrector(F[0]);
         TRC3 = new TrackResidualCorrector(F[0]);
         TRC4 = new TrackResidualCorrector(F[0]);
      }
      else
      {
         TRC1 = nullptr;
         TRC2 = nullptr;
         TRC3 = nullptr;
         TRC4 = nullptr;
      }
   }

   ~TrackResidualCentralityCorrector()
   {
      if(TRC1 != nullptr)   delete TRC1;
      if(TRC2 != nullptr)   delete TRC2;
      if(TRC3 != nullptr)   delete TRC3;
      if(TRC4 != nullptr)   delete TRC4;
   }

   double GetCorrectionFactor(double pt, double eta, double phi, int hiBin)
   {
      if(hiBin < 20)         return TRC1->GetCorrectionFactor(pt, eta, phi);
      else if(hiBin < 60)    return TRC2->GetCorrectionFactor(pt, eta, phi);
      else if(hiBin < 100)   return TRC3->GetCorrectionFactor(pt, eta, phi);
      else                   return TRC4->GetCorrectionFactor(pt, eta, phi);
      return 0;
   }

private:
   TrackResidualCorrector *TRC1;
   TrackResidualCorrector *TRC2;
   TrackResidualCorrector *TRC3;
   TrackResidualCorrector *TRC4;
};

class ZCorrector
{
   // extends ZPtEtaCorrector to (Zpt, Zeta) -> (Zpt, Zeta, event mult) correction
   //kyle
public:
   ZCorrector(std::string filename = "ZPtEtaCorrection/totalCorrection.root")
   {
      f = TFile::Open(filename.c_str());
      if (!f || f->IsZombie())
      {
         std::cerr << "Error: Unable to open file " << filename << std::endl;
         return;
      }

      int i = 1;
      while (true)
      {
         TF1* fitFunc_pt = (TF1*)f->Get(Form("fitFunc_pt-%i", i));
         if (!fitFunc_pt) break;
         fitFuncs_pt.push_back(fitFunc_pt);
         i++;
      }

      cout<<"n fitFuncs_pt: "<<fitFuncs_pt.size()<<endl;

      i = 1;
      while (true)
      {
         TF1* fitFunc_eta = (TF1*)f->Get(Form("fitFunc_eta-%i", i));
         if (!fitFunc_eta) break;
         fitFuncs_eta.push_back(fitFunc_eta);
         i++;
      }

      cout<<"n fitFuncs_eta: "<<fitFuncs_eta.size()<<endl;

      i = 1;
      while (true)
      {
         TF1* fitFunc_mult = (TF1*)f->Get(Form("fitFunc_mult-%i", i));
         if (!fitFunc_mult) break;
         fitFuncs_mult.push_back(fitFunc_mult);
         i++;
      }

      cout<<"n fitFuncs_mult: "<<fitFuncs_mult.size()<<endl;
   }

   ~ZCorrector()
   {
      f->Close();
      delete f;
   }

   double GetCorrectionFactor(double Zpt, double Zeta, double mult)
   {
      double corr = 1.;

      // Apply pT corrections
      for (size_t i = 0; i < fitFuncs_pt.size(); i++)
      {
         TF1 *fitFunc_pt = fitFuncs_pt.at(i);

         double xmin, xmax;
         fitFunc_pt->GetRange(xmin, xmax);
         if (Zpt < xmin || Zpt > xmax) continue;

         corr *= fitFunc_pt->Eval(Zpt);
      }

      // Apply eta corrections
      for (size_t i = 0; i < fitFuncs_eta.size(); i++)
      {
         TF1 *fitFunc_eta = fitFuncs_eta.at(i);

         double xmin, xmax;
         fitFunc_eta->GetRange(xmin, xmax);
         if (Zeta < xmin || Zeta > xmax) continue;

         corr *= fitFunc_eta->Eval(Zeta);
      }

      // Apply mult corrections
      for (size_t i = 0; i < fitFuncs_mult.size(); i++)
      {
         TF1 *fitFunc_mult = fitFuncs_mult.at(i);

         double xmin, xmax;
         fitFunc_mult->GetRange(xmin, xmax);
         if (mult < xmin || mult > xmax) continue;

         corr *= fitFunc_mult->Eval(mult);
      }

      return corr;
   }

private:
   TFile* f;
   vector<TF1*> fitFuncs_pt;
   vector<TF1*> fitFuncs_eta;
   vector<TF1*> fitFuncs_mult;
};

class TrackResidualPPbCorrector
{
public:
   TrackResidualPPbCorrector(std::string filename = "residualCorrection/totalCorrection.root")
   {
      f = TFile::Open(filename.c_str());
      if (!f || f->IsZombie())
      {
         std::cerr << "Error: Unable to open file " << filename << std::endl;
         return;
      }

      int i = 1;
      while (true)
      {
         TF1* fitFunc_pt = (TF1*)f->Get(Form("fitFunc_pt-%i", i));
         if (!fitFunc_pt) break;
         fitFuncs_pt.push_back(fitFunc_pt);
         i++;
      }

      cout<<"n fitFuncs_pt: "<<fitFuncs_pt.size()<<endl;

      i = 1;
      while (true)
      {
         TF1* fitFunc_eta = (TF1*)f->Get(Form("fitFunc_eta-%i", i));
         if (!fitFunc_eta) break;
         fitFuncs_eta.push_back(fitFunc_eta);
         i++;
      }

      cout<<"n fitFuncs_eta: "<<fitFuncs_eta.size()<<endl;

      i = 1;
      while (true)
      {
         TF1* fitFunc_phi = (TF1*)f->Get(Form("fitFunc_phi-%i", i));
         if (!fitFunc_phi) break;
         fitFuncs_phi.push_back(fitFunc_phi);
         i++;
      }

      cout<<"n fitFuncs_phi: "<<fitFuncs_phi.size()<<endl;
   }

   ~TrackResidualPPbCorrector()
   {
      f->Close();
      delete f;
   }

   double GetCorrectionFactor(double pt, double eta, double phi)
   {
      double corr = 1.;

      // Apply pT corrections
      for (size_t i = 0; i < fitFuncs_pt.size(); i++)
      {
         TF1 *fitFunc_pt = fitFuncs_pt.at(i);

         double xmin, xmax;
         fitFunc_pt->GetRange(xmin, xmax);
         if (pt < xmin || pt > xmax) continue;

         corr *= fitFunc_pt->Eval(pt);
      }

      // Apply eta corrections
      for (size_t i = 0; i < fitFuncs_eta.size(); i++)
      {
         TF1 *fitFunc_eta = fitFuncs_eta.at(i);

         double xmin, xmax;
         fitFunc_eta->GetRange(xmin, xmax);
         if (eta < xmin || eta > xmax) continue;

         corr *= fitFunc_eta->Eval(eta);
      }

      // Apply phi corrections
      for (size_t i = 0; i < fitFuncs_phi.size(); i++)
      {
         TF1 *fitFunc_phi = fitFuncs_phi.at(i);

         double xmin, xmax;
         fitFunc_phi->GetRange(xmin, xmax);
         if (phi < xmin || phi > xmax) continue;

         corr *= fitFunc_phi->Eval(phi);
      }

      return corr;
   }

private:
   TFile* f;
   vector<TF1*> fitFuncs_pt;
   vector<TF1*> fitFuncs_eta;
   vector<TF1*> fitFuncs_phi;
};
