#include <iostream>
#include <string>
#include <vector>
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"
#include "CommandLine.h"

using namespace std;

void Symmetrize1DEta(TH1D *h) {
    int n = h->GetNbinsX();
    for (int i = 0; i < n / 2; ++i) {
        int iL = i + 1, iR = n - i;
        double avg = 0.5 * (h->GetBinContent(iL) + h->GetBinContent(iR));
        double err = 0.5 * sqrt(pow(h->GetBinError(iL), 2) + pow(h->GetBinError(iR), 2));
        h->SetBinContent(iL, avg); h->SetBinContent(iR, avg);
        h->SetBinError(iL, err);   h->SetBinError(iR, err);
    }
}

void Symmetrize1DPhi(TH1D *h) {
    int n = h->GetNbinsX();
    int half = n / 2;
    for (int j = 0; j < half / 2; ++j) {
        int jNear = j + 1, mNear = half - j;
        double avg = 0.5 * (h->GetBinContent(jNear) + h->GetBinContent(mNear));
        double err = 0.5 * sqrt(pow(h->GetBinError(jNear), 2) + pow(h->GetBinError(mNear), 2));
        h->SetBinContent(jNear, avg); h->SetBinContent(mNear, avg);
        h->SetBinError(jNear, err);   h->SetBinError(mNear, err);

        int jAway = half + j + 1;
        int mAway = n + half + 1 - jAway;
        avg = 0.5 * (h->GetBinContent(jAway) + h->GetBinContent(mAway));
        err = 0.5 * sqrt(pow(h->GetBinError(jAway), 2) + pow(h->GetBinError(mAway), 2));
        h->SetBinContent(jAway, avg); h->SetBinContent(mAway, avg);
        h->SetBinError(jAway, err);   h->SetBinError(mAway, err);
    }
}

