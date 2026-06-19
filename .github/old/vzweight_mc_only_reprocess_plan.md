# Analyzer plan: MC-only VZ application fix and selective reprocessing

## Goal

Treat the current finding as a physics-policy bug fix: `VZWeightFile` must affect **MC simulation only**, never data. Audit work in the current workspace confirms that some official data paths still receive VZ weights, so a targeted reprocessing is required.

Your job is to patch the affected code/runner paths, rerun only the impacted products, refresh the downstream plots, sync the changed note figures to Overleaf, and write a completion summary for reviewer sign-off.

Do **not** change unrelated pp correction derivations, and do **not** rerun unaffected MC-only workflows just because they mention `VZWeightFile`.

## Confirmed audit findings

### 1. Runtime bug in the analysis binaries

The main-analysis binary applies the VZ correction whenever `VZWeightFile` is non-empty, without checking whether the input is data:

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`
  - signal-event VZ weighting around lines 294-303
  - mixed-event VZ weighting around lines 409-417
  - `par.VZWeightFile` / `par.isData` setup around lines 699-715

The same pattern is present in the pp energy-extrapolation analysis:

- `MainAnalysis/20260222_EnergyExtrapolation/CorrelationAnalysis.cpp`
  - VZ setup around lines 109-110 and 255
  - event weighting around line 134

For consistency/hardening, the same MC-only guard should also be applied in the corresponding correction-workflow binaries:

- `MainAnalysis/20260115_ZCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp`

Those two workflows are currently MC-only in their official wrappers, so they are **not** the reason official data plots changed, but the binaries should still be made policy-correct.

### 2. Confirmed affected data-path runner scripts

These scripts currently send `VZWeightFile` into data jobs that then get weighted because of the runtime bug above:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
  - `pPb_nominal`, `pPb_ZResidual`, `pPb_trkResidual`
  - `PbP_nominal`, `PbP_ZResidual`, `PbP_trkResidual`

- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
  - weighted pPb branch (`DOPPB=2`)
  - weighted PbP branch (`DOPBP=2`)
  - note: the pp branch in this script does **not** pass VZ to data

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
  - pp data at 5.02 TeV receives `--VZWeightFile`

### 3. Confirmed safe / not-in-scope workflows

These are **not** official fallout from the current bug and should not be rerun unless you uncover a new issue:

- pp central/closure support for `plot-pp`
  - `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
  - `Plots/20260213_Central/plot-pp.sh`
  - pp data jobs there do **not** receive `VZWeightFile`

- data/MC comparison support
  - `MainAnalysis/20241102_ZhadronVsZPt/check-dataMCComparison.sh`
  - `Plots/20260213_Central/plot-dataMCComparison.sh`
  - the pPb/PbP data jobs there do **not** receive `VZWeightFile`

- pp PU study
  - `MainAnalysis/20241102_ZhadronVsZPt/pp-PU.sh`
  - pp data jobs there do **not** receive `VZWeightFile`

- Z-correction derivation
  - `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh`
  - `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
  - these official wrappers are MC-only

- track-residual derivation
  - `MainAnalysis/20251211_ResidualCorrection/workflow/pythia-analysis.sh`
  - `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`
  - these official wrappers are MC-only

- MC-only closure / audit runners
  - `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
  - `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
  - `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh`
  - `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`
  - `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-zstep-audit.sh`

## Downstream plot fallout

### Required reruns

These products are downstream of the confirmed bad data-path weighting and must be refreshed.

#### A. pPb/PbP central-value analysis outputs

Regenerate the affected main-analysis ROOT outputs from:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`

Affected families:

- `pPb_nominal_*`
- `pPb_ZResidual_*`
- `pPb_trkResidual_*`
- `PbP_nominal_*`
- `PbP_ZResidual_*`
- `PbP_trkResidual_*`

These feed the official pPb/PbP central-value and final-result plotters.

#### B. pPb/PbP central-value / closure figures

Refresh:

- `Plots/20260213_Central/plot-central.sh`

This plotter reads pPb/PbP data outputs from `central.sh`, so the pPb/PbP central-value comparison panels are affected.

#### C. pPb-vs-PbP combining overlay figures

Refresh:

- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`

This plotter reads:

- `pPb_trkResidual_*`
- `PbP_trkResidual_*`
- corresponding MC GEN references

so the pPb/PbP separation-comparison section is affected.

#### D. pp energy extrapolation products and plots

Refresh:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `Plots/20260213_Central/plot-energyExtrapolation.sh`

The workflow currently sends `VZWeightFile` into pp data, so the pp energy-extrapolation ROOT outputs and derived figures are affected.

#### E. combined final-result plots

Refresh:

- `Plots/20260213_Central/plot-central-combined.sh`

This plotter consumes **both**:

- pPb/PbP central outputs from `central.sh`
- pp energy-extrapolation outputs from `pp_EExtrapolation_*`

So the combined final-result plots are definitely stale until both upstream pieces are rerun.

#### F. pPb/PbP Z-mass figures used in the note

Refresh the pPb/PbP mass figures derived from:

- `Plots/20260213_Central/plot-zmass.sh`

Reason:

- `plot_zmass.cpp` reads `pPb_nominal_*` and `PbP_nominal_*`
- `OverleafZHadronInPPb/src/z_reco.tex` includes `figures/z_reco/zmass_ppb.pdf` and `figures/z_reco/zmass_pbp.pdf`

Do **not** change the pp mass figure unless you find an independent bug in the pp input path.

### Optional / diagnostic reruns

These are not the primary official fallout, but they should be refreshed if the working convention is that all validation outputs must match the corrected MC-only policy.

#### G. pPb/PbP VZ application diagnostics

- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `Plots/20260213_Central/plot-VZ.sh`

This is the runner where the pPb/PbP weighted diagnostic currently weights data as well as MC. If the reviewer wants the VZ-application closure products to remain policy-correct, rerun them after the fix. If you do rerun them, interpret them strictly as **MC reweighted to data**, not a symmetric data+MC transform.

#### H. Z-spectrum plots, if they are still maintained

- `Plots/20260213_Central/plot-zspectrum.sh`

Reason:

- `plot_zspectrum.cpp` reads pPb/PbP `*_ZResidual_*`
- it also reads `pp_EE_*`

so it is downstream of both affected pPb/PbP central outputs and affected pp energy-extrapolation outputs.

## Past `.github` instructions/execution reports impacted by this finding

Treat the following documents as historically useful, but do **not** trust their cited outputs for the affected families until the MC-only rerun is complete:

- `.github/pPb_PbP_relabel_full_recalculation_instructions.md`
- `.github/pPb_PbP_relabel_full_recalculation_execution.md`
- `.github/pPb_PbP_official_recovery_and_revalidation_instructions.md`
- `.github/pPb_PbP_official_recovery_and_revalidation_execution.md`
- `.github/pPb_PbP_newVZFix_official_deployment_instructions.md`
- `.github/pPb_PbP_newVZFix_official_deployment_execution.md`
- `.github/pPb_PbP_VZWeight_Fix_Plan.md`
- `.github/pPb_PbP_VZWeight_Fix_execution.md`
- `.github/pPb_PbP_VZWeight_Fix_followup_instructions.md`
- `.github/pPb_PbP_VZWeight_Fix_followup_execution.md`
- `.github/overleaf_impact_reprocess_plan_after_eventweight_fix.md`
- `.github/pp-study-summary-20260317.md`
- `.github/pp_normalization_investigation_execution.md`
- `.github/pp_normalization_followup_execution.md`

These documents are not necessarily wrong about tags or sample mappings, but any outputs produced by the affected runners should be considered stale under the corrected MC-only policy.

## Required implementation approach

### 1. Fix the binaries first

Patch the VZ application so that it is MC-only in:

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`
- `MainAnalysis/20260222_EnergyExtrapolation/CorrelationAnalysis.cpp`
- `MainAnalysis/20260115_ZCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp`

Expected logic:

- keep the existing `VZWeightFile != ""` behavior
- additionally require `par.isData == false` before multiplying in the VZ correction

Do not introduce broad behavior changes beyond that.

### 2. Remove VZ-weight arguments from data jobs in the affected wrappers

Patch:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`

Rules:

- MC jobs should continue to receive the same VZ files they already use.
- Data jobs must no longer receive `--VZWeightFile`.
- In `closure-VZ.sh`, keep the pPb/PbP weighted branch meaningful as “MC with VZ reweighting vs unchanged data”.

### 3. Use script-driven reruns

Follow the repository convention:

- build from the dated directory you touch
- use existing scripts rather than ad hoc command chains for official products

Required environment bootstrap before builds/runs:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
```

## Ordered execution plan

Execute sequentially unless a step explicitly states otherwise.

### Step 1. Patch and rebuild

Rebuild the touched code in:

- `CommonCode/`
- `MainAnalysis/20241102_ZhadronVsZPt/`
- `MainAnalysis/20260222_EnergyExtrapolation/`
- `MainAnalysis/20260115_ZCorrection/`
- `MainAnalysis/20251211_ResidualCorrection/`
- `Plots/20260213_Central/` if any plotting helper changed

### Step 2. Remove stale affected outputs before rerun

Only remove outputs in the affected families so the refreshed products are unambiguous:

- pPb/PbP central ROOT outputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/`
- pp energy-extrapolation ROOT outputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EExtrapolation_*`
- refreshed plot families in `Plots/20260213_Central/plots/`:
  - `central/`
  - `central_overlay_PPbPbP/`
  - `central_combined/`
  - `energyExtrapolation/`
  - `zmass/` for the pPb/PbP mass figures
  - optional: `VZ/` and `zspectrum/` if you rerun those

Be surgical. Do not delete unaffected pp closure outputs or MC-only correction products.

### Step 3. Rerun pPb/PbP central outputs

From `MainAnalysis/20241102_ZhadronVsZPt/` rerun:

- `central.sh 0 1 1`

Use the current official pPb/PbP correction files already in place:

- VZ: current official pPb/PbP VZ files
- Z: `ZV6`
- track residual: `trkV24`

Do not regenerate the Z or track correction files in this task.

### Step 4. Rerun the pPb/PbP central-result plot families

From `Plots/20260213_Central/` rerun:

- `plot-central.sh`
- `plot-central-overlay-PPbPbP.sh`

### Step 5. Rerun the pp energy-extrapolation chain

From `MainAnalysis/20260222_EnergyExtrapolation/workflow/` rerun the pp workflow that produces the `EEV3` outputs, then from `Plots/20260213_Central/` rerun:

- `plot-energyExtrapolation.sh`

Do not touch pp Z or track correction derivations; only refresh the pp energy-extrapolation products that depended on the bad data weighting.

### Step 6. Rerun the combined final-result figures

From `Plots/20260213_Central/` rerun:

- `plot-central-combined.sh`

This must happen **after** both Step 3 and Step 5.

### Step 7. Refresh the pPb/PbP Z-mass note figures

Rerun the pPb/PbP mass plots derived from the refreshed central outputs.

If `plot-zmass.sh` still hardcodes outdated pPb/PbP tags, patch it narrowly so the pPb/PbP figures are regenerated from the refreshed official tag while leaving the pp path unchanged.

### Step 8. Optional diagnostic refresh

If you are asked to keep the validation/diagnostic family in sync, rerun:

- `closure-VZ.sh` for weighted pPb/PbP branches
- `plot-VZ.sh`
- `plot-zspectrum.sh` if the figure family is still maintained

## Validation requirements

You must verify all of the following before declaring completion.

### Code-path validation

- grep or equivalent evidence shows the affected data jobs no longer receive `--VZWeightFile` in:
  - `central.sh`
  - `closure-VZ.sh`
  - `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`

- code inspection shows the VZ correction is guarded so it applies only when:
  - `VZWeightFile` is non-empty
  - and the event is **not** data

### Scope validation

- no pp correction derivation reruns were done in:
  - `MainAnalysis/20260115_ZCorrection/`
  - `MainAnalysis/20251211_ResidualCorrection/`

- `plot-pp` support outputs were left alone unless you found a new independent bug

### Output validation

- refreshed pPb/PbP central plots exist
- refreshed pPb/PbP overlay plots exist
- refreshed pp energy-extrapolation plots exist
- refreshed combined final-result plots exist
- refreshed pPb/PbP z-mass figures exist

If you rerun diagnostics:

- refreshed pPb/PbP VZ diagnostic plots exist
- refreshed z-spectrum plots exist

## Overleaf update requirements

Update only the figure families that actually changed in this task.

Use the repository’s current Overleaf policy:

- preserve the source basename exactly when copying
- update `\includegraphics` references if the source filename changes
- do not rename refreshed figures to stale legacy basenames just to match old note references

At minimum, audit and update the Overleaf references for:

- combined final-result figures
- pPb/PbP separation overlay figures
- pp energy-extrapolation figures if they are used in the note
- pPb/PbP z-mass figures
- optional VZ diagnostics only if the note actually references them

## Final deliverable

Write a completion summary `.md` that includes:

- exact files patched
- commands run
- outputs regenerated
- which figure families were updated in Overleaf
- validation evidence that VZ weights now apply to MC only
- confirmation that pp correction derivations were left unchanged
- any failures/retries and how they were resolved
- final status and any remaining caveats
