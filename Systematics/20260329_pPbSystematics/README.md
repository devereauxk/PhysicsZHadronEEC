# 20260329 pPb systematics workspace

This directory is the compiled harvesting and plotting workspace for the note-facing Z-hadron systematic uncertainties derived from `MainAnalysis/20241102_ZhadronVsZPt` final corrected outputs.

## Inputs

- Nominal corrected result ROOT files from `MainAnalysis/20241102_ZhadronVsZPt/plots/`:
  - `pp_trkResidual_<OFFICIAL_TAG_PP>_ZPT*-result.root`
  - `pPb_trkResidual_<OFFICIAL_TAG_PPB>_ZPT*-result.root`
  - `PbP_trkResidual_<OFFICIAL_TAG_PPB>_ZPT*-result.root`
- Systematic variations from the rewritten `MainAnalysis/20241102_ZhadronVsZPt/systematics.sh` runner:
  - `Loose`, `Tight`
  - `IsMuTaggedFalse`
  - `IsPURejectTrue`
  - `MuVar0..3`

## Main files

- `legacy_systematics.C`: copied reference macro from `MainAnalysis/20241102_ZhadronVsZPt/systematics.C`
- `CalculateSystematics.cpp`: compiled bin-by-bin uncertainty calculator
- `PlotSystematics.cpp`: compiled plotter for absolute/relative uncertainty overlays and central-value plots with systematic error bars
- `PlotPUComparison.cpp`: dedicated corrected-data nominal vs `IsPURejectTrue` plotter with the nominal/PU overlay on top and a subtraction panel (`PU reject - nominal`) below
- `PlotTrackSelectionDiff.cpp`: dedicated corrected-data nominal/loose/tight overlay plotter with a subtraction lower panel relative to nominal
- `PlotTrackCorrectionDiff.cpp`: dedicated corrected-data nominal/0.976/1.024 overlay plotter with a subtraction lower panel relative to nominal
- `PlotMuonTrackComparison.cpp`: dedicated corrected-data nominal vs `IsMuTaggedFalse` plotter with the nominal/rejection overlay on top and a subtraction panel (`variation - nominal`) below
- `PlotEnergyExtrapolationComparison.cpp`: dedicated pp corrected-data nominal vs `_EEPrivate` (`MC-driven`) plotter with the nominal/variation overlay on top and a subtraction panel (`MC-driven - nominal`) below
- `run.sh`: runner for calculation and plotting over selected systems and kinematic bins
- `run-pu.sh`: runner for standalone PU comparison PDFs
- `run-track-selection.sh`: runner for standalone loose/nominal/tight PDFs
- `run-track-correction.sh`: runner for standalone track-correction comparison PDFs
- `run-muon-track.sh`: runner for standalone muon-track rejection comparison PDFs
- `run-energy-extrapolation.sh`: runner for standalone pp nominal vs `MC-driven` energy-extrapolation comparison PDFs
- `makefile`: local build rules

## Output content

For each nominal result file and requested track-pt selection, `CalculateSystematics.cpp` writes the quoted `0.5`-normalized family magnitudes:

- `TrackSelection_{DeltaPhi,DeltaEta}`
- `TrackCorrection_{DeltaPhi,DeltaEta}`
- `MuonRejection_{DeltaPhi,DeltaEta}`
- `PUpp_{DeltaPhi,DeltaEta}`
- `PUpPb_{DeltaPhi,DeltaEta}`
- `ScaleFactor_{DeltaPhi,DeltaEta}`
- `EnergyExtrapolation_{DeltaPhi,DeltaEta}`
- `Total_{DeltaPhi,DeltaEta}`

The tracking-correction term is harvested from the dedicated corrected-data `TrackCorrection0p976` / `TrackCorrection1p024` variation family, taking the maximum absolute difference with respect to nominal in each bin. `Total` is formed by quadrature over the families selected through the runner. The maintained defaults now omit `MuonRejection` from that harvested total unless it is explicitly re-added through `INCLUDE_FAMILIES`.

`run.sh` now applies the PU family collision-by-collision by default:

- `pp`: `PUpp` only, with `TrackSelection` omitted because the official pp skim has no loose/tight trees
- `pp`: `EnergyExtrapolation` is harvested from the corrected-data `_EEPrivate` result against the nominal pp corrected output
- `pPb`: `PUpPb` only
- `PbP`: `PUpPb` only

