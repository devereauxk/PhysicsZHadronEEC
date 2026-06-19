# Jackknife bin-width normalization bug — review document

## Bug description

`ApplyDiagonalJackknifeErrors` / `ApplyProjectedJackknifeErrors` / `ComputeProjectedJackknifeSigma`
compute leave-one-out JK sigma from raw entries in the `JackknifeProjection*` TTree.
The tree stores un-normalized projected sums (not divided by bin width). When these
raw JK errors are set on histograms whose values are already bin-width-normalized
(from `makeProjection.C`'s `divideByWidth`), the errors are too small by a factor
of `binWidth`:

- DeltaPhi bin width = π/6 ≈ 0.524 → errors ~0.52× correct
- DeltaEta bin width = 0.645 → errors ~0.65× correct

Verified numerically: `DeltaPhi_Result` stored errors match `raw_JK / binWidth`
exactly (ratio = 1.000 for all 12 bins in both observables).

## Session edits

### 1. KEPT — `plot_pPbPbp_unsymmetrized.cpp` (first task)

**File**: `Plots/20260213_Central/plot_pPbPbp_unsymmetrized.cpp` (untracked)

**Edit**: In `ApplyDiagonalJackknifeErrors`, divided by bin width:
```cpp
// Before:
hEta->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varEta));
hPhi->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varPhi));

// After:
hEta->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varEta) / hEta->GetBinWidth(b + 1));
hPhi->SetBinError(b + 1, sqrt((validN - 1.0) / validN * varPhi) / hPhi->GetBinWidth(b + 1));
```

**Status**: Kept. Rebuilt and rerun. Output at `Plots/20260213_Central/plots/pPbPbp_unsymmetrized/`.

### 2. REVERTED — `plot_central_overlay_PPbPbP.cpp`

**File**: `Plots/20260213_Central/plot_central_overlay_PPbPbP.cpp`

**Edit made**: Same bin-width division in `ApplyDiagonalJackknifeErrors` (lines 106–107).

**Status**: Reverted. `git diff` now shows no changes for this file.

**Note**: Not referenced in any `\includegraphics` in the Overleaf `src/*.tex` files.
The scan overlay PDFs exist in `figures/analysis/combining/` but are unused in the note.

### 3. REVERTED — `plot_central_combined.cpp`

**File**: `Plots/20260213_Central/plot_central_combined.cpp`

**Edit made**: Added 6 lines after `ApplyProjectedJackknifeErrors` call for pp
(~line 413) to divide errors by bin width:
```cpp
for (int i = 1; i <= hDeltaEta_pp->GetNbinsX(); ++i)
    hDeltaEta_pp->SetBinError(i, hDeltaEta_pp->GetBinError(i) / hDeltaEta_pp->GetBinWidth(i));
for (int i = 1; i <= hDeltaPhi_pp->GetNbinsX(); ++i)
    hDeltaPhi_pp->SetBinError(i, hDeltaPhi_pp->GetBinError(i) / hDeltaPhi_pp->GetBinWidth(i));
```

**Status**: Reverted. Remaining diff is only the prior-session label changes
(`|y_{Z}| < 2.4` → `|y^{Z}_{CM}| < 1.935`).

## Full audit: all code that reads JackknifeProjection tree

### AFFECTED — bug present, errors too small

| File | Bug location | What it affects |
|------|-------------|----------------|
| `Plots/20260213_Central/plot_pPbPbp_unsymmetrized.cpp` | `ApplyDiagonalJackknifeErrors` L121–122 | pPb/Pbp individual stat errors. **FIXED** (kept). |
| `Plots/20260213_Central/plot_central_overlay_PPbPbP.cpp` | `ApplyDiagonalJackknifeErrors` L106–107 | pPb/Pbp individual stat errors. **NOT FIXED** (reverted). Not in Overleaf. |
| `Plots/20260213_Central/plot_central_combined.cpp` | `ApplyProjectedJackknifeErrors` L413, via `ComputeProjectedJackknifeSigma` L216 | **pp stat errors only** in result plots. Combined pPb+Pbp path is correct (L581–590: JK applied to raw projection, then `divideByWidth` normalizes both values and errors). **NOT FIXED** (reverted). In Overleaf results. |

### NOT AFFECTED — correct bin-width handling

| File | Why correct |
|------|------------|
| `MainAnalysis/20260506_Jackknife/compute_pPbPbp_compatibility.cpp` | `buildCovMatrix` L174: `d[i] = (thetaLOO - theta[i]) / bw[i]` |
| `MainAnalysis/20260506_Jackknife/compute_pPbPbp_compatibility_sym.cpp` | L234: divides by `bw[i]` |
| `Systematics/20260329_pPbSystematics/ComputeChiSquared.cpp` | L207, L224: divides by `GetBinWidth()` |
| `MainAnalysis/20260523_NmixStudy/PlotNmixComparison.cpp` | L153: `sigma[i-1] / h->GetBinWidth(i)` |
| `MainAnalysis/20260523_NmixStudy/PlotNmixSigma.cpp` | L124: `sigma[i] / hRef->GetBinWidth(i+1)` |
| `MainAnalysis/20260414_pPbPbpCombining/plot_12x12_combined_signed.cpp` | L124: divides by `bw[i]` |
| `MainAnalysis/20260414_pPbPbpCombining/plot_12x12_symmetrized.cpp` | L123, L195: divides by `bw[i]` |
| `MainAnalysis/20260506_Jackknife/plot_pPbPbp_sym_overlay.cpp` | No JK recalculation; reads pre-computed errors from histogram |
| `MainAnalysis/20260506_Jackknife/plot_pPbPbp_unsym_overlay.cpp` | No JK recalculation; reads pre-computed errors from histogram |
| `makeProjection.C` (all copies) | Producer of both the tree and the histograms; applies `divideByWidth` after setting JK errors |

### NOT AFFECTED — Overleaf note figures from correct plotters

| Figure in Overleaf | Source plotter | Status |
|---|---|---|
| Fig 25: symmetrized pPb vs Pbp overlay | `plot_pPbPbp_sym_overlay.cpp` | Correct (no JK recalculation) |
| Correlation matrices (figs 27–28) | `compute_pPbPbp_compatibility.cpp` | Correct (divides by bw) |
| Compatibility chi-squared/KS tables | `compute_pPbPbp_compatibility*.cpp` | Correct (divides by bw) |
| pp-vs-pPb agreement table (table 10) | `ComputeChiSquared.cpp` | Correct (divides by bw) |
| All systematics figures | `20260329_pPbSystematics/Plot*.cpp` | Independent pipeline, not affected |
| Combined pPb+Pbp stat errors in result plots | `plot_central_combined.cpp` combined path | Correct (`divideByWidth` after JK) |

## Overleaf figures with remaining bug (pp stat errors)

All produced by `plot_central_combined.cpp` pp path (L399–414):

| results.tex line | Figure | Description |
|---|---|---|
| 16 | `figures/result/all_ZPT0_500_trkPT0.5_15_ZV10_trkV29_nmix10-DeltaPhi-result.pdf` | Inclusive DeltaPhi |
| 112 | `figures/result/all_ZPT0_500_trkPT0.5_15_ZV10_trkV29_nmix10-DeltaEta-result.pdf` | Inclusive DeltaEta |
| 23 | `overleaf_result_scan_signed_deltaphi_combined.pdf` | DeltaPhi scan 1×3 (ZPT 0-500) |
| 33 | `overleaf_result_scan_signed_deltaphi_ZPT0_30_30_500_combined.pdf` | DeltaPhi scan 2×3 (ZPT 0-30/30-500) |
| 119 | `overleaf_result_scan_signed_deltaeta_combined.pdf` | DeltaEta scan 1×3 (ZPT 0-500) |
| 128 | `overleaf_result_scan_signed_deltaeta_ZPT0_30_30_500_combined.pdf` | DeltaEta scan 2×3 (ZPT 0-30/30-500) |

**Impact**: pp stat error bars ~0.52× correct for DeltaPhi, ~0.65× correct for DeltaEta.
Combined pPb+Pbp stat errors are unaffected (correct pipeline).
