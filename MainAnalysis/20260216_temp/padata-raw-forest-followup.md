# pPb-data raw-forest muon-track follow-up (`20260216_temp`)

This note is the raw-forest follow-up to the earlier rectangle-localization study. The helper was extended minimally in the sandbox so it can scan the **raw PAData source forests** with no event selection, no weighting, no dimuon preselection, and no track-quality cuts.

## Sandbox helper change

Updated sandbox-only helper:

- `MainAnalysis/20260216_temp/scan_muon_track_rectangle.cpp`

New mode added:

- `--ForestRecoMode raw`

In that mode the helper uses the raw forest branches directly:

- reco muons from `MuTreeMessenger::NGlb`, `GlbEta`, `GlbPhi`, `GlbPT`
- dimuon multiplicity summary from `MuTreeMessenger::NDi`
- reco tracks from `TrackTreeMessenger::nTrk`, `trkEta`, `trkPhi`, `trkPt`

and produces three raw `(DeltaEta, DeltaPhi)` histograms:

1. all reco-muon / reco-track pairs
2. the closest track to each muon (minimum `DeltaR` in the event, per muon)
3. all reco-muon / reco-track pairs after excluding the event-level union of those closest-track indices

## Definitions used here

### Primary muon definition for the raw plots

For the raw pair plots I used the **single reco-muon list** (`NGlb`, `GlbEta`, `GlbPhi`) as the primary definition, because the user asked whether the **muon itself** reappears inside the raw reco-track list.

### Ambiguous “exactly two muons” question

I report both raw multiplicities explicitly:

- **single reco muon multiplicity:** `NGlb`
- **raw dimuon candidate multiplicity:** `NDi`

`NDi == 1` means the forest stores exactly one dimuon candidate (with two legs in `DiEta1/2`, `DiPhi1/2`, etc.), but that is a dimuon-candidate count, not a raw single-muon count. For the rest of this note, the more direct answer to “does the muon appear in the track list?” uses the raw `NGlb` muons.

## Input sample and event count

Input sample used for the final study:

- first **300 files** from
  - `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/`

Processed event count:

- exactly **1,000,000 events** (`--MaxEvents 1000000`)

## Commands run

Environment / build:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd CommonCode && make -j2
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp
make ExecuteMuonTrackRectangleScan -j2
```

Smoke validation:

```bash
mkdir -p output/raw_forest_followup plots/raw_forest_followup
PADATA=$(find /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000 -maxdepth 1 -type f | sort | head -n 3 | paste -sd, -)
./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --Input "$PADATA" \
  --Label 'PA data raw forest reco/reco smoke (1k events)' \
  --MaxEvents 1000 \
  --Output plots/raw_forest_followup/pPbData_rawForest_smoke \
  > output/raw_forest_followup/pPbData_rawForest_smoke.log
```

Final 1M-event study:

```bash
PADATA=$(find /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000 -maxdepth 1 -type f | sort | head -n 300 | paste -sd, -)
./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --Input "$PADATA" \
  --Label 'PA data raw forest reco/reco (1M events, first 300 files from Samples/PAData/0000)' \
  --MaxEvents 1000000 \
  --Output plots/raw_forest_followup/pPbData_rawForest_1M_first300 \
  > output/raw_forest_followup/pPbData_rawForest_1M_first300.log
