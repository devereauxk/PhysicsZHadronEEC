#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <iostream>
#include "MITHIG_CMSStyle.h"

const int ccolors[13] = {
    kBlue+2,     // deep blue
    kAzure+7,    // cyan
    kTeal+3,     // teal
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

void divideByWidth(TH1* input) {
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

void setErrors(TH1* hRatio, TH1* num, TH1* denom){
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

void setDifferenceErrors(TH1 *hDiff, TH1 *first, TH1 *second)
{
    for (int bin = 1; bin <= hDiff->GetNbinsX(); bin++) {
        double error1 = first->GetBinError(bin);
        double error2 = second->GetBinError(bin);
        double diffError = sqrt(error1 * error1 + error2 * error2);
        hDiff->SetBinError(bin, diffError);
    }
}

void plotRatioLogy(vector<TH1*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logy = true,
    bool errorBars = true) {

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

    // Find global min/max for all hists in the visible x range
    double global_min = 1e30, global_max = -1e30;
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
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
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1* hist = hists[i];

        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->SetLineColor(ccolors[i]);

        // Auto-scale y axis if ymin < ymax, otherwise use global min/max
        if (ymin < ymax) {
            // If logy, ensure ymin > 0
            if (logy && ymin <= 0) {
            hist->GetYaxis()->SetRangeUser(1, ymax);
            } else {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
            }
        } else {
            if (logy && global_min-margin <= 0) {
                hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            } else {
                hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
            }
        }

        if (i == baseline) {
            hist->Draw("E SAME");
        } else {
            hist->Draw("HIST SAME");
        }
        leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1* hRatio = (TH1*)hist->Clone(Form("ratio_TrkPt_%d", i));
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
            if (errorBars) {
                //setErrors(hRatio, hist, hists.at(baseline));
                hRatio->Draw("E SAME");
            } else {
                hRatio->Draw("HIST SAME");
            }

            cout << title << " " << labels[i] << " " << hRatio->Integral() << endl;

            // Print (ratio y value, ratio y value * bin width) for each bin
            for (int bin = 1; bin <= hRatio->GetNbinsX(); ++bin) {
                double y = (1 - hRatio->GetBinContent(bin)) * hists[baseline]->GetBinContent(bin);
                double w = hRatio->GetBinWidth(bin);
                cout << y << " " << y * w << endl;
            }

            TLine *line = new TLine(xmin, 1, xmax, 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");
}

void plot2D(TH2* hist, const char* title,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* zTitle, double zmin, double zmax,
    bool logx = false, bool logy = true, bool logz = false) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    pad1->SetLogx(logx);
    pad1->SetLogy(logy);
    pad1->SetLogz(logz);
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
    hist->GetZaxis()->SetTitle(zTitle); // Remove z axis title
    hist->GetZaxis()->SetTitleSize(0.05);
    hist->GetZaxis()->SetLabelSize(0.045);
    hist->GetZaxis()->SetRangeUser(zmin, zmax);

    hist->Draw("COLZ");
}

void plotSimple(vector<TH1*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    bool logx = false, bool logy = false, bool binnums = false) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    pad1->cd();

    TLegend *leg = new TLegend(0.65, 0.7, 0.85, 0.87);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.025); // Reduce font size

    double global_min = 1e30, global_max = -1e30;

    // Find global min/max for all hists in the visible x range
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
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
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
        cout<<"Global min: " << global_min << ", Global max: " << global_max << ", Margin: " << margin << endl;
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
        hist->SetStats(0);
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
        if (ymin < ymax) {
            // If logy, ensure ymin > 0
            if (logy && ymin <= 0) {
            hist->GetYaxis()->SetRangeUser(1, ymax);
            } else {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
            }
        } else {
            if (logy && global_min <= 0) {
                hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            } else {
                hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
            }
        }

        hist->SetLineColor(scolors[ih]);
        hist->SetLineWidth(2);

        // Set dashed line style for the second curve (ih == 1)
        if (ih == 1) {
            hist->SetLineStyle(2); // dashed
        } else {
            hist->SetLineStyle(1); // solid
        }

        // Draw histogram as usual
        if (ih == 0) {
            hist->Draw("HIST");
        } else {
            hist->Draw("HIST SAME");
        }

        // Old way: draw bin content above each bin using "TEXT0" option
        if (binnums) {
            hist->Draw("SAME TEXT0");
        }

        /*
        if (binnums) {
            for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
            double x = hist->GetBinCenter(bin);
            double y = hist->GetBinContent(bin);
            // Place 'a' slightly above the bin content
            double y_offset = (y > 0) ? y + 0.03 * (hist->GetMaximum() - hist->GetMinimum()) : 0.03 * (hist->GetMaximum() - hist->GetMinimum());
            TLatex latex;
            latex.SetTextAlign(22);
            latex.SetTextSize(0.027);
            latex.DrawLatex(x, y_offset, Form("%.0f", y));
            }
        }
        */

        if (labels.size() > ih)
            leg->AddEntry(hist, labels[ih].c_str(), "l");
    }

    leg->Draw("SAME");
}

void LabelBinContent(TH1* hist) {
  for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
    double x = hist->GetBinCenter(bin);
    double y = hist->GetBinContent(bin);
    double y_offset = (y > 0) ? y + 0.03 * (hist->GetMaximum() - hist->GetMinimum()) : 0.03 * (hist->GetMaximum() - hist->GetMinimum());
    TLatex latex;
    latex.SetTextAlign(22);
    latex.SetTextSize(0.027);
    latex.DrawLatex(x, y_offset, Form("%.0f", y));
  }
}

