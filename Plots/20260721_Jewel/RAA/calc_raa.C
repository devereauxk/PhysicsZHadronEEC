// JEWEL pPb nuclear modification ratio (Z-tagged):
//   R_pPb(pT) = [1/N_Z dN_ch/dpT]_pPb-hydro / [1/N_Z dN_ch/dpT]_pp-vacuum
// Both samples Z pT-reweighted to the Pythia8+MG spectrum so numerator and
// denominator share identical Z kinematics.
// Event and track selection mirror MainAnalysis/20241102_ZhadronVsZPt defaults
// for the JEWEL runs (gen Z mass 60-120, signed |y_Z| < 1.935, ZPT 0-500,
// muon-tagged track rejection, charged-only, holes kept at weight -0.67 for
// pPb), except the R_AA kinematic region: pT > 0.5 GeV, |eta| < 1.965.
//
// Run: root -l -b -q 'calc_raa.C+'
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TMath.h"
#include <vector>
#include <cstdio>

struct SpecResult { TH1D *h; double nz; };

SpecResult FillSpectrum(const char *skimFile, const char *zWeightFile, bool isPPb,
                        int nBins, const double *edges, const char *name,
                        double minZPt = 0) {
    TFile *fw = TFile::Open(zWeightFile);
    TH1D *hZW = (TH1D*)fw->Get("hPtCorrTotal");
    if (!hZW) { printf("ERROR: no hPtCorrTotal in %s\n", zWeightFile); return {nullptr, 0}; }

    TFile *f = TFile::Open(skimFile);
    TTree *t = (TTree*)f->Get("Tree");
    float eventWeight = 1;
    std::vector<float> *genZPt = nullptr, *genZY = nullptr, *genZMass = nullptr;
    std::vector<float> *trackPt = nullptr, *trackEta = nullptr, *trackWeight = nullptr;
    std::vector<int> *trackCharge = nullptr;
    std::vector<float> *trackMuTagged = nullptr;   // stored as float in the JEWEL skims
    t->SetBranchStatus("*", 0);
    for (auto s : {"EventWeight", "genZPt", "genZY", "genZMass",
                   "trackPt", "trackEta", "trackWeight", "trackCharge", "trackMuTagged"})
        t->SetBranchStatus(s, 1);
    t->SetBranchAddress("EventWeight", &eventWeight);
    t->SetBranchAddress("genZPt", &genZPt);
    t->SetBranchAddress("genZY", &genZY);
    t->SetBranchAddress("genZMass", &genZMass);
    t->SetBranchAddress("trackPt", &trackPt);
    t->SetBranchAddress("trackEta", &trackEta);
    t->SetBranchAddress("trackWeight", &trackWeight);
    t->SetBranchAddress("trackCharge", &trackCharge);
    t->SetBranchAddress("trackMuTagged", &trackMuTagged);

    TH1D *h = new TH1D(name, ";p_{T} (GeV);(1/N_{Z}) dN_{ch}/dp_{T} (GeV^{-1})", nBins, edges);
    h->Sumw2();
    h->SetDirectory(nullptr);
    double nz = 0;

    long long nEv = t->GetEntries();
    for (long long i = 0; i < nEv; ++i) {
        t->GetEntry(i);
        // event selection (main-analysis defaults for JEWEL gen-Z runs)
        if (genZMass->size() == 0) continue;
        if ((*genZMass)[0] < 60 || (*genZMass)[0] > 120) continue;
        float zY = (*genZY)[0];
        if (zY < -1.935 || zY > 1.935) continue;
        float zPt = (*genZPt)[0];
        if (zPt < minZPt || zPt > 500) continue;

        double wEv = eventWeight * hZW->GetBinContent(hZW->FindBin(zPt));
        nz += wEv;

        for (size_t j = 0; j < trackPt->size(); ++j) {
            if ((*trackMuTagged)[j] > 0.5) continue;           // muon rejection (IsMuTagged)
            if (j < trackCharge->size() && (*trackCharge)[j] == 0) continue;  // charged-only
            if ((*trackPt)[j] < 0.5) continue;
            if (fabs((*trackEta)[j]) > 1.965) continue;
            double tw = (*trackWeight)[j];
            if (isPPb && tw < 0) tw *= 0.67;                   // hole charged-share scaling
            h->Fill((*trackPt)[j], wEv * tw);
        }
    }
    f->Close();
    fw->Close();
    printf("%s: weighted N_Z = %.2f\n", name, nz);
    return {h, nz};
}

