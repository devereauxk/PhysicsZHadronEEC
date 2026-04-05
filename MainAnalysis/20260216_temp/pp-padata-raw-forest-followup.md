# pp and pPb raw-forest muon-track follow-up (`20260216_temp`)

This note extends the earlier pPb-only raw-forest check to **pp data** and **pPb data**, keeping the work sandbox-local. The goal was to compare raw reco-muon vs raw reco-track `(DeltaEta, DeltaPhi)` structures for about **1M events per system**, using both:

1. the raw single-muon list (`NGlb`, `GlbEta`, `GlbPhi`, `GlbPT`), and
2. the raw dimuon-leg list built directly from `NDi` with `DiEta1/2`, `DiPhi1/2`, `DiPT1/2`.

For each muon definition I made three plots:

- all muon-track pairs,
- only the closest track to each muon,
- all pairs after excluding the union of those closest-track indices.

## Minimal sandbox helper change

Updated sandbox helper:

- `scan_muon_track_rectangle.cpp`

Minimal extension added in raw-forest mode:

- retained existing `--ForestRecoMode raw` behavior for the raw `NGlb` path,
- added `--RawMuonSource {single,dimuonleg}`.

`single` means the raw single reco-muon list from `NGlb`.

`dimuonleg` means **every stored dimuon leg from every raw dimuon candidate** in `NDi`. This is intentionally branch-level and minimal: if the same physical muon appears in multiple dimuon candidates, it is counted multiple times here. The `unique_closest_tracks_excluded / total_raw_muons` ratios below (`~0.93-0.94` for dimuon legs) are consistent with that repeated-leg behavior.

## Inputs used

Source forests:

- `SampleGeneration/20250929_ReducedTreePA/Samples/PPData/...`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/...`

Final subsets used:

- **pp data:** first **400** numbered files under `Samples/PPData/[0-9][0-9][0-9][0-9]/`
- **pPb data:** first **300** numbered files under `Samples/PAData/[0-9][0-9][0-9][0-9]/`

Each final run processed exactly **1,000,000 events**.

## Commands run

Build:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd CommonCode && make -j2
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp
make ExecuteMuonTrackRectangleScan -j2
```

Smoke validation (1k events per case):

```bash
PPDATA=$(find -L /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PPData \
  -path '*/250419_234027' -prune -o -type f | sort | head -n 3 | paste -sd, -)
PADATA=$(find -L /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAData \
  -path '*/250419_234027' -prune -o -type f | sort | head -n 3 | paste -sd, -)

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource single --Input "$PPDATA" \
  --MaxEvents 1000 --Output plots/raw_forest_followup/ppData_rawForest_single_smoke \
  > output/raw_forest_followup/ppData_rawForest_single_smoke.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource dimuonleg --Input "$PPDATA" \
  --MaxEvents 1000 --Output plots/raw_forest_followup/ppData_rawForest_dimuonleg_smoke \
  > output/raw_forest_followup/ppData_rawForest_dimuonleg_smoke.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource single --Input "$PADATA" \
  --MaxEvents 1000 --Output plots/raw_forest_followup/pPbData_rawForest_single_smoke \
  > output/raw_forest_followup/pPbData_rawForest_single_smoke.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource dimuonleg --Input "$PADATA" \
  --MaxEvents 1000 --Output plots/raw_forest_followup/pPbData_rawForest_dimuonleg_smoke \
  > output/raw_forest_followup/pPbData_rawForest_dimuonleg_smoke.log
```

Final 1M-event studies:

```bash
PPDATA=$(find /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PPData/[0-9][0-9][0-9][0-9] \
  -maxdepth 1 -type f | sort | head -n 400 | paste -sd, -)
PADATA=$(find /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAData/[0-9][0-9][0-9][0-9] \
  -maxdepth 1 -type f | sort | head -n 300 | paste -sd, -)

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource single --Input "$PPDATA" \
  --Label 'pp data raw forest single-muon (1M events, first 400 numbered files)' \
  --MaxEvents 1000000 \
  --Output plots/raw_forest_followup/ppData_rawForest_single_1M_first400 \
  > output/raw_forest_followup/ppData_rawForest_single_1M_first400.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource dimuonleg --Input "$PPDATA" \
  --Label 'pp data raw forest dimuon-leg (1M events, first 400 numbered files)' \
  --MaxEvents 1000000 \
  --Output plots/raw_forest_followup/ppData_rawForest_dimuonleg_1M_first400 \
  > output/raw_forest_followup/ppData_rawForest_dimuonleg_1M_first400.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource single --Input "$PADATA" \
  --Label 'pPb data raw forest single-muon (1M events, first 300 numbered files)' \
  --MaxEvents 1000000 \
  --Output plots/raw_forest_followup/pPbData_rawForest_single_1M_first300 \
  > output/raw_forest_followup/pPbData_rawForest_single_1M_first300.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --ForestRecoMode raw --RawMuonSource dimuonleg --Input "$PADATA" \
  --Label 'pPb data raw forest dimuon-leg (1M events, first 300 numbered files)' \
  --MaxEvents 1000000 \
  --Output plots/raw_forest_followup/pPbData_rawForest_dimuonleg_1M_first300 \
  > output/raw_forest_followup/pPbData_rawForest_dimuonleg_1M_first300.log
```

ROOT-file validation:

```bash
root -l -b <<'EOF2'
TFile f1("plots/raw_forest_followup/ppData_rawForest_single_1M_first400.root"); f1.ls();
TFile f2("plots/raw_forest_followup/ppData_rawForest_dimuonleg_1M_first400.root"); f2.ls();
TFile f3("plots/raw_forest_followup/pPbData_rawForest_single_1M_first300.root"); f3.ls();
TFile f4("plots/raw_forest_followup/pPbData_rawForest_dimuonleg_1M_first300.root"); f4.ls();
.q
EOF2
```

Each ROOT file contains:

- `hAllRawPairs`
- `hClosestRawPairs`
- `hExcludingClosestRawPairs`

## Final outputs

Primary study products:

- `plots/raw_forest_followup/ppData_rawForest_single_1M_first400.pdf`
- `plots/raw_forest_followup/ppData_rawForest_single_1M_first400.root`
- `output/raw_forest_followup/ppData_rawForest_single_1M_first400.log`
- `plots/raw_forest_followup/ppData_rawForest_dimuonleg_1M_first400.pdf`
- `plots/raw_forest_followup/ppData_rawForest_dimuonleg_1M_first400.root`
- `output/raw_forest_followup/ppData_rawForest_dimuonleg_1M_first400.log`
- `plots/raw_forest_followup/pPbData_rawForest_single_1M_first300.pdf`
- `plots/raw_forest_followup/pPbData_rawForest_single_1M_first300.root`
- `output/raw_forest_followup/pPbData_rawForest_single_1M_first300.log`
- `plots/raw_forest_followup/pPbData_rawForest_dimuonleg_1M_first300.pdf`
- `plots/raw_forest_followup/pPbData_rawForest_dimuonleg_1M_first300.root`
- `output/raw_forest_followup/pPbData_rawForest_dimuonleg_1M_first300.log`

Smoke outputs were also kept in the same sandbox folders.

## Quantitative summary

### Event-level raw multiplicities

| System | `NGlb == 2` | `NDi == 1` |
| --- | ---: | ---: |
| pp data | 106690 / 1M = **10.669%** | 106392 / 1M = **10.6392%** |
| pPb data | 55118 / 1M = **5.5118%** | 54922 / 1M = **5.4922%** |

So neither raw data sample is “exactly two muons per event” in the single-muon sense; the dimuon-candidate interpretation is also only a minority of events.

### Main comparison table

Near-origin means `|DeltaEta|, |DeltaPhi| < 0.001`.

