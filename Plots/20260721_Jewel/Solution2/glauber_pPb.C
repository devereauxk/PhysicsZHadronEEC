// Minimum-bias Glauber MC for pPb at 8.16 TeV: P(Ncoll | Ncoll >= 1).
// Pb: Woods-Saxon R = 6.62 fm, a = 0.546 fm, A = 208. Black-disk NN collision
// with d < sqrt(sigma_NN/pi), sigma_NN = 72 mb = 7.2 fm^2.
// Output: output/glauber_pPb_pncoll.tsv  (n  P(n))
void glauber_pPb(long long nEvents = 2000000) {
    const double sigmaNN = 7.2;                     // fm^2
    const double d2max = sigmaNN / TMath::Pi();     // d^2 threshold
    const double RPb = 6.62, aPb = 0.546;
    const int A = 208;
    const double bMax = 12.0;

    TRandom3 rng(42);
    const int NMAX = 60;
    std::vector<double> count(NMAX + 1, 0.0);
    long long nInel = 0;

    // Woods-Saxon radial sampler (rejection, r^2 weight)
    auto sampleR = [&]() {
        while (true) {
            double r = rng.Uniform(0, RPb + 8 * aPb);
            double w = r * r / (1.0 + TMath::Exp((r - RPb) / aPb));
            double wmax = (RPb + 2 * aPb) * (RPb + 2 * aPb);
            if (rng.Uniform(0, wmax) < w) return r;
        }
    };

    for (long long ev = 0; ev < nEvents; ++ev) {
        double b = TMath::Sqrt(rng.Uniform(0, bMax * bMax));
        int ncoll = 0;
        for (int i = 0; i < A; ++i) {
            double r = sampleR();
            double ct = rng.Uniform(-1, 1);
            double phi = rng.Uniform(0, 2 * TMath::Pi());
            double st = TMath::Sqrt(1 - ct * ct);
            double x = r * st * TMath::Cos(phi), y = r * st * TMath::Sin(phi);
            double dx = x - b, dy = y;
            if (dx * dx + dy * dy < d2max) ncoll++;
        }
        if (ncoll >= 1) {
            nInel++;
            count[TMath::Min(ncoll, NMAX)] += 1.0;
        }
    }

    double mean = 0, m2 = 0;
    for (int n = 1; n <= NMAX; ++n) { mean += n * count[n]; m2 += double(n) * n * count[n]; }
    mean /= nInel; m2 /= nInel;
    printf("MB pPb Glauber (sigmaNN=72 mb): <Ncoll> = %.2f   hard-weighted <Ncoll> = <n^2>/<n> = %.2f\n",
           mean, m2 / mean);
    printf("P(Ncoll<=3) = %.3f\n", (count[1] + count[2] + count[3]) / nInel);

    gSystem->mkdir("output", true);
    FILE *fo = fopen("output/glauber_pPb_pncoll.tsv", "w");
    fprintf(fo, "# n\tP(n)  (MB pPb 8.16 TeV, sigmaNN=72mb, <n>=%.3f, <n2>/<n>=%.3f)\n", mean, m2 / mean);
    for (int n = 1; n <= NMAX; ++n)
        fprintf(fo, "%d\t%.6e\n", n, count[n] / nInel);
    fclose(fo);
    printf("Wrote output/glauber_pPb_pncoll.tsv\n");
}