TPad* plotCMSSimple(TCanvas* c, vector<TH1*> hists, const char* title, vector<string> labels,
    vector<Int_t> linecolors, vector<Int_t> linestyles, vector<Int_t> markercolors, vector<Int_t> markerstyles,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    bool logx = false, bool logy = false,
    bool binnums = false, string horizonalLine = "") {

    // Get the canvas pad to pass to other functions
    TPad* pad1 = (TPad*) c->GetPad(0);
    pad1->cd();
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    pad1->SetLeftMargin(0.15); // Increase left margin for more blank space
    
    // >>> Apply the CMS TDR style <<<
    SetTDRStyle();

    TLegend* leg = new TLegend(0.25, 0.7, 0.68, 0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.035);

    // Find global min/max for all hists in the visible x range
    double global_min = 1e30, global_max = -1e30;
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
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
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    for (int i = 0; i < hists.size(); i++) {

        TH1* hist = hists[i];

        hist->SetLineColor(linecolors[i]);
        if (linestyles[i] == 0) hist->SetLineStyle(1);
        else if (linestyles[i] > 0) hist->SetLineStyle(linestyles[i]);
        hist->SetMarkerColor(markercolors[i]);
        hist->SetMarkerStyle(markerstyles[i]);
        if (linestyles[i] == 0) hist->SetLineWidth(3);
        else if (linestyles[i] > 0) hist->SetLineWidth(2);

        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.04);

        // Auto-scale y axis if ymin < ymax, otherwise use global min/max
        if (ymin < ymax) {
            // If logy, ensure ymin > 0
            if (logy && ymin <= 0) {
            hist->GetYaxis()->SetRangeUser(1, ymax);
            } else {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
            }
        } else {
            if (logy && global_min-margin <= 0) {
                hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            } else {
                hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
            }
        }

        if (binnums) hist->Draw("SAME TEXT0");
        else if (linestyles[i] == 0) hist->Draw("SAME");
        else if (linestyles[i] == -1) hist->Draw("P SAME");
        else hist->Draw("HIST SAME");
        
        if (linestyles[i] == 0) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "pl");
        else if (linestyles[i] == -1) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "p");
        else leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");
    }
    leg->Draw("SAME");

    if (horizonalLine != "") {
        TF1* line = new TF1("line", horizonalLine.c_str(), xmin, xmax);
        line->SetLineColor(kGray);
        line->SetLineStyle(7);
        line->Draw("same");
    }

    return pad1; // Return the pad for further customization if needed
}


TPad* plotCMSRatio(vector<TH1*> hists, const char* title, vector<string> labels,
    vector<Int_t> linecolors, vector<Int_t> linestyles, vector<Int_t> markercolors, vector<Int_t> markerstyles,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    int baseline = 0,
    bool logx = false, bool logy = false,
    bool errorBars = true, 
    float xLegend = 0.55) {

    // linestyle options
    // -1: no line, only markers [homemade]
    // 0: solid histogram with point in center
    // 1: solid histogram
    // 2: dashed histogram

    // Get the canvas pad to pass to other functions
    // Leave a 50% larger border around the figure within the canvas
    double border = 0.06; // 7.5% border on all sides (50% larger than default 5%)
    TPad *pad1 = new TPad(title, title, border, 0.25 + border, 1.0 - border, 1.0 - border);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, border, border, 1.0 - border, 0.25 + border);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    logx ? pad2->SetLogx() : pad2->SetLogx(0);
    pad2->Draw();
    
    // >>> Apply the CMS TDR style <<<
    SetTDRStyle();

    TLegend* leg = new TLegend(xLegend, (labels.size() > 5) ? 0.6 : 0.7, xLegend+0.23, 0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.035);

    // Find global min/max for all hists in the visible x range
    double global_min = 1e30, global_max = -1e30;
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
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
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1* hist = hists[i];

        if (linestyles[i] == -1) hist->SetLineColorAlpha(0, 0);
        else hist->SetLineColor(linecolors[i]);
        if (linestyles[i] == 0) hist->SetLineStyle(1);
        else if (linestyles[i] > 0) hist->SetLineStyle(linestyles[i]);
        hist->SetMarkerColor(markercolors[i]);
        hist->SetMarkerStyle(markerstyles[i]);
        if (linestyles[i] == 0) hist->SetLineWidth(3);
        else if (linestyles[i] > 0) hist->SetLineWidth(2);

        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->GetYaxis()->SetTitleOffset(0.7);

        // Auto-scale y axis if ymin < ymax, otherwise use global min/max
        if (ymin < ymax) {
            // If logy, ensure ymin > 0
            if (logy && ymin <= 0) {
            hist->GetYaxis()->SetRangeUser(1, ymax);
            } else {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
            }
        } else {
            if (logy && global_min-margin <= 0) {
                hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            } else {
                hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
            }
        }

        // get draw command sequence
        string drawCommand = "SAME";

        if (linestyles[i] == -1) drawCommand = "P " + drawCommand;
        else if (linestyles[i] != 0) drawCommand = "HIST " + drawCommand;

        if (errorBars && linestyles[i] == -1) drawCommand = "E " + drawCommand;

        hist->Draw(drawCommand.c_str());
        

        if (linestyles[i] == 0) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "pl");
        else if (linestyles[i] == -1) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "p");
        else leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1* hRatio = (TH1*)hist->Clone(Form("ratio_%s_%d", title, i));
            hRatio->Divide(hists[baseline]);

            /*
            cout<<"num : " << hist->GetBinContent(3) << " +/- " << hist->GetBinError(3) << endl;
            cout<<"denom : " << hists[baseline]->GetBinContent(3) << " +/- " << hists[baseline]->GetBinError(3) << endl;
            cout<<"ratio : " << hRatio->GetBinContent(3) << " +/- " << hRatio->GetBinError(3) << endl;
            */

            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(1);
            hRatio->GetYaxis()->SetTitle(rTitle);
            hRatio->GetYaxis()->SetRangeUser(rmin, rmax);
            hRatio->GetYaxis()->SetTitleSize(0.08);
            hRatio->GetYaxis()->SetLabelSize(0.06);
            hRatio->GetYaxis()->SetTitleOffset(0.5);
            hRatio->SetLineColor(linecolors[i]);
            hRatio->SetLineStyle(linestyles[i]);
            hRatio->SetLineWidth(2);

            // Draw error bars on the ratio plot if requested
            if (errorBars) {
                hRatio->Draw("E SAME");
            } else {
                hRatio->Draw("HIST SAME");
            }

            double xlow = hRatio->GetXaxis()->GetBinLowEdge(hRatio->GetXaxis()->GetFirst());
            double xhigh = hRatio->GetXaxis()->GetBinUpEdge(hRatio->GetXaxis()->GetLast());
            TLine *line = new TLine(xlow, 1, xhigh, 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");

    return pad1; // Return the main pad for further customization if needed
}

