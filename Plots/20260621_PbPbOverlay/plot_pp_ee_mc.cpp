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
    string outputDir    = CL.Get("OutputDir", "plots/pp-ee-mc");

    SetTDRStyle();

    // -----------------------------------------------------------------------
    // Plot 1: MC-driven EE correction factor vs Z pT
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

        TLatex latex;
        latex.SetNDC();
        latex.SetTextFont(42);
        latex.SetTextSize(0.036);
        latex.DrawLatex(0.19, 0.84, "Pythia8 + MadGraph");
        latex.DrawLatex(0.19, 0.79, "|y_{cm}| < 1.935");
        latex.DrawLatex(0.19, 0.74, "pp energy extrapolation");
        latex.DrawLatex(0.19, 0.69, "pp 8.16 TeV / pp 5.02 TeV");

        c1->SaveAs(Form("%s/ee_mc_correction_factor.pdf", outputDir.c_str()));
        cout << "Saved: " << outputDir << "/ee_mc_correction_factor.pdf" << endl;
        delete c1;
    }

    // -----------------------------------------------------------------------
    // Plot 2: Z pT spectra closure (MC-driven EE)
    // -----------------------------------------------------------------------
    {
        TFile *fNoEE = TFile::Open(ppNoEEFile.c_str(), "READ");
        TFile *fEE   = TFile::Open(ppEEFile.c_str(), "READ");
        if (!fNoEE || fNoEE->IsZombie() || !fEE || fEE->IsZombie()) {
            cerr << "Error: cannot open nosub files" << endl;
            return 1;
        }

        TH3D *h3NoEE = (TH3D *)fNoEE->Get("hZPtEtaPhi_0.5_15");
        TH1D *hPtNoEE = h3NoEE->ProjectionX("zpt_noEE");
        hPtNoEE->SetDirectory(nullptr);
        divideByWidth(hPtNoEE);

        TH3D *h3EE = (TH3D *)fEE->Get("hZPtEtaPhi_0.5_15");
        TH1D *hPtEE = h3EE->ProjectionX("zpt_EE");
        hPtEE->SetDirectory(nullptr);
        divideByWidth(hPtEE);

        fNoEE->Close(); fEE->Close();

        TH1D *hRatio = (TH1D *)hPtEE->Clone("hRatio");
        hRatio->Divide(hPtNoEE);

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
        pad2->SetTopMargin(0.02);
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

        hPtEE->SetLineColor(cmsBlue);
        hPtEE->SetMarkerColor(cmsBlue);
        hPtEE->SetMarkerStyle(22);
        hPtEE->SetMarkerSize(1.0);
        hPtEE->SetLineWidth(2);
        hPtEE->GetXaxis()->SetRangeUser(xlo, xhi);
        hPtEE->Draw("PE SAME");

        TLegend *leg = new TLegend(0.19, 0.05, 0.60, 0.22);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextFont(42);
        leg->SetTextSize(0.048);
        leg->AddEntry(hPtNoEE, "pp (5.02 TeV)", "PE");
        leg->AddEntry(hPtEE, "pp (8.16 TeV, MC extrap.)", "PE");
        leg->Draw();

        TLatex latex2;
        latex2.SetNDC();
        latex2.SetTextFont(42);
        latex2.SetTextSize(0.050);
        latex2.DrawLatex(0.19, 0.84, "Pythia8 + MadGraph");
        latex2.DrawLatex(0.19, 0.78, "|y_{cm}| < 1.935");

        // --- Bottom pad ---
        pad2->cd();

        TH1D *frameR = new TH1D("frameR", "", 1, xlo, xhi);
        frameR->SetMinimum(0.3);
        frameR->SetMaximum(1.7);
        frameR->GetXaxis()->SetTitle("p_{T}^{Z} (GeV)");
        frameR->GetYaxis()->SetTitle("8 TeV / 5 TeV");
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

        hRatio->SetLineColor(cmsBlue);
        hRatio->SetMarkerColor(cmsBlue);
        hRatio->SetMarkerStyle(22);
        hRatio->SetMarkerSize(1.0);
        hRatio->SetLineWidth(2);
        hRatio->GetXaxis()->SetRangeUser(xlo, xhi);
        hRatio->Draw("PE SAME");

        c2->SaveAs(Form("%s/ee_mc_zpt_closure.pdf", outputDir.c_str()));
        cout << "Saved: " << outputDir << "/ee_mc_zpt_closure.pdf" << endl;
        delete c2;
    }

    // -----------------------------------------------------------------------
    // Plot 3: Z y spectra closure (MC-driven EE)
    // -----------------------------------------------------------------------
    {
        TFile *fNoEE = TFile::Open(ppNoEEFile.c_str(), "READ");
        TFile *fEE   = TFile::Open(ppEEFile.c_str(), "READ");
        if (!fNoEE || fNoEE->IsZombie() || !fEE || fEE->IsZombie()) {
            cerr << "Error: cannot open nosub files for Z y" << endl;
            return 1;
        }

        TH3D *h3NoEE = (TH3D *)fNoEE->Get("hZPtEtaPhi_0.5_15");
        TH1D *hYNoEE = h3NoEE->ProjectionY("zy_noEE");
        hYNoEE->SetDirectory(nullptr);
        divideByWidth(hYNoEE);

        TH3D *h3EE = (TH3D *)fEE->Get("hZPtEtaPhi_0.5_15");
        TH1D *hYEE = h3EE->ProjectionY("zy_EE");
        hYEE->SetDirectory(nullptr);
        divideByWidth(hYEE);

        fNoEE->Close(); fEE->Close();

        TH1D *hYRatio = (TH1D *)hYEE->Clone("hYRatio");
        hYRatio->Divide(hYNoEE);

        double yxlo = -2.4, yxhi = 2.4;

        TCanvas *c3 = new TCanvas("c3", "c3", 600, 600);

        TPad *pad3t = new TPad("pad3t", "", 0, 0.3, 1, 1);
        pad3t->SetLeftMargin(0.17);
        pad3t->SetRightMargin(0.05);
        pad3t->SetTopMargin(0.08);
        pad3t->SetBottomMargin(0);
        pad3t->Draw();

        TPad *pad3b = new TPad("pad3b", "", 0, 0, 1, 0.3);
        pad3b->SetLeftMargin(0.17);
        pad3b->SetRightMargin(0.05);
        pad3b->SetTopMargin(0.035);
        pad3b->SetBottomMargin(0.30);
        pad3b->Draw();

        // --- Top pad ---
        pad3t->cd();

        double ymain_hi = -1;
        for (int i = 1; i <= hYNoEE->GetNbinsX(); ++i) {
            double v = hYNoEE->GetBinContent(i);
            if (v > ymain_hi) ymain_hi = v;
        }
        for (int i = 1; i <= hYEE->GetNbinsX(); ++i) {
            double v = hYEE->GetBinContent(i);
            if (v > ymain_hi) ymain_hi = v;
        }
        ymain_hi *= 1.5;

        TH1D *frame3 = new TH1D("frame3", "", 1, yxlo, yxhi);
        frame3->SetMinimum(0.025);
        frame3->SetMaximum(ymain_hi);
        frame3->GetXaxis()->SetTitle("");
        frame3->GetYaxis()->SetTitle("1/N_{Z}  dN_{Z}/dy_{Z}");
        frame3->GetXaxis()->SetTitleSize(0);
        frame3->GetXaxis()->SetLabelSize(0);
        frame3->GetYaxis()->SetTitleSize(0.065);
        frame3->GetYaxis()->SetLabelSize(0.06);
        frame3->GetYaxis()->SetTitleOffset(1.15);
        frame3->Draw();

        hYNoEE->SetLineColor(cmsYellow);
        hYNoEE->SetMarkerColor(cmsYellow);
        hYNoEE->SetMarkerStyle(21);
        hYNoEE->SetMarkerSize(1.0);
        hYNoEE->SetLineWidth(2);
        hYNoEE->Draw("PE SAME");

        hYEE->SetLineColor(cmsBlue);
        hYEE->SetMarkerColor(cmsBlue);
        hYEE->SetMarkerStyle(22);
        hYEE->SetMarkerSize(1.0);
        hYEE->SetLineWidth(2);
        hYEE->Draw("PE SAME");

        TLatex latex3;
        latex3.SetNDC();
        latex3.SetTextFont(42);
        latex3.SetTextSize(0.050);
        latex3.DrawLatex(0.22, 0.81, "Pythia8 + MadGraph");
        latex3.DrawLatex(0.22, 0.75, "|y_{cm}| < 1.935");

        TLegend *leg3 = new TLegend(0.30, 0.05, 0.71, 0.22);
        leg3->SetBorderSize(0);
        leg3->SetFillStyle(0);
        leg3->SetTextFont(42);
        leg3->SetTextSize(0.048);
        leg3->AddEntry(hYNoEE, "pp (5.02 TeV)", "PE");
        leg3->AddEntry(hYEE, "pp (8.16 TeV, MC extrap.)", "PE");
        leg3->Draw();

        // --- Bottom pad ---
        pad3b->cd();

        TH1D *frameR3 = new TH1D("frameR3", "", 1, yxlo, yxhi);
        frameR3->SetMinimum(0.9);
        frameR3->SetMaximum(1.1);
        frameR3->GetXaxis()->SetTitle("y_{Z}");
        frameR3->GetYaxis()->SetTitle("8 TeV / 5 TeV");
        frameR3->GetXaxis()->SetTitleSize(0.12);
        frameR3->GetYaxis()->SetTitleSize(0.12);
        frameR3->GetXaxis()->SetLabelSize(0.11);
        frameR3->GetYaxis()->SetLabelSize(0.11);
        frameR3->GetYaxis()->SetTitleOffset(0.45);
        frameR3->GetXaxis()->SetTitleOffset(1.0);
        frameR3->GetYaxis()->SetNdivisions(505);
        frameR3->Draw();

        TLine *lineR3 = new TLine(yxlo, 1.0, yxhi, 1.0);
        lineR3->SetLineStyle(2);
        lineR3->SetLineColor(kGray + 2);
        lineR3->Draw("SAME");

        hYRatio->SetLineColor(cmsBlue);
        hYRatio->SetMarkerColor(cmsBlue);
        hYRatio->SetMarkerStyle(22);
        hYRatio->SetMarkerSize(1.0);
        hYRatio->SetLineWidth(2);
        hYRatio->Draw("PE SAME");

        c3->SaveAs(Form("%s/ee_mc_zy_closure.pdf", outputDir.c_str()));
        cout << "Saved: " << outputDir << "/ee_mc_zy_closure.pdf" << endl;
        delete c3;
    }

    return 0;
}
