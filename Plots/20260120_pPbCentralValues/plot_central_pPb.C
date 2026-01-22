#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include <vector>
#include <string>

void plot_central_pPb() {

    // The first input file is considered the baseline (Gen+EPOS)
    // currently using a certain ZPT and track PT range

    //pPb
    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_residual_V17_nmix5_ZPT0_10-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_V16_nmix5_ZPT0_10-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_V16_nmix5_ZPT0_10-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_residual_V16_nmix5_ZPT0_10-result.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_residual_V16_nmix5_ZPT0_10-result.root"
    };
    vector<string> labels = {
        "pp DATA",
        "MC Gen + EPOS",
        "MC Reco",
        "MC Reco (corrected)",
        "pPb DATA (corrected)"
    };
    const char* pT_range = "1_2";
    const char* output =  "plots/pp-pPb_ZPT0_10-1_2_nmix5-data";

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;
    vector<TH1*> hTrkPhi;
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaPhi;

    // Loop over all input files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {
        TFile* fin = TFile::Open(input_ZPT.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // track pt eta phi
        TH3D* this_hTrkPtEtaPhi = (TH3D*)fin->Get(Form("hTrkPtEtaPhiData_%s", pT_range));
        TH1D* this_hTrkPt = this_hTrkPtEtaPhi->ProjectionX(Form("trkPt_%d", i));
        TH1D* this_hTrkEta = this_hTrkPtEtaPhi->ProjectionY(Form("trkEta_%d", i));
        TH1D* this_hTrkPhi = this_hTrkPtEtaPhi->ProjectionZ(Form("trkPhi_%d", i));

        // delta phi, delta eta
        TH1D* this_hDeltaEta = (TH1D*)fin->Get(Form("DeltaEta_Result%s", pT_range)  );
        this_hDeltaEta->SetName(Form("DeltaEta_%d", i));
        TH1D* this_hDeltaPhi = (TH1D*)fin->Get(Form("DeltaPhi_Result%s", pT_range));
        this_hDeltaPhi->SetName(Form("DeltaPhi_%d", i));

        this_hDeltaEta->Scale(0.5); // scale by 0.5 since we double count during filling
        this_hDeltaPhi->Scale(0.5);

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hTrkPhi);

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);
        hTrkPhi.push_back(this_hTrkPhi);

        hDeltaEta.push_back(this_hDeltaEta);
        hDeltaPhi.push_back(this_hDeltaPhi);

        i++;
    }

    vector<int> markerColors = {cmsBlue, cmsRed, cmsYellow, kSpring+7, cmsTealL1, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, cmsYellow, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 1, 1, 2, 0, 1};

    // make canvas
    TCanvas* cTrk1 = new TCanvas("cTrk", "cTrk", 600, 600);

    TPad* pTrk1 = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "p_{T}^{ch}", 0, 10,
        "(1/N_{Z}) dN_{ch}/dp_{T}^{ch}", -27, 160,
        "Ratio to Gen+EPOS", 0.8, 1.2,
        0,
        true, false, false
    );

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, "8 TeV (5 TeV)", "pPb (pp)");

    cTrk1->Update();
    cTrk1->SaveAs(Form("%s-pt.pdf", output));

    TCanvas* cTrk2 = new TCanvas("cTrk2", "cTrk2", 600, 600);
    TPad* pTrk2 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "y_{ch}", -4, 4,
        "(1/N_{Z}) dN_{ch}/d y_{ch}", 8, 22,
        "Ratio to Gen+EPOS", 0.8, 1.2,
        0,
        false, false, false
    );

    cTrk2->Update();
    cTrk2->SaveAs(Form("%s-eta.pdf", output));

    TCanvas* cTrk3 = new TCanvas("cTrk3", "cTrk3", 600, 600);
    TPad* pTrk3 = (TPad*) plotCMSRatio(
        hTrkPhi, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#phi_{ch}", -3.2, 3.2,
        "(1/N_{Z}) dN_{ch}/d #phi_{ch}", 6, 18,
        "Ratio to Gen+EPOS", 0.8, 1.2,
        0,
        false, false, false
    );

    cTrk3->Update();
    cTrk3->SaveAs(Form("%s-phi.pdf", output));


    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* p1 = (TPad*) plotCMSDiff(
        hDeltaEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1.5, 1.5,
        "PbPb - pp", -2.5, 2.5,
        0,
        false, false, false
    );

    AddCMSHeader(
        p1,
        "Internal",
        false
    );

    AddUPCHeader(p1, "8 TeV (5 TeV)", "pPb (pp)");
    p1->Update();

    c1->SaveAs(Form("%s-DeltaEta.pdf", output));



    // make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
    
    TPad* p2 = (TPad*) plotCMSDiff(
        hDeltaPhi, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#phi_{ch,Z}", -1.5758, 4.7275,
        "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -2.5, 12,
        "PbPb - pp", -1.5, 1.5,
        0,
        false, false, false
    );

    AddCMSHeader(
        p2,
        "Internal",
        false
    );

    AddUPCHeader(p2, "8 TeV (5 TeV)", "pPb (pp)");
    p2->Update();

    c2->SaveAs(Form("%s-DeltaPhi.pdf", output));


    // save histograms
    TFile* fout = TFile::Open(Form("%s.root", output), "RECREATE");
    fout->cd();
    for (size_t i = 0; i < hTrkPt.size(); i++) {
        hTrkPt[i]->Write();
        hTrkEta[i]->Write();
        hTrkPhi[i]->Write();
        hDeltaEta[i]->Write();
        hDeltaPhi[i]->Write();
    }
    fout->Close();

}
