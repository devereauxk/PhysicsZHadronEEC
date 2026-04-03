# Muon-track \DeltaR follow-up (`20260216_temp`)

## What I changed

- `MainAnalysis/20260216_temp/run.sh` now defaults the temp-study runner to `IsMuTagged=false` via `ISMUTAGGED=${ISMUTAGGED:-false}` and passes that explicitly to `system-analysis.sh`.
- Removed the dead local `isPPbEvent` helper from `MainAnalysis/20260216_temp/CorrelationAnalysis.cpp`.
- Removed the deprecated active `isPPbEvent` orientation-matching logic from `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp` so mixed-event selection no longer depends on inferred PA orientation.

## Confirmed findings

### 1. The temp `hDeltaRMuTrk` plot does **not** use the mu-tag rejection switch
In `MainAnalysis/20260216_temp/CorrelationAnalysis.cpp:315-333`, the `hDeltaRMuTrk` fill block only applies track `p_{T}` / `\eta` cuts and then fills raw `(trackEta - muEta, trackPhi - muPhi)` for both muons.

It does **not** call `trackSelection(...)`, and it does **not** check `trackMuTagged`.

So toggling `--IsMuTagged` cannot change this histogram.

### 2. Actual validation: `IsMuTagged=false` and `true` give identical `hDeltaRMuTrk`
I rebuilt `MainAnalysis/20260216_temp/` and ran the executable directly on pPb reco MC and pPb reco data with the same temp-study settings except for `--IsMuTagged`.

Validation outputs:

- `output/deltaR_followup/pPbMCReco_IsMuTaggedFalse.root`
- `output/deltaR_followup/pPbMCReco_IsMuTaggedTrue.root`
- `output/deltaR_followup/pPbData_IsMuTaggedFalse.root`
- `output/deltaR_followup/pPbData_IsMuTaggedTrue.root`

Histogram comparison results:

- **pPb reco MC**
  - `parIsMuTagged`: `0 / 1`
  - `hDeltaRMuTrk` entries: `703410 / 703410`
  - `hDeltaRMuTrk` integral: `11223 / 11223`
  - bin-by-bin differences: `0`
  - `hTrkPtEtaPhi` integral: `420763 / 406328`
- **pPb data**
  - `parIsMuTagged`: `0 / 1`
  - `hDeltaRMuTrk` entries: `25624 / 25624`
  - `hDeltaRMuTrk` integral: `352 / 352`
  - bin-by-bin differences: `0`
  - `hTrkPtEtaPhi` integral: `15343.9 / 14888.1`

So the mu-tag switch is active in the normal selected-track path (`hTrkPtEtaPhi` changes), but the temp muon-track diagnostic plot is unchanged because it bypasses that selection.

### 3. The square plateau is not created by the runtime `IsMuTagged` toggle
I scanned the pPb V0.2 reco inputs directly (`PPbMC_Reco.root` and `PPbData_Reco.root`) over the first 20k events.

Observed in that scan:

- **pPb reco MC**
  - muon-track pairs inside `|\Delta\eta|<=0.001` and `|\Delta\phi|<=0.001`: `18035`
  - untagged pairs inside that square: `0`
  - selected tracks with valid `trackMuDR`: `564069`
  - tagged tracks: `18382`
  - tagged tracks with `trackMuDR <= sqrt(2) * 0.001`: `18364`
  - rare tagged tracks outside that square-diagonal region: `18`
  - maximum tagged `trackMuDR` seen: `0.00245222`
- **pPb data**
  - muon-track pairs inside `|\Delta\eta|<=0.001` and `|\Delta\phi|<=0.001`: `104`
  - untagged pairs inside that square: `0`
  - selected tracks with valid `trackMuDR`: `3811`
  - tagged tracks: `106`
  - tagged tracks with `trackMuDR <= sqrt(2) * 0.001`: `106`
  - maximum tagged `trackMuDR` seen in this sample: `0.00133163`

## Strong evidence / likely cause

The square-like plateau is a property of the **reco/data branch content being plotted** in the temp diagnostic, not a consequence of the runtime `IsMuTagged` toggle.

The evidence is:

1. the plot is filled before any mu-tag rejection is applied;
2. the plot is exactly identical for `IsMuTagged=false` and `true`;
3. every scanned pair inside the apparent central square was already associated with `trackMuTagged=true`;
4. the skim-side definition in `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp` is still circular (`DeltaR < 0.0025`), and the reco MC scan still shows a few tagged tracks extending beyond the square core up to `trackMuDR \approx 0.00245`.

So the misleading part is the visualization: `hDeltaRMuTrk` is showing the raw reco/data near-muon population, while the maintained analysis rejects tracks using the precomputed `trackMuTagged` flag.

## Remaining uncertainty / recommended next follow-up

I did **not** isolate the upstream origin of why the reco/data near-muon population is so concentrated inside a roughly square `|\Delta\eta|,|\Delta\phi| \lesssim 0.001` core.

What is established is that this shape is **not** introduced by the temp analysis toggle and **not** by replacing the skim's circular veto with a square cut at runtime.

Recommended next follow-up:

1. in a dedicated debug study, dump the nearest-muon `(\Delta\eta, \Delta\phi, trackMuDR, trackMuTagged)` tuples for tagged reco/data tracks directly from the skim;
2. compare those tuples to the upstream pre-skim reco track and muon coordinates;
3. if the systematic itself still looks too large, make a dedicated tagged-vs-untagged diagnostic plot rather than using the current raw `hDeltaRMuTrk` plot, which is not a visualization of the actual vetoed set.
