# 2026-05-01 audit: maintained pPb/PbP combination and Powheg+EPOS treatment

## Scope

This note documents the **currently implemented** logic for the note-facing combined heavy-ion result in:

- `Plots/20260213_Central/plot_central_combined.cpp:178-488`
- `Systematics/20260329_pPbSystematics/ResultCombinationUtilities.h:56-144`
- `Systematics/20260329_pPbSystematics/CalculateSystematics.cpp:56-335`
- `CommonCode/include/KylesPlotting.h:68-76,798-979`

No combination formula was changed for this audit.

---

## 1. Central-value combination

### 1.1 Implemented input objects

For the combined heavy-ion curve, the maintained code does **not** average already projected `*-result.root` histograms. Instead, it reopens the `*-nosub.root` ingredients and reconstructs the combined result from:

- `hData_<trk>`
- `hMixData_<trk>`
- `hNZData_<trk>`
- `hNZMixData_<trk>`

This is explicit in `plot_central_combined.cpp:246-365` and in the reusable helper `BuildCombinedResultHistogram(...)` in `ResultCombinationUtilities.h:89-144`.

Define, for collision system \(c \in \{\mathrm{pPb},\mathrm{PbP}\}\),

- stored signal histogram: \(\tilde S_c(\Delta y,\Delta\phi)\equiv\)`hData`
- stored mixed histogram: \(\tilde B_c(\Delta y,\Delta\phi)\equiv\)`hMixData`
- stored signal normalization: \(N_c\equiv\)`hNZData->GetBinContent(1)`
- stored mixed normalization: \(M_c\equiv\)`hNZMixData->GetBinContent(1)`

The stored 2D histograms are already normalized per system, so the code first undoes that normalization.

### 1.2 Undoing the per-system stored normalization

From `plot_central_combined.cpp:263-265,302-304` and `ResultCombinationUtilities.h:117-129`, the code reconstructs the numerators as

\[
S_c^{\mathrm{num}}(\Delta y,\Delta\phi)=N_c\,\tilde S_c(\Delta y,\Delta\phi),
\]
\[
B_c^{\mathrm{num}}(\Delta y,\Delta\phi)=M_c\,\tilde B_c(\Delta y,\Delta\phi).
\]

Operationally this is the `Scale(hNZ...)` / `Scale(hNZMix...)` step before the systems are added.

### 1.3 Summing pPb and PbP numerators

The maintained combined heavy-ion signal numerator is built as

\[
S_{\mathrm{HI}}^{\mathrm{num}}(\Delta y,\Delta\phi)
=
S_{\mathrm{pPb}}^{\mathrm{num}}(\Delta y,\Delta\phi)
+
S_{\mathrm{PbP}}^{\mathrm{num}}(\Delta y,\Delta\phi),
\]

and the mixed-event numerator as

\[
B_{\mathrm{HI}}^{\mathrm{num}}(\Delta y,\Delta\phi)
=
B_{\mathrm{pPb}}^{\mathrm{num}}(\Delta y,\Delta\phi)
+
B_{\mathrm{PbP}}^{\mathrm{num}}(\Delta y,\Delta\phi).
\]

This is the `Add(...)` step in `plot_central_combined.cpp:331-347` and `ResultCombinationUtilities.h:117-129`.

### 1.4 Renormalizing by the summed heavy-ion normalizations

The code then divides the summed numerators by the summed normalizations,

\[
\tilde S_{\mathrm{HI}}(\Delta y,\Delta\phi)
=
\frac{S_{\mathrm{pPb}}^{\mathrm{num}}+S_{\mathrm{PbP}}^{\mathrm{num}}}{N_{\mathrm{pPb}}+N_{\mathrm{PbP}}},
\]
\[
\tilde B_{\mathrm{HI}}(\Delta y,\Delta\phi)
=
\frac{B_{\mathrm{pPb}}^{\mathrm{num}}+B_{\mathrm{PbP}}^{\mathrm{num}}}{M_{\mathrm{pPb}}+M_{\mathrm{PbP}}}.
\]

In code this is

- `S_combined->Scale(1. / S_NZ)` with `S_NZ = N_pPb + N_PbP`
- `B_combined->Scale(1. / B_NZ)` with `B_NZ = M_pPb + M_PbP`

from `plot_central_combined.cpp:335-347`, and equivalently in `ResultCombinationUtilities.h:120-129`.

### 1.5 Background subtraction

The maintained 2D background-subtracted combined surface is

\[
C_{\mathrm{HI}}(\Delta y,\Delta\phi)
=
\tilde S_{\mathrm{HI}}(\Delta y,\Delta\phi)
-
\tilde B_{\mathrm{HI}}(\Delta y,\Delta\phi).
\]