TPad* plotCMSDiff(vector<TH1*> hists, const char* title, vector<string> labels,
    vector<Int_t> linecolors, vector<Int_t> linestyles, vector<Int_t> markercolors, vector<Int_t> markerstyles,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logx = false, bool logy = false,
    bool errorBars = true,
    float xLegend = 0.55) {

    // linestyle options
    // -1: no line, only markers [homemade]
    // 0: solid histogram with point in center
    // 1: solid histogram
    // 2: dashed histogram

    // Get the canvas pad to pass to other functions
    // Leave a 50% larger border around the figure within the canvas
    double border = 0.06; // 7.5% border on all sides (50% larger than default 5%)
    TPad *pad1 = new TPad(title, title, border, 0.25 + border, 1.0 - border, 1.0 - border);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, border, border, 1.0 - border, 0.25 + border);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    logx ? pad2->SetLogx() : pad2->SetLogx(0);
    pad2->Draw();
    
    // >>> Apply the CMS TDR style <<<
    SetTDRStyle();

    TLegend* leg = new TLegend(xLegend, 0.7, xLegend+0.23, 0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.035);

    // Find global min/max for all hists in the visible x range
    double global_min = 1e30, global_max = -1e30;
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
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
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1* hist = hists[i];

        if (linestyles[i] == -1) hist->SetLineColorAlpha(0, 0);
        else hist->SetLineColor(linecolors[i]);
        if (linestyles[i] == 0) hist->SetLineStyle(1);
        else if (linestyles[i] > 0) hist->SetLineStyle(linestyles[i]);
        hist->SetMarkerColor(markercolors[i]);
        hist->SetMarkerStyle(markerstyles[i]);
        if (linestyles[i] == 0) hist->SetLineWidth(3);
        else if (linestyles[i] > 0) hist->SetLineWidth(2);

        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->GetYaxis()->SetTitleOffset(0.7);

        // Auto-scale y axis if ymin < ymax, otherwise use global min/max
        if (ymin < ymax) {
            // If logy, ensure ymin > 0
            if (logy && ymin <= 0) {
            hist->GetYaxis()->SetRangeUser(1, ymax);
            } else {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
            }
        } else {
            if (logy && global_min-margin <= 0) {
                hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            } else {
                hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
            }
        }

        if (linestyles[i] == 0) hist->Draw("SAME");
        else if (linestyles[i] == -1) hist->Draw("P SAME");
        else hist->Draw("HIST SAME");
        
        if (linestyles[i] == 0) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "pl");
        else if (linestyles[i] == -1) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "p");
        else leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1* hRatio = (TH1*)hist->Clone(Form("ratio_%s_%d", title, i));
            hRatio->Add(hists[baseline], -1);
            setDifferenceErrors(hRatio, hist, hists[baseline]);
            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(1);
            hRatio->GetYaxis()->SetTitle(rTitle);
            hRatio->GetYaxis()->SetRangeUser(rmin, rmax);
            hRatio->GetYaxis()->SetTitleSize(0.08);
            hRatio->GetYaxis()->SetLabelSize(0.06);
            hRatio->GetYaxis()->SetTitleOffset(0.5);
            hRatio->SetLineColor(linecolors[i]);
            hRatio->SetLineStyle(linestyles[i]);
            hRatio->SetMarkerColor(markercolors[i]);
            hRatio->SetMarkerStyle(markerstyles[i]);
            hRatio->SetLineWidth(2);
            hRatio->Draw("HIST SAME");

            if (errorBars)
                hRatio->Draw("E SAME");
            else
                hRatio->Draw("HIST SAME");

            double xlow = hRatio->GetXaxis()->GetBinLowEdge(hRatio->GetXaxis()->GetFirst());
            double xhigh = hRatio->GetXaxis()->GetBinUpEdge(hRatio->GetXaxis()->GetLast());
            TLine *line = new TLine(xlow, 0, xhigh, 0);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");

    return pad1; // Return the main pad for further customization if needed
}

TGraphAsymmErrors *BuildSystematicBand(TH1 *central, TH1 *uncertainty,
    const char *name, Int_t color, float alpha = 0.25)
{
    if (central == nullptr || uncertainty == nullptr)
        return nullptr;

    TGraphAsymmErrors *band = new TGraphAsymmErrors(central->GetNbinsX());
    band->SetName(name);
    band->SetFillColorAlpha(color, alpha);
    band->SetLineColorAlpha(color, alpha);
    band->SetLineWidth(0);
    band->SetMarkerSize(0);

    for (int i = 1; i <= central->GetNbinsX(); i++) {
        double x = central->GetBinCenter(i);
        double y = central->GetBinContent(i);
        double ex = central->GetBinWidth(i) / 2;
        double ey = fabs(uncertainty->GetBinContent(i));
        band->SetPoint(i - 1, x, y);
        band->SetPointError(i - 1, ex, ex, ey, ey);
    }

    return band;
}

void updateDiffRange(double value, double uncertainty, double &minimum, double &maximum)
{
    minimum = min(minimum, value - uncertainty);
    maximum = max(maximum, value + uncertainty);
}

