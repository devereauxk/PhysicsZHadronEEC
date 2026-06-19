#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLatex.h>
#include <TSystem.h>
#include <TLine.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cmath>
using namespace std;

#include "../../CommonCode/include/KylesPlotting.h"
#include "CommandLine.h"
#include "SetStyle.h"

// ============================================================
// Helpers
// ============================================================

TH1D *LoadSystematicHistogram(const string &fileName, const string &histogramName,
    const string &cloneName)
{
    TFile input(fileName.c_str(), "READ");
    if (input.IsZombie()) return nullptr;
    TH1D *histogram = (TH1D *)input.Get(histogramName.c_str());
    if (histogram == nullptr) return nullptr;
    histogram = (TH1D *)histogram->Clone(cloneName.c_str());
    histogram->SetDirectory(nullptr);
    return histogram;
}

auto ParseRange = [](const string &range) -> pair<string, string> {
    size_t pos = range.find('_');
    if (pos == string::npos) return {range, ""};
    return {range.substr(0, pos), range.substr(pos + 1)};
};

struct ResultProjectionWindow {
    int DeltaPhiXFirst = 0;
    int DeltaPhiXLast = 0;
    int DeltaEtaYFirst = 1;
    int DeltaEtaYLast = 1;
};

int FindLastFullBinAtOrBelow(const TAxis *axis, double boundary)
{
    if (axis == nullptr)
        return 0;

    const double tolerance = std::max(1.0, std::abs(boundary)) * 1e-12;
    int lastBin = 0;
    for (int bin = 1; bin <= axis->GetNbins(); ++bin) {
        if (axis->GetBinUpEdge(bin) <= boundary + tolerance)
            lastBin = bin;
        else
            break;
    }
    return lastBin;
}

int FindFirstFullBinAbove(const TAxis *axis, double boundary)
{
    if (axis == nullptr)
        return 1;

    const double tolerance = std::max(1.0, std::abs(boundary)) * 1e-12;
    for (int bin = 1; bin <= axis->GetNbins(); ++bin) {
        if (axis->GetBinUpEdge(bin) > boundary + tolerance)
            return bin;
    }
    return axis->GetNbins();
}

ResultProjectionWindow GetProjectionWindow(const TH2D *input)
{
    ResultProjectionWindow window;
    if (input == nullptr)
        return window;

    window.DeltaPhiXFirst = 0;
    window.DeltaPhiXLast = FindLastFullBinAtOrBelow(input->GetXaxis(), 0.0);
    window.DeltaEtaYFirst = FindFirstFullBinAbove(input->GetYaxis(), 0.0);
    window.DeltaEtaYLast = FindLastFullBinAtOrBelow(input->GetYaxis(), M_PI / 2);
    return window;
}

struct BinResult {
    TH1D *hDeltaPhi_pp = nullptr;
    TH1D *hDeltaEta_pp = nullptr;
    vector<TH1D*> hDeltaPhi;  // [pp, pPb, MC(optional)]
    vector<TH1D*> hDeltaEta;
    // Convenience accessors that return vector<TH1*> for plotting functions
    vector<TH1*> getDeltaPhi() const { return vector<TH1*>(hDeltaPhi.begin(), hDeltaPhi.end()); }
    vector<TH1*> getDeltaEta() const { return vector<TH1*>(hDeltaEta.begin(), hDeltaEta.end()); }
    // systematics
    vector<TH1*> topSystPhi, topSystEta;
    vector<TH1*> diffSystPhi, diffSystEta;
    string zPt, trkPt;
};

// Keeps TFiles alive in a static vector so histograms don't go out of scope
static vector<TFile*> gOpenFiles;

