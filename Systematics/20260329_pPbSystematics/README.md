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
  - `IsPURejectFalse`
  - `MuVar0..3`

## Main files

- `legacy_systematics.C`: copied reference macro from `MainAnalysis/20241102_ZhadronVsZPt/systematics.C`
- `CalculateSystematics.cpp`: compiled bin-by-bin uncertainty calculator
- `PlotSystematics.cpp`: compiled plotter for absolute/relative uncertainty overlays and central-value plots with systematic error bars
- `PlotPUComparison.cpp`: dedicated corrected-data nominal vs `IsPURejectFalse` plotter with the nominal/PU overlay on top and a subtraction panel (`PU=1 - nominal`) below
- `PlotTrackSelectionDiff.cpp`: dedicated corrected-data nominal/loose/tight overlay plotter with a subtraction lower panel relative to nominal
- `run.sh`: runner for calculation and plotting over selected systems and kinematic bins
- `run-pu.sh`: runner for standalone PU comparison PDFs
- `run-track-selection.sh`: runner for standalone loose/nominal/tight PDFs
- `makefile`: local build rules

## Output content

For each nominal result file and requested track-pt selection, `CalculateSystematics.cpp` writes the quoted `0.5`-normalized family magnitudes:

- `TrackSelection_{DeltaPhi,DeltaEta}`
- `TrackCorrection_{DeltaPhi,DeltaEta}`
- `MuonRejection_{DeltaPhi,DeltaEta}`
- `PUpp_{DeltaPhi,DeltaEta}`
- `PUpPb_{DeltaPhi,DeltaEta}`
- `ScaleFactor_{DeltaPhi,DeltaEta}`
- `Total_{DeltaPhi,DeltaEta}`

The tracking-correction term is implemented as a flat 2.1% absolute contribution per bin, and `Total` is formed by quadrature over the families selected through the runner.

`run.sh` now applies the PU family collision-by-collision by default:

- `pp`: `PUpp` only
- `pPb`: `PUpPb` only
- `PbP`: `PUpPb` only

The `MuonRejection` input is the corrected-data `IsMuTaggedFalse` variation for the matching collision system, and the overlay legend shows a single `PU` entry for the active pileup family.

The plotter now writes, for each observable:

- `<base>-DeltaPhi-absolute.pdf` / `<base>-DeltaEta-absolute.pdf`: absolute uncertainty overlays
- `<base>-DeltaPhi-relative.pdf` / `<base>-DeltaEta-relative.pdf`: relative uncertainty overlays, defined bin-by-bin as `absolute uncertainty / |nominal central value|`
- `<base>-DeltaPhi-central.pdf` / `<base>-DeltaEta-central.pdf`: central-value plots where the bin errors are replaced by the harvested total systematic uncertainty

The dedicated comparison runners read the corrected-data result ROOT files directly from `MainAnalysis/20241102_ZhadronVsZPt/plots/`, not the harvested `*-systematics.root` outputs:

- `run-pu.sh` writes `plots/pu/<system>_<official-tag>_ZPT<range>_trkPT<range>-PU-Delta{Phi,Eta}.pdf`
- `run-track-selection.sh` writes `plots/trackSelection/<system>_<official-tag>_ZPT<range>_trkPT<range>-TrackSelection-Delta{Phi,Eta}.pdf`

Both standalone plotters apply the note-facing quoted normalization `0.5 * Delta{Phi,Eta}_Result<trkPT>` before drawing, matching the compiled `CalculateSystematics.cpp` convention for harvested systematics and the compiled `PlotSystematics.cpp` treatment of nominal central values.
For PU, the lower panel is the subtraction `0.5 * (IsPURejectFalse - nominal)`; for track selection, the output uses the same two-pad layout with `0.5`-normalized nominal/loose/tight overlaid above and `0.5 * (Loose - nominal)` / `0.5 * (Tight - nominal)` below.

## Typical usage

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd Systematics/20260329_pPbSystematics
make
SYSTEMS=pPb ZPT_RANGES=40_350 TRACK_RANGES=2_500 INCLUDE_FAMILIES=TrackSelection,TrackCorrection,MuonRejection,ScaleFactor PLOT_FAMILIES=TrackSelection,TrackCorrection,MuonRejection,ScaleFactor ./run.sh
SYSTEMS=pp,pPb,PbP ZPT_RANGES=40_350 TRACK_RANGES=2_500 ./run-pu.sh
SYSTEMS=pp,pPb,PbP ZPT_RANGES=40_350 TRACK_RANGES=2_500 ./run-track-selection.sh
```

Use `SYSTEMS`, `ZPT_RANGES`, and `TRACK_RANGES` to narrow the standalone comparison runners without changing the code. For `run.sh`, `INCLUDE_FAMILIES`, `PLOT_FAMILIES`, `DO_CALC`, and `DO_PLOT` remain available; if `INCLUDE_FAMILIES` / `PLOT_FAMILIES` are left unset, `run.sh` picks the collision-specific default PU family listed above.
