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

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("tag", "V16_nmix5");

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;

    // files to load
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s", collisionType.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_ZResidual_%s_ZPT%s", collisionType.c_str(), tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_trkResidual_%s_ZPT%s", collisionType.c_str(), tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "MC DY-GEN",
        "MC DY-RECO",
        "MC DY-RECO (corrected)"
    };
    string output = Form("plots/%s/%s_ZPT%s_%s-nosub-closure", collisionType.c_str(), collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;
    vector<TH1*> hTrkPhi;
    vector<TH1*> hDeltaEta_all;
    vector<TH1*> hDeltaPhi_all;

    vector<TH1*> hMixData;
    vector<TH1*> hDeltaEta_mix;
    vector<TH1*> hDeltaPhi_mix;

    vector<TH1*> hDeltaEta_my;
    vector<TH1*> hDeltaPhi_my;

    // Loop over nosub files
    int i = 0;
    for (const auto& input_ZPT : input_ZPT_files) {

        cout<<"opening file: "<<Form("%s-nosub.root", input_ZPT.c_str())<<endl;

        TFile* fin = TFile::Open(Form("%s-nosub.root", input_ZPT.c_str()), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // track pt eta phi
        TH3D* this_hTrkPtEtaPhi = (TH3D*)fin->Get(Form("hTrkPtEtaPhiData_%s", trkPtRange.c_str()));
        TH1D* this_hTrkPt = this_hTrkPtEtaPhi->ProjectionX(Form("trkPt_%d", i));
        TH1D* this_hTrkEta = this_hTrkPtEtaPhi->ProjectionY(Form("trkEta_%d", i));
        TH1D* this_hTrkPhi = this_hTrkPtEtaPhi->ProjectionZ(Form("trkPhi_%d", i));

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hTrkPhi);

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);
        hTrkPhi.push_back(this_hTrkPhi);

        // delta phi, delta eta
        TH1D* this_hDeltaEta_all = (TH1D*)fin->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
        this_hDeltaEta_all->SetName(Form("DeltaEta_all_%d", i));
        TH1D* this_hDeltaPhi_all = (TH1D*)fin->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
        this_hDeltaPhi_all->SetName(Form("DeltaPhi_all_%d", i));

        hDeltaEta_all.push_back(this_hDeltaEta_all); 
        hDeltaPhi_all.push_back(this_hDeltaPhi_all);

        // mixed
        TH2D* this_hMixData2D = (TH2D*)fin->Get(Form("hMixData_%s", trkPtRange.c_str()));
        TH1D* this_hDeltaPhi_mix = this_hMixData2D->ProjectionY(Form("hMixPhi_%d", i), 0, 10);
        TH1D* this_hDeltaEta_mix = this_hMixData2D->ProjectionX(Form("hMixEta_%d", i), 6, 10);

        divideByWidth(this_hDeltaPhi_mix);
        divideByWidth(this_hDeltaEta_mix);

        hMixData.push_back(this_hMixData2D);
        hDeltaEta_mix.push_back(this_hDeltaEta_mix);
        hDeltaPhi_mix.push_back(this_hDeltaPhi_mix);

        // my calculation
        TH2D* this_hData2D = (TH2D*)fin->Get(Form("hData_%s", trkPtRange.c_str()));
        TH2D* this_myResult2D = (TH2D*)this_hData2D->Clone(Form("myResult2D_%d", i));
        this_myResult2D->Add(this_hMixData2D, -1);

        TH1D* this_hDeltaPhi_my = this_myResult2D->ProjectionY(Form("DeltaPhi_my_%d", i), 0, 10);
        TH1D* this_hDeltaEta_my = this_myResult2D->ProjectionX(Form("DeltaEta_my_%d", i), 6, 10);

        divideByWidth(this_hDeltaPhi_my);
        divideByWidth(this_hDeltaEta_my);

        hDeltaEta_my.push_back(this_hDeltaEta_my);
        hDeltaPhi_my.push_back(this_hDeltaPhi_my);

        i++;
    }

    // read results file
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaPhi;

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

        hDeltaEta.push_back(this_hDeltaEta);
        hDeltaPhi.push_back(this_hDeltaPhi);

        i++;
        
    }


    vector<int> markerColors = {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, cmsYellow, kOrange+7, kSpring+7, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 1, 1, 1};

    // make canvas
    TCanvas* cTrk1 = new TCanvas("cTrk", "cTrk", 600, 600);

    TPad* pTrk1 = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "p_{T}^{ch}", 0, 10,
        "(1/N_{Z}) dN_{ch}/dp_{T}^{ch}", -27, 160,
        "Ratio to GEN", 0.9, 1.1,
        0,
        true, false, false
    );

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, "8 TeV", "pPb MC");

    cTrk1->Update();
    cTrk1->SaveAs(Form("%s-pt.pdf", output.c_str()));

    TCanvas* cTrk2 = new TCanvas("cTrk2", "cTrk2", 600, 600);
    TPad* pTrk2 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "y_{ch}", -4, 4,
        "(1/N_{Z}) dN_{ch}/d y_{ch}", (collisionType == "pp" ? -1 : 8), (collisionType == "pp" ? -1 : 22),
        "Ratio to GEN", 0.9, 1.1,
        0,
        false, false, false
    );

    cTrk2->Update();
    cTrk2->SaveAs(Form("%s-eta.pdf", output.c_str()));

    TCanvas* cTrk3 = new TCanvas("cTrk3", "cTrk3", 600, 600);
    TPad* pTrk3 = (TPad*) plotCMSRatio(
        hTrkPhi, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#phi_{ch}", 0, 2*M_PI,
        "(1/N_{Z}) dN_{ch}/d #phi_{ch}", (collisionType == "pp" ? -1 : 6), (collisionType == "pp" ? -1 : 18),
        "Ratio to GEN", 0.9, 1.1,
        0,
        false, false, false
    );

    cTrk3->Update();
    cTrk3->SaveAs(Form("%s-phi.pdf", output.c_str()));

    return 0;

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);

    TPad* p1 = (TPad*) plotCMSRatio(
        hDeltaEta_all, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "All #Delta y_{ch,Z}", -4, 4,
        "d#DeltaN_{ch}/d#Delta y_{ch,Z}", 0, 18,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, true
    );

    AddCMSHeader(
        p1,
        "Internal",
        false
    );

    AddUPCHeader(p1, "8 TeV", "pPb");
    p1->Update();

    c1->SaveAs(Form("%s-DeltaEta-all.pdf", output.c_str()));


    // make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
    
    TPad* p2 = (TPad*) plotCMSRatio(
        hDeltaPhi_all, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "All #Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", 15, 50,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, true
    );

    AddCMSHeader(
        p2,
        "Internal",
        false
    );

    AddUPCHeader(p2, "8 TeV", "pPb");
    p2->Update();

    c2->SaveAs(Form("%s-DeltaPhi-all.pdf", output.c_str()));


    TCanvas* cMix1 = new TCanvas("cMix1", "cMix1", 600, 600);
    TPad* pMix1 = (TPad*) plotCMSRatio(
        hDeltaEta_mix, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "Mixed d#DeltaN_{ch}/d#Delta y_{ch,Z}", 0, 18,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, true
    );

    AddCMSHeader(
        pMix1,
        "Internal",
        false
    );
    AddUPCHeader(pMix1, "8 TeV", "pPb");
    cMix1->Update();
    cMix1->SaveAs(Form("%s-DeltaEta-bkg.pdf", output.c_str()));

    TCanvas* cMix2 = new TCanvas("cMix2", "cMix2", 600, 600);
    TPad* pMix2 = (TPad*) plotCMSRatio(
        hDeltaPhi_mix, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Mixed d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", 15, 50,
        "Ratio to GEN", 0.8, 1.2,
        0,
        false, false, true
    );

    AddCMSHeader(
        pMix2,
        "Internal",
        false
    );
    AddUPCHeader(pMix2, "8 TeV", "pPb");
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
    AddUPCHeader(pResult1, "8 TeV", "pPb");
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
    AddUPCHeader(pResult2, "8 TeV", "pPb");
    cResult2->Update();
    cResult2->SaveAs(Form("%s-DeltaPhi-result.pdf", output.c_str()));

    TCanvas* cResult1_my = new TCanvas("cResult1_my", "cResult1_my", 600, 600);
    TPad* pResult1_my = (TPad*) plotCMSRatio(
        hDeltaEta_my, "", labels,
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
    AddUPCHeader(pResult1_my, "8 TeV", "pPb");
    cResult1_my->Update();

    cResult1_my->SaveAs(Form("%s-DeltaEta-myresult.pdf", output.c_str()));


    // save histograms
    /*
    TFile* fout = TFile::Open(Form("%s-closure.root", output.c_str()), "RECREATE");
    fout->cd();
    for (size_t i = 0; i < hTrkPt.size(); i++) {
        hTrkPt[i]->Write();
        hTrkEta[i]->Write();
        hTrkPhi[i]->Write();
        hDeltaEta[i]->Write();
        hDeltaPhi[i]->Write();
    }
    fout->Close();
    */

    return 0;
}
