#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TSystem.h>
#include <TTree.h>
#include <iostream>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h" // Kyle's plotting utilities

#include "CommandLine.h"
#include "SetStyle.h"

#include <vector>
#include <string>
#include <cmath>

TH1D *LoadSystematicHistogram(const string &fileName, const string &histogramName,
    const string &cloneName)
{
    TFile input(fileName.c_str(), "READ");
    if (input.IsZombie())
        return nullptr;

    TH1D *histogram = (TH1D *)input.Get(histogramName.c_str());
    if (histogram == nullptr)
        return nullptr;

    histogram = (TH1D *)histogram->Clone(cloneName.c_str());
    histogram->SetDirectory(nullptr);
    return histogram;
}

struct JackknifeProjectionContribution {
    double SignalNZ = 0;
    double MixNZ = 0;
    vector<float> SignalEta;
    vector<float> MixEta;
    vector<float> SignalPhi;
    vector<float> MixPhi;
};

void AppendJackknifeProjectionContributions(TFile *file, const string &treeName,
    vector<JackknifeProjectionContribution> &events)
{
    if (file == nullptr)
        return;

    TTree *tree = (TTree *)file->Get(treeName.c_str());
    if (tree == nullptr)
        return;

    double signalNZ = 0;
    double mixNZ = 0;
    vector<float> *signalEta = nullptr;
    vector<float> *mixEta = nullptr;
    vector<float> *signalPhi = nullptr;
    vector<float> *mixPhi = nullptr;

    tree->SetBranchAddress("SignalNZ", &signalNZ);
    tree->SetBranchAddress("MixNZ", &mixNZ);
    tree->SetBranchAddress("SignalEta", &signalEta);
    tree->SetBranchAddress("MixEta", &mixEta);
    tree->SetBranchAddress("SignalPhi", &signalPhi);
    tree->SetBranchAddress("MixPhi", &mixPhi);

    Long64_t entryCount = tree->GetEntries();
    for (Long64_t entry = 0; entry < entryCount; ++entry) {
        tree->GetEntry(entry);
        JackknifeProjectionContribution current;
        current.SignalNZ = signalNZ;
        current.MixNZ = mixNZ;
        current.SignalEta = *signalEta;
        current.MixEta = *mixEta;
        current.SignalPhi = *signalPhi;
        current.MixPhi = *mixPhi;
        events.push_back(current);
    }
}

vector<double> ComputeProjectedJackknifeSigma(const vector<JackknifeProjectionContribution> &events,
    const TH1D *fullHistogram, bool useEta)
{
    vector<double> sigma(fullHistogram->GetNbinsX(), 0);
    if (events.size() < 2)
        return sigma;

    double totalSignalNZ = 0;
    double totalMixNZ = 0;
    vector<double> fullSignal(fullHistogram->GetNbinsX(), 0);
    vector<double> fullMix(fullHistogram->GetNbinsX(), 0);
    for (const JackknifeProjectionContribution &event : events) {
        totalSignalNZ += event.SignalNZ;
        totalMixNZ += event.MixNZ;
        for (int i = 0; i < fullHistogram->GetNbinsX(); ++i) {
            fullSignal[i] += (useEta ? event.SignalEta[i] : event.SignalPhi[i]);
            fullMix[i] += (useEta ? event.MixEta[i] : event.MixPhi[i]);
        }
    }

    int validEvents = 0;
    for (const JackknifeProjectionContribution &event : events) {
        if (totalSignalNZ - event.SignalNZ <= 0)
            continue;
        if (totalMixNZ - event.MixNZ <= 0)
            continue;
        validEvents = validEvents + 1;
    }
    if (validEvents < 2)
        return sigma;

    for (int i = 1; i <= fullHistogram->GetNbinsX(); ++i) {
        double fullValue = fullSignal[i - 1] / totalSignalNZ - fullMix[i - 1] / totalMixNZ;
        double varianceSum = 0;
        for (const JackknifeProjectionContribution &event : events) {
            if (totalSignalNZ - event.SignalNZ <= 0)
                continue;
            if (totalMixNZ - event.MixNZ <= 0)
                continue;

            double signalWithoutEvent = (fullSignal[i - 1] - (useEta ? event.SignalEta[i - 1] : event.SignalPhi[i - 1]))
                / (totalSignalNZ - event.SignalNZ);
            double mixWithoutEvent = (fullMix[i - 1] - (useEta ? event.MixEta[i - 1] : event.MixPhi[i - 1]))
                / (totalMixNZ - event.MixNZ);
            double valueWithoutEvent = signalWithoutEvent - mixWithoutEvent;
            double delta = valueWithoutEvent - fullValue;
            varianceSum += delta * delta;
        }

        sigma[i - 1] = std::sqrt((validEvents - 1.0) / validEvents * varianceSum);
    }

    return sigma;
}

