#include <TFile.h>
#include <TH1D.h>
#include <TMath.h>
#include <TSystem.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../../CommonCode/include/CommandLine.h"

using namespace std;

struct CompatibilityResult {
    string ObservableLabel;
    string ErrorModel;
    int TotalBins = 0;
    int UsedBins = 0;
    double Chi2 = 0;
    double Chi2PerNDF = 0;
    double PValue = 0;
};

CompatibilityResult computeCompatibility(TH1D *pPb, TH1D *PbP,
    const string &observableLabel, const string &errorModel)
{
    CompatibilityResult result;
    result.ObservableLabel = observableLabel;
    result.ErrorModel = errorModel;
    if (pPb == nullptr || PbP == nullptr)
        return result;

    result.TotalBins = min(pPb->GetNbinsX(), PbP->GetNbinsX());
    for (int bin = 1; bin <= result.TotalBins; ++bin) {
        const double sigma2 = pPb->GetBinError(bin) * pPb->GetBinError(bin)
            + PbP->GetBinError(bin) * PbP->GetBinError(bin);
        if (sigma2 <= 0)
            continue;

        const double delta = pPb->GetBinContent(bin) - PbP->GetBinContent(bin);
        result.Chi2 += delta * delta / sigma2;
        result.UsedBins = result.UsedBins + 1;
    }

    if (result.UsedBins > 0) {
        result.Chi2PerNDF = result.Chi2 / result.UsedBins;
        result.PValue = TMath::Prob(result.Chi2, result.UsedBins);
    }

    return result;
}

TH1D *loadHistogram(TFile &file, const string &name, const string &cloneName)
{
    TH1D *histogram = (TH1D *)file.Get(name.c_str());
    if (histogram == nullptr)
        return nullptr;

    histogram = (TH1D *)histogram->Clone(cloneName.c_str());
    histogram->SetDirectory(nullptr);
    return histogram;
}

void writeMarkdown(const string &path, const vector<CompatibilityResult> &results)
{
    ofstream out(path);
    out << "# pPb vs Pbp compatibility: Sumw2 vs jackknife\n\n";
    out << "| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |\n";
    out << "| --- | --- | ---: | ---: | ---: | ---: | ---: |\n";
    out << setprecision(17);
    for (const CompatibilityResult &result : results)
        out << "| `" << result.ObservableLabel << "` | **" << result.ErrorModel << "** | "
            << result.UsedBins << " | " << result.Chi2 << " | " << result.UsedBins << " | "
            << result.Chi2PerNDF << " | " << result.PValue << " |\n";
    out << "\n";
    for (const CompatibilityResult &result : results)
        out << "- `" << result.ObservableLabel << "` / **" << result.ErrorModel << "** excluded "
            << (result.TotalBins - result.UsedBins) << " bins because the combined variance was zero.\n";
}

void writeTSV(const string &path, const vector<CompatibilityResult> &results)
{
    ofstream out(path);
    out << "Observable\tErrorModel\tN_bins\tchi2\tndf\tchi2_per_ndf\tp_value\tExcludedBins\n";
    out << setprecision(17);
    for (const CompatibilityResult &result : results)
        out << result.ObservableLabel << '\t' << result.ErrorModel << '\t' << result.UsedBins << '\t'
            << result.Chi2 << '\t' << result.UsedBins << '\t' << result.Chi2PerNDF << '\t'
            << result.PValue << '\t' << (result.TotalBins - result.UsedBins) << '\n';
}

void writeTeX(const string &path, const vector<CompatibilityResult> &results)
{
    ofstream out(path);
    out << "\\begin{table}[htbp]\n";
    out << "\\centering\n";
    out << "\\begin{tabular}{|l|l|r|r|r|r|r|}\n";
    out << "\\hline\n";
    out << "Observable & Error model & $N_{\\mathrm{bins}}$ & $\\chi^2$ & ndf & $\\chi^2/\\mathrm{ndf}$ & p-value \\\\\n";
    out << "\\hline\n";
    out << setprecision(17);
    for (const CompatibilityResult &result : results)
        out << "\\texttt{" << result.ObservableLabel << "} & " << result.ErrorModel << " & "
            << result.UsedBins << " & " << result.Chi2 << " & " << result.UsedBins << " & "
            << result.Chi2PerNDF << " & " << result.PValue << " \\\\\n";
    out << "\\hline\n";
    out << "\\end{tabular}\n";
    out << "\\caption{pPb vs Pbp compatibility comparison using nominal Sumw2 and jackknife statistical uncertainties.}\n";
    out << "\\label{tab:ppbp-sumw2-vs-jackknife-compatibility}\n";
    out << "\\end{table}\n";
}

int main(int argc, char *argv[])
{
    CommandLine CL(argc, argv);

    const string tag = CL.Get("Tag", "ZV9_trkV27_nmix10");
    const string baseDir = CL.Get("BaseDir",
        "/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots");
    const string outputDir = CL.Get("OutputDir", "output");

    const string pPbFileName = baseDir + "/pPb_trkResidual_" + tag + "_ZPT0_500-result.root";
    const string PbPFileName = baseDir + "/PbP_trkResidual_" + tag + "_ZPT0_500-result.root";

    TFile pPbFile(pPbFileName.c_str(), "READ");
    TFile PbPFile(PbPFileName.c_str(), "READ");
    if (pPbFile.IsZombie() || PbPFile.IsZombie()) {
        cerr << "Failed to open compatibility inputs:\n"
            << "  " << pPbFileName << "\n"
            << "  " << PbPFileName << endl;
        return 1;
    }

    vector<CompatibilityResult> results;
    vector<pair<string, string>> histogramPairs = {
        {"DeltaEta_Result0.5_15", "DeltaEta_ResultSumw20.5_15"},
        {"DeltaPhi_Result0.5_15", "DeltaPhi_ResultSumw20.5_15"}
    };

    for (const auto &[jackknifeName, sumw2Name] : histogramPairs) {
        TH1D *pPbJackknife = loadHistogram(pPbFile, jackknifeName, "pPb_" + jackknifeName);
        TH1D *PbPJackknife = loadHistogram(PbPFile, jackknifeName, "PbP_" + jackknifeName);
        TH1D *pPbSumw2 = loadHistogram(pPbFile, sumw2Name, "pPb_" + sumw2Name);
        TH1D *PbPSumw2 = loadHistogram(PbPFile, sumw2Name, "PbP_" + sumw2Name);

        if (pPbJackknife == nullptr || PbPJackknife == nullptr || pPbSumw2 == nullptr || PbPSumw2 == nullptr) {
            cerr << "Missing compatibility histograms for " << jackknifeName << endl;
            return 1;
        }

        results.push_back(computeCompatibility(pPbSumw2, PbPSumw2, jackknifeName, "Sumw2"));
        results.push_back(computeCompatibility(pPbJackknife, PbPJackknife, jackknifeName, "Jackknife"));
    }

    gSystem->mkdir(outputDir.c_str(), true);
    writeMarkdown(outputDir + "/pPbPbp_sumw2_vs_jackknife_compatibility.md", results);
    writeTSV(outputDir + "/pPbPbp_sumw2_vs_jackknife_compatibility.tsv", results);
    writeTeX(outputDir + "/pPbPbp_sumw2_vs_jackknife_compatibility.tex", results);

    for (const CompatibilityResult &result : results)
        cout << result.ObservableLabel << " [" << result.ErrorModel << "] chi2=" << result.Chi2
            << ", ndf=" << result.UsedBins << ", p=" << result.PValue << endl;

    return 0;
}
