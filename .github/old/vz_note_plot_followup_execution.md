# Analyzer execution summary: skim-VZ-off VZ weight regeneration follow-up

## Final status

Completed the reviewer-defined follow-up in `.github/vz_note_plot_followup_instructions.md`.

Scope delivered:

- regenerated the pp, pPb, and PbP raw VZ derivation inputs with `--UseVZWeight false` explicitly passed,
- produced the new date-tagged 20260320 VZ fit roots and VZ summary PDFs for all three systems,
- preserved the relabel-fixed pPb/PbP sample mapping,
- left Overleaf untouched.

## Files modified

- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ-20260320.sh`
- `Plots/20251001_pPbVZReweighting/run-reweight-20260320.sh`

## Commands run

Build / syntax checks:

```bash
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
bash -n ./closure-VZ-20260320.sh
make

cd /home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting
bash -n ./run-reweight-20260320.sh
make ExecuteVZ
```

Stage A: raw derivation inputs with skim VZ explicitly disabled:

```bash
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=8
export NSLICE_FACTOR=1
./closure-VZ-20260320.sh 1 1 1
```

Stage B: fit-root and summary-PDF production:

```bash
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting
./run-reweight-20260320.sh
```

Validation helpers:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
stat -c '%y %n' \
  plots/pp_nominal_noEvtWeight_VZOnly_nmix1_ZPT0_350-result.root \
  plots/pythiaMC_nominal_noEvtWeight_VZOnly_nmix1_ZPT0_350-result.root \
  plots/pPb_nominal_noVZWeight_nmix0_ZPT0_500-result.root \
  plots/pPbMC_nominal_noVZWeight_nmix0_ZPT0_500-result.root \
  plots/PbP_nominal_noVZWeight_nmix0_ZPT0_500-result.root \
  plots/PbPMC_nominal_noVZWeight_nmix0_ZPT0_500-result.root

cd /home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting
stat -c '%y %n' \
  summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root \
  summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root \
  summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root \
  summary/20260320_skimVZOff_ZPT0_350-Vz_pp.pdf \
  summary/20260320_skimVZOff_ZPT0_350-VzRatio_pp.pdf \
  summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_pPb.pdf \
  summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_pPb.pdf \
  summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_PbP.pdf \
  summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_PbP.pdf
```

## Raw-stage proof that `--UseVZWeight false` was used

The raw-stage execution log captured by the runner is:

- `/tmp/copilot-tool-output-1774036417948-ppte4g.txt`

Representative command lines from that log:

### pp raw branch

```text
./threader.sh output/pp_nominal_noEvtWeight_VZOnly_nmix1_ZPT0_350 1_10 pp_nominal_noEvtWeight_VZOnly_nmix1 --IsPP true --IsGenZ false --IsData true --Input mergedSample/pp-v11-Zpt0.root --MixFile mergedSample/pp-v11-Zpt0.root --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false --UseVZWeight false --yBoost 0 --nMix 1 --MinTrackPT 1 --MaxTrackPT 10 --MinZPT 0 --MaxZPT 350
```

### pPb raw branch

```text
./threader.sh output/pPb_nominal_noVZWeight_nmix0_ZPT0_500 1_10 pPb_nominal_noVZWeight_nmix0 --IsPP false --IsGenZ false --IsData true --IsPPb true --Input pPbSample/V0.2/PbPData_Reco.root --MixFile pPbSample/V0.2/PbPData_Reco.root --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false --UseVZWeight false --yBoost 0 --nMix 0 --MinTrackPT 1 --MaxTrackPT 10 --MinZPT 0 --MaxZPT 500
```

### PbP raw branch

```text
./threader.sh output/PbP_nominal_noVZWeight_nmix0_ZPT0_500 1_10 PbP_nominal_noVZWeight_nmix0 --IsPP false --IsGenZ false --IsData true --IsPPb false --Input pPbSample/V0.2/PPbData_Reco.root --MixFile pPbSample/V0.2/PPbData_Reco.root --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false --UseVZWeight false --yBoost 0 --nMix 0 --MinTrackPT 1 --MaxTrackPT 10 --MinZPT 0 --MaxZPT 500
```

The dedicated runner also makes the raw/application split explicit in the script itself:

- `closure-VZ-20260320.sh:46-48` forward `--UseVZWeight "${USE_VZ_WEIGHT}"` to MC reco jobs,
- `closure-VZ-20260320.sh:58-59` force pp data raw jobs to `--UseVZWeight false`,
- `closure-VZ-20260320.sh:85-87` forward `--UseVZWeight "${USE_VZ_WEIGHT}"` to pPb/PbP MC reco jobs,
- `closure-VZ-20260320.sh:97-98` force pPb/PbP data raw jobs to `--UseVZWeight false`,
- `closure-VZ-20260320.sh:103,116,132` run the raw branches with the explicit `false` setting,
- `closure-VZ-20260320.sh:108,124,140` reserve the weighted/application branches for explicit `true + fresh VZ root`.

## Produced fit-root manifest

Verified fresh fit roots:

- `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

Observed timestamps:

- `2026-03-20 20:57:49.647235945 +0100 summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root`
- `2026-03-20 20:57:50.434237351 +0100 summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root`
- `2026-03-20 20:57:51.210238738 +0100 summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root`

## Produced PDF manifest

Verified fresh PDFs:

- `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350-Vz_pp.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350-VzRatio_pp.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_pPb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_pPb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_PbP.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_PbP.pdf`

Observed timestamps:

- `2026-03-20 20:57:49.952236490 +0100 summary/20260320_skimVZOff_ZPT0_350-Vz_pp.pdf`
- `2026-03-20 20:57:50.133236814 +0100 summary/20260320_skimVZOff_ZPT0_350-VzRatio_pp.pdf`
- `2026-03-20 20:57:50.723237868 +0100 summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_pPb.pdf`
- `2026-03-20 20:57:50.905238193 +0100 summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_pPb.pdf`
- `2026-03-20 20:57:51.501239258 +0100 summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-Vz_PbP.pdf`
- `2026-03-20 20:57:51.682239581 +0100 summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500-VzRatio_PbP.pdf`

## How the corrected curve was produced from the fresh fit

The fit roots and the displayed corrected curves were produced by the same `ExecuteVZ` invocations in `run-reweight-20260320.sh`:

- pp: `run-reweight-20260320.sh:10-18`
- pPb: `run-reweight-20260320.sh:20-28`
- PbP: `run-reweight-20260320.sh:30-38`

Inside `reweight_VZ.cpp`:

- lines `81-84` write the fresh fit function to `summary/<output>_VzReweightFits_<collision>.root`,
- lines `87-93` immediately apply that just-fitted function to the raw MC histogram in memory,
- lines `99-119` save the reweighted-curve `-Vz_<collision>.pdf`,
- lines `123-144` save the `-VzRatio_<collision>.pdf`.

That means the “corrected” / “reweighted” curve in the 20260320 PDFs is driven by the newly produced fit root from the same run, not by an older file.

## Relabel-fixed pPb/PbP mapping confirmation

The dedicated raw/application runner preserved the reviewer-required relabel-fixed mapping:

### pPb

- reco MC input: `pPbSample/V0.2/PbPMC_Reco.root`
- data input: `pPbSample/V0.2/PbPData_Reco.root`

### PbP

- reco MC input: `pPbSample/V0.2/PPbMC_Reco.root`
- data input: `pPbSample/V0.2/PPbData_Reco.root`

These are wired directly in:

- `closure-VZ-20260320.sh:113-124`
- `closure-VZ-20260320.sh:129-140`

No regression of the relabel-fixed sample mapping was introduced.

## Raw-output refresh evidence

Refreshed raw derivation outputs were observed with new timestamps:

- `2026-03-20 20:54:17.359846095 +0100 plots/pythiaMC_nominal_noEvtWeight_VZOnly_nmix1_ZPT0_350-result.root`
- `2026-03-20 20:56:28.493087738 +0100 plots/pp_nominal_noEvtWeight_VZOnly_nmix1_ZPT0_350-result.root`
- `2026-03-20 20:56:32.843095754 +0100 plots/pPbMC_nominal_noVZWeight_nmix0_ZPT0_500-result.root`
- `2026-03-20 20:56:42.486113524 +0100 plots/pPb_nominal_noVZWeight_nmix0_ZPT0_500-result.root`
- `2026-03-20 20:56:46.881121622 +0100 plots/PbPMC_nominal_noVZWeight_nmix0_ZPT0_500-result.root`
- `2026-03-20 20:56:53.268133392 +0100 plots/PbP_nominal_noVZWeight_nmix0_ZPT0_500-result.root`

## Failures / retries

No production-stage reruns were needed.

One validation helper command used `printf '--- pdfs\n'`, which Bash interpreted as an option because of the leading hyphens. I reran the PDF timestamp check with a plain `stat` command; this did not affect any produced outputs.

## Overleaf

No Overleaf update happened in this task.

- no files were copied into `~/OverleafZHadronInPPb/`
- no TeX was edited

This was intentional and matches the reviewer instruction for this follow-up.
