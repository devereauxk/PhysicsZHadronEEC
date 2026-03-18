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

## Production workflow style (official outputs)

- For official production/reprocessing, favor:
  - modifying existing scripts, or
  - adding new dedicated scripts in-repo.
- Avoid one-off arbitrary terminal command chains for official plot/correction production.
- Prefer driving these via scripted `system-analysis.sh` calls.
- One-off ad hoc terminal commands are acceptable for debugging/diagnostics.

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
- `MainAnalysis/20260222_EnergyExtrapolation/`: the workflow for extrapolating the Z-hadron correlation measurement to higher collision energies. For pp data only.
- `Plots/20260213_Central/plot_dataMCComparison.cpp` + `plot-dataMCComparison.sh`: data/MC comparison plotting entrypoint; reads ROOT inputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/` and writes figures to `Plots/20260213_Central/plots/dataMCComparison/`.
- `Plots/20260213_Central/plot_pp.cpp` + `plot-pp.sh`: pp closure/data-vs-GEN plotting entrypoint; it expects `plots/{pp_nominal,pp_ZResidual,pp_trkResidual,pythiaMC_Gen_nominal}_<tag>_ZPT*-result.root` in `MainAnalysis/20241102_ZhadronVsZPt/plots/`, with all requested track-pt histograms merged into each ZPT result file.
- `~/OverleafZHadronInPPb/`: analysis-note source tree; main file is `AN-23-ABC.tex` with sections in `src/*.tex`. Figure assets are organized by topic under `figures/` (`event`, `z_reco/eff`, `tracking`, `tracking/appendix`, `analysis/closure`, etc.).

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

When updating note plots, copy generated PDFs from this analysis repository to Overleaf **only when explicitly called for by the user or reviewer plan**, then ensure `\includegraphics{...}` paths in `src/*.tex` match those filenames.

Typical copy pattern for central-analysis products:
```bash
cp -f /home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/<subdir>/*.pdf \
      /home/kdeverea/OverleafZHadronInPPb/figures/<target-subdir>/
```

After copying, run a reference audit to ensure there are no missing or uncited figures in Overleaf.

## Correction-stack order and closure expectation

The order with which corrections should be calculated is: VZ, Z correction, track residual correction, energy extrapolation (for pp only). The VZ reweighting is used in the Z correction, and both the VZ and Z corrections are used in the track residual correction. The final correlation analysis can be run with any combination of these corrections, but the central result should use all of them together. "Total" closure of the correction in the main analysis script is demonstrated on MC sets as run by `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh` and plots produced in `Plots/20260120_ZhadronVsZPtClosure/`.

Finally, the main results are produced in `MainAnalysis/20241102_ZhadronVsZPt/central.sh` and plots are produced in `Plots/20260213_Central/`.

## Known issue context to keep in mind

- A mixed-event `UseEventWeight` bug was identified in `CorrelationAnalysis.cpp` and fixed in later work; this affected mixed-event normalization.
- Historical VZ/event-weight interpretations may be biased if outputs were generated pre-fix.
- In planning/review documents, explicitly label outputs as pre-fix or post-fix where relevant.