BinResult LoadBin(const string &zPt, const string &trkPt,
    const string &tag_pp, const string &tag_ppb,
    bool includeMC, const string &systematicsDir,
    const string &baseDir)
{
    BinResult r;
    r.zPt = zPt;
    r.trkPt = trkPt;
    string plotsDir = baseDir;

    // --- pp ---
    string ppPath = Form("%s/pp_trkResidual_%s_ZPT%s-nosub.root", plotsDir.c_str(), tag_pp.c_str(), zPt.c_str());
    TFile* fin_pp = TFile::Open(ppPath.c_str(), "READ");
    gOpenFiles.push_back(fin_pp);

    TH2D* hData_pp = (TH2D*)fin_pp->Get(Form("hData_%s", trkPt.c_str()));
    TH2D* hMixData_pp = (TH2D*)fin_pp->Get(Form("hMixData_%s", trkPt.c_str()));
    hData_pp->Add(hMixData_pp, -1);
    ResultProjectionWindow ppWindow = GetProjectionWindow(hData_pp);

    r.hDeltaPhi_pp = (TH1D*) hData_pp->ProjectionY(Form("phi_pp_%s_%s",zPt.c_str(),trkPt.c_str()),
        ppWindow.DeltaPhiXFirst, ppWindow.DeltaPhiXLast);
    divideByWidth(r.hDeltaPhi_pp);
    r.hDeltaPhi_pp->Scale(1./2);

    r.hDeltaEta_pp = (TH1D*) hData_pp->ProjectionX(Form("eta_pp_%s_%s",zPt.c_str(),trkPt.c_str()),
        ppWindow.DeltaEtaYFirst, ppWindow.DeltaEtaYLast);
    divideByWidth(r.hDeltaEta_pp);
    r.hDeltaEta_pp->Scale(1./2);

    r.hDeltaPhi.push_back(r.hDeltaPhi_pp);
    r.hDeltaEta.push_back(r.hDeltaEta_pp);

    // --- pPb + PbP combined ---
    auto loadSystem = [&](const string &sysTag, const string &prefix, const string &tag) {
        string path = Form("%s/%s_%s_ZPT%s-nosub.root", plotsDir.c_str(), prefix.c_str(), tag.c_str(), zPt.c_str());
        TFile* f = TFile::Open(path.c_str(), "READ");
        gOpenFiles.push_back(f);
        TH2D* hD  = (TH2D*)f->Get(Form("hData_%s", trkPt.c_str()));
        TH2D* hMD = (TH2D*)f->Get(Form("hMixData_%s", trkPt.c_str()));
        TH1D* hNZ = (TH1D*)f->Get(Form("hNZData_%s", trkPt.c_str()));
        TH1D* hNZM= (TH1D*)f->Get(Form("hNZMixData_%s", trkPt.c_str()));
        hD->SetName(Form("hD_%s_%s_%s", sysTag.c_str(), zPt.c_str(), trkPt.c_str()));
        hMD->SetName(Form("hMD_%s_%s_%s", sysTag.c_str(), zPt.c_str(), trkPt.c_str()));
        hD->Scale(hNZ->GetBinContent(1));
        hMD->Scale(hNZM->GetBinContent(1));
        struct R { TH2D *D; TH2D *MD; TH1D *NZ; TH1D *NZM; };
        return R{hD, hMD, hNZ, hNZM};
    };

    auto ppb = loadSystem("ppb", "pPb_trkResidual", tag_ppb);
    auto pbp = loadSystem("pbp", "PbP_trkResidual", tag_ppb);

    TH2D* S = (TH2D*) ppb.D->Clone(Form("S_%s_%s", zPt.c_str(), trkPt.c_str()));
    S->Add(pbp.D);
    float S_NZ = ppb.NZ->GetBinContent(1) + pbp.NZ->GetBinContent(1);
    S->Scale(1. / S_NZ);

    TH2D* B = (TH2D*) ppb.MD->Clone(Form("B_%s_%s", zPt.c_str(), trkPt.c_str()));
    B->Add(pbp.MD);
    float B_NZ = ppb.NZM->GetBinContent(1) + pbp.NZM->GetBinContent(1);
    B->Scale(1. / B_NZ);

    S->Add(B, -1);
    ResultProjectionWindow combinedWindow = GetProjectionWindow(S);

    TH1D* phiComb = (TH1D*) S->ProjectionY(Form("phi_comb_%s_%s",zPt.c_str(),trkPt.c_str()),
        combinedWindow.DeltaPhiXFirst, combinedWindow.DeltaPhiXLast);
    divideByWidth(phiComb);
    phiComb->Scale(1./2);
    r.hDeltaPhi.push_back(phiComb);

    TH1D* etaComb = (TH1D*) S->ProjectionX(Form("eta_comb_%s_%s",zPt.c_str(),trkPt.c_str()),
        combinedWindow.DeltaEtaYFirst, combinedWindow.DeltaEtaYLast);
    divideByWidth(etaComb);
    etaComb->Scale(1./2);
    r.hDeltaEta.push_back(etaComb);

    // --- MC (Powheg+EPOS) ---
    if (includeMC) {
        auto mc_ppb = loadSystem("mcppb", "pPbMC_Gen_nominal", tag_ppb);
        auto mc_pbp = loadSystem("mcpbp", "PbPMC_Gen_nominal", tag_ppb);
        TH2D* SM = (TH2D*) mc_ppb.D->Clone(Form("SM_%s_%s", zPt.c_str(), trkPt.c_str()));
        SM->Add(mc_pbp.D);
        float SM_NZ = mc_ppb.NZ->GetBinContent(1) + mc_pbp.NZ->GetBinContent(1);
        SM->Scale(1. / SM_NZ);
        TH2D* BM = (TH2D*) mc_ppb.MD->Clone(Form("BM_%s_%s", zPt.c_str(), trkPt.c_str()));
        BM->Add(mc_pbp.MD);
        float BM_NZ = mc_ppb.NZM->GetBinContent(1) + mc_pbp.NZM->GetBinContent(1);
        BM->Scale(1. / BM_NZ);
        SM->Add(BM, -1);
        ResultProjectionWindow mcWindow = GetProjectionWindow(SM);

        TH1D* phiMC = (TH1D*) SM->ProjectionY(Form("phi_mc_%s_%s",zPt.c_str(),trkPt.c_str()),
            mcWindow.DeltaPhiXFirst, mcWindow.DeltaPhiXLast);
        divideByWidth(phiMC);
        phiMC->Scale(1./2);
        r.hDeltaPhi.push_back(phiMC);

        TH1D* etaMC = (TH1D*) SM->ProjectionX(Form("eta_mc_%s_%s",zPt.c_str(),trkPt.c_str()),
            mcWindow.DeltaEtaYFirst, mcWindow.DeltaEtaYLast);
        divideByWidth(etaMC);
        etaMC->Scale(1./2);
        r.hDeltaEta.push_back(etaMC);
    }

    // --- Systematics ---
    string currentSys = "pPbPbp";
    string ppSystFile = Form("%s/pp_%s_ZPT%s_trkPT%s-systematics.root",
        systematicsDir.c_str(), tag_pp.c_str(), zPt.c_str(), trkPt.c_str());
    string combSystFile = Form("%s/%s_%s_ZPT%s_trkPT%s-systematics.root",
        systematicsDir.c_str(), currentSys.c_str(), tag_ppb.c_str(), zPt.c_str(), trkPt.c_str());

    string uid = Form("%s_%s", zPt.c_str(), trkPt.c_str());
    r.topSystPhi = {
        LoadSystematicHistogram(ppSystFile, "Total_DeltaPhi", Form("ppSysPhi_%s",uid.c_str())),
        LoadSystematicHistogram(combSystFile, "Total_DeltaPhi", Form("combSysPhi_%s",uid.c_str()))
    };
    r.topSystEta = {
        LoadSystematicHistogram(ppSystFile, "Total_DeltaEta", Form("ppSysEta_%s",uid.c_str())),
        LoadSystematicHistogram(combSystFile, "Total_DeltaEta", Form("combSysEta_%s",uid.c_str()))
    };
    r.diffSystPhi = {
        nullptr,
        LoadSystematicHistogram(combSystFile, "DifferenceTotal_DeltaPhi", Form("diffSysPhi_%s",uid.c_str()))
    };
    r.diffSystEta = {
        nullptr,
        LoadSystematicHistogram(combSystFile, "DifferenceTotal_DeltaEta", Form("diffSysEta_%s",uid.c_str()))
    };
    if (includeMC) {
        r.topSystPhi.push_back(nullptr);
        r.topSystEta.push_back(nullptr);
        r.diffSystPhi.push_back(nullptr);
        r.diffSystEta.push_back(nullptr);
    }

    return r;
}

