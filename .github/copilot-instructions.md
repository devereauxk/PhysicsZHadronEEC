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
- **V0.2 heavy-ion input convention**: for official pPb / PbP correction and closure work, do not trust the raw `V0.2` basename as the physical-system label. Follow the switched convention already used in the correction producers: the physical `PPb` branch uses `PbPMC_*` plus `PbPData_Reco.root` and `mergedEPOS/PPbMC_Gen.root`, while the physical `PbP` branch uses `PPbMC_*` plus `PPbData_Reco.root` and `mergedEPOS/PbPMC_Gen.root`.
- **Canonical 20241102 runner convention**: in `MainAnalysis/20241102_ZhadronVsZPt/`, the maintained official entrypoints are `closure-VZ.sh`, `closure-Z.sh`, `closure-trk.sh`, and `central.sh`. When a rerun needs fixes, fold them into these scripts directly rather than creating new persistent variants such as `*-newVZFix.sh`.
- **Official weight dictionary convention**: correction runner scripts must source `/home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh` and consume `VZWeightFile_{PP,PPb,PbP}`, `ZWeightFile_{PP,PPb,PbP}`, `RWeightFile_{PP,PPb,PbP}`, and `EEWeightFile_PP` from that dictionary, rather than hardcoding weight paths in each script.
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
- `SampleGeneration/20250929_ReducedTreePA/`: the PA reduced-tree skimming workflow that produced the skim inputs now used under `pPbSample/V0.2/`. `ReduceForest.cpp` converts full forest ROOT inputs into skim trees; `make Prepare` creates `Samples/{PAMC,APMC,PAData}` symlinks to the forest locations, and the forest-side pPb/PbP orientation labels are the correct physical ones even where downstream skim naming later became swapped.
- `MainAnalysis/20260222_EnergyExtrapolation/`: the workflow for extrapolating the Z-hadron correlation measurement to higher collision energies. For pp data only.
- `Plots/20260213_Central/plot_dataMCComparison.cpp` + `plot-dataMCComparison.sh`: data/MC comparison plotting entrypoint; reads ROOT inputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/` and writes figures to `Plots/20260213_Central/plots/dataMCComparison/`.
- `Plots/20260213_Central/plot_pp.cpp` + `plot-pp.sh`: pp closure/data-vs-GEN plotting entrypoint; it expects `plots/{pp_nominal,pp_ZResidual,pp_trkResidual,pythiaMC_Gen_nominal}_<tag>_ZPT*-result.root` in `MainAnalysis/20241102_ZhadronVsZPt/plots/`, with all requested track-pt histograms merged into each ZPT result file.
- `~/OverleafZHadronInPPb/`: analysis-note source tree; main file is `AN-23-ABC.tex` with sections in `src/*.tex`. Figure assets are organized by topic under `figures/` (`event`, `z_reco/eff`, `tracking`, `tracking/appendix`, `analysis/closure`, etc.).

## PA skimming workflow (`SampleGeneration/20250929_ReducedTreePA`)

- This directory contains the skim producer that turns PA forest files into the reduced ROOT datasets later consumed as `pPbSample/V0.2/*`.
- The main executable is built from `ReduceForest.cpp`; it reads one or more forest files via `--Input` and writes a skim via `--Output`, with toggles such as `--DoGenLevel`, `--IsData`, `--RunStart/RunEnd`, `--TrackEfficiencyPath`, and `--IgnoreEventWeight`.
- `make Prepare` creates local `Samples/` symlinks to the forest locations on `/eos`:
  - `Samples/PAMC` -> correctly labeled pPb MC forest directory
  - `Samples/APMC` -> correctly labeled PbP MC forest directory
  - `Samples/PAData` -> PA single-muon data forests
- Accessing those `/eos` forest paths requires the user to run `kinit -5` first. If a future task needs to inspect the forest files directly, ask the user to do that first. Do not modify or execute files on `/eos`; treat those locations as read-only unless the user explicitly says otherwise in a future task.
- Condor preparation is done by `PrepareCondor.sh`, which writes a submit description file `SubmitPA.condor` in the working directory. Submission is then done explicitly from that directory (for example with `condor_submit SubmitPA.condor`).
- Each Condor job runs `Condor.sh`, which:
  - bootstraps the CMSSW runtime on lxplus,
  - copies the local `Execute` binary into the job sandbox,
  - runs one reco skim and one gen skim for MC inputs,
  - or runs two reco skims for data, split into `_AP` and `_PA` outputs using the run ranges embedded in the script.
- Output naming convention from the Condor wrapper is:
  - MC: `Reco*.root` and `Gen*.root`
  - data: split reco outputs suffixed with `_AP.root` and `_PA.root`

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
