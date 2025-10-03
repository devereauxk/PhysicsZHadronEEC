#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void NormalizeHistogram(TH1D *h, TH1D *h2) {
    if (!h || !h2) {
        cerr << "Error: One or both of the histograms are null." << endl;
        return;
    }

    double integral_h = h->Integral();
    double integral_h2 = h2->Integral();

    if (integral_h == 0 || integral_h2 == 0) {
        cerr << "Error: One or both of the histograms have zero integral." << endl;
        return;
    }

    double shift = (integral_h2 - integral_h) / h->GetNbinsX();
    cout <<"shift="<<shift<<endl;

    for (int i = 1; i <= h->GetNbinsX(); ++i) {
        h->SetBinContent(i, h->GetBinContent(i) + shift);
    }
}

void plot_centralValues() {

    vector<string> input_ZPT_files = {
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20240223_Zhadron/workflow/plots/pp-v11-nosub.root",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_ZPT40_350-nosub.root"
    };
    vector<string> labels = {
        "pp old analysis", "pp new analysis"
    };

    const char* output =  "central_values/20251001_pp_central_ZPT40_350";
    const char* header_label = "pp";

    const char* ptBin = "1_2";

    std::vector<TH1*> hDeltaPhi;
    std::vector<TH1*> hDeltaEta;
    std::vector<TH1*> hData_DeltaPhi;
    std::vector<TH1*> hData_DeltaEta;
    std::vector<TH1*> hMixData_DeltaPhi;
    std::vector<TH1*> hMixData_DeltaEta;
    std::vector<TH1*> hDeltaPhi_result;
    std::vector<TH1*> hDeltaEta_result;

    // Only fill for the pT bin defined as ptBin
    for (size_t fileIdx = 0; fileIdx < input_ZPT_files.size(); ++fileIdx) {
        const char* input_ZPT = input_ZPT_files[fileIdx].c_str();
        TFile* fin = TFile::Open(input_ZPT, "READ");
        if (!fin || fin->IsZombie()) {
            std::cerr << "Error: Unable to open file " << input_ZPT << std::endl;
            continue;
        }

        TString phiName = Form("DeltaPhi_Result%s", ptBin);
        TString etaName = Form("DeltaEta_Result%s", ptBin);

        TH1D* hPhi = (TH1D*)fin->Get(phiName);
        hPhi->SetName(Form("DeltaPhi_%s", labels[fileIdx].c_str()));
        TH1D* hEta = (TH1D*)fin->Get(etaName);
        hEta->SetName(Form("DeltaEta_%s", labels[fileIdx].c_str()));

        hDeltaPhi.push_back(hPhi);
        hDeltaEta.push_back(hEta);

        TH2D* hMixData = (TH2D*)fin->Get(Form("hMixData_%s", ptBin));
        TH1D* hMixPhi = (TH1D*) hMixData->ProjectionY(Form("MixDeltaPhi_%s", labels[fileIdx].c_str()), 0, 10);
        TH1D* hMixEta = (TH1D*) hMixData->ProjectionX(Form("MixDeltaEta_%s", labels[fileIdx].c_str()), 6, 10);

        hMixData_DeltaPhi.push_back(hMixPhi);
        hMixData_DeltaEta.push_back(hMixEta);

        TH2D* hData = (TH2D*)fin->Get(Form("hData_%s", ptBin));
        TH1D* hDataPhi = (TH1D*) hData->ProjectionY(Form("DataDeltaPhi_%s", labels[fileIdx].c_str()), 0, 10);
        TH1D* hDataEta = (TH1D*) hData->ProjectionX(Form("DataDeltaEta_%s", labels[fileIdx].c_str()), 6, 10);

        TH1D* hPhi_result = (TH1D*)hDataPhi->Clone(Form("DeltaPhi_result_%s", labels[fileIdx].c_str()));
        hPhi_result->Add(hMixPhi, -1);
        TH1D* hEta_result = (TH1D*)hDataEta->Clone(Form("DeltaEta_result_%s", labels[fileIdx].c_str()));
        hEta_result->Add(hMixEta, -1);

        hDeltaPhi_result.push_back(hPhi_result);
        hDeltaEta_result.push_back(hEta_result);
    }

    // Define style vectors to be used in all plots
    std::vector<int> linecolors = {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsGray};
    std::vector<int> linestyles = {0, 0, 2, 1, 1, 0};
    std::vector<int> markercolors = {cmsBlue, cmsRed, cmsTealL1, kOrange+7, kSpring+7, kMagenta+1, cmsRed, cmsRed};
    std::vector<int> markerstyles = {mSquareFill, mCircleFill, mCircleFill, mCircleFill};

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 650, 600);

    TPad* p1 = (TPad*) plotCMSSimple(
        c1, hDeltaEta, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        false, false, false, "0"
    );

    AddCMSHeader(
        p1,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    AddUPCHeader(p1, "5.02 TeV", header_label);
    p1->Update();

    c1->SaveAs(Form("%s-DeltaEta.png", output));

    //make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 650, 600);
    TPad* p2 = (TPad*) plotCMSSimple(
        c2, hDeltaPhi, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "#Delta#phi_{ch,Z}", -1.5758, 4.7275,
        "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, -1,
        false, false, false, "0"
    );

    AddCMSHeader(
        p2,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );

    AddUPCHeader(p2, "5.02 TeV", header_label);
    p2->Update();

    c2->SaveAs(Form("%s-DeltaPhi.png", output));

    //make canvas
    TCanvas* c3 = new TCanvas("c3", "c3", 650, 600);
    TPad* p3 = (TPad*) plotCMSSimple(
        c3, hMixData_DeltaEta, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z} (mixed)", -0.5, 1.2,
        false, false, false, "0"
    );

    AddCMSHeader(
        p3,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );

    AddUPCHeader(p3, "5.02 TeV", header_label);
    p3->Update();

    c3->SaveAs(Form("%s-DeltaEta_Mixed.png", output));

    //make canvas
    TCanvas* c4 = new TCanvas("c4", "c4", 650, 600);
    TPad* p4 = (TPad*) plotCMSSimple(
        c4, hMixData_DeltaPhi, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "#Delta#phi_{ch,Z}", -1.5758, 4.7275,
        "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z} (mixed)", -0.4, 3,
        false, false, false, "0"
    );

    AddCMSHeader(
        p4,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );

    AddUPCHeader(p4, "5.02 TeV", header_label);
    p4->Update();

    c4->SaveAs(Form("%s-DeltaPhi_Mixed.png", output));

    //make canvas
    TCanvas* c5 = new TCanvas("c5", "c5", 650, 600);
    TPad* p5 = (TPad*) plotCMSSimple(
        c5, hDeltaEta_result, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z} (result)", -0.5, 1.2,
        false, false, false, "0"
    );

    AddCMSHeader(
        p5,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );

    AddUPCHeader(p5, "5.02 TeV", header_label);
    p5->Update();
    c5->SaveAs(Form("%s-DeltaEta_Result.png", output));

    //make canvas
    TCanvas* c6 = new TCanvas("c6", "c6", 650, 600);
    TPad* p6 = (TPad*) plotCMSSimple(
        c6, hDeltaPhi_result, "", labels,
        linecolors, linestyles,
        markercolors, markerstyles,
        "#Delta#phi_{ch,Z}", -1.5758, 4.7275,
        "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z} (result)", -1, 2,
        false, false, false, "0"
    );

    AddCMSHeader(
        p6,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );

    AddUPCHeader(p6, "5.02 TeV", header_label);
    p6->Update();
    c6->SaveAs(Form("%s-DeltaPhi_Result.png", output));

}