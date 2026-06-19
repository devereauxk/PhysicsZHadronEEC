# pPb/PbP VZ-Weight Plan for Analyzer (Post EventWeight-Bug Context)

## Reviewer preface and constraints
- This plan is for analyzer execution; reviewer scope is planning/review only.
- The mixed-event `UseEventWeight` bug in `CorrelationAnalysis.cpp` is now known and may have contributed to prior VZ-weight behavior/closure interpretations.
- **Mandatory ordering:** first reproduce current note-result plots with current pPb/PbP weights; only after that begin exploratory new-weight workflow.
- When running production, use `system-analysis.sh` via maintained shell scripts (modify existing scripts or add new scripts). Avoid one-off arbitrary terminal command chains.

## High-level objective
1) Recover/update note-facing baseline results with the current weighting configuration under fixed code.
2) Explore a new VZ-weight derivation method (correct orientation mapping), propagate through correction stack to **MC central-value closure only**.
3) Stop there for exploratory branch; do **not** push exploratory new-weight outputs to Overleaf.

---

## Phase A (required first): Reproduce note-facing results with CURRENT pPb/PbP weights

### A1. Baseline reproduction setup
- Use existing note tags currently in use (`ZV5_trkV23` family for pPb/PbP outputs in note).
- Re-run central production scripts used for note-facing pPb/PbP result plots, with fixed analysis code.
- Ensure script-driven workflow only (`central.sh`, plotting scripts in `Plots/20260213_Central/`, or dedicated wrapper scripts committed locally).

### A2. Required validation before Overleaf copy
- Confirm key output ROOT and PDF files are regenerated (fresh timestamps and non-empty).
- Check subtraction-normalization sanity for pPb/PbP central products:
  - verify `DeltaPhi_Result*` and `DeltaEta_Result*` integrals are stable and physically reasonable.
  - verify no large GEN-only mixed-subtraction offset remains in the corresponding MC cross-checks.

### A3. Overleaf update for current-weight results (this step was previously missed)
- Copy regenerated **current-weight** note-facing result PDFs to:
  - `~/OverleafZHadronInPPb/figures/result/` (and any other currently referenced target folders).
- Keep filenames consistent with existing TeX references unless a rename is explicitly needed.
- Record exact copied file list.

---

## Phase B (exploratory): New VZ-weight method with corrected mapping

### B1. New VZ-weight derivation
- In VZ reweighting workflow, enforce corrected mapping explicitly:
  - pPb correction uses `PPbMC` with `PbPData`.
  - PbP correction uses `PbPMC` with `PPbData`.
- Add explicit runtime logging of selected MC/data inputs in script output.
- Write new outputs under new tags (example pattern: `..._newVZFix_...`) without overwriting current-weight artifacts.

### B2. Propagate through correction stack
- Feed new VZ weights into:
  - Z correction workflow
  - Track residual correction workflow
- Keep all products namespaced under new exploratory tags/version suffixes.

### B3. End point for exploratory branch
- Run closure chain to **MC central-value closure** (pp/pPb/PbP where relevant for comparison).
- Required checks:
  - `DeltaPhi` and `DeltaEta` closure consistency vs GEN in all relevant Z bins.
  - compare against current-weight baseline closure metrics.
- **Stop here.** Do not produce/push exploratory central result plots to Overleaf in this phase.

---

## Impact/risk checks to include
- Re-check whether any compensating label flips remain in production/plot scripts.
- Confirm `IsPPb` logic and orientation handling remain consistent across signal and mixed-event paths.
- Ensure no cross-contamination between current-weight and exploratory outputs (strict tag separation).

## Deliverables required from analyzer
1. Command/script log (which scripts were modified/added and executed).
2. Phase A summary:
   - regenerated current-weight pPb/PbP note-facing outputs
   - list of files copied to Overleaf
   - validation notes.
3. Phase B summary:
   - new VZ-weight files
   - propagated Z/residual products
   - MC central-closure comparison table (current vs new method)
   - recommendation on whether to proceed to note-level plotting in a later step.
