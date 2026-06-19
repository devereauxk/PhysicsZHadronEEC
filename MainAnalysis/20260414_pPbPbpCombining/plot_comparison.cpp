#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TSystem.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "KylesPlotting.h"
#include "CommandLine.h"
#include "SetStyle.h"

int main(int argc, char *argv[]) {
    SetThesisStyle();

    CommandLine CL(argc, argv);
    string inputDir = CL.Get("InputDir", "output");
    string plotDir = CL.Get("PlotDir", "plots");
    string pPbPrefix = CL.Get("PPbPrefix", "pPb");
    string PbPPrefix = CL.Get("PbPPrefix", "PbP");
    string pPbLabel = CL.Get("PPbLabel", "pPb data");
    string PbPLabel = CL.Get("PbPLabel", "Pbp data");
    gSystem->mkdir(plotDir.c_str(), true);

    auto joinPath = [](const string &dir, const string &file) {
        if (dir.empty() || dir == ".")
            return file;
        if (dir.back() == '/')
            return dir + file;
        return dir + "/" + file;
    };
    auto fileExists = [](const string &path) {
        return gSystem->AccessPathName(path.c_str()) == false;
    };

    struct CorrLevel {
        string label;
        string title;
        string pPbFile;
        string PbPFile;
    };

    vector<CorrLevel> levels = {
        {"raw",                    "No corrections",               joinPath(inputDir, pPbPrefix + "_raw.root"),                    joinPath(inputDir, PbPPrefix + "_raw.root")},
        {"VZ",                     "+VZ",                          joinPath(inputDir, pPbPrefix + "_VZ.root"),                     joinPath(inputDir, PbPPrefix + "_VZ.root")},
        {"VZ_Z",                   "+VZ+Z",                        joinPath(inputDir, pPbPrefix + "_VZ_Z.root"),                   joinPath(inputDir, PbPPrefix + "_VZ_Z.root")},
        {"VZ_Z_ZSF",               "+VZ+Z+ZSF",                    joinPath(inputDir, pPbPrefix + "_VZ_Z_ZSF.root"),               joinPath(inputDir, PbPPrefix + "_VZ_Z_ZSF.root")},
        {"VZ_Z_ZSF_trk",           "+VZ+Z+ZSF+trk",                joinPath(inputDir, pPbPrefix + "_VZ_Z_ZSF_trk.root"),           joinPath(inputDir, PbPPrefix + "_VZ_Z_ZSF_trk.root")},
        {"VZ_Z_ZSF_trk_res",       "+VZ+Z+ZSF+trk+res",            joinPath(inputDir, pPbPrefix + "_VZ_Z_ZSF_trk_res.root"),       joinPath(inputDir, PbPPrefix + "_VZ_Z_ZSF_trk_res.root")},
        {"raw_PUReject",           "No corrections, N_{vtx}=1",    joinPath(inputDir, pPbPrefix + "_raw_PUReject.root"),            joinPath(inputDir, PbPPrefix + "_raw_PUReject.root")},
        {"VZ_PUReject",            "+VZ, N_{vtx}=1",               joinPath(inputDir, pPbPrefix + "_VZ_PUReject.root"),             joinPath(inputDir, PbPPrefix + "_VZ_PUReject.root")},
        {"VZ_Z_PUReject",          "+VZ+Z, N_{vtx}=1",             joinPath(inputDir, pPbPrefix + "_VZ_Z_PUReject.root"),           joinPath(inputDir, PbPPrefix + "_VZ_Z_PUReject.root")},
        {"VZ_Z_ZSF_PUReject",      "+VZ+Z+ZSF, N_{vtx}=1",         joinPath(inputDir, pPbPrefix + "_VZ_Z_ZSF_PUReject.root"),      joinPath(inputDir, PbPPrefix + "_VZ_Z_ZSF_PUReject.root")},
        {"VZ_Z_ZSF_trk_PUReject",  "+VZ+Z+ZSF+trk, N_{vtx}=1",     joinPath(inputDir, pPbPrefix + "_VZ_Z_ZSF_trk_PUReject.root"),  joinPath(inputDir, PbPPrefix + "_VZ_Z_ZSF_trk_PUReject.root")},
        {"VZ_Z_ZSF_trk_res_PUReject", "+VZ+Z+ZSF+trk+res, N_{vtx}=1", joinPath(inputDir, pPbPrefix + "_VZ_Z_ZSF_trk_res_PUReject.root"), joinPath(inputDir, PbPPrefix + "_VZ_Z_ZSF_trk_res_PUReject.root")},
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
        {"hMultData",   "N_{trk}",             "Events / N_{Z}",                false, 0, 150},
        {"hDEtaData",   "|#Delta#eta|",        "1/N_{Z} dN/d|#Delta#eta|",      false, 0, 4.8},
        {"hDPhiData",   "|#Delta#phi|",        "1/N_{Z} dN/d|#Delta#phi|",      false, 0, M_PI},
    };

    vector<string> shortNames = {"trkPt", "trkEta", "ZPt", "Zy", "Mult", "DEta", "DPhi"};

    const Int_t cmsGreen = cmsTeal;
    vector<Int_t> markerColors = {cmsBlue, cmsGreen};
    vector<Int_t> markerStyles = {mCircleFill, mSquareFill};
    vector<Int_t> lineColors = {cmsBlue, cmsGreen};
    vector<Int_t> lineStyles = {1, 1};

    for (int il = 0; il < (int)levels.size(); il++) {
        if(!fileExists(levels[il].pPbFile) || !fileExists(levels[il].PbPFile)) {
            cout << "Skipping level " << levels[il].label << " because an input file is missing." << endl;
            continue;
        }
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
            vector<string> labels = {pPbLabel, PbPLabel};

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

            c->SaveAs(joinPath(plotDir, Form("%s_%s.pdf", shortNames[ih].c_str(), levels[il].label.c_str())).c_str());
            delete c;
            delete h_pPb;
            delete h_PbP;
        }

        fPPb->Close();
        fPbP->Close();
    }

    struct Hist2DDef {
        string name;
        string outputStem;
        string xTitle;
        double xmin;
        double xmax;
        string yTitle;
        double ymin;
        double ymax;
        string zTitle;
        double zmin;
        double zmax;
    };

    vector<Hist2DDef> heatmaps = {
        {"hDEtaDPhiData", "DEtaDPhi_ratio", "|#Delta#eta|", 0, 4.8, "|#Delta#phi|", 0, M_PI, "Pbp / pPb", 0.85, 1.15},
        {"hZEtaPhiData",  "ZEtaPhi_ratio",  "Z #eta",       -2.4, 2.4, "Z #phi",       0, 2 * M_PI, "Pbp / pPb", 0.85, 1.15},
    };

    // 2D ratio heatmap plots (PbP/pPb)
    for (int il = 0; il < (int)levels.size(); il++) {
        if(!fileExists(levels[il].pPbFile) || !fileExists(levels[il].PbPFile)) {
            cout << "Skipping heatmap level " << levels[il].label << " because an input file is missing." << endl;
            continue;
        }
        TFile *fPPb = TFile::Open(levels[il].pPbFile.c_str(), "READ");
        TFile *fPbP = TFile::Open(levels[il].PbPFile.c_str(), "READ");

        double nZ_pPb = ((TH1D*)fPPb->Get("hNZData"))->GetBinContent(1);
        double nZ_PbP = ((TH1D*)fPbP->Get("hNZData"))->GetBinContent(1);

        for(const Hist2DDef &heatmap : heatmaps) {
            TH2D *h2_pPb = (TH2D*)fPPb->Get(heatmap.name.c_str())->Clone(Form("h2_pPb_%s_%s", heatmap.outputStem.c_str(), levels[il].label.c_str()));
            TH2D *h2_PbP = (TH2D*)fPbP->Get(heatmap.name.c_str())->Clone(Form("h2_PbP_%s_%s", heatmap.outputStem.c_str(), levels[il].label.c_str()));
            h2_pPb->SetDirectory(nullptr);
            h2_PbP->SetDirectory(nullptr);

            h2_pPb->Scale(1.0 / nZ_pPb);
            h2_PbP->Scale(1.0 / nZ_PbP);

            TH2D *h2_ratio = (TH2D*)h2_PbP->Clone(Form("h2_ratio_%s_%s", heatmap.outputStem.c_str(), levels[il].label.c_str()));
            h2_ratio->Divide(h2_pPb);

            TCanvas *c = new TCanvas(Form("c2d_ratio_%s_%s", heatmap.outputStem.c_str(), levels[il].label.c_str()), "", 700, 600);
            TPad* pad = plotCMSSimple2D(c, h2_ratio, "",
                heatmap.xTitle.c_str(), heatmap.xmin, heatmap.xmax,
                heatmap.yTitle.c_str(), heatmap.ymin, heatmap.ymax,
                heatmap.zTitle.c_str(), heatmap.zmin, heatmap.zmax,
                false, false, false);
            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "8.16 TeV", Form("Pbp/pPb %s", levels[il].title.c_str()));
            c->SaveAs(joinPath(plotDir, Form("%s_%s.pdf", heatmap.outputStem.c_str(), levels[il].label.c_str())).c_str());
            delete c;
            delete h2_pPb;
            delete h2_PbP;
            delete h2_ratio;
        }
        fPPb->Close();
        fPbP->Close();
    }

    return 0;
}
