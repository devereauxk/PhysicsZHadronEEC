# Analyzer Plan: PA skimmer golden-JSON, event-selection, trigger-turn-on, and track-mode update

## Reviewer role and scope

Use the current working tree in `/home/kdeverea/PhysicsZHadronEEC` as the source of truth.

This is an analyzer implementation task. Execute it sequentially unless a step explicitly allows parallel work, and return a completion summary in:

- `.github/pa_skimmer_data_selection_trigger_execution.md`

Do not start by editing code blindly. First audit the existing forest content and the current skimmer behavior so we can distinguish:

- what is already applied upstream in the forests,
- what is currently applied only in `ReduceForest.cpp`,
- and what is still missing and must be implemented in the skimmer.

If access to the forest files on `/eos` or the golden JSONs on `/afs` is blocked, stop and document the exact blocker. Do not work around missing access by guessing.

---

## Ground truth from the reviewer request

### 1. Golden JSON orientation convention

Ignore the misleading beam-orientation words in the golden-JSON filenames. For this task, treat the physical orientation mapping as:

- **Pbp** = earlier runs `285479-285832`
- **pPb** = later runs `285952-286496`

Golden JSON paths:

- Pbp / earlier runs:
  - `/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt`
- pPb / later runs:
  - `/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt`

### 2. Offline-event-selection scope

For this task, focus on **DATA** only for the event-selection audit.

You must compare the note text in:

- `~/OverleafZHadronInPPb/src/event_selection_trigger_centrality.tex`

against what is actually enforced when data are skimmed.

### 3. Trigger-turn-on request

For **DATA**, store trigger-turn-on numerator and denominator histograms versus **leading muon** `pT` for:

- pPb trigger suite (`HLT_PAL2Mu12`, `HLT_PAL3Mu12`)
- pp trigger suite (`HLT_HIL2Mu12`, `HLT_HIL3Mu12`, and the already-supported `HLT_HIL3SingleMu12` compatibility path if relevant)

Do **not** divide the histograms in the skimmer; they must remain mergeable across files.

### 4. Track-mode request

`ReduceForest.cpp` currently hardcodes:

- `PassZHadron2022Cut`

which corresponds to the **nominal** track selection.

The analyzer must add skimmer modes for:

- `nominal`
- `loose`
- `tight`

and wire the selected mode to the matching cut function and track-efficiency correction file.

---

## Current code facts you should use

### Existing skimmer code and wrappers

Primary files:

- `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`
- `SampleGeneration/20250929_ReducedTreePA/local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/run_local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/Condor.sh`
- `SampleGeneration/20250929_ReducedTreePA/makefile`

### Existing data-event selection already in `ReduceForest.cpp`

The current data skimmer already applies:

- run-range filtering via `RunStart` / `RunEnd`
- `MSkim.PVFilter`
- `MSkim.BeamScrapingFilter`
- `MSkim.HFCoincidenceFilter` for PA data
- HLT gating:
  - pp: `HLT_HIL2Mu12`, `HLT_HIL3Mu12`, `HLT_HIL3SingleMu12`
  - PA: `HLT_PAL2Mu12`, `HLT_PAL3Mu12`

The current code does **not** show:

- an explicit golden-JSON `(run, lumi)` whitelist,
- or an explicit `|vz| < 15` cut in the skimmer logic.

Do not assume the missing code means the forests did not already apply those selections. Verify first.

### Prior art for track-selection switching

Reuse the existing pattern from:

- `SampleGeneration/20240220_ForestReducer/ReduceForest.cpp`

which already supports:

- `DoAlternateTrackSelection`
- `AlternateTrackSelection`
- `PassZHadron2022Cut`
- `PassZHadron2022CutLoose`
- `PassZHadron2022CutTight`

Prefer adapting that pattern rather than inventing a completely different mode system.

### Likely tracking-correction files to audit

Under:

- `SampleGeneration/Corrections/Tracking/`

the likely relevant files are:

- `Hijing_8TeV_dataBS.root` (current nominal path in wrappers)
- `Hijing_8TeV_MB_eff_v3_loose.root`
- `Hijing_8TeV_MB_eff_v3_tight.root`

Confirm the intended loose/tight files before hardcoding them. If the correct loose/tight mapping is ambiguous, stop and document the ambiguity rather than guessing.

---

## Execution plan

### Stage 1. Audit the forest content before changing the skimmer

Inspect one or more real data forests under:

- `SampleGeneration/20250929_ReducedTreePA/Samples/PAData/`

and determine:

1. Whether the forest content already reflects golden-JSON filtering on `(run, lumi)`.
2. Whether the surviving runs/lumis align with the correct physical beam orientation under the reviewer’s convention:
   - earlier runs = Pbp
   - later runs = pPb
3. Whether the required event-selection ingredients are present in the forests:
   - primary-vertex filter support
   - beam-scraping filter support
   - HF coincidence filter support
   - vertex `vz`
4. Whether `|vz| < 15` is already enforced upstream at forest level.

Minimum deliverable for this stage in your summary:

- exact files inspected,
- exact branches/objects checked,
- conclusion table for:
  - golden JSON already applied? yes/no
  - orientation consistent? yes/no
  - HF coincidence branch present? yes/no
  - `vz` cut already applied? yes/no

If the HF coincidence filter needed by the note is not present in the forest content, do **not** silently fake it. Report that clearly in the summary.

### Stage 2. Implement golden-JSON handling only if it is not already applied upstream

If Stage 1 shows that the forests do **not** already enforce the correct golden JSON:

1. Add explicit `(run, lumi)` filtering in `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`.
2. Prefer an explicit input-driven mechanism such as a `--GoldenJSON` argument or equivalent, rather than hardcoding one specific certification file inside the binary.
3. Wire the data wrapper scripts so the correct golden JSON is passed for the correct run window:
   - `local_skim.sh`
   - `Condor.sh`
   - any other maintained data-entry scripts you touch

Requirements:

- the earlier-run output must be filtered with the earlier-run golden JSON,
- the later-run output must be filtered with the later-run golden JSON,
- the reviewer’s physical orientation convention must be documented clearly in comments and in your summary.

If Stage 1 shows the forests already apply the correct golden JSON, do **not** reimplement it in a way that double-filters the data. Instead document that conclusion and move on.

### Stage 3. Audit and complete the offline event selection for DATA

Compare the note text in `src/event_selection_trigger_centrality.tex` against the data-skimming logic in `ReduceForest.cpp`.

Verify each note requirement:

1. primary vertex filter
2. beam-scraping filter
3. HF coincidence filter for PA
4. `|vz| < 15`

Required actions:

- if a requirement is already applied in the forests or in the skimmer, document where,
- if `vz` is applied nowhere, implement it in `ReduceForest.cpp` at skim level,
- if HF coincidence support is missing from the actual forest content, report that explicitly in the summary instead of pretending the note statement is already satisfied.

Do not extend this stage to MC. Keep the event-selection audit focused on DATA as requested.

### Stage 4. Add trigger-turn-on numerator/denominator outputs for DATA

In `ReduceForest.cpp`, add output objects for trigger-efficiency bookkeeping versus **leading muon `pT`**.

Requirements:

1. Determine the leading muon `pT` from the reconstructed muons used by the data selection.
2. Build:
   - denominator = events that pass event selection with the relevant leading-muon `pT`
   - numerator = events that pass event selection **and** trigger selection with that leading-muon `pT`
3. Do **not** divide the histograms in the skimmer.
4. Store the outputs in the skim file under a clearly named object path/container associated with `HltTree`, preserving discoverability for later merge and post-processing.

At minimum, cover:

- pp data trigger suite
- PA data trigger suite

If the split PA outputs naturally represent the two beam orientations separately, keep the histograms in each split output so later merging can build the orientation-specific turn-on curves cleanly.

