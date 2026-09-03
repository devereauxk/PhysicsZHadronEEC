// Derive Z pT reweighting for JEWEL pp -> Pythia+MG spectrum.
// Builds hPtCorrTotal = f_MG(pT) / f_JEWEL(pT) (both shape-normalized) and
// hEtaCorrTotal = 1, in the ZCorrector file format.
void derive_zpt_weight(const char *jewelFile, const char *mgFile, const char *outFile) {
    TFile *fJ = TFile::Open(jewelFile);
    TFile *fM = TFile::Open(mgFile);
    if (!fJ || fJ->IsZombie() || !fM || fM->IsZombie()) {
        printf("ERROR: cannot open inputs\n");
        return;
    }
    TH3D *h3J = (TH3D*)fJ->Get("hZPtEtaPhi_0.5_15");
    TH3D *h3M = (TH3D*)fM->Get("hZPtEtaPhi_0.5_15");
    if (!h3J || !h3M) { printf("ERROR: missing hZPtEtaPhi_0.5_15\n"); return; }

    TH1D *hJ = h3J->ProjectionX("hJ");
    TH1D *hM = h3M->ProjectionX("hM");
    hJ->Scale(1.0 / hJ->Integral());
    hM->Scale(1.0 / hM->Integral());

    int n = hJ->GetNbinsX();
    // Weight hist axis: prepend [0, firstEdge] and append [lastEdge, 500]
    std::vector<double> edges;
    edges.push_back(0);
    for (int i = 1; i <= n + 1; ++i) edges.push_back(hJ->GetXaxis()->GetBinLowEdge(i));
    edges.push_back(500);

    TH1D *hW = new TH1D("hPtCorrTotal", "Z p_{T} weight;p_{T}^{Z} (GeV);MG/JEWEL", edges.size() - 1, edges.data());
    for (int i = 1; i <= n; ++i) {
        double j = hJ->GetBinContent(i), m = hM->GetBinContent(i);
        double w = (j > 0) ? m / j : 1.0;
        hW->SetBinContent(i + 1, w);
    }
    // Edge bins: copy nearest interior ratio
    hW->SetBinContent(1, hW->GetBinContent(2));
    hW->SetBinContent(n + 2, hW->GetBinContent(n + 1));

    TH1D *hE = new TH1D("hEtaCorrTotal", "flat;y^{Z};weight", 1, -10, 10);
    hE->SetBinContent(1, 1.0);

    double wmin = 1e30, wmax = -1e30;
    for (int i = 1; i <= hW->GetNbinsX(); ++i) {
        wmin = TMath::Min(wmin, hW->GetBinContent(i));
        wmax = TMath::Max(wmax, hW->GetBinContent(i));
    }
    printf("weight range: [%.3f, %.3f]\n", wmin, wmax);
    printf("sample bins: pT=1: %.3f  pT=5: %.3f  pT=15: %.3f  pT=40: %.3f  pT=90: %.3f\n",
        hW->GetBinContent(hW->FindBin(1.0)), hW->GetBinContent(hW->FindBin(5.0)),
        hW->GetBinContent(hW->FindBin(15.0)), hW->GetBinContent(hW->FindBin(40.0)),
        hW->GetBinContent(hW->FindBin(90.0)));

    TFile *fo = TFile::Open(outFile, "RECREATE");
    hW->Write();
    hE->Write();
    fo->Close();
    printf("Wrote %s\n", outFile);
}
