# Handoff: KS-test pPb vs Pbp Compatibility

## Goal

Add a KS-like shape-compatibility test for pPb vs Pbp to complement the existing chi-squared tests in `MainAnalysis/20260506_Jackknife/`. Then inject a paragraph + table into the Overleaf note compatibility section (`OverleafZHadronInPPb/src/analysis.tex`).

The full KS algorithm spec is in `.claude/saved_prompts/20260601.md`.

---

## What was done

### Files created / modified

1. **`MainAnalysis/20260506_Jackknife/compute_pPbPbp_ks.cpp`** — DONE, compiles cleanly.
   - Reads same inputs as `compute_pPbPbp_compatibility.cpp`: `pPb_trkResidual_<tag>_ZPT0_500-result.root` / `PbP_trkResidual_<tag>_ZPT0_500-result.root` from `MainAnalysis/20241102_ZhadronVsZPt/plots/`.
   - Histograms used: `DeltaEta_Result<trkRange>` and `DeltaPhi_Result<trkRange>` (jackknife, errors ignored).
   - CLI: `--Tag`, `--BaseDir`, `--OutputDir`, `--TrkRange`.
   - Implements nonneg / shifted KS-like statistic per spec. Shifted procedure is always triggered because all bins in both observables are negative (background-subtracted result).
   - Outputs: `pPbPbp_ks.{md,tsv,tex}` in `OutputDir`.

2. **`MainAnalysis/20260506_Jackknife/makefile`** — DONE. `ExecuteKS` target added; `ExecuteKS` is in the default target and clean.

3. **`MainAnalysis/20260506_Jackknife/run-pPbPbp-ks.sh`** — DONE, executable. Runs both:
   - 20-bin: `TAG=ZV9_trkV28_nmix10` → `output/`
   - 12-bin: `TAG=ZV9_trkV28_nmix10_bin12x12_20260507` → `output_12x12/`

### Run results (already obtained, files written to output/ and output_12x12/)

| Observable | N_bins | D_KS | KS p-value (as-computed) |
|---|---|---|---|
| DeltaEta_Result0.5_15 | 20 | 0.1093 | 1 (invalid) |
| DeltaPhi_Result0.5_15 | 20 | 0.0452 | 1 (invalid) |
| DeltaEta_Result0.5_15 | 12 | 0.1194 | 1 (invalid) |
| DeltaPhi_Result0.5_15 | 12 | 0.0538 | 1 (invalid) |

All bins of both DeltaEta and DeltaPhi are negative (background-subtracted, per-Z-normalized distributions), so the shifted procedure is used throughout. The current p=1 is an artifact of using the shifted histogram integral as n_eff — the integrals are dominated by N_bins * shift (~0.8–1.4 per bin), not the physical signal.

---

## Remaining issue: KS p-value

The p-value calculation is broken. `hNZData_0.5_15` gives the actual weighted Z counts:
- pPb: N_Z = 10323.5
- PbP: N_Z = 5942.6

**Fix needed**: In `compute_pPbPbp_ks.cpp`, read `hNZData_<trkRange>` from both files, and use those as the effective sample sizes in the KS p-value formula instead of the shifted histogram integrals:

```cpp
// Load NZ scalars
TH1D *pPbNZ = loadHistogram(fpPb, "hNZData_" + trkRange, "pPb_nz");
TH1D *PbPNZ = loadHistogram(fPbP, "hNZData_" + trkRange, "PbP_nz");
double n_pPb = pPbNZ ? pPbNZ->GetBinContent(1) : 0;
double n_Pbp = PbPNZ ? PbPNZ->GetBinContent(1) : 0;
```

Pass `n_pPb` and `n_Pbp` into `computeKS()` (or `approxKSPValue()`) instead of deriving them from the histogram integral. Rerun both surfaces.

Expected p-values with N_Z-based n_eff:
- n_eff = 10323.5 * 5942.6 / (10323.5 + 5942.6) ≈ 3785
- For DeltaEta 20-bin: z = 0.1093 * sqrt(3785) ≈ 6.7 → p ≈ 2e-9 (very small, confirming systematic offset)
- For DeltaPhi 20-bin: z = 0.0452 * sqrt(3785) ≈ 2.8 → p ≈ 0.009

---

## Remaining task: Overleaf writer role

After fixing and rerunning to get correct p-values, insert into `OverleafZHadronInPPb/src/analysis.tex` **after line 378** (`\end{table}` that closes `table:combining_stats`):

1. A paragraph (~3–4 sentences) explaining:
   - KS-like statistic: maximum absolute difference of normalized CDFs, pure shape diagnostic with no statistical uncertainties
   - More sensitive than per-bin χ² to systematic offsets (one distribution consistently above/below another)
   - Negative bins handled by common-shift procedure (all distributions here are background-subtracted)
   - Reference `Table~\ref{table:combining_ks}`

2. A 4-column LaTeX table `table:combining_ks`:
   - Columns: Observable | N_bins | D_KS | Approx. KS p-value
   - 4 rows: (DeltaEta 20-bin, DeltaEta 12-bin, DeltaPhi 20-bin, DeltaPhi 12-bin)
   - Fill values from the rerun TSV outputs
   - Caption: note no uncertainties used, shifted procedure used throughout, p-value heuristic
   - `\label{table:combining_ks}`

---

## Key file locations

| Path | Purpose |
|---|---|
| `MainAnalysis/20260506_Jackknife/compute_pPbPbp_ks.cpp` | KS C++ source |
| `MainAnalysis/20260506_Jackknife/run-pPbPbp-ks.sh` | Runner (20-bin + 12-bin) |
| `MainAnalysis/20260506_Jackknife/output/pPbPbp_ks.tsv` | 20-bin results (p-values wrong, need rerun) |
| `MainAnalysis/20260506_Jackknife/output_12x12/pPbPbp_ks.tsv` | 12-bin results (p-values wrong, need rerun) |
| `OverleafZHadronInPPb/src/analysis.tex` | Note to edit (writer role); insert after line 378 |
| `.claude/saved_prompts/20260601.md` | Full KS algorithm spec |
| `OfficialWeightDictionary.sh` | Exports `OFFICIAL_TAG_PPB=ZV10_trkV28_nmix10` (but ZV9 files are what exist for the current note tables) |

## Note on tags

The existing chi-squared compatibility table in the note was computed with `ZV9_trkV28_nmix10` and `ZV9_trkV28_nmix10_bin12x12_20260507`. These are also the tags the KS runner uses, for consistency. `OFFICIAL_TAG_PPB` is now `ZV10` but ZV10 result files don't yet exist.
