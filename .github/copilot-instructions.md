# Copilot Instructions for PhysicsZHadronEEC

## Mandatory session bootstrap

At the start of **every Copilot interaction in this repository**, first read and apply:

`/home/kdeverea/PhysicsZHadronEEC/.github/AGENT_REQUIREMENTS.md`

Operationally, treat this as the first-step checklist before any exploration, edits, installs, or git operations:

- Confirm all actions stay within `/home/kdeverea`.
- Do not use `sudo`.
- Ask for permission before any installation.
- Prefer preinstalled dependencies; if unavailable, install only under `/home/kdeverea` (never system-wide) and only after permission is granted.
- Ask for permission before any commit or push.
- Do not modify repositories not owned by `devereauxk`.
- Do not share project data with external systems.
- Do not use repository data/files for training.
- Do not interfere with other users' processes.
- Do not merged files that are not on the /home/ file system. In particular, do not merge or hadd root files on the /eos/ system.

## Reviewer / Analyzer role model (mandatory when prompted)

When initialized with wording like **"you are a reviewer..."** or **"you are an analyzer..."**, apply this role contract strictly.

### Reviewer role
- The reviewer communicates directly with the user.
- Reviewer scope is limited to:
  - code review,
  - impact analysis,
  - task planning,
  - writing task instructions in `.md` files for analyzers.
- Reviewer should generally not run analyzer-assigned production work; instead delegate via task markdown and review the returned summary.

### Analyzer role
- Analyzers generally do not interact directly with the user.
- Analyzers execute tasks from reviewer-written `.md` files.
- Execute tasks sequentially unless the task file explicitly allows parallel tracks.
- Return a completion summary `.md` including:
  - commands run,
  - outputs produced,
  - validation checks,
  - failures/retries,
  - final status.

### Relationship and handoff
- Reviewer defines scope, acceptance criteria, ordering, and stop conditions.
- Analyzer executes and reports.
- Reviewer validates analyzer results and communicates final conclusions/actions to the user.

## Build, test, and lint commands

### Environment setup (required before most builds)
```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
```

`SetupAnalysis.sh` defines `ProjectBase` and prepends `CommonCode/binary` to `PATH`; many makefiles depend on `$(ProjectBase)`.

### Shared code build
```bash
cd CommonCode
make
```

### Build an analysis module
Use `make` inside the specific dated folder you are modifying (there is no single top-level build):
```bash
cd MainAnalysis/20260222_EnergyExtrapolation
make
```

### Run a single test / smoke run
Most modules expose a `TestRun` target in their local `makefile`:
```bash
cd MainAnalysis/20240223_Zhadron
make TestRun
```

For workflow-level quick checks, use the local workflow test scripts when present (examples):
```bash
cd MainAnalysis/20240223_Zhadron/workflow && ./testAnalysis.sh ...
cd MainAnalysis/20251211_ResidualCorrection/workflow && ./test.sh
```

### Lint
No repository-wide linter configuration was found (`.clang-tidy`, `clang-format` config, `ruff`, `flake8`, etc.). Keep style consistent with nearby files.

## High-level architecture

This repository is organized as **standalone, date-stamped analysis snapshots**. Major rewrites are usually done by creating a new dated directory rather than refactoring old ones in place.

- `CommonCode/`: shared C++ utilities and data interfaces (`Messenger`, `CommandLine`, correction helpers). Build this first so modules can link `CommonCode/library/Messenger.o`.
- `SampleGeneration/`: forest reduction and conversion steps that produce skimmed ROOT trees (for example, `ReduceForest.cpp` modules).
- `MainAnalysis/`: correlation and leading-track analyses (`CorrelationAnalysis.cpp` / `LeadingTrkAnalysis.cpp`) over reduced samples.
- `TrackingEfficiency/`: residual correction production and validation workflows.
- `Systematics/`: systematic-harvesting/combination utilities.
- `Plots/`: plotting/post-processing executables and ROOT macro-driven figure production.

Typical flow: **SampleGeneration -> MainAnalysis/TrackingEfficiency -> Systematics -> Plots**.

## Key conventions specific to this codebase

- **Directory granularity is workflow-level**: each dated folder is intentionally self-contained with its own `makefile`, scripts, and assumptions.
- **Use local makefiles, not a global build**: compile/run from the target dated directory.
- **Command-line interface pattern**: executables use `CommandLine` from `CommonCode/include/CommandLine.h` and expect long options (`--Input`, `--Output`, `--MinZPT`, etc.).
- **Chunked execution pattern**: many `workflow/finalAnalysis.sh` scripts fan out many `ExecuteCorrelationAnalysis` jobs via `--nThread/--nChunk`, then merge with `hadd`.
- **System-analysis tuning knobs (20241102)**: `system-analysis.sh` now supports `CONFIG_FILE` (alternate range config), `CUT_PARALLELISM` (run multiple `PT_RANGES` in parallel), and `SKIP_CLEAN=1` for iterative reruns without rebuild.
- **Threader tuning knobs (20241102)**: `threader.sh` supports `NTHREAD`, `NSLICE_FACTOR`, and `ANALYSIS_EXECUTABLE` for worker count, slice over-decomposition, and executable selection during benchmarking.
- **Measured speedup (pp MC Gen benchmark)**: for `ZPT40_350` with `trkPT={1_2,2_4,4_10}`, runtime improved from `224.39s` to `76.36s` (~`2.94x`, `66%` reduction) with histogram-identical outputs.
- **Minimal usage pattern**:
  - `export SKIP_CLEAN=1`
  - `export CUT_PARALLELISM=3` (parallelize track-pt cuts)
  - `export NTHREAD=8` (or your preferred worker count)
  - optional: `export NSLICE_FACTOR=1` and `export CONFIG_FILE=/path/to/custom_ranges.sh`
  - then run `./system-analysis.sh ...` as usual.
