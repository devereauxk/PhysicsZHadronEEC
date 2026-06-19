#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

struct ResultProjectionWindow {
    int DeltaPhiXFirst = 0;
    int DeltaPhiXLast = 0;
    int DeltaEtaYFirst = 1;
    int DeltaEtaYLast = 1;
};

int findLastFullBinAtOrBelow(const TAxis *axis, double boundary)
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

int findFirstFullBinAbove(const TAxis *axis, double boundary)
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

ResultProjectionWindow getProjectionWindow(const TH2D *input)
{
    ResultProjectionWindow window;
    if (input == nullptr)
        return window;

    window.DeltaPhiXFirst = 0;
    window.DeltaPhiXLast = findLastFullBinAtOrBelow(input->GetXaxis(), 0.0);
    window.DeltaEtaYFirst = findFirstFullBinAbove(input->GetYaxis(), 0.0);
    window.DeltaEtaYLast = findLastFullBinAtOrBelow(input->GetYaxis(), M_PI / 2);
    return window;
}

void divideByWidth(TH1D *histogram)
{
    for (int bin = 1; bin <= histogram->GetNbinsX(); ++bin) {
        const double width = histogram->GetBinWidth(bin);
        if (width == 0)
            continue;
        histogram->SetBinContent(bin, histogram->GetBinContent(bin) / width);
        histogram->SetBinError(bin, histogram->GetBinError(bin) / width);
    }
}

TH1D *loadDetachedTH1(TFile &file, const string &name)
{
    TH1D *histogram = (TH1D *)file.Get(name.c_str());
    if (histogram == nullptr)
        return nullptr;
    histogram = (TH1D *)histogram->Clone((name + "_clone").c_str());
    histogram->SetDirectory(nullptr);
    return histogram;
}

TH2D *loadDetachedTH2(TFile &file, const string &name)
{
    TH2D *histogram = (TH2D *)file.Get(name.c_str());
    if (histogram == nullptr)
        return nullptr;
    histogram = (TH2D *)histogram->Clone((name + "_clone").c_str());
    histogram->SetDirectory(nullptr);
    return histogram;
}

double computeDisplayedIntegral(TH2D *signal, TH2D *mix, bool subtract, ResultProjectionWindow *windowOut = nullptr)
{
    ResultProjectionWindow window = getProjectionWindow(signal);
    if (windowOut != nullptr)
        *windowOut = window;

    TH2D *finalHistogram = (TH2D *)signal->Clone("finalHistogram");
    if (subtract)
        finalHistogram->Add(mix, -1);

    TH1D *projection = (TH1D *)finalHistogram->ProjectionY("projection",
        window.DeltaPhiXFirst, window.DeltaPhiXLast);
    divideByWidth(projection);
    projection->Scale(0.5);
    const double integral = projection->Integral("width");

    delete projection;
    delete finalHistogram;
    return integral;
}

double computePPIntegral(const string &baseDir, const string &tag, const string &zpt, const string &trkpt,
    bool subtract, ResultProjectionWindow *windowOut = nullptr)
{
    TFile file((baseDir + "/pp_trkResidual_" + tag + "_ZPT" + zpt + "-nosub.root").c_str(), "READ");
    TH2D *signal = loadDetachedTH2(file, "hData_" + trkpt);
    TH2D *mix = loadDetachedTH2(file, "hMixData_" + trkpt);
    if (signal == nullptr || mix == nullptr)
        return std::numeric_limits<double>::quiet_NaN();

    const double integral = computeDisplayedIntegral(signal, mix, subtract, windowOut);
    delete signal;
    delete mix;
    return integral;
}

