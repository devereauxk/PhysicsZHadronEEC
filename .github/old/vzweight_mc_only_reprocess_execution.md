# MC-only VZ reprocess execution summary

## Scope completed

Implemented the reviewer-requested MC-only `VZWeightFile` policy fix, refreshed the affected pPb/PbP central data products, refreshed the pp energy-extrapolation chain and downstream central/overlay/combined/zmass figures, and updated the note checkout using source basenames directly.

## Files patched

Physics analysis repo:

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `MainAnalysis/20260222_EnergyExtrapolation/CorrelationAnalysis.cpp`
- `MainAnalysis/20260222_EnergyExtrapolation/include/parameter.h`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `MainAnalysis/20260115_ZCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20260115_ZCorrection/include/parameter.h`
- `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20251211_ResidualCorrection/include/parameter.h`
- `Plots/20260213_Central/plot-zmass.sh`
- `Plots/20260213_Central/plot-energyExtrapolation.sh`

Note checkout:

- `OverleafZHadronInPPb/src/z_reco.tex`

## Code-path validation

- File-based VZ correction is now guarded by both a non-empty `VZWeightFile` and `!par.isData` in all four target binaries.
- `central.sh` no longer passes `--VZWeightFile` to the pPb/PbP data jobs.
- `closure-VZ.sh` already had the data leg unweighted in this worktree; no patch was needed there.
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh` no longer passes `--VZWeightFile` to the pp data jobs.

Validation snippets collected during execution:

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`
  - `if (par.VZWeightFile != "" && !par.isData) {`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
  - no `VZWeightFile` remains on the pp data lines
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
  - no `IsData true ... VZWeightFile` matches remain

## Build / validation commands run

- `source SetupAnalysis.sh && cd CommonCode && make -j4`
- `source SetupAnalysis.sh && cd MainAnalysis/20241102_ZhadronVsZPt && make -j4`
- `source SetupAnalysis.sh && cd MainAnalysis/20260222_EnergyExtrapolation && make -j4`
- `source SetupAnalysis.sh && cd MainAnalysis/20260115_ZCorrection && make -j4`
- `source SetupAnalysis.sh && cd MainAnalysis/20251211_ResidualCorrection && make -j4`

Targeted grep / inspection validation:

- `rg -n "VZWeightFile|isData" MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`
- `rg -n "VZWeightFile|isData" MainAnalysis/20260222_EnergyExtrapolation/CorrelationAnalysis.cpp`
- `rg -n "VZWeightFile|isData" MainAnalysis/20260115_ZCorrection/CorrelationAnalysis.cpp`
- `rg -n "VZWeightFile|isData" MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp`
- `rg -n "VZWeightFile" MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `rg -n "VZWeightFile" MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `rg -n "VZWeightFile" MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`

## Output cleanup performed

Removed only the affected stale families before rerun:

- pPb/PbP data roots for `pPb_{nominal,ZResidual,trkResidual}_ZV6_trkV24_nmix10_*`
- pPb/PbP data roots for `PbP_{nominal,ZResidual,trkResidual}_ZV6_trkV24_nmix10_*`
- pp extrapolation roots for `pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10_*`
- stale PDFs in:
  - `Plots/20260213_Central/plots/central/`
  - `Plots/20260213_Central/plots/central_overlay_PPbPbP/`
  - `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/`
  - `Plots/20260213_Central/plots/energyExtrapolation/`
  - pPb/PbP entries in `Plots/20260213_Central/plots/zmass/`

## Regeneration commands run

Heavy-ion central data families:

- targeted `system-analysis.sh` reruns in `MainAnalysis/20241102_ZhadronVsZPt/` for:
  - `pPb_nominal_ZV6_trkV24_nmix10`
  - `pPb_ZResidual_ZV6_trkV24_nmix10`
  - `pPb_trkResidual_ZV6_trkV24_nmix10`
  - `PbP_nominal_ZV6_trkV24_nmix10`
  - `PbP_ZResidual_ZV6_trkV24_nmix10`
  - `PbP_trkResidual_ZV6_trkV24_nmix10`
- config phases used:
  - `ZPT_RANGES=("5_30" "30_500"), PT_RANGES=("0.5_4" "4_500")`
  - `ZPT_RANGES=("5_500"), PT_RANGES=("0.5_500")`

Heavy-ion MC references restored for overlay after an accidental cleanup of unaffected refs:

- targeted `system-analysis.sh` reruns for:
  - `pPbMC_Gen_nominal_ZV6_trkV24_nmix10`
  - `PbPMC_Gen_nominal_ZV6_trkV24_nmix10`

