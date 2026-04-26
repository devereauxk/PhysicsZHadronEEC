# Analyzer follow-up: `zmass` regression fix and pp MC-data comparison refresh

## Scope

Use the current working tree as the source of truth and treat `.github/vzweight_mc_only_reprocess_execution.md` as the completed upstream context.

The MC-only VZ reprocess was mostly successful, but there are two follow-up tasks:

1. fix the new pPb/PbP `zmass` plots so all three curves are clearly visible, and
2. regenerate the pp MC-data comparison plots for the requested kinematic grids, making sure VZ files are not applied to data.

Do not broaden the scope beyond those two tasks.

## Task 1 — Fix the pPb/PbP `zmass` plot regression first

### Problem statement

The refreshed pPb/PbP Z invariant-mass plots produced by the recent MC-only VZ reprocess do not meet the plotting requirement:

- the pPb plots currently show only **two visibly distinct curves** instead of the required three:
  - pPb Data
  - MC Gen
  - MC Reco

The plots must also be scaled so the mass peaks do **not** overlap the legend at the top of the frame.

### Files to inspect first

- `Plots/20260213_Central/plot_zmass.cpp`
- `Plots/20260213_Central/plot-zmass.sh`
- `.github/vzweight_mc_only_reprocess_execution.md`

Current affected outputs:

- `Plots/20260213_Central/plots/zmass/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf`
- `Plots/20260213_Central/plots/zmass/PbP_ZPT0_500-ZV6_trkV24_nmix10.pdf`

Current Overleaf refs:

- `OverleafZHadronInPPb/src/z_reco.tex`
  - `figures/z_reco/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf`
  - `figures/z_reco/PbP_ZPT0_500-ZV6_trkV24_nmix10.pdf`

### Required outcome

Produce pPb and PbP `zmass` figures where:

- all three intended curves are visibly distinguishable,
- the legend remains at the top but the peaks stay below it,
- the styling remains consistent with the rest of the note-quality plots,
- filenames stay source-preserving unless there is a truly unavoidable reason to change them.

### Constraints

- Treat this as a plotting fix, not a rerun of the whole correction chain.
- Do **not** change the pp `zmass` output unless a shared-code fix makes that unavoidable.
- Do **not** revert unrelated user changes.

### Expected work

1. Diagnose the exact cause of the missing/hidden third curve.
   - Check whether the issue is:
     - histogram loading,
     - histogram scaling,
     - draw order,
     - line/marker styling,
     - legend overlap / frame range,
     - or some combination.

2. Patch `plot_zmass.cpp` and, only if necessary, `plot-zmass.sh`.

3. Rebuild the plotting executable in:
   - `Plots/20260213_Central/`

4. Regenerate the pPb/PbP `zmass` plots.

5. Copy the refreshed pPb/PbP PDFs to Overleaf:
   - destination: `~/OverleafZHadronInPPb/figures/z_reco/`
   - preserve the source basenames exactly
   - update `src/z_reco.tex` only if a filename actually changes

### Validation for Task 1

Your summary must state:

- the root cause of the hidden/merged curve,
- which code change fixed it,
- how the final y-scaling / legend spacing was chosen,
- and confirm that the three visible curves are:
  - Data
  - MC Gen
  - MC Reco

Also include the exact regenerated PDF paths.

## Task 2 — Regenerate the pp MC-data comparison plots

### Goal

Regenerate the pp comparison products driven by:

- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
- `Plots/20260213_Central/plot_pp.cpp`
- `Plots/20260213_Central/plot-pp.sh`

using the current implementation as the guide for **which curves** to run.

### Requested kinematics

Produce both grids:

1. `ZPT40_350` with:
   - `trkPT1_2`
   - `trkPT2_4`
   - `trkPT4_10`

2. `ZPT20_40`, `ZPT40_60`, `ZPT60_500` with:
   - `trkPT2_500`

### Current code context to respect

`plot_pp.cpp` currently expects the pp comparison set to include:

- corrected data: `pp_trkResidual_*`
- uncorrected data: `pp_nominal_*`
- GEN MC: `pythiaMC_Gen_nominal_*`
- corrected RECO MC: `pythiaMC_trkResidual_*`
- uncorrected RECO MC: `pythiaMC_nominal_*`

Use that curve family unless you find a clear bug in the current intended comparison.

### Important policy requirement

VZ files must **not** be applied to data.

That means:

- `pp_nominal`
- `pp_ZResidual`
- `pp_trkResidual`

must not receive `--VZWeightFile`.

MC jobs may still use the pp VZ file.

### Known script issue to resolve cleanly

The current producer/plotter wrappers are not perfectly aligned:

- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
  - currently runs one active tag and one kinematic grid by default
- `Plots/20260213_Central/plot-pp.sh`
  - currently points at only the `20_40/40_60/60_500 × 2_500` grid
  - and uses a tag that may not match the producer output exactly

Your job is to make the producer and plotter **self-consistent** for this task.

### Required execution approach

1. Inspect the current wrappers and choose one consistent tag for the refreshed pp comparison set.
   - If you keep the current tag, make producer and plotter agree.
   - If you must change the tag, propagate it consistently and preserve the source basenames in outputs/Overleaf.

2. Patch `pp-plotpp.sh` and/or `plot-pp.sh` as needed so both requested kinematic grids are produced.

3. Verify explicitly that no pp data line passes `--VZWeightFile`.

4. Rebuild where needed:
   - `MainAnalysis/20241102_ZhadronVsZPt/`
   - `Plots/20260213_Central/`

5. Run the pp production and plotting scripts to generate all requested ROOT inputs and PDFs.

### Overleaf handling for Task 2

Copy the refreshed pp comparison PDFs to Overleaf when done.

Current reviewer audit found **no current `src/*.tex` references** to these `plot-pp` outputs, so do the following:

- copy the refreshed PDFs into a dedicated analysis-note location:
  - `~/OverleafZHadronInPPb/figures/analysis/pp/`
- preserve the source basenames exactly
- do **not** invent stale legacy filenames
- do **not** edit TeX unless you find an existing reference that should be updated

In your summary, list the Overleaf destination paths you used.

## Build / run conventions

Use the repository’s existing build/run style:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
```

Then build/run locally from the dated directories you touch.

Prefer the existing wrapper scripts over ad hoc command chains for official output production.

## Required validation checklist

Before declaring completion, include evidence for all of the following:

### `zmass`

- pPb `zmass` PDF refreshed
- PbP `zmass` PDF refreshed
- three visibly distinct curves confirmed in both
- peaks no longer overlap the top legend
- Overleaf `figures/z_reco/` updated

### pp comparison

- requested `ZPT40_350 × {1_2,2_4,4_10}` PDFs exist
- requested `ZPT20_40/40_60/60_500 × 2_500` PDFs exist
- corresponding input ROOT products exist
- producer and plotter tags are consistent
- no pp data job still receives `--VZWeightFile`
- Overleaf copies exist under `figures/analysis/pp/`

## Final deliverable

Write a completion summary `.md` that includes:

- files changed
- commands run
- outputs produced
- validation checks
- failures/retries
- final status

Be explicit about:

- the `zmass` root cause and fix,
- the final pp tag used,
- the exact pp kinematic grids produced,
- and the Overleaf copy destinations.