double computeCombinedHIIntegral(const string &baseDir, const string &tag, const string &zpt, const string &trkpt,
    bool subtract, ResultProjectionWindow *windowOut = nullptr)
{
    TFile ppbFile((baseDir + "/pPb_trkResidual_" + tag + "_ZPT" + zpt + "-nosub.root").c_str(), "READ");
    TFile pbpFile((baseDir + "/PbP_trkResidual_" + tag + "_ZPT" + zpt + "-nosub.root").c_str(), "READ");

    TH2D *signalPPb = loadDetachedTH2(ppbFile, "hData_" + trkpt);
    TH2D *mixPPb = loadDetachedTH2(ppbFile, "hMixData_" + trkpt);
    TH1D *signalNZPPb = loadDetachedTH1(ppbFile, "hNZData_" + trkpt);
    TH1D *mixNZPPb = loadDetachedTH1(ppbFile, "hNZMixData_" + trkpt);
    TH2D *signalPbP = loadDetachedTH2(pbpFile, "hData_" + trkpt);
    TH2D *mixPbP = loadDetachedTH2(pbpFile, "hMixData_" + trkpt);
    TH1D *signalNZPbP = loadDetachedTH1(pbpFile, "hNZData_" + trkpt);
    TH1D *mixNZPbP = loadDetachedTH1(pbpFile, "hNZMixData_" + trkpt);

    signalPPb->Scale(signalNZPPb->GetBinContent(1));
    mixPPb->Scale(mixNZPPb->GetBinContent(1));
    signalPbP->Scale(signalNZPbP->GetBinContent(1));
    mixPbP->Scale(mixNZPbP->GetBinContent(1));

    TH2D *signal = (TH2D *)signalPPb->Clone("signalCombined");
    signal->Add(signalPbP);
    signal->Scale(1.0 / (signalNZPPb->GetBinContent(1) + signalNZPbP->GetBinContent(1)));

    TH2D *mix = (TH2D *)mixPPb->Clone("mixCombined");
    mix->Add(mixPbP);
    mix->Scale(1.0 / (mixNZPPb->GetBinContent(1) + mixNZPbP->GetBinContent(1)));

    const double integral = computeDisplayedIntegral(signal, mix, subtract, windowOut);

    delete signalPPb;
    delete mixPPb;
    delete signalNZPPb;
    delete mixNZPPb;
    delete signalPbP;
    delete mixPbP;
    delete signalNZPbP;
    delete mixNZPbP;
    delete signal;
    delete mix;
    return integral;
}

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string ppTag = CL.Get("PPTag", "EEV5_ZV9_trkV27_nmix10");
    const string hiTag = CL.Get("HITag", "ZV9_trkV27_nmix10");
    const string zpt = CL.Get("ZPT", "30_500");
    const string trkpt = CL.Get("TrackPT", "4_15");
    const string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");

    ResultProjectionWindow ppWindow;
    ResultProjectionWindow hiWindow;
    const double ppRaw = computePPIntegral(baseDir, ppTag, zpt, trkpt, false, &ppWindow);
    const double ppSub = computePPIntegral(baseDir, ppTag, zpt, trkpt, true, nullptr);
    const double hiRaw = computeCombinedHIIntegral(baseDir, hiTag, zpt, trkpt, false, &hiWindow);
    const double hiSub = computeCombinedHIIntegral(baseDir, hiTag, zpt, trkpt, true, nullptr);

    cout << "Selection: ZPT" << zpt << ", trkPT" << trkpt << endl;
    cout << "PP DeltaPhi window: x bins " << ppWindow.DeltaPhiXFirst << ".." << ppWindow.DeltaPhiXLast
        << ", DeltaEta window: y bins " << ppWindow.DeltaEtaYFirst << ".." << ppWindow.DeltaEtaYLast << endl;
    cout << "HI DeltaPhi window: x bins " << hiWindow.DeltaPhiXFirst << ".." << hiWindow.DeltaPhiXLast
        << ", DeltaEta window: y bins " << hiWindow.DeltaEtaYFirst << ".." << hiWindow.DeltaEtaYLast << endl;
    cout << "PP raw integral = " << ppRaw << endl;
    cout << "PP subtracted integral = " << ppSub << endl;
    cout << "PP residual fraction = " << (ppRaw != 0 ? fabs(ppSub) / fabs(ppRaw) : 0) << endl;
    cout << "Combined HI raw integral = " << hiRaw << endl;
    cout << "Combined HI subtracted integral = " << hiSub << endl;
    cout << "Combined HI residual fraction = " << (hiRaw != 0 ? fabs(hiSub) / fabs(hiRaw) : 0) << endl;

    return 0;
}
