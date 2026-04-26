# PP Study Summary (2026-03-17)

## Scope

Reproduced the pp comparison workflow used by `Plots/20260213_Central/plot_pp.cpp` with the updated pp correction chain:

- VZ weight: `20260317_ZV6_ZPT0_350_VzReweightFits_pp.root`
- Z weight: `20260317_ZCorrection_V6_pp_zPt0-500.root`
- Track residual: `20260317_TrackResidualCorrection_V24_ZWeight_V6_pp_zPt*`

Requested kinematics:

- `ZPT40_350`
- `trkPT`: `1_2`, `2_4`, `4_10`

## Code / Workflow Updates

1. Added a dedicated production script:
   - `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
   - Produces `plot_pp.cpp` inputs with the new VZ/Z/track weights.
   - Configured for `ZPT40_350` and track bins `1_2`, `2_4`, `4_10`.
   - Uses `SKIP_CLEAN=1` by default and keeps workflow local/clean.

2. Updated plotting runner tag:
   - `Plots/20260213_Central/plot-pp.sh`
   - Uses `evtWeightOn_ZV6_trkV24_nmix10` for the new comparison output set.

3. Updated project instructions:
   - `.github/copilot-instructions.md`
   - Added Overleaf structure and figure copy workflow guidance.

## Production Outputs

Regenerated pp analysis ROOT inputs (example `-result.root` products):

- `MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_Gen_nominal_evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350-result.root`

Regenerated pp comparison PDFs:

- `Plots/20260213_Central/plots/pp/evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350_trkPT1_2-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350_trkPT1_2-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/pp/evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350_trkPT2_4-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350_trkPT2_4-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/pp/evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350_trkPT4_10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/pp/evtWeightOn_ZV6_trkV24_nmix10_ZPT40_350_trkPT4_10-DeltaPhi-result.pdf`

## Validation Performed

- Confirmed the new weighted input ROOT files exist and are non-empty.
- Ran `Plots/20260213_Central/plot-pp.sh` successfully.
- Confirmed all six requested `DeltaEta/DeltaPhi` pp comparison PDFs were produced under `Plots/20260213_Central/plots/pp/`.

## Notes for Reviewer Agent

- This task intentionally kept plots inside `Plots/20260213_Central` (no Overleaf copy in this step).
- The dedicated production script supports both `evtWeightOn` and `evtWeightOff` tag generation; current plotted output is the updated `evtWeightOn_ZV6_trkV24_nmix10` set used for this refresh.
