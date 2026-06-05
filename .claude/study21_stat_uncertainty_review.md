# Study 21 Statistical Uncertainty — Review Document

**Study**: Symmetrized combined pPb+Pbp vs pp result in the common CM frame (CMA).  
**Source file**: `MainAnalysis/20260414_pPbPbpCombining/plot_12x12_symmetrized.cpp`  
**Runner**: `plot-12x12-sym-cma.sh`  
**Output**: `plots/12x12_sym_cma/cma_sym-{Result-DeltaEta,Result-DeltaPhi}.pdf` + `cma_sym-pvalue.tex`

---

## 1. The symmetrized result

For each observable x (DeltaEta or DeltaPhi), the symmetrized result is:

```
R_sym(x) = 0.5 * (R(+x) + R(-x))
```

Applied after forming the N_Z-weighted combination:

```
R_comb = w_pPb * R_pPb + w_Pbp * R_Pbp
```

where `w_pPb = N_Z_pPb / (N_Z_pPb + N_Z_Pbp) ≈ 0.635`, `w_Pbp ≈ 0.365`
(from N_Z: pPb=9269.24, Pbp=5331.09).

Symmetrization is also applied to the pp histogram independently.

**DeltaEta mirror partner** (0-indexed): bin `i ↔ n−1−i`  
Correct because the [-4, 4] axis has a bin edge at η=0.

**DeltaPhi mirror partner** (0-indexed, 12 bins on [-π/2, 3π/2]):  
Near-side (bins 0–5): `i ↔ 5−i` (pairs: 0↔5, 1↔4, 2↔3)  
Away-side (bins 6–11): `i ↔ 17−i` (pairs: 6↔11, 7↔10, 8↔9)  
Implements φ → −φ symmetry about φ=0.

---

## 2. Projection windows

| Observable | Integration window | Scale |
|---|---|---|
| DeltaEta result | DeltaPhi Y-bins 4–6 (φ ∈ [0, π/2]) | 0.5 |
| DeltaPhi result | DeltaEta X-bins 1–12 (η ∈ [−4, 4], full range) | 1.0 |

**DeltaPhi uses the full DeltaEta range by design.** Using only positive eta (bins 7–12) would project over the proton hemisphere exclusively; using full eta averages the pPb (proton-going = large positive eta) and Pbp (proton-going = large negative eta) contributions. Both pp and the combined HI have near-symmetric distributions in eta after symmetrization, making the full-range integral well-behaved for the chi-square comparison.

---

## 3. Jackknife error calculation

### 3.1 Jackknife data sources

Two separate TTree sources are used, for different observables:

**DeltaEta JK** — loaded from `JackknifeProjection0.5_15` TTree in the `-result.root` files:
- Branch `SignalEta[i]`: per-Z-event contribution to DeltaEta bin `i`, summed over DeltaPhi Y-bins 4–6.
- This projection window matches the DeltaEta histogram projection window exactly.

**DeltaPhi JK** — loaded from `Jackknife2DData` TTree in the raw intermediate `.root` files (e.g., `output/pPb_trkResidual_..._12x12_cma_ZPT0_500-0.5_15.root`):
- Branch `SignalBins[144]`: flat 2D array for the full 12×12 histogram, indexed as `(xBin−1)*12 + (yBin−1)` with xBin=DeltaEta, yBin=DeltaPhi.
- The full-eta phi projection per event is computed as `sum_{xBin=0..11} SignalBins[xBin*12 + phiBin]`.

**Why different sources for eta vs phi?**  
The `JackknifeProjection` tree's `SignalPhi[i]` is pre-projected using only DeltaEta X-bins 7–12 (positive eta, [0, 4]), hardcoded by `setModified12x12ProjectionWindow` in `makeProjection.C`. If `SignalPhi` were used with the full-eta phi histogram, the HI error bars would be inflated by ~10× because the histogram value (~0.05, pos+neg eta nearly cancel for the asymmetric pPb+Pbp combination) is much smaller than what `SignalPhi` represents (~0.24, pos-eta only). The `Jackknife2DData` tree retains the full 2D information and allows correct full-eta phi JK covariance computation.

### 3.2 Leave-one-out (LOO) formula

For each system (pPb, Pbp, pp) with N events, the LOO jackknife deviation for leaving out event k is:

```
d_k[i] = [ (totSig[i] - sig_k[i]) / (totSigNZ - sigNZ_k)
          - (totMix[i] - mix_k[i]) / (totMixNZ - mixNZ_k)
          - theta[i] ] / binWidth[i]
```

where `theta[i] = totSig[i]/totSigNZ - totMix[i]/totMixNZ` is the full-sample estimate.

The per-system covariance matrix is:

```
C[i,j] = (N−1)/N * Σ_k d_k[i] * d_k[j]
```

### 3.3 Combined HI covariance

pPb and Pbp are statistically independent. The combined covariance is:

```
C_comb[i,j] = w_pPb² * C_pPb[i,j] + w_Pbp² * C_Pbp[i,j]
```

### 3.4 Symmetrized covariance

After forming C_comb (and C_pp independently), the symmetrized covariance is:

```
C_sym[i,j] = 0.25 * (C[i,j] + C[i,nj] + C[ni,j] + C[ni,nj])
```

where `ni` and `nj` are the mirror partners of bins `i` and `j`.

This is the exact propagation of `R_sym(x) = 0.5*(R(+x)+R(−x))` through the jackknife: the LOO deviation of the symmetrized quantity is `0.5*(d_k[i] + d_k[ni])`, so its variance is `0.25*(Var(d[i]) + 2*Cov(d[i],d[ni]) + Var(d[ni]))`.