| System | Raw muon source | Total raw muons | All pairs | Closest pairs | Excluding-closest pairs | Near-origin all | Near-origin closest | Near-origin excluding |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| pp | single | 1071027 | 126246849 | 1071027 | 124935111 | 1032641 | 1032494 | 143 |
| pp | dimuon-leg | 239948 | 29198966 | 239948 | 28690160 | 226507 | 226477 | 27 |
| pPb | single | 799850 | 135734143 | 799850 | 134806633 | 747922 | 747826 | 96 |
| pPb | dimuon-leg | 127210 | 23841954 | 127210 | 23568682 | 117180 | 117168 | 12 |

### Closest-track match windows

| System | Raw muon source | exact `(0,0)` | `|DeltaEta|,|DeltaPhi| < 1e-5` | `< 1e-4` | `< 1e-3` | `DeltaR < 1e-3` |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| pp | single | 0 | 107 | 10730 | 1032494 | 824373 |
| pp | dimuon-leg | 0 | 18 | 2284 | 226477 | 181541 |
| pPb | single | 0 | 69 | 7806 | 747826 | 600000 |
| pPb | dimuon-leg | 0 | 19 | 1222 | 117168 | 94306 |

Equivalent closest-track fractions:

| System | Raw muon source | near-origin closest / muons | `DeltaR < 1e-3` / muons | unique excluded tracks / muons |
| --- | --- | ---: | ---: | ---: |
| pp | single | **96.40%** | **76.97%** | **0.99999** |
| pp | dimuon-leg | **94.39%** | **75.66%** | **0.94241** |
| pPb | single | **93.50%** | **75.01%** | **0.99999** |
| pPb | dimuon-leg | **92.11%** | **74.13%** | **0.93003** |

## What the plots show

### 1. The near-origin feature is overwhelmingly a closest-track effect

This is the strongest conclusion of the study.

For every system and both muon definitions:

- the near-origin population in the **all-pairs** panel is almost identical to the **closest-only** panel,
- after excluding closest tracks, the near-origin occupancy collapses to `12-143` entries out of tens to hundreds of millions of pairs.

So the rectangle/core is **not** a general background of many track pairs. It is carried almost entirely by the single nearest track assigned to each muon.

### 2. The raw track is a near-copy of the muon, but not an exact duplicate

In all four 1M-event studies:

- `closest_exact_coordinate_match = 0`,
- but `92-96%` of muons have their closest raw track inside `|DeltaEta|, |DeltaPhi| < 1e-3`.

This is the same qualitative answer as in the earlier pPb-only note: the muon effectively reappears in the reco-track list as a **very nearby** track, not as a literal branch-level `(eta,phi)` clone.

### 3. pp vs pPb: same qualitative mechanism, slightly stronger in pp

Comparing the all-pairs near-origin fractions:

- pp single: `0.81795%`
- pPb single: `0.55102%`
- pp dimuon-leg: `0.77574%`
- pPb dimuon-leg: `0.49149%`

So the effect is visibly stronger in **pp** than **pPb**, but the morphology and the closest-track localization are the same in both systems. The system dependence is therefore secondary; it changes the strength, not the mechanism.

### 4. Single-muon vs dimuon-leg: the central box survives in both, but the extra arms weaken in the dimuon-leg view

The **single-muon** plots show the strongest full near-origin structure: a dense central box together with broader axis-aligned arms/cross-like leakage, especially in the closest-only panel.

The **dimuon-leg** plots still show the same dense central box, but the broader structure is thinner and more dominated by the `DeltaEta ~ 0` vertical band; the horizontal arm is much weaker.

So the central box is **not specific to the `NGlb` source**; it survives when the muons are taken only from the dimuon channel. But the fuller rectangle/cross pattern is more pronounced in the raw single-muon representation.

### 5. Interpreting the “rectangle feature”

The raw evidence supports the following conservative picture:

1. the reconstructed charged-track collection contains a track corresponding to the reconstructed muon,
2. its stored coordinates are extremely close to the muon coordinates, but not exactly equal,
3. that nearest-track population makes the dense central box,
4. the apparent rectangle/cross around it is an axis-aligned residual structure of that same nearest-track component, not a broad combinatorial background.