// ============================================================
// Draw custom header: "CMS" bold top-left, system label top-right
// ============================================================
void DrawPaperHeader(TPad* pad, float textSizePx = 22)
{
    pad->cd();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(43);
    latex.SetTextSize(textSizePx);

    float l = pad->GetLeftMargin();
    float r = pad->GetRightMargin();
    float t = pad->GetTopMargin();
    float b = pad->GetBottomMargin();

    // "CMS" bold, top-left inside frame
    latex.SetTextAlign(13);
    latex.SetTextFont(63); // bold Helvetica, pixel
    latex.SetTextSize(textSizePx * 1.25);
    latex.DrawLatex(l + (1-l-r)*0.05, 1-(t+(1-t-b)*0.05), "CMS");

    // System label top-right
    latex.SetTextFont(43);
    latex.SetTextSize(textSizePx * 0.9);
    latex.SetTextAlign(33);
    latex.DrawLatex(1-r-(1-l-r)*0.02, 1-(t+(1-t-b)*0.05),
        "pPb (pp) 8.16 TeV 174 nb^{-1} (301 pb^{-1})");
}

// ============================================================
// main
// ============================================================
int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);

    string tag_pp  = CL.Get("pptag", "EEV5_ZV9_trkV27_nmix10");
    string tag_ppb = CL.Get("pPbtag", "ZV9_trkV27_nmix10");
    bool includeMC = CL.GetBool("includeMC", true);
    string outputBase = CL.Get("outputBase", "plots/paper");
    string systematicsDir = CL.Get("systematicsDir",
        "/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output");
    string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");

    gSystem->mkdir(Form("%s/%s", outputBase.c_str(), tag_pp.c_str()), true);
    string outPrefix = Form("%s/%s/paper", outputBase.c_str(), tag_pp.c_str());

    // Style vectors (same as plot_central_combined.cpp)
    vector<int> markerColors = {cmsBlue, cmsRed, kSpring-8, kMagenta-3, cmsYellow, cmsGray};
    vector<int> markerStyles = {mSquareFill, mCircleFill, 0, mCircleFill, mCircleFill};
    vector<int> lineColors = {cmsBlue, cmsRed, kSpring-8, kMagenta-3, cmsTealL1, cmsRed, cmsRed};
    vector<int> lineStyles = {0, 0, 1};
    vector<string> labels = {"pp", "pPb"};
    if (includeMC) labels.push_back("Powheg+EPOS");

    // ============================
    // Load all bins
    // ============================
    BinResult inc = LoadBin("0_500", "0.5_15", tag_pp, tag_ppb, includeMC, systematicsDir, baseDir);

    struct ScanCell { string zPt; string trkPt; };
    vector<ScanCell> scanCells = {
        {"0_30","0.5_2"}, {"0_30","2_4"}, {"0_30","4_15"},
        {"30_500","0.5_2"}, {"30_500","2_4"}, {"30_500","4_15"}
    };
    vector<BinResult> scan(6);
    for (int i = 0; i < 6; i++)
        scan[i] = LoadBin(scanCells[i].zPt, scanCells[i].trkPt, tag_pp, tag_ppb, includeMC, systematicsDir, baseDir);

    // ============================
    // PDF 1: DeltaPhi inclusive
    // ============================
    {
        TCanvas *c = new TCanvas("cPhi", "cPhi", 600, 600);
        TPad *p = PlotCMSPaperDiffResult(
            inc.getDeltaPhi(), inc.topSystPhi, inc.diffSystPhi,
            "incPhi", labels, lineColors, lineStyles,
            markerColors, markerStyles,
            "#Delta#varphi_{ch,Z}", -1.5707, 4.7123,
            "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}", -1, -1,
            "pPb - pp", -1, -1,
            0, false, false, true, 0.2, 1.5, true);

        DrawPaperHeader(p, 22);

        p->cd();
        TLatex lx;
        lx.SetNDC();
        lx.SetTextFont(43);
        lx.SetTextSize(16);
        lx.SetTextAlign(11);
        auto zR = ParseRange("0_500");
        auto tR = ParseRange("0.5_15");
        lx.DrawLatex(0.21, 0.60, Form("%s < p_{T}^{Z} < %s", zR.first.c_str(), zR.second.c_str()));
        lx.DrawLatex(0.21, 0.53, Form("%s < p_{T}^{ch} < %s", tR.first.c_str(), tR.second.c_str()));
        lx.DrawLatex(0.21, 0.46, "|y^{Z}_{CM}| < 1.935");

        c->Update();
        c->SaveAs(Form("%s-DeltaPhi-inclusive.pdf", outPrefix.c_str()));
        delete c;
    }

    // ============================
    // PDF 2: DeltaEta inclusive
    // ============================
    {
        TCanvas *c = new TCanvas("cEta", "cEta", 600, 600);
        TPad *p = PlotCMSPaperDiffResult(
            inc.getDeltaEta(), inc.topSystEta, inc.diffSystEta,
            "incEta", labels, lineColors, lineStyles,
            markerColors, markerStyles,
            "#Delta y_{ch,Z}", -4, 4,
            "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}", -1, -1,
            "pPb - pp", -1, -1,
            0, false, false, true, 0.2, 1.5, true);

        DrawPaperHeader(p, 22);

        p->cd();
        TLatex lx;
        lx.SetNDC();
        lx.SetTextFont(43);
        lx.SetTextSize(16);
        lx.SetTextAlign(31);
        auto zR = ParseRange("0_500");
        auto tR = ParseRange("0.5_15");
        lx.DrawLatex(0.8, 0.82, Form("%s < p_{T}^{Z} < %s", zR.first.c_str(), zR.second.c_str()));
        lx.DrawLatex(0.8, 0.75, Form("%s < p_{T}^{ch} < %s", tR.first.c_str(), tR.second.c_str()));
        lx.DrawLatex(0.8, 0.68, "|y^{Z}_{CM}| < 1.935, |#Delta#varphi_{ch,Z}| < #pi/2");

        c->Update();
        c->SaveAs(Form("%s-DeltaEta-inclusive.pdf", outPrefix.c_str()));
        delete c;
    }

    // ============================
    // PDF 3: DeltaPhi 2x3 scan
    // ============================
    auto DrawScanGrid = [&](const string &observable, // "Phi" or "Eta"
                            const string &xTitle,
                            double xmin, double xmax,
                            const string &yTitle,
                            const string &rTitle,
                            const string &suffix)
    {
        int cW = 1800, cH = 1200;
        TCanvas *c = new TCanvas(Form("cScan%s",suffix.c_str()), "", cW, cH);

        double gapX = 0.005;
        double totalColWidth = (1.0 - 2*gapX) / 3.0;

        double h0 = 0.32;  // top main
        double h1 = 0.15;  // top diff
        double h2 = 0.30;  // bot main
        double h3 = 0.23;  // bot diff (extra for x labels)

        double y3bot = 0.0;
        double y3top = h3;
        double y2bot = y3top;
        double y2top = y2bot + h2;
        double y1bot = y2top;
        double y1top = y1bot + h1;
        double y0bot = y1top;
        // double y0top = 1.0; // unused, use 1.0 directly

        int titlePx = 28;
        int labelPx = 22;
        int legPx   = 18;
        int kinPx   = 18;

        for (int col = 0; col < 3; col++) {
            double xlo = col * (totalColWidth + gapX);
            double xhi = xlo + totalColWidth;

            double lMargin = (col == 0) ? 0.20 : 0.08;
            double rMargin = 0.02;

            for (int row = 0; row < 2; row++) {
                int idx = row * 3 + col;
                BinResult &br = scan[idx];

                vector<TH1*> hists = (observable == "Phi") ?
                    br.getDeltaPhi() : br.getDeltaEta();
                vector<TH1*> &topSyst = (observable == "Phi") ? br.topSystPhi : br.topSystEta;
                vector<TH1*> &diffSyst = (observable == "Phi") ? br.diffSystPhi : br.diffSystEta;

                double mainYbot, mainYtop, diffYbot, diffYtop;
                if (row == 0) {
                    mainYbot = y0bot; mainYtop = 1.0;
                    diffYbot = y1bot; diffYtop = y1top;
                } else {
                    mainYbot = y2bot; mainYtop = y2top;
                    diffYbot = y3bot; diffYtop = y3top;
                }

                // --- Main pad ---
                string mainName = Form("main_%d_%d", row, col);
                c->cd();
                TPad *mainPad = new TPad(mainName.c_str(), "", xlo, mainYbot, xhi, mainYtop);
                mainPad->SetLeftMargin(lMargin);
                mainPad->SetRightMargin(rMargin);
                mainPad->SetTopMargin(row == 0 ? 0.08 : 0.02);
                mainPad->SetBottomMargin(0.0);
                mainPad->Draw();
                mainPad->cd();

                // Auto-range y
                double gmin=1e30, gmax=-1e30;
                for (size_t ih=0; ih<hists.size(); ih++) {
                    TH1* h = hists[ih];
                    TH1* s = (ih<topSyst.size()) ? topSyst[ih] : nullptr;
                    for (int b=1; b<=h->GetNbinsX(); b++) {
                        double v = h->GetBinContent(b);
                        double e = h->GetBinError(b);
                        double se = s ? fabs(s->GetBinContent(b)) : 0;
                        gmin = min(gmin, v - max(e,se));
                        gmax = max(gmax, v + max(e,se));
                    }
                }
                double ymarg = 0.2*(gmax-gmin);

                TLegend *leg = new TLegend(0.55, 0.65, 0.95, 0.90);
                leg->SetBorderSize(0);
                leg->SetFillStyle(0);
                leg->SetTextFont(43);
                leg->SetTextSize(legPx);

                for (int i = 0; i < (int)hists.size(); i++) {
                    TH1* hist = hists[i];

                    if (lineStyles[i] == -1) hist->SetLineColorAlpha(0,0);
                    else hist->SetLineColor(lineColors[i]);
                    if (lineStyles[i] == 0) hist->SetLineStyle(1);
                    else if (lineStyles[i] > 0) hist->SetLineStyle(lineStyles[i]);
                    hist->SetMarkerColor(markerColors[i]);
                    hist->SetMarkerStyle(markerStyles[i]);
                    hist->SetStats(0);
                    if (lineStyles[i] == 0) hist->SetLineWidth(3);
                    else if (lineStyles[i] > 0) hist->SetLineWidth(2);

                    hist->GetXaxis()->SetTitle("");
                    hist->GetXaxis()->SetLabelSize(0);
                    hist->GetXaxis()->SetRangeUser(xmin, xmax);
                    hist->GetYaxis()->SetTitle((col == 0) ? yTitle.c_str() : "");
                    hist->GetYaxis()->SetTitleFont(43);
                    hist->GetYaxis()->SetTitleSize(titlePx);
                    hist->GetYaxis()->SetLabelFont(43);
                    hist->GetYaxis()->SetLabelSize((col == 0) ? labelPx : 0);
                    hist->GetYaxis()->SetTitleOffset(2.0);
                    hist->GetYaxis()->SetRangeUser(gmin-ymarg, gmax+ymarg);

                    if (lineStyles[i] == 0) hist->Draw(i==0 ? "" : "SAME");
                    else if (lineStyles[i] == -1) hist->Draw(i==0 ? "P" : "P SAME");
                    else hist->Draw(i==0 ? "HIST" : "HIST SAME");

                    TH1* topS = (i < (int)topSyst.size()) ? topSyst[i] : nullptr;
                    if (topS) {
                        TGraphAsymmErrors *band = BuildSystematicBand(hist, topS,
                            Form("tband_%d_%d_%d",row,col,i), lineColors[i]);
                        if (band) band->Draw("2 SAME");
                        if (lineStyles[i] == 0) hist->Draw("SAME");
                        else if (lineStyles[i] == -1) hist->Draw("P SAME");
                        else hist->Draw("HIST SAME");
                    }
                    hist->Draw("E1 SAME");

                    if (lineStyles[i] == 0) leg->AddEntry(hist, labels[i].c_str(), "pl");
                    else if (lineStyles[i] == -1) leg->AddEntry(hist, labels[i].c_str(), "p");
                    else leg->AddEntry(hist, labels[i].c_str(), "l");
                }

                // Kinematic labels
                auto zR = ParseRange(br.zPt);
                auto tR = ParseRange(br.trkPt);
                TLatex lx;
                lx.SetNDC();
                lx.SetTextFont(43);
                lx.SetTextSize(kinPx);
                lx.SetTextAlign(13);
                float kinX = lMargin + 0.05;
                float kinY = 0.87;
                lx.DrawLatex(kinX, kinY, Form("%s < p_{T}^{Z} < %s", zR.first.c_str(), zR.second.c_str()));
                lx.DrawLatex(kinX, kinY - 0.08, Form("%s < p_{T}^{ch} < %s", tR.first.c_str(), tR.second.c_str()));
                if (observable == "Eta")
                    lx.DrawLatex(kinX, kinY - 0.16, "|#Delta#varphi_{ch,Z}| < #pi/2");

                leg->Draw("SAME");

                // Header only on row 0, col 0
                if (row == 0 && col == 0) {
                    TLatex hdr;
                    hdr.SetNDC();
                    hdr.SetTextFont(63);
                    hdr.SetTextSize(26);
                    hdr.SetTextAlign(13);
                    hdr.DrawLatex(lMargin + 0.05, 0.97, "CMS");
                }
                if (row == 0 && col == 2) {
                    TLatex hdr;
                    hdr.SetNDC();
                    hdr.SetTextFont(43);
                    hdr.SetTextSize(20);
                    hdr.SetTextAlign(33);
                    hdr.DrawLatex(1.0-rMargin-0.02, 0.97,
                        "pPb (pp) 8.16 TeV 174 nb^{-1} (301 pb^{-1})");
                }

                // --- Diff pad ---
                string diffName = Form("diff_%d_%d", row, col);
                c->cd();
                TPad *diffPad = new TPad(diffName.c_str(), "", xlo, diffYbot, xhi, diffYtop);
                diffPad->SetLeftMargin(lMargin);
                diffPad->SetRightMargin(rMargin);
                diffPad->SetTopMargin(0.0);
                bool isBottomRow = (row == 1);
                diffPad->SetBottomMargin(isBottomRow ? 0.30 : 0.0);
                diffPad->Draw();
                diffPad->cd();

                // Compute diff range
                double dmin=1e30, dmax=-1e30;
                for (int i=1; i<(int)hists.size(); i++) {
                    TH1* dh = (TH1*)hists[i]->Clone(Form("dr_%d_%d_%d",row,col,i));
                    dh->Add(hists[0], -1);
                    TH1* ds = (i<(int)diffSyst.size()) ? diffSyst[i] : nullptr;
                    for (int b=1; b<=dh->GetNbinsX(); b++) {
                        double v=dh->GetBinContent(b);
                        double e=dh->GetBinError(b);
                        double se = ds ? fabs(ds->GetBinContent(b)) : 0;
                        updateDiffRange(v, max(e,se), dmin, dmax);
                    }
                    delete dh;
                }
                double dm = 0.15*(dmax-dmin);
                if (dm<=0) dm = max(fabs(dmax),1.0)*0.15;

                bool first = true;
                for (int i=1; i<(int)hists.size(); i++) {
                    if (lineStyles[i] > 0) continue; // skip MC from diff panel
                    TH1* hDiff = (TH1*)hists[i]->Clone(Form("hd_%d_%d_%d",row,col,i));
                    hDiff->Add(hists[0], -1);
                    TH1* bSyst = (i<(int)diffSyst.size()) ? diffSyst[i] : nullptr;

                    if (isBottomRow) {
                        hDiff->GetXaxis()->SetTitle(xTitle.c_str());
                        hDiff->GetXaxis()->SetTitleFont(43);
                        hDiff->GetXaxis()->SetTitleSize(titlePx);
                        hDiff->GetXaxis()->SetLabelFont(43);
                        hDiff->GetXaxis()->SetLabelSize(labelPx);
                        hDiff->GetXaxis()->SetTitleOffset(1.2);
                    } else {
                        hDiff->GetXaxis()->SetTitle("");
                        hDiff->GetXaxis()->SetLabelSize(0);
                    }
                    hDiff->GetYaxis()->SetTitle((col == 0) ? rTitle.c_str() : "");
                    hDiff->GetYaxis()->SetRangeUser(dmin-dm, dmax+dm);
                    hDiff->GetYaxis()->SetTitleFont(43);
                    hDiff->GetYaxis()->SetTitleSize(titlePx);
                    hDiff->GetYaxis()->SetLabelFont(43);
                    hDiff->GetYaxis()->SetLabelSize(labelPx);
                    hDiff->GetYaxis()->SetTitleOffset(2.0);
                    hDiff->GetYaxis()->SetNdivisions(505);
                    hDiff->SetLineColor(lineColors[i]);
                    hDiff->SetMarkerColor(markerColors[i]);
                    hDiff->SetMarkerStyle(markerStyles[i]);
                    hDiff->SetLineWidth(2);

                    TString opt = first ? "E1" : "E1 SAME";
                    hDiff->Draw(opt);

                    if (bSyst) {
                        TGraphAsymmErrors *band = BuildSystematicBand(hDiff, bSyst,
                            Form("dband_%d_%d_%d",row,col,i), lineColors[i]);
                        if (band) band->Draw("2 SAME");
                    }
                    hDiff->Draw("E1 SAME");

                    double xlo_line = hDiff->GetXaxis()->GetBinLowEdge(hDiff->GetXaxis()->GetFirst());
                    double xhi_line = hDiff->GetXaxis()->GetBinUpEdge(hDiff->GetXaxis()->GetLast());
                    TLine *line = new TLine(xlo_line, 0, xhi_line, 0);
                    line->SetLineColor(kGray+2);
                    line->SetLineStyle(2);
                    line->Draw("SAME");
                    first = false;
                }
            } // row
        } // col

        c->Update();
        c->SaveAs(Form("%s-%s.pdf", outPrefix.c_str(), suffix.c_str()));
        delete c;
    };

    DrawScanGrid("Phi",
        "#Delta#varphi_{ch,Z}", -1.5707, 4.7123,
        "d#LT#DeltaN_{ch}#GT/d#Delta#varphi_{ch,Z}",
        "pPb - pp",
        "DeltaPhi-scan");

    DrawScanGrid("Eta",
        "#Delta y_{ch,Z}", -4, 4,
        "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}",
        "pPb - pp",
        "DeltaEta-scan");

    cout << "Paper plots written to: " << outPrefix << "-*.pdf" << endl;
    return 0;
}
