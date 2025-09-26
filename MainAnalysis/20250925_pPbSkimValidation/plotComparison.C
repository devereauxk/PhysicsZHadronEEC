#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

const int ccolors[14] = {
    kBlue+2,     // deep blue
    kAzure+7,    // cyan
    kTeal+3,     // teal
    kGreen+2,    // green
    kSpring+7,   // turquoise-green
    kOrange+7,   // orange (avoiding yellow)
    kRed,        // red
    kPink+7,     // pink
    kMagenta+2,  // magenta
    kViolet+7,   // purple
    kGray+2,     // gray
    kBlack,      // black
    kOrange+2,   // extra orange
    kCyan+2      // extra cyan
};

const int scolors[4] = {
    kAzure+7,    // cyan
    kOrange+7,   // orange (avoiding yellow)
    kTeal+3,     // teal
    kPink+7,     // pink
};

void divideByWidth(TH1D* input) {
    if (!input) {
        std::cerr << "Error: Null histogram pointer passed to divideByWidth function." << std::endl;
        return;
    }

    int nBins = input->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double binContent = input->GetBinContent(i);
        double binError = input->GetBinError(i);
        double binWidth = input->GetBinWidth(i);

        if (binWidth != 0) {
            input->SetBinContent(i, binContent / binWidth);
            input->SetBinError(i, binError / binWidth);
        } else {
            std::cerr << "Warning: Bin width is zero for bin " << i << ". Skipping division for this bin." << std::endl;
        }
    }
}

void setErrors(TH1D* hRatio, TH1D* num, TH1D* denom){
    for (int bin = 1; bin <= hRatio->GetNbinsX(); bin++) {
        double content1 = num->GetBinContent(bin);
        double error1 = num->GetBinError(bin);
        double content2 = denom->GetBinContent(bin);
        double error2 = denom->GetBinError(bin);

        double ratioError = 0;
        if (content2 > 0) {
            ratioError = sqrt((error1 * error1) / (content2 * content2) +
                              (content1 * content1 * error2 * error2) / (content2 * content2 * content2 * content2));
        }
        hRatio->SetBinError(bin, ratioError);
    }
}

void plotRatioLogy(vector<TH1D*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logy = true) {

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    TPad *pad1 = new TPad(title, title, 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1D* hist = hists[i];

        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetRangeUser(ymin, ymax);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->SetLineColor(ccolors[i]);

        if (i == baseline) {
            hist->Draw("E SAME");
        } else {
            hist->Draw("HIST SAME");
        }
        leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1D* hRatio = (TH1D*)hist->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hists[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(rTitle);
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(rmin, rmax);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            setErrors(hRatio, hist, hists.at(baseline));

            cout<<title<<" "<<labels[i]<<" "<<hRatio->Integral()<<endl;

            hRatio->Draw("E SAME");

            TLine *line = new TLine(xmin, 1, xmax, 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");
}

void plot2D(TH2D* hist, const char* title,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    pad1->SetLogx(0);
    pad1->SetLogy(0);
    pad1->SetLogz(1);
    pad1->Draw();
    pad1->cd();

    hist->SetStats(0);
    hist->SetTitle(title);
    hist->SetTitleSize(0.03, "XYZ"); // Decrease title size
    hist->GetXaxis()->SetTitle(xTitle);
    hist->GetXaxis()->SetTitleSize(0.05);
    hist->GetXaxis()->SetLabelSize(0.045);
    hist->GetXaxis()->SetRangeUser(xmin, xmax);
    hist->GetYaxis()->SetTitle(yTitle);
    hist->GetYaxis()->SetTitleSize(0.05);
    hist->GetYaxis()->SetLabelSize(0.045);
    hist->GetYaxis()->SetRangeUser(ymin, ymax);

    hist->Draw("COLZ");
}

void plotSimple(vector<TH1D*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    bool logx = false, bool logy = false, bool binnums = false) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    pad1->cd();

    TLegend *leg = new TLegend(0.68, 0.75, 0.58, 0.85); // smaller, upper right
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);

    double global_min = 1e30, global_max = -1e30;

    // Find global min/max for all hists in the visible x range
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1D* hist = hists[ih];
        double x1 = (xmin < xmax) ? xmin : hist->GetXaxis()->GetXmin();
        double x2 = (xmin < xmax) ? xmax : hist->GetXaxis()->GetXmax();
        int binmin = hist->GetXaxis()->FindBin(x1);
        int binmax = hist->GetXaxis()->FindBin(x2);
        for (int i = binmin; i <= binmax; ++i) {
            double val = hist->GetBinContent(i);
            if (val < global_min) global_min = val;
            if (val > global_max) global_max = val;
        }
    }
    // Add some margin
    double margin = 0.2 * (global_max - global_min);

    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1D* hist = hists[ih];
        //hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetTitleSize(0.05);
        hist->GetXaxis()->SetLabelSize(0.045);

        // Auto-scale x axis if xmin >= xmax
        if (xmin < xmax) {
            hist->GetXaxis()->SetRangeUser(xmin, xmax);
        } else {
            hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
        }

        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->GetYaxis()->SetLabelSize(0.03);
        hist->GetYaxis()->SetTitleOffset(1);

        // Auto-scale y axis if ymin < ymax, otherwise use global min/max
        if(logy && ymin < 1) ymin = 1.0;
        if (ymin < ymax) {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
        } else {
            hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
        }

        hist->SetLineColor(scolors[ih]);
        hist->SetLineWidth(2);

        // Set dashed line style for the second curve (ih == 1)
        if (ih == 1) {
            hist->SetLineStyle(2); // dashed
        } else {
            hist->SetLineStyle(1); // solid
        }

        if (ih == 0) {
            binnums ? hist->Draw("HIST TEXT0") : hist->Draw("HIST");
        } else {
            binnums ? hist->Draw("HIST TEXT0 SAME") : hist->Draw("HIST SAME");
        }

        if (labels.size() > ih)
            leg->AddEntry(hist, labels[ih].c_str(), "l");
    }

    leg->Draw("SAME");
}


