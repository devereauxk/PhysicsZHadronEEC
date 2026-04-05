# Muon-track near-(0,0) rectangle follow-up (`20260216_temp`)

This follow-up starts from `muon-track-deltaR-investigation.md`, which already established that the temp-study `hDeltaRMuTrk` histogram is filled **before** the runtime `IsMuTagged` rejection and therefore does **not** diagnose the actual vetoed-track set.

The remaining question was upstream: where do the near-origin reco/data muon-track pairs themselves acquire the square/rectangular core?

## Scope and files inspected

- Temp sandbox:
  - `MainAnalysis/20260216_temp/run.sh`
  - `MainAnalysis/20260216_temp/plot-MuTrk.sh`
  - `MainAnalysis/20260216_temp/CorrelationAnalysis.cpp`
  - `MainAnalysis/20260216_temp/scan_muon_track_rectangle.cpp` (new sandbox-only helper)
- Official input dictionary:
  - `OfficialWeightDictionary.sh`
- Skimmer:
  - `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`
- Source forests:
  - `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/...`
  - `SampleGeneration/20250929_ReducedTreePA/Samples/PAMC/...`
  - `SampleGeneration/20250929_ReducedTreePA/Samples/APMC/...`

## Commands run

Environment / build:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd CommonCode && make -j2
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp && make -j2
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp && make ExecuteMuonTrackRectangleScan -j2
```

Refresh the temp-study V0.2 PDFs:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260216_temp
./run.sh 0 1 1 Data MuTrkV02_RectangleDebug
./plot-MuTrk.sh Data MuTrkV02_RectangleDebug 0 1 1
./run.sh 0 1 1 Reco MuTrkV02_RectangleDebug
./plot-MuTrk.sh Reco MuTrkV02_RectangleDebug 0 1 1
```

Skim-level direct scans (representative examples):

```bash
./ExecuteMuonTrackRectangleScan --Mode skim --TrackType reco --MuonType reco \
  --Input /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.2/PPbData_Reco.root \
  --Label 'pPb data skim reco/reco (200k events)' --MaxEvents 200000 \
  --Output plots/rectangle_debug/pPbData_skim_recoReco \
  > output/rectangle_debug/pPbData_skim_recoReco.log

./ExecuteMuonTrackRectangleScan --Mode skim --TrackType reco --MuonType reco \
  --Input /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.2/PbPMC_Reco.root \
  --Label 'PbP MC skim reco/reco (200k events)' --MaxEvents 200000 \
  --Output plots/rectangle_debug/PbPMC_skim_recoReco \
  > output/rectangle_debug/PbPMC_skim_recoReco.log

./ExecuteMuonTrackRectangleScan --Mode skim --TrackType reco --MuonType gen \
  --Input /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.2/PPbMC_Reco.root \
  --Label 'pPb MC skim reco/gen (200k events)' --MaxEvents 200000 \
  --Output plots/rectangle_debug/pPbMC_skim_recoGenMu \
  > output/rectangle_debug/pPbMC_skim_recoGenMu.log
```

Forest-level scans:

```bash
PADATA=$(find -L /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAData -type f | sort | head -n 50 | paste -sd, -)
PAMC=$(find -L /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAMC -type f | sort | head -n 5 | paste -sd, -)
APMC=$(find -L /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/APMC -type f | sort | head -n 5 | paste -sd, -)

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --Input "$PADATA" --Label 'PA data forest reco/reco (1M events, first 50 files)' \
  --MaxEvents 1000000 --Output plots/rectangle_debug/PAData_forest_recoReco_1M \
  > output/rectangle_debug/PAData_forest_recoReco_1M.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType reco \
  --Input "$PAMC" --Label 'pPb MC forest reco/reco (50k events, first 5 files)' \
  --MaxEvents 50000 --Output plots/rectangle_debug/PAMC_forest_recoReco \
  > output/rectangle_debug/PAMC_forest_recoReco.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType reco --MuonType gen \
  --Input "$PAMC" --Label 'pPb MC forest reco/gen (50k events, first 5 files)' \
  --MaxEvents 50000 --Output plots/rectangle_debug/PAMC_forest_recoGenMu \
  > output/rectangle_debug/PAMC_forest_recoGenMu.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType gen --MuonType reco \
  --Input "$PAMC" --Label 'pPb MC forest gen/reco (50k events, first 5 files)' \
  --MaxEvents 50000 --Output plots/rectangle_debug/PAMC_forest_genRecoMu \
  > output/rectangle_debug/PAMC_forest_genRecoMu.log

./ExecuteMuonTrackRectangleScan --Mode forest --TrackType gen --MuonType gen \
  --Input "$PAMC" --Label 'pPb MC forest gen/gen (50k events, first 5 files)' \
  --MaxEvents 50000 --Output plots/rectangle_debug/PAMC_forest_genGenMu \
  > output/rectangle_debug/PAMC_forest_genGenMu.log
```

Analogous `APMC_*` scans were run for the PbP reco MC forests.

## Plots produced

Refreshed temp-study PDFs:

- `plots/pPbData_ZPT5_500_MuTrkV02_RectangleDebug-Delta2D-muTrk.pdf`
- `plots/pPbMC_Reco_ZPT5_500_MuTrkV02_RectangleDebug-Delta2D-muTrk.pdf`
- `plots/PbPData_ZPT5_500_MuTrkV02_RectangleDebug-Delta2D-muTrk.pdf`
- `plots/PbPMC_Reco_ZPT5_500_MuTrkV02_RectangleDebug-Delta2D-muTrk.pdf`

Ad hoc direct-inspection outputs live under:

- `plots/rectangle_debug/*.pdf`
- `plots/rectangle_debug/*.root`
- `output/rectangle_debug/*.log`

Each `rectangle_debug` PDF is a 3-panel `(all, tagged, untagged)` nearest-muon `(\Delta\eta,\Delta\phi)` plot for the requested object pairing.

## Confirmed findings

### 1. The rectangle is already present in the V0.2 skim content, independent of the temp plotter

For 200k-event skim scans:

| Case | central box `|\Delta\eta|,|\Delta\phi|<0.001` | tagged in box | untagged in box | tagged in box but outside `\Delta R<0.001` |
| --- | ---: | ---: | ---: | ---: |
| pPb data skim reco/reco | 952 | 952 | 0 | 197 |
| PbP data skim reco/reco | 961 | 961 | 0 | 217 |
| pPb MC skim reco/reco | 182169 | 182169 | 0 | 37395 |
| PbP MC skim reco/reco | 183241 | 183241 | 0 | 37493 |

So the square is **not** created by the sandbox plotter: it is already in the stored skim branches.

Also, the square core is not a tagged/untagged mixture. In every direct skim scan above, the central box is entirely `trackMuTagged=true`.

### 2. The maintained skimmer still applies a circular veto, not a square one

`SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp` still computes:

```cpp
DeltaEta = TrackEta - MuEta;
DeltaPhi = DeltaPhi(TrackPhi, MuPhi);
DeltaR   = sqrt(DeltaEta * DeltaEta + DeltaPhi * DeltaPhi);
trackMuTagged = (DeltaR < MuonVeto);
trackMuDR = min(DeltaRMu1, DeltaRMu2);
```

with `MuonVeto = 0.0025`.

So the square is not explained by a maintained square cut in repository code.

### 3. The earliest confirmed stage is the source reco forest / mu-tree content

The same structure is visible before skimming:

| Source forest case | tagged in central box | tagged in central box but outside `\Delta R<0.001` |
| --- | ---: | ---: |
| PA data forest reco/reco (1M events, first 50 files) | 235 | 52 |
| pPb MC forest reco/reco (50k events, first 5 files) | 24348 | 5035 |
| PbP MC forest reco/reco (50k events, first 5 files) | 34479 | 7062 |

