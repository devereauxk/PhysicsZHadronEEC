# Analyzer follow-up: regenerate pp/pPb/PbP VZ weights with skim VZ explicitly disabled in the raw calculation stage

## Scope

This follow-up replaces the earlier plot-only plan.

The reviewer conclusion is now:

- we should **recalculate** the VZ weights and summary PDFs for `pp`, `pPb`, and `PbP`
- the raw inputs used to derive those weights must be regenerated with `--UseVZWeight false` explicitly passed in the runner scripts
- the fresh date-tagged VZ weight files produced in this task must be the source of the “corrected” / “reweighted” curve in the output PDFs
- do **not** push any of these refreshed plots to Overleaf in this task

This is a repo-only production/update task plus a reviewer-facing execution summary.

## Reviewer findings you should treat as ground truth

### 1. `UseEventWeight=false` does **not** disable VZ weighting

The main analysis code treats `UseEventWeight` and `UseVZWeight` independently.

In:

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`

`UseVZWeight` defaults to `true`, and if it remains true while no external `VZWeightFile` is provided, the code can fall back to the skim-embedded `VZWeight` branch.

Therefore:

- “no external `VZWeightFile`” is **not** enough to guarantee an unweighted raw input
- the VZ derivation runners must explicitly pass:
  - `--UseVZWeight false`

for the raw “derive the new weight” stage.

### 2. `closure-VZ.sh` currently does not force VZ off in the raw derivation stage

Inspect:

- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`

Today, the raw branches are named:

- pp:
  - `_noEvtWeight_VZOnly_nmix1`
- pPb:
  - `_noVZWeight_nmix0`
- PbP:
  - `_noVZWeight_nmix0`

but the runner currently does **not** explicitly pass `--UseVZWeight false` on those raw jobs.

That is the key bug you must fix before deriving new VZ weights.

### 3. `ExecuteVZ` already uses the freshly fitted weight as the corrected curve

Inspect:

- `Plots/20251001_pPbVZReweighting/reweight_VZ.cpp`

Current behavior:

1. load raw MC and data `hVZ`
2. fit `Data / MC`
3. write:
   - `summary/<output>_VzReweightFits_<collision>.root`
4. apply that fresh fit in memory to the raw MC histogram
5. save:
   - `summary/<output>-Vz_<collision>.pdf`
   - `summary/<output>-VzRatio_<collision>.pdf`

So the required “corrected” curve for this task should come from the **newly produced** date-tagged fit root from the same run.

### 4. pPb / PbP must keep the corrected relabel mapping

Use the relabel-fixed definitions:

- real pPb data:
  - `pPbSample/V0.2/PbPData_Reco.root`
- real pPb GEN MC:
  - `pPbSample/V0.2/PbPMC_Gen.root`
- real pPb reco MC:
  - `pPbSample/V0.2/PbPMC_Reco.root`
- real pPb EPOS:
  - `mergedEPOS/PPbMC_Gen.root`

- real PbP data:
  - `pPbSample/V0.2/PPbData_Reco.root`
- real PbP GEN MC:
  - `pPbSample/V0.2/PPbMC_Gen.root`
- real PbP reco MC:
  - `pPbSample/V0.2/PPbMC_Reco.root`
- real PbP EPOS:
  - `mergedEPOS/PbPMC_Gen.root`

Do not regress those definitions while modifying the runner scripts.

## Required tag convention

Use today’s date in the official new output tags.

### pp VZ tag

Use:

- `20260320_skimVZOff_ZPT0_350`

Expected fit root:

- `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`

Expected PDFs:

- `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350-Vz_pp.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350-VzRatio_pp.pdf`

### pPb / PbP VZ tag

Use:

- `20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500`

Expected fit roots:

- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

Expected PDFs:

- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_pPb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_pPb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_PbP.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_PbP.pdf`

## Required code / runner updates

### 1. Patch the raw derivation stage to force skim VZ off

Modify the VZ derivation runner path so the raw inputs used for fitting are generated with:

- `--UseVZWeight false`

At minimum this must cover:

- pp raw branch feeding the pp VZ calculation
- pPb raw branch feeding the pPb VZ calculation
- PbP raw branch feeding the PbP VZ calculation

The cleanest acceptable approaches are:

1. update `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh` so its helper functions accept an explicit `USE_VZ_WEIGHT` argument and forward:
   - `--UseVZWeight false`
   - for raw derivation jobs
   - and
   - `--UseVZWeight true`
   - for weighted / application jobs

2. or add a dedicated new runner for this production, for example:
   - `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ-20260320.sh`

Either is acceptable, but the final behavior must be explicit from the command line, not implicit through defaults.

### 2. Keep the weighted/application stage explicit too

When a stage is intended to apply a VZ fit file, pass both:

- `--UseVZWeight true`
- `--VZWeightFile <new-fit-root>`

This is important so the raw and corrected branches are unambiguous in logs and future audits.

### 3. Add or update the VZ weight production driver

Update the weight-production entrypoint so it can build all three systems with the new date tags:

- pp
- pPb
- PbP

You may:

- modify `Plots/20251001_pPbVZReweighting/run-reweight.sh`
- add a dedicated pp invocation there
- or add a new dedicated date-specific runner, for example:
  - `Plots/20251001_pPbVZReweighting/run-reweight-20260320.sh`

Recommended behavior:

- pp call should use the raw pp tag from the VZ-only branch
- pPb/PbP calls should use the relabel-fixed pPb/PbP raw tag
- the output tags must match the conventions above

### 4. Ensure the corrected curve is the fresh one from the new fit root

Do **not** build the displayed corrected curve from an older weight file.

The required output PDFs for this task must come from rerunning `ExecuteVZ` so that:

- the quartic fit is re-derived from the fresh raw inputs
- the date-tagged `*_VzReweightFits_*.root` is written
- the reweighted curve in `-Vz_*.pdf` is generated from that new fit

## Required execution order

### Stage A. Regenerate raw VZ derivation inputs with skim VZ explicitly off

Regenerate the raw inputs that feed the weight fits.

Requirements:

- pp raw derivation branch must run with:
  - `--UseVZWeight false`
- pPb raw derivation branch must run with:
  - `--UseVZWeight false`
- PbP raw derivation branch must run with:
  - `--UseVZWeight false`

Do not rely on the absence of `VZWeightFile`; pass the flag explicitly.

### Stage B. Build the new date-tagged VZ weights and summary PDFs

After the raw inputs are refreshed, run the VZ summary producer for:

- `pp`
- `pPb`
- `PbP`

Use the date-tagged outputs defined above.

The required products are:

- three fit roots:
  - one for `pp`
  - one for `pPb`
  - one for `PbP`
- six PDFs:
  - `Vz`
  - `VzRatio`
  - for each of `pp`, `pPb`, `PbP`

### Stage C. If you keep a corrected/application closure stage, point it to the fresh roots

If your updated runner also produces the weighted closure/application branches, make sure those stages use:

- `20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`
- `20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`
- `20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

Do not leave the scripts pointing at older `20260311`, `20260317`, `20260318`, or `20260319` VZ roots for this task.

## Build / run conventions

Use the standard environment bootstrap:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
```

Build only the touched local components, following repo convention.

Likely build points:

- `MainAnalysis/20241102_ZhadronVsZPt/`
- `Plots/20251001_pPbVZReweighting/`

Do not use ad-hoc one-off production commands for official outputs if a script can be updated instead.

## Validation requirements

Before declaring completion, include evidence for all of the following.

### 1. Raw derivation jobs truly had skim VZ disabled

Show the exact commands or script excerpts proving that the raw derivation jobs ran with:

- `--UseVZWeight false`

for:

- pp
- pPb
- PbP

### 2. New fit roots were produced

Confirm existence of:

- `summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`
- `summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`
- `summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

### 3. New PDFs were produced

Confirm existence of:

- `summary/20260320_skimVZOff_ZPT0_350-Vz_pp.pdf`
- `summary/20260320_skimVZOff_ZPT0_350-VzRatio_pp.pdf`
- `summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_pPb.pdf`
- `summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_pPb.pdf`
- `summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_PbP.pdf`
- `summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_PbP.pdf`

### 4. The corrected curve came from the fresh fit, not an older file

State explicitly which commands produced the fit roots and which commands produced the PDFs.

If you introduced a helper script, explain the flow clearly enough that the reviewer can see the new fit is what drove the corrected curve.

### 5. No Overleaf update happened

For this task:

- do **not** copy figures into `~/OverleafZHadronInPPb/`
- do **not** edit TeX in Overleaf

Explicitly confirm in the summary that Overleaf was intentionally left untouched.

## Deliverable

Write a completion summary:

- `.github/vz_note_plot_followup_execution.md`

Include:

1. exact files modified
2. exact commands run
3. raw-stage proof that `--UseVZWeight false` was used
4. produced fit-root manifest
5. produced PDF manifest
6. short confirmation that pPb/PbP relabel-fixed sample mapping was preserved
7. short confirmation that no Overleaf copy/update was performed
