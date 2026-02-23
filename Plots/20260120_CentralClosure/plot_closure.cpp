#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>


void cutY(TH2D* h, int ybinmin, int ybinmax) {
    for (int ix = 1; ix <= h->GetNbinsX(); ix++) {
        for (int iy = 1; iy <= h->GetNbinsY(); iy++) {
            if (iy > ybinmax || iy < ybinmin) h->SetBinContent(ix, iy, 0);
        }
    }
}

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("tag", "V16_nmix5");

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;

    string mctag = (collisionType == "pp" ? "pythia" : collisionType);

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_nominal_%s_ZPT%s", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_ZResidual_%s_ZPT%s", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_trkResidual_%s_ZPT%s", mctag.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "MC DY-GEN",
        "MC DY-RECO",
        "  & Z correction",
        "  & Z + track correction"
    };
    string output = Form("plots/%s/%s_ZPT%s_trkPT%s_%s-closure", collisionType.c_str(), collisionType.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag.c_str());

    vector<TH1*> hDeltaEta_all;
    vector<TH1*> hDeltaPhi_all;

    vector<TH2*> hMixData;
    vector<TH2*> hData;
    vector<TH1*> hDeltaEta_mix;
    vector<TH1*> hDeltaPhi_mix;

    // Loop over nosub files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        cout<<"opening file: "<<Form("%s-nosub.root", input_ZPT.c_str())<<endl;

        TFile* fin = TFile::Open(Form("%s-nosub.root", input_ZPT.c_str()), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // delta phi, delta eta
        TH1D* this_hDeltaEta_all = (TH1D*)fin->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
        this_hDeltaEta_all->SetName(Form("DeltaEta_all_%d", i));
        TH1D* this_hDeltaPhi_all = (TH1D*)fin->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
        this_hDeltaPhi_all->SetName(Form("DeltaPhi_all_%d", i));

        hData.push_back((TH2D*)fin->Get(Form("hData_%s", trkPtRange.c_str())));
        hDeltaEta_all.push_back(this_hDeltaEta_all); 
        hDeltaPhi_all.push_back(this_hDeltaPhi_all);

        this_hDeltaEta_all->Print("all");
        this_hDeltaPhi_all->Print("all");

        // mixed
        TH2D* this_hMixData2D = (TH2D*)fin->Get(Form("hMixData_%s", trkPtRange.c_str()));
        TH1D* this_hDeltaPhi_mix = this_hMixData2D->ProjectionY(Form("hMixPhi_%d", i), 0, 10);
        TH1D* this_hDeltaEta_mix = this_hMixData2D->ProjectionX(Form("hMixEta_%d", i), 6, 10);

        divideByWidth(this_hDeltaPhi_mix);
        divideByWidth(this_hDeltaEta_mix);

        hMixData.push_back(this_hMixData2D);
        hDeltaEta_mix.push_back(this_hDeltaEta_mix);
        hDeltaPhi_mix.push_back(this_hDeltaPhi_mix);

        i++;
    }

    // read results file
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaPhi;
    vector<TH2*> hData_result;

    for (const auto& input_ZPT : input_ZPT_files) {

        TFile* fin = TFile::Open(Form("%s-result.root", input_ZPT.c_str()), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TH1D* this_hDeltaEta = (TH1D*)fin->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
        this_hDeltaEta->SetName(Form("DeltaEta_%d", i));
        TH1D* this_hDeltaPhi = (TH1D*)fin->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
        this_hDeltaPhi->SetName(Form("DeltaPhi_%d", i));

        TH2D* this_hData2D = (TH2D*)fin->Get(Form("hData_%s", trkPtRange.c_str()));

        hDeltaEta.push_back(this_hDeltaEta);
        hDeltaPhi.push_back(this_hDeltaPhi);
        hData_result.push_back(this_hData2D);

        i++;
        
    }

    // 2D eta-phi maps (corrected reco) / (gen)
    TH2D* hDelta2D_ratio_all = (TH2D*)hData[3]->Clone("hDelta2D_ratio_all");
    hDelta2D_ratio_all->Divide(hData[0]);

    TH2D* hDelta2D_ratio_mix = (TH2D*)hMixData[3]->Clone("hDelta2D_ratio_mix");
    hDelta2D_ratio_mix->Divide(hMixData[0]);
    
    TH2D* hDelta2D_ratio = (TH2D*)hData_result[3]->Clone("hDelta2D_ratio_result");
    hDelta2D_ratio->Divide(hData_result[0]);
    //hDelta2D_ratio->Print("all");




    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 1, 0, 1};

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* p1 = (TPad*) plotCMSRatio(
        hDeltaEta_all, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "Signal d#DeltaN_{ch}/d#Delta y_{ch,Z}", -1, -1,
        "Ratio to GEN", 0.92, 1.08,
        0,
        false, false, true
    );

    AddCMSHeader(
        p1,
        "Internal",
        false
    );

    AddUPCHeader(p1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    p1->Update();

    c1->SaveAs(Form("%s-DeltaEta-all.pdf", output.c_str()));


    // make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
    
    TPad* p2 = (TPad*) plotCMSRatio(
        hDeltaPhi_all, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Signal d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", (collisionType == "pp") ? 0 : 15, (collisionType == "pp") ? 35 : 50,
        "Ratio to GEN", 0.92, 1.08,
        0,
        false, false, true
    );

    AddCMSHeader(
        p2,
        "Internal",
        false
    );

    AddUPCHeader(p1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    p2->Update();

    c2->SaveAs(Form("%s-DeltaPhi-all.pdf", output.c_str()));


    TCanvas* cMix1 = new TCanvas("cMix1", "cMix1", 600, 600);
    TPad* pMix1 = (TPad*) plotCMSRatio(
        hDeltaEta_mix, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "Mixed d#DeltaN_{ch}/d#Delta y_{ch,Z}", -1, -1,
        "Ratio to GEN", 0.92, 1.08,
        0,
        false, false, true
    );

    AddCMSHeader(
        pMix1,
        "Internal",
        false
    );
    AddUPCHeader(pMix1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    cMix1->Update();
    cMix1->SaveAs(Form("%s-DeltaEta-bkg.pdf", output.c_str()));

    TCanvas* cMix2 = new TCanvas("cMix2", "cMix2", 600, 600);
    TPad* pMix2 = (TPad*) plotCMSRatio(
        hDeltaPhi_mix, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Mixed d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", (collisionType == "pp") ? 0 : 15, (collisionType == "pp") ? 35 : 50,
        "Ratio to GEN", 0.92, 1.08,
        0,
        false, false, true
    );

    AddCMSHeader(
        pMix2,
        "Internal",
        false
    );
    AddUPCHeader(pMix2, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    cMix2->Update();
    cMix2->SaveAs(Form("%s-DeltaPhi-bkg.pdf", output.c_str()));


    TCanvas* cResult1 = new TCanvas("cResult1", "cResult1", 600, 600);
    TPad* pResult1 = (TPad*) plotCMSRatio(
        hDeltaEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, true
    );

    AddCMSHeader(
        pResult1,
        "Internal",
        false
    );
    AddUPCHeader(pResult1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    cResult1->Update();
    cResult1->SaveAs(Form("%s-DeltaEta-result.pdf", output.c_str()));

    TCanvas* cResult2 = new TCanvas("cResult2", "cResult2", 600, 600);
    TPad* pResult2 = (TPad*) plotCMSRatio(
        hDeltaPhi, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, -1,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, true
    );

    AddCMSHeader(
        pResult2,
        "Internal",
        false
    );
    AddUPCHeader(pResult2, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    cResult2->Update();
    cResult2->SaveAs(Form("%s-DeltaPhi-result.pdf", output.c_str()));

    return 0;

    // =================================== //    
    // eta phi maps
    // =================================== //
    TCanvas* c2D = new TCanvas("c2D", "c2D", 600, 600);
    TPad* p2D = (TPad*) plotCMSSimple2D(
        c2D, hDelta2D_ratio, "Eta-Phi Map GEN vs RECO",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "RECO / GEN", 0.5, 1.5,
        false, false, false
    );

    c2D->SaveAs(Form("%s-Delta2D-ratio-result.pdf", output.c_str()));

    TCanvas* c2D_all = new TCanvas("c2D_all", "c2D_all", 600, 600);
    TPad* p2D_all = (TPad*) plotCMSSimple2D(
        c2D_all, hDelta2D_ratio_all, "Eta-Phi Map GEN vs RECO (all)",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "RECO / GEN", 0.97, 1.03,
        false, false, false
    );

    c2D_all->SaveAs(Form("%s-Delta2D-ratio-all.pdf", output.c_str()));

    TCanvas* c2D_mix = new TCanvas("c2D_mix", "c2D_mix", 600, 600);
    TPad* p2D_mix = (TPad*) plotCMSSimple2D(
        c2D_mix, hDelta2D_ratio_mix, "Eta-Phi Map GEN vs RECO (mixed)",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "RECO / GEN", 0.97, 1.03,
        false, false, false
    );

    c2D_mix->SaveAs(Form("%s-Delta2D-ratio-mix.pdf", output.c_str()));

    TCanvas* c2D_gen = new TCanvas("c2D_gen", "c2D_gen", 600, 600); 
    TPad* p2D_gen = (TPad*) plotCMSSimple2D(
        c2D_gen, hData_result[0], "Eta-Phi Map GEN",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "GEN", -0.15, 0.35,
        false, false, false
    );

    c2D_gen->SaveAs(Form("%s-Delta2D-result-gen.pdf", output.c_str()));

    TCanvas* c2D_reco = new TCanvas("c2D_reco", "c2D_reco", 600, 600);
    TPad* p2D_reco = (TPad*) plotCMSSimple2D(
        c2D_reco, hData_result[3], "Eta-Phi Map RECO (corrected)",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "RECO", -0.15, 0.35,
        false, false, false
    );

    c2D_reco->SaveAs(Form("%s-Delta2D-result-reco.pdf", output.c_str()));

    TCanvas* c2D_gen_all = new TCanvas("c2D_gen_all", "c2D_gen_all", 600, 600);
    TPad* p2D_gen_all = (TPad*) plotCMSSimple2D(
        c2D_gen_all, hData[0], "Eta-Phi Map GEN (all)",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "GEN", -1, -1,
        false, false, false
    );

    c2D_gen_all->SaveAs(Form("%s-Delta2D-all-gen.pdf", output.c_str()));

    TCanvas* c2D_reco_all = new TCanvas("c2D_reco_all", "c2D_reco_all", 600, 600);
    TPad* p2D_reco_all = (TPad*) plotCMSSimple2D(
        c2D_reco_all, hData[3], "Eta-Phi Map RECO (all)",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "RECO", -1, -1,
        false, false, false
    );

    c2D_reco_all->SaveAs(Form("%s-Delta2D-all-reco.pdf", output.c_str()));

    TCanvas* c2D_mix_all = new TCanvas("c2D_mix_all", "c2D_mix_all", 600, 600);
    TPad* p2D_mix_all = (TPad*) plotCMSSimple2D(
        c2D_mix_all, hMixData[0], "Eta-Phi Map GEN (mixed)",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "GEN", -1, -1,
        false, false, false
    );

    c2D_mix_all->SaveAs(Form("%s-Delta2D-mix-gen.pdf", output.c_str()));

    TCanvas* c2D_mix_reco = new TCanvas("c2D_mix_reco", "c2D_mix_reco", 600, 600);
    TPad* p2D_mix_reco = (TPad*) plotCMSSimple2D(
        c2D_mix_reco, hMixData[3], "Eta-Phi Map RECO (mixed)",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "RECO", -1, -1,
        false, false, false
    );

    c2D_mix_reco->SaveAs(Form("%s-Delta2D-mix-reco.pdf", output.c_str()));


    // =================================== //
    // my result
    // =================================== //

    // gen
    TH2D* hDelta2D_gen_all_cut = (TH2D*) hData[0]->Clone("hDelta2D_gen_all_cut");
    cutY(hDelta2D_gen_all_cut, 0, 10);

    TH2D* hDelta2D_gen_bkg_cut = (TH2D*) hMixData[0]->Clone("hDelta2D_gen_bkg_cut");
    cutY(hDelta2D_gen_bkg_cut, 0, 10);

    TH2D* hDelta2D_gen_cut = (TH2D*) hDelta2D_gen_all_cut->Clone("hDelta2D_gen_cut");
    hDelta2D_gen_cut->Add(hDelta2D_gen_bkg_cut, -1);

    TCanvas* testResult = new TCanvas("testResult", "testResult", 600, 600);
    TPad* ptestResult = (TPad*) plotCMSSimple2D(
        testResult, hDelta2D_gen_cut, "Eta-Phi Map GEN (all - bkg) with cut",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "GEN", -1, -1,
        false, false, false
    );
    testResult->SaveAs(Form("%s-test-result-gen.pdf", output.c_str()));

    TH1D* hDeltaEta_gen_my = (TH1D*) hDelta2D_gen_cut->ProjectionX("hDeltaEta_my");
    

    // corrected reco
    TH2D* hDelta2D_reco_all_cut = (TH2D*) hData[3]->Clone("hDelta2D_reco_all_cut");
    cutY(hDelta2D_reco_all_cut, 0, 10);

    TH2D* hDelta2D_reco_bkg_cut = (TH2D*) hMixData[3]->Clone("hDelta2D_reco_bkg_cut");
    cutY(hDelta2D_reco_bkg_cut, 0, 10);

    TH2D* hDelta2D_reco_cut = (TH2D*) hDelta2D_reco_all_cut->Clone("hDelta2D_reco_cut");
    hDelta2D_reco_cut->Add(hDelta2D_reco_bkg_cut, -1);

    TH1D* hDeltaEta_reco_my = (TH1D*) hDelta2D_reco_cut->ProjectionX("hDeltaEta_reco_my");
    
    TCanvas* testResult_reco = new TCanvas("testResult_reco", "testResult_reco", 600, 600);
    TPad* ptestResult_reco = (TPad*) plotCMSSimple2D(
        testResult_reco, hDelta2D_reco_cut, "Eta-Phi Map RECO (all - bkg) with cut",
        "#Delta y_{ch,Z}", -4, 4,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "RECO", -1, -1,
        false, false, false
    );
    testResult_reco->SaveAs(Form("%s-test-result-reco.pdf", output.c_str()));


    TCanvas* cResult1_my = new TCanvas("cResult1_my", "cResult1_my", 600, 600);
    TPad* pResult1_my = (TPad*) plotCMSRatio(
        {hDeltaEta_gen_my, hDeltaEta_reco_my}, "", {"GEN" , "RECO"},
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "Result d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, true
    );

    AddCMSHeader(
        pResult1_my,
        "Internal",
        false
    );
    AddUPCHeader(pResult1_my, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
    cResult1_my->Update();
    cResult1_my->SaveAs(Form("%s-test-comparison.pdf", output.c_str()));



    return 0;
}
