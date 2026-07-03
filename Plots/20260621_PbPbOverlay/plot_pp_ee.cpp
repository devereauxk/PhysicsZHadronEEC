#include <TFile.h>
#include <TH1D.h>
#include <TH3D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TStyle.h>
#include <TMath.h>
#include <iostream>
#include <string>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    string eeWeightFile = CL.Get("EEWeightFile");
    string ppNoEEFile   = CL.Get("PPNoEEFile");
    string ppEEFile     = CL.Get("PPEEFile");
    string ppbNosubFile = CL.Get("PPbNosubFile");
    string pbpNosubFile = CL.Get("PbPNosubFile");
    string outputDir    = CL.Get("OutputDir", "plots/pp-ee");

    SetTDRStyle();

    // -----------------------------------------------------------------------
    // Plot 1: EE correction factor vs Z pT
    // -----------------------------------------------------------------------
    {
        TFile *fEE = TFile::Open(eeWeightFile.c_str(), "READ");
        if (!fEE || fEE->IsZombie()) {
            cerr << "Error: cannot open " << eeWeightFile << endl;
            return 1;
        }
        TH1D *hCorr = (TH1D *)fEE->Get("hPtCorrTotal");
        if (!hCorr) {
            cerr << "Error: hPtCorrTotal not found" << endl;
            return 1;
        }
        hCorr = (TH1D *)hCorr->Clone("hCorr_clone");
        hCorr->SetDirectory(nullptr);
        fEE->Close();

        TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
        c1->SetLeftMargin(0.15);
        c1->SetRightMargin(0.05);
        c1->SetTopMargin(0.08);
        c1->SetBottomMargin(0.13);
        c1->SetLogx();

        TH1D *frame1 = new TH1D("frame1", "", 1, 0.5, 100);
        frame1->SetMinimum(0.35);
        frame1->SetMaximum(1.65);
        frame1->GetXaxis()->SetTitle("p_{T}^{Z} (GeV)");
        frame1->GetYaxis()->SetTitle("Correction factor");
        frame1->GetXaxis()->SetTitleSize(0.045);
        frame1->GetYaxis()->SetTitleSize(0.045);
        frame1->GetXaxis()->SetLabelSize(0.04);
        frame1->GetYaxis()->SetLabelSize(0.04);
        frame1->GetYaxis()->SetTitleOffset(1.5);
        frame1->GetXaxis()->SetMoreLogLabels();
        frame1->GetXaxis()->SetNoExponent();
        frame1->Draw();

        TLine *line = new TLine(0.5, 1.0, 100, 1.0);
        line->SetLineStyle(2);
        line->SetLineColor(kGray + 2);
        line->Draw("SAME");

        hCorr->SetLineColor(cmsBlue);
        hCorr->SetLineWidth(2);
        hCorr->GetXaxis()->SetRangeUser(0.5, 100);
        hCorr->Draw("HIST ][ SAME");

        AddCMSHeader(c1, "Preliminary", false);

        TLatex hdr1;
        hdr1.SetNDC();
        hdr1.SetTextFont(42);
        hdr1.SetTextSize(0.036);
        hdr1.SetTextAlign(31);
        hdr1.DrawLatex(0.95, 0.935, "pPb (8.16 TeV)  pp (5.02 TeV)");

        TLatex latex;
        latex.SetNDC();
        latex.SetTextFont(42);
        latex.SetTextSize(0.036);
        latex.DrawLatex(0.19, 0.84, "pp energy extrapolation");
        latex.DrawLatex(0.19, 0.79, "pPb 8.16 TeV / pp 5.02 TeV");
        latex.DrawLatex(0.19, 0.74, "|y_{cm}| < 1.935");

        c1->SaveAs(Form("%s/ee_correction_factor.pdf", outputDir.c_str()));
        cout << "Saved: " << outputDir << "/ee_correction_factor.pdf" << endl;
        delete c1;
    }

    // -----------------------------------------------------------------------
    // Plot 2: Z pT spectra closure
    // -----------------------------------------------------------------------
    {
        TFile *fNoEE = TFile::Open(ppNoEEFile.c_str(), "READ");
        TFile *fEE   = TFile::Open(ppEEFile.c_str(), "READ");
        TFile *fPPb  = TFile::Open(ppbNosubFile.c_str(), "READ");
        TFile *fPbP  = TFile::Open(pbpNosubFile.c_str(), "READ");
        if (!fNoEE || fNoEE->IsZombie() || !fEE || fEE->IsZombie() ||
            !fPPb || fPPb->IsZombie() || !fPbP || fPbP->IsZombie()) {
            cerr << "Error: cannot open nosub files" << endl;
            return 1;
        }

        // pp @ 5.02 TeV (no EE weight)
        TH3D *h3NoEE = (TH3D *)fNoEE->Get("hZPtEtaPhi_0.5_15");
        TH1D *hPtNoEE = h3NoEE->ProjectionX("zpt_noEE");
        hPtNoEE->SetDirectory(nullptr);
        divideByWidth(hPtNoEE);

        // pp @ 8.16 TeV extrapolated (with EE weight)
        TH3D *h3EE = (TH3D *)fEE->Get("hZPtEtaPhi_0.5_15");
        TH1D *hPtEE = h3EE->ProjectionX("zpt_EE");
        hPtEE->SetDirectory(nullptr);
        divideByWidth(hPtEE);

        // pPb: combine pPb + PbP weighted by NZ
        TH3D *h3PPb = (TH3D *)fPPb->Get("hZPtEtaPhi_0.5_15");
        TH3D *h3PbP = (TH3D *)fPbP->Get("hZPtEtaPhi_0.5_15");
        TH1D *nzPPb = (TH1D *)fPPb->Get("hNZData_0.5_15");
        TH1D *nzPbP = (TH1D *)fPbP->Get("hNZData_0.5_15");
        double NZ_pPb = nzPPb->GetBinContent(1);
        double NZ_PbP = nzPbP->GetBinContent(1);

        TH1D *hPtPPb = h3PPb->ProjectionX("zpt_pPb_raw");
        TH1D *hPtPbP = h3PbP->ProjectionX("zpt_PbP_raw");
        hPtPPb->Scale(NZ_pPb);
        hPtPbP->Scale(NZ_PbP);
        hPtPPb->Add(hPtPbP);
        hPtPPb->Scale(1.0 / (NZ_pPb + NZ_PbP));
        hPtPPb->SetDirectory(nullptr);
        divideByWidth(hPtPPb);

        fNoEE->Close(); fEE->Close(); fPPb->Close(); fPbP->Close();

        TH1D *hRatioNoEE = (TH1D *)hPtNoEE->Clone("hRatioNoEE");
        hRatioNoEE->Divide(hPtPPb);
        TH1D *hRatioEE = (TH1D *)hPtEE->Clone("hRatioEE");
        hRatioEE->Divide(hPtPPb);

        double xlo = 0.5, xhi = 100;

        TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);

        TPad *pad1 = new TPad("pad1", "", 0, 0.3, 1, 1);
        pad1->SetLeftMargin(0.15);
        pad1->SetRightMargin(0.05);
        pad1->SetTopMargin(0.08);
        pad1->SetBottomMargin(0);
        pad1->SetLogx();
        pad1->SetLogy();
        pad1->Draw();

        TPad *pad2 = new TPad("pad2", "", 0, 0, 1, 0.3);
        pad2->SetLeftMargin(0.15);
        pad2->SetRightMargin(0.05);
        pad2->SetTopMargin(0);
        pad2->SetBottomMargin(0.30);
        pad2->SetLogx();
        pad2->Draw();

        // --- Top pad ---
        pad1->cd();

        double ylo = 0.2e-3, yhi = 0.5;

        TH1D *frame2 = new TH1D("frame2", "", 1, xlo, xhi);
        frame2->SetMinimum(ylo);
        frame2->SetMaximum(yhi);
        frame2->GetXaxis()->SetTitle("");
        frame2->GetYaxis()->SetTitle("1/N_{Z}  dN_{Z}/dp_{T}^{Z}");
        frame2->GetXaxis()->SetTitleSize(0);
        frame2->GetXaxis()->SetLabelSize(0);
        frame2->GetYaxis()->SetTitleSize(0.065);
        frame2->GetYaxis()->SetLabelSize(0.06);
        frame2->GetYaxis()->SetTitleOffset(1.0);
        frame2->GetXaxis()->SetMoreLogLabels();
        frame2->GetXaxis()->SetNoExponent();
        frame2->Draw();

        hPtNoEE->SetLineColor(cmsYellow);
        hPtNoEE->SetMarkerColor(cmsYellow);
        hPtNoEE->SetMarkerStyle(21);
        hPtNoEE->SetMarkerSize(1.0);
        hPtNoEE->SetLineWidth(2);
        hPtNoEE->GetXaxis()->SetRangeUser(xlo, xhi);
        hPtNoEE->Draw("PE SAME");

        hPtPPb->SetLineColor(cmsRed);
        hPtPPb->SetMarkerColor(cmsRed);
        hPtPPb->SetMarkerStyle(20);
        hPtPPb->SetMarkerSize(1.0);
        hPtPPb->SetLineWidth(2);
        hPtPPb->GetXaxis()->SetRangeUser(xlo, xhi);
        hPtPPb->Draw("PE SAME");

        hPtEE->SetLineColor(cmsBlue);
        hPtEE->SetMarkerColor(cmsBlue);
        hPtEE->SetMarkerStyle(22);
        hPtEE->SetMarkerSize(1.0);
        hPtEE->SetLineWidth(2);
        hPtEE->GetXaxis()->SetRangeUser(xlo, xhi);
        hPtEE->Draw("PE SAME");

        AddCMSHeader(pad1, "Preliminary", false);

        TLatex hdr2;
        hdr2.SetNDC();
        hdr2.SetTextFont(42);
        hdr2.SetTextSize(0.050);
        hdr2.SetTextAlign(31);
        hdr2.DrawLatex(0.95, 0.935, "pPb (8.16 TeV)  pp (5.02 TeV)");

        TLegend *leg = new TLegend(0.19, 0.05, 0.60, 0.28);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextFont(42);
        leg->SetTextSize(0.048);
        leg->AddEntry(hPtNoEE, "pp (5.02 TeV)", "PE");
        leg->AddEntry(hPtPPb, "pPb (8.16 TeV)", "PE");
        leg->AddEntry(hPtEE, "pp (8.16 TeV, extrap.)", "PE");
        leg->Draw();

        TLatex latex2;
        latex2.SetNDC();
        latex2.SetTextFont(42);
        latex2.SetTextSize(0.050);
        latex2.DrawLatex(0.19, 0.84, "|y_{cm}| < 1.935");

        // --- Bottom pad ---
        pad2->cd();

        TH1D *frameR = new TH1D("frameR", "", 1, xlo, xhi);
        frameR->SetMinimum(0.3);
        frameR->SetMaximum(1.7);
        frameR->GetXaxis()->SetTitle("p_{T}^{Z} (GeV)");
        frameR->GetYaxis()->SetTitle("pp / pPb");
        frameR->GetXaxis()->SetTitleSize(0.12);
        frameR->GetYaxis()->SetTitleSize(0.12);
        frameR->GetXaxis()->SetLabelSize(0.11);
        frameR->GetYaxis()->SetLabelSize(0.11);
        frameR->GetYaxis()->SetTitleOffset(0.45);
        frameR->GetXaxis()->SetTitleOffset(1.0);
        frameR->GetYaxis()->SetNdivisions(505);
        frameR->GetXaxis()->SetMoreLogLabels();
        frameR->GetXaxis()->SetNoExponent();
        frameR->Draw();

        TLine *lineR = new TLine(xlo, 1.0, xhi, 1.0);
        lineR->SetLineStyle(2);
        lineR->SetLineColor(kGray + 2);
        lineR->Draw("SAME");

        hRatioNoEE->SetLineColor(cmsYellow);
        hRatioNoEE->SetMarkerColor(cmsYellow);
        hRatioNoEE->SetMarkerStyle(21);
        hRatioNoEE->SetMarkerSize(1.0);
        hRatioNoEE->SetLineWidth(2);
        hRatioNoEE->GetXaxis()->SetRangeUser(xlo, xhi);
        hRatioNoEE->Draw("PE SAME");

        hRatioEE->SetLineColor(cmsBlue);
        hRatioEE->SetMarkerColor(cmsBlue);
        hRatioEE->SetMarkerStyle(22);
        hRatioEE->SetMarkerSize(1.0);
        hRatioEE->SetLineWidth(2);
        hRatioEE->GetXaxis()->SetRangeUser(xlo, xhi);
        hRatioEE->Draw("PE SAME");

        c2->SaveAs(Form("%s/ee_zpt_closure.pdf", outputDir.c_str()));
        cout << "Saved: " << outputDir << "/ee_zpt_closure.pdf" << endl;
        delete c2;
    }

    return 0;
}
