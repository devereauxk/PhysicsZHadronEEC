#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TStyle.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>

static string ReplaceAll(string Value, const string &Needle, const string &Replacement)
{
    size_t Position = 0;
    while((Position = Value.find(Needle, Position)) != string::npos)
    {
        Value.replace(Position, Needle.size(), Replacement);
        Position += Replacement.size();
    }
    return Value;
}

int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string collisionType = CL.Get("collisionType", "pPb");
    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("tag", "V16_nmix5");
    string inputTag = CL.Get("inputTag", "");

    cout<<"Collision Type: "<<collisionType<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Tag: "<<tag<<endl;

    vector<string> inputNosubFiles;
    bool UseWorkflowInputs = (inputTag.empty() == false);
    if(UseWorkflowInputs == true)
    {
        string zPtRangeDash = ReplaceAll(zPtRange, "_", "-");
        string base = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260518_ResidualCorrection2D/workflow_2D/output/closure_inputs";
        inputNosubFiles = {
            Form("%s/%s_%s_zPt%s_gen.root", base.c_str(), collisionType.c_str(), inputTag.c_str(), zPtRangeDash.c_str()),
            Form("%s/%s_%s_zPt%s_reco.root", base.c_str(), collisionType.c_str(), inputTag.c_str(), zPtRangeDash.c_str()),
            Form("%s/%s_%s_zPt%s_corrected.root", base.c_str(), collisionType.c_str(), inputTag.c_str(), zPtRangeDash.c_str())
        };
    }
    else
    {
        string mctag = (collisionType == "pp" ? "pythia" : collisionType);
        inputNosubFiles = {
            Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_Gen_nominal_%s_ZPT%s-nosub.root", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
            Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_ZResidual_%s_ZPT%s-nosub.root", mctag.c_str(), tag.c_str(), zPtRange.c_str()),
            Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/%sMC_trkResidual_%s_ZPT%s-nosub.root", mctag.c_str(), tag.c_str(), zPtRange.c_str())
        };
    }
    vector<string> labels = {
        "MC DY-GEN",
        "MC DY-RECO",
        "MC DY-RECO (corrected)"
    };
    string output = Form("plots/%s/%s_ZPT%s_%s-nosub-closure", collisionType.c_str(), collisionType.c_str(), zPtRange.c_str(), tag.c_str());

    vector<TH1*> hTrkPt;
    vector<TH1*> hTrkEta;
    vector<TH1*> hTrkPhi;
    vector<TH2D*> hTrkEtaPhiRaw;
    vector<TH2D*> hTrkEtaPhi;
    vector<TH1*> hDeltaEta_all;
    vector<TH1*> hDeltaPhi_all;
    vector<TH1*> hDeltaEta_mix;
    vector<TH1*> hDeltaPhi_mix;
    vector<TH1*> hDeltaEta_my;
    vector<TH1*> hDeltaPhi_my;
    vector<TH1*> hDeltaEta;
    vector<TH1*> hDeltaPhi;
    // Loop over nosub files
    int i = 0;
    for (const auto& inputFile : inputNosubFiles) {

        cout<<"opening file: "<<inputFile<<endl;

        TFile* fin = TFile::Open(inputFile.c_str(), "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << inputFile << std::endl;
            continue;
        }

        // track pt eta phi
        TH3D* this_hTrkPtEtaPhi = nullptr;
        TH1D* hNZ = nullptr;
        if(UseWorkflowInputs == true)
        {
            this_hTrkPtEtaPhi = (TH3D*)fin->Get("hTrkPtEtaPhiData");
            hNZ = (TH1D*)fin->Get("hNZData");
        }
        else
        {
            this_hTrkPtEtaPhi = (TH3D*)fin->Get(Form("hTrkPtEtaPhiData_%s", trkPtRange.c_str()));
            hNZ = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        }
        if(this_hTrkPtEtaPhi == nullptr || hNZ == nullptr)
        {
            cerr << "Missing closure histograms in " << inputFile << endl;
            continue;
        }
        TH1D* this_hTrkPt = this_hTrkPtEtaPhi->ProjectionX(Form("trkPt_%d", i));
        TH1D* this_hTrkEta = this_hTrkPtEtaPhi->ProjectionY(Form("trkEta_%d", i));
        TH1D* this_hTrkPhi = this_hTrkPtEtaPhi->ProjectionZ(Form("trkPhi_%d", i));
        TH2D* this_hTrkEtaPhi = (TH2D *)this_hTrkPtEtaPhi->Project3D("zy");
        this_hTrkEtaPhi->SetName(Form("trkEtaPhi_%d", i));
        TH2D* this_hTrkEtaPhiRaw = (TH2D *)this_hTrkEtaPhi->Clone(Form("trkEtaPhiRaw_%d", i));

        if(UseWorkflowInputs == true)
        {
            double NZ = hNZ->GetBinContent(1);
            if(NZ > 0)
            {
                this_hTrkPt->Scale(1.0 / NZ);
                this_hTrkEta->Scale(1.0 / NZ);
                this_hTrkPhi->Scale(1.0 / NZ);
                this_hTrkEtaPhi->Scale(1.0 / NZ);
            }
        }

        divideByWidth(this_hTrkPt);
        divideByWidth(this_hTrkEta);
        divideByWidth(this_hTrkPhi);

        hTrkPt.push_back(this_hTrkPt);
        hTrkEta.push_back(this_hTrkEta);
        hTrkPhi.push_back(this_hTrkPhi);
        hTrkEtaPhiRaw.push_back(this_hTrkEtaPhiRaw);
        hTrkEtaPhi.push_back(this_hTrkEtaPhi);

        i++;
    }

    // read results file
    if(UseWorkflowInputs == false)
    for (const auto& inputFile : inputNosubFiles) {

        string resultFile = ReplaceAll(inputFile, "-nosub.root", "-result.root");
        TFile* fin = TFile::Open(resultFile.c_str(), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << resultFile << std::endl;
            continue;
        }

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
        "p_{T}^{ch}", 0, 15,
        "(1/N_{Z}) dN_{ch}/dp_{T}^{ch}", (collisionType == "pp" ? 0 : 0), (collisionType == "pp" ? 100 : 150),
        "Ratio to GEN", 0.9, 1.1,
        0,
        true, false, false
    );

    AddCMSHeader(
        pTrk1,
        "Internal",
        false
    );

    AddUPCHeader(pTrk1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);

    cTrk1->Update();
    cTrk1->SaveAs(Form("%s-pt.pdf", output.c_str()));

    TCanvas* cTrk2 = new TCanvas("cTrk2", "cTrk2", 600, 600);
    TPad* pTrk2 = (TPad*) plotCMSRatio(
        hTrkEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "y_{ch}", -4, 4,
        "(1/N_{Z}) dN_{ch}/d y_{ch}", (collisionType == "pp" ? 0 : 8), (collisionType == "pp" ? 12 : 22),
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
        "(1/N_{Z}) dN_{ch}/d #phi_{ch}", (collisionType == "pp" ? 0 : 6), (collisionType == "pp" ? 10 : 18),
        "Ratio to GEN", 0.9, 1.1,
        0,
        false, false, false
    );

    cTrk3->Update();
    cTrk3->SaveAs(Form("%s-phi.pdf", output.c_str()));

    if(hTrkEtaPhi.size() >= 3)
    {
        gStyle->SetPalette(kBird);
        TH2D *hTrkEtaPhiRatio = (TH2D *)hTrkEtaPhi[2]->Clone("hTrkEtaPhiRatio");
        hTrkEtaPhiRatio->SetTitle(";#eta_{ch};#phi_{ch};MC RECO corrected / GEN");
        hTrkEtaPhiRatio->Divide(hTrkEtaPhi[0]);
        hTrkEtaPhiRatio->SetMinimum(0.8);
        hTrkEtaPhiRatio->SetMaximum(1.2);
        hTrkEtaPhiRatio->SetContour(100);

        gStyle->SetOptStat(0);

        TCanvas *cTrkEtaPhi = new TCanvas("cTrkEtaPhi", "cTrkEtaPhi", 700, 600);
        cTrkEtaPhi->SetRightMargin(0.18);
        cTrkEtaPhi->SetLeftMargin(0.12);
        cTrkEtaPhi->SetBottomMargin(0.12);

        hTrkEtaPhiRatio->Draw("COLZ");
        AddCMSHeader((TPad *)cTrkEtaPhi, "Internal", false);
        AddUPCHeader((TPad *)cTrkEtaPhi, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);

        cTrkEtaPhi->Update();
        cTrkEtaPhi->SaveAs(Form("%s-eta-phi-ratio.pdf", output.c_str()));
    }

    if(hTrkEtaPhiRaw.size() >= 3)
    {
        gStyle->SetPalette(kBird);
        const double sharedMaximum = max(hTrkEtaPhiRaw[0]->GetMaximum(), hTrkEtaPhiRaw[2]->GetMaximum());
        const double zMax = (sharedMaximum > 0) ? sharedMaximum : 1;

        auto drawHeatmap = [&](TH2D *histogram, const string &title, const string &suffix)
        {
            histogram->SetTitle(Form(";#eta_{ch};#phi_{ch};%s", title.c_str()));
            histogram->SetMinimum(0);
            histogram->SetMaximum(zMax);
            histogram->SetContour(100);

            gStyle->SetOptStat(0);

            TCanvas *canvas = new TCanvas(Form("cTrkEtaPhi_%s", suffix.c_str()),
                Form("cTrkEtaPhi_%s", suffix.c_str()), 700, 600);
            canvas->SetRightMargin(0.18);
            canvas->SetLeftMargin(0.12);
            canvas->SetBottomMargin(0.12);

            histogram->Draw("COLZ");
            AddCMSHeader((TPad *)canvas, "Internal", false);
            AddUPCHeader((TPad *)canvas, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);

            canvas->Update();
            canvas->SaveAs(Form("%s-%s.pdf", output.c_str(), suffix.c_str()));
        };

        TH2D *hTrkEtaPhiGen = (TH2D *)hTrkEtaPhiRaw[0]->Clone("hTrkEtaPhiGen");
        TH2D *hTrkEtaPhiCorrected = (TH2D *)hTrkEtaPhiRaw[2]->Clone("hTrkEtaPhiCorrected");

        drawHeatmap(hTrkEtaPhiGen, "MC GEN weighted counts", "eta-phi-gen");
        drawHeatmap(hTrkEtaPhiCorrected, "MC RECO corrected weighted counts", "eta-phi-corrected");
    }

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

   AddUPCHeader(pTrk1, (collisionType == "pp") ? "5.02 TeV" : "8.16 TeV", collisionType);
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
