#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TSystem.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"
#include "SetStyle.h"

struct SystemResult {
    TH1D *hDeltaEta;
    TH1D *hDeltaPhi;
    double nZ;
};

SystemResult loadResult(const string &nosubFile, const string &trkPtRange, const string &suffix) {
    TFile *f = TFile::Open(nosubFile.c_str(), "READ");
    if (!f || f->IsZombie()) {
        cerr << "Cannot open " << nosubFile << endl;
        return {nullptr, nullptr, 0};
    }

    TH1D *hNZ = (TH1D*)f->Get(Form("hNZData_%s", trkPtRange.c_str()));
    double nZ = hNZ->GetBinContent(1);

    TH1D *deta = (TH1D*)f->Get(Form("DeltaEta_Result%s", trkPtRange.c_str()));
    TH1D *dphi = (TH1D*)f->Get(Form("DeltaPhi_Result%s", trkPtRange.c_str()));

    TH1D *deta_c = (TH1D*)deta->Clone(Form("deta_%s", suffix.c_str()));
    deta_c->SetDirectory(nullptr);
    TH1D *dphi_c = (TH1D*)dphi->Clone(Form("dphi_%s", suffix.c_str()));
    dphi_c->SetDirectory(nullptr);

    return {deta_c, dphi_c, nZ};
}

