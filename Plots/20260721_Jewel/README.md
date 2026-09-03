# JEWEL Z+hadron comparison plots

JEWEL 2.4.0-2D-MOD comparisons against the Pythia8+MadGraph gen-level baseline:
vacuum pp and pPb with the Trajectum 2D hydro medium. All plots use the signed
common-CM convention (12x12 binning, DEtaRange=3.87) with symmetrization and the
standard 0.5 normalization.

## Z pT reweighting

JEWEL generates Z+jet at leading order without Sudakov resummation, so the Z pT
spectrum rises monotonically toward pT -> 0 instead of turning over at ~4 GeV as in
NLO+parton-shower generators. This causes a large shape mismatch at low Z pT that
propagates into the Z+hadron correlations, particularly for the ZPT < 30 GeV bin.

The fix is a shape-normalized spectrum reweighting: the weight at each Z pT is
f_MG(pT) / f_JEWEL(pT), where both spectra are normalized to unit integral. The
weight is stored in ZCorrector format (`hPtCorrTotal` + flat `hEtaCorrTotal`) at
`output/jewel_pp_zptweight.root` and applied multiplicatively to the per-Z event
weight during the correlation analysis via `--UseZWeight true --ZWeightFile`.

The reweighting is derived from the inclusive (ZPT 0-500) nosub files by
`derive_zpt_weight.C`. Measured weight range: [0.200, 2.903] for pp and
[0.227, 2.971] for pPb (nearly identical — the unresummed LO spectrum shape is
common to vacuum and medium generation). At ZPT > 30 GeV, the weights are close
to unity; the correction is concentrated at low Z pT. Weight files:
`output/jewel_pp_zptweight.root`, `output/jewel_pPb_zptweight.root`.

Validation: a separate JEWEL pp sample generated with ptmin=15 (which avoids the
low-pT spectrum issue entirely) reproduces the reweighted ptmin=0 correlations at
ZPT > 30 GeV within statistical uncertainties. The verification composite plot
overlays all four curves (Pythia+MG, JEWEL raw, JEWEL reweighted, JEWEL ptmin=15)
to demonstrate this agreement.

## Charged-particle selection

JEWEL MOD skims store all final-state particles (charged + neutral). Without an
explicit charged-only cut, ~41% (pp) to ~59% (pPb, where recoil hadronization
adds thermal particles) of tracks are neutrals (photons, K0, neutrons), causing
a large flat excess in the correlations. The analysis applies
`par.isJewel && trackCharge == 0 -> reject` in `CorrelationAnalysis.cpp`, gated
on the `--IsJewel true` flag so non-JEWEL datasets are unaffected.

## Hole treatment (pPb)

JEWEL medium runs with KEEPRECOILS store the thermal momenta subtracted from the
medium ("holes") as extra tracks with weight -1; the skim marks them with
sentinel `trackCharge = 999`, so they pass the charged-only cut. Holes are
thermal partons (gluons and u quarks), so their momentum represents the full
(charged + neutral) medium response, while we measure charged particles only.
The analysis therefore scales hole weights to -0.67 — the charged share of
final-state momentum, measured at 0.60-0.67 in the soft region of the JEWEL pPb
sample itself. Implemented in `CorrelationAnalysis.cpp` gated on
`par.isJewel && !par.isPP`.

## Plots

### `jewel_pp_composite_deltaphi_3row.pdf`
Three-row (ZPT 0-500, 0-30, 30-500) x three-column (trkPT 0.5-2, 2-4, 4-15)
DeltaPhi composite. Single method: Z pT reweighted JEWEL pp vs Pythia8+MG baseline.
Primary result figure for pp vacuum comparison.

### `jewel_pp_composite_deltaphi.pdf`
Two-row (ZPT 0-30, 30-500) per-row method composite. Uses Z pT reweighted ptmin=0
for the low-ZPT row and unweighted ptmin=15 for the high-ZPT row.

### `jewel_pp_composite_deltaphi_verification.pdf`
Two-row composite showing all four curves (Pythia+MG, JEWEL ptmin=0 raw, JEWEL
ptmin=0 reweighted, JEWEL ptmin=15). Demonstrates ptmin=15 matching ptmin=0 at
ZPT > 30 GeV.

### `jewel_pp_deltaphi.pdf`
Single-row inclusive (ZPT 0-500) DeltaPhi comparison in the three trkPT columns:
Pythia8+MG vs JEWEL pp (Z pT reweighted).

### `jewel_zpt_spectra.pdf`
Z pT spectrum comparison (log-log top panel, ratio to Pythia+MG bottom panel).
Four curves: Pythia8+MG, JEWEL ptmin=0 (raw), JEWEL ptmin=0 (reweighted),
JEWEL ptmin=15. Shows the LO spectrum excess at low pT and the reweighting closure.

