#include <TFile.h>
#include <TH1D.h>
#include <TH3D.h>
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

    string pPbRawFile  = "output/pPb_raw.root";
    string PbPRawFile  = "output/PbP_raw.root";
    string pPbZcorrFile = "output/pPb_Zcorr_raw.root";

    TFile *fPPbRaw  = TFile::Open(pPbRawFile.c_str(), "READ");
    TFile *fPbPRaw  = TFile::Open(PbPRawFile.c_str(), "READ");
    TFile *fPPbZcorr = TFile::Open(pPbZcorrFile.c_str(), "READ");

    if (!fPPbRaw || !fPbPRaw || !fPPbZcorr) {
        cerr << "Error opening input files" << endl;
        return 1;
    }

    double nZ_pPbRaw  = ((TH1D*)fPPbRaw->Get("hNZData"))->GetBinContent(1);
    double nZ_PbPRaw  = ((TH1D*)fPbPRaw->Get("hNZData"))->GetBinContent(1);
    double nZ_pPbZcorr = ((TH1D*)fPPbZcorr->Get("hNZData"))->GetBinContent(1);

    cout << "NZ pPb raw: " << nZ_pPbRaw << endl;
    cout << "NZ PbP raw: " << nZ_PbPRaw << endl;
    cout << "NZ pPb Zcorr: " << nZ_pPbZcorr << endl;

    TH3D *h3_pPbRaw  = (TH3D*)fPPbRaw->Get("hZ3D")->Clone("h3_pPbRaw");
    TH3D *h3_PbPRaw  = (TH3D*)fPbPRaw->Get("hZ3D")->Clone("h3_PbPRaw");
    TH3D *h3_pPbZcorr = (TH3D*)fPPbZcorr->Get("hZ3D")->Clone("h3_pPbZcorr");
    h3_pPbRaw->SetDirectory(nullptr);
    h3_PbPRaw->SetDirectory(nullptr);
    h3_pPbZcorr->SetDirectory(nullptr);

    struct AxisDef {
        string shortName;
        string xTitle;
        bool logx;
        int axis; // 1=X(pT), 2=Y(y_CM), 3=Z(phi)
    };

    vector<AxisDef> axes = {
        {"ZPt",  "Z p_{T} [GeV]", true,  1},
        {"Zy",   "Z y_{CM}",      false, 2},
        {"ZPhi", "Z #phi",        false, 3},
    };

    const Int_t cmsGreen = cmsTeal;
    vector<Int_t> markerColors = {cmsBlue, cmsGreen, cmsRed};
    vector<Int_t> markerStyles = {mCircleFill, mSquareFill, mDiamondFill};
    vector<Int_t> lineColors = {cmsBlue, cmsGreen, cmsRed};
    vector<Int_t> lineStyles = {1, 1, 1};

    for (auto &ax : axes) {
        TH1D *h_pPbRaw = nullptr, *h_PbPRaw = nullptr, *h_pPbZcorr = nullptr;
        if (ax.axis == 1) {
            h_pPbRaw   = h3_pPbRaw->ProjectionX(Form("proj_pPbRaw_%s", ax.shortName.c_str()));
            h_PbPRaw   = h3_PbPRaw->ProjectionX(Form("proj_PbPRaw_%s", ax.shortName.c_str()));
            h_pPbZcorr = h3_pPbZcorr->ProjectionX(Form("proj_pPbZcorr_%s", ax.shortName.c_str()));
        } else if (ax.axis == 2) {
            h_pPbRaw   = h3_pPbRaw->ProjectionY(Form("proj_pPbRaw_%s", ax.shortName.c_str()));
            h_PbPRaw   = h3_PbPRaw->ProjectionY(Form("proj_PbPRaw_%s", ax.shortName.c_str()));
            h_pPbZcorr = h3_pPbZcorr->ProjectionY(Form("proj_pPbZcorr_%s", ax.shortName.c_str()));
        } else {
            h_pPbRaw   = h3_pPbRaw->ProjectionZ(Form("proj_pPbRaw_%s", ax.shortName.c_str()));
            h_PbPRaw   = h3_PbPRaw->ProjectionZ(Form("proj_PbPRaw_%s", ax.shortName.c_str()));
            h_pPbZcorr = h3_pPbZcorr->ProjectionZ(Form("proj_pPbZcorr_%s", ax.shortName.c_str()));
        }
        h_pPbRaw->SetDirectory(nullptr);
        h_PbPRaw->SetDirectory(nullptr);
        h_pPbZcorr->SetDirectory(nullptr);

        // Normalize by NZ and bin width
        h_pPbRaw->Scale(1.0 / nZ_pPbRaw);
        h_PbPRaw->Scale(1.0 / nZ_PbPRaw);
        h_pPbZcorr->Scale(1.0 / nZ_pPbZcorr);
        divideByWidth(h_pPbRaw);
        divideByWidth(h_PbPRaw);
        divideByWidth(h_pPbZcorr);

        double xmin = h_pPbRaw->GetXaxis()->GetXmin();
        double xmax = h_pPbRaw->GetXaxis()->GetXmax();

        vector<TH1*> overlay = {(TH1*)h_pPbRaw, (TH1*)h_PbPRaw, (TH1*)h_pPbZcorr};
        vector<string> labels = {"pPb", "Pbp", "pPb (Z-corrected)"};

        TCanvas *c = new TCanvas(Form("c_closure_%s", ax.shortName.c_str()), "", 700, 700);
        TPad *pad = plotCMSRatio(overlay, Form("pad_closure_%s", ax.shortName.c_str()), labels,
            lineColors, lineStyles, markerColors, markerStyles,
            ax.xTitle.c_str(), xmin, xmax,
            Form("1/N_{Z} dN/d%s", ax.xTitle.c_str()), -1, -1,
            "/ Pbp", 0.8, 1.2,
            1,  // baseline = PbP (index 1)
            ax.logx, false, true, 0.50);

        pad->cd();
        for (int i = 0; i < 3; i++) {
            overlay[i]->SetLineColor(lineColors[i]);
            overlay[i]->SetMarkerColor(markerColors[i]);
            overlay[i]->SetMarkerStyle(markerStyles[i]);
            overlay[i]->SetLineStyle(1);
            overlay[i]->SetLineWidth(2);
            overlay[i]->Draw("E SAME");
        }
        AddCMSHeader(pad, "Internal", false);
        AddUPCHeader(pad, "8.16 TeV", "Z correction closure");

        c->SaveAs(Form("plots/zcorr_closure_%s.pdf", ax.shortName.c_str()));
        delete c;
        delete h_pPbRaw;
        delete h_PbPRaw;
        delete h_pPbZcorr;
    }

    fPPbRaw->Close();
    fPbPRaw->Close();
    fPPbZcorr->Close();

    cout << "Z correction closure plots saved to plots/" << endl;
    return 0;
}
