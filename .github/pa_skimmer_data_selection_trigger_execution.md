# PA skimmer golden-JSON, event-selection, trigger-turn-on, and track-mode execution

## Final status

Completed.

The PA skimmer is now updated and validated for:

- explicit golden-JSON filtering,
- explicit skim-level `|vz| < 15` enforcement for data,
- mergeable trigger-turn-on numerator/denominator histograms versus leading-muon `pT`,
- and `nominal|loose|tight` track-selection modes with matching maintained-wrapper correction-file mapping.

Important limitation: the audited PA data forests do **not** contain the HF-coincidence skim branch (`phfCoincFilter3` or `phfCoincFilter2Th4`), so the reducer can only log that missing support. It cannot newly enforce the note-level HF coincidence requirement from this forest version.

## Files inspected

Code / config / documentation:

- `.github/AGENT_REQUIREMENTS.md`
- `.github/pa_skimmer_data_selection_trigger_plan.md`
- `.github/copilot-instructions.md`
- `~/OverleafZHadronInPPb/src/event_selection_trigger_centrality.tex`
- `CommonCode/include/Messenger.h`
- `CommonCode/source/Messenger.cpp`
- `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`
- `SampleGeneration/20250929_ReducedTreePA/TrackEfficiencyCorrector.h`
- `SampleGeneration/20250929_ReducedTreePA/local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/run_local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/Condor.sh`
- `SampleGeneration/20250929_ReducedTreePA/PrepareCondor.sh`
- `SampleGeneration/20250929_ReducedTreePA/makefile`
- `SampleGeneration/20240220_ForestReducer/ReduceForest.cpp`
- `MainAnalysis/20250925_pPbSkimValidation/skimValidation.C`

Live forest / JSON inputs:

- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_1.root`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_10.root`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_19.root`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_324.root`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_551.root`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_777.root`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_911.root`
- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_999.root`
- first-40-file scan over `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/0000/HiForestAOD_*.root`
- `/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt`
- `/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt`

## Forest audit: exact objects / branches checked

From `hiEvtAnalyzer/HiTree`:

- `run`
- `lumi`
- `vz`

From `skimanalysis/HltTree`:

- `pPAprimaryVertexFilter`
- `pBeamScrapingFilter`
- `pVertexFilterCutGplus`
- `pVertexFilterCutVtx1`
- `pVertexFilterCutdz1p0`

Confirmed **absent** in the audited PA forest version:

- `phfCoincFilter3`
- `phfCoincFilter2Th4`

From `hltanalysis/HltTree`:

- `HLT_PAL2Mu12_v1`
- `HLT_PAL3Mu12_v1`

Presence check:

- `hltMuTree/HLTMuTree` exists and is the muon source used by `MuTreeMessenger`

## Stage 1 audit conclusions

| Check | Conclusion | Evidence |
| --- | --- | --- |
| Golden JSON already applied upstream? | **No** | 40-file run/lumi scan found non-analysis run `285419` and out-of-JSON early-window lumis such as `(285517, 1577-1581)` |
| Orientation consistent with reviewer mapping? | **Yes for the observed local sample** | The maintained `Samples/PAData` content that was reachable in this environment stayed in the earlier run block (`285419`, `285505`, `285517`, `285530`, `285537`, `285538`), which matches the reviewer’s earlier-runs=`Pbp` convention once split by run window |
| HF coincidence branch present? | **No** | `skimanalysis/HltTree` lacked both `phfCoincFilter3` and `phfCoincFilter2Th4` |
| `vz` cut already applied upstream? | **No** | first-40-file scan found `141` events with `|vz| > 15`, up to `34.5358` |

Additional observed scan outputs:

- first-40-file scan totals:
  - `entries = 125319`
  - `min_run = 285419`
  - `max_run = 285517`
  - `abs_vz_gt_15 = 141`
  - `abs_vz_max = 34.5358`
- example `|vz| > 15` events from the raw forest:
  - `HiForestAOD_10.root`: `(285505, 296, vz = 15.7202)`
  - `HiForestAOD_106.root`: `(285517, 1062, vz = -16.2660)`

## Files modified

- `CommonCode/source/Messenger.cpp`
- `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`
- `SampleGeneration/20250929_ReducedTreePA/local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/run_local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/Condor.sh`
- `SampleGeneration/20250929_ReducedTreePA/PrepareCondor.sh`
- `SampleGeneration/20250929_ReducedTreePA/makefile`
- `.github/copilot-instructions.md`
- `.github/pa_skimmer_data_selection_trigger_execution.md`

## What changed

### 1. Golden JSON handling

Added explicit reducer-side `--GoldenJSON` support in `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`.