TPad* PlotCMSDiffResult(vector<TH1*> hists, vector<TH1*> topSystematics, vector<TH1*> bottomSystematics,
    const char* title, vector<string> labels, vector<Int_t> linecolors, vector<Int_t> linestyles,
    vector<Int_t> markercolors, vector<Int_t> markerstyles, const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax, const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logx = false, bool logy = false, bool errorBars = true, float xLegend = 0.55)
{
    double border = 0.06;
    TPad *pad1 = new TPad(title, title, border, 0.25 + border, 1.0 - border, 1.0 - border);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, border, border, 1.0 - border, 0.25 + border);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    logx ? pad2->SetLogx() : pad2->SetLogx(0);
    pad2->Draw();

    SetTDRStyle();

    TLegend* leg = new TLegend(xLegend, 0.7, xLegend+0.23, 0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.035);

    double global_min = 1e30, global_max = -1e30;
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
        TH1* syst = (ih < topSystematics.size()) ? topSystematics[ih] : nullptr;
        double x1 = (xmin < xmax) ? xmin : hist->GetXaxis()->GetXmin();
        double x2 = (xmin < xmax) ? xmax : hist->GetXaxis()->GetXmax();
        int binmin = hist->GetXaxis()->FindBin(x1);
        int binmax = hist->GetXaxis()->FindBin(x2);
        for (int i = binmin; i <= binmax; ++i) {
            double value = hist->GetBinContent(i);
            bool drawStat = errorBars && (linestyles[ih] == 0 || linestyles[ih] == -1);
            double stat = drawStat ? hist->GetBinError(i) : 0;
            double systError = (syst != nullptr) ? fabs(syst->GetBinContent(i)) : 0;
            global_min = min(global_min, value - max(stat, systError));
            global_max = max(global_max, value + max(stat, systError));
        }
    }
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    double diff_min = 1e30, diff_max = -1e30;
    bool hasDifference = false;
    for (int i = 0; i < hists.size(); i++) {
        if (i == baseline)
            continue;
        TH1 *hist = hists[i];
        TH1 *syst = (i < bottomSystematics.size()) ? bottomSystematics[i] : nullptr;
        TH1 *diff = (TH1 *)hist->Clone(Form("range_%s_%d", title, i));
        diff->Add(hists[baseline], -1);
        setDifferenceErrors(diff, hist, hists[baseline]);
        for (int bin = 1; bin <= diff->GetNbinsX(); bin++) {
            double value = diff->GetBinContent(bin);
            bool drawStat = errorBars && (linestyles[i] == 0 || linestyles[i] == -1);
            double stat = drawStat ? diff->GetBinError(bin) : 0;
            double systError = (syst != nullptr) ? fabs(syst->GetBinContent(bin)) : 0;
            updateDiffRange(value, max(stat, systError), diff_min, diff_max);
            hasDifference = true;
        }
        delete diff;
    }
    if (hasDifference == false) {
        diff_min = -1;
        diff_max = 1;
    }
    double diff_margin = 0.15 * (diff_max - diff_min);
    if (diff_margin <= 0)
        diff_margin = max(fabs(diff_max), 1.0) * 0.15;

    bool firstDifference = true;
    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1* hist = hists[i];
        TH1* topSyst = (i < topSystematics.size()) ? topSystematics[i] : nullptr;

        if (linestyles[i] == -1) hist->SetLineColorAlpha(0, 0);
        else hist->SetLineColor(linecolors[i]);
        if (linestyles[i] == 0) hist->SetLineStyle(1);
        else if (linestyles[i] > 0) hist->SetLineStyle(linestyles[i]);
        hist->SetMarkerColor(markercolors[i]);
        hist->SetMarkerStyle(markerstyles[i]);
        hist->SetStats(0);
        if (linestyles[i] == 0) hist->SetLineWidth(3);
        else if (linestyles[i] > 0) hist->SetLineWidth(2);

        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->GetYaxis()->SetTitleOffset(0.7);

        if (ymin < ymax) {
            if (logy && ymin <= 0)
                hist->GetYaxis()->SetRangeUser(1, ymax);
            else
                hist->GetYaxis()->SetRangeUser(ymin, ymax);
        } else {
            if (logy && global_min - margin <= 0)
                hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            else
                hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
        }

        if (linestyles[i] == 0) hist->Draw("SAME");
        else if (linestyles[i] == -1) hist->Draw("P SAME");
        else hist->Draw("HIST SAME");

        if (topSyst != nullptr) {
            TGraphAsymmErrors *band = BuildSystematicBand(hist, topSyst,
                Form("top_band_%s_%d", title, i), linecolors[i]);
            if (band != nullptr)
                band->Draw("2 SAME");
            if (linestyles[i] == 0) hist->Draw("SAME");
            else if (linestyles[i] == -1) hist->Draw("P SAME");
            else hist->Draw("HIST SAME");
        }

        if (errorBars == true && (linestyles[i] == 0 || linestyles[i] == -1))
            hist->Draw("E1 SAME");

        if (linestyles[i] == 0) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "pl");
        else if (linestyles[i] == -1) leg->AddEntry(hist, Form("%s", labels[i].c_str()), "p");
        else leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1* hDiff = (TH1*)hist->Clone(Form("diff_%s_%d", title, i));
            hDiff->Add(hists[baseline], -1);
            setDifferenceErrors(hDiff, hist, hists[baseline]);
            TH1 *bottomSyst = (i < bottomSystematics.size()) ? bottomSystematics[i] : nullptr;
            hDiff->GetXaxis()->SetTitle(xTitle);
            hDiff->GetXaxis()->SetTitleSize(0.1);
            hDiff->GetXaxis()->SetLabelSize(0.08);
            hDiff->GetXaxis()->SetTitleOffset(1);
            hDiff->GetYaxis()->SetTitle(rTitle);
            if (rmin < rmax)
                hDiff->GetYaxis()->SetRangeUser(rmin, rmax);
            else
                hDiff->GetYaxis()->SetRangeUser(diff_min - diff_margin, diff_max + diff_margin);
            hDiff->GetYaxis()->SetTitleSize(0.08);
            hDiff->GetYaxis()->SetLabelSize(0.06);
            hDiff->GetYaxis()->SetTitleOffset(0.5);
            hDiff->SetLineColor(linecolors[i]);
            hDiff->SetLineStyle(linestyles[i] == 0 ? 1 : linestyles[i]);
            hDiff->SetMarkerColor(markercolors[i]);
            hDiff->SetMarkerStyle(markerstyles[i]);
            hDiff->SetLineWidth(2);

            bool drawStat = errorBars && (linestyles[i] == 0 || linestyles[i] == -1);
            TString diffDrawOption = drawStat ? "E1" : ((linestyles[i] > 0) ? "HIST" : "P");
            if (firstDifference == false)
                diffDrawOption += " SAME";
            hDiff->Draw(diffDrawOption);

            if (bottomSyst != nullptr) {
                TGraphAsymmErrors *band = BuildSystematicBand(hDiff, bottomSyst,
                    Form("bottom_band_%s_%d", title, i), linecolors[i]);
                if (band != nullptr)
                    band->Draw("2 SAME");
            }

            if (drawStat == true)
                hDiff->Draw("E1 SAME");
            else if (linestyles[i] > 0)
                hDiff->Draw("HIST SAME");
            else
                hDiff->Draw("P SAME");

            double xlow = hDiff->GetXaxis()->GetBinLowEdge(hDiff->GetXaxis()->GetFirst());
            double xhigh = hDiff->GetXaxis()->GetBinUpEdge(hDiff->GetXaxis()->GetLast());
            TLine *line = new TLine(xlow, 0, xhigh, 0);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
            firstDifference = false;
        }
    }
    pad1->cd();
    leg->Draw("SAME");
    AddCMSHeader(pad1, "Preliminary", false);

    return pad1;
}

// Helper: build TGraphErrors from a TH1 for bins whose center is inside (inRegion=true)
// or outside (inRegion=false) the range [rMin, rMax].
static TGraphErrors* HistToRegionGraph(TH1* h, double rMin, double rMax, bool inRegion, const char* name)
{
    vector<double> vx, vy, vex, vey;
    for (int i = 1; i <= h->GetNbinsX(); ++i) {
        double center = h->GetXaxis()->GetBinCenter(i);
        bool inside = (center >= rMin && center <= rMax);
        if (inside == inRegion) {
            vx.push_back(center);
            vy.push_back(h->GetBinContent(i));
            vex.push_back(0.0);
            vey.push_back(h->GetBinError(i));
        }
    }
    if (vx.empty()) return nullptr;
    TGraphErrors* g = new TGraphErrors(vx.size(), vx.data(), vy.data(), vex.data(), vey.data());
    g->SetName(name);
    return g;
}

// ROOT: filled→open is +4 for standard marker styles 20-23 (circle, square, triangle, diamond).
static int OpenMarkerStyle(int style)
{
    if (style >= 20 && style <= 23) return style + 4;
    return 24; // fallback: open circle
}

