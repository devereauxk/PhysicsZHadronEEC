# pp Normalization Investigation Plan (Event Mixing / Weighting)

## Objective
Determine why pp `DeltaPhi` in `trkPT 1_2` shows a negative offset in GEN MC, despite mixed-event normalization expecting near-zero integral after subtraction.

## Preliminary finding (confirming current guess)
From quick checks on existing outputs:
- `pp_trkResidual_evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350-result.root`  
  `DeltaPhi_Result1_2` sum: **~ +0.0516** (near zero)
- `pythiaMC_Gen_nominal_evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350-result.root`  
  `DeltaPhi_Result1_2` sum: **~ -3.903** (clearly non-zero)
- `pythiaMC_Gen_nominal_evtWeightOff_ZV6_trkV24_nmix10_ZPT40_350-result.root`  
  `DeltaPhi_Result1_2` sum: **~ +0.0030** (near zero)

This supports the hypothesis that the main issue is in GEN MC normalization under event weighting.

## Primary code suspect to investigate first
`MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp` in mixed-event block:
- Outer variable is initialized:
  - `float eventWeightMix = eventWeightSignal;`
- Inside `if (par.mix)` it redeclares:
  - `float eventWeightMix = 1;`

This inner declaration shadows the outer variable. As a result, computed mix-event weights are discarded, while downstream normalization/fills use the outer `eventWeightMix` (effectively `eventWeightSignal`). This can bias mixed-event normalization, especially when `UseEventWeight=true`.

## Investigation workflow for analyzer

### 1) Reproduce and log baseline discrepancy
- Use `pp-plotpp.sh` outputs at `ZPT40_350` for `trkPT={1_2,2_4,4_10}`.
- For each bin and each set (`pp_nominal`, `pp_ZResidual`, `pp_trkResidual`, `pythiaMC_Gen_nominal`):
  - Record integral of `DeltaPhi_Result*`, `DeltaEta_Result*`.
  - Record sums of `hData_*`, `hMixData_*`, `hDataAll_*`.
  - Record `hNZData_*` and `hNZMixData_*`.
- Compare `evtWeightOn` vs `evtWeightOff`.

### 2) Weight-path instrumentation (temporary debug prints)
- Add controlled debug output in `getDphi(...)` for a small event sample:
  - `eventWeightSignal`
  - intended `eventWeightMix` (computed from mix event)
  - final weight used in mixed-track filling
  - contributions to `nZ`
- Confirm whether mixed tracks are using signal-event weights when `par.mix=true`.

### 3) Isolate VZ vs event-weight effects
Run GEN MC in a toggle matrix (same kinematics/config):
- A: `UseEventWeight=false`, `UseVZWeight=true`
- B: `UseEventWeight=true`, `UseVZWeight=false`
- C: `UseEventWeight=true`, `UseVZWeight=true`
- D: `UseEventWeight=false`, `UseVZWeight=false`

Goal: identify which factor drives non-zero integral and whether interaction terms exist.

### 4) Fix candidate and controlled validation
- Correct the mixed-event weight scoping bug (remove shadowing; ensure computed mix weight is the one used for:
  - `nZ` accumulation in mix mode,
  - mixed-track histogram filling).
- Re-run same matrix and compare pre/post values.

### 5) Physics/normalization acceptance criteria
- For each trk bin, post-subtraction `DeltaPhi_Result*` integral should be near zero for both corrected data and GEN MC (within statistical tolerance).
- `evtWeightOn` should not introduce a large GEN-only offset relative to `evtWeightOff`.
- Closure shape agreement should improve or remain stable in all three trk bins.

### 6) Regression checks
- Verify no degradation in:
  - pp closure plots (`Plots/20260213_Central/plot_pp.cpp` outputs),
  - pPb/PbP workflows using same analyzer binary.
- Recheck `hNZData/hNZMixData` consistency and expected scaling by `nMix`.

## Deliverables
- Patch implementing fix (if confirmed).
- Short table of integrals before/after by trk bin and weight-toggle scenario.
- One-paragraph conclusion:
  - confirmed root cause or alternative cause,
  - whether hypothesis (“data near zero, GEN is issue”) remains true after fix.
