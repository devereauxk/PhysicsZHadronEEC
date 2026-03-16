#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void reweight_VZ() {

    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_noVZWeight_nmix0_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_noVZWeight_nmix0_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_noVZWeight_nmix0_ZPT0_350-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_w0_nominal_noVZWeight_nmix0_ZPT0_350-result.root"
    };
    vector<string> labels = {
        "Reco PPb",
        "Data PPb",
        "Reco PbP",
        "Data PbP"
    };
    const char* output =  "summary/20260311_ZPT0_350";

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;
    vector<TH1*> hMult;
    vector<TH1*> hZPt;
    vector<TH1*> hZEta;
    vector<TH1*> hVZ;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH2D* this_hTrkPtEta = (TH2D*)fin->Get("hTrkPtEtaData_1_10");
        TH3D* this_hZPtEtaMult = (TH3D*)fin->Get("hZPtEtaMult_1_10");

        TH1D* this_hTrkPt = this_hTrkPtEta->ProjectionX(Form("trkPt_%s", labels[i].c_str()));
        TH1D* this_hTrkEta = this_hTrkPtEta->ProjectionY(Form("trkEta_%s", labels[i].c_str()));
        TH1D* this_hMult = this_hZPtEtaMult->ProjectionZ(Form("mult_%s", labels[i].c_str()));
        TH1D* this_hZPt = this_hZPtEtaMult->ProjectionX(Form("ZPt_%s", labels[i].c_str()));
        TH1D* this_hZEta = this_hZPtEtaMult->ProjectionY(Form("ZEta_%s", labels[i].c_str()));

        TH1D* this_hVZ = (TH1D*)fin->Get("hVZ_1_10");

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hZEta);
        divideByWidth(this_hZPt);
        this_hVZ->Scale(1.0 / this_hVZ->Integral());

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);
        hMult.push_back(this_hMult);
        hZPt.push_back(this_hZPt);
        hZEta.push_back(this_hZEta);
        hVZ.push_back(this_hVZ);

        i++;
    }

    // fit quartic function to Data / MC VZ Ratio
    TH1D* hVZRatio_ppb = (TH1D*)hVZ[1]->Clone("hVZRatio_ppb");
    hVZRatio_ppb->Divide(hVZ[0]);
    TF1* fitFunc_ppb = new TF1("VZ_reweight_ppb", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x", -20, 20);
    fitFunc_ppb->SetParameters(1, 0, 0, 0, 0);
    hVZRatio_ppb->Fit(fitFunc_ppb, "R");

    TH1D* hVZRatio_pbp = (TH1D*)hVZ[3]->Clone("hVZRatio_pbp");
    hVZRatio_pbp->Divide(hVZ[2]);
    TF1* fitFunc_pbp = new TF1("VZ_reweight_pbp", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x", -20, 20);
    fitFunc_pbp->SetParameters(1, 0, 0, 0, 0);
    hVZRatio_pbp->Fit(fitFunc_pbp, "R");

    // save fits to separate root files with identical object name: VZ_reweight
    {
        TFile* fout_ppb = TFile::Open("20251003_VzReweightFits_PPb.root", "RECREATE");
        TF1* out_ppb = (TF1*)fitFunc_ppb->Clone("VZ_reweight");
        out_ppb->Write();
        fout_ppb->Close();
    }
    {
        TFile* fout_pbp = TFile::Open("20251003_VzReweightFits_PbP.root", "RECREATE");
        TF1* out_pbp = (TF1*)fitFunc_pbp->Clone("VZ_reweight");
        out_pbp->Write();
        fout_pbp->Close();
    }

    // reweight the MC
    TH1D* hVZReweighted_ppb = (TH1D*)hVZ[0]->Clone("hVZReweighted_ppb");
    for (int bin = 1; bin <= hVZReweighted_ppb->GetNbinsX(); ++bin) {
        double vz = hVZReweighted_ppb->GetBinCenter(bin);
        double weight = fitFunc_ppb->Eval(vz);
        double originalContent = hVZReweighted_ppb->GetBinContent(bin);
        hVZReweighted_ppb->SetBinContent(bin, originalContent * weight);
    }

    TH1D* hVZReweighted_pbp = (TH1D*)hVZ[2]->Clone("hVZReweighted_pbp");
    for (int bin = 1; bin <= hVZReweighted_pbp->GetNbinsX(); ++bin) {
        double vz = hVZReweighted_pbp->GetBinCenter(bin);
        double weight = fitFunc_pbp->Eval(vz);
        double originalContent = hVZReweighted_pbp->GetBinContent(bin);
        hVZReweighted_pbp->SetBinContent(bin, originalContent * weight);
    }

    // ================================================== //
    // ppb plots
    // ================================================== //
    // make canvas
    TCanvas* c6 = new TCanvas("c6", "c6", 600, 600);
    TPad* p6 = (TPad*) plotCMSRatio(
        {hVZ[1], hVZ[0], hVZReweighted_ppb}, "", {labels[1], labels[0], "Reco PPb (reweighted)"},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsGray}, {1, 2, 1, 2, 1, 0},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "V_{Z}", -20, 20,
        "counts [unit normalization]", -1, -1,
        "MC / Data", 0, 2,
        0,
        false, false, false
    );

    AddCMSHeader(
        p6,
        "Internal",
        false
    );

    AddUPCHeader(p6, "8.16 TeV", "pPb");
    p6->Update();
    c6->SaveAs(Form("%s-Vz_ppb.pdf", output));


    // make canvas
    TCanvas* c7 = new TCanvas("c7", "c7", 650, 600);
    TPad* p7 = (TPad*) plotCMSSimple(
        c7, {hVZRatio_ppb}, "", {"Data / MC VZ Ratio"},
        {cmsRed}, {0},
        {cmsRed}, {mCircleFill},
        "V_{Z}", -20, 20,
        "Data / MC V_{Z} Ratio", 0, 2,
        false, false, false, 1
    );

    AddCMSHeader(
        p7,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    fitFunc_ppb->SetLineColor(kBlue+2);
    fitFunc_ppb->Draw("same");

    AddUPCHeader(p7, "5.02 TeV", "pPb");
    p7->Update();
    c7->SaveAs(Form("%s-VzRatio_ppb.pdf", output));


    // ================================================== //
    // pbp plots
    // ================================================== //
    // make canvas
    TCanvas* c8 = new TCanvas("c8", "c8", 600, 600);
    TPad* p8 = (TPad*) plotCMSRatio(
        {hVZ[3], hVZ[2], hVZReweighted_pbp}, "", {labels[3], labels[2], "Reco PbP (reweighted)"},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsGray}, {1, 2, 1, 2, 1, 0},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "V_{Z}", -20, 20,
        "counts [unit normalization]", -1, -1,
        "MC / Data", 0, 2,
        0,
        false, false, false
    );

    AddCMSHeader(
        p8,
        "Internal",
        false
    );

    AddUPCHeader(p8, "8.16 TeV", "PbP");
    p8->Update();
    c8->SaveAs(Form("%s-Vz_pbp.pdf", output));


    // make canvas
    TCanvas* c9 = new TCanvas("c9", "c9", 650, 600);
    TPad* p9 = (TPad*) plotCMSSimple(
        c9, {hVZRatio_pbp}, "", {"Data / MC VZ Ratio"},
        {cmsRed}, {0},
        {cmsRed}, {mCircleFill},
        "V_{Z}", -20, 20,
        "Data / MC V_{Z} Ratio", 0, 2,
        false, false, false, 1
    );

    AddCMSHeader(
        p9,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    fitFunc_pbp->SetLineColor(kBlue+2);
    fitFunc_pbp->Draw("same");

    AddUPCHeader(p9, "5.02 TeV", "PbP");
    p9->Update();
    c9->SaveAs(Form("%s-VzRatio_pbp.pdf", output));


    // ================================================== //
    // ppb pbp comparison
    // ================================================== //
    // make canvas
    TCanvas* c10 = new TCanvas("c10", "c10", 600, 600);
    TPad* p10 = (TPad*) plotCMSRatio(
        {hVZ[1], hVZ[3]}, "", {labels[1], labels[3]},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsGray}, {1, 1, 1, 2, 1, 0},
        {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "V_{Z}", -20, 20,
        "counts [unit normalization]", -1, -1,
        "MC / Data", 0.8, 1.2,
        0,
        false, false, false
    );

    AddCMSHeader(
        p10,
        "Internal",
        false
    );

    AddUPCHeader(p10, "8 TeV", "PbP");
    p10->Update();
    c10->SaveAs(Form("%s-Vz_comparison.pdf", output));

    // make canvas
    TCanvas* c11 = new TCanvas("c11", "c11", 650, 600);
    TPad* p11 = (TPad*) plotCMSSimple(
        c11, {hZEta[0], hZEta[2]}, "", {"Reco PPb", "Reco PbP"},
        {cmsBlue, cmsRed}, {0, 0},
        {cmsBlue, cmsRed}, {mCircleFill, mCircleFill},
        "#eta^{Z}", -2.4, 2.4,
        "dN/d#eta", -1, -1,
        false, false, false
    );

    AddCMSHeader(
        p11,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    AddUPCHeader(p11, "5.02 TeV", "pPb & PbP");
    p11->Update();
    c11->SaveAs(Form("%s-ZEta_comparison.pdf", output));

    // make canvas
    TCanvas* c12 = new TCanvas("c12", "c12", 650, 600);
    TPad* p12 = (TPad*) plotCMSSimple(
        c12, {hTrkEta[0], hTrkEta[2]}, "", {"Reco PPb", "Reco PbP"},
        {cmsBlue, cmsRed}, {0, 0},
        {cmsBlue, cmsRed}, {mCircleFill, mCircleFill},
        "#eta^{track}", -2.4, 2.4,
        "dN/d#eta", -1, -1,
        false, false, false
    );

    AddCMSHeader(
        p12,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );
    AddUPCHeader(p12, "5.02 TeV", "pPb & PbP");
    p12->Update();
    c12->SaveAs(Form("%s-trkEta_comparison.pdf", output));

    // make canvas
    TCanvas* c13 = new TCanvas("c13", "c13", 650, 600);
    TPad* p13 = (TPad*) plotCMSSimple(
        c13, {hZEta[1], hZEta[3]}, "", {"Data PPb", "Data PbP"},
        {cmsBlue, cmsRed}, {0, 0},
        {cmsBlue, cmsRed}, {mCircleFill, mCircleFill},
        "#eta^{Z}", -2.4, 2.4,
        "dN/d#eta", -1, -1,
        false, false, false
    );

    AddCMSHeader(
        p13,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    AddUPCHeader(p13, "5.02 TeV", "pPb & PbP");
    p13->Update();
    c13->SaveAs(Form("%s-ZEtaData_comparison.pdf", output));

}