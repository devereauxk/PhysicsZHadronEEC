#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

const int rcolors[4] = {kRed-4, kOrange+1, kSpring-8, kTeal-2};
const int ccolors[4] = {kBlack,  kGreen, kViolet, kOrange+1};


void overlay_basic_pp(const char* pt_select) {

    const int ncontours = 4;
    const char *pp_names[ncontours] = {"pp", "pythia", "jewelPP", "hybridPP"};

    TH1D* hTrkPt[ncontours];
    
    // Load histograms for pp
    for (int i = 0; i < ncontours; i++) {
        TFile *file = new TFile(Form("plots/%s-result.root", pp_names[i]), "READ");
        hTrkPt[i] = (TH1D*)file->Get(Form("hTrkPtData_%s", pt_select));

        // Set stats off
        hTrkPt[i]->SetStats(0);
    }

    // Create a canvas to draw the histograms for pp
    TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 800);

    TPad *pad1 = new TPad("pad1_1", "pad1", 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetLogy();
    pad1->Draw();
    TPad *pad2 = new TPad("pad2_1", "pad2", 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    
    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    for (int i = 0; i < ncontours; i++) {
        pad1->cd();
        hTrkPt[i]->SetTitle("Track pT");
        hTrkPt[i]->GetXaxis()->SetTitle("pT (GeV/c)");
        hTrkPt[i]->GetYaxis()->SetTitle("Entries / N_Z");
        hTrkPt[i]->SetLineColor(ccolors[i]);
        hTrkPt[i]->Draw("HIST SAME");
        leg->AddEntry(hTrkPt[i], pp_names[i], "l");

        pad2->cd();
        if (i != 1) { // Use pythia (index 1) as the baseline
            TH1D* hRatio = (TH1D*)hTrkPt[i]->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hTrkPt[1]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle("pT (GeV/c)");
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle("ratio wrt pythia-gen");
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(0, 2);
            hRatio->SetLineColor(ccolors[i]);
            hRatio->Draw("HIST SAME");

            TLine *line = new TLine(hRatio->GetXaxis()->GetXmin(), 1, hRatio->GetXaxis()->GetXmax(), 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("same");

    // Optionally: Save the canvas as an image
    c1->SaveAs(Form("trkpt_result_pp_%s.png", pt_select));

}


void MakePlotTrkPt() {

    const char* pt_select[3] = {"1_2", "2_4", "4_10"};
    for (int i = 0; i<3; i++){
        overlay_basic_pp(pt_select[i]);
    }

}
