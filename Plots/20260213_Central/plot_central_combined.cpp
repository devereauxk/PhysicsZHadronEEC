#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TSystem.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>


int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("pPbtag", "V16_nmix5");
    string tag_pp = CL.Get("pptag", "V16_nmix5");
    bool doCombine = CL.GetBool("doCombine", false);
    string collisionType = CL.Get("collisionType", "pPb");

    cout<<"=================================================="<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Track Pt Range: "<<trkPtRange<<endl;
    cout<<"pPb Tag: "<<tag<<endl;
    cout<<"pp Tag: "<<tag_pp<<endl;

    // files to load
    string input_ZPT_files_pp = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_%s_ZPT%s", tag_pp.c_str(), zPtRange.c_str());
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_%s_ZPT%s", tag.c_str(), zPtRange.c_str())
    };
    vector<string> input_ZPT_files_pbp = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str()),
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_%s_ZPT%s", tag.c_str(), zPtRange.c_str())
    };
    vector<string> labels = {
        "pp",
        Form("%s", collisionType.c_str()),
        //"  & Z correction",
        //"  & Z + track correction",
        "Powheg+EPOS"
    };
    string file_tag = (doCombine) ? "all" : collisionType;
    gSystem->mkdir(Form("plots/central_combined/%s", tag_pp.c_str()), true);
    string output = Form("plots/central_combined/%s/%s_ZPT%s_trkPT%s_%s", tag_pp.c_str(), file_tag.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag.c_str());

    // plotted histograms
    vector<TH1*> hDeltaEta_combined;
    vector<TH1*> hDeltaPhi_combined;

    // ============================
    // pp
    // ============================
    TH1D* hDeltaEta_pp;
    TH1D* hDeltaPhi_pp;

    TFile* fin_pp = TFile::Open(Form("%s-nosub.root", input_ZPT_files_pp.c_str()), "READ");
    if (!fin_pp || fin_pp->IsZombie()) {
        std::cerr << "Error: Unable to open file " << input_ZPT_files_pp << std::endl;
        return 1;
    }
    TH2D* hData_pp = (TH2D*)fin_pp->Get(Form("hData_%s", trkPtRange.c_str()));
    TH2D* hMixData_pp = (TH2D*)fin_pp->Get(Form("hMixData_%s", trkPtRange.c_str()));
    TH1D* hNZData_pp = (TH1D*)fin_pp->Get(Form("hNZData_%s", trkPtRange.c_str()));
    TH1D* hNZMixData_pp = (TH1D*)fin_pp->Get(Form("hNZMixData_%s", trkPtRange.c_str()));
    
    hData_pp->Add(hMixData_pp, -1);
    cout<<"pp hNZ integral: "<<hNZData_pp->Integral()<<endl;
    cout<<"pp S-B integral: "<<hData_pp->Integral()<<endl;

    cout<<"SUBTRACTION EFFICIENCY: "<<hData_pp->Integral() / hMixData_pp->Integral()<<endl;

    hDeltaPhi_pp = (TH1D*) hData_pp->ProjectionY("hDeltaPhi_pp",0,10);
    divideByWidth(hDeltaPhi_pp);
    hDeltaPhi_pp->Scale(1./2);
    cout<<"pp DeltaPhi integral: "<<hDeltaPhi_pp->Integral()<<endl;

    hDeltaEta_pp = (TH1D*) hData_pp->ProjectionX("hDeltaEta_pp",6,10);
    divideByWidth(hDeltaEta_pp);
    hDeltaEta_pp->Scale(1./2);
    cout<<"pp DeltaEta integral: "<<hDeltaEta_pp->Integral()<<endl;

    hDeltaPhi_combined.push_back(hDeltaPhi_pp);
    hDeltaEta_combined.push_back(hDeltaEta_pp);


    // ============================
    // pPb and PbP
    // ============================

    // read results file ppb
    vector<TH2*> hData_ppb;
    vector<TH2*> hMixData_ppb;
    vector<TH1*> hNZData_ppb;
    vector<TH1*> hNZMixData_ppb;
    for (const auto& input_ZPT : input_ZPT_files) {

        TFile* fin = TFile::Open(Form("%s-nosub.root", input_ZPT.c_str()), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // read results
        TH2D* this_hData = (TH2D*)fin->Get(Form("hData_%s", trkPtRange.c_str()));
        this_hData->SetName(Form("hData_%s", input_ZPT.c_str()));
        TH2D* this_hMixData = (TH2D*)fin->Get(Form("hMixData_%s", trkPtRange.c_str()));
        this_hMixData->SetName(Form("hMixData_%s", input_ZPT.c_str()));
        TH1D* this_hNZData = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        this_hNZData->SetName(Form("hNZData_%s", input_ZPT.c_str()));
        TH1D* this_hNZMixData = (TH1D*)fin->Get(Form("hNZMixData_%s", trkPtRange.c_str()));
        this_hNZMixData->SetName(Form("hNZMixData_%s", input_ZPT.c_str()));

        // undo NZ normalization
        this_hData->Scale(this_hNZData->GetBinContent(1));
        this_hMixData->Scale(this_hNZMixData->GetBinContent(1));

        cout<<"pPb hNZ integral: "<<this_hNZData->Integral()<<endl;

        hData_ppb.push_back(this_hData);
        hMixData_ppb.push_back(this_hMixData);
        hNZData_ppb.push_back(this_hNZData);
        hNZMixData_ppb.push_back(this_hNZMixData);
    }

    // read results file pbp
    vector<TH2*> hData_pbp;
    vector<TH2*> hMixData_pbp;
    vector<TH1*> hNZData_pbp;
    vector<TH1*> hNZMixData_pbp;
    for (const auto& input_ZPT : input_ZPT_files_pbp) {

        TFile* fin = TFile::Open(Form("%s-nosub.root", input_ZPT.c_str()), "READ");
        
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        // read results
        TH2D* this_hData = (TH2D*)fin->Get(Form("hData_%s", trkPtRange.c_str()));
        this_hData->SetName(Form("hData_pbp_%s", input_ZPT.c_str()));
        TH2D* this_hMixData = (TH2D*)fin->Get(Form("hMixData_%s", trkPtRange.c_str()));
        this_hMixData->SetName(Form("hMixData_pbp_%s", input_ZPT.c_str()));
        TH1D* this_hNZData = (TH1D*)fin->Get(Form("hNZData_%s", trkPtRange.c_str()));
        this_hNZData->SetName(Form("hNZData_pbp_%s", input_ZPT.c_str()));
        TH1D* this_hNZMixData = (TH1D*)fin->Get(Form("hNZMixData_%s", trkPtRange.c_str()));
        this_hNZMixData->SetName(Form("hNZMixData_pbp_%s", input_ZPT.c_str()));

        // undo NZ normalization
        this_hData->Scale(this_hNZData->GetBinContent(1));
        this_hMixData->Scale(this_hNZMixData->GetBinContent(1));

        cout<<"PbP hNZ integral: "<<this_hNZData->Integral()<<endl;

        hData_pbp.push_back(this_hData);
        hMixData_pbp.push_back(this_hMixData);
        hNZData_pbp.push_back(this_hNZData);
        hNZMixData_pbp.push_back(this_hNZMixData);
    }

    // mix it up if only one of pPb or PbP
    if (!doCombine && collisionType == "PbP") {
        hData_ppb = hData_pbp;
        hMixData_ppb = hMixData_pbp;
        hNZData_ppb = hNZData_pbp;
        hNZMixData_ppb = hNZMixData_pbp;
    }


    // combined results
    for (int i = 0; i < input_ZPT_files.size(); i++) {

        cout<<"Combining ppb and pbp for index "<<i<<endl;
        
        // sig+bkg
        TH2D* S_combined = (TH2D*) hData_ppb[i]->Clone("S_combined");
        if(doCombine) S_combined->Add(hData_pbp[i]);
        cout<<"combined S integral: "<<S_combined->Integral()<<endl;

        float S_NZ = hNZData_ppb[i]->GetBinContent(1);
        if(doCombine) S_NZ += hNZData_pbp[i]->GetBinContent(1);
        S_combined->Scale(1. / S_NZ);
        cout<<"combined S NZ: "<<S_NZ<<endl;

        // bkg
        TH2D* B_combined = (TH2D*) hMixData_ppb[i]->Clone("B_combined");
        if(doCombine) B_combined->Add(hMixData_pbp[i]);
        cout<<"combined B integral: "<<B_combined->Integral()<<endl;

        float B_NZ = hNZMixData_ppb[i]->GetBinContent(1);
        if(doCombine) B_NZ += hNZMixData_pbp[i]->GetBinContent(1);
        B_combined->Scale(1. / B_NZ);
        cout<<"combined B NZ: "<<B_NZ<<endl;

        // bkg subtraction
        S_combined->Add(B_combined, -1);
        cout<<"combined S-B integral: "<<S_combined->Integral()<<endl;
        cout<<"SUBTRACTION EFFICIENCY: "<<S_combined->Integral() / B_combined->Integral()<<endl;

        // projections
        TH1D* hProjY = (TH1D*) S_combined->ProjectionY(Form("DeltaPhi_Result%i",i),0,10);
        divideByWidth(hProjY);
        hProjY->Scale(1./2);
        hDeltaPhi_combined.push_back(hProjY);
        cout<<"DeltaPhi combined integral: "<<hProjY->Integral()<<endl;

        TH1D* hProjX = (TH1D*) S_combined->ProjectionX(Form("DeltaEta_Result%i",i),6,10);
        divideByWidth(hProjX);
        hProjX->Scale(1./2);
        hDeltaEta_combined.push_back(hProjX);
        cout<<"DeltaEta combined integral: "<<hProjX->Integral()<<endl;

    }

    /*
    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kMagenta-3, cmsYellow, cmsGray};
    vector<int> markerStyles = {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-6, kOrange+7, kMagenta-3, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 2, 2, 0, 1};
    */
    
    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-8, kMagenta-3, cmsYellow, cmsGray};
    vector<int> markerStyles = {mSquareFill, mCircleFill, 0, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-8, kMagenta-3, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 0, 1};

    float diffMin = (trkPtRange == "4_500") ? -0.1 : -0.35;
    float diffMax = (trkPtRange == "4_500") ? 0.1 : 0.35;

    // ===========================================
    // results
    // ===========================================
    // Parse pT ranges from "LOW_HIGH" strings
    auto ParseRange = [](const string &range) -> pair<string, string> {
        size_t pos = range.find('_');
        if (pos == string::npos) return {range, ""};
        return {range.substr(0, pos), range.substr(pos + 1)};
    };

    pair<string, string> zRange = ParseRange(zPtRange);
    pair<string, string> trkRange = ParseRange(trkPtRange);

    TCanvas* cResult1 = new TCanvas("cResult1", "cResult1", 600, 600);
    TPad* pResult1 = (TPad*) plotCMSDiff(
        hDeltaEta_combined, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        "pPb - pp", diffMin, diffMax,
        0,
        false, false, true,
        0.2
    );

    AddCMSHeader(
        pResult1,
        "Internal",
        false
    );
    AddUPCHeader(pResult1, "8.16 TeV", Form("%s, pp", collisionType.c_str()));

    // Top-right eta-plot label
    pResult1->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAlign(31);   // right-aligned
    latex.SetTextSize(0.035);
    latex.DrawLatex(0.8, 0.82, Form("%s < p_{T}^{Z} < %s", zRange.first.c_str(), zRange.second.c_str()));
    latex.DrawLatex(0.8, 0.77, Form("%s < p_{T}^{ch} < %s", trkRange.first.c_str(), trkRange.second.c_str()));
    latex.DrawLatex(0.8, 0.72, "|y_{Z}| < 2.4, |#Delta#varphi_{ch,Z}| < #pi/2");

    cResult1->Update();
    cResult1->SaveAs(Form("%s-DeltaEta-result.pdf", output.c_str()));

    TCanvas* cResult2 = new TCanvas("cResult2", "cResult2", 600, 600);
    TPad* pResult2 = (TPad*) plotCMSDiff(
        hDeltaPhi_combined, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#varphi_{ch,Z}", -1.5707, 4.7123,
        "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}", -1, -1,
        "pPb - pp", diffMin, diffMax,
        0,
        false, false, true,
        0.2
    );

    AddCMSHeader(
        pResult2,
        "Internal",
        false
    );
    AddUPCHeader(pResult2, "8.16 TeV", Form("%s, pp", collisionType.c_str()));

    // Phi-plot labels (lowest line at x=0.5, y=0.3)
    pResult2->cd();
    TLatex latex2;
    latex2.SetNDC();
    latex2.SetTextAlign(11);   // centered
    latex2.SetTextSize(0.035);
    latex2.DrawLatex(0.21, 0.60, Form("%s < p_{T}^{Z} < %s", zRange.first.c_str(), zRange.second.c_str()));
    latex2.DrawLatex(0.21, 0.55, Form("%s < p_{T}^{ch} < %s", trkRange.first.c_str(), trkRange.second.c_str()));
    latex2.DrawLatex(0.21, 0.5, "|y_{Z}| < 2.4");

    cResult2->Update();
    cResult2->SaveAs(Form("%s-DeltaPhi-result.pdf", output.c_str()));


}
