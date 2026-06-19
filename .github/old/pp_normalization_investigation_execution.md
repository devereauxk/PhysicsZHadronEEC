# pp Normalization Investigation — Execution Summary (Analyzer)

Plan executed from `.github/pp_normalization_investigation_plan.md`.

## 1) Baseline discrepancy reproduction

Collected metrics for `ZPT40_350`, `trkPT={1_2,2_4,4_10}`, sets `{pp_nominal, pp_ZResidual, pp_trkResidual, pythiaMC_Gen_nominal}` for tags `evtWeightOn/Off_ZV6_trkV24_nmix10`.

### Key baseline check (DeltaPhi integral, trkPT=1_2)

| Sample | evtWeightOn | evtWeightOff |
|---|---:|---:|
| `pp_nominal` | 0.00862696 | 0.00862696 |
| `pp_ZResidual` | 0.0509305 | 0.0509305 |
| `pp_trkResidual` | 0.0516209 | 0.0516209 |
| `pythiaMC_Gen_nominal` | -3.90324 | 0.00295677 |

Observation: data-like chains remain near zero while GEN shows a large negative offset only when `UseEventWeight=true`.

## 2) Weight-path instrumentation (temporary)

Added temporary debug prints in `CorrelationAnalysis.cpp::getDphi(...)` and ran a small GEN sample (`Fraction=0.001`).

Representative debug output showed:
- `eventWeightSignal` and `usedEventWeightMix` were identical
- `computedMixWeight` differed event-by-event
This confirmed mixed-track filling was using the signal-event weight, not the computed mixed-event weight.

## 3) Toggle matrix (GEN) pre-fix

Scenarios:
- A: `UseEventWeight=false`, `UseVZWeight=true`
- B: `UseEventWeight=true`, `UseVZWeight=false`
- C: `UseEventWeight=true`, `UseVZWeight=true`
- D: `UseEventWeight=false`, `UseVZWeight=false`

### DeltaPhi integral comparison (pre vs post)

| Scenario | trkPT | pre | post |
|---|---|---:|---:|
| A | 1_2 | 0.00295677 | 0.00295579 |
| A | 2_4 | -0.00458651 | -0.00470556 |
| A | 4_10 | -0.000956497 | -0.000866842 |
| B | 1_2 | -3.90415 | 0.0262811 |
| B | 2_4 | -2.0676 | -0.00360028 |
| B | 4_10 | -0.667786 | 0.00382917 |
| C | 1_2 | -3.90324 | 0.0262882 |
| C | 2_4 | -2.0673 | -0.00359735 |
| C | 4_10 | -0.667895 | 0.00383951 |
| D | 1_2 | 0.00296183 | 0.00296183 |
| D | 2_4 | -0.00470409 | -0.00470409 |
| D | 4_10 | -0.000871127 | -0.000871127 |

## 4) Fix applied

In `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`, fixed mixed-event weight scoping:
- Removed effective shadowing behavior by assigning `eventWeightMix = computedMixWeight` in the mixed-event branch.
- Temporary debug instrumentation was removed after confirmation.

## 5) Post-fix validation against acceptance criteria

- GEN `DeltaPhi` integrals moved from large negative offsets (B/C pre) to near-zero values in all trk bins post-fix.
- Example (`trkPT=1_2`):
  - B: -3.90415 -> 0.0262811
  - C: -3.90324 -> 0.0262882
- `evtWeightOn` no longer shows large GEN-only offset relative to `evtWeightOff` for this study setup.
- `hNZMixData/hNZData` remains consistent with `nMix~10` before and after (small expected statistical fluctuation).

## 6) Regression checks

- Re-ran `Plots/20260213_Central/plot-pp.sh` successfully (updated pp plots produced).
- Ran smoke checks with rebuilt analyzer on pPb and PbP inputs (`Fraction=0.0005`), both completed and wrote ROOT outputs:
  - `/tmp/pPb_smoke_norm.root`
  - `/tmp/PbP_smoke_norm.root`

## Artifacts

- Baseline metrics: `.../files/ppnorm_baseline_pre.csv`
- Matrix pre-fix: `.../files/ppnorm_matrix_pre.csv`
- Matrix post-fix: `.../files/ppnorm_matrix_post.csv`

## Conclusion

Root cause confirmed: mixed-event normalization used the signal-event weight due to mixed-weight scoping behavior. After fixing the mixed weight assignment, the GEN event-weight-induced offset disappeared and normalization behavior matched expectations across all requested trk bins.