This is the earliest stage I confirmed directly. The skimmer preserves the effect; it does not create it.

### 4. The object-choice dependence is strongly diagnostic

For the reco MC source forests:

| pPb source forest pairing | tagged in veto circle | tagged in box but outside `\Delta R<0.001` |
| --- | ---: | ---: |
| reco track vs reco muon | 24812 | 5035 |
| reco track vs gen muon | 21244 | 2750 |
| gen track vs reco muon | 25159 | 27 |
| gen track vs gen muon | 21582 | 0 |

| PbP source forest pairing | tagged in veto circle | tagged in box but outside `\Delta R<0.001` |
| --- | ---: | ---: |
| reco track vs reco muon | 35139 | 7062 |
| reco track vs gen muon | 30031 | 3820 |
| gen track vs reco muon | 35599 | 57 |
| gen track vs gen muon | 30461 | 0 |

Interpretation:

- the **full square-corner population** is strongest when using **reco tracks**
- it is reduced, but still visible, for **reco track vs gen muon**
- it essentially disappears for **gen track vs reco muon**
- it disappears entirely for **gen track vs gen muon**

So the rectangular core is primarily tied to the **reconstructed track object choice**, with additional smearing/modulation from the reco-muon coordinates.

## What is ruled out

1. **Runtime `IsMuTagged`** creating the feature  
   Already ruled out in the earlier note and not revisited here except through the upstream localization.

2. **The temp sandbox plotter** creating the feature  
   False; direct skim scans reproduce it without using the temp-study histogram.

3. **A maintained square cut in `ReduceForest.cpp`**  
   False; the skimmer still tags with a circular `\Delta R < 0.0025`.

4. **A generic tagged/untagged overlap near zero**  
   False in every direct scan above; the central box is entirely tagged.

## Strongest current root-cause hypothesis

The rectangle is an **upstream reco-object effect**, not a maintained repository bug in the temp plotter or the skimmer.

Most likely:

1. the reco charged-track collection contains the muons' own reconstructed tracker tracks (or very closely matched reco-track surrogates),
2. the reco muon coordinates stored in `hltMuTree/HLTMuTree` are not exactly the same object definition as the charged-track coordinates,
3. that reco-vs-reco object comparison generates an approximately axis-aligned `(\Delta\eta,\Delta\phi)` core at the `10^{-3}` level,
4. the skimmer then applies its usual circular `\Delta R` veto to that already-rectangular population.

The object-choice scans support this strongly: replacing the reco track with the gen track removes the square-corner occupancy almost completely, while pure gen/gen removes it entirely.

## Earliest confirmed stage

- **Data:** `Samples/PAData/...` source forests (`reco track` vs `reco muon`)
- **Reco MC pPb:** `Samples/PAMC/...` source forests
- **Reco MC PbP:** `Samples/APMC/...` source forests

Therefore the earliest confirmed stage is the **source forest reco content**, not the skim or the temp-study histogram.

## Remaining uncertainty / next steps

I did **not** identify a concrete maintained repository line that creates the square. The remaining uncertainty is upstream of this repo's skimmer logic and is likely in the reco object definitions feeding `ppTrack/trackTree` and `hltMuTree/HLTMuTree`.

Most useful next steps if this needs to be pushed further:

1. dump a handful of matched central reco-track / reco-muon tuples from the source forest, including track quality variables and any muon-track quality fields available in `MuTree`,
2. inspect the CMSSW / forest producer that builds `hltMuTree/HLTMuTree` to see whether the dimuon legs are matched to tracker tracks with explicit `\Delta\eta/\Delta\phi` windows,
3. if the systematic study only needs a faithful veto diagnostic, use the new helper or a tagged-only skim diagnostic rather than the raw temp `hDeltaRMuTrk` plot.
