# Analyzer Instructions: Recover Missing Closure Curves for `newVZFix_ZV6_trkV24_nmix10`

## Context
From `.github/pPb_PbP_VZWeight_Fix_followup_execution.md`, central-closure plots were produced for exploratory new VZ weights, but the plots appear to be missing intermediate correction curves (Z-only and Z+track sequence not fully shown).

Most likely cause: an MC-only production path was used (`closure-trk-newVZFix-mc.sh`) that does not generate the full set of inputs expected by closure plotting code.

Your task is to troubleshoot and regenerate what is needed so closure plots include all intended curves.

---

## Task 1: Diagnose missing-curve root cause (file-level)

For both `pPb` and `PbP`, and each `ZPT` bin in `{0_10,10_20,20_40,40_500}`, verify existence of **all** required inputs for tag:

- `newVZFix_ZV6_trkV24_nmix10`

Required prefixes:
- `pPbMC_Gen_nominal`
- `pPbMC_nominal`
- `pPbMC_ZResidual`
- `pPbMC_trkResidual`
- `PbPMC_Gen_nominal`
- `PbPMC_nominal`
- `PbPMC_ZResidual`
- `PbPMC_trkResidual`

Required suffixes per file:
- `-nosub.root`
- `-result.root`

If any are missing, that explains missing curves.

---

## Task 2: Regenerate full MC central-closure chain for new VZ fix

Do **not** use the MC-only helper for final closure production.

Use/repair script-driven flow so the full correction stack is produced:
- GEN
- RECO nominal
- Z-corrected
- Z+track-corrected

Primary script to use/repair:
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`

Then regenerate central-closure plots:
- `Plots/20260120_CentralClosure/plot-central-newVZFix.sh`

Outputs should be in:
- `Plots/20260120_CentralClosure/plots/pPb/`
- `Plots/20260120_CentralClosure/plots/PbP/`

with tag:
- `newVZFix_ZV6_trkV24_nmix10`

---

## Task 3: Ensure Z-correction closure plots exist for NEW weights

Generate Z-correction closure products specifically for new weights (not legacy `Zclosure` tag only).

If needed, create/modify scripts (preferred over ad hoc command chains):
- analysis production side under `MainAnalysis/20241102_ZhadronVsZPt/` (newVZFix-aware Z-closure script)
- plotting side under `Plots/20260115_ZResidualClosure/` (newVZFix-aware plot script)

Expected outcome:
- Z correction closure PDFs for pPb and PbP exist for newVZFix naming and are non-empty.

---

## Task 4: Ensure track-correction closure plots exist for NEW weights

Generate track-residual closure plots for newVZFix weights:
- use/extend `Plots/20251202_trackResidualClosure/` scripts for `newVZFix_ZV6_trkV24_nmix10`.

Expected outcome:
- Track closure PDFs (pt/eta/phi and/or defined closure suite) for pPb and PbP with newVZFix naming exist and are non-empty.

---

## Task 5: Explicit curve-visibility validation

For central closure plots in `Plots/20260120_CentralClosure/plots/{pPb,PbP}/...newVZFix...`:
- confirm all four intended curves are present:
  - GEN
  - RECO nominal
  - Z-corrected
  - Z+track-corrected

Provide at least one deterministic check per representative plot:
- either object-count check from source ROOT/hist vectors,
- or scripted legend/series count assertion,
- plus visual spot check notes.

---

## Execution rules
- Use script-driven production (`system-analysis.sh` via scripts).
- If scripts are missing support, add/modify scripts in repo; avoid fragile one-off production command chains.
- `python3` only.
- Use `/tmp/kdeverea` for temporary artifacts.

---

## Required return summary (`.md`)

Include:
1. Root-cause diagnosis for missing curves.
2. Exact scripts changed/added.
3. Ordered command log.
4. File manifests for:
   - central closure (newVZFix),
   - Z closure (newVZFix),
   - track closure (newVZFix).
5. Curve-visibility validation evidence.
6. Any bins/plots still problematic and recommended next fix.