```

Near-origin occupancy cross-check from the output ROOT file:

```bash
root -l -b -q output/raw_forest_followup/measure_core.C
```

(The macro was only a scratch check and was removed after extracting the numbers below.)

## Final outputs

Primary output files:

- `plots/raw_forest_followup/pPbData_rawForest_1M_first300.pdf`
  - 3 panels: all pairs, closest only, excluding closest
- `plots/raw_forest_followup/pPbData_rawForest_1M_first300.root`
  - `hAllRawPairs`
  - `hClosestRawPairs`
  - `hExcludingClosestRawPairs`
- `output/raw_forest_followup/pPbData_rawForest_1M_first300.log`

Validation / smoke outputs kept in the same sandbox folders:

- `plots/raw_forest_followup/pPbData_rawForest_smoke.pdf`
- `plots/raw_forest_followup/pPbData_rawForest_smoke.root`
- `output/raw_forest_followup/pPbData_rawForest_smoke.log`

## Raw multiplicity summary (1M events)

From `output/raw_forest_followup/pPbData_rawForest_1M_first300.log`:

- `processed_events = 1000000`
- `events_nglb_eq_2 = 55118`
- `events_ndi_eq_1 = 54922`
- `events_nglb_eq_2_and_ndi_eq_1 = 54920`

`NGlb` multiplicity breakdown:

- `NGlb = 0`: 260958 events (26.0958%)
- `NGlb = 1`: 681138 events (68.1138%)
- `NGlb = 2`: 55118 events (5.5118%)
- `NGlb = 3`: 2673 events (0.2673%)
- `NGlb = 4`: 108 events (0.0108%)
- `NGlb >= 5`: 5 events (0.0005%)

`NDi` multiplicity breakdown:

- `NDi = 0`: 942295 events (94.2295%)
- `NDi = 1`: 54922 events (5.4922%)
- `NDi = 2`: 22 events (0.0022%)
- `NDi = 3`: 2648 events (0.2648%)
- `NDi >= 5`: 113 events (0.0113%)

## Direct answers to the four requested questions

### 1. For each data event, is there exactly two muons?

**No, not in the raw forest if “muons” means the raw reco single-muon list `NGlb`.**

Only **55,118 / 1,000,000 = 5.51%** of events have `NGlb == 2`.
Most events have either:

- `NGlb == 1` (68.11%), or
- `NGlb == 0` (26.10%).

If the question is instead interpreted as **one raw dimuon candidate** (`NDi == 1`), that happens in **54,922 / 1,000,000 = 5.49%** of events.

So the clean answer is:

- **raw single-muon interpretation (`NGlb`)**: usually **not** exactly two muons
- **raw dimuon-candidate interpretation (`NDi`)**: also only about **5.5%** of events have exactly one dimuon candidate

### 2. Does the muon appear in the list of tracks as well?

**Practically yes, as a very nearby reco track, but not as an exact `(eta,phi)` duplicate.**

For the 799,850 raw reco muons in the 1M-event scan:

- `muons_with_closest_track = 799850` (every raw muon had a closest reco track)
- `closest_exact_coordinate_match = 0`
- `closest_abs_window_1e-5 = 69`
- `closest_abs_window_1e-4 = 7806`
- `closest_abs_window_1e-3 = 747826`
- `closest_deltar_window_1e-3 = 600000`

So:

- **93.50%** of muons have a closest reco track within `|DeltaEta|, |DeltaPhi| < 1e-3`
- **75.01%** have a closest reco track within `DeltaR < 1e-3`
- **0** are exact float-coordinate matches in both `eta` and `phi`

This strongly supports “the muon reappears in the reco-track list,” but with small coordinate differences rather than exact branch duplication.

### 3. Make a 2D plot for reco-muon vs reco-track `(DeltaEta, DeltaPhi)` using raw branch values only, with no matching, event selections, or weighting.

Done.

The plot is the **left panel** of:

- `plots/raw_forest_followup/pPbData_rawForest_1M_first300.pdf`

and the corresponding histogram is:

- `hAllRawPairs` in `plots/raw_forest_followup/pPbData_rawForest_1M_first300.root`

Quantitatively, that all-pairs histogram contains:

- `135734143` total muon-track pairs
- `747922` entries inside the near-origin box `|DeltaEta|, |DeltaPhi| < 0.001`

### 4. Make the same kind of plot for (a) only the closest track to each muon and (b) all tracks excluding those closest tracks.

Done.

These are the **middle** and **right** panels of:

- `plots/raw_forest_followup/pPbData_rawForest_1M_first300.pdf`

and the corresponding histograms are:

- `hClosestRawPairs`
- `hExcludingClosestRawPairs`

in:

- `plots/raw_forest_followup/pPbData_rawForest_1M_first300.root`

Near-origin occupancy shows that the raw central feature is almost entirely carried by the closest-track component:

| Histogram | Total entries | Entries with `|DeltaEta|, |DeltaPhi| < 0.001` |
| --- | ---: | ---: |
| all pairs | 135734143 | 747922 |
| closest only | 799850 | 747826 |
| excluding closest | 134806633 | 96 |

So the near-origin rectangle/core is overwhelmingly a **closest-track-to-muon** effect. Once those closest tracks are removed, the central structure essentially disappears.

## Bottom line

For this raw pPb-data forest study:

1. **No**, events do not generally contain exactly two raw reco muons; `NGlb == 2` only in **5.51%** of events.
2. **Yes**, the reco muon almost always has a very nearby reco track counterpart in the raw track list, though not at exactly identical stored `(eta,phi)` values.
3. The requested raw all-pairs plot was produced.
4. The closest-only and excluding-closest plots were also produced, and the central near-origin structure is almost entirely contained in the **closest-track** component.