- **ROOT-first toolchain**: builds rely on `g++` + ``root-config --cflags --glibs`` and typically link `$(ProjectBase)/CommonCode/library/Messenger.o`.
- **Output layout convention**: scripts usually write intermediates to `output/` and merged products/plots to `plots/`, with suffixes like `-result.root` and `-nosub.root`.
- **20260403 private-generator convention**: `SampleGeneration/20260403_PythiaMadgraph/` is the local pp `Z -> mu mu` generator workflow for private studies. Source `env.sh` after `SetupAnalysis.sh`, keep private generator installs under `~/pythia`, use `generate_madgraph.sh --Energy {5020,8160}` for the hard process, `ExecuteShowerLHE` with `CP5.cmnd` for the shower/HepMC step, and `ExecuteConvertHepMC3` to produce a single-`Tree` `ZHadronMessenger` skim. The promoted 100k private validation skims are `SampleGeneration/20260403_PythiaMadgraph/output/ZMuMu_{5020,8160}_nnlo.root` (generated with `--PDFLabel lhapdf --LHAID 303600`, i.e. NNPDF31_nnlo_as_0118). The older `ZMuMu_{5020,8160}_validation.root` skims used inconsistent PDFs across energies and are superseded for note-facing work.
- **20260403 private-generator PDF convention**: `generate_madgraph.sh` accepts `--PDFLabel` (default `lhapdf`) and `--LHAID` (default `303600`) to control the MadGraph PDF. For consistent private MC, always use the same `--PDFLabel`/`--LHAID` pair for both 5.02 and 8.16 TeV samples. The `nn23lo1` built-in PDF ignores the `lhaid` parameter entirely and uses MadGraph's internal NNPDF23 LO set; do not mix `nn23lo1` at one energy with `lhapdf` at the other.
- **20260403 MG5 runtime convention**: the maintained local MG5 wrapper in `SampleGeneration/20260403_PythiaMadgraph/generate_madgraph.sh` should run with `MG5AMC_PYTHON=/usr/bin/python3.11`, because the default local `python3` is too old for the checked-in private MG5 setup.
- **20260404 pp MadGraph skim convention**: for official-vs-private pp validation, the maintained private `ConvertHepMC3` path keeps only leaf charged particles with `p_{T} > 0.5 GeV` and `|eta| < 2.4` in the stored track branches, computes `SignalHF`/`SubEvent*HF` separately from all leaf particles in `3 < |eta| < 5` with `p_{T} > 0.4 GeV`, and tags gen muons through recursive Z-ancestor lookup before filling `trackMuTagged`/`trackMuDR`. The harmonized private skim should therefore be regenerated from the retained HepMC when this threshold changes, so `trkPT=0.5_500` generator-shape and closure comparisons stay acceptance-matched to `OFFICIAL_MCGENINPUT_PP` (`MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-gen-v11-Zpt0.root`).
- **20260404 pp MadGraph closure convention**: the maintained pp validation entrypoints are `MainAnalysis/20241102_ZhadronVsZPt/pp-madgraphclosure.sh` for the gen-mode `nmix=10` closure run and `Plots/20260404_ppClosure/{plot-comparison.sh,plot-central.sh}` for the requested generator-shape and closure overlays. The private default input for this campaign is `SampleGeneration/20260403_PythiaMadgraph/output/ZMuMu_5020_validation.root`, the official gen-level comparison default should come from `OFFICIAL_MCGENINPUT_PP` (`MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-gen-v11-Zpt0.root`) rather than the reco-style `OFFICIAL_DATAINPUT_PP`, and the maintained generator-shape/closure paths should apply both `EventWeight` and `TrackWeight` for the official and private pp MC inputs.
- **20260407 pp private-MadGraph dictionary convention**: the maintained private pp MadGraph skim paths should be surfaced from `OfficialWeightDictionary.sh` as `OFFICIAL_PRIVATE_PP_5020_INPUT` / `OFFICIAL_PRIVATE_PP_8160_INPUT` and consumed through `PRIVATE_PP_5020_INPUT` / `PRIVATE_PP_8160_INPUT` in maintained closure, energy-extrapolation, and comparison entrypoints instead of hardcoding skim paths. The promoted 100k NNLO skims (`ZMuMu_{5020,8160}_nnlo.root`) are LO/unweighted and carry one constant positive event weight. The MC-driven energy-extrapolation weight derived from these skims is `20260417_EE_nnlo.root`.
- **20260407 pp private-production plot convention**: `Plots/20260404_ppClosure/plot_production.cpp` must treat `hZPtEtaPhi_*` in the `*-nosub.root` inputs as already normalized by `hNZData_*` in `makeProjection.C`; the maintained production overlays should divide only by bin width and must not apply a second `1/N_Z` normalization.
- **V0.2 heavy-ion input convention**: for the merged `/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.2/` skims, use the literal orientation labels directly: physical `pPb` uses `PPbData_Reco.root`, `PPbMC_Reco.root`, and `PPbMC_Gen.root`, while physical `PbP` uses `PbPData_Reco.root`, `PbPMC_Reco.root`, and `PbPMC_Gen.root`.
- **20241102 track-selection mode convention**: `ExecuteCorrelationAnalysis` accepts `--TrackSelectionMode={Nominal,Loose,Tight}` and maps those to the skim trees `Tree`, `TreeLoose`, and `TreeTight`.
- **20241102 tracking-selection runner convention**: `MainAnalysis/20241102_ZhadronVsZPt/systematics.sh` is the maintained runner for the official corrected-data tracking-selection variations within the broader systematic enumeration.
- **V0.1 heavy-ion tracking-selection input convention**: the tracking-selection systematic uses the direct-label skims in `SampleGeneration/20250929_ReducedTreePA/V0.1/`, so physical pPb reads `PPbData_Reco.root` and physical PbP reads `PbPData_Reco.root`.
- **V0.3 pp tracking-selection convention**: the promoted `SampleGeneration/20250929_ReducedTreePA/V0.3/PP{Data,MCReco,MCGen}.root` bundle now provides the pp `Tree`, `TreeLoose`, and `TreeTight` skims, so the maintained corrected-data pp systematic flow should produce `TrackSelectionMode={Loose,Tight}` outputs and consume dedicated pp loose/tight residual-correction files from `OfficialWeightDictionary.sh`.
- **Canonical 20241102 runner convention**: in `MainAnalysis/20241102_ZhadronVsZPt/`, the maintained official entrypoints are `closure-VZ.sh`, `closure-Z.sh`, `closure-trk.sh`, and `central.sh`. When a rerun needs fixes, fold them into these scripts directly rather than creating new persistent variants such as `*-newVZFix.sh`.
- **Official weight dictionary convention**: correction runner scripts must source `/home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh` and consume `VZWeightFile_{PP,PPb,PbP}`, `ZWeightFile_{PP,PPb,PbP}`, `RWeightFile_{PP,PPb,PbP}`, and `EEWeightFile_PP` from that dictionary, rather than hardcoding weight paths in each script.
- **pp private-EE dictionary convention**: the MC-driven pp energy-extrapolation systematic weight must be surfaced from `OfficialWeightDictionary.sh` as `OFFICIAL_EE_WEIGHT_FILE_PP_PRIVATE` / `EEWeightFile_PP_PRIVATE`, while the promoted nominal central-value input remains `OFFICIAL_EE_WEIGHT_FILE_PP` / `EEWeightFile_PP`.
- **Official input dictionary convention**: maintained official runners should also source `OfficialWeightDictionary.sh` for `OFFICIAL_{MCGEN,MCRECO,EPOS,DATA}INPUT_{PP,PPB,PBP}` instead of hardcoding skim paths. For the promoted `V0.3` bundle, physical pp uses `PP{Data,MCReco,MCGen}.root`, physical pPb uses `PPb{Data,MCReco,MCGen}.root`, and physical PbP uses `PbPData_Reco.root` / `PbPMC_{Reco,Gen}.root`; the EPOS helpers remain on the maintained merged EPOS paths.
- **Official analysis tag convention**: the promoted final-result tags are exported from `OfficialWeightDictionary.sh` as `OFFICIAL_TAG_PP` and `OFFICIAL_TAG_PPB`; `central.sh`, `systematics.sh`, and downstream runners should use those variables instead of hardcoding tag strings. For the current V0.3 campaign the promoted tags are `EEV5_ZV8_trkV26_nmix10` for pp-facing helpers and `ZV8_trkV26_nmix10` for pPb/PbP.
- **V0.3 muon-rejection residual convention**: the promoted corrected-data muon-rejection systematic surface now uses the dedicated `IsMuTaggedFalse` residual/corrected-data branches for pp, pPb, and PbP, while the nominal case remains the skim-default `--IsMuTagged true` (`trackMuDR = 0.0025`). The older `TrackMuDR0p004` family remains only as a dedicated study surface and should not be treated as the maintained note-facing muon-rejection variation.
- **20241102 systematics-runner convention**: `MainAnalysis/20241102_ZhadronVsZPt/systematics.sh` is the explicit enumerator for corrected-data systematic variations. For pp it now produces `Loose`, `Tight`, `IsMuTaggedFalse`, `IsPURejectTrue`, `MuVar0..3`, inclusive `TrackCorrection0p976/TrackCorrection1p024`, and `_EEPrivate`; for pPb/PbP it produces `Loose`, `Tight`, `IsMuTaggedFalse`, `IsPURejectTrue`, `MuVar0..3`, and inclusive `TrackCorrection0p976/TrackCorrection1p024`. The loose/tight and `IsMuTaggedFalse` branches must consume their dedicated residual-correction files from `OfficialWeightDictionary.sh`, the `IsPURejectTrue` branches must consume the dedicated PU-stack VZ, Z, and track residual corrections, and the maintained pp nominal (`central.sh`) plus variation (`systematics.sh`) entrypoints should still be reused directly with `CONFIG_FILE` for narrowed reruns. When `CONFIG_FILE` drives a narrowed custom-range block, that block must still include `TrackCorrection0p976/TrackCorrection1p024` for pp, pPb, and PbP; otherwise narrowed note-facing reruns can refresh the nominal outputs while leaving stale track-correction variations behind. For the pp PU family, the `NVertex == 1` requirement is a corrected-data selection only and should not be imposed on the pp MC legs of the VZ, Z-correction, or residual-correction derivations.
- **Maintained PU-selection convention**: in the maintained `CorrelationAnalysis.cpp` surfaces for the main analysis, Z correction, and residual correction, the PU veto must be `if (par.isPUReject && par.isData && b->NVertex != 1) return 0;`. This applies the single-vertex selection to corrected data for both pp and pPb/Pbp while keeping MC derivation legs untouched.
- **20241102 narrowed-test convention**: when `CONFIG_FILE` is supplied externally, `central.sh` and `systematics.sh` should honor that custom range file for quick validation and skip their built-in second range block to avoid duplicate reruns.
- **|vz| window convention**: the VZ reweighting production stage must run without the analysis `|vz| < 15` cut, but all subsequent maintained correction and analysis workflows should default to enforcing `|vz| < 15` unless a VZ-study runner explicitly disables it with `--UseVZWindow false`.
- **20250929 skimmer TnP convention**: in `SampleGeneration/20250929_ReducedTreePA/`, pp and PA MC keep `MZHadron.ZWeight = 1` and `ExtraZWeight = 1`, pp data nominal `ZWeight` must come from a dedicated local pp dimuon TnP helper (nominal pp TightID × TightID × double-trigger, not `CommonFunctions` Z-shape helpers), and PA data uses the local pPb dimuon TnP helper with `ExtraZWeight[0..3]` reserved for ID up/down and trigger up/down. The PA TnP helpers should only be applied when both muons stay inside the documented high-`p_{T}` validity window (`20 <= p_{T} < 200 GeV`, `|eta| < 2.4`); otherwise keep the nominal and variation weights at `1`.
- **20260329 systematic-harvesting convention**: `Systematics/20260329_pPbSystematics/` is the compiled workspace for note-facing Z-hadron systematics. It reads the final corrected `*_trkResidual_<official-tag>_ZPT*-result.root` outputs, applies the quoted `0.5` normalization consistently when harvesting and plotting note-facing central values/systematic magnitudes, keeps only `TrackSelection`, `TrackCorrection`, `MuonRejection`, `PUpp`, `PUpPb`, `ScaleFactor`, `EnergyExtrapolation`, and `Total`, uses the corrected-data `IsMuTaggedFalse` outputs for the muon-rejection (`MuonRejection`) term, applies the pileup family collision-specifically by default (`PUpp` for `pp`, `PUpPb` for `pPb` and `PbP`), and builds the tracking-correction term from the corrected-data `TrackCorrection0p976` / `TrackCorrection1p024` family bin-by-bin rather than a flat constant.
- **20260329 combined-systematics convention**: `Systematics/20260329_pPbSystematics/` should now keep the separate `pPb` and `PbP` outputs and also produce a combined `pPbPbp` output. The combined nominal and variation histograms must be formed from `hData_*`, `hMixData_*`, `hNZData_*`, and `hNZMixData_*` by undoing the per-system `N_Z` normalization, summing pPb and Pbp, renormalizing by the summed `N_Z`, projecting, and then applying the extra `0.5` normalization before taking bin-by-bin deviations. Operationally, the maintained combined runners (`run.sh`, `run-pu.sh`, `run-track-selection.sh`, and `run-track-correction.sh`) must therefore feed the helper the paired `*-nosub.root` inputs for `pPbPbp`; using `*-result.root` there will inflate the reconstructed combined central values and the resulting combined systematics. Note-facing combined labels should render the merged collision as `pPb`, not `pPb + Pbp`.
- **20260421 pPb-pp difference-systematics convention**: in `Systematics/20260329_pPbSystematics/`, keep `pPb` and `PbP` fully correlated inside the maintained `pPbPbp` combined builder, but treat the combined heavy-ion and pp sides as uncorrelated when building `Difference*` families. The maintained `Difference<Family>_*` path must therefore take the bin-by-bin combined heavy-ion family magnitude, the matching pp family magnitude, and combine them as `sqrt(delta_combined^2 + delta_pp^2)` with missing sides treated as zero; do not rebuild a varied `(pPbPbp - pp)` histogram and compare that to the nominal difference.
- **20260329 note-facing systematics overlay convention**: in `Systematics/20260329_pPbSystematics/run.sh`, `INCLUDE_FAMILIES` controls which families are calculated into the output ROOT file while `PLOT_FAMILIES` controls only which families are drawn in the overlay PDFs. The maintained note-facing defaults should omit `MuonRejection` from the harvested total and overlay legends, while keeping the `IsMuTaggedFalse` input hooks available so `MuonRejection` can still be re-enabled through an explicit `INCLUDE_FAMILIES` override.
- **20260405 pp energy-extrapolation systematic convention**: `MainAnalysis/20241102_ZhadronVsZPt/systematics.sh` is the maintained pp corrected-data entrypoint for the `_EEPrivate` variation, which should swap only `EEWeightFile_PP_PRIVATE` for the nominal energy weight while keeping the official pp tag family unchanged. In `Systematics/20260329_pPbSystematics/`, the pp defaults now also harvest and plot an `EnergyExtrapolation` family from the nominal vs `_EEPrivate` corrected-data difference, and the standalone note-facing comparison entrypoint is `run-energy-extrapolation.sh` with the variation labeled `MC-driven`.
- **20260216 temp-study convention**: `MainAnalysis/20260216_temp/` should source `OfficialWeightDictionary.sh`, use the official pp merged inputs plus direct-label `V0.1` PA inputs, and keep study ROOT outputs in `output/` while reserving `plots/` for PDFs. For note-facing track-muon refreshes, use the maintained runtime knob `CIRCLE_RADII=0.0025,0.004 ./plot-MuTrk.sh ...` so the published PDFs show exactly the nominal `0.0025` circle and the `0.004` systematic variation circle without changing the default code path.
- **20260216 pp muon-map interpretation convention**: the current temp-study `hDeltaRMuTrk` map in `MainAnalysis/20260216_temp/CorrelationAnalysis.cpp` now shares the same muon-rejection mode choice (`IsMuTagged`, `TrackMuDR`, `TrackMuClosest`) and track pt/eta acceptance as the maintained study selection, but it is still only an occupancy diagnostic. It does not include residual weights, event/Z/VZ/energy weights, mixed-event subtraction, or the full maintained note normalization, so it must not be treated as a direct predictor of the final note-facing `IsMuTaggedFalse` systematic magnitude.
- **20260412 pp IsMuTaggedFalse forensic convention**: for pp `ZPT30_500`, `trkPT4_15`, the promoted pp data skim and current temp map both give zero tracks inside `DeltaR < 0.0025`; the surviving nominal-vs-`IsMuTaggedFalse` difference in that bin therefore comes from the dedicated `IsMuTaggedFalse` residual-weight family, not from direct near-muon occupancy. For pp muon-track forensics, always run a narrow no-residual diagnostic rerun before calling a remaining effect a genuine occupancy-driven near-side signal.
- **20251211 residual-systematics runner convention**: in `MainAnalysis/20251211_ResidualCorrection/workflow/`, the maintained common residual-variation wrappers are `run-pp-systematics.sh` and `run-pPb-systematics.sh`. They should cover `Loose`, `Tight`, `IsMuTaggedFalse`, and `TrackCorrection{0p976,1p024}`, while the PU residual family stays in the dedicated `run-pp-pu.sh` / `run-pPb-pu.sh` helpers; the pp wrapper should call `run-pp-pu.sh` explicitly so the full pp residual-systematics rerun surface is unambiguous.
- **20251211 capped-pt residual-test convention**: for capped track-pt residual studies in `MainAnalysis/20251211_ResidualCorrection/`, propagate the cap through the workflow with the shared `TRACK_PT_MAX` / `RESIDUAL_HISTOGRAM_MAX_TRACK_PT` path in `workflow/pythia-analysis.sh` so RECO, GEN, iterative correction production, and final closure all use the same study range. In this workspace the upper track-pt bound should be treated as exclusive (`trackPt >= MaxTrackPT` rejected) to avoid clipped-at-cap overflow leaking into the x-overflow bin, and the correction/closure pt plotting macros should read their displayed range from the histogram axis instead of hardcoded legacy limits.
- **20251211 residual systematic-policy convention**: in `MainAnalysis/20251211_ResidualCorrection/workflow/{pythia-analysis.sh,pPb-DY-analysis.sh}`, route residual MC command-line extras through explicit buckets instead of one generic extra-arg bundle. Shared RECO+GEN track arguments are `IsMuTagged`, `TrackMuDR`, and `TrackMuClosest`; RECO-only arguments are `TrackSelectionMode` and `TrackExtraWeight`; `IsPUReject` should stay off the MC residual-derivation commands because the maintained PU residual family is defined by alternate VZ/Z weights while the direct PU veto remains data-only in the executable.
- **20251211 residual muon-rejection convention**: `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp` should mirror the maintained main-analysis `rejectMuonMatchedTrack(...)` semantics: explicit `TrackMuDR` overrides first, then `TrackMuClosest`, then boolean `IsMuTagged`. Residual policy or study reruns that exercise `TrackMuDR` / `TrackMuClosest` should therefore treat those knobs as applying to both MC RECO and MC GEN, matching the maintained track-level rejection logic in `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`.
- **20260216 temp-study raw-forest convention**: `MainAnalysis/20260216_temp/scan_muon_track_rectangle.cpp` raw-forest reco/reco mode should auto-detect `ppTrack/trackTree` vs `PbPbTracks/trackTree`, and optional `--RawTriggerAND=<prefix1>,<prefix2>,...` should require each trigger family via nonzero `TriggerTreeMessenger::CheckTriggerStartWith(...)`.
- **20260216 temp-study old-skims replay convention**: `MainAnalysis/20260216_temp/` should replay alternate pp / pPb / PbP data, reco-MC, and gen-MC skim bundles through the maintained `*_INPUT_OVERRIDE` hooks (plus `PPB_EPOS_INPUT_OVERRIDE` / `PBP_EPOS_INPUT_OVERRIDE` for PA gen), route replayed ROOT/PDF outputs through `output/<subdir>/` and `plots/<subdir>/`, keep the maintained old-`v6` replay on `mergedSample/{pp-v6,pythia-v6,pythia-gen-v6,HISingleMuon-v6,DY-v6,DY-gen-v6}.root` with outputs isolated under `output/oldskims/` and `plots/oldskims/`, and fill missing old-PA UE `trackMuTagged` / `trackMuDR` entries with `false` / `-1` when those branches are absent.
- **20260329 systematics-plot convention**: the compiled plotter in `Systematics/20260329_pPbSystematics/` should emit, per observable, the absolute overlay (`-Delta{Phi,Eta}-absolute.pdf`), the relative overlay (`-Delta{Phi,Eta}-relative.pdf`, using absolute uncertainty divided by the absolute nominal bin content), and the central-value plot (`-Delta{Phi,Eta}-central.pdf`) with total systematic uncertainties used as the bin errors.
- **20260329 individual-systematics plot convention**: `Systematics/20260329_pPbSystematics/run-pu.sh`, `run-track-selection.sh`, `run-track-correction.sh`, and `run-muon-track.sh` are the maintained standalone runners for the PU, loose/nominal/tight, track-correction, and muon-track rejection comparison figures. They read the corrected-data nominal and variation `*_trkResidual_<official-tag>_*_ZPT*-result.root` inputs directly from `MainAnalysis/20241102_ZhadronVsZPt/plots/`, write PDFs to `plots/pu/`, `plots/trackSelection/`, `plots/trackCorrection/`, and `plots/muonTrack/`, and should emit both the separate `pPb` / `PbP` plots and the combined `pPbPbp` plots by default. Because the analysis effectively double-fills bins, both the standalone comparison plots and the compiled `20260329` systematic calculation/plotting path should apply the same quoted `0.5` normalization to `Delta{Phi,Eta}_Result<trkPT>`. The standalone pp comparison figures should label the extrapolated pp reference as `pp (8.16 TeV)`, while the combined `pPbPbp` comparison figures should label the merged result as `pPb (8.16 TeV)`. The PU figures keep the nominal/variation overlay with a subtraction lower panel (`variation - nominal`), the track-selection figures use the same overlay-plus-difference layout for `0.5`-normalized nominal, loose, and tight, and the track-correction figures do the analogous nominal/0.976/1.024 comparison. The muon-track workflow should use the maintained `systematics-trackMuDR.sh` family (`TrackMuDR0p001`, `TrackMuDR0p0025`, `TrackMuDR0p0035`, `TrackMuDR0p004`, `TrackMuClosestTrue`) together with `run-muon-track.sh`, whose defaults/overrides are driven by `SYSTEMS`, `ZPT_RANGES`, `TRACK_RANGES`, `VARIATION_TAGS`, and `VARIATION_LABELS`; note-facing reruns should refresh `pp`, `pPb`, `PbP`, and combined `pPbPbp` from the promoted V0.3 skim inputs or explicit input overrides. When this workflow overlaps with skim production, set `SKIP_CLEAN=1` on the upstream variation runner and avoid `clean.sh` / `CommonCode` rebuilds. The muon-track lower panels should use the fixed range `[-0.2, 0.2]`, and the `DeltaPhi` top panels should be given the larger headroom requested for the note figures.
- **20260407 non-pp TrackMuDR equivalence convention**: in the current official `V0.2` pPb / PbP data skims, `trackMuTagged` matches `trackMuDR >= 0 && trackMuDR < 0.0025` exactly. If a note-facing non-pp nominal vs `TrackMuDR0p0025` comparison disagrees, first check whether the nominal corrected-data outputs are stale relative to the freshly rerun variation outputs; refresh the narrowed nominal non-pp products before diagnosing a downstream code bug.
- **20260213 combined-result plot convention**: `Plots/20260213_Central/plot-central-combined.sh` should source `OfficialWeightDictionary.sh` for `OFFICIAL_TAG_PP` and `OFFICIAL_TAG_PPB`, accept an external `CONFIG_FILE` to scan arbitrary Z/track grids, honor `PLOT_OUTPUT_BASE` when redirecting the `<OFFICIAL_TAG_PP>/` PDF staging directory, and pass through `PLOT_INCLUDE_MC={true,false}` to `plot_central_combined.cpp`. The plotter should always read the corrected `pp_trkResidual_*`, `pPb_trkResidual_*`, and `PbP_trkResidual_*` central outputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/`; when `PLOT_INCLUDE_MC=true` it should also keep the Powheg+EPOS MC legs from `pPbMC_Gen_nominal_*` and `PbPMC_Gen_nominal_*`, while `PLOT_INCLUDE_MC=false` is the maintained data-only fallback for note/presentation scans that only need the pp and combined pPb+PbP central curves.
- **20260424 pre-subtraction central-overlay convention**: `Plots/20260213_Central/plot-central-sb.sh` + `plot_central_sb.cpp` are the maintained pPb-vs-Pbp corrected-data pre-subtraction overlay path. They must follow the `Plots/20260120_CentralClosure/plot_closure.cpp` closure-style plotting path rather than the older sandbox-style helper, read the fully corrected `pPb_trkResidual_*` / `PbP_trkResidual_*` `-nosub.root` inputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/`, keep the inclusive defaults `ZPT0_500`, `trkPT0.5_15`, use the signed `#Delta y_{ch,Z}` / `#Delta#phi_{ch,Z}` axes from `Delta{Eta,Phi}_Result<trkPT>` together with the mixed projections from `hMixData_<trkPT>`, and write the four data-only `Delta{Eta,Phi}-{all,bkg}.pdf` overlays to `Plots/20260213_Central/plots/central_sb/`.
- **20260213 result-style convention**: `CommonCode/include/KylesPlotting.h` `PlotCMSDiffResult(...)` lower pads should default to the point/statistical/systematic presentation only and must not draw a base `HIST` line before the lower-panel markers. Note-facing reruns of inclusive and scan result plots should therefore show points with stat bars plus the systematic band, without the extra vertical histogram-bar look in the lower panel.
- **20260418 paper-plot convention**: `Plots/20260213_Central/plot_paper.cpp` + `plot-paper.sh` produce 4 draft-paper PDFs in `plots/paper/<pp_tag>/`: `paper-DeltaPhi-inclusive.pdf`, `paper-DeltaEta-inclusive.pdf`, `paper-DeltaPhi-scan.pdf`, `paper-DeltaEta-scan.pdf`. The plotter uses `PlotCMSPaperDiffResult` from `KylesPlotting.h` for the inclusive plots (pixel-based fonts, no auto CMS header, `labelScale` parameter) and a manual 12-pad grid for the 2×3 scan (rows: ZPT {0_30, 30_500}; cols: trkPT {0.5_2, 2_4, 4_15}). Paper header is "CMS" bold (no "Preliminary") top-left and "pPb (pp) 8.16 TeV 174 nb^{-1} (301 pb^{-1})" top-right. Data loading uses the same combine logic as `plot_central_combined.cpp` via a shared `LoadBin()` helper.
- **Finalized note-facing bin convention**: the maintained note-facing result, systematics, and pp energy-extrapolation scan defaults are `ZPT={0_30,30_500}` with `trkPT={0.5_2,2_4,4_15}`, plus the inclusive `ZPT0_500`, `trkPT0.5_15` selection. Older `5_30`, `0.5_4`, `4_500`, and `0.5_500` defaults are stale for note-facing products.
- **pp energy-extrapolation study convention**: the maintained progressive pp energy-extrapolation study in `MainAnalysis/20241102_ZhadronVsZPt/pp-EE.sh` / `Plots/20260213_Central/plot_energyExtrapolation.cpp` should run on the finalized note-facing bins, include the maintained pp VZ weighting in all curves, apply the energy weight multiplicatively to the pp corrected-data leg, and label the four overlays as baseline, `+ Z correction`, `+ Z and track correction`, and `+ Z, track, energy correction`.
- **Official tag/version convention**: do not promote ad hoc descriptive suffixes such as `newVZFix` or `skimVZOff` into official correction or note-facing tags. If a correction family is unchanged, keep the existing promoted tag; if the physics content changes, increment the canonical version families (`ZV*`, `trkV*`) instead of adding another descriptor.