void ApplyProjectedJackknifeErrors(TH1D *etaHistogram, TH1D *phiHistogram,
    const vector<JackknifeProjectionContribution> &events)
{
    if (events.size() < 2)
        return;

    vector<double> etaSigma = ComputeProjectedJackknifeSigma(events, etaHistogram, true);
    vector<double> phiSigma = ComputeProjectedJackknifeSigma(events, phiHistogram, false);

    for (int i = 1; i <= etaHistogram->GetNbinsX(); ++i)
        etaHistogram->SetBinError(i, etaSigma[i - 1]);
    for (int i = 1; i <= phiHistogram->GetNbinsX(); ++i)
        phiHistogram->SetBinError(i, phiSigma[i - 1]);
}


int main(int argc, char *argv[]) {

    CommandLine CL(argc, argv);

    string zPtRange = CL.Get("zPtRange", "40_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_500");
    string tag = CL.Get("pPbtag", "V16_nmix5");
    string mcTag = CL.Get("pPbMCTag", tag);
    string systematicsTag = CL.Get("pPbSystematicsTag", tag);
    string tag_pp = CL.Get("pptag", "V16_nmix5");
    string systematicsTagPP = CL.Get("ppSystematicsTag", tag_pp);
    bool doCombine = CL.GetBool("doCombine", false);
    bool includeMC = CL.GetBool("includeMC", true);
    string collisionType = CL.Get("collisionType", "pPb");
    string outputBase = CL.Get("outputBase", "plots/central_combined");
    string systematicsDir = CL.Get("systematicsDir",
        "/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output");

    cout<<"=================================================="<<endl;
    cout<<"Z Pt Range: "<<zPtRange<<endl;
    cout<<"Track Pt Range: "<<trkPtRange<<endl;
    cout<<"pPb Tag: "<<tag<<endl;
    cout<<"pp Tag: "<<tag_pp<<endl;

    // files to load
    string input_ZPT_files_pp = Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_%s_ZPT%s", tag_pp.c_str(), zPtRange.c_str());
    vector<string> input_ZPT_files = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str())
    };
    vector<string> input_ZPT_files_pbp = {
        Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_trkResidual_%s_ZPT%s", tag.c_str(), zPtRange.c_str())
    };
    if (includeMC) {
        input_ZPT_files.push_back(Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_%s_ZPT%s", mcTag.c_str(), zPtRange.c_str()));
        input_ZPT_files_pbp.push_back(Form("/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_%s_ZPT%s", mcTag.c_str(), zPtRange.c_str()));
    }
    vector<string> labels = {
        "pp",
        Form("%s", collisionType.c_str())
    };
    if (includeMC)
        labels.push_back("Powheg+EPOS");
    string file_tag = (doCombine) ? "all" : collisionType;
    gSystem->mkdir(Form("%s/%s", outputBase.c_str(), tag_pp.c_str()), true);
    string output = Form("%s/%s/%s_ZPT%s_trkPT%s_%s", outputBase.c_str(), tag_pp.c_str(), file_tag.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag.c_str());

    // plotted histograms
    vector<TH1*> hDeltaEta_combined;
    vector<TH1*> hDeltaPhi_combined;

    // ============================
    // pp
    // ============================
    TH1D* hDeltaEta_pp;
    TH1D* hDeltaPhi_pp;

    TFile* fin_pp = TFile::Open(Form("%s-result.root", input_ZPT_files_pp.c_str()), "READ");
    if (!fin_pp || fin_pp->IsZombie()) {
        std::cerr << "Error: Unable to open file " << input_ZPT_files_pp << std::endl;
        return 1;
    }
    hDeltaPhi_pp = (TH1D*)fin_pp->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
    hDeltaEta_pp = (TH1D*)fin_pp->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
    if (hDeltaPhi_pp == nullptr || hDeltaEta_pp == nullptr) {
        std::cerr << "Error: Missing pp result histograms in " << input_ZPT_files_pp << std::endl;
        return 1;
    }
    hDeltaPhi_pp = (TH1D*)hDeltaPhi_pp->Clone("hDeltaPhi_pp");
    hDeltaEta_pp = (TH1D*)hDeltaEta_pp->Clone("hDeltaEta_pp");
    hDeltaPhi_pp->SetDirectory(nullptr);
    hDeltaEta_pp->SetDirectory(nullptr);
    hDeltaPhi_pp->Scale(1. / 2);
    hDeltaEta_pp->Scale(1. / 2);
    cout<<"pp DeltaPhi integral: "<<hDeltaPhi_pp->Integral()<<endl;
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
    vector<JackknifeProjectionContribution> jackknifePPb;
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
        if (hData_ppb.size() == 1)
            AppendJackknifeProjectionContributions(fin, Form("JackknifeProjection%s", trkPtRange.c_str()), jackknifePPb);
    }

    // read results file pbp
    vector<TH2*> hData_pbp;
    vector<TH2*> hMixData_pbp;
    vector<TH1*> hNZData_pbp;
    vector<TH1*> hNZMixData_pbp;
    vector<JackknifeProjectionContribution> jackknifePbP;
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
        if (hData_pbp.size() == 1)
            AppendJackknifeProjectionContributions(fin, Form("JackknifeProjection%s", trkPtRange.c_str()), jackknifePbP);
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
        TH1D* hProjX = (TH1D*) S_combined->ProjectionX(Form("DeltaEta_Result%i",i),6,10);
        if (i == 0) {
            vector<JackknifeProjectionContribution> jackknifeEvents = jackknifePPb;
            if (doCombine)
                jackknifeEvents.insert(jackknifeEvents.end(), jackknifePbP.begin(), jackknifePbP.end());
            else if (collisionType == "PbP")
                jackknifeEvents = jackknifePbP;
            ApplyProjectedJackknifeErrors(hProjX, hProjY, jackknifeEvents);
        }
        divideByWidth(hProjY);
        hProjY->Scale(1./2);
        divideByWidth(hProjX);
        hProjX->Scale(1./2);
        hDeltaPhi_combined.push_back(hProjY);
        hDeltaEta_combined.push_back(hProjX);
        cout<<"DeltaPhi combined integral: "<<hProjY->Integral()<<endl;
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

    string currentSystem = doCombine ? "pPbPbp" : collisionType;
    string ppSystematicsFile = Form("%s/pp_%s_ZPT%s_trkPT%s-systematics.root",
        systematicsDir.c_str(), systematicsTagPP.c_str(), zPtRange.c_str(), trkPtRange.c_str());
    string currentSystematicsFile = Form("%s/%s_%s_ZPT%s_trkPT%s-systematics.root",
        systematicsDir.c_str(), currentSystem.c_str(), systematicsTag.c_str(), zPtRange.c_str(), trkPtRange.c_str());

    vector<TH1 *> topSystematicsEta = {
        LoadSystematicHistogram(ppSystematicsFile, "Total_DeltaEta", "PPSystematicsEta"),
        LoadSystematicHistogram(currentSystematicsFile, "Total_DeltaEta", "CurrentSystematicsEta")
    };
    vector<TH1 *> topSystematicsPhi = {
        LoadSystematicHistogram(ppSystematicsFile, "Total_DeltaPhi", "PPSystematicsPhi"),
        LoadSystematicHistogram(currentSystematicsFile, "Total_DeltaPhi", "CurrentSystematicsPhi")
    };
    vector<TH1 *> differenceSystematicsEta = {
        nullptr,
        doCombine ? LoadSystematicHistogram(currentSystematicsFile, "DifferenceTotal_DeltaEta", "DifferenceSystematicsEta") : nullptr
    };
    vector<TH1 *> differenceSystematicsPhi = {
        nullptr,
        doCombine ? LoadSystematicHistogram(currentSystematicsFile, "DifferenceTotal_DeltaPhi", "DifferenceSystematicsPhi") : nullptr
    };
    if (includeMC) {
        topSystematicsEta.push_back(nullptr);
        topSystematicsPhi.push_back(nullptr);
        differenceSystematicsEta.push_back(nullptr);
        differenceSystematicsPhi.push_back(nullptr);
    }

    string differenceLabel = doCombine ? "pPb - pp" : Form("%s - pp", collisionType.c_str());
    string lumiLabel = doCombine ? "pPb, pp" : Form("%s, pp", collisionType.c_str());

    TCanvas* cResult1 = new TCanvas("cResult1", "cResult1", 600, 600);
    TPad* pResult1 = (TPad*) PlotCMSDiffResult(
        hDeltaEta_combined, topSystematicsEta, differenceSystematicsEta, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        differenceLabel.c_str(), -1, -1,
        0,
        false, false, true,
        0.2
    );

    AddUPCHeader(pResult1, "8.16 TeV", lumiLabel.c_str());

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
    TPad* pResult2 = (TPad*) PlotCMSDiffResult(
        hDeltaPhi_combined, topSystematicsPhi, differenceSystematicsPhi, "", labels,
        lineColors, lineStyles, 
        markerColors, markerStyles,
        "#Delta#varphi_{ch,Z}", -1.5707, 4.7123,
        "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}", -1, -1,
        differenceLabel.c_str(), -1, -1,
        0,
        false, false, true,
        0.2
    );

    AddUPCHeader(pResult2, "8.16 TeV", lumiLabel.c_str());

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