The study does **not** identify an exact upstream CMSSW/forest-production algorithm for the axis-aligned shape, so I will not over-claim beyond the raw evidence. But the feature is clearly tied much more strongly to:

- **closest-track component** than to the rest,
- **muon representation** (single vs dimuon-leg) more than to the collision system,
- and only weakly/moderately to **pp vs pPb**.

## What can be done within the scope of this analysis

### Rejecting or isolating the contribution

Within the scope of this analysis, the practical handle remains the **muon-track `DeltaR` rejection**. This study does **not** motivate a new square cut in `(DeltaEta, DeltaPhi)`:

- the maintained rejection is circular in `DeltaR`,
- the raw feature is nearest-track dominated,
- excluding the nearest track already removes the core almost completely,
- exact coordinate duplication is absent.

So the clean analysis-level action is still to use a **circular muon-track veto**, not a new ad hoc rectangle veto.

A useful diagnostic split is:

- nominal circular rejection,
- tighter/looser circular variations,
- `IsMuTaggedFalse` only as a stress test / upper-bound comparison, not as the quoted nominal systematic.

### How to assign a systematic for muon-track `DeltaR` rejection

Based on this study, the systematic should be assigned from **variations of the same circular veto concept**, because that is what actually rejects the nearest-track contribution in the analysis.

A conservative practical recipe is:

1. keep the maintained nominal circular veto,
2. rerun with a physically reasonable tighter and looser `DeltaR` threshold around that nominal value,
3. take the bin-by-bin envelope (or maximum absolute deviation) relative to nominal as the quoted muon-track rejection systematic,
4. use the fully unrejected `IsMuTaggedFalse` case only as a qualitative stress test showing the scale of the effect if the veto failed entirely.

This raw-forest study argues **against** using a square `(DeltaEta, DeltaPhi)` variation as the main systematic model, because the underlying selection concept in the maintained workflow is radial, not rectangular.

## Direct answers to the reviewer questions

1. **Raw reco-muon vs reco-track with no matching / selection / weighting:** done for pp and pPb, for both raw muon definitions.
2. **Raw single-muon channel vs dimuon-channel muons:** done. The dimuon-channel interpretation used here is explicitly the stored raw dimuon legs from `DiEta1/2`, `DiPhi1/2`, `DiPT1/2`.
3. **All / closest / excluding-closest splits:** done for every case.
4. **Rectangle explanation:** the central feature is overwhelmingly the muon's own closest reco-track counterpart; the broader axis-aligned structure is still part of that nearest-track component and is stronger for the raw single-muon representation.
5. **Rejection / systematic guidance:** stay with circular `DeltaR` veto logic; assign the systematic from tighter/looser circular veto variations, not from a square cut.

## Copilot workflow Q&A

### Q1. How does Copilot CLI decide how many subagents/background tasks to start?
The fetched Copilot CLI docs mention subagent/background-task features such as `/fleet` and `/tasks`, but they do **not** specify an exact automatic algorithm for how many subagents are launched. In this repository's reviewer/analyzer workflow, the **reviewer** should choose whether to split work into one or multiple analyzers based on independence, ordering, and ease of review.

### Q2. Is usage accounted “intensity-based” or “prompt-by-prompt”?
The fetched docs explicitly say: **each submitted prompt reduces the monthly premium-request quota by one**. They do **not** provide a finer-grained token-accounting formula for subagents in the retrieved README/help text. So the documentation-backed answer is: **premium requests are described prompt-by-prompt**, while exact token formulas are not specified in the fetched docs.

### Q3. When should the user specify analyzer/subagent count explicitly?
A good workflow answer is: specify it when the work naturally splits into independent tracks, or when ordering/resource constraints matter. Otherwise, the reviewer can choose the division of labor.

## Instruction update

I did **not** update `.github/copilot-instructions.md`. This task produced a useful sandbox-local debug pattern, but not a clearly stable repository-wide convention that should be promoted into the standing instructions.
