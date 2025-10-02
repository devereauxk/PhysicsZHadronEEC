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

void plot_single(const char* input_ZPT = "", const char* output = "", const char* header_label = "") {

    // pp
    input_ZPT =  {"oldplots/pp-nosub.root"};
    output =  "summary/20251001_pp_old_ZPT40_350-nosub";
    header_label = "pp";

    //pPb
    //input_ZPT =  {"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZPT40_350-result.root"};
    //output =  "summary/20250930_pPbMC_nominal_ZPT40_350";
    //header_label = "pPb";

    TFile* fin = TFile::Open(input_ZPT, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin << std::endl;
        return;
    }

    // Define pt bins
    const char* ptBins[] = {"1_2", "2_4", "4_10"};
    const int nPtBins = 1;

    std::vector<TH1*> hDeltaPhi;
    std::vector<TH1*> hDeltaEta;
    std::vector<TH2D*> hMixData;
    std::vector<TH1*> hMixData_DeltaPhi;
    std::vector<TH1*> hMixData_DeltaEta;

    for (int i = 0; i < nPtBins; ++i) {
        TString phiName = Form("DeltaPhi_Result%s", ptBins[i]);
        TString etaName = Form("DeltaEta_Result%s", ptBins[i]);
        TString mixName = Form("hMixData_%s", ptBins[i]);
        TString nMixName = Form("hNZMixData_%s", ptBins[i]);

        hDeltaPhi.push_back((TH1D*)fin->Get(phiName));
        hDeltaEta.push_back((TH1D*)fin->Get(etaName));
        hMixData.push_back((TH2D*)fin->Get(mixName));

        // Get normalization factor from hNZMixData_%s histogram, bin 1
        TH1D* hNZMix = (TH1D*)fin->Get(nMixName);
        double norm = (hNZMix) ? hNZMix->GetBinContent(1) : 1.0;

        // Project and normalize
        TString mixPhiName = Form("hMixData_%s_DeltaPhi", ptBins[i]);
        TString mixEtaName = Form("hMixData_%s_DeltaEta", ptBins[i]);
        TH1D* projPhi = hMixData.back()->ProjectionY(mixPhiName, 0, 10);
        TH1D* projEta = hMixData.back()->ProjectionX(mixEtaName, 6, 10);

        if (norm != 0) {
            projPhi->Scale(1.0 / norm);
            projEta->Scale(1.0 / norm);
        }

        hMixData_DeltaPhi.push_back(projPhi);
        hMixData_DeltaEta.push_back(projEta);

        // Scale all TH1Ds by 0.5
        if (hDeltaPhi.back()) hDeltaPhi.back()->Scale(0.5);
        if (hDeltaEta.back()) hDeltaEta.back()->Scale(0.5);
        if (projPhi) projPhi->Scale(0.5);
        if (projEta) projEta->Scale(0.5);
    }

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 650, 600);

    TPad* p1 = (TPad*) plotCMSSimple(
        c1, hDeltaEta, "", {"1<p_{T}^{trk}<2 GeV", "2<p_{T}^{trk}<4 GeV", "4<p_{T}^{trk}<10 GeV"},
        {cmsRed, cmsYellow, cmsTealL1, cmsYellow, cmsGray}, {0, 0, 0, 1, 1},
        {cmsRed, cmsYellow, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
        false, false, false, 0
    );

    AddCMSHeader(
        p1,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );

    AddUPCHeader(p1, "5.02 TeV", header_label);
    p1->Update();

    c1->SaveAs(Form("%s-DeltaEta.png", output));

    //make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 650, 600);
    TPad* p2 = (TPad*) plotCMSSimple(
        c2, hDeltaPhi, "", {"1<p_{T}^{trk}<2 GeV", "2<p_{T}^{trk}<4 GeV", "4<p_{T}^{trk}<10 GeV"},
        {cmsRed, cmsYellow, cmsTealL1, cmsYellow, cmsGray}, {0, 0, 0, 1, 1},
        {cmsRed, cmsYellow, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta#phi_{ch,Z}", -1.5758, 4.7275,
        "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}", -1, -1,
        false, false, false, 0
    );

    AddCMSHeader(
        p2,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );

    AddUPCHeader(p2, "5.02 TeV", header_label);
    p2->Update();

    c2->SaveAs(Form("%s-DeltaPhi.png", output));

    //make canvas
    TCanvas* c3 = new TCanvas("c3", "c3", 650, 600);
    TPad* p3 = (TPad*) plotCMSSimple(
        c3, hMixData_DeltaEta, "", {"1<p_{T}^{trk}<2 GeV", "2<p_{T}^{trk}<4 GeV", "4<p_{T}^{trk}<10 GeV"},
        {cmsRed, cmsYellow, cmsTealL1, cmsYellow, cmsGray}, {0, 0, 0, 1, 1},
        {cmsRed, cmsYellow, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z} (mixed)", -1, -1,
        false, false, false, 0
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
        c4, hMixData_DeltaPhi, "", {"1<p_{T}^{trk}<2 GeV", "2<p_{T}^{trk}<4 GeV", "4<p_{T}^{trk}<10 GeV"},
        {cmsRed, cmsYellow, cmsTealL1, cmsYellow, cmsGray}, {0, 0, 0, 1, 1},
        {cmsRed, cmsYellow, cmsTealL1, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
        "#Delta#phi_{ch,Z}", -1.5758, 4.7275,
        "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z} (mixed)", -1, -1,
        false, false, false, 0
    );

    AddCMSHeader(
        p4,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );

    AddUPCHeader(p4, "5.02 TeV", header_label);
    p4->Update();

    c4->SaveAs(Form("%s-DeltaPhi_Mixed.png", output));

}