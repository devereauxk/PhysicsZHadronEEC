# Reviewer template: V0.3 fullstack recovery / refresh campaign

## Required first reads
1. `/home/kdeverea/PhysicsZHadronEEC/.github/AGENT_REQUIREMENTS.md`
2. Reviewer handoff / task markdown for the specific campaign
3. Most recent accepted correction-stack summary for the promoted stack being refreshed

## Reviewer prompt skeleton
- State the promoted skim/input bundle to use.
- State the promoted tags to use for pp and pPb/PbP.
- Explicitly separate:
  1. correction-product restoration,
  2. plot-only refresh,
  3. any blocker-forced non-plot reruns,
  4. Overleaf sync,
  5. final summary/template deliverables.
- Require single-writer execution in one context.
- Name the summary markdown path and reviewer-template output path.

## Parameter block to update per campaign
- `PROMOTED_DATE_TAG=`
- `PROMOTED_PP_TAG=`
- `PROMOTED_PPB_TAG=`
- `OFFICIAL_DICTIONARY=`
- `SKIM_INPUT_VERSION=`
- `CORRECTION_SUMMARY=`
- `SUMMARY_OUTPUT=`
- `TEMPLATE_OUTPUT=`
- `OVERLEAF_ROOT=`

## Execution checklist
### Phase 1: correction-product restoration
- Restore nominal VZ products.
- Restore nominal Z-correction products.
- Restore nominal residual-correction products.
- Validate dictionary-resolved files exist and are non-empty.

### Phase 2: plot-only refresh
- HLT plots.
- VZ fit/ratio products.
- Z-correction factors and closure.
- Residual-correction factors and closure.

### Phase 3: remaining campaign plots
- Z-mass / invariant-mass plots.
- Energy-extrapolation visual outputs.
- Central-value / combined overlays.
- Central MC closure audit.
- Systematics plots.
- Temporary / diagnostic plots requested by the campaign.

### Phase 4: Overleaf sync
- Copy accepted assets into maintained figure locations.
- Make only minimal TeX filename/path edits.
- Document any note-compilation blocker separately from asset sync.

### Phase 5: closeout
- Remove rejected-run temporary config files if they were campaign-specific.
- Write final analyzer summary.
- Update reviewer template if a new durable convention was learned.

## Validation expectations
- Record exact commands run.
- Record every restored official product.
- Record every refreshed plot output actually regenerated.
- Record visual differences vs current note-facing assets.
- Record closure-quality observations and any blocker.
- Record retries, shell stops, and thread-setting changes.


## Durable gotchas learned in this recovery
- `Plots/20260213_Central/plot-zmass.sh` needs current-tag `pPb/PbP_nominal_*` data inputs; if those are missing, narrow the rerun to regenerate the note-facing current-tag nominal data roots before trusting the wrapper output.
- `Plots/20260213_Central/plot-central.sh` and `plot-energyExtrapolation.sh` can still emit PDFs when intermediate stage files are missing. Treat any missing-current-tag input warning as a hard blocker and rerun the exact missing `pp_nominal`, `pp_ZResidual`, `pp_EExtrapolation`, `pPb/PbP_nominal`, or `pPb/PbP_ZResidual` families before accepting the plots.
- `Plots/20260213_Central/plot-central-combined.sh` supports `PLOT_INCLUDE_MC=false`; this is useful to refresh the note-facing combined pp-vs-(pPb+PbP) result figures even if current-tag PA MC-gen overlay inputs are still being regenerated separately.
- `MainAnalysis/20260216_temp/plot-MuTrk.sh` positional arguments are `MODE TAG DOPP DOPPB DOPBP`, while `run.sh` uses `DOPP DOPPB DOPBP MODE TAG`; reviewers should call that out explicitly in the handoff to avoid a wasted retry.

## Stop conditions / escalation rules
- If a maintained script unexpectedly rewrites promoted correction roots, classify it as production restoration, not plot-only.
- If a plot step is blocked by missing derived inputs, prefer the narrowest maintained rerun needed to unblock it and document that boundary.
- If a previously rejected analyzer shell is still actively writing the same workspace, stop and report before proceeding.
