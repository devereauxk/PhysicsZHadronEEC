// Forest-level study: false positive / false negative rates for track-to-PV matching
// via 3-sigma dxy/dz IP significance cut vs trkAssocVtx ground truth.
//
// Event selection: pPAprimaryVertexFilter, pBeamScrapingFilter, HLT_PAL2Mu12, HLT_PAL3Mu12
// Trees used: ppTrack/trackTree, hltanalysis/HltTree, skimanalysis/HltTree

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
using namespace std;

#include "TBranch.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include "CommandLine.h"
#include "KylesPlotting.h"
#include "SetStyle.h"

static const int MAXTRACKS = 10000;
static const int MAXVTX    = 200;

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);
   vector<string> inputs  = CL.GetStringVector("Input");
   double         sigCut  = CL.GetDouble("SigmaCut", 3.0);
   string         outFile = CL.Get("Output", "output/vtx_assoc_study.root");
   int            maxEvt  = CL.GetInt("MaxEvents", -1);
   string         hlt1    = CL.Get("HLT1", "HLT_PAL2Mu12_v1");
   string         hlt2    = CL.Get("HLT2", "HLT_PAL3Mu12_v1");

   gSystem->mkdir("output", true);
   gROOT->SetBatch(kTRUE);

   // ---- chain all three trees ----
   TChain trkChain("ppTrack/trackTree");
   TChain hltChain("hltanalysis/HltTree");
   TChain skimChain("skimanalysis/HltTree");
   for(auto &f : inputs)
   {
      trkChain.Add(f.c_str());
      hltChain.Add(f.c_str());
      skimChain.Add(f.c_str());
   }
   printf("Loaded %lld entries from %zu files\n", trkChain.GetEntries(), inputs.size());

   // ---- track tree branches ----
   int    nTrk = 0, nVtx = 0, maxPtVtx = 0, nTrkTimesnVtx = 0;
   float  trkDxy1          [MAXTRACKS];
   float  trkDxyErr1       [MAXTRACKS];
   float  trkDz1           [MAXTRACKS];
   float  trkDzErr1        [MAXTRACKS];
   float  trkPt            [MAXTRACKS];
   float  trkEta           [MAXTRACKS];
   // trkAssocVtx is ALL ZEROS in forest due to TrackAnalyzer off-by-one bug:
   //   false written at it*nVtx+i; true written at (trkCount+1)*nVtx+i after increment;
   //   next iteration overwrites the true with false.
   // Truth instead comes from the unrounded per-vertex significance arrays,
   //   which are stored at trkCount BEFORE increment (= track index when all pT > min):
   // Large 2D arrays: 10000*200*4 = 8 MB each — must be static to avoid stack overflow
   static float  trkDxyOverSig    [MAXTRACKS * MAXVTX];
   static float  trkDzOverSig     [MAXTRACKS * MAXVTX];

   trkChain.SetBranchStatus("*", 0);
   trkChain.SetBranchStatus("nTrk",           1); trkChain.SetBranchAddress("nTrk",           &nTrk);
   trkChain.SetBranchStatus("nVtx",           1); trkChain.SetBranchAddress("nVtx",           &nVtx);
   trkChain.SetBranchStatus("maxPtVtx",       1); trkChain.SetBranchAddress("maxPtVtx",       &maxPtVtx);
   trkChain.SetBranchStatus("nTrkTimesnVtx",  1); trkChain.SetBranchAddress("nTrkTimesnVtx",  &nTrkTimesnVtx);
   // trkAssocVtx disabled — zeroed in forest due to off-by-one bug in TrackAnalyzer.cc
   trkChain.SetBranchStatus("trkDxy1",            1); trkChain.SetBranchAddress("trkDxy1",            trkDxy1);
   trkChain.SetBranchStatus("trkDxyError1",       1); trkChain.SetBranchAddress("trkDxyError1",       trkDxyErr1);
   trkChain.SetBranchStatus("trkDz1",             1); trkChain.SetBranchAddress("trkDz1",             trkDz1);
   trkChain.SetBranchStatus("trkDzError1",        1); trkChain.SetBranchAddress("trkDzError1",        trkDzErr1);
   trkChain.SetBranchStatus("trkPt",              1); trkChain.SetBranchAddress("trkPt",              trkPt);
   trkChain.SetBranchStatus("trkEta",             1); trkChain.SetBranchAddress("trkEta",             trkEta);
   trkChain.SetBranchStatus("trkDxyOverDxyError", 1); trkChain.SetBranchAddress("trkDxyOverDxyError", trkDxyOverSig);
   trkChain.SetBranchStatus("trkDzOverDzError",   1); trkChain.SetBranchAddress("trkDzOverDzError",   trkDzOverSig);

   // ---- HLT branches ----
   int hltBit1 = 0, hltBit2 = 0;
   hltChain.SetBranchStatus("*", 0);
   hltChain.SetBranchStatus(hlt1.c_str(), 1); hltChain.SetBranchAddress(hlt1.c_str(), &hltBit1);
   hltChain.SetBranchStatus(hlt2.c_str(), 1); hltChain.SetBranchAddress(hlt2.c_str(), &hltBit2);

   // ---- skim branches ----
   int pPAprimaryVtxFilter = 0, pBeamScrapFilter = 0;
   skimChain.SetBranchStatus("*", 0);
   skimChain.SetBranchStatus("pPAprimaryVertexFilter", 1); skimChain.SetBranchAddress("pPAprimaryVertexFilter", &pPAprimaryVtxFilter);
   skimChain.SetBranchStatus("pBeamScrapingFilter",    1); skimChain.SetBranchAddress("pBeamScrapingFilter",    &pBeamScrapFilter);

   // ---- histograms ----
   // IP significance distributions split by truth
   TH1D hDxySig_truth1("hDxySig_truth1", ";|d_{xy}/#sigma(d_{xy})|;Tracks", 100, 0, 10);
   TH1D hDxySig_truth0("hDxySig_truth0", ";|d_{xy}/#sigma(d_{xy})|;Tracks", 100, 0, 10);
   TH1D hDzSig_truth1 ("hDzSig_truth1",  ";|d_{z}/#sigma(d_{z})|;Tracks",   100, 0, 10);
   TH1D hDzSig_truth0 ("hDzSig_truth0",  ";|d_{z}/#sigma(d_{z})|;Tracks",   100, 0, 10);
   // 2D: dxy sig vs dz sig for FP and FN tracks
   TH2D hSigFP("hSigFP", "False Positives;|d_{xy}/#sigma|;|d_{z}/#sigma|", 50, 0, sigCut, 50, 0, sigCut);
   TH2D hSigFN("hSigFN", "False Negatives;|d_{xy}/#sigma|;|d_{z}/#sigma|", 50, 0, 10, 50, 0, 10);
   // FP/FN vs track pT
   const double ptBins[] = {0.5, 1, 2, 4, 8, 15};
   const int    nPtBins  = 5;
   TH1D hFP_pt("hFP_pt", ";p_{T}^{ch} (GeV);Count", nPtBins, ptBins);
   TH1D hFN_pt("hFN_pt", ";p_{T}^{ch} (GeV);Count", nPtBins, ptBins);
   TH1D hTP_pt("hTP_pt", ";p_{T}^{ch} (GeV);Count", nPtBins, ptBins);
   TH1D hTN_pt("hTN_pt", ";p_{T}^{ch} (GeV);Count", nPtBins, ptBins);
   // NVtx distribution
   TH1D hNVtx("hNVtx", ";N_{vtx};Events", 10, 0, 10);
   // NVtx vs FPR per event
   TH1D hFP_nvtx("hFP_nvtx", ";N_{vtx};Count", 10, 0, 10);
   TH1D hTN_nvtx("hTN_nvtx", ";N_{vtx};Count", 10, 0, 10);

   // ---- event loop ----
   long long nEvtTotal = 0, nEvtPass = 0;
   long long TP = 0, FP = 0, TN = 0, FN = 0;
   long long nTrkTotal = 0;

   // per-nVtx confusion matrix for reporting
   // index 0 = 1 vtx, 1 = 2 vtx, 2 = 3+ vtx
   long long TP_v[3]={}, FP_v[3]={}, TN_v[3]={}, FN_v[3]={};

   long long nEntries = trkChain.GetEntries();
   if(maxEvt > 0 && maxEvt < nEntries) nEntries = maxEvt;

   for(long long iE = 0; iE < nEntries; iE++)
   {
      trkChain.GetEntry(iE);
      hltChain.GetEntry(iE);
      skimChain.GetEntry(iE);
      nEvtTotal++;

      // event selection
      if(!pPAprimaryVtxFilter) continue;
      if(!pBeamScrapFilter)    continue;
      if(!hltBit1)             continue;
      if(!hltBit2)             continue;
      nEvtPass++;

      hNVtx.Fill(nVtx);

      // vtx category for breakdown
      int vcat = (nVtx <= 1) ? 0 : (nVtx == 2) ? 1 : 2;

      for(int it = 0; it < nTrk; it++)
      {
         nTrkTotal++;

         // truth: passes 3-sigma IP significance relative to vertex 0 using unrounded per-vertex
         // arrays (trkDxyOverDxyError / trkDzOverDzError stored before trkCount increment,
         // so index it*nVtx+0 is correct when all tracks pass pT minimum).
         // This is what trkAssocVtx[it*nVtx+0] was supposed to encode before the
         // TrackAnalyzer.cc off-by-one bug zeroed it.
         bool truth = (it * nVtx + 0 < nTrkTimesnVtx) ?
            (fabs(trkDxyOverSig[it * nVtx + 0]) < sigCut && fabs(trkDzOverSig[it * nVtx + 0]) < sigCut) : false;

         // prediction: 3-sigma IP significance cut relative to maxPtVtx (= trkDxy1/trkDz1)
         float dxySig = (trkDxyErr1[it] > 0) ? fabs(trkDxy1[it] / trkDxyErr1[it]) : 1e6;
         float dzSig  = (trkDzErr1[it]  > 0) ? fabs(trkDz1[it]  / trkDzErr1[it])  : 1e6;
         bool  pred   = (dxySig < sigCut && dzSig < sigCut);

         // confusion matrix
         if     ( pred &&  truth) { TP++; TP_v[vcat]++; hTP_pt.Fill(trkPt[it]); }
         else if( pred && !truth) { FP++; FP_v[vcat]++; hFP_pt.Fill(trkPt[it]); hSigFP.Fill(dxySig, dzSig); hFP_nvtx.Fill(nVtx); }
         else if(!pred &&  truth) { FN++; FN_v[vcat]++; hFN_pt.Fill(trkPt[it]); hSigFN.Fill(dxySig, dzSig); }
         else                     { TN++; TN_v[vcat]++; hTN_pt.Fill(trkPt[it]); hTN_nvtx.Fill(nVtx); }

         // IP significance distributions
         if(truth) { hDxySig_truth1.Fill(dxySig); hDzSig_truth1.Fill(dzSig); }
         else      { hDxySig_truth0.Fill(dxySig); hDzSig_truth0.Fill(dzSig); }
      }
   }

   // ---- report ----
   long long P = TP + FN; // total truth-positive tracks
   long long N = TN + FP; // total truth-negative tracks
   double FPR = (N > 0) ? (double)FP / N  : 0;
   double FNR = (P > 0) ? (double)FN / P  : 0;
   double PPV = (TP+FP > 0) ? (double)TP / (TP+FP) : 0; // precision
   double TPR = (P > 0) ? (double)TP / P : 0;            // recall

   printf("\n=== Track-to-PV Vertex Association Study ===\n");
   printf("Triggers: %s && %s\n", hlt1.c_str(), hlt2.c_str());
   printf("Truth: unrounded trkDxyOverDxyError[it*nVtx+0] & trkDzOverDzError[it*nVtx+0] < %.1f sigma\n", sigCut);
   printf("Pred:  rounded trkDxy1/trkDxyError1 (re maxPtVtx) < %.1f sigma\n", sigCut);
   printf("Events total / passing selection: %lld / %lld\n", nEvtTotal, nEvtPass);
   printf("Tracks analysed: %lld  (truth+ = %lld, truth- = %lld)\n", nTrkTotal, P, N);
   printf("Sigma cut: %.1f\n\n", sigCut);
   printf("Confusion matrix:\n");
   printf("              Pred +      Pred -\n");
   printf("  Truth +   %8lld   %8lld   (P=%lld)\n", TP, FN, P);
   printf("  Truth -   %8lld   %8lld   (N=%lld)\n", FP, TN, N);
   printf("\n");
   printf("False Positive Rate  FPR = FP/N  = %.4f  (%.2f%%)\n", FPR, 100*FPR);
   printf("False Negative Rate  FNR = FN/P  = %.4f  (%.2f%%)\n", FNR, 100*FNR);
   printf("Precision            PPV = TP/(TP+FP) = %.4f\n", PPV);
   printf("Recall/Sensitivity   TPR = TP/P  = %.4f\n", TPR);

   const char *vcatName[] = {"nVtx=1", "nVtx=2", "nVtx>=3"};
   printf("\nBreakdown by nVtx:\n");
   printf("  %-10s  %8s  %8s  %8s  %8s  %8s  %8s\n","","TP","FP","TN","FN","FPR","FNR");
   for(int v = 0; v < 3; v++)
   {
      long long Pv = TP_v[v]+FN_v[v], Nv = TN_v[v]+FP_v[v];
      printf("  %-10s  %8lld  %8lld  %8lld  %8lld  %8.4f  %8.4f\n",
             vcatName[v], TP_v[v], FP_v[v], TN_v[v], FN_v[v],
             Nv?double(FP_v[v])/Nv:0, Pv?double(FN_v[v])/Pv:0);
   }

   // ---- save histograms ----
   TFile *fOut = TFile::Open(outFile.c_str(), "RECREATE");
   hDxySig_truth1.Write(); hDxySig_truth0.Write();
   hDzSig_truth1.Write();  hDzSig_truth0.Write();
   hSigFP.Write(); hSigFN.Write();
   hFP_pt.Write(); hFN_pt.Write(); hTP_pt.Write(); hTN_pt.Write();
   hNVtx.Write(); hFP_nvtx.Write(); hTN_nvtx.Write();
   fOut->Close();
   printf("\nHistograms written to %s\n", outFile.c_str());

   return 0;
}
