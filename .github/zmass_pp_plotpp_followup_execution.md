# Analyzer execution summary: `zmass` follow-up and pp MC-data comparison refresh

## Final status

Completed the two reviewer-scoped follow-up tasks from `.github/zmass_pp_plotpp_followup_instructions.md`:

1. fixed the pPb/PbP `zmass` plotting regression, and
2. regenerated the requested pp MC-data comparison products and PDFs with a single consistent tag and no VZ file applied to pp data.

## Files changed

- `Plots/20260213_Central/plot_zmass.cpp`
- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
- `Plots/20260213_Central/plot-pp.sh`

## Task 1: pPb/PbP `zmass` fix

### Root cause

The hidden / merged third curve was caused by a file-selection bug in `Plots/20260213_Central/plot_zmass.cpp`.

- The code requested the heavy-ion data file at:
  - `.../{pPb,PbP}_nominal_ZV6_trkV24_nmix10_ZPT0_500-nosub.root`
- After the earlier MC-only VZ reprocess, the refreshed heavy-ion data outputs existed at:
  - `.../{pPb,PbP}_nominal_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`

Because the requested `ZPT0_500` data file did not exist, the code silently skipped it, kept the fixed three-entry label vector, and then plotted the first surviving MC histogram under the `DATA` label. That left only two visibly distinct curves in the final figure and mislabeled the first one.

### Code fix

Patched `plot_zmass.cpp` to:

- resolve the data input from existing candidates instead of hardcoding one path,
- keep the MC Gen / MC Reco pair synchronized during input resolution,
- fail loudly if a required file or histogram is missing instead of silently relabeling the remaining histograms,
- detach histograms from the input file before closing it, and
- compute the displayed y-range from the maximum across all loaded curves.

### Y-scaling / legend spacing choice

The final y-range is set to:

- `0` to `1.60 * max_peak`

where `max_peak` is the maximum bin content across the loaded Data, MC Gen, and MC Reco histograms after the MC normalization-to-data step.

This was chosen so the legend can remain at the top of the frame while keeping the tallest peak comfortably below it in both pPb and PbP.

### Commands run

```bash
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central
make
./plot-zmass.sh
```

Validation / spot-check helpers:

```bash
pdftoppm -png Plots/20260213_Central/plots/zmass/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf /tmp/kdeverea/zmass_followup_fixed/pPb
pdftoppm -png Plots/20260213_Central/plots/zmass/PbP_ZPT0_500-ZV6_trkV24_nmix10.pdf /tmp/kdeverea/zmass_followup_fixed/PbP
```

### Regenerated `zmass` PDFs

- `Plots/20260213_Central/plots/zmass/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf`
- `Plots/20260213_Central/plots/zmass/PbP_ZPT0_500-ZV6_trkV24_nmix10.pdf`

### Validation

Confirmed from rendered PNG spot-checks that both refreshed heavy-ion plots now show three visibly distinct curves:

- Data
- MC Gen
- MC Reco

and that the peaks remain below the top legend in both:

- `/tmp/kdeverea/zmass_followup_fixed/pPb-1.png`
- `/tmp/kdeverea/zmass_followup_fixed/PbP-1.png`

### Overleaf update

Copied the refreshed PDFs, preserving source basenames exactly, to:

- `~/OverleafZHadronInPPb/figures/z_reco/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf`
- `~/OverleafZHadronInPPb/figures/z_reco/PbP_ZPT0_500-ZV6_trkV24_nmix10.pdf`

No additional `src/z_reco.tex` edit was needed for this follow-up, because the current refs already matched those basenames.

## Task 2: pp MC-data comparison refresh

### Final tag used

- `ZV6_trkV24_nmix10`

This tag is now used consistently by both:

- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
- `Plots/20260213_Central/plot-pp.sh`

### Requested kinematic grids produced

Produced the requested comparison set for:

- `ZPT40_350` with:
  - `trkPT1_2`
  - `trkPT2_4`
  - `trkPT4_10`
- `ZPT20_40`, `ZPT40_60`, `ZPT60_500` with:
  - `trkPT2_500`

### Producer / plotter alignment change

Patched:

- `pp-plotpp.sh` to drive both requested grid groups with the same tag and without `--VZWeightFile` on pp data jobs,
- `plot-pp.sh` to render exactly the six requested PDF combinations from the same tag, and
- `pp-plotpp.sh` again to drop the unused `pp_ZResidual` family so the producer matches the actual five-curve family consumed by `plot_pp.cpp`.

### VZ policy validation

Explicitly verified that the pp data families:

- `pp_nominal`
- `pp_trkResidual`

do not pass `--VZWeightFile`.

MC jobs still keep the pp VZ file as intended.

### Commands run

Initial build / production attempt:

```bash
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
make
./pp-plotpp.sh
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central
make ExecutePPPlot
./plot-pp.sh
```

Because the original wrapper still spent time on the unused `pp_ZResidual` family, I stopped that long run after the required MC families and `pp_nominal` outputs were in place, then finished only the missing required family (`pp_trkResidual` on the second grid) and reran the plotter:

```bash
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
cat > config_plotpp_remaining.sh <<'EOF'
ZPT_RANGES=("20_40" "40_60" "60_500")
PT_RANGES=("2_500")
EOF
export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=8
export NSLICE_FACTOR=1
export CONFIG_FILE=$PWD/config_plotpp_remaining.sh
./system-analysis.sh "pp_trkResidual_ZV6_trkV24_nmix10" \
  --IsPP true --IsGenZ false --IsData true \
  --Input mergedSample/pp-v11-Zpt0.root \
  --MixFile mergedSample/pp-v11-Zpt0.root \
  --UseEventWeight true --UseZWeight true \
  --UseTrackWeight true --UseResidualWeight true \
  --ZWeightFile my_ZWeights/20260317_ZCorrection_V6_pp_zPt0-500.root \
  --ResidualWeightFile my_residualWeights/20260317_TrackResidualCorrection_V24_ZWeight_V6_pp_zPt \
  --yBoost 0 --nMix 10
rm -f config_plotpp_remaining.sh
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central
make ExecutePPPlot
./plot-pp.sh
```

Validation / spot-check helpers:

```bash
pdftoppm -png Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_350_trkPT1_2-DeltaEta-result.pdf /tmp/kdeverea/pp_plotpp_spotcheck/pp_40350_12_eta
pdftoppm -png Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT20_40_trkPT2_500-DeltaEta-result.pdf /tmp/kdeverea/pp_plotpp_spotcheck/pp_2040_2500_eta
```

### Required ROOT products confirmed

All required comparison ROOT inputs exist for the final tag:

- `pp_trkResidual_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `pp_trkResidual_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `pp_trkResidual_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `pp_trkResidual_ZV6_trkV24_nmix10_ZPT60_500-result.root`
- `pp_nominal_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `pp_nominal_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `pp_nominal_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `pp_nominal_ZV6_trkV24_nmix10_ZPT60_500-result.root`
- `pythiaMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `pythiaMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `pythiaMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `pythiaMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT60_500-result.root`
- `pythiaMC_trkResidual_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `pythiaMC_trkResidual_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `pythiaMC_trkResidual_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `pythiaMC_trkResidual_ZV6_trkV24_nmix10_ZPT60_500-result.root`
- `pythiaMC_nominal_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `pythiaMC_nominal_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `pythiaMC_nominal_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `pythiaMC_nominal_ZV6_trkV24_nmix10_ZPT60_500-result.root`

### Requested pp PDFs confirmed

All 12 requested pp comparison PDFs exist:

- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_350_trkPT1_2-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_350_trkPT1_2-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_350_trkPT2_4-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_350_trkPT2_4-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_350_trkPT4_10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_350_trkPT4_10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT20_40_trkPT2_500-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT20_40_trkPT2_500-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_60_trkPT2_500-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT40_60_trkPT2_500-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT60_500_trkPT2_500-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_nmix10_ZPT60_500_trkPT2_500-DeltaPhi-result.pdf`

### Spot-check validation

Rendered and visually spot-checked one PDF from each requested grid:

- `/tmp/kdeverea/pp_plotpp_spotcheck/pp_40350_12_eta-1.png`
- `/tmp/kdeverea/pp_plotpp_spotcheck/pp_2040_2500_eta-1.png`

These show the expected five-curve pp comparison format:

- corrected data
- uncorrected data
- GEN MC
- corrected RECO MC
- uncorrected RECO MC

### Overleaf handling

Copied the 12 refreshed pp PDFs, preserving source basenames exactly, into:

- `~/OverleafZHadronInPPb/figures/analysis/pp/`

The reviewer audit was confirmed: no matching current `src/*.tex` references were found for these `plot-pp` outputs, so no TeX files were edited.

## Failures / retries / notable execution choices

- `python3` + `ROOT` was not available for the original `zmass` histogram probe (`ModuleNotFoundError: No module named 'ROOT'`), so I switched to ROOT CLI and rendered PDF-to-PNG spot-checks instead.
- The original `plot_zmass.cpp` behavior silently skipped a missing data file; the fix now aborts loudly on missing required inputs.
- The first full pp wrapper run was allowed to progress through the required MC families and `pp_nominal`, then I stopped it once it entered the unused `pp_ZResidual` stage and finished only the remaining required `pp_trkResidual` second-grid outputs with a targeted rerun. I then patched `pp-plotpp.sh` so future official reruns stay aligned with the five-curve family actually used by `plot_pp.cpp`.
