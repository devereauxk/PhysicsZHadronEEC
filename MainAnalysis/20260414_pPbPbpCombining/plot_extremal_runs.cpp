#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TPad.h>
#include <TStyle.h>
#include <TSystem.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "KylesPlotting.h"
#include "CommandLine.h"
#include "SetStyle.h"

string joinPath(const string &dir, const string &file)
{
    if(dir.empty() || dir == ".")
        return file;
    if(dir.back() == '/')
        return dir + file;
    return dir + "/" + file;
}

int main(int argc, char *argv[])
{
    SetThesisStyle();

    CommandLine CL(argc, argv);

    string plotDir = CL.Get("PlotDir", "plots/extremal_runs");
    gSystem->mkdir(plotDir.c_str(), true);

    struct RunInput
    {
        string label;
        string title;
        string inputDir;
        string prefix;
    };

    vector<RunInput> runs = {
        {"pPb max |#LT|#Delta#eta|#GT| (Run 286327)", "Run 286327", CL.Get("PPbMaxDir", "output/pPb_run286327"), "pPb"},
        {"pPb min |#LT|#Delta#eta|#GT| (Run 286441)", "Run 286441", CL.Get("PPbMinDir", "output/pPb_run286441"), "pPb"},
        {"Pbp max |#LT|#Delta#eta|#GT| (Run 285718)", "Run 285718", CL.Get("PbPMaxDir", "output/PbP_run285718"), "PbP"},
        {"Pbp min |#LT|#Delta#eta|#GT| (Run 285537)", "Run 285537", CL.Get("PbPMinDir", "output/PbP_run285537"), "PbP"},
    };

    struct CorrLevel
    {
        string label;
        string title;
        string suffix;
    };

    vector<CorrLevel> levels = {
        {"raw", "No corrections", "raw"},
        {"VZ", "+VZ", "VZ"},
        {"VZ_Z", "+VZ+Z", "VZ_Z"},
        {"VZ_Z_ZSF", "+VZ+Z+ZSF", "VZ_Z_ZSF"},
        {"VZ_Z_ZSF_trk", "+VZ+Z+ZSF+trk", "VZ_Z_ZSF_trk"},
        {"VZ_Z_ZSF_trk_res", "+VZ+Z+ZSF+trk+res", "VZ_Z_ZSF_trk_res"},
        {"raw_PUReject", "No corrections, N_{vtx}=1", "raw_PUReject"},
        {"VZ_PUReject", "+VZ, N_{vtx}=1", "VZ_PUReject"},
        {"VZ_Z_PUReject", "+VZ+Z, N_{vtx}=1", "VZ_Z_PUReject"},
        {"VZ_Z_ZSF_PUReject", "+VZ+Z+ZSF, N_{vtx}=1", "VZ_Z_ZSF_PUReject"},
        {"VZ_Z_ZSF_trk_PUReject", "+VZ+Z+ZSF+trk, N_{vtx}=1", "VZ_Z_ZSF_trk_PUReject"},
        {"VZ_Z_ZSF_trk_res_PUReject", "+VZ+Z+ZSF+trk+res, N_{vtx}=1", "VZ_Z_ZSF_trk_res_PUReject"},
    };

    struct HistDef
    {
        string name;
        string shortName;
        string xTitle;
        string yTitle;
        bool logx;
        double xmin;
        double xmax;
        double labelScale;
    };

    vector<HistDef> hists = {
        {"hTrkPtData", "trkPt", "Track p_{T} [GeV]", "1/N_{Z} dN/dp_{T} [GeV^{-1}]", true, 0.5, 15, 0.58},
        {"hTrkEtaData", "trkEta", "Track #eta_{CM}", "1/N_{Z} dN/d#eta", false, -3.5, 2.5, 0.58},
        {"hZPtData", "ZPt", "Z p_{T} [GeV]", "1/N_{Z} dN/dp_{T} [GeV^{-1}]", true, 0.5, 500, 0.58},
        {"hZYData", "Zy", "Z y_{CM}", "1/N_{Z} dN/dy", false, -3.5, 2.5, 0.58},
        {"hMultData", "Mult", "N_{trk}", "Events / N_{Z}", false, 0, 150, 0.58},
        {"hDEtaData", "DEta", "|#Delta#eta|", "1/N_{Z} dN/d|#Delta#eta|", false, 0, 4.8, 0.58},
        {"hDPhiData", "DPhi", "|#Delta#phi|", "1/N_{Z} dN/d|#Delta#phi|", false, 0, M_PI, 0.20},
    };

    vector<Int_t> markerColors = {cmsBlue, cmsTeal, cmsRed, kOrange + 7};
    vector<Int_t> markerStyles = {mCircleFill, mSquareFill, mDiamondFill, 29};
    vector<Int_t> lineColors = {cmsBlue, cmsTeal, cmsRed, kOrange + 7};
    vector<Int_t> lineStyles = {1, 1, 1, 1};

    for(const CorrLevel &level : levels)
    {
        vector<TFile *> files(runs.size(), nullptr);
        vector<double> nZ(runs.size(), 0);

        for(int ir = 0; ir < (int)runs.size(); ir++)
        {
            string fileName = joinPath(runs[ir].inputDir, Form("%s_%s.root", runs[ir].prefix.c_str(), level.suffix.c_str()));
            files[ir] = TFile::Open(fileName.c_str(), "READ");
            if(files[ir] == nullptr || files[ir]->IsZombie())
            {
                cerr << "Error opening " << fileName << endl;
                return 1;
            }
            TH1D *hNZ = (TH1D *)files[ir]->Get("hNZData");
            if(hNZ == nullptr)
            {
                cerr << "Error: missing hNZData in " << fileName << endl;
                return 1;
            }
            nZ[ir] = hNZ->GetBinContent(1);
        }

        for(const HistDef &hist : hists)
        {
            vector<TH1 *> overlay;
            for(int ir = 0; ir < (int)runs.size(); ir++)
            {
                TH1D *h = (TH1D *)files[ir]->Get(hist.name.c_str());
                if(h == nullptr)
                {
                    cerr << "Error: missing " << hist.name << " in " << files[ir]->GetName() << endl;
                    return 1;
                }

                TH1D *clone = (TH1D *)h->Clone(Form("%s_%s_%d", hist.shortName.c_str(), level.suffix.c_str(), ir));
                clone->SetDirectory(nullptr);
                clone->Scale(1.0 / nZ[ir]);
                divideByWidth(clone);
                overlay.push_back(clone);
            }

            TCanvas *canvas = new TCanvas(Form("c_%s_%s", hist.shortName.c_str(), level.suffix.c_str()), "", 700, 700);
            TPad *pad = (TPad *)plotCMSRatio(overlay, "",
                {runs[0].label, runs[1].label, runs[2].label, runs[3].label},
                lineColors, lineStyles,
                markerColors, markerStyles,
                hist.xTitle.c_str(), hist.xmin, hist.xmax,
                hist.yTitle.c_str(), -1, -1,
                "Ratio to pPb max", 0.7, 1.3,
                0,
                hist.logx, false, true,
                hist.labelScale);

            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "8.16 TeV", Form("Extremal runs %s", level.title.c_str()));
            canvas->SaveAs(joinPath(plotDir, Form("%s_%s.pdf", hist.shortName.c_str(), level.suffix.c_str())).c_str());
            delete canvas;

            for(TH1 *h : overlay)
                delete h;
        }

        TCanvas *c2d = new TCanvas(Form("c2d_%s", level.suffix.c_str()), "", 1100, 900);
        c2d->Divide(2, 2);
        gStyle->SetOptStat(0);

        for(int ir = 0; ir < (int)runs.size(); ir++)
        {
            TH2D *h2 = (TH2D *)files[ir]->Get("hDEtaDPhiData");
            if(h2 == nullptr)
            {
                cerr << "Error: missing hDEtaDPhiData in " << files[ir]->GetName() << endl;
                return 1;
            }

            TH2D *clone = (TH2D *)h2->Clone(Form("DEtaDPhi_%s_%d", level.suffix.c_str(), ir));
            clone->SetDirectory(nullptr);
            clone->Scale(1.0 / nZ[ir]);

            c2d->cd(ir + 1);
            gPad->SetRightMargin(0.16);
            gPad->SetLeftMargin(0.12);
            gPad->SetBottomMargin(0.12);
            clone->SetTitle(Form("%s, %s", runs[ir].title.c_str(), level.title.c_str()));
            clone->GetZaxis()->SetTitle("1/N_{Z} yield");
            clone->Draw("COLZ");

            TLatex latex;
            latex.SetNDC();
            latex.SetTextSize(0.04);
            latex.DrawLatex(0.14, 0.93, runs[ir].label.c_str());
            delete clone;
        }

        c2d->SaveAs(joinPath(plotDir, Form("DEtaDPhi_maps_%s.pdf", level.suffix.c_str())).c_str());
        delete c2d;

        for(TFile *file : files)
            file->Close();
    }

    return 0;
}