void plotComparison(const char* forestInput =   "output/skimValidation_forest.root",
                const char* skimInput =     "output/skimValidation_skim.root",
                const char* output =        "plots/forest_and_skim") {

    // Open the input ROOT files
    TFile* fForest = TFile::Open(forestInput, "READ");
    if (!fForest || fForest->IsZombie()) {
        std::cerr << "Error: Unable to open forest file " << forestInput << std::endl;
        return;
    }
    TFile* fSkim = TFile::Open(skimInput, "READ");
    if (!fSkim || fSkim->IsZombie()) {
        std::cerr << "Error: Unable to open skim file " << skimInput << std::endl;
        return;
    }

    // Read in histograms from both files and fill vectors
    std::vector<string> labels = {"PPbMC_Gen", "PPbMC_Reco"};    
    // Fetch and uniquely name 1D histograms

    TH1D* hzY_forest = (TH1D*)fForest->Get("hzY");
    TH1D* hzY_skim = (TH1D*)fSkim->Get("hzY");
    vector<TH1D*> hzY = {hzY_forest, hzY_skim};

    TH1D* hzPt_forest = (TH1D*)fForest->Get("hzPt");
    TH1D* hzPt_skim = (TH1D*)fSkim->Get("hzPt");
    vector<TH1D*> hzPt = {hzPt_forest, hzPt_skim};

    TH1D* hgenZY_forest = (TH1D*)fForest->Get("hgenZY");
    TH1D* hgenZY_skim = (TH1D*)fSkim->Get("hgenZY");
    vector<TH1D*> hgenZY = {hgenZY_forest, hgenZY_skim};

    TH1D* hgenZPt_forest = (TH1D*)fForest->Get("hgenZPt");
    TH1D* hgenZPt_skim = (TH1D*)fSkim->Get("hgenZPt");
    vector<TH1D*> hgenZPt = {hgenZPt_forest, hgenZPt_skim};

    TH1D* hEventWeight_forest = (TH1D*)fForest->Get("hEventWeight");
    TH1D* hEventWeight_skim = (TH1D*)fSkim->Get("hEventWeight");
    vector<TH1D*> hEventWeights = {hEventWeight_forest, hEventWeight_skim};

    TH1D* hZWeight_forest = (TH1D*)fForest->Get("hZWeight");
    TH1D* hZWeight_skim = (TH1D*)fSkim->Get("hZWeight");
    vector<TH1D*> hZWeights = {hZWeight_forest, hZWeight_skim};

    TH1D* htrackPt_forest = (TH1D*)fForest->Get("htrackPt");
    TH1D* htrackPt_skim = (TH1D*)fSkim->Get("htrackPt");
    vector<TH1D*> htrackPt = {htrackPt_forest, htrackPt_skim};

    TH1D* htrackEta_forest = (TH1D*)fForest->Get("htrackEta");
    TH1D* htrackEta_skim = (TH1D*)fSkim->Get("htrackEta");
    vector<TH1D*> htrackEta = {htrackEta_forest, htrackEta_skim};

    TH1D* htrackWeight_forest = (TH1D*)fForest->Get("htrackWeight");
    TH1D* htrackWeight_skim = (TH1D*)fSkim->Get("htrackWeight");
    vector<TH1D*> htrackWeight = {htrackWeight_forest, htrackWeight_skim};

    TH1D* htrackResidualWeight_forest = (TH1D*)fForest->Get("htrackResidualWeight");
    TH1D* htrackResidualWeight_skim = (TH1D*)fSkim->Get("htrackResidualWeight");
    vector<TH1D*> htrackResidualWeight = {htrackResidualWeight_forest, htrackResidualWeight_skim};

    TH1D* hMult_forest = (TH1D*)fForest->Get("hMult");
    TH1D* hMult_skim = (TH1D*)fSkim->Get("hMult");
    vector<TH1D*> hMult = {hMult_forest, hMult_skim};

    TH1D* htrackPt_trackWeight_forest = (TH1D*)fForest->Get("htrackPt_trackWeight");
    TH1D* htrackPt_trackWeight_skim = (TH1D*)fSkim->Get("htrackPt_trackWeight");
    vector<TH1D*> htrackPt_trackWeight = {htrackPt_trackWeight_forest, htrackPt_trackWeight_skim};

    TH1D* htrackEta_trackWeight_forest = (TH1D*)fForest->Get("htrackEta_trackWeight");
    TH1D* htrackEta_trackWeight_skim = (TH1D*)fSkim->Get("htrackEta_trackWeight");
    vector<TH1D*> htrackEta_trackWeight = {htrackEta_trackWeight_forest, htrackEta_trackWeight_skim};

    TH1D* hMult_trackWeight_forest = (TH1D*)fForest->Get("hMult_trackWeight");
    TH1D* hMult_trackWeight_skim = (TH1D*)fSkim->Get("hMult_trackWeight");
    vector<TH1D*> hMult_trackWeight = {hMult_trackWeight_forest, hMult_trackWeight_skim};


    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 2400, 2700);
    c1->Divide(3, 4);

    c1->cd(1);
    plotSimple(
        hzY, "Z Boson Rapidity", labels,
        "y_{Z}", -3, 3,
        "dN/dy_{Z}", -1, -1,
        false, false, false
    );

    c1->cd(2);
    plotSimple(
        hzPt, "Z Boson p_{T}", labels,
        "p_{T}^{Z}", 0, 500,
        "dN/dp_{T}^{Z}", 1, 2e5,
        false, true, false
    );

    c1->cd(3);
    plotSimple(
        hgenZY, "Gen Z Boson Rapidity", labels,
        "y_{Z}", -3, 3,
        "dN/dy_{Z}", -1, -1,
        false, false, false
    );

    c1->cd(4);
    plotSimple(
        hgenZPt, "Gen Z Boson p_{T}", labels,
        "p_{T}^{Z}", 0, 500,
        "dN/dp_{T}^{Z}", 1, 2e5,
        false, true, false
    );

    c1->cd(5);
    plotSimple(
        hEventWeights, "Event Weights", labels,
        "Event Weight", 0.8, 1.2,
        "Counts", -1, -1,
        false, false, false
    );

    c1->cd(6);
    plotSimple(
        hZWeights, "Z Weights", labels,
        "Z Weight", 0.8, 1.2,
        "Counts", -1, -1,
        false, false, false
    );

    c1->cd(7);
    plotSimple(
        hMult, "Event Multiplicity", labels,
        "N_{ch}", 1, 300,
        "counts", -1, -1,
        false, false, false
    );

    c1->cd(8);
    plotSimple(
        hMult_trackWeight, "Event Multiplicity weighted by Track Weight", labels,
        "N_{ch}", 1, 300,
        "counts weighted", -1, -1,
        false, false, false
    );

    c1->SaveAs(Form("%s-eventlevel.png", output));

    TCanvas* c2 = new TCanvas("c2", "c2", 2400, 2700);
    c2->Divide(3, 4);

    c2->cd(1);
    plotSimple(
        htrackPt, "Track p_{T}", labels,
        "p_{T}^{track}", 0, 25,
        "dN/dp_{T}^{track}", 1, 2e7,
        false, true, false
    );

    c2->cd(2);
    plotSimple(
        htrackEta, "Track Y", labels,
        "Y^{track}", -3, 3,
        "dN/dY^{track}", -1, -1,
        false, false, false
    );

    c2->cd(3);
    plotSimple(
        htrackWeight, "Track Weights", labels,
        "Track Weight", 0, 2,
        "Counts", -1, -1,
        false, false, false
    );

    c2->cd(4);
    plotSimple(
        htrackResidualWeight, "Track Residual Weights", labels,
        "Track Residual Weight", 0, 2,
        "Counts", -1, -1,
        false, false, false
    );

    c2->cd(5);
    plotSimple(
        htrackPt_trackWeight, "Track p_{T} Weighted by Track Weight", labels,
        "p_{T}^{track}", 0, 25,
        "dN/dp_{T}^{track} #times Track Weight", 11, 2e7,
        false, true, false
    );

    c2->cd(6);
    plotSimple(
        htrackEta_trackWeight, "Track Y Weighted by Track Weight", labels,
        "Y^{track}", -3, 3,
        "dN/dY^{track}", -1, -1,
        false, false, false
    );

    c2->SaveAs(Form("%s-tracklevel.png", output));

}