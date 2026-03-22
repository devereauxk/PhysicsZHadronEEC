# Reviewer plan: propagate the 20260320 skim-VZ-off VZ weights through the full pp / pPb / PbP stack

## Reviewer conclusion

Yes: the VZ weights produced today are **not** in agreement with the previously deployed VZ settings, and this is true for **all three** collision systems:

- `pp`
- `pPb`
- `PbP`

This is not a cosmetic difference. The fresh `20260320` fits move substantially relative to the previously used roots, so the new VZ files must be propagated through the whole downstream correction and plotting chain.

## Evidence for non-agreement

The reviewer compared the fitted `TF1("VZ_reweight")` stored in the old and new ROOT files, sampling the functions every `0.5` in `VZ in [-15, 15]`.

### pp

- old:
  - `Plots/20251001_pPbVZReweighting/summary/20260317_ZV6_ZPT0_350_VzReweightFits_pp.root`
- new:
  - `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`

Observed difference:

- `max_abs = 0.495779`
- `rms = 0.181752`

This is a strong disagreement, especially in the tails.

### pPb

- old:
  - `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root`
- new:
  - `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`

Observed difference:

- `max_abs = 0.534684`
- `rms = 0.288044`

Again, this is not compatible with “same weight within noise”.

### PbP

- old:
  - `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root`
- new:
  - `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

Observed difference:

- `max_abs = 0.631582`
- `rms = 0.332873`

This is the largest change of the three and is clearly incompatible with reusing the previous PbP-derived downstream products.

## Physics-policy standard you must enforce

Treat the following as mandatory repo policy for this task.

### 1. Skim `VZWeight` must never be used

Do **not** use the embedded skim `VZWeight` branch as a fallback weight source in any official production path.

### 2. VZ-weight derivation stage must run with VZ explicitly off

When deriving a VZ weight:

- pass `--UseVZWeight false`
- do **not** pass a `VZWeightFile`

This is the raw stage used to build the Data / MC VZ ratio that becomes the new VZ fit.

### 3. Downstream MC correction / analysis stages must use the external VZ root explicitly

After the VZ fit is derived, every MC stage that should receive a VZ correction must use:

- `--UseVZWeight true`
- `--VZWeightFile <20260320 root>`

### 4. Data stages must not receive VZ weighting

For data jobs:

- pass `--UseVZWeight false`
- do not pass `VZWeightFile`

### 5. Unsafe default behavior must be removed from the analysis code

The current behavior is unsafe because several analyzers still fall back to `MZSignal->VZWeight` / `MMix->VZWeight`.

The analyzer must change this so the code reflects the policy above.

## Required code changes

### A. Change default VZ behavior in the main analysis binary

Patch:

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`

Required behavior:

1. `UseVZWeight` must **not** default to `true`.
2. If `UseVZWeight` is `false`, apply no VZ weight.
3. If `UseVZWeight` is `true`:
   - require an explicit external `VZWeightFile`
   - apply it only for MC (`!isData`)
4. Do **not** fall back to the skim `VZWeight` branch.

Reviewer preference:

- default `UseVZWeight` to `false`
- when `UseVZWeight=true` and `VZWeightFile=""`, fail loudly with a clear error message rather than silently using the skim branch

Apply this consistently to both:

- signal-event weighting
- mixed-event weighting

### B. Remove skim-VZ fallback from correction / extrapolation analyzers too

Patch the same policy into:

- `MainAnalysis/20260115_ZCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20260222_EnergyExtrapolation/CorrelationAnalysis.cpp`

These files currently still fall back to `MZSignal->VZWeight` when `VZWeightFile` is absent.

They must instead follow the same standard:

- no skim fallback
- explicit file for MC if VZ is enabled
- no VZ for data

## Required script audit and updates

Audit every running script that references VZ roots or depends on a `CorrelationAnalysis` binary that previously allowed skim-VZ fallback.

At minimum, update these files so they use the **20260320** VZ roots explicitly where appropriate.

### pp consumers

- `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pythia-analysis.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/check-dataMCComparison.sh`

### pPb / PbP consumers

- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-zstep-audit.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`

### VZ-derivation runner

Keep or refine:

- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ-20260320.sh`

This script already encodes the correct raw/application split and should be used as the reference behavior.

## Official VZ roots to use everywhere after the audit

### pp

- `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`

### pPb

- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`

### PbP

- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

After your script audit, no official rerun in this task should still point at:

- `20260311_*`
- `20260317_*`
- `20260318_*`
- `20260319_*`

unless the script is explicitly historical / archival and you document why it was intentionally left untouched.

## Required rerun plan

Use date-specific outputs for this propagation so the reviewer can validate the refreshed chain before any overwrite of old products.

Recommended downstream naming:

- Z corrections:
  - `20260320_ZCorrection_V6_skimVZOff_pp_zPt0-500.root`
  - `20260320_ZCorrection_V6_skimVZOff_PPb_zPt0-500.root`
  - `20260320_ZCorrection_V6_skimVZOff_PbP_zPt0-500.root`
- track residuals:
  - `20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt`
  - `20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt`
  - `20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt`
- central / comparison tags:
  - `ZV6_trkV24_vz20260320_nmix10`
  - `ZV6_trkV24_EEV3_vz20260320_nmix10`

You may choose a slightly different date-tagged naming scheme, but it must be:

- collision-system specific where needed,
- consistent across workflows and plotters,
- distinct from the existing deployed tag families.

### Stage 1. Rebuild patched binaries

Rebuild the touched modules:

- `MainAnalysis/20241102_ZhadronVsZPt`
- `MainAnalysis/20260115_ZCorrection`
- `MainAnalysis/20251211_ResidualCorrection`
- `MainAnalysis/20260222_EnergyExtrapolation`
- `Plots/20251001_pPbVZReweighting`

### Stage 2. Confirm / reuse the new VZ roots

Use the existing 20260320 VZ roots from:

- `.github/vz_note_plot_followup_execution.md`

Do not regenerate them again unless your code-change validation requires it.

### Stage 3. Re-derive Z corrections with the 20260320 VZ roots

#### pp

Run:

- `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh`

with:

- `VZWeightFile=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`

#### pPb / PbP

Run:

- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`

with:

- `VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`
- `VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

### Stage 4. Re-derive track residual corrections with the new Z + VZ inputs

#### pp

Run:

- `MainAnalysis/20251211_ResidualCorrection/workflow/pythia-analysis.sh`

using the new pp VZ root and the new pp Z correction root.

#### pPb / PbP

Run:

- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`

using the new pPb/PbP VZ roots and the new pPb/PbP Z correction roots.

### Stage 5. Rebuild closure products and closure plots

Re-run the MC closure / correction-validation chain with the new date-tagged inputs.

Required workflow scripts:

- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`

Required plot scripts:

- `Plots/20260115_ZResidualClosure/plot-Z.sh`
- `Plots/20260115_ZResidualClosure/plot-Z-newVZFix.sh`
- `Plots/20251202_trackResidualClosure/plot-track.sh`
- `Plots/20251202_trackResidualClosure/plot-track-newVZFix.sh`
- `Plots/20260120_CentralClosure/plot-central.sh`
- `Plots/20260120_CentralClosure/plot-central-newVZFix.sh`

### Stage 6. Rebuild analysis-support MC products that depend on pp VZ

Because the pp VZ fit changed too, re-run the pp support chains:

- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
- `Plots/20260213_Central/plot-pp.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/check-dataMCComparison.sh`
- `Plots/20260213_Central/plot-dataMCComparison.sh`

If any of these were using legacy tags, move them to the new date-tagged pp chain for this validation cycle.

### Stage 7. Re-run pPb / PbP central-value outputs

Run:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`

with the new pPb/PbP VZ roots, new Z corrections, and new residual corrections, producing a date-tagged central family.

### Stage 8. Re-run energy extrapolation

Run:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `Plots/20260213_Central/plot-energyExtrapolation.sh`

This stage must use the refreshed pp correction products and produce a date-tagged pp energy-extrapolation family.

### Stage 9. Re-run central / combination / result plots

Required plot scripts:

- `Plots/20260213_Central/plot-central.sh`
- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
- `Plots/20260213_Central/plot-central-combined.sh`
- `Plots/20260213_Central/plot-zmass.sh`

Also refresh:

- `Plots/20260213_Central/plot-VZ.sh`

so the VZ application diagnostics align with the new date-tagged roots and the new no-skim policy.

If `plot-zspectrum.sh` is still part of any note-facing chain, refresh it too and document whether it changed.

## Validation requirements

The analyzer summary must include evidence for all of the following.

### 1. No skim fallback remains in the touched binaries

Provide code references proving that:

- skim `VZWeight` is no longer used as a fallback
- `UseVZWeight` is disabled by default where applicable
- MC VZ weighting now requires an explicit `VZWeightFile`

### 2. Script audit completeness

Provide a grep-style audit summary showing which scripts were updated to:

- use the `20260320` VZ roots,
- pass `--UseVZWeight false` for raw VZ derivation and all data stages,
- pass explicit `--UseVZWeight true --VZWeightFile ...` for MC stages that should receive VZ reweighting.

### 3. Produced file manifests

List the new date-tagged outputs for:

- Z corrections
- track residual corrections
- closure ROOT files
- closure PDFs
- central-value ROOT files
- central / combination / final-result PDFs
- energy extrapolation outputs

### 4. Impact confirmation by collision system

State explicitly that propagation was required for:

- `pp`
- `pPb`
- `PbP`

and tie each to the observed VZ-fit disagreement.

### 5. No stale VZ root usage remains in the rerun chain

Show that the refreshed rerun chain no longer depends on old VZ roots for the outputs produced in this task.

## Overleaf

Do **not** update Overleaf automatically in this task unless the reviewer explicitly asks for that after inspecting the refreshed outputs.

This task is to regenerate and validate the stack in-repo first.

## Required analyzer return artifact

Write the completion summary to:

- `.github/vzweight_20260320_full_propagation_execution.md`

Include:

1. comparison table old vs new VZ roots for `pp`, `pPb`, `PbP`
2. files modified
3. exact commands run
4. script-audit results
5. produced output manifest
6. validation notes on the new default VZ behavior
7. explicit statement that Overleaf was not changed
