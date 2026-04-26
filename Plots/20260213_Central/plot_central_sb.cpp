#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TPad.h>
#include <TSystem.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"

#include "CommandLine.h"
#include "SetStyle.h"

int main(int argc, char *argv[])
{
    SetThesisStyle();

    CommandLine CL(argc, argv);

    string inputDir   = CL.Get("InputDir", "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    string plotDir    = CL.Get("PlotDir", "plots/central_sb");
    string zPtRange   = CL.Get("zPtRange", "0_500");
    string trkPtRange = CL.Get("trkPtRange", "0.5_15");
    string tag        = CL.Get("tag", "ZV9_trkV27_nmix10");

    gSystem->mkdir(plotDir.c_str(), true);

    vector<string> systems = {"pPb", "PbP"};
    vector<string> labels = {"pPb data", "Pbp data"};

    vector<TH1 *> hDeltaEtaAll;
    vector<TH1 *> hDeltaPhiAll;
    vector<TH1 *> hDeltaEtaBkg;
    vector<TH1 *> hDeltaPhiBkg;

    for(int i = 0; i < (int)systems.size(); i++)
    {
        string path = Form("%s/%s_trkResidual_%s_ZPT%s-nosub.root",
            inputDir.c_str(), systems[i].c_str(), tag.c_str(), zPtRange.c_str());

        TFile *input = TFile::Open(path.c_str(), "READ");
        if(input == nullptr || input->IsZombie())
        {
            cerr << "Error: unable to open " << path << endl;
            return 1;
        }

        TH1D *this_hDeltaEtaAll = (TH1D *)input->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
        TH1D *this_hDeltaPhiAll = (TH1D *)input->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));
        TH2D *this_hMix2D       = (TH2D *)input->Get(Form("hMixData_%s", trkPtRange.c_str()));

        if(this_hDeltaEtaAll == nullptr || this_hDeltaPhiAll == nullptr || this_hMix2D == nullptr)
        {
            cerr << "Error: missing requested histograms in " << path << endl;
            return 1;
        }

        this_hDeltaEtaAll = (TH1D *)this_hDeltaEtaAll->Clone(Form("DeltaEta_all_%s", systems[i].c_str()));
        this_hDeltaPhiAll = (TH1D *)this_hDeltaPhiAll->Clone(Form("DeltaPhi_all_%s", systems[i].c_str()));
        TH1D *this_hDeltaEtaBkg = this_hMix2D->ProjectionX(Form("DeltaEta_bkg_%s", systems[i].c_str()), 6, 10);
        TH1D *this_hDeltaPhiBkg = this_hMix2D->ProjectionY(Form("DeltaPhi_bkg_%s", systems[i].c_str()), 0, 10);

        this_hDeltaEtaAll->SetDirectory(nullptr);
        this_hDeltaPhiAll->SetDirectory(nullptr);
        this_hDeltaEtaBkg->SetDirectory(nullptr);
        this_hDeltaPhiBkg->SetDirectory(nullptr);

        divideByWidth(this_hDeltaEtaBkg);
        divideByWidth(this_hDeltaPhiBkg);

        this_hDeltaEtaAll->Scale(0.5);
        this_hDeltaPhiAll->Scale(0.5);
        this_hDeltaEtaBkg->Scale(0.5);
        this_hDeltaPhiBkg->Scale(0.5);

        hDeltaEtaAll.push_back(this_hDeltaEtaAll);
        hDeltaPhiAll.push_back(this_hDeltaPhiAll);
        hDeltaEtaBkg.push_back(this_hDeltaEtaBkg);
        hDeltaPhiBkg.push_back(this_hDeltaPhiBkg);

        input->Close();
    }

    vector<int> markerColors = {cmsBlue, cmsTeal};
    vector<int> markerStyles = {mCircleFill, mSquareFill};
    vector<int> lineColors = {cmsBlue, cmsTeal};
    vector<int> lineStyles = {1, 1};

    string outputBase = Form("%s/pPbPbpData_ZPT%s_trkPT%s_%s",
        plotDir.c_str(), zPtRange.c_str(), trkPtRange.c_str(), tag.c_str());

    auto drawPlot = [&](const vector<TH1 *> &histograms, const string &xTitle,
        double xmin, double xmax, const string &yTitle, const string &suffix, double labelScale)
    {
        TCanvas *canvas = new TCanvas(Form("c_%s", suffix.c_str()), "", 700, 700);
        TPad *pad = (TPad *)plotCMSRatio(histograms, "",
            labels,
            lineColors, lineStyles,
            markerColors, markerStyles,
            xTitle.c_str(), xmin, xmax,
            yTitle.c_str(), -1, -1,
            "Pbp / pPb", 0.95, 1.05,
            0,
            false, false, true,
            labelScale);

        AddCMSHeader(pad, "Internal", false);
        AddUPCHeader(pad, "8.16 TeV", "pPb vs Pbp corrected data");
        canvas->SaveAs(Form("%s-%s.pdf", outputBase.c_str(), suffix.c_str()));
        delete canvas;
    };

    drawPlot(hDeltaEtaAll, "#Delta y_{ch,Z}", -4, 4,
        "Signal+bkg d#DeltaN_{ch}/d#Delta y_{ch,Z}", "DeltaEta-all", 0.60);
    drawPlot(hDeltaPhiAll, "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Signal+bkg d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", "DeltaPhi-all", 0.20);
    drawPlot(hDeltaEtaBkg, "#Delta y_{ch,Z}", -4, 4,
        "Mixed d#DeltaN_{ch}/d#Delta y_{ch,Z}", "DeltaEta-bkg", 0.60);
    drawPlot(hDeltaPhiBkg, "#Delta#phi_{ch,Z}", -1.5707, 4.7123,
        "Mixed d#DeltaN_{ch}/d#Delta#phi_{ch,Z}", "DeltaPhi-bkg", 0.20);

    return 0;
}