- The reducer now parses CMS JSON files and applies a `(run, lumi)` whitelist after the existing run-window cut.
- This is only activated when `--GoldenJSON` is provided, so MC and any diagnostic data runs without JSON remain unaffected.

Wrapper wiring:

- `local_skim.sh`
- `Condor.sh`
- `PrepareCondor.sh`

now pass the reviewer-correct JSONs:

- earlier runs `285479-285832` -> `/afs/.../Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt`
- later runs `285952-286496` -> `/afs/.../Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt`

The later run start was also corrected from `285852` to `285952`.

### 2. Data offline event selection

For data, the reducer now applies:

- run window
- optional golden JSON
- `MSkim.PVFilter`
- `MSkim.BeamScrapingFilter`
- explicit skim-level `|VZ| < 15` using the selected best vertex

HF coincidence:

- The current PA forest content does not expose `phfCoincFilter3` or `phfCoincFilter2Th4`.
- The reducer now logs this limitation explicitly instead of silently relying on a missing branch default.
- No fake substitute branch was introduced.

### 3. Trigger turn-on bookkeeping

Added mergeable leading-muon-`pT` histograms in `ReduceForest.cpp`.

Leading-muon definition:

- `max(muPt1, muPt2)` from the first accepted reconstructed dimuon candidate stored in the skim event

Binning:

- `120` bins from `0` to `120` GeV

Storage location in output ROOT:

- `HltTree/TriggerTurnOn/`

Objects written:

- PP denominators / numerators:
  - `LeadingMuPt_Denominator_PP`
  - `LeadingMuPt_Numerator_TriggerSuiteOR_PP`
  - `LeadingMuPt_Numerator_HLT_HIL2Mu12_PP`
  - `LeadingMuPt_Numerator_HLT_HIL3Mu12_PP`
  - `LeadingMuPt_Numerator_HLT_HIL3SingleMu12_PP`
- PA denominators / numerators:
  - `LeadingMuPt_Denominator_PA`
  - `LeadingMuPt_Numerator_TriggerSuiteOR_PA`
  - `LeadingMuPt_Numerator_HLT_PAL2Mu12_PA`
  - `LeadingMuPt_Numerator_HLT_PAL3Mu12_PA`

Fill policy:

- denominator: event passes offline event selection and has an accepted reco dimuon candidate
- numerator: same event also passes the relevant trigger path (or suite OR histogram)

No efficiency division is performed in the skimmer, so the histograms remain mergeable across files.

### 4. Track modes

Added readable CLI:

- `--TrackSelectionMode nominal`
- `--TrackSelectionMode loose`
- `--TrackSelectionMode tight`

Track-cut mapping in `ReduceForest.cpp`:

- `nominal` -> `PassZHadron2022Cut`
- `loose` -> `PassZHadron2022CutLoose`
- `tight` -> `PassZHadron2022CutTight`

Maintained wrapper correction-file mapping:

- `nominal` -> `SampleGeneration/Corrections/Tracking/Hijing_8TeV_dataBS.root`
- `loose` -> `SampleGeneration/Corrections/Tracking/Hijing_8TeV_MB_eff_v3_loose.root`
- `tight` -> `SampleGeneration/Corrections/Tracking/Hijing_8TeV_MB_eff_v3_tight.root`

Wrapper interface:

- `local_skim.sh` and `Condor.sh` accept the mode as an optional fifth positional argument and also honor `TRACK_SELECTION_MODE`
- `run_local_skim.sh` exports `TRACK_SELECTION_MODE` and forwards it
- `PrepareCondor.sh` forwards `TRACK_SELECTION_MODE` into the generated submit arguments

### 5. Shared trigger lookup fix

`CommonCode/source/Messenger.cpp` now includes:

- `HLT_PAL2Mu12_v*`
- `HLT_PAL3Mu12_v*`
- `HLT_HIL3SingleMu12_v*`

in `TriggerTreeMessenger::FillTriggerNames()`, so `CheckTriggerStartWith(...)` can correctly find the PA muon triggers and the pp compatibility path.

## Commands run

### Forest / JSON audit

