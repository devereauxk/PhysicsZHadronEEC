// z_correction.C - Direct 2D Z-side correction from pPb to PbP
#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TParameter.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

void z_correction(const char* pPbFile, const char* PbPFile, const char* outputFile) {
    TFile *fPPb = TFile::Open(pPbFile);
    TFile *fPbP = TFile::Open(PbPFile);

    TH2D *hPPb = (TH2D*)fPPb->Get("hZYPhiData")->Clone("hPPb_ZYPhi");
    TH2D *hPbP = (TH2D*)fPbP->Get("hZYPhiData")->Clone("hPbP_ZYPhi");
    TH1D *hNZPPb = (TH1D*)fPPb->Get("hNZData");
    TH1D *hNZPbP = (TH1D*)fPbP->Get("hNZData");
    hPPb->SetDirectory(nullptr);
    hPbP->SetDirectory(nullptr);

    const double nZPPb = hNZPPb->GetBinContent(1);
    const double nZPbP = hNZPbP->GetBinContent(1);

    TH2D *hPPbNorm = (TH2D*)hPPb->Clone("hPPbNorm");
    TH2D *hPbPNorm = (TH2D*)hPbP->Clone("hPbPNorm");
    hPPbNorm->Scale(1.0 / nZPPb);
    hPbPNorm->Scale(1.0 / nZPbP);

    TH2D *hRatioPPbOverPbP = (TH2D*)hPPbNorm->Clone("hRatioPPbOverPbP");
    TH2D *hWeightToApplyRaw = (TH2D*)hPbPNorm->Clone("hWeightToApplyRaw");
    TH2D *hWeightToApply = (TH2D*)hPbPNorm->Clone("hWeightToApply");
    TH2D *hPPbBalancedNorm = (TH2D*)hPbPNorm->Clone("hPPbBalancedNorm");

    const int nXBins = hPPbNorm->GetNbinsX();
    const int nYBins = hPPbNorm->GetNbinsY();
    const double epsilon = 1e-18;
    const int maxIterations = 10000;
    const double tolerance = 1e-14;

    std::vector<std::vector<double>> balanced(nXBins + 1, std::vector<double>(nYBins + 1, 0));
    std::vector<double> rowTarget(nXBins + 1, 0);
    std::vector<double> colTarget(nYBins + 1, 0);

    int protectedBins = 0;
    int sourceOnlyBins = 0;
    int targetOnlyBins = 0;

    for(int ix = 1; ix <= nXBins; ix++)
    {
        for(int iy = 1; iy <= nYBins; iy++)
        {
            const double ppb = hPPbNorm->GetBinContent(ix, iy);
            const double pbp = hPbPNorm->GetBinContent(ix, iy);

            double ratio = 1.0;
            double rawWeight = 1.0;

            if(ppb > 0 && pbp > 0)
            {
                ratio = ppb / pbp;
                rawWeight = pbp / ppb;
                balanced[ix][iy] = pbp;
            }
            else if(ppb > 0)
            {
                sourceOnlyBins++;
                protectedBins++;
                balanced[ix][iy] = epsilon * ppb;
            }
            else
            {
                if(pbp > 0)
                    targetOnlyBins++;
                protectedBins++;
            }

            hRatioPPbOverPbP->SetBinContent(ix, iy, ratio);
            hRatioPPbOverPbP->SetBinError(ix, iy, 0);
            hWeightToApplyRaw->SetBinContent(ix, iy, rawWeight);
            hWeightToApplyRaw->SetBinError(ix, iy, 0);

            rowTarget[ix] += pbp;
            colTarget[iy] += pbp;
        }
    }

    bool converged = false;
    int iterationsUsed = 0;
    double maxMarginDeviation = 0;

    for(int iteration = 0; iteration < maxIterations; iteration++)
    {
        bool impossible = false;

        for(int ix = 1; ix <= nXBins; ix++)
        {
            double rowSum = 0;
            for(int iy = 1; iy <= nYBins; iy++)
                rowSum += balanced[ix][iy];

            if(rowTarget[ix] <= 0)
            {
                for(int iy = 1; iy <= nYBins; iy++)
                    balanced[ix][iy] = 0;
                continue;
            }

            if(rowSum <= 0)
            {
                impossible = true;
                break;
            }

            const double scale = rowTarget[ix] / rowSum;
            for(int iy = 1; iy <= nYBins; iy++)
                balanced[ix][iy] *= scale;
        }

        if(impossible)
            break;

        for(int iy = 1; iy <= nYBins; iy++)
        {
            double colSum = 0;
            for(int ix = 1; ix <= nXBins; ix++)
                colSum += balanced[ix][iy];

            if(colTarget[iy] <= 0)
            {
                for(int ix = 1; ix <= nXBins; ix++)
                    balanced[ix][iy] = 0;
                continue;
            }

            if(colSum <= 0)
            {
                impossible = true;
                break;
            }

            const double scale = colTarget[iy] / colSum;
            for(int ix = 1; ix <= nXBins; ix++)
                balanced[ix][iy] *= scale;
        }

        if(impossible)
            break;

        maxMarginDeviation = 0;
        for(int ix = 1; ix <= nXBins; ix++)
        {
            double rowSum = 0;
            for(int iy = 1; iy <= nYBins; iy++)
                rowSum += balanced[ix][iy];
            maxMarginDeviation = std::max(maxMarginDeviation, std::fabs(rowSum - rowTarget[ix]));
        }
        for(int iy = 1; iy <= nYBins; iy++)
        {
            double colSum = 0;
            for(int ix = 1; ix <= nXBins; ix++)
                colSum += balanced[ix][iy];
            maxMarginDeviation = std::max(maxMarginDeviation, std::fabs(colSum - colTarget[iy]));
        }

        iterationsUsed = iteration + 1;
        if(maxMarginDeviation < tolerance)
        {
            converged = true;
            break;
        }
    }

    for(int ix = 1; ix <= nXBins; ix++)
    {
        for(int iy = 1; iy <= nYBins; iy++)
        {
            const double ppb = hPPbNorm->GetBinContent(ix, iy);
            const double finalWeight = (ppb > 0) ? balanced[ix][iy] / ppb : 1.0;

            hPPbBalancedNorm->SetBinContent(ix, iy, balanced[ix][iy]);
            hPPbBalancedNorm->SetBinError(ix, iy, 0);
            hWeightToApply->SetBinContent(ix, iy, finalWeight);
            hWeightToApply->SetBinError(ix, iy, 0);
        }
    }

    TFile *fOut = new TFile(outputFile, "RECREATE");
    hPPbNorm->Write();
    hPbPNorm->Write();
    hRatioPPbOverPbP->Write();
    hWeightToApplyRaw->Write();
    hWeightToApply->Write();
    hPPbBalancedNorm->Write();

    TParameter<int> ProtectedBins("ProtectedBins", protectedBins);
    ProtectedBins.Write();
    TParameter<int> SourceOnlyBins("SourceOnlyBins", sourceOnlyBins);
    SourceOnlyBins.Write();
    TParameter<int> TargetOnlyBins("TargetOnlyBins", targetOnlyBins);
    TargetOnlyBins.Write();
    TParameter<int> BalanceConverged("BalanceConverged", converged ? 1 : 0);
    BalanceConverged.Write();
    TParameter<int> BalanceIterations("BalanceIterations", iterationsUsed);
    BalanceIterations.Write();
    TParameter<double> BalanceMaxMarginDeviation("BalanceMaxMarginDeviation", maxMarginDeviation);
    BalanceMaxMarginDeviation.Write();
    fOut->Close();

    std::cout << "Direct 2D Z correction written to " << outputFile
              << " with " << protectedBins << " protected bins"
              << ", source-only bins = " << sourceOnlyBins
              << ", target-only bins = " << targetOnlyBins
              << ", converged = " << converged
              << ", max margin deviation = " << maxMarginDeviation
              << std::endl;

    fPPb->Close();
    fPbP->Close();
    delete fOut;
}