This is the `S_combined->Add(B_combined, -1)` step in `plot_central_combined.cpp:350-351` and `signalPPb->Add(backgroundPPb, -1)` in `ResultCombinationUtilities.h:131`.

### 1.6 Projection and presentation normalization

The maintained projection windows are not inferred from the already-projected `result.root` outputs; they are applied to the combined 2D `nosub` ingredients:

- `DeltaPhi`: `ProjectionY(..., 0, 10)`
- `DeltaEta`: `ProjectionX(..., 6, 10)`

from `plot_central_combined.cpp:356-357` and `ResultCombinationUtilities.h:77-86`.

Let \(P_{\Delta\phi,j}\) and \(P_{\Delta y,i}\) denote those projected bin sums. The final displayed 1D observables are then

\[
R_{\Delta\phi,j} = \frac{0.5}{w^{\phi}_j}\,P_{\Delta\phi,j},
\]
\[
R_{\Delta y,i} = \frac{0.5}{w^{y}_i}\,P_{\Delta y,i},
\]

where \(w_j^{\phi}\) and \(w_i^{y}\) are the final 1D bin widths.

This is the `divideByWidth(...)` plus `Scale(1./2)` sequence in `plot_central_combined.cpp:366-369`, mirrored by `NormalizeByBinWidth(...)` plus `Scale(0.5)` in `ResultCombinationUtilities.h:83-86`.

### 1.7 Explicit statement of what is **not** done

The combined heavy-ion central value is **not** built as

\[
\frac{1}{2}\left(R_{\mathrm{pPb}}^{\mathrm{result}} + R_{\mathrm{PbP}}^{\mathrm{result}}\right),
\]

nor by any other average of already-projected `DeltaEta_Result*` / `DeltaPhi_Result*` histograms from `*-result.root`.

Instead, the implemented logic is

\[
(\text{combine `nosub` numerators})
\rightarrow
(\text{renormalize by summed }N_Z\text{ and }N_Z^{\mathrm{mix}})
\rightarrow
(\text{subtract in 2D})
\rightarrow
(\text{project})
\rightarrow
(0.5/\text{bin width}).
\]

That is the same combination strategy used for the maintained systematic builder through `BuildCombinedResultHistogram(...)`.

---

## 2. Statistical uncertainty combination

### 2.1 Implemented jackknife input set

The combined-data statistical bars are built from the `JackknifeProjection<trk>` trees appended in `plot_central_combined.cpp:243-245,273-274,282-313`.

For each selected event \(e\), the tree stores:

- signal normalization contribution \(n^S_e\equiv\)`SignalNZ`
- mixed normalization contribution \(n^B_e\equiv\)`MixNZ`
- projected signal vectors \(s^{y}_{e,i}, s^{\phi}_{e,j}\)
- projected mixed vectors \(b^{y}_{e,i}, b^{\phi}_{e,j}\)

The combined event list is formed as

\[
\mathcal E_{\mathrm{HI}} = \mathcal E_{\mathrm{pPb}} \sqcup \mathcal E_{\mathrm{PbP}},
\]

implemented by concatenating `jackknifePPb` and `jackknifePbP` when `doCombine == true` (`plot_central_combined.cpp:358-364`).

### 2.2 Full projected estimator

Before the final divide-by-bin-width and `0.5` scaling, the estimator used for a projected bin is

\[
\hat\theta_i
=
\frac{\sum_{e\in\mathcal E} s_{e,i}}{\sum_{e\in\mathcal E} n^S_e}
-
\frac{\sum_{e\in\mathcal E} b_{e,i}}{\sum_{e\in\mathcal E} n^B_e},
\]

with `useEta ? event.SignalEta[i] : event.SignalPhi[i]` and the analogous mixed vector in `ComputeProjectedJackknifeSigma(...)` (`plot_central_combined.cpp:90-116`).

This is exactly the bin-by-bin form of the projected estimator after subtraction but before the final presentation scaling.

### 2.3 Leave-one-event-out estimator

For each valid event \(e\), the leave-one-event-out estimator is

\[
\hat\theta_i^{(-e)}
=
\frac{\sum_{e'\neq e} s_{e',i}}{\sum_{e'\neq e} n^S_{e'}}
-
\frac{\sum_{e'\neq e} b_{e',i}}{\sum_{e'\neq e} n^B_{e'}}.
\]

This is implemented in `plot_central_combined.cpp:117-127` as the `signalWithoutEvent` / `mixWithoutEvent` construction. Events for which either denominator would become non-positive are excluded (`plot_central_combined.cpp:103-112,117-121`).

### 2.4 Jackknife variance

Let \(N_{\mathrm{valid}}\) be the number of valid leave-one-out replicas. The maintained jackknife variance is