// Variant of PlotCMSDiffResult that draws data points with filled markers only in
// [signalXMin, signalXMax] and open markers (+4 in ROOT convention) outside that range.
// Line-style entries (Powheg MC) are drawn as HIST with no change.
TPad* PlotCMSDiffResultRegion(
    vector<TH1*> hists, vector<TH1*> topSystematics, vector<TH1*> bottomSystematics,
    const char* title, vector<string> labels, vector<Int_t> linecolors, vector<Int_t> linestyles,
    vector<Int_t> markercolors, vector<Int_t> markerstyles, const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax, const char* rTitle, double rmin, double rmax,
    double signalXMin, double signalXMax,
    int baseline = 0, bool logx = false, bool logy = false, bool errorBars = true, float xLegend = 0.55)
{
    SetTDRStyle();

    double border = 0.06;
    TPad *pad1 = new TPad(title, title, 0, 0.25 + border, 1.0 - border, 1.0 - border);
    pad1->SetTickx(1);
    pad1->SetTicky(1);
    pad1->SetLeftMargin(0.15);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, 0, 0, 1.0 - border, 0.25 + border);
    pad2->SetTickx(1);
    pad2->SetTicky(1);
    pad2->SetLeftMargin(0.15);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.25);
    logx ? pad2->SetLogx() : pad2->SetLogx(0);
    pad2->Draw();

    TLegend* leg = new TLegend(xLegend, 0.7, xLegend + 0.23, 0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.035);

    // Auto Y-range
    double global_min = 1e30, global_max = -1e30;
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
        TH1* syst = (ih < topSystematics.size()) ? topSystematics[ih] : nullptr;
        double x1 = (xmin < xmax) ? xmin : hist->GetXaxis()->GetXmin();
        double x2 = (xmin < xmax) ? xmax : hist->GetXaxis()->GetXmax();
        int binmin = hist->GetXaxis()->FindBin(x1);
        int binmax = hist->GetXaxis()->FindBin(x2);
        for (int i = binmin; i <= binmax; ++i) {
            double value = hist->GetBinContent(i);
            bool drawStat = errorBars && (linestyles[ih] == 0 || linestyles[ih] == -1);
            double stat = drawStat ? hist->GetBinError(i) : 0;
            double systError = (syst != nullptr) ? fabs(syst->GetBinContent(i)) : 0;
            global_min = min(global_min, value - max(stat, systError));
            global_max = max(global_max, value + max(stat, systError));
        }
    }
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    // Auto difference Y-range
    double diff_min = 1e30, diff_max = -1e30;
    bool hasDifference = false;
    for (int i = 0; i < (int)hists.size(); i++) {
        if (i == baseline) continue;
        TH1 *hist = hists[i];
        TH1 *syst = (i < (int)bottomSystematics.size()) ? bottomSystematics[i] : nullptr;
        TH1 *diff = (TH1*)hist->Clone(Form("rangeReg_%s_%d", title, i));
        diff->Add(hists[baseline], -1);
        setDifferenceErrors(diff, hist, hists[baseline]);
        for (int bin = 1; bin <= diff->GetNbinsX(); bin++) {
            double value = diff->GetBinContent(bin);
            bool drawStat = errorBars && (linestyles[i] == 0 || linestyles[i] == -1);
            double stat = drawStat ? diff->GetBinError(bin) : 0;
            double systError = (syst != nullptr) ? fabs(syst->GetBinContent(bin)) : 0;
            updateDiffRange(value, max(stat, systError), diff_min, diff_max);
            hasDifference = true;
        }
        delete diff;
    }
    if (!hasDifference) { diff_min = -1; diff_max = 1; }
    double diff_absmax = max(fabs(diff_min), fabs(diff_max));
    double diff_margin = 0.15 * diff_absmax;
    if (diff_margin <= 0) diff_margin = 0.15;
    diff_min = -(diff_absmax + diff_margin);
    diff_max =  (diff_absmax + diff_margin);

    bool firstDifference = true;
    for (int i = 0; i < (int)hists.size(); i++) {
        pad1->cd();

        TH1* hist = hists[i];
        TH1* topSyst = (i < (int)topSystematics.size()) ? topSystematics[i] : nullptr;
        bool isMarker = (linestyles[i] == 0 || linestyles[i] == -1);

        if (linestyles[i] == -1) hist->SetLineColorAlpha(0, 0);
        else hist->SetLineColor(linecolors[i]);
        if (linestyles[i] == 0) hist->SetLineStyle(1);
        else if (linestyles[i] > 0) hist->SetLineStyle(linestyles[i]);
        hist->SetMarkerColor(markercolors[i]);
        hist->SetMarkerStyle(markerstyles[i]);
        hist->SetStats(0);
        if (linestyles[i] == 0) hist->SetLineWidth(3);
        else if (linestyles[i] > 0) hist->SetLineWidth(2);

        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->GetYaxis()->SetTitleOffset(1.2);

        if (ymin < ymax) {
            if (logy && ymin <= 0) hist->GetYaxis()->SetRangeUser(1, ymax);
            else hist->GetYaxis()->SetRangeUser(ymin, ymax);
        } else {
            if (logy && global_min - margin <= 0) hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            else hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
        }

        if (isMarker) {
            // Draw invisible clone to establish axis frame and Y range
            TH1* hFrame = (TH1*)hist->Clone(Form("reg_frame_%s_%d", title, i));
            hFrame->SetMarkerColorAlpha(0, 0);
            hFrame->SetLineColorAlpha(0, 0);
            hFrame->SetFillColorAlpha(0, 0);
            hFrame->Draw("SAME");

            // Systematic band (full range, drawn before data points)
            if (topSyst != nullptr) {
                TGraphAsymmErrors *band = BuildSystematicBand(hist, topSyst,
                    Form("reg_top_band_%s_%d", title, i), linecolors[i]);
                if (band != nullptr) band->Draw("2 SAME");
            }

            // Signal region: filled marker
            TGraphErrors* gSig = HistToRegionGraph(hist, signalXMin, signalXMax, true,
                Form("reg_gsig_%s_%d", title, i));
            if (gSig != nullptr) {
                gSig->SetMarkerColor(markercolors[i]);
                gSig->SetMarkerStyle(markerstyles[i]);
                gSig->SetLineColor(markercolors[i]);
                gSig->Draw(errorBars ? "PE SAME" : "P SAME");
            }

            // Outside region: open marker (+4 in ROOT convention)
            TGraphErrors* gOut = HistToRegionGraph(hist, signalXMin, signalXMax, false,
                Form("reg_gout_%s_%d", title, i));
            if (gOut != nullptr) {
                gOut->SetMarkerColor(markercolors[i]);
                gOut->SetMarkerStyle(OpenMarkerStyle(markerstyles[i]));
                gOut->SetLineColor(markercolors[i]);
                gOut->Draw(errorBars ? "PE SAME" : "P SAME");
            }

            // Legend entry: proxy TGraph with filled marker
            TGraph* legProxy = new TGraph(1);
            legProxy->SetMarkerColor(markercolors[i]);
            legProxy->SetMarkerStyle(markerstyles[i]);
            legProxy->SetLineColor(markercolors[i]);
            leg->AddEntry(legProxy, labels[i].c_str(), "p");
        } else {
            hist->Draw("HIST SAME");
            leg->AddEntry(hist, labels[i].c_str(), "l");
        }

        // Lower pad
        pad2->cd();
        if (i != baseline) {
            TH1* hDiff = (TH1*)hist->Clone(Form("reg_diff_%s_%d", title, i));
            hDiff->Add(hists[baseline], -1);
            setDifferenceErrors(hDiff, hist, hists[baseline]);
            TH1* bottomSyst = (i < (int)bottomSystematics.size()) ? bottomSystematics[i] : nullptr;

            hDiff->GetXaxis()->SetTitle(xTitle);
            hDiff->GetXaxis()->SetTitleSize(0.1);
            hDiff->GetXaxis()->SetLabelSize(0.08);
            hDiff->GetXaxis()->SetTitleOffset(1);
            hDiff->GetYaxis()->SetTitle(rTitle);
            if (rmin < rmax)
                hDiff->GetYaxis()->SetRangeUser(rmin, rmax);
            else
                hDiff->GetYaxis()->SetRangeUser(diff_min - diff_margin, diff_max + diff_margin);
            hDiff->GetYaxis()->SetTitleSize(0.08);
            hDiff->GetYaxis()->SetLabelSize(0.06);
            hDiff->GetYaxis()->SetTitleOffset(0.6);
            hDiff->SetLineColor(linecolors[i]);
            hDiff->SetLineStyle(linestyles[i] == 0 ? 1 : linestyles[i]);
            hDiff->SetMarkerColor(markercolors[i]);
            hDiff->SetMarkerStyle(markerstyles[i]);
            hDiff->SetLineWidth(2);

            bool drawStat = errorBars && (linestyles[i] == 0 || linestyles[i] == -1);

            if (isMarker) {
                // Invisible frame to establish axis (no SAME on first draw in pad2)
                TH1* hDiffFrame = (TH1*)hDiff->Clone(Form("reg_diff_frame_%s_%d", title, i));
                hDiffFrame->SetMarkerColorAlpha(0, 0);
                hDiffFrame->SetLineColorAlpha(0, 0);
                hDiffFrame->SetFillColorAlpha(0, 0);
                hDiffFrame->Draw(firstDifference ? "" : "SAME");

                if (bottomSyst != nullptr) {
                    TGraphAsymmErrors *band = BuildSystematicBand(hDiff, bottomSyst,
                        Form("reg_bottom_band_%s_%d", title, i), linecolors[i]);
                    if (band != nullptr) band->Draw("2 SAME");
                }

                TGraphErrors* gSigD = HistToRegionGraph(hDiff, signalXMin, signalXMax, true,
                    Form("reg_diff_gsig_%s_%d", title, i));
                if (gSigD != nullptr) {
                    gSigD->SetMarkerColor(markercolors[i]);
                    gSigD->SetMarkerStyle(markerstyles[i]);
                    gSigD->SetLineColor(markercolors[i]);
                    gSigD->Draw(drawStat ? "PE SAME" : "P SAME");
                }

                TGraphErrors* gOutD = HistToRegionGraph(hDiff, signalXMin, signalXMax, false,
                    Form("reg_diff_gout_%s_%d", title, i));
                if (gOutD != nullptr) {
                    gOutD->SetMarkerColor(markercolors[i]);
                    gOutD->SetMarkerStyle(OpenMarkerStyle(markerstyles[i]));
                    gOutD->SetLineColor(markercolors[i]);
                    gOutD->Draw(drawStat ? "PE SAME" : "P SAME");
                }
            } else {
                TString diffDrawOption = drawStat ? "E1" : ((linestyles[i] > 0) ? "HIST" : "P");
                if (!firstDifference) diffDrawOption += " SAME";
                hDiff->Draw(diffDrawOption);

                if (bottomSyst != nullptr) {
                    TGraphAsymmErrors *band = BuildSystematicBand(hDiff, bottomSyst,
                        Form("reg_bottom_band_%s_%d", title, i), linecolors[i]);
                    if (band != nullptr) band->Draw("2 SAME");
                    if (drawStat) hDiff->Draw("E1 SAME");
                    else if (linestyles[i] > 0) hDiff->Draw("HIST SAME");
                    else hDiff->Draw("P SAME");
                }

                if (drawStat) hDiff->Draw("E1 SAME");
                else if (linestyles[i] > 0) hDiff->Draw("HIST SAME");
                else hDiff->Draw("P SAME");
            }

            double xlow = hDiff->GetXaxis()->GetBinLowEdge(hDiff->GetXaxis()->GetFirst());
            double xhigh = hDiff->GetXaxis()->GetBinUpEdge(hDiff->GetXaxis()->GetLast());
            TLine *line = new TLine(xlow, 0, xhigh, 0);
            line->SetLineColor(kGray + 2);
            line->SetLineStyle(2);
            line->Draw("SAME");
            firstDifference = false;
        }
    }
    pad1->cd();
    leg->Draw("SAME");
    AddCMSHeader(pad1, "Preliminary", false);

    return pad1;
}

