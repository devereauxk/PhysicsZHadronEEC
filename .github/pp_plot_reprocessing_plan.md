# Analyzer plan: refresh pp `plot-pp` outputs and `dataMCComparison` plots with the promoted `ZV6` / `trkV24` families

## Reviewer scope and objective

This handoff is limited to the plotting task requested by the reviewer:

1. Reprocess the pp comparison plots produced by `Plots/20260213_Central/plot_pp.cpp` / `plot-pp.sh` for:
   - `ZPT40_350` with `trkPT = 1_2, 2_4, 4_10`
   - `ZPT20_40`, `ZPT40_60`, `ZPT60_500` with `trkPT = 2_500`
2. Remake the `dataMCComparison` `ZPT5_500` plots using the most recent `ZV6` / `trkV24` correction family, replacing the stale `vz20260320`-tagged outputs.

Do **not** broaden this into a full correction propagation. Only regenerate the upstream `MainAnalysis/20241102_ZhadronVsZPt/plots/*.root` inputs if they are strictly required for these plotting entrypoints.

Do **not** update Overleaf in this task unless you discover active note references to these exact outputs and the reviewer explicitly approves a follow-up. The reviewer audit found no current `src/*.tex` references to the `plot-pp` or `dataMCComparison` output families.

## Ground truth from the reviewer audit

### 1. The actual pp plotting entrypoint is `plot_pp.cpp`

The user referred to `Plots/20260213_Central/plot-pp.cpp`, but the real source file is:

- `Plots/20260213_Central/plot_pp.cpp`

The maintained wrapper is:

- `Plots/20260213_Central/plot-pp.sh`

### 2. `plot-pp.sh` already loops over the exact requested kinematic bins, but it is pinned to an old tag

Current wrapper state:

- `Plots/20260213_Central/plot-pp.sh`
  - hardcodes `PP_TAG="ZV6_trkV24_vz20260320_nmix10"`
  - already runs:
    - `ZPT40_350 x trkPT(1_2, 2_4, 4_10)`
    - `ZPT(20_40, 40_60, 60_500) x trkPT2_500`

`plot_pp.cpp` reads:

- `pp_trkResidual_<tag>_ZPT<range>-result.root`
- `pp_nominal_<tag>_ZPT<range>-result.root`
- `pythiaMC_Gen_nominal_<tag>_ZPT<range>-result.root`
- `pythiaMC_trkResidual_<tag>_ZPT<range>-result.root`
- `pythiaMC_nominal_<tag>_ZPT<range>-result.root`

from:

- `MainAnalysis/20241102_ZhadronVsZPt/plots/`

Reviewer check: the promoted pp inputs already exist for `ZV6_trkV24_nmix10`, and the desired promoted outputs already exist once under:

- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_...`

alongside stale `ZV6_trkV24_vz20260320_nmix10_...` copies.

### 3. `dataMCComparison` is still tied to the stale `vz20260320` family

Relevant files:

- `Plots/20260213_Central/plot_dataMCComparison.cpp`
- `Plots/20260213_Central/plot-dataMCComparison.sh`

Current wrapper state:

- `plot-dataMCComparison.sh` hardcodes:
  - `PP_TAG="ZV6_trkV24_vz20260320_nmix10"`
  - `PPB_TAG="ZV6_trkV24_vz20260320_nmix0"`
- only the `pPb` command is active
- the `PbP` command is currently commented out

Current promoted-tag output state:

- reviewer found only stale `vz20260320` `plots/dataMCComparison/pPb_...` PDFs
- reviewer found **no** promoted-tag `plots/dataMCComparison/*ZV6_trkV24_nmix0*`
- reviewer found **no** promoted-tag `plots/dataMCComparison/*ZV6_trkV24_nmix10*`

### 4. `plot_dataMCComparison.cpp` expects a wider nosub input set than currently appears to be present for the promoted tag

For a heavy-ion collision type, `plot_dataMCComparison.cpp` expects these seven `-nosub.root` inputs:

1. `<system>MC_Gen_nominal_<tag>_ZPT5_500-nosub.root`
2. `<system>NoEPOSMC_Gen_nominal_<tag>_ZPT5_500-nosub.root`
3. `<system>MC_nominal_<tag>_ZPT5_500-nosub.root`
4. `<system>MC_trkResidual_<tag>_ZPT5_500-nosub.root`
5. `pythia<system>EPOSMC_Gen_nominal_<tag>_ZPT5_500-nosub.root`
6. `<system>_nominal_<tag>_ZPT5_500-nosub.root`
7. `<system>_trkResidual_<tag>_ZPT5_500-nosub.root`

Reviewer check for the promoted heavy-ion family found:

- present for `ZV6_trkV24_nmix10`:
  - `pPbMC_Gen_nominal_...`
  - `pPb_nominal_...`
  - `pPb_trkResidual_...`
  - `PbPMC_Gen_nominal_...`
  - `PbP_nominal_...`
  - `PbP_trkResidual_...`
- not found during reviewer audit for `ZV6_trkV24_nmix10`:
  - `pPbMC_nominal_...`
  - `pPbMC_trkResidual_...`
  - `pPbNoEPOSMC_Gen_nominal_...`
  - `pythiapPbEPOSMC_Gen_nominal_...`
  - and the analogous PbP-side variants

So this task is **not yet guaranteed** to be a pure plotting rerun for `dataMCComparison`; you must audit the exact filenames in `MainAnalysis/20241102_ZhadronVsZPt/plots/` before deciding whether a minimal upstream refresh is required.

## Required analyzer work

### Step 1. Confirm the promoted tag families before changing any wrapper

Use the reviewer findings above as the starting point and confirm the maintained convention:

- for `plot-pp`, use the promoted pp tag family:
  - `ZV6_trkV24_nmix10`
- for `dataMCComparison`, do **not** blindly preserve the stale `vz20260320` suffix

For `dataMCComparison`, first determine which promoted heavy-ion tag family is the correct maintained target:

- if the study is intentionally still a mixing-off study, the target may remain `ZV6_trkV24_nmix0`
- if the currently maintained promoted heavy-ion inputs only exist and are intended under `ZV6_trkV24_nmix10`, use that instead

Do not guess. Base the decision on the actual maintained script conventions and the available `MainAnalysis` input roots, then record that decision clearly in your execution summary.

### Step 2. Update only the maintained wrappers that belong to this task

Make only the minimal script edits needed for this refresh:

1. `Plots/20260213_Central/plot-pp.sh`
   - replace the stale `PP_TAG="ZV6_trkV24_vz20260320_nmix10"`
   - point it at the promoted pp family
2. `Plots/20260213_Central/plot-dataMCComparison.sh`
   - remove the stale `vz20260320` suffix from the tag you selected in Step 1
   - keep the scope limited to the collision systems you actually regenerate

If `PbP` should be regenerated together with `pPb`, un-comment and run the `PbP` line. If you keep the scope to `pPb` only, explain why that is still consistent with the maintained usage of this study.

Do **not** edit unrelated central/energy/combination wrappers in this task.

### Step 3. Audit whether `dataMCComparison` needs upstream input production

Before running `ExecuteDataMCComparison`, verify that every required `-nosub.root` input listed above exists for the chosen promoted tag and collision system(s).

If every required input exists:

- treat `dataMCComparison` as a pure plotting rerun

If some required inputs are missing:

- determine whether the absence is due to stale file naming, stale wrapper assumptions, or genuinely missing upstream production
- then regenerate **only** the missing upstream `MainAnalysis/20241102_ZhadronVsZPt/plots/*.root` inputs needed by `plot_dataMCComparison.cpp`
- do not launch a broader full-stack propagation

If the current C++ input naming is itself inconsistent with the maintained files on disk, document that explicitly and stop for reviewer guidance instead of inventing a new naming scheme.

### Step 4. Regenerate the pp `plot-pp` outputs

Run the maintained pp plotting wrapper after updating its tag, and produce:

- `ZPT40_350 x trkPT1_2`
- `ZPT40_350 x trkPT2_4`
- `ZPT40_350 x trkPT4_10`
- `ZPT20_40 x trkPT2_500`
- `ZPT40_60 x trkPT2_500`
- `ZPT60_500 x trkPT2_500`

Expected output family:

- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT...-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT...-DeltaPhi-result.pdf`

Retain the promoted basename exactly; do not create another `vz20260320` alias.

### Step 5. Regenerate the `dataMCComparison` `ZPT5_500` outputs

After resolving Step 3, run the maintained `dataMCComparison` plotting workflow for:

- `ZPT5_500`
- `trkPT0.5_500`
- the promoted heavy-ion tag selected in Step 1
- the collision system(s) justified by the maintained wrapper and available inputs

Expected output family:

- `Plots/20260213_Central/plots/dataMCComparison/<system>_<tag>_ZPT5_500-Zmass.pdf`
- `...-Zpt.pdf`
- `...-Zeta.pdf`
- `...-pt.pdf`
- `...-eta.pdf`

Again, do not keep or recreate the stale `vz20260320` basename.

### Step 6. Validate the refresh

Validation must include:

1. A listing of the newly produced PDFs for all requested `plot-pp` bins
2. A listing of the newly produced `dataMCComparison` PDFs
3. Confirmation that the wrappers now point at the promoted tag family, not `vz20260320`
4. For `dataMCComparison`, an explicit note stating whether:
   - it was a pure plotting rerun, or
   - minimal upstream `MainAnalysis` regeneration was required

If you had to regenerate upstream inputs, list exactly which roots were missing and exactly which commands produced them.

## Stop conditions

Stop and report back to the reviewer if any of the following happens:

1. `plot_dataMCComparison.cpp` requires input filenames that no longer correspond to the maintained `MainAnalysis` output naming
2. the promoted heavy-ion tag family cannot be determined cleanly from maintained wrappers and existing roots
3. the task starts to require unrelated central/energy/final-result reruns

## Required execution summary

Write an execution summary markdown in `.github/` that includes:

- exact files edited
- exact commands run
- exact tags used
- whether `dataMCComparison` stayed `pPb`-only or was run for both `pPb` and `PbP`
- whether any upstream `MainAnalysis` regeneration was required
- the final list of output PDFs produced
- any unresolved naming or scope questions for reviewer follow-up
