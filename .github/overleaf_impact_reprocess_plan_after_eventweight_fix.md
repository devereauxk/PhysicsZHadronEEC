# Reviewer Plan: Overleaf Impact and Reprocessing After Mixed-Event EventWeight Fix

## Reviewed inputs
- `.github/pp_normalization_investigation_plan.md`
- `.github/pp_normalization_investigation_execution.md`
- Code diff in `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp` (mixed-event weight assignment fixed)

## Bug impact conclusion (review)
The fix changes **mixed-event normalization** when `UseEventWeight=true`.

This directly affects MC closure chains that compare GEN vs RECO where mixed-event subtraction is used, especially:
- `pPbMC_*` and `PbPMC_*` closure products in `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- Their plotted outputs in `Plots/20260120_CentralClosure/plots/{pPb,PbP}/...`

### Overleaf figures likely affected
Overleaf includes these affected pPb/PbP closure figures (same filenames/tags currently):
- `src/analysis.tex` (`figures/analysis/closure/pPb_*ZV5_trkV23_nmix10-closure-*`, `PbP_*...`)
- `src/appendix.tex` same closure family for `10_20`, `20_40`, `40_500`

### Overleaf figures likely **not** affected
- pp closure figures from the same section likely unaffected because pp closure production uses `UseEventWeight=false` in the relevant closure workflow.
- Track-only no-sub closure (`figures/tracking/*nosub-closure-{pt,eta,phi}.pdf`) should be minimally impacted by this specific bug because those come from signal track spectra, not mixed-event subtraction terms.
- Final pPb central result plots in `src/results.tex` are data-driven and not direct GEN-vs-RECO closure overlays; still do a sanity spot-check.

## Reprocessing plan for analyzer (execute exactly)

### 1) Rebuild fixed analysis binary
- In `MainAnalysis/20241102_ZhadronVsZPt/`, rebuild after fix.

### 2) Re-run affected MC closure production
- Run `closure-trk.sh` for pPb/PbP branches with the same published tags (`ZV5_trkV23_nmix10`) to preserve Overleaf filename compatibility.
- Scope: pPb + PbP; pp optional sanity only.

### 3) Hard validation gate before plotting
- For each Z bin used in note closure (`0_10`, `10_20`, `20_40`, `40_500`), verify GEN MC `DeltaPhi` integrals are near zero after subtraction for pPb and PbP.
- Require a numeric table for:
  - `pPbMC_Gen_nominal_*_ZPT*-result.root`
  - `PbPMC_Gen_nominal_*_ZPT*-result.root`
  - bins used by closure plots (`trkPT0.5_500`)

### 4) Rebuild closure PDFs used by Overleaf
- Re-run plotting in `Plots/20260120_CentralClosure/` to regenerate:
  - `*-closure-DeltaPhi-all.pdf`
  - `*-closure-DeltaPhi-bkg.pdf`
  - `*-closure-DeltaPhi-result.pdf`
  - `*-closure-DeltaEta-all.pdf`
  - `*-closure-DeltaEta-bkg.pdf`
  - `*-closure-DeltaEta-result.pdf`
for both `pPb` and `PbP`, all note-referenced Z bins.

### 5) Push updated PDFs to Overleaf
- Copy regenerated pPb/PbP closure PDFs from:
  - `Plots/20260120_CentralClosure/plots/pPb/`
  - `Plots/20260120_CentralClosure/plots/PbP/`
- into:
  - `../OverleafZHadronInPPb/figures/analysis/closure/`
- Keep filenames unchanged so no TeX edits are required.

### 6) Sanity compile and citation check
- Build Overleaf note locally (if workflow available) or at minimum verify file presence for all referenced closure figure paths in `src/analysis.tex` and `src/appendix.tex`.

## Required analyzer summary for reviewer sign-off
Analyzer must submit a summary containing:
1. Exact commands run.
2. GEN integral validation table (pre/post or final with thresholds).
3. List of updated PDF paths copied to Overleaf.
4. Any unchanged sections and explicit justification.
5. A short statement confirming whether pp Overleaf plots were changed or intentionally left unchanged.