int main(int argc, char *argv[]) {
    SetThesisStyle();

    string basePath = "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots";
    string trkPt = "0.5_15";
    string ppTag = "EEV5_ZV8_trkV26_nmix10";
    string paTag = "ZV8_trkV26_nmix10";
    string zPt = "0_500";

    gSystem->mkdir("plots/ExtraZW_study", true);

    struct System {
        string name;
        string nomFile;
        string muvarFile;
    };

    vector<System> systems = {
        {"pp",
         Form("%s/pp_trkResidual_%s_ZPT%s-nosub.root", basePath.c_str(), ppTag.c_str(), zPt.c_str()),
         Form("%s/pp_trkResidual_%s_MuVar0_ZPT%s-nosub.root", basePath.c_str(), ppTag.c_str(), zPt.c_str())},
        {"pPb",
         Form("%s/pPb_trkResidual_%s_ZPT%s-nosub.root", basePath.c_str(), paTag.c_str(), zPt.c_str()),
         Form("%s/pPb_trkResidual_%s_MuVar0_ZPT%s-nosub.root", basePath.c_str(), paTag.c_str(), zPt.c_str())},
        {"PbP",
         Form("%s/PbP_trkResidual_%s_ZPT%s-nosub.root", basePath.c_str(), paTag.c_str(), zPt.c_str()),
         Form("%s/PbP_trkResidual_%s_MuVar0_ZPT%s-nosub.root", basePath.c_str(), paTag.c_str(), zPt.c_str())}
    };

    const Int_t cmsGreen = cmsTeal;
    vector<Int_t> lineColors = {cmsBlue, cmsRed};
    vector<Int_t> lineStyles = {0, 0};
    vector<Int_t> markerColors = {cmsBlue, cmsRed};
    vector<Int_t> markerStyles = {mCircleFill, mSquareFill};

    // Per-system overlays
    for (auto &sys : systems) {
        SystemResult nom  = loadResult(sys.nomFile, trkPt, sys.name + "_nom");
        SystemResult muv0 = loadResult(sys.muvarFile, trkPt, sys.name + "_muv0");

        if (!nom.hDeltaEta || !muv0.hDeltaEta) continue;

        cout << sys.name << " Nominal NZ=" << nom.nZ << " MuVar0 NZ=" << muv0.nZ
             << " ratio=" << muv0.nZ / nom.nZ << endl;

        // Delta Eta
        {
            vector<TH1*> overlay = {(TH1*)nom.hDeltaEta, (TH1*)muv0.hDeltaEta};
            vector<string> labels = {"Nominal", "+Z TnP SF"};
            TCanvas *c = new TCanvas(Form("c_deta_%s", sys.name.c_str()), "", 700, 700);
            TPad *pad = plotCMSDiff(overlay, Form("pad_deta_%s", sys.name.c_str()), labels,
                lineColors, lineStyles, markerColors, markerStyles,
                "#Delta#eta", -4, 4,
                "1/N_{Z} dN/d#Delta#eta", -1, -1,
                "SF/Nom", 0.95, 1.05);
            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, Form("%s 8.16 TeV", sys.name.c_str()), Form("Z p_{T} 0-500, trk p_{T} %s", trkPt.c_str()));
            c->SaveAs(Form("plots/ExtraZW_study/ExtraZW_deta_%s.pdf", sys.name.c_str()));
            delete c;
        }

        // Delta Phi
        {
            vector<TH1*> overlay = {(TH1*)nom.hDeltaPhi, (TH1*)muv0.hDeltaPhi};
            vector<string> labels = {"Nominal", "+Z TnP SF"};
            TCanvas *c = new TCanvas(Form("c_dphi_%s", sys.name.c_str()), "", 700, 700);
            TPad *pad = plotCMSDiff(overlay, Form("pad_dphi_%s", sys.name.c_str()), labels,
                lineColors, lineStyles, markerColors, markerStyles,
                "#Delta#phi", -M_PI/2, 3*M_PI/2,
                "1/N_{Z} dN/d#Delta#phi", -1, -1,
                "SF/Nom", 0.95, 1.05);
            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, Form("%s 8.16 TeV", sys.name.c_str()), Form("Z p_{T} 0-500, trk p_{T} %s", trkPt.c_str()));
            c->SaveAs(Form("plots/ExtraZW_study/ExtraZW_dphi_%s.pdf", sys.name.c_str()));
            delete c;
        }
    }

    // Combined pPb+PbP vs pp (with ExtraZWeight=0)
    {
        SystemResult pp_muv0  = loadResult(systems[0].muvarFile, trkPt, "pp_combined");
        SystemResult ppb_nom  = loadResult(systems[1].nomFile, trkPt, "ppb_comb_nom");
        SystemResult ppb_muv0 = loadResult(systems[1].muvarFile, trkPt, "ppb_comb_muv0");
        SystemResult pbp_nom  = loadResult(systems[2].nomFile, trkPt, "pbp_comb_nom");
        SystemResult pbp_muv0 = loadResult(systems[2].muvarFile, trkPt, "pbp_comb_muv0");

        // Combine pPb+PbP: undo NZ normalization, add, renormalize
        // Nominal combined
        TH1D *comb_deta_nom = (TH1D*)ppb_nom.hDeltaEta->Clone("comb_deta_nom");
        comb_deta_nom->Scale(ppb_nom.nZ);
        TH1D *pbp_deta_scaled = (TH1D*)pbp_nom.hDeltaEta->Clone("pbp_deta_scaled");
        pbp_deta_scaled->Scale(pbp_nom.nZ);
        comb_deta_nom->Add(pbp_deta_scaled);
        comb_deta_nom->Scale(1.0 / (ppb_nom.nZ + pbp_nom.nZ));

        TH1D *comb_dphi_nom = (TH1D*)ppb_nom.hDeltaPhi->Clone("comb_dphi_nom");
        comb_dphi_nom->Scale(ppb_nom.nZ);
        TH1D *pbp_dphi_scaled = (TH1D*)pbp_nom.hDeltaPhi->Clone("pbp_dphi_scaled");
        pbp_dphi_scaled->Scale(pbp_nom.nZ);
        comb_dphi_nom->Add(pbp_dphi_scaled);
        comb_dphi_nom->Scale(1.0 / (ppb_nom.nZ + pbp_nom.nZ));

        // MuVar0 combined
        TH1D *comb_deta_muv = (TH1D*)ppb_muv0.hDeltaEta->Clone("comb_deta_muv");
        comb_deta_muv->Scale(ppb_muv0.nZ);
        TH1D *pbp_deta_muv_scaled = (TH1D*)pbp_muv0.hDeltaEta->Clone("pbp_deta_muv_sc");
        pbp_deta_muv_scaled->Scale(pbp_muv0.nZ);
        comb_deta_muv->Add(pbp_deta_muv_scaled);
        comb_deta_muv->Scale(1.0 / (ppb_muv0.nZ + pbp_muv0.nZ));

        TH1D *comb_dphi_muv = (TH1D*)ppb_muv0.hDeltaPhi->Clone("comb_dphi_muv");
        comb_dphi_muv->Scale(ppb_muv0.nZ);
        TH1D *pbp_dphi_muv_scaled = (TH1D*)pbp_muv0.hDeltaPhi->Clone("pbp_dphi_muv_sc");
        pbp_dphi_muv_scaled->Scale(pbp_muv0.nZ);
        comb_dphi_muv->Add(pbp_dphi_muv_scaled);
        comb_dphi_muv->Scale(1.0 / (ppb_muv0.nZ + pbp_muv0.nZ));

        // Final result: pp + combined pPb (both with ExtraZWeight applied)
        vector<Int_t> resultLineColors = {cmsBlue, cmsRed};
        vector<Int_t> resultLineStyles = {0, 0};
        vector<Int_t> resultMarkerColors = {cmsBlue, cmsRed};
        vector<Int_t> resultMarkerStyles = {mCircleFill, mSquareFill};

        // DeltaEta: nominal combined
        {
            vector<TH1*> overlay = {(TH1*)comb_deta_nom, (TH1*)comb_deta_muv};
            vector<string> labels = {"pPb+Pbp Nominal", "pPb+Pbp +Z TnP SF"};
            TCanvas *c = new TCanvas("c_comb_deta_comp", "", 700, 700);
            TPad *pad = plotCMSDiff(overlay, "pad_comb_deta_comp", labels,
                resultLineColors, resultLineStyles, resultMarkerColors, resultMarkerStyles,
                "#Delta#eta", -4, 4,
                "1/N_{Z} dN/d#Delta#eta", -1, -1,
                "SF/Nom", 0.95, 1.05);
            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "pPb+Pbp 8.16 TeV", Form("Z p_{T} 0-500, trk p_{T} %s", trkPt.c_str()));
            c->SaveAs("plots/ExtraZW_study/ExtraZW_deta_combined.pdf");
            delete c;
        }

        // DeltaPhi: nominal combined
        {
            vector<TH1*> overlay = {(TH1*)comb_dphi_nom, (TH1*)comb_dphi_muv};
            vector<string> labels = {"pPb+Pbp Nominal", "pPb+Pbp +Z TnP SF"};
            TCanvas *c = new TCanvas("c_comb_dphi_comp", "", 700, 700);
            TPad *pad = plotCMSDiff(overlay, "pad_comb_dphi_comp", labels,
                resultLineColors, resultLineStyles, resultMarkerColors, resultMarkerStyles,
                "#Delta#phi", -M_PI/2, 3*M_PI/2,
                "1/N_{Z} dN/d#Delta#phi", -1, -1,
                "SF/Nom", 0.95, 1.05);
            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "pPb+Pbp 8.16 TeV", Form("Z p_{T} 0-500, trk p_{T} %s", trkPt.c_str()));
            c->SaveAs("plots/ExtraZW_study/ExtraZW_dphi_combined.pdf");
            delete c;
        }

        // Final result overlays: pp + combined pPb with ExtraZWeight
        // DeltaEta
        {
            vector<TH1*> overlay = {(TH1*)pp_muv0.hDeltaEta, (TH1*)comb_deta_muv};
            vector<string> labels = {"pp +Z TnP SF", "pPb+Pbp +Z TnP SF"};
            TCanvas *c = new TCanvas("c_final_deta", "", 700, 700);
            TPad *pad = plotCMSDiff(overlay, "pad_final_deta", labels,
                resultLineColors, resultLineStyles, resultMarkerColors, resultMarkerStyles,
                "#Delta#eta", -4, 4,
                "1/N_{Z} dN/d#Delta#eta", -1, -1,
                "pPb/pp", 0.5, 1.5);
            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "8.16 TeV", Form("Z p_{T} 0-500, trk p_{T} %s +Z TnP SF", trkPt.c_str()));
            c->SaveAs("plots/ExtraZW_study/ExtraZW_deta_final_result.pdf");
            delete c;
        }

        // DeltaPhi
        {
            vector<TH1*> overlay = {(TH1*)pp_muv0.hDeltaPhi, (TH1*)comb_dphi_muv};
            vector<string> labels = {"pp +Z TnP SF", "pPb+Pbp +Z TnP SF"};
            TCanvas *c = new TCanvas("c_final_dphi", "", 700, 700);
            TPad *pad = plotCMSDiff(overlay, "pad_final_dphi", labels,
                resultLineColors, resultLineStyles, resultMarkerColors, resultMarkerStyles,
                "#Delta#phi", -M_PI/2, 3*M_PI/2,
                "1/N_{Z} dN/d#Delta#phi", -1, -1,
                "pPb/pp", 0.5, 1.5);
            pad->cd();
            AddCMSHeader(pad, "Internal", false);
            AddUPCHeader(pad, "8.16 TeV", Form("Z p_{T} 0-500, trk p_{T} %s +Z TnP SF", trkPt.c_str()));
            c->SaveAs("plots/ExtraZW_study/ExtraZW_dphi_final_result.pdf");
            delete c;
        }
    }

    cout << "ExtraZWeight effect study plots saved to plots/ExtraZW_study/" << endl;
    return 0;
}
