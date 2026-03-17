# pPb/PbP VZ-Weight Label-Mismatch Remediation Plan (Reviewer Handoff)

## Goal
Correct the pPb/PbP data-label mismatch in VZ reweighting and downstream corrections, then reproduce MC central-value closure quality for both beam orientations while regenerating note-facing plots with new tags.

## Context and findings from review
- Current note-facing tags are `ZV5` + `trkV23` (seen across Overleaf `src/analysis.tex` and `src/results.tex` figure names).
- VZ weights are produced from same-labeled data/MC pairs in `Plots/20251001_pPbVZReweighting/reweight_VZ.cpp` and `run-reweight.sh`.
- Final central production currently flips data files by orientation in `MainAnalysis/20241102_ZhadronVsZPt/central.sh`:
  - pPb chain reads `PbPData_Reco.root`
  - PbP chain reads `PPbData_Reco.root`
- This likely explains why closure can look good even though VZ weights were computed with literal labels.

## Scope
- In scope:
  - Recompute VZ weights with the **physically correct opposite-orientation data file mapping** for each beam orientation.
  - Propagate new VZ weights through Z correction and track residual correction.
  - Reproduce closure and central/result plots for pPb, PbP, and combined pPb where applicable.
  - Update Overleaf figure assets/references that depend on updated tags.
- Out of scope:
  - pp correction logic (except compatibility checks).
  - Unrelated systematic variations.

## Proposed tag/versioning strategy
- Keep old outputs intact (`ZV5_trkV23`) for reproducibility.
- Introduce a new, explicit label family (example): `ZV6_trkV24` for corrected-orientation workflow.
- Add a short README note in relevant workflow folders documenting old vs new semantics.

## Execution plan (for implementation agent)

### 1) Baseline snapshot and comparators
- Freeze current references:
  - Existing VZ fit files in `Plots/20251001_pPbVZReweighting/summary/`.
  - Existing closure plots in `Plots/20251202_trackResidualClosure/plots/` and `Plots/20260120_CentralClosure/plots/`.
  - Existing central plots in `Plots/20260213_Central/plots/`.
- Define quantitative closure metrics (same metric for old/new):
  - Ratio-to-GEN envelope over plotted bins.
  - Mean absolute deviation and max deviation for `DeltaEta` and `DeltaPhi`.

### 2) Make VZ reweighting orientation mapping explicit and correct
- Update VZ-weight production workflow (`Plots/20251001_pPbVZReweighting/`):
  - Enforce mapping table in code or wrapper script:
    - pPb correction uses `PPbMC_*` with `PbPData_*`.
    - PbP correction uses `PbPMC_*` with `PPbData_*`.
  - Add runtime printout of `(collisionType, MC input, data input)` for auditability.
- Produce new fit files with new tag stamp (e.g. date + `ZV6`).

### 3) Thread corrected VZ files into Z-correction workflow
- Update `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh` (and any companion scripts) to consume new VZ fit files.
- Regenerate Z correction outputs for both orientations.
- Preserve prior files; write new output names rather than overwrite legacy V5/V23 products.

### 4) Thread corrected VZ+Z into track residual correction
- Update `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh` to point at new Z-correction and VZ-correction products.
- Rebuild track residual correction for pPb and PbP with new tags (`trkV24` override allowed).

### 5) Regenerate closure chains end-to-end
- Run closure drivers in `MainAnalysis/20241102_ZhadronVsZPt/`:
  - `closure-VZ.sh`
  - `closure-Z.sh`
  - `closure-trk.sh`
- Confirm pPb and PbP MC central-value closure is statistically comparable to baseline.
- If degradation appears, inspect whether any later-stage label flip (data vs display) still exists and remove hidden compensation.

### 6) Regenerate central/result production with corrected semantics
- In `MainAnalysis/20241102_ZhadronVsZPt/central.sh`, make orientation mapping intentional and documented.
- Ensure no silent “flip at plotting stage” assumptions remain in:
  - `Plots/20260213_Central/plot-central*.sh`
  - `Plots/20260213_Central/plot_central_combined.cpp`
  - `Plots/20260213_Central/plot_central_overlay_PPbPbP.cpp`
- Regenerate pPb, PbP, and combined plots with new tags.

### 7) Overleaf update pass
- Replace note figure assets under `~/OverleafZHadronInPPb/figures/...` with new tagged outputs.
- Update references in:
  - `src/analysis.tex` (closure figures currently hardcoded to `ZV5_trkV23`)
  - `src/results.tex` (result figures currently hardcoded to `ZV5_trkV23`)
- Add a short note in text/changelog clarifying corrected orientation treatment.

### 8) Validation and sign-off checklist
- For each of pPb and PbP:
  - VZ fit plots (`Vz`, `VzRatio`) look physical and stable.
  - Z-correction closure unchanged within tolerance.
  - Track-residual closure unchanged within tolerance.
  - Final MC central closure (DeltaEta/DeltaPhi) matches or improves vs baseline.
- For combined pPb result:
  - Reproduces prior qualitative behavior and no unexpected asymmetry artifacts.
- Archive comparison package:
  - old vs new plots
  - summary table of closure metrics
  - exact file/version manifest.

## Risk points to watch
- Hidden compensating flips between production and plotting can mask incorrect physics mapping.
- Mixed-event orientation handling (`IsPPb`) in `CorrelationAnalysis.cpp` should remain consistent with corrected file mapping.
- Tag collisions (old/new outputs sharing names) can silently contaminate plot scripts.

## Deliverables expected from implementation agent
- Updated scripts/code with explicit orientation mapping and new tags.
- Rebuilt correction ROOT files (VZ, Z, track residual).
- Regenerated closure and central/result plots for note usage.
- One comparison summary (old vs corrected) with closure metrics and pass/fail decision.
