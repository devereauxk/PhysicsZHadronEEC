# 20260506 DeltaPhi integral nonclosure audit

## Prior reference

The prior quantitative artifact for this issue is:

- `Plots/20260213_Central/presentations/central_combined_bin10x10_20260502_integrals.tsv`

That table is the source of the previously quoted worst-case 10-bin residual fractions, including:

- `ZPT30_500, trkPT4_15`: pp `5.80%`, combined heavy-ion (`pPb+PbP`) `5.98%`

I did not find an earlier repo-local markdown note dedicated to this specific audit; the TSV above is the maintained in-repo reference.

## Question being checked

For the displayed `DeltaPhi` result histograms, define the projected same-event and mixed-event integrals

\[
I_{\mathrm{sig}} = \int d\Delta\phi \; S(\Delta\phi), \qquad
I_{\mathrm{mix}} = \int d\Delta\phi \; B(\Delta\phi),
\]

where the `DeltaPhi` projection is taken over the code's nominal `DeltaEta < 0` sideband window. The residual subtraction offset is

\[
\Delta I = I_{\mathrm{sig}} - I_{\mathrm{mix}}, \qquad
f = \frac{|\Delta I|}{|I_{\mathrm{sig}}|}.
\]

The concern was whether the large `f` values in the exploratory `10 x 10` result study came from:

1. a trivial result-stage bug,
2. finite `nMix` statistics,
3. or a pre-existing effect already present in the frozen `20 x 20` result path.

## Code audit

The 10-bin study does **not** use a separate fill path. The same reflected pair filling is used in both the frozen and exploratory productions:

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp:610-613`

```cpp
h->Fill(trackDeta,  trackDphi,  weight);
h->Fill(-trackDeta, trackDphi,  weight);
h->Fill(trackDeta,  trackDphi2, weight);
h->Fill(-trackDeta, trackDphi2, weight);
```

So the large 10-bin residual is **not** caused by a 10-bin-only fill asymmetry.

The relevant result-stage projection logic is in:

- `MainAnalysis/20241102_ZhadronVsZPt/makeProjection.C:53-90`

The `DeltaPhi` projection window is built with

```cpp
const double epsilonBelowZero = std::nextafter(0.0, -std::numeric_limits<double>::infinity());
window.DeltaPhiXLast = input->GetXaxis()->FindFixBin(epsilonBelowZero);
```

The important finding is that, for these ROOT histograms, this lookup selects the **first nonnegative** `DeltaEta` bin rather than the last strictly negative one.

For the worst-bin pp study output (`ZPT30_500`, `trkPT4_15`):

- `10 x 10` histogram:
  - selected last x bin = `6`
  - x bin 6 is `[0, 0.8)`
- frozen `20 x 20` histogram:
  - selected last x bin = `11`
  - x bin 11 is `[0, 0.4)`

So the nominal `DeltaEta < 0` projection is actually integrating

- `DeltaEta < 0.8` in the 10-bin study,
- `DeltaEta < 0.4` in the frozen 20-bin study,

rather than a strictly negative half-plane.

This is the main source of the apparent nonclosure increase in the coarse-binned study.

## 10-bin vs frozen 20-bin comparison

Using the currently produced roots, the same residual pattern is already present in the frozen outputs:

| Selection | pp 10-bin | pp 20-bin | HI 10-bin | HI 20-bin |
| --- | ---: | ---: | ---: | ---: |
| `ZPT0_500, trkPT0.5_15` | `0.92%` | `0.56%` | `0.32%` | `0.18%` |
| `ZPT30_500, trkPT0.5_2` | `0.93%` | `0.53%` | `0.80%` | `0.81%` |
| `ZPT30_500, trkPT2_4` | `3.45%` | `2.32%` | `2.45%` | `1.81%` |
| `ZPT30_500, trkPT4_15` | `5.79%` | `4.06%` | `5.98%` | `4.31%` |

So:

- **yes**, the effect was already present in the frozen 20-bin result;
- the exploratory 10-bin study made it look worse because the projection window accidentally includes a wider positive-`DeltaEta` slice.

## `nMix` stress test

I ran a worst-bin full-dataset audit rerun with unique throwaway tags and `nMix = 50`:

- pp: `pp_trkResidual_EEV5_ZV9_trkV27_nmix50_bin10x10_audit_20260506`
- pPb: `pPb_trkResidual_ZV9_trkV27_nmix50_bin10x10_audit_20260506`
- PbP: `PbP_trkResidual_ZV9_trkV27_nmix50_bin10x10_audit_20260506`

All were run with the reproducible slicing convention:

- `NTHREAD=25`
- `NSLICE_FACTOR=1`

For the worst note-bin (`ZPT30_500`, `trkPT4_15`), increasing `nMix` did **not** materially change the residual:

| System | `nMix=10` signal | `nMix=10` mix | `nMix=10` residual | `nMix=50` signal | `nMix=50` mix | `nMix=50` residual |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| pp 10-bin | `3.55895` | `3.35286` | `0.206085` | `3.55895` | `3.34871` | `0.210241` |
| combined HI 10-bin | `4.56341` | `4.29066` | `0.272752` | `4.56341` | `4.29212` | `0.271293` |

So the large residual is **not** explained by finite `nMix=10` statistics.

## Strictly negative-window cross-check

To isolate the projection-window issue, I re-evaluated the same already-produced roots using a truly strict negative-side selection (`FindFixBin(-1e-6)` rather than the current `nextafter(0,-inf)` lookup).

For the worst note bin (`ZPT30_500`, `trkPT4_15`):

| System | Current window signal | Current window mix | Current residual | Strict-negative signal | Strict-negative mix | Strict-negative residual |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| pp 10-bin | `3.55895` | `3.35286` | `0.206085` | `2.45310` | `2.45257` | `0.000521` |
| pp 20-bin | `3.03623` | `2.91309` | `0.123136` | `2.45310` | `2.45257` | `0.000521` |
| combined HI 10-bin | `4.56341` | `4.29066` | `0.272752` | `3.18805` | `3.18058` | `0.007475` |
| combined HI 20-bin | `3.91896` | `3.75004` | `0.168917` | `3.18805` | `3.18058` | `0.007475` |

This is the clearest diagnostic result of the audit:

1. With the current window logic, the 10-bin and 20-bin integrals differ substantially.
2. With a truly strict negative-`DeltaEta` window, the 10-bin and 20-bin answers become identical.
3. The large apparent residual collapses to a tiny leftover offset.

So the dominant effect is a **projection-window bug**, not a mixed-event normalization instability.

## Conclusion

The residual integral discrepancy is now understood:

1. The exploratory `10 x 10` study did not introduce a new fill bug.
2. The same issue already existed in the frozen `20 x 20` result.
3. The dominant source is the result-stage projection-window lookup in `makeProjection.C`, which currently includes the first nonnegative `DeltaEta` bin instead of stopping at `DeltaEta < 0`.
4. The coarser `10 x 10` study amplifies that bug because its extra included bin is twice as wide in `DeltaEta` (`0.8` instead of `0.4`).
5. Raising `nMix` from `10` to `50` does not fix the issue, so this is not primarily a finite-mixing-statistics problem.
6. After enforcing a truly strict negative-side window in the audit, the 10-bin and 20-bin results agree and the residual becomes very small.

No maintained production code was changed in this audit note; only the diagnosis was carried out.