The `MuonRejection` input is the corrected-data `IsMuTaggedFalse` variation for the matching collision system, and the overlay legend shows a single `PU` entry for the active pileup family. The standalone muon-track comparison runner keeps the nominal versus `IsMuTaggedFalse` shape comparison available independently of the default total.

The plotter now writes, for each observable:

- `<base>-DeltaPhi-absolute.pdf` / `<base>-DeltaEta-absolute.pdf`: absolute uncertainty overlays
- `<base>-DeltaPhi-relative.pdf` / `<base>-DeltaEta-relative.pdf`: relative uncertainty overlays, defined bin-by-bin as `absolute uncertainty / |nominal central value|`
- `<base>-DeltaPhi-central.pdf` / `<base>-DeltaEta-central.pdf`: central-value plots where the bin errors are replaced by the harvested total systematic uncertainty

The dedicated comparison runners read the corrected-data result ROOT files directly from `MainAnalysis/20241102_ZhadronVsZPt/plots/`, not the harvested `*-systematics.root` outputs:

- `run-pu.sh` writes `plots/pu/<system>_<official-tag>_ZPT<range>_trkPT<range>-PU-Delta{Phi,Eta}.pdf`
- `run-track-selection.sh` writes `plots/trackSelection/<system>_<official-tag>_ZPT<range>_trkPT<range>-TrackSelection-Delta{Phi,Eta}.pdf`
- `run-track-correction.sh` writes `plots/trackCorrection/<system>_<official-tag>_ZPT<range>_trkPT<range>-TrackCorrection-Delta{Phi,Eta}.pdf`
- `run-muon-track.sh` writes `plots/muonTrack/<system>_<official-tag>_ZPT<range>_trkPT<range>-MuonTrack-Delta{Phi,Eta}.pdf`
- `run-energy-extrapolation.sh` writes `plots/energyExtrapolation/pp_<official-tag>_ZPT<range>_trkPT<range>-EnergyExtrapolation-Delta{Phi,Eta}.pdf`

All standalone plotters apply the note-facing quoted normalization `0.5 * Delta{Phi,Eta}_Result<trkPT>` before drawing, matching the compiled `CalculateSystematics.cpp` convention for harvested systematics and the compiled `PlotSystematics.cpp` treatment of nominal central values.
For PU, the lower panel is the subtraction `0.5 * (IsPURejectTrue - nominal)`; for track selection, the output uses the same two-pad layout with `0.5`-normalized nominal/loose/tight overlaid above and `0.5 * (Loose - nominal)` / `0.5 * (Tight - nominal)` below. The dedicated track-correction comparison does the analogous `0.5`-normalized nominal/0.976/1.024 overlay with lower panels showing `variation - nominal`, and the muon-track comparison uses the same layout for nominal versus `IsMuTaggedFalse`.

## Typical usage

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd Systematics/20260329_pPbSystematics
make
SYSTEMS=pp,pPb,PbP ZPT_RANGES=40_350 TRACK_RANGES=2_500 INCLUDE_FAMILIES=TrackSelection,TrackCorrection,MuonRejection,ScaleFactor PLOT_FAMILIES=TrackSelection,TrackCorrection,MuonRejection,ScaleFactor ./run.sh
SYSTEMS=pp,pPb,PbP ZPT_RANGES=40_350 TRACK_RANGES=2_500 ./run-pu.sh
SYSTEMS=pPb,PbP ZPT_RANGES=40_350 TRACK_RANGES=2_500 ./run-track-selection.sh
SYSTEMS=pp,pPb,PbP ZPT_RANGES=40_350 TRACK_RANGES=2_500 ./run-track-correction.sh
SYSTEMS=pp,pPb,PbP,pPbPbp ZPT_RANGES=40_350 TRACK_RANGES=2_500 ./run-muon-track.sh
SYSTEMS=pp ZPT_RANGES=5_500 TRACK_RANGES=0.5_500 ./run-energy-extrapolation.sh
```

Use `SYSTEMS`, `ZPT_RANGES`, and `TRACK_RANGES` to narrow the standalone comparison runners without changing the code. For `run.sh`, `INCLUDE_FAMILIES`, `PLOT_FAMILIES`, `DO_CALC`, and `DO_PLOT` remain available; if `INCLUDE_FAMILIES` / `PLOT_FAMILIES` are left unset, `run.sh` picks the collision-specific default PU family listed above, adds `EnergyExtrapolation` for `pp`, and leaves `MuonRejection` out of the default harvested total.