### 3.5 Plot error bars

The error bar on bin `i` of the symmetrized result histogram is:

```
σ_i = sqrt(C_sym[i,i])
```

These are the **diagonal elements of the symmetrized jackknife covariance matrix** — they account for the symmetrization correlation between bins `i` and `ni`, but treat different bin pairs as independent.

---

## 4. Chi-squared p-value table

Current values (from `plots/12x12_sym_cma/cma_sym-pvalue.tex`):

| Observable | χ² | ndf | χ²/ndf | p-value |
|---|---|---|---|---|
| ΔyZ diagonal (JK) | 2.47 | 6 | 0.412 | 0.87 |
| ΔφZ diagonal (JK) | 9.34 | 6 | 1.56 | 0.155 |
| ΔyZ full covariance | 2.28 | 6 | 0.381 | 0.89 |
| ΔφZ full covariance | 21 | 6 | 3.50 | 0.0018 |

### Row 1: ΔyZ diagonal (JK)

**Formula:**
```
χ² = Σ_i (HI_i − pp_i)² / (σ_HI_i² + σ_pp_i²)
```
where `σ_HI_i = sqrt(C_combEtaSym[i,i])` and `σ_pp_i = sqrt(C_ppEtaSym[i,i])`.

**ndf = 6** — the 12 symmetrized bins contain only 6 unique mirrored pairs (bins `i` and `ni = 13−i` are identical after symmetrization). The sum is evaluated over the 6 first-of-pair bins (1..6) only.

**Interpretation**: No significant tension (p=0.87). The symmetrized DeltaEta shapes of the combined pPb+Pbp and pp are compatible within statistical uncertainties on a bin-by-bin basis.

### Row 2: ΔφZ diagonal (JK)

Same formula, applied to the DeltaPhi bins using `C_combPhiSym` and `C_ppPhiSym` (full-eta phi JK). **ndf = 6** — 6 unique mirrored pairs: near-side {1,2,3} (partners 6,5,4) and away-side {7,8,9} (partners 12,11,10). The sum is evaluated over these 6 bins only.

**Interpretation**: No significant tension (p=0.155) bin-by-bin. The diagonal test ignores cross-bin correlations.

### Row 3: ΔyZ full covariance

**Formula:**
```
C_tot = C_combEtaSym + C_ppEtaSym    (12×12 total covariance)
χ² = δᵀ C_tot⁺ δ
```
where `δ_i = HI_i − pp_i` and `C_tot⁺` is the Moore-Penrose pseudoinverse computed via SVD.

**SVD tolerance**: singular values below `sv_max × 10⁻⁸` are treated as zero.

**ndf = SVD rank of C_tot = 6** (not 12).  
The symmetrized covariance has rank 6 because symmetrization forces `C_sym[i,j] = C_sym[ni,nj]`: the 12 bins collapse to 6 independent degrees of freedom. The SVD pseudoinverse correctly handles this rank deficiency and yields ndf=6.

**Interpretation**: No significant tension (p=0.89). The full covariance chi-square properly accounts for bin-to-bin correlations from shared Z events and the symmetrization structure.

### Row 4: ΔφZ full covariance

Same as Row 3 applied to DeltaPhi. **C_tot = C_combPhiSym + C_ppPhiSym**, **ndf = SVD rank = 6**.

**Interpretation**: p=0.0018 — significant tension at the ~3σ level. This reflects a genuine difference in the full-eta-integrated phi distribution between combined pPb+Pbp and pp. The combined HI phi integral over all DeltaEta is ~0.05 (positive, because pPb contributes +0.105 and Pbp contributes −0.047, with w_pPb > w_Pbp), while the pp phi integral is ≈0 (symmetric in eta). After phi-symmetrization, this normalization offset persists in the symmetrized phi distribution. The full-covariance chi-square (which exploits the off-diagonal structure from the symmetrization) detects this more powerfully than the diagonal test (p=0.155).

---

## 5. Summary of design choices and potential review questions

1. **Why full-eta for DeltaPhi but phi-windowed for DeltaEta?**  
   The DeltaEta JK tree (`SignalEta`) was stored with the phi window [0, π/2] baked in — this matches the analysis convention (project DeltaEta over the near-side phi region). The DeltaPhi JK is computed fresh from the raw 2D data to allow the full-eta integration. These are consistent within their respective projections.

2. **Why is ndf=6 for both the diagonal and full-covariance tests?**  
   After symmetrization, each pair of bins `(i, ni)` carries identical values and errors — the 12-bin histogram has only 6 independent degrees of freedom. The diagonal test iterates only the 6 first-of-pair bins. The full covariance SVD independently confirms this: the symmetrized 12×12 matrix has rank 6, so the pseudoinverse yields ndf=6.

3. **Is the DeltaPhi full-cov p=0.0018 a physics effect or an analysis artifact?**  
   It is a real physics effect: the combined pPb+Pbp has a non-zero phi integral because the N_Z weighting favors pPb (which has larger N_Z) and pPb has a large proton-going DeltaEta excess that shows up as a phi normalization offset. This asymmetry is present regardless of the statistical method used.

4. **Are the JK event counts consistent between eta and phi?**  
   Yes: both trees (JackknifeProjection and Jackknife2DData) yield N=10524 (pPb), 6046 (Pbp), 88745 (pp) events — confirmed at runtime.