## Production workflow style (official outputs)

- For official production/reprocessing, favor:
  - modifying existing scripts, or
  - adding new dedicated scripts in-repo.
- Avoid one-off arbitrary terminal command chains for official plot/correction production.
- Prefer driving these via scripted `system-analysis.sh` calls.
- One-off ad hoc terminal commands are acceptable for debugging/diagnostics.
- For the `20241102_ZhadronVsZPt` workflow specifically, avoid parallel “fixup” runner families for official reruns. Update the canonical script in place, commenting or replacing superseded calls there, so future reviewer/analyzer work has one authoritative entrypoint per stage.

## Runtime and filesystem norms

- Use `python3` (not `python`).
- Files intended for `/tmp` should instead be written under `/tmp/kdeverea`.
- Default behavior: write plots/products to analysis repo paths unless task explicitly says otherwise.

## User defined folder meanings

- `MainAnalysis/20241102_ZhadronVsZPt/`: the main correlation analysis workflow for the Z-hadron correlation measurement.
- `Plots/20251001_pPbVZReweighting/summary/`: current Vz reweighting summary outputs; latest updated note-ready pp/pPb/PbP Vz plots are `20260311_*`.
- `MainAnalysis/20260115_ZCorrection/`: the workflow for deriving Z reweighting corrections for the Z-hadron correlation analysis. Closure for this correction is demonstrated by running it through the main analysis script `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh` and plots are produced in `Plots/20260115_ZResidualClosure/`.
- `Plots/20260115_ZResidualClosure/`: Z-correction closure plotting outputs used for note figures (e.g. `ZPT0_500_Zclosure-closure-*.pdf` under `plots/{pp,pPb,PbP}`).
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/`: track residual correction-iteration outputs; latest updated set uses `TrackResidualCorrection_V24_ZWeight_V6`.
- `Plots/20251202_trackResidualClosure/plots/`: track-level no-subtraction closure outputs (`*-nosub-closure-*`), including the `trkV24` updated pPb/PbP figures.
- `Plots/20260120_CentralClosure/plots/`: background-subtracted and pre-subtraction closure outputs (`*-closure-Delta{Eta,Phi}-{all,bkg,result}.pdf`), including `trkV24` updated pPb/PbP figures.
- `Plots/20260120_CentralClosure/`: for current-tag pp closure refreshes, the maintained path is to run `ExecuteClosureTest` directly for `collisionType=pp` over `ZPT={0_10,10_20,20_40,40_500}` with `trkPtRange=0.5_500` and `tag=$OFFICIAL_TAG_PP`; `plot-central.sh` does not by itself refresh the pp closure panels.
- `SampleGeneration/20250929_ReducedTreePA/`: the PA reduced-tree skimming workflow that produced the skim inputs now used under `pPbSample/V0.2/`. `ReduceForest.cpp` converts full forest ROOT inputs into skim trees; `make Prepare` creates `Samples/{PAMC,APMC,PAData}` symlinks to the forest locations, and the forest-side pPb/PbP orientation labels are the correct physical ones even where downstream skim naming later became swapped.
- `SampleGeneration/20260403_PythiaMadgraph/`: the private pp generator workflow for local `Z -> mu mu` studies. It generates MG5 LHE events at `5.02` or `8.16` TeV, showers them with local Pythia8 using `CP5`, converts HepMC output into a single-`Tree` `ZHadronMessenger` skim, and includes `run_validation.sh` / `QuickDndEta.cpp` for quick `dN/deta` validation products under `output/`.
- `MainAnalysis/20260222_EnergyExtrapolation/`: the workflow for extrapolating the Z-hadron correlation measurement to higher collision energies. For pp data only.
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/private-closure.sh`: the maintained private pp 8.16/5.02 energy-extrapolation closure entrypoint. It compares `ZMuMu_{5020,8160}_validation.root`, writes unique `output/20260405_EnergyExtrapolation_private8160over5020_closure*` products without touching the promoted `EEV*` files, and keeps the public-facing correction basename ending in `_closure`.
- `Plots/20260213_Central/plot_dataMCComparison.cpp` + `plot-dataMCComparison.sh`: data/MC comparison plotting entrypoint; reads ROOT inputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/` and writes figures to `Plots/20260213_Central/plots/dataMCComparison/`.
- `Plots/20260213_Central/plot_pp.cpp` + `plot-pp.sh`: pp closure/data-vs-GEN plotting entrypoint; it expects `plots/{pp_nominal,pp_ZResidual,pp_trkResidual,pythiaMC_Gen_nominal}_<tag>_ZPT*-result.root` in `MainAnalysis/20241102_ZhadronVsZPt/plots/`, with all requested track-pt histograms merged into each ZPT result file.
- `Plots/20260213_Central/plot-zmass.sh`: the pp wrapper also requires current-tag `pythiaMC_Gen_nominal_${OFFICIAL_TAG_PP}_ZPT0_500-nosub.root` and `pythiaMC_nominal_${OFFICIAL_TAG_PP}_ZPT0_500-nosub.root`; if those are missing, regenerate those exact pp MC nominal roots before trusting the emitted PDF.
- **20260425 dimuon-mass ratio convention**: `Plots/20260213_Central/plot_zmass.cpp` should now use the standard CMS ratio helper so the maintained default PDF includes a bottom ratio panel. Keep the existing top-panel data/simulation overlay, use data as the ratio baseline, and show at least the scaled `MC Reco / data` ratio within a maintained `0.5-1.5` y-range (the scaled `MC Gen / data` ratio may appear alongside it).
- `MainAnalysis/20260414_pPbPbpCombining/`: self-contained pPb/PbP comparison sandbox. Observables: trkPt, trkEta, ZPt, Zy, Mult, DEta, DPhi, DEtaDPhi 2D ratio, and presentation-only ZEtaPhi 2D ratio. No DR. `trackCharge` is empty in V0.2/V0.3 skims (ReduceForest never populates it) so charge-based observables are unavailable. Uses `runner.sh` (6 correction levels × 2 PU families = 24 runs), `run_z_correction.sh` (direct 2D pPb→PbP Z correction), `runner_with_zcorr.sh` (Z-corrected runs), and `_chunk${N}.root` naming for hadd safety. Presentations in `presentations/`.
- **20260424 pPb/PbP sandbox Z-correction convention**: in `MainAnalysis/20260414_pPbPbpCombining/`, the maintained Z-data correction is the direct 2D pPb$\to$Pbp event weight derived from `hZYPhiData` with the `+VZ+Z+ZSF` weighting context already applied. `run_z_correction.sh` should read `output/pPb_VZ_Z_ZSF.root` and `output/PbP_VZ_Z_ZSF.root`, store both the raw pPb/Pbp ratio and the multiplicative `hWeightToApply = Pbp/pPb` surface in `output/z_correction_final.root`, and the maintained followup now uses 24 `y_\text{CM}` bins (with 12 `\phi` bins) in `CorrelationAnalysis.cpp` for `hZYData`, `hZYPhiData`, and the plotted `Zy` closure. If a prompt says `(Z eta, Z phi)` for this sandbox, keep the maintained correction and closure on `(Z y_\text{CM}, Z \phi)` and document that wording resolution explicitly in the zcorr presentation/summary.
- **20260423 sandbox run-average convention**: `MainAnalysis/20260414_pPbPbpCombining/tabulate-run-average-deta.sh` + `tabulate_run_average_deta.cpp` are the maintained run-by-run `|DeltaEta|` tabulation path. The maintained default is the inclusive `ZPT0_500`, `trkPT0.5_15` data selection with `UseEventWeight`, `UseVZWeight`, `UseZWeight`, `UseZScaleFactor`, `UseTrackWeight`, and `UseResidualWeight` enabled but no sandbox `ZCorrectionFile`, and the default table path is `MainAnalysis/20260414_pPbPbpCombining/output/run_average_abs_deta_no_zcorr.tsv`.
- **20260425 sandbox run-consistency convention**: the maintained run-consistency followup is `MainAnalysis/20260414_pPbPbpCombining/analyze-run-average-deta.sh`, which refreshes `output/run_average_abs_deta_no_zcorr.tsv`, writes the markdown chi-square assessment to `output/run_average_abs_deta_no_zcorr_consistency.md`, and writes the equally spaced run-order plot (Pbp runs first, then pPb, with actual run numbers as tick labels) to `plots/run_average_abs_deta_no_zcorr_by_run.pdf`. The run central value remains `avg |DeltaEta| = sum_i w_i x_i / sum_i w_i`, but the per-run statistical uncertainty stored in the table and used for the plot must treat each selected Z event as the independent unit: for event-level `X_e = sum_t w_et` and `Y_e = sum_t w_et x_et`, use `sigma_run = sqrt(sum_e (Y_e - xbar X_e)^2) / sum_e X_e`. `effective_tracks = (sum_i w_i)^2 / sum_i w_i^2` may still be reported as a track-level diagnostic, and runs with fewer than two selected events containing accepted tracks should be excluded from the `chi2/nu` consistency calculation.
- **20260425 sandbox 2D-map convention**: in `MainAnalysis/20260414_pPbPbpCombining/`, the maintained debug and zcorr presentation surfaces should keep the `|DeltaEta|` and `|DeltaPhi|` 1D histograms on their original 12-bin presentation binning, fill `hDEtaDPhiData` with `120 x 120` bins (10x the original 12-bin map per axis), keep the existing Pbp/pPb `(|DeltaEta|, |DeltaPhi|)` ratio panels for all 6 correction levels and their PUReject variants, and add separate `ZEtaPhi_ratio_*` / `zcorr_ZEtaPhi_ratio_*` presentation panels without replacing the maintained `(Z y_CM, Z phi)` correction/closure surface.
- **20260426 sandbox MCReco convention**: the maintained MC comparison companion in `MainAnalysis/20260414_pPbPbpCombining/` is `runner_mc.sh` + `plot-mc.sh` + `presentations/pPbPbp_mc_sandbox_presentation.tex`, using `OFFICIAL_MCRECOINPUT_PPB` / `OFFICIAL_MCRECOINPUT_PBP` from `OfficialWeightDictionary.sh` together with the official `VZWeightFile_{PPb,PbP}`, `ZWeightFile_{PPb,PbP}`, and `RWeightFile_{PPb,PbP}`. The MCReco skims carry `ZWeight = 1`, so the maintained `+ZSF` stage is intentionally a no-op that is kept only to match the existing correction-stack naming; there is no PUReject branch in this MC companion flow, and `plot_comparison.cpp` should skip missing correction levels cleanly.
- **20260426 sandbox MC run-average convention**: the maintained MC run-average companion is `MainAnalysis/20260414_pPbPbpCombining/analyze-run-average-deta-mc.sh`, which reads the official `V0.3` `OFFICIAL_MCRECOINPUT_PPB` / `OFFICIAL_MCRECOINPUT_PBP` skims, writes `output/run_average_abs_deta_mc_no_zcorr.tsv` plus `output/run_average_abs_deta_mc_no_zcorr_consistency.md`, and writes the plot to `plots/pPbPbp_run_average_abs_deta_mc_by_run.pdf`. The current official MCReco skims have `run = 1` for both systems, so the maintained plot should label the two points distinctly via system-prefixed tick labels (`Pbp:1`, `pPb:1`) rather than pretending there is a multi-run spread within each system.
- **20260426 sandbox pp run-average convention**: the maintained pp run-average companion is `MainAnalysis/20260414_pPbPbpCombining/analyze-run-average-deta-pp.sh`, which reads `OFFICIAL_DATAINPUT_PP` from `OfficialWeightDictionary.sh`, uses the official pp `VZWeightFile_PP`, `ZWeightFile_PP`, and `RWeightFile_PP`, writes `output/run_average_abs_deta_pp.tsv` plus `output/run_average_abs_deta_pp_consistency.md`, and writes the plot to `plots/run_average_abs_deta_pp_by_run.pdf`. The helper reuses the same propagated weighted-mean uncertainty definition and inverse-variance `chi2/nu` calculation as the maintained pPb/Pbp run-average path, but in pp-only mode the summary should report just the single `pp only` grouping.
- **20260421 pPb half-split sandbox convention**: in `MainAnalysis/20260414_pPbPbpCombining/`, the maintained pPb half-split study keeps `PbP` at full statistics but splits the official `V0.3` pPb skim by the `Tree` `run` branch. The skim entry order is not strictly increasing in `Run` (and is not monotonic overall), so the maintained lower/upper split is defined by `Run < 286302` (2,307,996 entries) vs `Run >= 286302` (2,181,222 entries), with outputs staged under `output/pPb_runlt286302/`, `output/pPb_runge286302/`, `plots/pPb_runlt286302/`, and `plots/pPb_runge286302/`, and presentation PDFs emitted as `presentations/pPbPbp_{runlt,runge}286302_sandbox_presentation.pdf`.
- **20260422 PbP half-split sandbox convention**: in `MainAnalysis/20260414_pPbPbpCombining/`, the additive PbP half-split study keeps `pPb` at full statistics but splits the official `V0.3` PbP skim by the `Tree` `run` branch. The skim entry order is not strictly increasing in `Run` (and is not monotonic overall), so the maintained lower/upper split is defined by `Run < 285549` (1,341,303 entries) vs `Run >= 285549` (1,223,786 entries), driven through `runner_PbP_{first,second}_half.sh` / `plot_PbP_{first,second}_half.sh`, with outputs staged under `output/PbP_runlt285549/`, `output/PbP_runge285549/`, `plots/PbP_runlt285549/`, and `plots/PbP_runge285549/`, and presentation PDFs emitted as `presentations/pPbPbp_PbP_{runlt,runge}285549_sandbox_presentation.pdf`.
- **20260424 sandbox extremal-run convention**: in `MainAnalysis/20260414_pPbPbpCombining/`, the maintained four-run discrepancy followup is driven from `output/run_average_abs_deta_no_zcorr.tsv` using the singled-run selections `pPb` max/min `Run 286327` / `Run 286441` and `Pbp` max/min `Run 285718` / `Run 285537`, implemented through `runner_extremal_runs.sh` (`--MinRun run --MaxRun run+1` on the relevant collision only), `plot-extremal-runs.sh` + `plot_extremal_runs.cpp`, with outputs staged under `output/pPb_run286327/`, `output/pPb_run286441/`, `output/PbP_run285718/`, `output/PbP_run285537/`, `plots/extremal_runs/`, and presentation PDFs emitted as `presentations/pPbPbp_extremal_runs_presentation.pdf`.
- `~/OverleafZHadronInPPb/`: analysis-note source tree; main file is `AN-23-ABC.tex` with sections in `src/*.tex`. Figure assets are organized by topic under `figures/` (`event`, `z_reco/eff`, `tracking`, `tracking/appendix`, `analysis/closure`, etc.).

## PA skimming workflow (`SampleGeneration/20250929_ReducedTreePA`)

- This directory contains the skim producer that turns PA forest files into the reduced ROOT datasets later consumed as `pPbSample/V0.2/*`.
- The main executable is built from `ReduceForest.cpp`; it reads one or more forest files via `--Input` and writes a skim via `--Output`, with toggles such as `--DoGenLevel`, `--IsData`, `--RunStart/RunEnd`, `--TrackEfficiencyPath`, and `--IgnoreEventWeight`.
- `make Prepare` creates local `Samples/` symlinks to the forest locations on `/eos`:
  - `Samples/PAMC` -> correctly labeled pPb MC forest directory
  - `Samples/APMC` -> correctly labeled PbP MC forest directory
  - `Samples/PAData` -> PA single-muon data forests
  - `Samples/PPData` -> 2017 pp single-muon data forests
  - `Samples/PPMC` -> 2017 pp DY MC forests
- The maintained local pp skim runner branches are `PPData5TeV` and `PPMC5TeV`. For local validation, keep subset outputs under the repository `output/` tree rather than `/eos`.
- Accessing those `/eos` forest paths requires the user to run `kinit -5` first. If a future task needs to inspect the forest files directly, ask the user to do that first. The user-owned EOS area `/eos/cms/store/group/phys_heavyions/kdeverea/` may be written for sanctioned skim or production tasks when the prompt explicitly calls for it, but do not run `hadd` there unless the prompt explicitly allows merging.
- Condor preparation is done by `PrepareCondor.sh`, which writes a submit description file `SubmitPA.condor` in the working directory. Submission is then done explicitly from that directory (for example with `condor_submit SubmitPA.condor`).
- Each Condor job runs `Condor.sh`, which:
  - bootstraps the CMSSW runtime on lxplus,
  - copies the local `Execute` binary into the job sandbox,
  - runs one reco skim and one gen skim for MC inputs,
  - or runs two reco skims for data, split into `_AP` and `_PA` outputs using the run ranges embedded in the script.
- Output naming convention from the Condor wrapper is:
  - MC: `Reco*.root` and `Gen*.root`
  - data: split reco outputs suffixed with `_AP.root` and `_PA.root`
- **20250929 pp tracking-efficiency convention**: in `SampleGeneration/20250929_ReducedTreePA/`, keep the PA/non-pp tracking path on `TrackEfficiencyCorrector.h` with `rTotalEff3D_0`, and route pp reco skims (`Tree`, `TreeLoose`, `TreeTight`) through `TrkEff2017pp` from `CommonCode/include/trackingEfficiency2017pp.h` inside `ReduceForest.cpp`. The maintained pp runner `local_skim.sh` passes the tracking correction as a full ROOT file path, so the pp skimmer wiring should convert that to the directory prefix expected by `TrkEff2017pp` rather than moving pp logic back into `TrackEfficiencyCorrector.h`.
- **V0.3 pp skim production convention**: for the unmerged `SampleGeneration/20250929_ReducedTreePA/V0.3` pp campaign, run `PPData` and `PPMC` sequentially (not overlapped), use `NTHREAD=15` for pp data and `NTHREAD=5` for pp MC unless the buffer issue forces a documented reduction, validate every expected output file for existence/openability/required keys, rerun only the bad subset with direct `local_skim.sh` calls, and never merge the outputs with `hadd`.

## Overleaf workflow and structure

- Overleaf repository path: `~/OverleafZHadronInPPb/`
- Main note entrypoint: `AN-23-ABC.tex`
- Section files: `src/*.tex` (for example `src/analysis.tex`, `src/results.tex`, `src/appendix.tex`)
- Figure hierarchy: topic-based subdirectories under `figures/`, commonly:
  - `figures/analysis/energy/`
  - `figures/analysis/closure/`
  - `figures/result/`
  - `figures/tracking/`
  - `figures/z_reco/eff/`

When updating note plots, copy generated PDFs from this analysis repository to Overleaf **only when explicitly called for by the user or reviewer plan**.
- In note text and captions, avoid internal production jargon (for example skims, versions, or workflow/execution details), do not say `PA`, and use `Pbp` capitalization exactly that way.

Standards for figure updates:
- derive an **explicit manifest** of the figures to update from `~/OverleafZHadronInPPb/src/*.tex`; do not use broad basename matching or global sync,
- preserve the **source basename exactly** when copying refreshed figures into Overleaf; do **not** rename a new `ZV6/trkV24` file to an older `ZV5/trkV23` or other stale note filename,
- if the note should use a new figure filename, update the corresponding `\includegraphics{...}` path in `src/*.tex` to that exact copied filename,
- if multiple figures would collide in the same Overleaf directory, preserve the source basename and resolve the collision with directory structure or a deliberate TeX path update rather than silently renaming to an old filename,
- after updating references, remove stale superseded copies only if they are no longer referenced by any `src/*.tex` file.

Concrete method:
1. Extract the current note references from `src/*.tex` and identify the exact figures that must be refreshed.
2. Build a source-to-destination manifest for only those figures.
3. Copy each refreshed PDF to Overleaf using the source basename.
4. Update `src/*.tex` so every changed `\includegraphics{...}` points to the copied source basename and directory.
5. Run a post-copy audit on the changed figure set.

Typical copy/update pattern for central-analysis products:
```bash
python3 - <<'PY'
from pathlib import Path
import shutil

source = Path("/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf")
destdir = Path("/home/kdeverea/OverleafZHadronInPPb/figures/result")
destdir.mkdir(parents=True, exist_ok=True)
shutil.copy2(source, destdir / source.name)
PY
```

Typical TeX update pattern after copying:
```bash
python3 - <<'PY'
from pathlib import Path
tex = Path("/home/kdeverea/OverleafZHadronInPPb/src/results.tex")
old = "figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf"
new = "figures/result/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf"
text = tex.read_text()
tex.write_text(text.replace(old, new))
PY
```

Required validation after copying:
- verify the copied Overleaf PDF is byte-identical to the analysis source for the changed figure set (for example with `sha256`),
- verify there are no missing `\includegraphics{...}` targets among the changed figures,
- distinguish unrelated pre-existing missing references from the figures changed in the current task,
- check `git status --short` in `~/OverleafZHadronInPPb` and confirm the modified files match the intended manifest.

## Correction-stack order and closure expectation

The order with which corrections should be calculated is: VZ, Z correction, track residual correction, energy extrapolation (for pp only). The VZ reweighting is used in the Z correction, and both the VZ and Z corrections are used in the track residual correction. The final correlation analysis can be run with any combination of these corrections, but the central result should use all of them together. "Total" closure of the correction in the main analysis script is demonstrated on MC sets as run by `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh` and plots produced in `Plots/20260120_ZhadronVsZPtClosure/`.

Finally, the main results are produced in `MainAnalysis/20241102_ZhadronVsZPt/central.sh` and plots are produced in `Plots/20260213_Central/`.

## Known issue context to keep in mind

- A mixed-event `UseEventWeight` bug was identified in `CorrelationAnalysis.cpp` and fixed in later work; this affected mixed-event normalization.
- Historical VZ/event-weight interpretations may be biased if outputs were generated pre-fix.
- In planning/review documents, explicitly label outputs as pre-fix or post-fix where relevant.
- Closure-only regressions do not automatically imply downstream central/result plots are stale. Propagate reruns only when the underlying promoted correction ROOTs or central ROOTs actually change; for example, pp correction changes can affect energy extrapolation and final combined plots, while heavy-ion closure-label fixes alone do not.