\[
\sigma^2_{\mathrm{JK},i}
=
\frac{N_{\mathrm{valid}}-1}{N_{\mathrm{valid}}}
\sum_{e\in\mathcal E_{\mathrm{valid}}}
\left(\hat\theta_i^{(-e)}-\hat\theta_i\right)^2,
\]

with

\[
\sigma_{\mathrm{JK},i} = \sqrt{\sigma^2_{\mathrm{JK},i}}.
\]

This is exactly `sqrt((validEvents - 1.0) / validEvents * varianceSum)` in `plot_central_combined.cpp:114-135`.

### 2.5 Where the final presentation scaling enters

The jackknife errors are assigned to the projected histograms **before** the final `divideByWidth` and `Scale(1./2)` operations (`plot_central_combined.cpp:358-369`). Therefore the displayed uncertainty is

\[
\sigma^{\mathrm{plot}}_i = \frac{0.5}{w_i}\,\sigma_{\mathrm{JK},i},
\]

for the relevant projected bin width \(w_i\). The code relies on the histogram error propagation under the same rescaling operations applied to the bin contents.

### 2.6 Which curves receive statistical bars

Only the first heavy-ion histogram in the `input_ZPT_files` / `input_ZPT_files_pbp` vectors receives jackknife errors (`if (i == 0)` in `plot_central_combined.cpp:358-365`). Because the vectors are constructed as

1. data
2. MC (only when `includeMC == true`)

from `plot_central_combined.cpp:180-189`, only the combined **data** histogram gets jackknife statistical bars.

The Powheg+EPOS curve does **not** get jackknife bars in this plotting path.

### 2.7 Lower-panel statistical bars

In the maintained lower panel, the plotted difference histogram is formed as

\[
D_i = H^{(1)}_i - H^{(0)}_i,
\]

and its statistical error is set by

\[
\sigma_{D_i} = \sqrt{\sigma_{1,i}^2 + \sigma_{0,i}^2},
\]

via `setDifferenceErrors(...)` in `CommonCode/include/KylesPlotting.h:68-76`. `PlotCMSDiffResult(...)` applies that helper at `KylesPlotting.h:855-857,934-936`.

So the data lower-panel bars use the quadrature of both curves' statistical uncertainties. For the MC line, the heavy-ion MC contribution has zero statistical error in this path, so the visible difference-bar uncertainty is inherited entirely from the baseline data histogram.

---

## 3. Systematic uncertainty combination

### 3.1 Family-by-family heavy-ion combination

The maintained systematics code does not combine pPb and PbP family magnitudes after projection. Instead, for each family variation pair it rebuilds the combined heavy-ion varied histogram through the same 2D `nosub` combination helper used for the nominal central value.

This occurs in:

- `BuildFamilyHistogramCombined(...)` in `CalculateSystematics.cpp:86-116`
- `BuildCombinedResultHistogram(...)` in `ResultCombinationUtilities.h:89-144`

For a given family variation \(v\), the varied combined observable is therefore

\[
R^{(v)}_{\mathrm{HI},i}
=
\mathcal P\left[
\frac{S^{\mathrm{num},(v)}_{\mathrm{pPb}}+S^{\mathrm{num},(v)}_{\mathrm{PbP}}}{N^{(v)}_{\mathrm{pPb}}+N^{(v)}_{\mathrm{PbP}}}
-
\frac{B^{\mathrm{num},(v)}_{\mathrm{pPb}}+B^{\mathrm{num},(v)}_{\mathrm{PbP}}}{M^{(v)}_{\mathrm{pPb}}+M^{(v)}_{\mathrm{PbP}}}
\right]_i,
\]

where \(\mathcal P\) denotes the implemented projection plus \(0.5/\text{bin width}\) normalization.

### 3.2 Per-bin family magnitude

For each family \(f\), the code scans the available varied inputs and stores the maximum absolute deviation from the nominal combined result,

\[
\delta_{f,i}^{\mathrm{HI}}
=
\max_{v\in f}
\left|
R^{(0)}_{\mathrm{HI},i} - R^{(v)}_{\mathrm{HI},i}
\right|.
\]

This is the `max(value, fabs(...))` step in `CalculateSystematics.cpp:95-115` for combined heavy-ion and `CalculateSystematics.cpp:64-83` for single-system / pp families.

### 3.3 Total systematic in quadrature

Once each included family magnitude is available, the maintained total systematic is

\[
\Delta^{\mathrm{tot}}_{\mathrm{HI},i}
=
\sqrt{\sum_{f\in F_{\mathrm{included}}} \left(\delta_{f,i}^{\mathrm{HI}}\right)^2},
\]

implemented as `sqrt(sum2)` in `CalculateSystematics.cpp:274-290`.

This `Total_DeltaEta` / `Total_DeltaPhi` output is what `plot_central_combined.cpp:408-415` loads as the top-panel systematic band for the combined heavy-ion data curve.