Representative probes:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA
ls -ld Samples Samples/*
find -L Samples/PAData -maxdepth 2 -type f -name 'HiForestAOD_*.root' | head -n 5
```

```bash
python3  # uproot-based probes on HiForestAOD_{1,10,19,324,551,777,911,999}.root and first-40-file scans
```

```bash
python3  # compare raw (run,lumi) pairs to the two reviewer JSON files
```

### Build

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd CommonCode
make
cd /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA
make
```

Observed behavior:

- `make` in `SampleGeneration/20250929_ReducedTreePA` compiled `Execute` and then ran the built-in `TestRun` target automatically
- log scan over the resulting build/test output found no `error`, `undefined`, `cannot`, or `No such file` messages

### One-file data validation

Used the maintained local wrapper with non-default mode:

```bash
mkdir -p /tmp/kdeverea
cd /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA
TRACK_SELECTION_MODE=tight ./local_skim.sh \
  PAData8TeV \
  Samples/PAData/0000/HiForestAOD_10.root \
  /tmp/kdeverea/pa_skimmer_validation.root \
  Dummy
```

## Validation evidence

### Wrapper / log evidence

Validation log showed:

- `TrackSelectionMode = tight`
- later-window pass used:
  - `GoldenJSON = /afs/.../Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt`
- earlier-window pass used:
  - `GoldenJSON = /afs/.../Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt`
- reducer warning (expected and desired):
  - missing `phfCoincFilter3` / `phfCoincFilter2Th4` in the PA forest

### Output files produced

- `/tmp/kdeverea/pa_skimmer_validation_AP.root`
- `/tmp/kdeverea/pa_skimmer_validation_PA.root`

### Output structure

Both split outputs contain:

- main skim `Tree`
- `InfoTree`
- `HltTree/TriggerTurnOn/` with the new trigger numerator/denominator histograms

### Run-window behavior for the tested early-run file

For input `Samples/PAData/0000/HiForestAOD_10.root`:

- `_AP.root` (later-run split) had:
  - `entries = 0`
  - trigger histogram directory present
- `_PA.root` (earlier-run split) had:
  - `entries = 2281`
  - `run_minmax = 285505 .. 285505`
  - `bad_run_lumi_count_vs_early_json = 0`
  - `abs_vz_max = 14.9648`

This demonstrates:

- run-window filtering behaves as intended for the tested input,
- earlier-run output is JSON-clean,
- and the new skim-level `|vz| < 15` cut is active.

### Trigger-turn-on objects in the validated PA output

From `/tmp/kdeverea/pa_skimmer_validation_PA.root`:

- `LeadingMuPt_Denominator_PA` sum = `1.0`
- `LeadingMuPt_Numerator_TriggerSuiteOR_PA` sum = `1.0`
- `LeadingMuPt_Numerator_HLT_PAL2Mu12_PA` sum = `1.0`
- `LeadingMuPt_Numerator_HLT_PAL3Mu12_PA` sum = `1.0`

### Track-mode evidence

The validated run used `TRACK_SELECTION_MODE=tight`.

Saved skim evidence:

- at least one saved event carried selected tracks under tight mode:
  - `events_with_tracks = 1`
  - `max_tracks_in_event = 59`
  - first nonzero saved event had `59` tracks with representative `trackPt` values

## Event-selection conclusion for DATA

- Primary vertex filter:
  - available in forest as `pPAprimaryVertexFilter`
  - enforced in the reducer through `MSkim.PVFilter`
- Beam scraping:
  - available in forest as `pBeamScrapingFilter`
  - enforced in the reducer through `MSkim.BeamScrapingFilter`
- HF coincidence:
  - not available in the audited PA forest version (`phfCoincFilter3` / `phfCoincFilter2Th4` absent)
  - reducer now logs this explicitly; it cannot newly enforce the note-level HF coincidence requirement from this forest
- `|vz| < 15`:
  - not already enforced upstream
  - now enforced explicitly in `ReduceForest.cpp` using the selected best vertex

## Golden-JSON conclusion

Golden JSON was **not already applied upstream** in the raw PA forests used by this workflow.

It is now **implemented explicitly in the skimmer** through `--GoldenJSON` and wired into the maintained data wrappers.

## Blockers / ambiguities / unresolved points

- No access blocker remained: both `/eos` and the reviewer JSON paths on `/afs` were readable.
- The only substantive limitation is the missing HF-coincidence branch in the audited PA forest content. I did **not** guess a substitute or fake that requirement.
- The currently reachable maintained `Samples/PAData` content appeared to stay within the earlier run block only, so the one-file live validation directly exercised the earlier/Pbp split. The later/pPb path was still validated structurally through the wrapper call and produced the expected empty `_AP.root` for the tested early-run input.

## Final readiness statement

The PA skimmer is now ready for official data reskimming with respect to:

- golden-JSON correctness,
- explicit documented `|vz| < 15` enforcement,
- trigger-turn-on bookkeeping output,
- and loose/nominal/tight track-mode support.

The one remaining caveat is PA HF coincidence: because the audited forest version does not carry the required HF-coincidence skim branch, the reducer cannot newly enforce that note-level requirement from these inputs. That limitation is now surfaced explicitly instead of being silently hidden.
