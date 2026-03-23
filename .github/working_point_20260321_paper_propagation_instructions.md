# Analyzer Instructions: propagate the 20260321 working-point corrections to the paper plots and Overleaf

## Reviewer role and scope

Use the current working tree in `/home/kdeverea/PhysicsZHadronEEC` as the source of truth.

This is an analyzer execution task. Execute it sequentially unless a step explicitly allows parallel work, and return a completion summary in:

- `.github/working_point_20260321_paper_propagation_execution.md`

If you hit any mismatch between the expected `20260321` working-point files and the actual hand-made plot products already present in the tree, stop and document the discrepancy clearly for reviewer follow-up rather than guessing.

---

## Ground truth working-point correction files

Treat the following roots from `.github/for-reviewer.md` as the new promoted working point for this task:

### VZ

- `Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pp.root`
- `Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pPb.root`
- `Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_PbP.root`

### Z correction

- `MainAnalysis/20260115_ZCorrection/workflow/my_ZWeights/20260321_ZCorrection_V6_pp_zPt0-500.root`
- `MainAnalysis/20260115_ZCorrection/workflow/my_ZWeights/20260321_ZCorrection_V6_PPb_zPt0-500.root`
- `MainAnalysis/20260115_ZCorrection/workflow/my_ZWeights/20260321_ZCorrection_V6_PbP_zPt0-500.root`

### Track residual correction

- `MainAnalysis/20241102_ZhadronVsZPt/my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_pp_zPt`
- `MainAnalysis/20241102_ZhadronVsZPt/my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_PPb_zPt`
- `MainAnalysis/20241102_ZhadronVsZPt/my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_PbP_zPt`

Do not revert to the older `20260320_*skimVZOff*` roots for official reruns in this task.

---

## Mandatory Overleaf standards

You must enforce the following standards exactly:

1. Build an explicit figure manifest from the current note references in `~/OverleafZHadronInPPb/src/*.tex`.
2. Copy only figures that are referenced in text.
3. Preserve the source basename exactly when copying into Overleaf.
4. If a note reference currently points to an older filename, update the TeX reference to the copied source basename instead of renaming the new PDF to the old name.
5. After the copy/update pass, audit the changed figure set and confirm:
   - every copied figure is referenced by `src/*.tex`,
   - every changed `\includegraphics{...}` target exists,
   - and the Overleaf filename matches the analysis-source basename exactly.

The note sections already known to be relevant are:

- `~/OverleafZHadronInPPb/src/software_mc_simulation.tex`
- `~/OverleafZHadronInPPb/src/appendix.tex`
- `~/OverleafZHadronInPPb/src/analysis.tex`
- `~/OverleafZHadronInPPb/src/results.tex`

Still derive the final manifest from the live TeX references rather than assuming this list is complete.

---

## Stage 1. Reuse the already hand-made upstream plots first

Do **not** regenerate the upstream correction and closure figures if the hand-made `ZV6` / `trkV24` versions already exist and match the intended `20260321` working point. Reuse those products and propagate them to Overleaf first.

### A. VZ distributions

Use the note-facing VZ distribution plots already present in:

- `Plots/20251001_pPbVZReweighting/summary/`

The expected working-point family is:

- `20260321_ZPT0_500-VzRatio_pp.pdf`
- `20260321_ZPT0_500-VzRatio_pPb.pdf`
- `20260321_ZPT0_500-VzRatio_PbP.pdf`
- `20260321_ZPT0_500-Vz_pp.pdf`
- `20260321_ZPT0_500-Vz_pPb.pdf`
- `20260321_ZPT0_500-Vz_PbP.pdf`

### B. Z correction and Z closure

Use the already-made Z-correction products from:

- `MainAnalysis/20260115_ZCorrection/workflow/plots/`
- `Plots/20260115_ZResidualClosure/plots/`

The current note-facing closure family already visible in the tree is:

- `Plots/20260115_ZResidualClosure/plots/pp/ZPT0_500_ZV6_trkV24_nmix0-closure-{pt,eta,phi}.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix0-closure-{pt,eta,phi}.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix0-closure-{pt,eta,phi}.pdf`

### C. Track correction and track closure

Use the already-made track-correction products from:

- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/`
- `Plots/20251202_trackResidualClosure/plots/`

The current note-facing track-closure family already visible in the tree includes:

- `Plots/20251202_trackResidualClosure/plots/pp/*ZV6_trkV24_nmix10-nosub-closure-{pt,eta,phi}.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/*ZV6_trkV24_nmix10-nosub-closure-{pt,eta,phi}.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/*ZV6_trkV24_nmix10-nosub-closure-{pt,eta,phi}.pdf`

If any of these hand-made upstream plot families are missing, use a mismatched tag, or appear inconsistent with the `20260321` roots above, stop and document the exact mismatch before doing anything broad.

---

## Stage 2. Update Overleaf with the upstream hand-made figures

Using the manifest derived from `src/*.tex`:

1. Copy the VZ distribution figures from `Plots/20251001_pPbVZReweighting/summary/` to the appropriate Overleaf `figures/event/` destinations using the source basenames unchanged.
2. Copy the Z-correction and Z-closure figures from `MainAnalysis/20260115_ZCorrection/workflow/plots/` and `Plots/20260115_ZResidualClosure/plots/` to the note destinations using the source basenames unchanged.
3. Copy the track-correction and track-closure figures from `MainAnalysis/20251211_ResidualCorrection/workflow/plots/` and `Plots/20251202_trackResidualClosure/plots/` using the source basenames unchanged.
4. Update the TeX references to the exact copied basenames wherever the note is still pointing at the older `20260320_*` or other stale families.

Do not leave Overleaf with mixed stale/new naming for the same figure family.

---

## Stage 3. Rerun the downstream plot chain using the 20260321 working-point roots

After the upstream note plots are handled, rerun the downstream products that depend on the promoted working point.

### A. MC central closure and result plots for pp, pPb, and PbP

Use the maintained main-analysis entrypoints in:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `Plots/20260120_CentralClosure/plot-central.sh`
- `Plots/20260213_Central/plot-central.sh`

Required outcome:

- refreshed MC central-closure plots for all three collision systems,
- refreshed note-facing central/result plots that reflect the `20260321` working point.

### B. pp energy extrapolation

Rerun the pp energy-extrapolation chain using the `20260321` working point:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `Plots/20260213_Central/plot-energyExtrapolation.sh`

If the current tree’s note-facing pp energy panels are generated by an additional maintained pp plotter in `Plots/20260213_Central/`, run that too and document why.

### C. pPb / PbP separate-comparison and combination plots

Regenerate the pPb-vs-PbP comparison and the combined-result products using:

- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
- `Plots/20260213_Central/plot-central-combined.sh`

These are the outputs that feed the note’s combining section and the final combined-result section.

### D. Tagging and naming

Follow the current official working-point naming in the produced outputs. Do not rename fresh `20260321`-based products back to `20260320` names just to avoid TeX edits.

If the downstream plotters still emit old tag families by default, update the references in a controlled way and document exactly which files changed.

---

## Stage 4. Update Overleaf with the downstream figures

After validating the rerun outputs:

1. Rebuild the Overleaf manifest from the current `src/*.tex` references for the affected sections.
2. Copy the changed downstream figures into Overleaf with their exact source basenames.
3. Update TeX references only where the copied basename differs from the old note reference.
4. Remove stale superseded Overleaf figure files only if they are no longer referenced anywhere in `src/*.tex`.

The note currently references relevant downstream families in:

- `src/analysis.tex` for central closure, pp energy, and pPb/PbP combining plots
- `src/results.tex` for the final combined results

Again, treat the live TeX as the real manifest source.

---

## Validation and summary requirements

Your completion summary in `.github/working_point_20260321_paper_propagation_execution.md` must include:

1. The exact commands run, grouped by stage.
2. The exact figure manifest copied to Overleaf, including source path and destination path.
3. The list of TeX references updated, with old path and new path.
4. The list of rerun outputs produced for:
   - MC central closure
   - central/result plots
   - pp energy extrapolation
   - pPb/PbP overlay and combined plots
5. Validation checks:
   - which changed Overleaf figures are referenced in text,
   - confirmation that every copied filename matches the source basename exactly,
   - confirmation that no copied figure in this task is unreferenced,
   - `git status --short` for both the analysis repository and `~/OverleafZHadronInPPb`.
6. Any discrepancies, failures, or manual judgment calls.

Final status should clearly state whether the note is fully updated to the `20260321` working point for the figure families in scope.