Document:

- histogram names,
- binning,
- where they are written in the ROOT output,
- and how they should be merged later.

### Stage 5. Add loose / nominal / tight track-selection modes

Implement configurable skimmer modes for:

- `nominal`
- `loose`
- `tight`

Requirements:

1. Reuse the prior-art selection pattern from `SampleGeneration/20240220_ForestReducer/ReduceForest.cpp`.
2. The selected mode must control:
   - which `PassZHadron2022Cut*` function is used
   - which tracking-correction file is used
3. Update the maintained wrappers to expose the mode cleanly for local and Condor execution.

Reviewer preference:

- use a readable CLI such as `--TrackSelectionMode nominal|loose|tight` if practical,
- and keep backward-compatible nominal behavior when no mode is provided.

If you instead reuse the older numeric `AlternateTrackSelection` pattern directly, document the mapping very clearly in the summary and in any touched scripts.

### Stage 6. Rebuild and validate on one DATA file

After making the changes:

1. Build the required code using the repository’s standard commands:
   - `source SetupAnalysis.sh`
   - build `CommonCode` first if needed
   - build `SampleGeneration/20250929_ReducedTreePA`
2. Run the skimmer on **one** real DATA forest file.

Use either:

- a direct `./Execute ...` call, or
- the maintained local wrapper path (`local_skim.sh` / `run_local_skim.sh`)

as long as the command is fully documented.

Validation requirements for the one-file run:

- skim completes successfully,
- output file contains the expected main skim tree,
- output file contains the new trigger numerator/denominator objects,
- run/lumi filtering behaves as intended for the tested run window,
- event-selection behavior matches the implemented policy,
- chosen track-selection mode is reflected in the output and/or logs.

### Stage 7. Update `.github/copilot-instructions.md`

After the implementation is validated, update `.github/copilot-instructions.md` with the new stable repo knowledge learned from this task.

Only add facts that were actually verified during this task, for example:

- whether PA forests already carry the golden JSON or whether the skimmer now applies it,
- the validated physical mapping between run windows and beam orientation in the PA skimming workflow,
- where the `vz` cut is enforced,
- how trigger numerator/denominator histograms are stored in the skim output,
- how the loose/nominal/tight track-mode selection is exposed in the maintained wrappers.

Do not add speculative documentation.

---

## Files you are expected to inspect and likely modify

Inspect at minimum:

- `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`
- `SampleGeneration/20250929_ReducedTreePA/local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/run_local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/Condor.sh`
- `SampleGeneration/20250929_ReducedTreePA/makefile`
- `SampleGeneration/20240220_ForestReducer/ReduceForest.cpp`
- `CommonCode/include/Messenger.h`
- `CommonCode/source/Messenger.cpp`
- `~/OverleafZHadronInPPb/src/event_selection_trigger_centrality.tex`
- `.github/copilot-instructions.md`

If you need to touch additional files, document why.

---

## Validation and summary requirements

Your completion summary in `.github/pa_skimmer_data_selection_trigger_execution.md` must include:

1. The exact files inspected in the forest audit.
2. The exact code/files modified.
3. The exact commands run for build and one-file validation.
4. A golden-JSON conclusion:
   - already applied upstream or newly implemented in the skimmer.
5. An event-selection conclusion for DATA:
   - PV filter
   - beam scraping
   - HF coincidence
   - `vz` cut
6. The trigger-turn-on output design:
   - leading-muon definition
   - histogram names
   - storage location in the skim ROOT output
7. The track-mode design:
   - CLI interface
   - nominal/loose/tight mapping
   - correction-file mapping
8. Validation evidence from the one-file run.
9. Any blockers, ambiguities, or unresolved questions.

Final status should state clearly whether the PA skimmer is now ready for official data reskimming for:

- golden-JSON correctness,
- documented offline event selection,
- trigger-turn-on bookkeeping,
- and loose/nominal/tight track-mode support.
