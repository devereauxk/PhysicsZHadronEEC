# Solution 2: per-Ncoll-class quenching study

Tests the leading hypothesis for the large JEWEL pPb quenching magnitude: that
the hydro-profile ensemble's centrality bias (missing minimum-bias peripheral
events) inflates the predicted effect. Method: split the v4 generation by
hydro-profile Ncoll class, measure the away-side modification per class, and
reweight to a proper minimum-bias mixture.

## Inputs

Class skims (v4 generation split by profile Ncoll, all generator fixes +
multiplicity weights included):

| File | Class | Events | Weighted N_Z | slot weight (mult x n) | <Ncoll>_hard |
|------|-------|--------|--------------|------------------------|--------------|
| `~/jewel/jewel_pPb_v4_MOD_Ncoll_le3.root`   | Ncoll <= 3  | 10.6k | 79.9  | 23  | 2.57 |
| `~/jewel/jewel_pPb_v4_MOD_Ncoll_8to12.root` | Ncoll 8-12  | 175k  | 1287  | 379 | 10.19 |
| `~/jewel/jewel_pPb_v4_MOD_Ncoll_ge17.root`  | Ncoll >= 17 | 188k  | 1395  | 407 | 18.64 |

Middle classes (4-7, weight 100, <n> = 5.88; 13-16, weight 174, <n> = 14.56)
are not converted; the analysis interpolates linearly in <Ncoll> between the
measured classes. Baseline: JEWEL pp (Z pT rw). All classes use the same
analysis treatment as the main result (charged-only, hole x0.67, pPb Z pT
weight, signed convention).

## How to run

```bash
# 1. Class analyses (from MainAnalysis/20241102_ZhadronVsZPt):
MODE=pPbMODv4_Ncoll_le3_zrw_scan   NTHREAD=30 bash jewel-signed.sh
MODE=pPbMODv4_Ncoll_8to12_zrw_scan NTHREAD=30 bash jewel-signed.sh
MODE=pPbMODv4_Ncoll_ge17_zrw_scan  NTHREAD=30 bash jewel-signed.sh
# 2. Study (Glauber reference + extraction + reweighting + plot):
bash run-solution2.sh
```

## Outputs

- `output/glauber_pPb_pncoll.tsv` — MB pPb Glauber P(Ncoll), sigma_NN = 72 mb
- `output/away_ratios_ncoll.tsv` — away-side yield ratio (class / JEWEL pp) per
  ZPT x trkPT x class
- `output/mb_reweight.tsv` — measured vs decomposition-reconstructed vs
  MB-reweighted ratios
- `plots/away_ratio_vs_ncoll.pdf` — ratio vs <Ncoll> per ZPT bin, three trkPT series

## Results

**Glauber MB reference** (sigma_NN = 72 mb): <Ncoll> = 7.06, hard-weighted
<n^2>/<n> = 10.99, P(Ncoll <= 3) = 0.349, hard-weight fraction of Ncoll <= 3 =
0.085. The v4 sample ensemble: <Ncoll> = 10.83, hard-weighted 13.51,
P(Ncoll <= 3) = 0.10 — the centrality bias is confirmed and quantified.

**Away-side ratios per class** (ZPT 30-500 GeV):

| trkPT (GeV) | Ncoll <= 3 | 8-12 | >= 17 |
|-------------|-----------|------|-------|
| 0.5-2  | 1.51 ± 0.07 | 1.36 ± 0.02 | 1.37 ± 0.03 |
| 2-4    | 1.18 ± 0.07 | 1.11 ± 0.02 | 1.10 ± 0.02 |
| 4-15   | 0.85 ± 0.06 | 0.79 ± 0.02 | 0.78 ± 0.02 |

**Validation**: the 5-bin class decomposition reconstructs the full-sample
measurement to better than 0.01 absolute in every ZPT x trkPT bin
(`R_recon` vs `R_meas` in `mb_reweight.tsv`), so the class split plus linear
interpolation describes the ensemble faithfully.

**MB reweighting**: the reweighted prediction differs from the measured
full-sample ratio by <= 0.03 absolute in every bin (e.g. ZPT 30-500,
trkPT 4-15: 0.792 -> 0.802). Treating all sub-Tc / Ncoll <= 3 events as pure
vacuum (the most aggressive corona assumption, kernel weight 0.085 at R = 1)
adds at most another ~0.015.

## Conclusion

**The ensemble-centrality-bias hypothesis is refuted as the driver of the large
magnitude.** The quenching is nearly independent of droplet size: even the
Ncoll <= 3 class — droplets with T_max = 0.17-0.24 GeV (barely above T_c),
r(T > T_c) = 0.4-1.4 fm, lifetimes 0.9-2.1 fm/c — shows ~15% suppression at
4-15 GeV and the *largest* soft enhancement (1.51 ± 0.07). Because the
suppression-vs-Ncoll curve is flat, correcting the mixture to minimum bias (or
even adding vacuum corona slots) changes the prediction by only 0.01-0.04.

The magnitude is therefore intrinsic to the JEWEL + 2D-hydro configuration:
every event starts the hard parton at the Ncoll-weighted center of its droplet,
where the earliest (most quenching-active) shower phase always sees medium.
Follow-up suspects, in order:

1. **Pre-hydro ramp phase**: GETTEMP ramps T linearly from tau = 0 to
   tau_i = 0.616 fm/c — the parton scatters from tau = 0 at the droplet core in
   every event. Test: return T = 0 for tau < tau_i (code variant).
2. **Infrared-floored scattering rate near T_c**: Debye mass floors at
   MDFACTOR = 0.45 GeV, so sigma ~ 1/md^2 stays large as T -> T_c and the
   scattering rate scales far more weakly than T^3 — small droplets quench
   almost as efficiently per unit path as big ones. Test: MDFACTOR / TC
   sensitivity scan.
3. **Medium response universality**: the soft enhancement does not scale with
   droplet size either (largest for the smallest class). Test: per-class
   norecoil comparison to separate parton energy loss from response.
4. **Rapidity-flat slab** (|eta_s| < 3.1): unchanged from the main diagnosis;
   bounding runs with reduced eta_max remain worthwhile.