### `jewel_pPb_composite_deltaphi_3row.pdf`
Three-row DeltaPhi composite: Pythia8+MG baseline, JEWEL pp (Z pT rw), and
JEWEL pPb v4 hydro (Z pT rw, hole x0.67, charged-only). Primary result figure
for the medium comparison. Produced by `plot-composite-pPb.sh`.

### `jewel_pPb_deltaphi.pdf`
Single-row inclusive (ZPT 0-500) version of the above. Produced by `plot-pPb.sh`.

## Inputs

| File | Description |
|------|-------------|
| `~/jewel/jewel_pp8160v3_MOD_500k.root` | JEWEL pp 8.16 TeV vacuum, ptmin=0, 500k events |
| `~/jewel/jewel_pp8160v4_MOD_500k.root` | JEWEL pp 8.16 TeV vacuum, ptmin=15, 500k events |
| `~/jewel/jewel_pPb_v4_MOD_500k.root` | JEWEL pPb 8.16 TeV, Trajectum 2D hydro, ptmin=0, 500k events, Ncoll-weighted profiles, PICKVTX/READNCOLL fixes, recoils kept |
| Pythia+MG theory prefix (from `OfficialProductDictionary.sh`) | Gen-level baseline |

The analysis is run from `MainAnalysis/20241102_ZhadronVsZPt/jewel-signed.sh` with
modes: `ppMOD_scan`, `ppMOD15_scan`, `ppMOD_zrw_scan`, `pPbMODv4`,
`pPbMODv4_zrw_scan`.

## Build

```bash
source ../../SetupAnalysis.sh
make
```

## Solution 2 study (`Solution2/`)

Per-Ncoll-class quenching study testing whether the hydro-ensemble centrality
bias drives the large pPb quenching magnitude. Result: refuted — the
modification is nearly independent of droplet size, and MB reweighting moves
the prediction by <= 0.03. See `Solution2/README.md`.

## Physics conclusions (pPb)

Away-side yields (DeltaPhi in [pi/2, 3pi/2]), JEWEL pPb / JEWEL pp ratio (both
Z pT reweighted, so this isolates the medium effect):

| ZPT (GeV) | trkPT 0.5-2 | trkPT 2-4 | trkPT 4-15 |
|-----------|-------------|-----------|------------|
| > 30      | 1.37 ± 0.02 | 1.11 ± 0.02 | 0.79 ± 0.01 |
| < 30      | 1.00 ± 0.03 | 0.89 ± 0.03 | 0.72 ± 0.03 |
| inclusive | 1.13 ± 0.02 | 1.00 ± 0.02 | 0.77 ± 0.01 |

- **Classic quenching pattern at high Z pT**: the away-side jet loses energy to
  the medium — 4-15 GeV fragments are suppressed by ~21% while the lost energy
  reappears as soft medium response, enhancing the 0.5-2 GeV yield by ~37%. The
  crossing point between suppression and enhancement sits in the 2-4 GeV bin.

- **Suppression without compensation at low Z pT**: the recoiling parton is soft
  (pT ~ Z pT < 30 GeV), so quenched fragments fall below the 0.5 GeV threshold or
  spread to large angles outside the acceptance — hard fragments are suppressed
  (~28% at 4-15 GeV) with no visible soft enhancement (1.00 at 0.5-2 GeV).

- **Magnitude caveat**: a 20-30% away-side modification in pPb is much larger
  than experimental limits on quenching in small systems. JEWEL run with a full
  hydro medium on Trajectum pPb profiles represents a maximal-quenching
  scenario, useful as a bracket that the measured pPb correlations can
  discriminate against. Main modeling uncertainties: medium profile
  normalization and centrality mixture, the hole charged-share factor (0.67),
  and the boost-invariant rapidity-symmetric medium (the last is second-order
  for the symmetrized observables).

## Physics conclusions (pp)

- JEWEL pp vacuum at 8.16 TeV reproduces the Pythia8+MG Z+hadron DeltaPhi
  correlations within ~5-10% after correcting for two generator-level mismatches:
  neutral particle contamination and Z pT spectrum shape.

- The Z pT reweighting is the dominant correction at low Z pT (< 30 GeV), where the
  LO matrix element overproduces soft Z bosons by up to 3x relative to the
  NLO+parton-shower Pythia+MG spectrum. At high Z pT (> 30 GeV) the reweighting is
  negligible and an independent ptmin=15 generation confirms the reweighted result.

- After corrections, the remaining pp JEWEL-vs-Pythia+MG differences are at the
  level of the statistical precision of the 500k-event JEWEL samples, establishing
  the vacuum baseline for the pPb medium comparison.