// Paper-quality variant of PlotCMSDiffResult.
// - Does NOT draw CMS header (caller adds custom header)
// - Uses pixel-based font (43) for absolute text sizes
// - labelScale multiplies base pixel sizes
TPad* PlotCMSPaperDiffResult(vector<TH1*> hists, vector<TH1*> topSystematics, vector<TH1*> bottomSystematics,
    const char* title, vector<string> labels, vector<Int_t> linecolors, vector<Int_t> linestyles,
    vector<Int_t> markercolors, vector<Int_t> markerstyles, const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax, const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logx = false, bool logy = false, bool errorBars = true, float xLegend = 0.55,
    float labelScale = 1.0, bool showXaxis = true)
{
    double border = 0.06;
    TPad *pad1 = new TPad(Form("p1_%s",title), title, border, 0.25 + border, 1.0 - border, 1.0 - border);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    pad1->Draw();
    TPad *pad2 = new TPad(Form("p2_%s",title), title, border, border, 1.0 - border, 0.25 + border);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    logx ? pad2->SetLogx() : pad2->SetLogx(0);
    pad2->Draw();

    SetTDRStyle();

    int baseLabelPx = (int)(18 * labelScale);
    int baseTitlePx = (int)(20 * labelScale);
    int baseLegPx   = (int)(16 * labelScale);

    TLegend* leg = new TLegend(xLegend, 0.7, xLegend+0.23, 0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(43);
    leg->SetTextSize(baseLegPx);

    double global_min = 1e30, global_max = -1e30;
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
        TH1* syst = (ih < topSystematics.size()) ? topSystematics[ih] : nullptr;
        double x1 = (xmin < xmax) ? xmin : hist->GetXaxis()->GetXmin();
        double x2 = (xmin < xmax) ? xmax : hist->GetXaxis()->GetXmax();
        int binmin = hist->GetXaxis()->FindBin(x1);
        int binmax = hist->GetXaxis()->FindBin(x2);
        for (int i = binmin; i <= binmax; ++i) {
            double value = hist->GetBinContent(i);
            double stat = errorBars ? hist->GetBinError(i) : 0;
            double systError = (syst != nullptr) ? fabs(syst->GetBinContent(i)) : 0;
            global_min = min(global_min, value - max(stat, systError));
            global_max = max(global_max, value + max(stat, systError));
        }
    }
    double margin;
    if (logy) {
        margin = exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2);
    } else {
        margin = 0.2 * (global_max - global_min);
    }

    double diff_min = 1e30, diff_max = -1e30;
    bool hasDifference = false;
    for (int i = 0; i < (int)hists.size(); i++) {
        if (i == baseline) continue;
        TH1 *hist = hists[i];
        TH1 *syst = (i < (int)bottomSystematics.size()) ? bottomSystematics[i] : nullptr;
        TH1 *diff = (TH1 *)hist->Clone(Form("range_%s_%d", title, i));
        diff->Add(hists[baseline], -1);
        setDifferenceErrors(diff, hist, hists[baseline]);
        for (int bin = 1; bin <= diff->GetNbinsX(); bin++) {
            double value = diff->GetBinContent(bin);
            double stat = errorBars ? diff->GetBinError(bin) : 0;
            double systError = (syst != nullptr) ? fabs(syst->GetBinContent(bin)) : 0;
            updateDiffRange(value, max(stat, systError), diff_min, diff_max);
            hasDifference = true;
        }
        delete diff;
    }
    if (!hasDifference) { diff_min = -1; diff_max = 1; }
    double diff_absmax = max(fabs(diff_min), fabs(diff_max));
    double diff_margin = 0.15 * diff_absmax;
    if (diff_margin <= 0) diff_margin = 0.15;
    diff_min = -(diff_absmax + diff_margin);
    diff_max =  (diff_absmax + diff_margin);

    bool firstDifference = true;
    for (int i = 0; i < (int)hists.size(); i++) {
        pad1->cd();
        TH1* hist = hists[i];
        TH1* topSyst = (i < (int)topSystematics.size()) ? topSystematics[i] : nullptr;

        if (linestyles[i] == -1) hist->SetLineColorAlpha(0, 0);
        else hist->SetLineColor(linecolors[i]);
        if (linestyles[i] == 0) hist->SetLineStyle(1);
        else if (linestyles[i] > 0) hist->SetLineStyle(linestyles[i]);
        hist->SetMarkerColor(markercolors[i]);
        hist->SetMarkerStyle(markerstyles[i]);
        hist->SetStats(0);
        if (linestyles[i] == 0) hist->SetLineWidth(3);
        else if (linestyles[i] > 0) hist->SetLineWidth(2);

        hist->GetXaxis()->SetTitle("");
        hist->GetXaxis()->SetLabelSize(0);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleFont(43);
        hist->GetYaxis()->SetTitleSize(baseTitlePx);
        hist->GetYaxis()->SetLabelFont(43);
        hist->GetYaxis()->SetLabelSize(baseLabelPx);
        hist->GetYaxis()->SetTitleOffset(1.4);

        if (ymin < ymax) {
            if (logy && ymin <= 0) hist->GetYaxis()->SetRangeUser(1, ymax);
            else hist->GetYaxis()->SetRangeUser(ymin, ymax);
        } else {
            if (logy && global_min - margin <= 0) hist->GetYaxis()->SetRangeUser(1, global_max + margin);
            else hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
        }

        if (linestyles[i] == 0) hist->Draw("SAME");
        else if (linestyles[i] == -1) hist->Draw("P SAME");
        else hist->Draw("HIST SAME");

        if (topSyst != nullptr) {
            TGraphAsymmErrors *band = BuildSystematicBand(hist, topSyst,
                Form("top_band_%s_%d", title, i), linecolors[i]);
            if (band != nullptr) band->Draw("2 SAME");
            if (linestyles[i] == 0) hist->Draw("SAME");
            else if (linestyles[i] == -1) hist->Draw("P SAME");
            else hist->Draw("HIST SAME");
        }
        if (errorBars) hist->Draw("E1 SAME");

        if (linestyles[i] == 0) leg->AddEntry(hist, labels[i].c_str(), "pl");
        else if (linestyles[i] == -1) leg->AddEntry(hist, labels[i].c_str(), "p");
        else leg->AddEntry(hist, labels[i].c_str(), "l");

        pad2->cd();
        if (i != baseline) {
            TH1* hDiff = (TH1*)hist->Clone(Form("diff_%s_%d", title, i));
            hDiff->Add(hists[baseline], -1);
            setDifferenceErrors(hDiff, hist, hists[baseline]);
            TH1 *bottomSyst = (i < (int)bottomSystematics.size()) ? bottomSystematics[i] : nullptr;

            if (showXaxis) {
                hDiff->GetXaxis()->SetTitle(xTitle);
                hDiff->GetXaxis()->SetTitleFont(43);
                hDiff->GetXaxis()->SetTitleSize(baseTitlePx);
                hDiff->GetXaxis()->SetLabelFont(43);
                hDiff->GetXaxis()->SetLabelSize(baseLabelPx);
                hDiff->GetXaxis()->SetTitleOffset(1.0);
            } else {
                hDiff->GetXaxis()->SetTitle("");
                hDiff->GetXaxis()->SetLabelSize(0);
            }
            hDiff->GetYaxis()->SetTitle(rTitle);
            if (rmin < rmax) hDiff->GetYaxis()->SetRangeUser(rmin, rmax);
            else hDiff->GetYaxis()->SetRangeUser(diff_min - diff_margin, diff_max + diff_margin);
            hDiff->GetYaxis()->SetTitleFont(43);
            hDiff->GetYaxis()->SetTitleSize(baseTitlePx);
            hDiff->GetYaxis()->SetLabelFont(43);
            hDiff->GetYaxis()->SetLabelSize(baseLabelPx);
            hDiff->GetYaxis()->SetTitleOffset(1.4);
            hDiff->SetLineColor(linecolors[i]);
            hDiff->SetLineStyle(linestyles[i] == 0 ? 1 : linestyles[i]);
            hDiff->SetMarkerColor(markercolors[i]);
            hDiff->SetMarkerStyle(markerstyles[i]);
            hDiff->SetLineWidth(2);

            TString diffDrawOption = errorBars ? "E1" : "P";
            if (!firstDifference) diffDrawOption += " SAME";
            hDiff->Draw(diffDrawOption);

            if (bottomSyst != nullptr) {
                TGraphAsymmErrors *band = BuildSystematicBand(hDiff, bottomSyst,
                    Form("bottom_band_%s_%d", title, i), linecolors[i]);
                if (band != nullptr) band->Draw("2 SAME");
            }
            if (errorBars) hDiff->Draw("E1 SAME");
            else hDiff->Draw("P SAME");

            double xlow = hDiff->GetXaxis()->GetBinLowEdge(hDiff->GetXaxis()->GetFirst());
            double xhigh = hDiff->GetXaxis()->GetBinUpEdge(hDiff->GetXaxis()->GetLast());
            TLine *line = new TLine(xlow, 0, xhigh, 0);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
            firstDifference = false;
        }
    }
    pad1->cd();
    leg->Draw("SAME");
    // No CMS header drawn — caller handles it

    return pad1;
}

