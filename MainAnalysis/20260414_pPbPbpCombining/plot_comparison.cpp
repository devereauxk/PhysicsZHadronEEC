#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "KylesPlotting.h"
#include "CommandLine.h"
#include "SetStyle.h"

int main(int argc, char *argv[]) {
    SetThesisStyle();

    struct CorrLevel {
        string label;
        string title;
        string pPbFile;
        string PbPFile;
    };

    vector<CorrLevel> levels = {
        {"raw",                    "No corrections",               "output/pPb_raw.root",                    "output/PbP_raw.root"},
        {"VZ",                     "+VZ",                          "output/pPb_VZ.root",                     "output/PbP_VZ.root"},
        {"VZ_Z",                   "+VZ+Z",                        "output/pPb_VZ_Z.root",                   "output/PbP_VZ_Z.root"},
        {"VZ_Z_ZSF",               "+VZ+Z+ZSF",                    "output/pPb_VZ_Z_ZSF.root",               "output/PbP_VZ_Z_ZSF.root"},
        {"VZ_Z_ZSF_trk",           "+VZ+Z+ZSF+trk",                "output/pPb_VZ_Z_ZSF_trk.root",           "output/PbP_VZ_Z_ZSF_trk.root"},
        {"VZ_Z_ZSF_trk_res",       "+VZ+Z+ZSF+trk+res",            "output/pPb_VZ_Z_ZSF_trk_res.root",       "output/PbP_VZ_Z_ZSF_trk_res.root"},
        {"raw_PUReject",           "No corrections, N_{vtx}=1",    "output/pPb_raw_PUReject.root",            "output/PbP_raw_PUReject.root"},
        {"VZ_PUReject",            "+VZ, N_{vtx}=1",               "output/pPb_VZ_PUReject.root",             "output/PbP_VZ_PUReject.root"},
        {"VZ_Z_PUReject",          "+VZ+Z, N_{vtx}=1",             "output/pPb_VZ_Z_PUReject.root",           "output/PbP_VZ_Z_PUReject.root"},
        {"VZ_Z_ZSF_PUReject",      "+VZ+Z+ZSF, N_{vtx}=1",        "output/pPb_VZ_Z_ZSF_PUReject.root",      "output/PbP_VZ_Z_ZSF_PUReject.root"},
        {"VZ_Z_ZSF_trk_PUReject",  "+VZ+Z+ZSF+trk, N_{vtx}=1",    "output/pPb_VZ_Z_ZSF_trk_PUReject.root",  "output/PbP_VZ_Z_ZSF_trk_PUReject.root"},
        {"VZ_Z_ZSF_trk_res_PUReject", "+VZ+Z+ZSF+trk+res, N_{vtx}=1", "output/pPb_VZ_Z_ZSF_trk_res_PUReject.root", "output/PbP_VZ_Z_ZSF_trk_res_PUReject.root"},
    };

    struct HistDef {
        string name;
        string xTitle;
        string yTitle;
        bool logx;
        double xmin;
        double xmax;
    };

    vector<HistDef> hists = {
        {"hTrkPtData",  "Track p_{T} [GeV]",  "1/N_{Z} dN/dp_{T} [GeV^{-1}]", true,  0.5, 15},
        {"hTrkEtaData", "Track #eta_{CM}",     "1/N_{Z} dN/d#eta",              false, -3.5, 2.5},
        {"hZPtData",    "Z p_{T} [GeV]",       "1/N_{Z} dN/dp_{T} [GeV^{-1}]", true,  0.5, 500},
        {"hZYData",     "Z y_{CM}",            "1/N_{Z} dN/dy",                 false, -3.5, 2.5},
    };

    vector<string> shortNames = {"trkPt", "trkEta", "ZPt", "Zy"};

    const Int_t cmsGreen = cmsTeal;
    vector<Int_t> markerColors = {cmsBlue, cmsGreen};
    vector<Int_t> markerStyles = {mCircleFill, mSquareFill};
    vector<Int_t> lineColors = {cmsBlue, cmsGreen};
    vector<Int_t> lineStyles = {1, 1};

    for (int il = 0; il < (int)levels.size(); il++) {
        TFile *fPPb = TFile::Open(levels[il].pPbFile.c_str(), "READ");
        TFile *fPbP = TFile::Open(levels[il].PbPFile.c_str(), "READ");

        double nZ_pPb = ((TH1D*)fPPb->Get("hNZData"))->GetBinContent(1);
        double nZ_PbP = ((TH1D*)fPbP->Get("hNZData"))->GetBinContent(1);

        cout << levels[il].label << ": NZ pPb=" << nZ_pPb << " PbP=" << nZ_PbP << endl;

        for (int ih = 0; ih < (int)hists.size(); ih++) {
            TH1D *h_pPb = (TH1D*)fPPb->Get(hists[ih].name.c_str())->Clone(Form("h_pPb_%s_%s", levels[il].label.c_str(), shortNames[ih].c_str()));
            TH1D *h_PbP = (TH1D*)fPbP->Get(hists[ih].name.c_str())->Clone(Form("h_PbP_%s_%s", levels[il].label.c_str(), shortNames[ih].c_str()));
            h_pPb->SetDirectory(nullptr);
            h_PbP->SetDirectory(nullptr);

            // Normalize by NZ
            h_pPb->Scale(1.0 / nZ_pPb);
            h_PbP->Scale(1.0 / nZ_PbP);

            // Divide by bin width
            divideByWidth(h_pPb);
            divideByWidth(h_PbP);

            vector<TH1*> overlay = {(TH1*)h_pPb, (TH1*)h_PbP};
            vector<string> labels = {"pPb data", "Pbp data"};

            TCanvas *c = new TCanvas(Form("c_%s_%s", shortNames[ih].c_str(), levels[il].label.c_str()), "", 700, 700);
            TPad *pad = plotCMSRatio(overlay, Form("pad_%s_%s", shortNames[ih].c_str(), levels[il].label.c_str()), labels,
                lineColors, lineStyles, markerColors, markerStyles,
                hists[ih].xTitle.c_str(), hists[ih].xmin, hists[ih].xmax,
                hists[ih].yTitle.c_str(), -1, -1,
                "Pbp / pPb", 0.7, 1.3,
                0,
                hists[ih].logx, false, true, 0.58);

            pad->cd();
            h_pPb->SetLineColor(cmsBlue);
            h_pPb->SetMarkerColor(cmsBlue);
            h_pPb->SetLineStyle(1);
            h_pPb->SetLineWidth(2);
            h_pPb->Draw("E SAME");
            h_PbP->SetLineColor(cmsGreen);
            h_PbP->SetMarkerColor(cmsGreen);
            h_PbP->SetLineStyle(1);
            h_PbP->SetLineWidth(2);
            h_PbP->Draw("E SAME");

            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "8.16 TeV", levels[il].title.c_str());

            c->SaveAs(Form("plots/%s_%s.pdf", shortNames[ih].c_str(), levels[il].label.c_str()));
            delete c;
            delete h_pPb;
            delete h_PbP;
        }

        fPPb->Close();
        fPbP->Close();
    }

    return 0;
}