### 3.4 Difference-systematic construction for `(combined - pp)`

When the combined heavy-ion and pp nominals are both present, the code forms

\[
D_i^{(0)} = R^{(0)}_{\mathrm{HI},i} - R^{(0)}_{\mathrm{pp},i}
\]

in `CalculateSystematics.cpp:230-245`.

For each difference family \(f\), the maintained implementation does **not** rebuild a varied `(HI - pp)` histogram and compare it directly to the nominal difference. Instead, it combines the already-built heavy-ion family magnitude and the corresponding pp family magnitude as

\[
\delta^{\mathrm{diff}}_{f,i}
=
\sqrt{\left(\delta^{\mathrm{HI}}_{f,i}\right)^2 + \left(\delta^{\mathrm{pp}}_{f,i}\right)^2},
\]

with missing sides treated as zero. This is the exact `sqrt(combinedValue * combinedValue + ppValue * ppValue)` formula in `BuildDifferenceFamilyHistogram(...)` (`CalculateSystematics.cpp:118-137`).

The maintained `DifferenceTotal` is then

\[
\Delta^{\mathrm{tot}}_{\mathrm{diff},i}
=
\sqrt{\sum_{f\in F^{\mathrm{diff}}_{\mathrm{included}}} \left(\delta^{\mathrm{diff}}_{f,i}\right)^2},
\]

implemented in `CalculateSystematics.cpp:292-312`. `plot_central_combined.cpp:416-423` loads `DifferenceTotal_DeltaEta` / `DifferenceTotal_DeltaPhi` for the lower-panel systematic band of `(combined - pp)`.

### 3.5 Correlation assumptions encoded by the implementation

The current maintained code implies the following correlation model:

1. **Within the heavy-ion combined result, pPb and PbP are treated as correlated through a shared varied reconstruction path.**
   - Each family variation is promoted to a combined heavy-ion varied histogram by pairing the varied pPb and varied PbP inputs and recomputing the 2D combined result before projection.
   - This is the `ZipFiles(...)` + `BuildFamilyHistogramCombined(...)` path in `CalculateSystematics.cpp:47-53,86-116,168-192,247-257`.

2. **Between the combined heavy-ion result and pp, the systematic contributions are treated as uncorrelated in the `Difference*` families and `DifferenceTotal`.**
   - That is precisely the quadrature rule in `BuildDifferenceFamilyHistogram(...)` (`CalculateSystematics.cpp:118-137`).

---

## 4. MC-curve treatment

### 4.1 Input roots

When `includeMC == true`, `plot_central_combined.cpp:186-189` appends the maintained official MC roots

- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_<PPB_MC_TAG>_ZPT<range>-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_<PPB_MC_TAG>_ZPT<range>-nosub.root`

and labels the extra curve as `Powheg+EPOS` (`plot_central_combined.cpp:190-195`).

### 4.2 Heavy-ion MC combination

The MC combination is performed with the **same** heavy-ion `nosub` combination path as the combined data curve:

- the loop over `input_ZPT_files` / `input_ZPT_files_pbp` runs once for data (`i = 0`) and once for MC (`i = 1`) when `includeMC == true`;
- each pass constructs `S_combined`, `B_combined`, subtracts, projects, divides by bin width, and scales by `0.5` (`plot_central_combined.cpp:325-375`).

So the MC curve is not read from a pre-combined heavy-ion ROOT file; it is reconstructed on the fly from the official pPbMC and PbPMC `nosub` inputs.

### 4.3 Why the MC curve is line-style and bar-free

The maintained style arrays are

- `lineStyles = {0, 0, 1}`
- `labels = {"pp", <heavy-ion data>, "Powheg+EPOS"}`

from `plot_central_combined.cpp:384-387,190-195`.

In `PlotCMSDiffResult(...)`, statistical bars are drawn only when

\[
\texttt{errorBars == true} \quad\text{and}\quad (\texttt{linestyle} = 0 \text{ or } -1),
\]

as seen in `KylesPlotting.h:834-836,925-926,956-970`.

Therefore the MC curve (`linestyle = 1`) is drawn as a `HIST` line and does **not** receive statistical bars in either panel.

This is consistent with the implementation and with the maintained frozen-plot behavior expected for the note-facing central-result overlays.

---

## 5. Operational conclusion for this audit

The maintained combined pPb/PbP result is currently constructed by combining the `nosub.root` ingredients first, not by averaging projected `result.root` histograms. The combined-data statistical bars come from a concatenated pPb+PbP event-level jackknife, while the systematic uncertainties come from family-by-family recomputation of the combined heavy-ion result and quadrature totals. The `(combined - pp)` systematic lower band treats the heavy-ion and pp sides as uncorrelated. The Powheg+EPOS curve enters as a separately combined heavy-ion MC line with no statistical bars in this plotting path.