int main(int argc, char *argv[]) {
    CommandLine CL(argc, argv);
    vector<string> files = CL.GetStringVector("files");
    vector<string> labels = CL.GetStringVector("labels");
    string output = CL.Get("output", "plots/jewel_scan_panel.pdf");
    string zptRange = CL.Get("zPtRange", "30_500");
    bool doEta = CL.GetBool("doEta", true);
    string obs = doEta ? "DeltaEta" : "DeltaPhi";

    const int NPANEL = 3;
    string trkPtBins[NPANEL] = {"0.5_2", "2_4", "4_15"};
    string trkPtLabels[NPANEL] = {"0.5 < p_{T}^{trk} < 2 GeV", "2 < p_{T}^{trk} < 4 GeV", "4 < p_{T}^{trk} < 15 GeV"};
    int nFiles = files.size();

    if (nFiles < 2 || files.size() != labels.size()) {
        cerr << "Need >=2 files with matching labels" << endl;
        return 1;
    }

    int colors[] = {kBlack, kRed+1, kBlue+1, kGreen+2, kMagenta+1};
    int markers[] = {20, 24, 21, 25, 22};

    gStyle->SetOptStat(0);

    double topFrac = 0.65;
    double botFrac = 1.0 - topFrac;
    double panelW = 1.0 / NPANEL;

    TCanvas c("c", "", 1500, 600);

    TPad *pTop[NPANEL], *pBot[NPANEL];
    for (int p = 0; p < NPANEL; ++p) {
        double xL = p * panelW, xR = (p + 1) * panelW;
        pTop[p] = new TPad(Form("pTop%d", p), "", xL, botFrac, xR, 1.0);
        pBot[p] = new TPad(Form("pBot%d", p), "", xL, 0, xR, botFrac);

        pTop[p]->SetBottomMargin(0.02);
        pTop[p]->SetTopMargin(0.08);
        pTop[p]->SetLeftMargin(p == 0 ? 0.16 : 0.05);
        pTop[p]->SetRightMargin(p == NPANEL - 1 ? 0.04 : 0.02);

        pBot[p]->SetTopMargin(0.02);
        pBot[p]->SetBottomMargin(0.35);
        pBot[p]->SetLeftMargin(p == 0 ? 0.16 : 0.05);
        pBot[p]->SetRightMargin(p == NPANEL - 1 ? 0.04 : 0.02);

        pTop[p]->Draw();
        pBot[p]->Draw();
    }

    for (int p = 0; p < NPANEL; ++p) {
        string trkPt = trkPtBins[p];
        string histName = Form("%s_Result%s", obs.c_str(), trkPt.c_str());

        vector<TH1D*> histos;
        for (int i = 0; i < nFiles; ++i) {
            TFile *f = TFile::Open(Form("%s-result.root", files[i].c_str()));
            if (!f || f->IsZombie()) { cerr << "Cannot open " << files[i] << endl; return 1; }
            TH1D *h = (TH1D*)f->Get(histName.c_str());
            if (!h) { cerr << "Missing " << histName << " in " << files[i] << endl; return 1; }
            h = (TH1D*)h->Clone(Form("h_%d_%d", p, i));
            h->SetDirectory(nullptr);
            h->Scale(1./2);
            if (h->GetNbinsX() == 12) { doEta ? Symmetrize1DEta(h) : Symmetrize1DPhi(h); }
            cout << "Panel " << p << " " << labels[i] << " integral: " << h->Integral() << endl;
            histos.push_back(h);
            f->Close();
        }

        // style
        for (int i = 0; i < nFiles; ++i) {
            histos[i]->SetLineColor(colors[i % 5]);
            histos[i]->SetLineWidth(2);
            histos[i]->SetMarkerColor(colors[i % 5]);
            histos[i]->SetMarkerStyle(markers[i % 5]);
            histos[i]->SetMarkerSize(0.7);
        }

        // top panel
        pTop[p]->cd();
        double ymin = 1e30, ymax = -1e30;
        for (auto *h : histos) {
            for (int b = 1; b <= h->GetNbinsX(); ++b) {
                double v = h->GetBinContent(b);
                ymin = min(ymin, v);
                ymax = max(ymax, v);
            }
        }
        double margin = 0.15 * (ymax - ymin);

        histos[0]->GetYaxis()->SetRangeUser(ymin - margin, ymax + 2.8 * margin);
        histos[0]->GetXaxis()->SetLabelSize(0);
        histos[0]->GetXaxis()->SetTickLength(0.03);
        if (p == 0) {
            histos[0]->GetYaxis()->SetTitle(doEta ? "#frac{1}{2} #frac{1}{N_{Z}} #frac{dN_{ch}}{d#Deltay}"
                                                  : "#frac{1}{2} #frac{1}{N_{Z}} #frac{dN_{ch}}{d#Delta#phi}");
            histos[0]->GetYaxis()->SetTitleSize(0.06);
            histos[0]->GetYaxis()->SetTitleOffset(1.1);
            histos[0]->GetYaxis()->SetLabelSize(0.05);
        } else {
            histos[0]->GetYaxis()->SetLabelSize(0);
            histos[0]->GetYaxis()->SetTickLength(0.01);
        }
        histos[0]->Draw("EP");
        for (int i = 1; i < nFiles; ++i) histos[i]->Draw("EP SAME");

        TLatex tex;
        tex.SetNDC();
        tex.SetTextSize(0.055);
        tex.DrawLatex(p == 0 ? 0.20 : 0.08, 0.88, trkPtLabels[p].c_str());
        if (p == 0) {
            tex.SetTextSize(0.05);
            string zptMin = zptRange.substr(0, zptRange.find('_'));
            string zptMax = zptRange.substr(zptRange.find('_') + 1);
            string zptLabel = zptMin + " < p_{T}^{Z} < " + zptMax + " GeV";
            tex.DrawLatex(0.20, 0.80, zptLabel.c_str());
        }

        // legend only on first panel
        if (p == 0) {
            TLegend *leg = new TLegend(0.20, 0.72 - 0.065 * nFiles, 0.85, 0.72);
            leg->SetBorderSize(0);
            leg->SetFillStyle(0);
            leg->SetTextSize(0.045);
            for (int i = 0; i < nFiles; ++i)
                leg->AddEntry(histos[i], labels[i].c_str(), "lep");
            leg->Draw();
        }

        // ratio panel
        pBot[p]->cd();
        for (int i = 1; i < nFiles; ++i) {
            TH1D *hRatio = (TH1D*)histos[i]->Clone(Form("hRatio_%d_%d", p, i));
            hRatio->Divide(histos[0]);
            hRatio->SetLineColor(colors[i % 5]);
            hRatio->SetMarkerColor(colors[i % 5]);
            hRatio->SetMarkerStyle(markers[i % 5]);
            hRatio->SetMarkerSize(0.6);
            if (i == 1) {
                hRatio->GetYaxis()->SetRangeUser(0.0, 2.5);
                hRatio->GetXaxis()->SetTitle(doEta ? "#Deltay_{ch,Z}" : "#Delta#phi_{ch,Z}");
                hRatio->GetXaxis()->SetTitleSize(0.12);
                hRatio->GetXaxis()->SetTitleOffset(1.0);
                hRatio->GetXaxis()->SetLabelSize(0.10);
                if (p == 0) {
                    hRatio->GetYaxis()->SetTitle(Form("ratio to %s", labels[0].c_str()));
                    hRatio->GetYaxis()->SetTitleSize(0.10);
                    hRatio->GetYaxis()->SetTitleOffset(0.55);
                    hRatio->GetYaxis()->SetLabelSize(0.09);
                } else {
                    hRatio->GetYaxis()->SetLabelSize(0);
                    hRatio->GetYaxis()->SetTickLength(0.01);
                }
                hRatio->GetYaxis()->SetNdivisions(505);
                hRatio->Draw("EP");
            } else {
                hRatio->Draw("EP SAME");
            }
        }

        TLine *line = new TLine(histos[0]->GetXaxis()->GetXmin(), 1, histos[0]->GetXaxis()->GetXmax(), 1);
        line->SetLineStyle(2);
        line->Draw();
    }

    c.SaveAs(output.c_str());
    cout << "Saved: " << output << endl;
    return 0;
}