Plot refreshes:

- `cd Plots/20260213_Central && ./plot-central.sh`
- `cd Plots/20260213_Central && ./plot-central-overlay-PPbPbP.sh`

pp energy extrapolation:

- `cd MainAnalysis/20260222_EnergyExtrapolation/workflow && export NTHREAD=8 && ./pythia-analysis.sh`
- targeted `system-analysis.sh` reruns in `MainAnalysis/20241102_ZhadronVsZPt/` for:
  - `pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10`
  - using `--EnergyExtraFile /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260317_EnergyExtrapolation_EEV3.root`
- downstream plotters:
  - `cd Plots/20260213_Central && ./plot-energyExtrapolation.sh`
  - `cd Plots/20260213_Central && ./plot-central-combined.sh`
  - `cd Plots/20260213_Central && ./plot-zmass.sh`

## Regenerated outputs verified

Counts after rerun:

- pPb/PbP central data roots: `9` pPb + `9` PbP result roots
- pPb/PbP MC overlay refs: `6` result roots total
- `pp_EExtrapolation_*`: `3` result roots
- `Plots/20260213_Central/plots/central/*ZV6_trkV24_nmix10*.pdf`: `20`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_*.pdf`: `10`
- `Plots/20260213_Central/plots/energyExtrapolation/ZV6_trkV24_EEV3_nmix10_*.pdf`: `10`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/*.pdf`: `30`
- `Plots/20260213_Central/plots/zmass/{pPb,PbP}_ZPT0_500-*.pdf`: `2`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260317_EnergyExtrapolation_EEV3*`: `3`

Representative refreshed artifact stats captured during validation:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260317_EnergyExtrapolation_EEV3.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10_ZPT5_500-result.root`
- `Plots/20260213_Central/plots/energyExtrapolation/ZV6_trkV24_EEV3_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/zmass/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf`

## Overleaf / note updates

Copied refreshed figure families with source basenames preserved:

- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_*.pdf`
  - to `OverleafZHadronInPPb/figures/result/`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_*.pdf`
  - to `OverleafZHadronInPPb/figures/analysis/combining/`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260317_EnergyExtrapolation_EEV3.pdf`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260317_EnergyExtrapolation_EEV3-closure.pdf`
- `Plots/20260213_Central/plots/energyExtrapolation/ZV6_trkV24_EEV3_nmix10_*.pdf`
  - to `OverleafZHadronInPPb/figures/analysis/energy/`
- `Plots/20260213_Central/plots/zmass/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf`
- `Plots/20260213_Central/plots/zmass/PbP_ZPT0_500-ZV6_trkV24_nmix10.pdf`
  - to `OverleafZHadronInPPb/figures/z_reco/`

TeX reference updates:

- `OverleafZHadronInPPb/src/z_reco.tex`
  - `figures/z_reco/zmass_ppb.pdf` -> `figures/z_reco/pPb_ZPT0_500-ZV6_trkV24_nmix10.pdf`
  - `figures/z_reco/zmass_pbp.pdf` -> `figures/z_reco/PbP_ZPT0_500-ZV6_trkV24_nmix10.pdf`

No result / overlay / energy TeX path changes were needed because those note references already matched the refreshed source basenames.

## Scope confirmation

- No pp correction derivation reruns were done in:
  - `MainAnalysis/20260115_ZCorrection/`
  - `MainAnalysis/20251211_ResidualCorrection/`
- No `plot-pp` support workflow was rerun for this task.

## Failures / retries / adjustments

1. Initial attempt to rerun `central.sh 0 1 1` directly was too slow for end-to-end completion.
   - Resolution: switched to reviewer-accurate targeted reruns of only the stale pPb/PbP data families via `system-analysis.sh`, using the same official correction inputs and cut config.

2. While clearing partial products from the abandoned full `central.sh` attempt, I also removed the unaffected `pPbMC_Gen_nominal_*` / `PbPMC_Gen_nominal_*` overlay reference roots.
   - Resolution: restored those two MC GEN nominal families with targeted reruns and then reran `plot-central-overlay-PPbPbP.sh`.

3. `Plots/20260213_Central/plot-energyExtrapolation.sh` did not recreate the inclusive `ZPT5_500 / trkPT0.5_500` pair needed by the note after the stale files were removed.
   - Resolution: patched the script to add the inclusive `ExecuteEnergyExtrapolationPlot` call, reran it, and verified the inclusive PDFs were regenerated and copied to the note checkout.