TPad* plotCMSSimple2D(TCanvas* c, TH2* hist, const char* title,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* zTitle, double zmin, double zmax,
    bool logx = false, bool logy = false,
    bool logz = false,
    bool binnums = false) {

    gStyle->SetPalette(kRainbow);

    // Get the canvas pad to pass to other functions
    TPad* pad1 = (TPad*) c->GetPad(0);
    pad1->cd();
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    logz ? pad1->SetLogz() : pad1->SetLogz(0);
    pad1->SetRightMargin(10);
    
    // >>> Apply the CMS TDR style <<<
    SetTDRStyle();

    pad1->SetRightMargin(0.18); // Increase right margin for z-axis labels

    hist->GetZaxis()->SetTitle(zTitle);
    hist->GetXaxis()->SetTitle(xTitle);
    hist->GetXaxis()->SetRangeUser(xmin, xmax);
    hist->GetYaxis()->SetTitle(yTitle);
    hist->GetYaxis()->SetRangeUser(ymin, ymax);
    hist->GetZaxis()->SetTitle(zTitle);

    if (zmin < zmax) {
        hist->GetZaxis()->SetRangeUser(zmin, zmax);
    } else {
        // Auto-scale z axis
        double zmin_auto = hist->GetMinimum();
        double zmax_auto = hist->GetMaximum();
        hist->GetZaxis()->SetRangeUser(zmin_auto-0.2*abs(zmin_auto), zmax_auto+0.2*abs(zmax_auto));
    }

    if (binnums) {
        gStyle->SetPalette(kBird);
        gStyle->SetPaintTextFormat("1.2f");
        hist->Draw("COLZ text e");
    } else {
        hist->Draw("COLZ");
    }

    return pad1; // Return the pad for further customization if needed
}

