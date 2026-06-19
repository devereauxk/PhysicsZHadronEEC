# Reviewer audit + analyzer handoff: investigate large Z-correction contribution in NEW VZ-fix closure

## Reviewer conclusion from code audit (no production run in this review)

The analyzer’s curve-recovery fix is **valid** for the missing-lines issue:
- `closure-trk-newVZFix-mc.sh` now delegates to full-chain production, avoiding partial outputs.
- `closure-trk-newVZFix.sh` now produces all four required central-closure series (`Gen_nominal`, `nominal`, `ZResidual`, `trkResidual`) with rerun toggles.
- New plotting wrappers for Z and track closure are consistent with tag `newVZFix_ZV6_trkV24_nmix10`.

However, the observed larger Z-correction step in NEW vs OLD can plausibly come from **configuration differences**, not only physics:

1. OLD chain had pPb/PbP `VZWeightFile=""` in `closure-trk.sh` (effectively no external VZ reweight file).
2. OLD PbP `ZResidual` stage in `closure-trk.sh` used `ZCorrection_V5_PPb` (PPb file) instead of `ZCorrection_V5_PbP` for that step.
3. NEW chain uses new VZ files + new Z-correction files (`V6_newVZFix`) consistently.
4. Event-weight bug fix changed normalization behavior globally, so pre-fix and post-fix correction magnitudes are not directly comparable unless controlled.

Because multiple axes changed at once, we need a controlled rerun matrix before drawing physics conclusions.

---

## Task objective for analyzer

Determine whether the large NEW Z-correction contribution is:
- a real consequence of corrected VZ weighting + updated Z-correction derivation, or
- inflated by legacy OLD configuration artifacts (especially PbP ZResidual file mismatch / VZ handling).

---

## Execution constraints

- Use script-driven production only (modify existing scripts or add dedicated helper scripts).
- Do not use ad-hoc one-off command chains for official comparison products.
- Use `python3` for checks.
- Write temporary artifacts only under `/tmp/kdeverea`.
- Do **not** push exploratory NEW-comparison plots to Overleaf in this task.

---

## Required controlled rerun matrix (pPb and PbP)

Run a reduced matrix first (`ZPT=20_40`, `trkPT=0.5_500`) to isolate source terms quickly.  
Implement this via a dedicated script, e.g.:

`MainAnalysis/20241102_ZhadronVsZPt/closure-trk-zstep-audit.sh`

### Case A: OLD-reference reproduction
- Match OLD tag family behavior (`ZV5/trkV23` conventions).
- Keep OLD VZ handling as in old chain.
- Keep all other settings as close to OLD baseline as possible.

### Case B: OLD + PbP ZResidual file fix only
- Same as Case A except fix PbP `ZResidual` stage to use `ZCorrection_V5_PbP` (not PPb file).
- Purpose: isolate impact of legacy PbP mismatch.

### Case C: OLD + VZ-file-on (no NEW Z files)
- Start from Case A and enable external VZ weight files for pPb/PbP, while still using OLD Z-correction version.
- Purpose: isolate effect of turning on VZ reweight in closure chain.

### Case D: NEW full chain
- Use current NEW setup (`newVZFix_ZV6_trkV24_nmix10`) with consistent VZ + Z + track correction files.

If reduced matrix is inconclusive, expand to all ZPT bins (`0_10,10_20,20_40,40_500`).

---

## Quantification requirements

For each case and collision system (pPb/PbP), extract from ROOT:
- Integral or mean-level proxy for each stage curve in central closure:
  - `Gen_nominal`
  - `nominal`
  - `ZResidual`
  - `trkResidual`
- Define and report:
  - `Z-step = (ZResidual - nominal)` metric
  - `Trk-step = (trkResidual - ZResidual)` metric

Use a single, deterministic python3 script (store under `.github` or workflow utility area) to compute the same metric across all cases/tags.

---

## Plot outputs required

Produce comparison overlays (for reviewer audit only) in:

`Plots/20260120_CentralClosure/plots/zstep_audit/`

Minimum required figures:
- pPb and PbP, `ZPT20_40`, `trkPT0.5_500`, for DeltaPhi-result and DeltaEta-result
- one panel per case (A/B/C/D) or one overlay with clear legend

Optional after reduced matrix:
- replicate for all four ZPT bins.

---

## Acceptance criteria

1. Missing-curve issue remains resolved (all 4 series present).
2. Case-to-case table clearly identifies what fraction of NEW Z-step increase is explained by:
   - PbP file mismatch fix,
   - VZ reweight activation,
   - NEW V6/newVZFix correction derivation.
3. Provide a short recommendation:
   - either “NEW behavior is expected/valid” or
   - “rerun correction derivation with adjusted inputs is required”.

---

## Required analyzer return artifact

Write execution summary to:

`.github/pPb_PbP_newVZFix_ZContribution_audit_execution.md`

Include:
- exact scripts added/modified,
- ordered command log,
- produced file manifest,
- quantitative table for Z-step/Trk-step per case,
- final recommendation on whether any correction production must be rerun.
