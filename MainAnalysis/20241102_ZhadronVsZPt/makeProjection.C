#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTree.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

struct JackknifeProjectionEvent {
    double SignalNZ = 0;
    double MixNZ = 0;
    vector<float> SignalBins2D;
    vector<float> MixBins2D;
    vector<float> SignalEta;
    vector<float> MixEta;
    vector<float> SignalPhi;
    vector<float> MixPhi;
};

struct ResultProjectionWindow {
    int DeltaPhiXFirst = 0;
    int DeltaPhiXLast = 0;
    int DeltaEtaYFirst = 1;
    int DeltaEtaYLast = 1;
};

enum class ResultGeometry {
    Official20x20,
    Shifted10x10,
    Modified12x12
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


// Check that boundary is an exact bin edge on axis (within tolerance).
bool axisHasExactEdge(const TAxis *axis, double boundary)
{
    if (axis == nullptr) return false;
    const double tolerance = std::max(1.0, std::abs(boundary)) * 1e-9;
    for (int i = 0; i <= axis->GetNbins(); ++i) {
        double edge = axis->GetBinLowEdge(i + 1);
        if (std::abs(edge - boundary) < tolerance) return true;
    }
    return false;
}

bool validateOfficial20BinAxes(const TH2D *input)
{
    if (input == nullptr)
        return false;
    if (input->GetNbinsX() != 20 || input->GetNbinsY() != 20) {
        std::cerr << "Error: official result path expects 20x20 histograms." << std::endl;
        return false;
    }
    return true;
}

void setOfficial20BinProjectionWindow(ResultProjectionWindow &window)
{
    window.DeltaPhiXFirst = 0;
    window.DeltaPhiXLast = 10;
    window.DeltaEtaYFirst = 6;
    window.DeltaEtaYLast = 10;
}

bool validateShifted10x10Axes(const TH2D *input)
{
    if (input == nullptr) {
        std::cerr << "Error: null histogram in shifted 10x10 projection validation." << std::endl;
        return false;
    }
    if (input->GetNbinsX() != 10 || input->GetNbinsY() != 10) {
        std::cerr << "Error (10-bin shifted): expected 10x10 histogram." << std::endl;
        return false;
    }
    const TAxis *xAxis = input->GetXaxis();
    const TAxis *yAxis = input->GetYaxis();
    bool ok = true;
    if (!axisHasExactEdge(xAxis, 0.0)) { std::cerr << "Error (10-bin shifted): DeltaEta axis has no exact edge at 0." << std::endl; ok = false; }
    if (!axisHasExactEdge(xAxis, 4.0)) { std::cerr << "Error (10-bin shifted): DeltaEta axis has no exact edge at 4." << std::endl; ok = false; }
    if (!axisHasExactEdge(yAxis, 0.0)) { std::cerr << "Error (10-bin shifted): DeltaPhi axis has no exact edge at 0." << std::endl; ok = false; }
    if (!axisHasExactEdge(yAxis, M_PI)) { std::cerr << "Error (10-bin shifted): DeltaPhi axis has no exact edge at pi." << std::endl; ok = false; }
    return ok;
}

void setShifted10x10ProjectionWindow(ResultProjectionWindow &window)
{
    window.DeltaPhiXFirst = 6;
    window.DeltaPhiXLast  = 10;
    window.DeltaEtaYFirst = 4;
    window.DeltaEtaYLast  = 8;
}

bool validateModified12x12Axes(const TH2D *input)
{
    if (input == nullptr) {
        std::cerr << "Error: null histogram in modified 12x12 projection validation." << std::endl;
        return false;
    }
    if (input->GetNbinsX() != 12 || input->GetNbinsY() != 12) {
        std::cerr << "Error (12-bin modified): expected 12x12 histogram." << std::endl;
        return false;
    }
    const TAxis *xAxis = input->GetXaxis();
    const TAxis *yAxis = input->GetYaxis();
    bool ok = true;
    if (!axisHasExactEdge(xAxis, 0.0)) { std::cerr << "Error (12-bin modified): DeltaEta axis has no exact edge at 0." << std::endl; ok = false; }
    if (!axisHasExactEdge(xAxis, 4.0)) { std::cerr << "Error (12-bin modified): DeltaEta axis has no exact edge at 4." << std::endl; ok = false; }
    if (!axisHasExactEdge(yAxis, 0.0)) { std::cerr << "Error (12-bin modified): DeltaPhi axis has no exact edge at 0." << std::endl; ok = false; }
    if (!axisHasExactEdge(yAxis, M_PI / 2)) { std::cerr << "Error (12-bin modified): DeltaPhi axis has no exact edge at pi/2." << std::endl; ok = false; }
    if (!axisHasExactEdge(yAxis, M_PI)) { std::cerr << "Error (12-bin modified): DeltaPhi axis has no exact edge at pi." << std::endl; ok = false; }
    return ok;
}

void setModified12x12ProjectionWindow(ResultProjectionWindow &window)
{
    window.DeltaPhiXFirst = 7;
    window.DeltaPhiXLast  = 12;
    window.DeltaEtaYFirst = 4;
    window.DeltaEtaYLast  = 6;
}

int getFlatIndex(int xBin, int yBin, int nYBins)
{
    return (xBin - 1) * nYBins + (yBin - 1);
}

void fillProjectedContributions(const vector<float> &input, int nXBins, int nYBins,
    int deltaPhiXFirst, int deltaPhiXLast, int deltaEtaYFirst, int deltaEtaYLast,
    vector<float> &etaBins, vector<float> &phiBins)
{
    etaBins.assign(nXBins, 0);
    phiBins.assign(nYBins, 0);

    for (int xBin = 1; xBin <= nXBins; ++xBin) {
        for (int yBin = 1; yBin <= nYBins; ++yBin) {
            float value = input[getFlatIndex(xBin, yBin, nYBins)];
            if (yBin >= deltaEtaYFirst && yBin <= deltaEtaYLast)
                etaBins[xBin - 1] += value;
            if (xBin >= deltaPhiXFirst && xBin <= deltaPhiXLast)
                phiBins[yBin - 1] += value;
        }
    }
}

bool loadJackknifeEvents(TFile *file, int sourceXBins, int sourceYBins, int targetXBins,
    int targetYBins, int deltaPhiXFirst, int deltaPhiXLast, int deltaEtaYFirst, int deltaEtaYLast,
    vector<JackknifeProjectionEvent> &events)
{
    TTree *jackknifeTree = (TTree *)file->Get("Jackknife2DData");
    if (jackknifeTree == nullptr)
        return false;

    double signalNZ = 0;
    double mixNZ = 0;
    vector<float> *signalBins = nullptr;
    vector<float> *mixBins = nullptr;

    jackknifeTree->SetBranchAddress("SignalNZ", &signalNZ);
    jackknifeTree->SetBranchAddress("MixNZ", &mixNZ);
    jackknifeTree->SetBranchAddress("SignalBins", &signalBins);
    jackknifeTree->SetBranchAddress("MixBins", &mixBins);

    Long64_t entryCount = jackknifeTree->GetEntries();
    events.clear();
    events.reserve(entryCount);

    for (Long64_t entry = 0; entry < entryCount; ++entry) {
        jackknifeTree->GetEntry(entry);
        JackknifeProjectionEvent current;
        current.SignalNZ = signalNZ;
        current.MixNZ = mixNZ;
        if (sourceXBins != targetXBins || sourceYBins != targetYBins) {
            std::cerr << "Error: makeProjection no longer supports variable result-stage rebinning."
                << " Input is " << sourceXBins << "x" << sourceYBins
                << " while projection expects " << targetXBins << "x" << targetYBins << "." << std::endl;
            return false;
        }
        current.SignalBins2D = *signalBins;
        current.MixBins2D = *mixBins;
        fillProjectedContributions(current.SignalBins2D, targetXBins, targetYBins,
            deltaPhiXFirst, deltaPhiXLast, deltaEtaYFirst, deltaEtaYLast,
            current.SignalEta, current.SignalPhi);
        fillProjectedContributions(current.MixBins2D, targetXBins, targetYBins,
            deltaPhiXFirst, deltaPhiXLast, deltaEtaYFirst, deltaEtaYLast,
            current.MixEta, current.MixPhi);
        events.push_back(current);
    }

    return events.empty() == false;
}

vector<double> computeJackknifeSigma2D(const vector<JackknifeProjectionEvent> &events,
    const TH2D *signalHistogram, const TH2D *mixHistogram, double totalSignalNZ, double totalMixNZ)
{
    vector<double> sigma(signalHistogram->GetNbinsX() * signalHistogram->GetNbinsY(), 0);
    if (events.size() < 2 || totalSignalNZ <= 0 || totalMixNZ <= 0)
        return sigma;

    int validEvents = 0;
    for (const JackknifeProjectionEvent &event : events) {
        if (totalSignalNZ - event.SignalNZ <= 0)
            continue;
        if (totalMixNZ - event.MixNZ <= 0)
            continue;
        validEvents = validEvents + 1;
    }
    if (validEvents < 2)
        return sigma;

    for (int xBin = 1; xBin <= signalHistogram->GetNbinsX(); ++xBin) {
        for (int yBin = 1; yBin <= signalHistogram->GetNbinsY(); ++yBin) {
            int flatIndex = getFlatIndex(xBin, yBin, signalHistogram->GetNbinsY());
            double fullSignal = signalHistogram->GetBinContent(xBin, yBin);
            double fullMix = mixHistogram->GetBinContent(xBin, yBin);
            double fullValue = fullSignal - fullMix;
            double varianceSum = 0;

            for (const JackknifeProjectionEvent &event : events) {
                if (totalSignalNZ - event.SignalNZ <= 0)
                    continue;
                if (totalMixNZ - event.MixNZ <= 0)
                    continue;

                double signalWithoutEvent = (fullSignal * totalSignalNZ - event.SignalBins2D[flatIndex]) / (totalSignalNZ - event.SignalNZ);
                double mixWithoutEvent = (fullMix * totalMixNZ - event.MixBins2D[flatIndex]) / (totalMixNZ - event.MixNZ);
                double valueWithoutEvent = signalWithoutEvent - mixWithoutEvent;
                varianceSum += (valueWithoutEvent - fullValue) * (valueWithoutEvent - fullValue);
            }

            sigma[flatIndex] = std::sqrt((validEvents - 1.0) / validEvents * varianceSum);
        }
    }

    return sigma;
}

vector<double> computeProjectedJackknifeSigma(const vector<JackknifeProjectionEvent> &events,
    const vector<double> &fullSignal, const vector<double> &fullMix,
    const TH1D *fullHistogram, double totalSignalNZ, double totalMixNZ, bool useEta)
{
    vector<double> sigma(fullHistogram->GetNbinsX(), 0);
    if (events.size() < 2 || totalSignalNZ <= 0 || totalMixNZ <= 0)
        return sigma;

    int validEvents = 0;
    for (const JackknifeProjectionEvent &event : events) {
        if (totalSignalNZ - event.SignalNZ <= 0)
            continue;
        if (totalMixNZ - event.MixNZ <= 0)
            continue;
        validEvents = validEvents + 1;
    }
    if (validEvents < 2)
        return sigma;

    for (int i = 1; i <= fullHistogram->GetNbinsX(); ++i) {
        double fullValue = fullSignal[i - 1] / totalSignalNZ - fullMix[i - 1] / totalMixNZ;
        double varianceSum = 0;

        for (const JackknifeProjectionEvent &event : events) {
            if (totalSignalNZ - event.SignalNZ <= 0)
                continue;
            if (totalMixNZ - event.MixNZ <= 0)
                continue;

            const vector<float> &signalSource = useEta ? event.SignalEta : event.SignalPhi;
            const vector<float> &mixSource = useEta ? event.MixEta : event.MixPhi;
            double signalWithoutEvent = (fullSignal[i - 1] - signalSource[i - 1]) / (totalSignalNZ - event.SignalNZ);
            double mixWithoutEvent = (fullMix[i - 1] - mixSource[i - 1]) / (totalMixNZ - event.MixNZ);
            double valueWithoutEvent = signalWithoutEvent - mixWithoutEvent;
            varianceSum += (valueWithoutEvent - fullValue) * (valueWithoutEvent - fullValue);
        }

        sigma[i - 1] = std::sqrt((validEvents - 1.0) / validEvents * varianceSum);
    }

    return sigma;
}

void writeJackknifeProjectionTree(const vector<JackknifeProjectionEvent> &events, const char *tag)
{
    double signalNZ = 0;
    double mixNZ = 0;
    vector<float> signalEta;
    vector<float> mixEta;
    vector<float> signalPhi;
    vector<float> mixPhi;

    TTree jackknifeProjection(Form("JackknifeProjection%s", tag), "Projected jackknife contributions");
    jackknifeProjection.Branch("SignalNZ", &signalNZ);
    jackknifeProjection.Branch("MixNZ", &mixNZ);
    jackknifeProjection.Branch("SignalEta", &signalEta);
    jackknifeProjection.Branch("MixEta", &mixEta);
    jackknifeProjection.Branch("SignalPhi", &signalPhi);
    jackknifeProjection.Branch("MixPhi", &mixPhi);

    for (const JackknifeProjectionEvent &event : events) {
        signalNZ = event.SignalNZ;
        mixNZ = event.MixNZ;
        signalEta = event.SignalEta;
        mixEta = event.MixEta;
        signalPhi = event.SignalPhi;
        mixPhi = event.MixPhi;
        jackknifeProjection.Fill();
    }

    jackknifeProjection.Write();
}

void makeProjectionInternal(const char *infname, const char *outfname, const char *tag,
    bool doSub, ResultGeometry geometry) {
    TFile *file = new TFile(infname, "READ");

    TH2D *hData = (TH2D*)file->Get("hData");
    TH1D *hNZData = (TH1D*)file->Get("hNZData");
    TH2D *hMixData = (TH2D*)file->Get("hMixData");
    TH1D *hNZMixData = (TH1D*)file->Get("hNZMixData");
    TH3D *hTrkPtEtaPhi = (TH3D*)file->Get("hTrkPtEtaPhiData");
    TH3D *hZPtEtaPhi = (TH3D*)file->Get("hZPtEtaPhiData");
    TH1D *hVZ = (TH1D*)file->Get("hVZData");
    TH1D *hZmass = (TH1D*)file->Get("hZmassData");
    TH3D *hTrkResidualCorrectionPtEtaPhi = (TH3D*)file->Get("hTrkResidualCorrectionPtEtaPhiData");

    hNZData->SetName(Form("hNZData_%s",tag));
    hNZMixData->SetName(Form("hNZMixData_%s",tag));
    hVZ->SetName(Form("hVZ_%s",tag));

    if (!hData || !hNZData || !hMixData || !hNZMixData) {
        if (!hData)
            std::cerr << "Error: Histogram 'hData' not found in file. Ensure the histogram is correctly loaded and the file path is accurate." << std::endl;
        if (!hNZData)
            std::cerr << "Error: Histogram 'hNZData' not found in file. Verify that this histogram is defined in the input file." << std::endl;
        if (!hMixData)
            std::cerr << "Error: Histogram 'hMixData' not found in file. Check that the histogram name matches what is expected." << std::endl;
        if (!hNZMixData)
            std::cerr << "Error: Histogram 'hNZMixData' not found in file. Confirm that this histogram is included in the file." << std::endl;
        std::cerr << "Error: One or more required histograms could not be loaded from the file. Exiting function." << std::endl;
        return;
    }

    ResultProjectionWindow projectionWindow;
    if (geometry == ResultGeometry::Shifted10x10) {
        if (validateShifted10x10Axes(hData) == false) {
            std::cerr << "Error: shifted 10x10 projection validation failed. Exiting." << std::endl;
            return;
        }
        setShifted10x10ProjectionWindow(projectionWindow);
    }
    else if (geometry == ResultGeometry::Modified12x12) {
        if (validateModified12x12Axes(hData) == false) {
            std::cerr << "Error: modified 12x12 projection validation failed. Exiting." << std::endl;
            return;
        }
        setModified12x12ProjectionWindow(projectionWindow);
    }
    else {
        if (validateOfficial20BinAxes(hData) == false) {
            std::cerr << "Error: official 20-bin projection validation failed. Exiting." << std::endl;
            return;
        }
        setOfficial20BinProjectionWindow(projectionWindow);
    }

    vector<JackknifeProjectionEvent> jackknifeEvents;
    bool hasJackknife = loadJackknifeEvents(file, hData->GetNbinsX(), hData->GetNbinsY(),
        hData->GetNbinsX(), hData->GetNbinsY(),
        projectionWindow.DeltaPhiXFirst, projectionWindow.DeltaPhiXLast,
        projectionWindow.DeltaEtaYFirst, projectionWindow.DeltaEtaYLast,
        jackknifeEvents);

    hData->SetName(Form("hData_%s",tag));
    hData->Scale(1. / hNZData->GetBinContent(1));
    cout << "NZs: " << hNZData->GetBinContent(1) << endl;

    hMixData->SetName(Form("hMixData_%s",tag));
    hMixData->Scale(1. / hNZMixData->GetBinContent(1));
    TH2D *hDataAll = (TH2D*)hData->Clone(Form("hDataAll_%s",tag));
    TH2D *hDataSumw2 = nullptr;

    hTrkPtEtaPhi->SetName(Form("hTrkPtEtaPhiData_%s",tag));
    hTrkPtEtaPhi->Scale(1. / hNZData->GetBinContent(1));

    hZPtEtaPhi->SetName(Form("hZPtEtaPhi_%s",tag));
    hZPtEtaPhi->Scale(1. / hNZData->GetBinContent(1));

    hZmass->SetName(Form("hZmassData_%s",tag));

    hTrkResidualCorrectionPtEtaPhi->SetName(Form("hTrkResidualCorrectionPtEtaPhi_%s",tag));
    hTrkResidualCorrectionPtEtaPhi->Scale(1. / hNZData->GetBinContent(1));

    if (doSub) {
        hData->Add(hMixData, -1);
        hDataSumw2 = (TH2D*)hData->Clone(Form("hData_Sumw2_%s",tag));
    }

    double totalSignalNZ = 0;
    double totalMixNZ = 0;
    if (hasJackknife) {
        for (const JackknifeProjectionEvent &event : jackknifeEvents) {
            totalSignalNZ += event.SignalNZ;
            totalMixNZ += event.MixNZ;
        }
    }

    if (doSub && hasJackknife) {
        vector<double> sigma2D = computeJackknifeSigma2D(jackknifeEvents, hDataAll, hMixData, totalSignalNZ, totalMixNZ);
        for (int xBin = 1; xBin <= hData->GetNbinsX(); ++xBin) {
            for (int yBin = 1; yBin <= hData->GetNbinsY(); ++yBin) {
                int flatIndex = getFlatIndex(xBin, yBin, hData->GetNbinsY());
                hData->SetBinError(xBin, yBin, sigma2D[flatIndex]);
            }
        }
    }

    TCanvas *c1 = new TCanvas("c1", "Canvas for Y projection", 800, 600);
    TH1D *hProjY = (TH1D*)hData->ProjectionY(Form("DeltaPhi_Result%s",tag),
        projectionWindow.DeltaPhiXFirst, projectionWindow.DeltaPhiXLast);
    TH1D *hProjYSumw2 = (TH1D*)hProjY->Clone(Form("DeltaPhi_ResultSumw2%s",tag));
    if (doSub && hasJackknife) {
        vector<double> fullSignal(hProjY->GetNbinsX(), 0);
        vector<double> fullMix(hProjY->GetNbinsX(), 0);
        for (const JackknifeProjectionEvent &event : jackknifeEvents) {
            for (int i = 0; i < hProjY->GetNbinsX(); ++i) {
                fullSignal[i] += event.SignalPhi[i];
                fullMix[i] += event.MixPhi[i];
            }
        }
        vector<double> sigmaPhi = computeProjectedJackknifeSigma(jackknifeEvents, fullSignal, fullMix, hProjY, totalSignalNZ, totalMixNZ, false);
        for (int i = 1; i <= hProjY->GetNbinsX(); ++i)
            hProjY->SetBinError(i, sigmaPhi[i - 1]);
    }
    hProjY->SetMarkerStyle(20);
    divideByWidth(hProjY);
    divideByWidth(hProjYSumw2);
    hProjY->Draw();

    TCanvas *c2 = new TCanvas("c2", "Canvas for X projection", 800, 600);
    TH1D *hProjX = (TH1D*)hData->ProjectionX(Form("DeltaEta_Result%s",tag),
        projectionWindow.DeltaEtaYFirst, projectionWindow.DeltaEtaYLast);
    TH1D *hProjXSumw2 = (TH1D*)hProjX->Clone(Form("DeltaEta_ResultSumw2%s",tag));
    if (doSub && hasJackknife) {
        vector<double> fullSignal(hProjX->GetNbinsX(), 0);
        vector<double> fullMix(hProjX->GetNbinsX(), 0);
        for (const JackknifeProjectionEvent &event : jackknifeEvents) {
            for (int i = 0; i < hProjX->GetNbinsX(); ++i) {
                fullSignal[i] += event.SignalEta[i];
                fullMix[i] += event.MixEta[i];
            }
        }
        vector<double> sigmaEta = computeProjectedJackknifeSigma(jackknifeEvents, fullSignal, fullMix, hProjX, totalSignalNZ, totalMixNZ, true);
        for (int i = 1; i <= hProjX->GetNbinsX(); ++i)
            hProjX->SetBinError(i, sigmaEta[i - 1]);
    }
    hProjX->SetMarkerStyle(20);
    hProjX->GetXaxis()->SetTitle("#Delta#eta");
    divideByWidth(hProjX);
    divideByWidth(hProjXSumw2);
    hProjX->Draw();

    TFile *outf = new TFile(outfname, "RECREATE");
    hData->Write();
    if (hDataSumw2 != nullptr)
        hDataSumw2->Write();
    hDataAll->Write();
    hMixData->Write();
    hProjY->Write();
    if (doSub)
        hProjYSumw2->Write();
    hProjX->Write();
    if (doSub)
        hProjXSumw2->Write();
    hNZData->Write();
    hNZMixData->Write();
    hTrkPtEtaPhi->Write();
    hZPtEtaPhi->Write();
    hVZ->Write();
    hZmass->Write();
    hTrkResidualCorrectionPtEtaPhi->Write();
    if (hasJackknife)
        writeJackknifeProjectionTree(jackknifeEvents, tag);
}

void makeProjection(const char *infname="output.root", const char *outfname="result.root",
    const char *tag="", bool doSub=1)
{
    makeProjectionInternal(infname, outfname, tag, doSub, ResultGeometry::Official20x20);
}

void makeProjectionShifted10x10(const char *infname="output.root", const char *outfname="result.root",
    const char *tag="", bool doSub=1)
{
    makeProjectionInternal(infname, outfname, tag, doSub, ResultGeometry::Shifted10x10);
}

void makeProjectionModified12x12(const char *infname="output.root", const char *outfname="result.root",
    const char *tag="", bool doSub=1)
{
    makeProjectionInternal(infname, outfname, tag, doSub, ResultGeometry::Modified12x12);
}

int main() {
    makeProjection();
    return 0;
}