void plotCMSTrifold(TCanvas* c, vector<vector<TH1D*>> hists, const char* title, vector<string> labels,
    vector<Int_t> linecolors, vector<Int_t> linestyles, vector<Int_t> markercolors, vector<Int_t> markerstyles,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    bool logx = false, bool logy = false, bool binnums = false)
{
    // >>> Apply the CMS TDR style <<<
    SetTDRStyle();

    // Divide canvas into 3 pads, flush together (no whitespace)
    c->Clear();
    c->Divide(3,1,0,0);

    // Pad margins: leftmost (0), middle (1), rightmost (2)
    double leftMargins[3]  = {0.0, 0.0, 0.0}; // Only rightmost has y label, so only it needs left margin
    double rightMargins[3] = {0.0, 0.0, 0.08}; // Only rightmost needs right margin for axis
    double yTitleOffsets[3] = {0.0, 0.0, 1.2}; // Only rightmost has y axis label

    for (int ipad = 0; ipad < 3; ++ipad) {
        TPad* pad = (TPad*)c->cd(ipad+1);
        pad->SetLeftMargin(leftMargins[ipad]);
        pad->SetRightMargin(rightMargins[ipad]);
        pad->SetBottomMargin(0.15);
        pad->SetTopMargin(0.08);
        logy ? pad->SetLogy() : pad->SetLogy(0);
        logx ? pad->SetLogx() : pad->SetLogx(0);

        // Find global min/max for all hists in this panel
        double global_min = 1e30, global_max = -1e30;
        for (auto hist : hists[ipad]) {
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
        double margin = logy ? exp((log(global_max) - log((global_min > 0) ? global_min : 1)) * 1.2)
                             : 0.2 * (global_max - global_min);

        // Draw all curves in this panel
        for (size_t ih = 0; ih < hists[ipad].size(); ++ih) {
            TH1D* hist = hists[ipad][ih];
            hist->SetStats(0);
            hist->SetTitle(ipad == 1 ? title : "");
            hist->GetXaxis()->SetTitle(ipad == 1 ? xTitle : "");
            // Only rightmost panel gets y axis label
            hist->GetYaxis()->SetTitle(ipad == 2 ? yTitle : "");
            hist->GetXaxis()->SetTitleSize(0.05);
            hist->GetXaxis()->SetLabelSize(0.045);
            hist->GetYaxis()->SetTitleSize(0.05);
            hist->GetYaxis()->SetLabelSize(0.04);
            hist->GetYaxis()->SetTitleOffset(yTitleOffsets[ipad]);

            // Axis ranges
            if (xmin < xmax) hist->GetXaxis()->SetRangeUser(xmin, xmax);
            if (ymin < ymax) {
                if (logy && ymin <= 0) hist->GetYaxis()->SetRangeUser(1, ymax);
                else hist->GetYaxis()->SetRangeUser(ymin, ymax);
            } else {
                if (logy && global_min-margin <= 0)
                    hist->GetYaxis()->SetRangeUser(1, global_max + margin);
                else
                    hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
            }

            hist->SetLineColor(linecolors[ih]);
            if (linestyles[ih] == 0) hist->SetLineStyle(1);
            else if (linestyles[ih] > 0) hist->SetLineStyle(linestyles[ih]);
            hist->SetMarkerColor(markercolors[ih]);
            hist->SetMarkerStyle(markerstyles[ih]);
            if (linestyles[ih] == 0) hist->SetLineWidth(3);
            else if (linestyles[ih] > 0) hist->SetLineWidth(2);

            if (ih == 0) hist->Draw("HIST");
            else hist->Draw("HIST SAME");
            if (binnums) hist->Draw("SAME TEXT0");
        }

        // Draw legend only on the last panel
        if (ipad == 2) {
            TLegend* leg = new TLegend(0.25, 0.7, 0.68, 0.85);
            leg->SetBorderSize(0);
            leg->SetFillStyle(0);
            leg->SetTextFont(42);
            leg->SetTextSize(0.035);
            for (size_t ih = 0; ih < hists[ipad].size(); ++ih) {
                leg->AddEntry(hists[ipad][ih], labels[ih].c_str(), "l");
            }
            leg->Draw("SAME");
        }
    }
}