void calc_raa() {
    // log-uniform pT bins, 0.5 - 80 GeV
    const int nBins = 22;
    double edges[nBins + 1];
    double lo = TMath::Log10(0.5), hi = TMath::Log10(80.0);
    for (int i = 0; i <= nBins; ++i) edges[i] = TMath::Power(10, lo + (hi - lo) * i / nBins);

    const char *wDir = "/home/kdeverea/PhysicsZHadronEEC/Plots/20260721_Jewel/output";
    const char *ppSkim  = "/home/kdeverea/jewel/jewel_pp8160v3_MOD_500k.root";
    const char *ppbSkim = "/home/kdeverea/jewel/jewel_pPb_v4_MOD_500k.root";
    SpecResult pp    = FillSpectrum(ppSkim,  Form("%s/jewel_pp_zptweight.root", wDir),  false, nBins, edges, "hPP");
    SpecResult ppb   = FillSpectrum(ppbSkim, Form("%s/jewel_pPb_zptweight.root", wDir), true,  nBins, edges, "hPPb");
    SpecResult pp30  = FillSpectrum(ppSkim,  Form("%s/jewel_pp_zptweight.root", wDir),  false, nBins, edges, "hPP30",  30);
    SpecResult ppb30 = FillSpectrum(ppbSkim, Form("%s/jewel_pPb_zptweight.root", wDir), true,  nBins, edges, "hPPb30", 30);
    if (!pp.h || !ppb.h || !pp30.h || !ppb30.h) return;

    pp.h->Scale(1.0 / pp.nz, "width");
    ppb.h->Scale(1.0 / ppb.nz, "width");
    pp30.h->Scale(1.0 / pp30.nz, "width");
    ppb30.h->Scale(1.0 / ppb30.nz, "width");

    TH1D *hR = (TH1D*)ppb.h->Clone("hRpPb");
    hR->Divide(pp.h);
    TH1D *hR30 = (TH1D*)ppb30.h->Clone("hRpPb30");
    hR30->Divide(pp30.h);

    // outputs
    gSystem->mkdir("output", true);
    gSystem->mkdir("plots", true);
    TFile *fo = TFile::Open("output/jewel_pPb_raa.root", "RECREATE");
    pp.h->Write("hSpecPP");
    ppb.h->Write("hSpecPPb");
    pp30.h->Write("hSpecPP_ZPT30");
    ppb30.h->Write("hSpecPPb_ZPT30");
    hR->Write("hRpPb");
    hR30->Write("hRpPb_ZPT30");
    fo->Close();

    FILE *ft = fopen("output/jewel_pPb_raa.tsv", "w");
    fprintf(ft, "# ptlo\tpthi\tR_pPb\terr\tR_pPb_ZPT30\terr30\tspecPP\tspecPPb\n");
    for (int b = 1; b <= nBins; ++b)
        fprintf(ft, "%.3f\t%.3f\t%.4f\t%.4f\t%.4f\t%.4f\t%.4e\t%.4e\n",
                hR->GetXaxis()->GetBinLowEdge(b), hR->GetXaxis()->GetBinUpEdge(b),
                hR->GetBinContent(b), hR->GetBinError(b),
                hR30->GetBinContent(b), hR30->GetBinError(b),
                pp.h->GetBinContent(b), ppb.h->GetBinContent(b));
    fclose(ft);

    // plot: spectra (top) + R_pPb (bottom)
    gStyle->SetOptStat(0);
    TCanvas c("c", "", 700, 800);
    TPad *pTop = new TPad("pTop", "", 0, 0.42, 1, 1);
    TPad *pBot = new TPad("pBot", "", 0, 0, 1, 0.42);
    pTop->SetBottomMargin(0.02); pTop->SetLeftMargin(0.15);
    pTop->SetLogx(); pTop->SetLogy();
    pBot->SetTopMargin(0.02); pBot->SetBottomMargin(0.28); pBot->SetLeftMargin(0.15);
    pBot->SetLogx();
    pTop->Draw(); pBot->Draw();

    pTop->cd();
    pp.h->SetLineColor(kRed+1); pp.h->SetMarkerColor(kRed+1);
    pp.h->SetMarkerStyle(20); pp.h->SetMarkerSize(0.8); pp.h->SetLineWidth(2);
    ppb.h->SetLineColor(kBlue+1); ppb.h->SetMarkerColor(kBlue+1);
    ppb.h->SetMarkerStyle(24); ppb.h->SetMarkerSize(0.8); ppb.h->SetLineWidth(2);
    pp30.h->SetLineColor(kRed+1); pp30.h->SetMarkerColor(kRed+1);
    pp30.h->SetMarkerStyle(25); pp30.h->SetMarkerSize(0.8); pp30.h->SetLineWidth(2);
    ppb30.h->SetLineColor(kGreen+2); ppb30.h->SetMarkerColor(kGreen+2);
    ppb30.h->SetMarkerStyle(25); ppb30.h->SetMarkerSize(0.8); ppb30.h->SetLineWidth(2);
    pp.h->GetXaxis()->SetLabelSize(0);
    pp.h->GetYaxis()->SetTitleSize(0.055);
    pp.h->GetYaxis()->SetTitleOffset(1.2);
    pp.h->SetMinimum(pp.h->GetMinimum(0) * 0.3);
    pp.h->SetMaximum(ppb30.h->GetMaximum() * 5);
    pp.h->SetTitle("");
    pp.h->Draw("EP");
    ppb.h->Draw("EP SAME");
    pp30.h->Draw("EP SAME");
    ppb30.h->Draw("EP SAME");

    TLegend *leg = new TLegend(0.20, 0.06, 0.62, 0.30);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.038);
    leg->AddEntry(pp.h, "JEWEL pp vacuum (Z p_{T} rw)", "lep");
    leg->AddEntry(ppb.h, "JEWEL pPb hydro (Z p_{T} rw)", "lep");
    leg->AddEntry(pp30.h, "JEWEL pp vacuum (Z p_{T} rw), p_{T}^{Z} > 30 GeV", "lep");
    leg->AddEntry(ppb30.h, "JEWEL pPb hydro (Z p_{T} rw), p_{T}^{Z} > 30 GeV", "lep");
    leg->Draw();

    TLatex tex; tex.SetNDC(); tex.SetTextSize(0.045);
    tex.DrawLatex(0.42, 0.82, "Z-tagged events, 8.16 TeV");
    tex.DrawLatex(0.42, 0.76, "charged particles, |#eta| < 1.965");

    pBot->cd();
    hR->SetTitle("");
    hR->SetLineColor(kBlue+1); hR->SetMarkerColor(kBlue+1);
    hR->SetMarkerStyle(24); hR->SetMarkerSize(0.8); hR->SetLineWidth(2);
    hR->GetYaxis()->SetTitle("R_{pPb}");
    hR->GetYaxis()->SetTitleSize(0.075); hR->GetYaxis()->SetTitleOffset(0.85);
    hR->GetYaxis()->SetLabelSize(0.06); hR->GetYaxis()->SetNdivisions(505);
    hR->GetXaxis()->SetTitle("p_{T} (GeV)");
    hR->GetXaxis()->SetTitleSize(0.09); hR->GetXaxis()->SetTitleOffset(1.25);
    hR->GetXaxis()->SetLabelSize(0.06);
    hR->GetYaxis()->SetRangeUser(0.4, 1.7);
    hR->Draw("EP");
    hR30->SetTitle("");
    hR30->SetLineColor(kGreen+2); hR30->SetMarkerColor(kGreen+2);
    hR30->SetMarkerStyle(25); hR30->SetMarkerSize(0.8); hR30->SetLineWidth(2);
    hR30->Draw("EP SAME");
    TLegend *legR = new TLegend(0.55, 0.75, 0.95, 0.95);
    legR->SetBorderSize(0); legR->SetFillStyle(0); legR->SetTextSize(0.055);
    legR->AddEntry(hR, "inclusive p_{T}^{Z}", "lep");
    legR->AddEntry(hR30, "p_{T}^{Z} > 30 GeV", "lep");
    legR->Draw();
    TLine *unity = new TLine(0.5, 1, 80, 1);
    unity->SetLineStyle(2); unity->Draw();

    c.SaveAs("plots/jewel_pPb_raa.pdf");
    printf("Wrote output/jewel_pPb_raa.{root,tsv} and plots/jewel_pPb_raa.pdf\n");
}
