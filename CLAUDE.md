# CLAUDE Instructions for PhysicsZHadronEEC

## Mandatory session bootstrap

At the start of every session in this repository, read and apply
`/home/kdeverea/PhysicsZHadronEEC/.claude/AGENT_REQUIREMENTS.md`. The operational
checklist before any exploration, edits, installs, or git operations:

- All actions stay under `/home/kdeverea`.
- No `sudo`. Ask before any install. Prefer preinstalled deps; if you must install,
  do it under `/home/kdeverea` only, after explicit permission.
- Ask before any commit or push.
- Do not modify repositories not owned by `devereauxk`.
- Do not share project data with external systems; do not use repository data for training.
- Do not interfere with other users' processes.
- Do not merge / `hadd` files that are not on the `/home/` filesystem; in particular
  never merge ROOT files on `/eos/`.

## Reviewer / Analyzer role model (mandatory when prompted)

When initialized with wording like "you are a reviewer..." or "you are an analyzer...",
apply this contract strictly.

- **Reviewer**: communicates with the user. Scope is code review, impact analysis,
  task planning, and writing task instructions in `.md` files for analyzers.
  Reviewer should not run analyzer production work; delegate via task markdown and
  review the returned summary.
- **Analyzer**: generally does not interact with the user. Executes tasks from
  reviewer-written `.md` files sequentially (parallel only if the task explicitly
  allows it). Returns a completion `.md` summary covering commands run, outputs
  produced, validation checks, failures/retries, and final status.
- **Handoff**: Reviewer defines scope, acceptance criteria, ordering, and stop
  conditions. Analyzer executes and reports. Reviewer validates and reports back to user.
- **Writer**: edits Overleaf note text only; does not write or run code. Constrained
  to the specific section(s) dispatched to it — must not edit, check typos, or
  reformat other sections. Follows the style contract below.

### Writer style contract (Overleaf `~/OverleafZHadronInPPb/`)

- **No implementation references**: never mention ROOT files, code variables, script
  names, file paths, tag strings, skim versions, or workflow details. Write at the
  physics level only.
- **Voice and tense**: present tense for descriptions of method ("We correct…",
  "The correction is applied…"); past tense for completed measurements ("The data
  were collected…"). First-person plural "we" throughout.
- **Collision system names**: `pp`, `pPb`, `Pbp` in running text (never `PbP` in
  prose; `PbP` appears only in file-name references). `$\rootsNN = 8.16$~\TeV`.
- **Kinematic ranges**: inline math with custom macros:
  `$0 < \ptz < 500$\GeV`, `$0.5 < \ptt < 15$\GeV`,
  `$0 < \ptz < 30$\GeV` / `$30 < \ptz < 500$\GeV`.
  Use `\GeV` (no `/c`), except `\GeVc` for momentum quantities when the note
  already does so (check surrounding context).
- **Custom macros** (defined in `custom-definitions.tex`):
  `\ptz`, `\ptt`, `\dphitz`, `\dytz`, `\detatz`, `\etat`, `\phit`, `\phiz`, `\yz`,
  `\zBoson`, `\zBosons`, `\PYTHIAEIGHT`, `\MGMCatNLO`, `\HYDJET`, `\rootsNN`, `\pp`.
- **Cross-references**: `Section~\ref{sec:...}` or `Sec.~\ref{sec:...}`;
  `Fig.~\ref{fig:...}`; `Table~\ref{table:...}`; `Eq.~\ref{eq:...}`.
  Non-breaking `~` before `\ref`.
- **Figure captions**: state what is plotted, name the collision system(s), specify
  the kinematic selection, and identify each panel/row/column. No trailing periods.
- **Common phrases**: "closure is seen", "sub-percent level agreement",
  "We apply kinematic selection of…", "The lower panels show…",
  "Variations are shown with an inclusive kinematic selection…".
- **Correction descriptions**: describe the physics idea and the iterative procedure
  in general terms (e.g. "iterative bin-by-bin reweighting"), never refer to ROOT
  macro names, histogram keys, or iteration-count implementation details unless
  they are physics-relevant (e.g. "three iterations is sufficient").
- **Author-note macro**: `\kd{...}` for Kyle's inline comments/TODOs.
- **Appendix labels**: `\label{subsec:appendix_...}` for appendix subsections.
- **No** footnotes, no acknowledgments section editing, no bibliography editing.

## Build, test, and lint

Environment setup is required before most builds:
```bash
cd /home/kdeverea/PhysicsZHadronEEC && source SetupAnalysis.sh
```
`SetupAnalysis.sh` defines `ProjectBase` and prepends `CommonCode/binary` to `PATH`;
many makefiles depend on `$(ProjectBase)`.

Build shared code first:
```bash
cd CommonCode && make
```

Each analysis module has its own makefile — `make` inside the dated folder you're
modifying (e.g. `cd MainAnalysis/20260222_EnergyExtrapolation && make`). There is no
top-level build.

Single test / smoke run: most modules expose a `TestRun` target
(`cd MainAnalysis/20240223_Zhadron && make TestRun`). Some workflows have local
quick-check scripts (`MainAnalysis/.../workflow/testAnalysis.sh`, `.../test.sh`).

No repo-wide linter config (`.clang-tidy`, `clang-format`, `ruff`, `flake8`).
Match nearby file style.

## High-level architecture

This repo is organized as **standalone, date-stamped analysis snapshots**. Major
rewrites usually create a new dated directory rather than refactoring old ones in place.

- `CommonCode/`: shared C++ utilities and data interfaces (`Messenger`,
  `CommandLine`, correction helpers). Build first so modules can link
  `CommonCode/library/Messenger.o`.
- `SampleGeneration/`: forest reduction + conversion producing skimmed ROOT trees
  (`ReduceForest.cpp` modules).
- `MainAnalysis/`: correlation and leading-track analyses (`CorrelationAnalysis.cpp`,
  `LeadingTrkAnalysis.cpp`) over reduced samples.
- `TrackingEfficiency/`: residual correction production and validation.
- `Systematics/`: systematic-harvesting/combination utilities.
- `Plots/`: plotting/post-processing executables and ROOT macro figure production.

Typical flow: **SampleGeneration → MainAnalysis / TrackingEfficiency → Systematics → Plots**.

## Key conventions

### Build / runner basics
- **Directory granularity is workflow-level**: each dated folder is self-contained with
  its own makefile/scripts.
- **Use local makefiles, not a global build**: compile/run from the target dated dir.
- **CLI pattern**: executables use `CommandLine` from `CommonCode/include/CommandLine.h`
  with long options (`--Input`, `--Output`, `--MinZPT`, etc.).
- **Chunked execution**: many `workflow/finalAnalysis.sh` scripts fan out
  `ExecuteCorrelationAnalysis` via `--nThread/--nChunk`, then merge with `hadd`.
- **`system-analysis.sh` knobs**: `CONFIG_FILE` (alternate range config),
  `CUT_PARALLELISM` (parallel `PT_RANGES`), `SKIP_CLEAN=1` for iterative reruns
  without rebuild.
- **`threader.sh` knobs**: `NTHREAD`, `NSLICE_FACTOR`, `ANALYSIS_EXECUTABLE`.
  Benchmark pp MC Gen `ZPT40_350` × `trkPT={1_2,2_4,4_10}` dropped 224 s → 76 s
  (~2.94 × / 66 %) with identical histograms.
- **NTHREAD parallelism constraints**: I/O is the binding resource — multiple legs
  that read the **same** skim file must share its 55-thread cap; legs reading
  **different** skim files can run simultaneously without I/O conflict. Hard limits:
  ≤ 55 threads per skim file; ≤ 90 total threads at any moment.
  `CUT_PARALLELISM` × `NTHREAD` = threads per file per leg; with multiple legs
  reading different files in parallel, keep `CUT_PARALLELISM=1` and maximize
  `NTHREAD`. Standard 3-leg parallel run (pp / pPb / PbP — each a different skim):
  `NTHREAD=30`, `CUT_PARALLELISM=1` → 3 × 30 = 90 total ≤ 90 ✓, 30/file ≤ 55 ✓.
- **Minimal usage**: `export SKIP_CLEAN=1; export CUT_PARALLELISM=3;
  export NTHREAD=8; ./system-analysis.sh ...` (`NSLICE_FACTOR`/`CONFIG_FILE` optional).
- **ROOT-first toolchain**: builds use `g++` + ``root-config --cflags --glibs`` and
  typically link `$(ProjectBase)/CommonCode/library/Messenger.o`.
- **Output layout**: intermediates → `output/`, merged products/plots → `plots/`,
  suffixes `-result.root` and `-nosub.root`.

### Skim inputs / dictionaries
- **V0.2 heavy-ion**: in `SampleGeneration/20250929_ReducedTreePA/V0.2/` use literal
  orientation labels — physical pPb uses `PPbData_Reco.root`, `PPbMC_Reco.root`,
  `PPbMC_Gen.root`; physical PbP uses `PbPData_Reco.root`, `PbPMC_Reco.root`,
  `PbPMC_Gen.root`.
- **V0.1 tracking-selection**: tracking-selection systematic reads the direct-label
  skims in `SampleGeneration/20250929_ReducedTreePA/V0.1/` (`P{Pb,bP}Data_Reco.root`).
- **V0.3 pp tracking-selection**: promoted `V0.3/PP{Data,MCReco,MCGen}.root` bundle
  provides pp `Tree`, `TreeLoose`, `TreeTight` skims, so the corrected-data pp
  systematic flow produces `TrackSelectionMode={Loose,Tight}` and consumes the
  dedicated pp loose/tight residual-correction files from `OfficialWeightDictionary.sh`.
- **Track-selection mode**: `ExecuteCorrelationAnalysis --TrackSelectionMode={Nominal,
  Loose,Tight}` maps to skim trees `Tree`, `TreeLoose`, `TreeTight`.
- **Official weight dictionary**: runner scripts must `source
  /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh` and consume
  `VZWeightFile_{PP,PPb,PbP}`, `ZWeightFile_{PP,PPb,PbP}`, `RWeightFile_{PP,PPb,PbP}`,
  `EEWeightFile_PP` instead of hardcoding paths. The MC-driven pp EE systematic is
  `OFFICIAL_EE_WEIGHT_FILE_PP_PRIVATE` / `EEWeightFile_PP_PRIVATE`; the nominal pp EE
  stays `EEWeightFile_PP`.
- **Official input dictionary**: source the same dictionary for
  `OFFICIAL_{MCGEN,MCRECO,EPOS,DATA}INPUT_{PP,PPB,PBP}`. The V0.3 bundle uses pp
  `PP{Data,MCReco,MCGen}.root`, pPb `PPb{Data,MCReco,MCGen}.root`, PbP
  `PbPData_Reco.root` / `PbPMC_{Reco,Gen}.root`; EPOS helpers remain on the merged
  EPOS paths.
- **Official tag convention**: `OFFICIAL_TAG_PP` / `OFFICIAL_TAG_PPB` exported from
  the dictionary. Current V0.3 campaign uses `EEV6_ZV10_trkV29_nmix10` (pp) and
  `ZV10_trkV29_nmix10` (pPb/PbP). Never invent ad hoc descriptive suffixes
  (`newVZFix`, `skimVZOff`, etc.); if physics changes, increment `ZV*` / `trkV*`.
- **Official product dictionary**: source `OfficialProductDictionary.sh` (after
  `OfficialWeightDictionary.sh`) to get canonical ROOT file prefixes for note-facing
  products. Exports: `OFFICIAL_RESULT_DIR` (the `plots/` output dir of
  `20241102_ZhadronVsZPt`), closure dirs `OFFICIAL_ZCORR_CLOSURE_DIR` /
  `OFFICIAL_TRKCORR_CLOSURE_DIR`, MC prefix variables for pp/pPb/PbP at four
  correction stages (`_GEN_`, `_RECO_`, `_ZRESIDUAL_`, `_TRKRESIDUAL_` prefixes),
  result/nosub file prefixes `OFFICIAL_{PP,PPB,PBP}_{RESULT,NOSUB}_PREFIX` (signed
  common-CM convention, 12×12, DEtaRange=3.87; uses base official tags without
  suffix) and `OFFICIAL_{PP,PPB,PBP}_{RESULT,NOSUB}_PREFIX_BIN12` (12×12 study
  surface; pp `_bin12x12_20260603`, pPb/PbP `_bin12x12_20260603`), and helper
  `assert_product_exists()` to abort if a production ROOT file is missing.
  Usage pattern: `${OFFICIAL_PPB_NOSUB_PREFIX}_ZPT0_500-nosub.root`.

### Private MC (pp Pythia+MadGraph)
- **20260403 private generator**: `SampleGeneration/20260403_PythiaMadgraph/` is the
  local pp `Z → μμ` workflow. Source `env.sh` after `SetupAnalysis.sh`; keep private
  generator installs under `~/pythia`; use `generate_madgraph.sh --Energy {5020,8160}`,
  `ExecuteShowerLHE` with `CP5.cmnd`, and `ExecuteConvertHepMC3` to produce a
  single-`Tree` `ZHadronMessenger` skim. Promoted 100 k skims:
  `output/ZMuMu_{5020,8160}_nnlo.root` (generated with `--PDFLabel lhapdf --LHAID 303600`
  = NNPDF31_nnlo_as_0118). Older `ZMuMu_*_validation.root` used inconsistent PDFs
  across energies and are superseded for note-facing work.
- **PDF convention**: `generate_madgraph.sh` accepts `--PDFLabel` (default `lhapdf`)
  and `--LHAID` (default `303600`). Always pair the same values across 5.02 and 8.16
  TeV. The `nn23lo1` built-in PDF ignores `lhaid` (uses MadGraph's internal NNPDF23
  LO); don't mix `nn23lo1` at one energy with `lhapdf` at the other.
- **MG5 runtime**: run `generate_madgraph.sh` with
  `MG5AMC_PYTHON=/usr/bin/python3.11` (default `python3` too old).
- **Private skim acceptance**: `ConvertHepMC3` keeps leaf charged particles with
  `pT > 0.5 GeV`, `|η| < 2.4` in track branches; computes `SignalHF`/`SubEvent*HF` from
  leaf particles in `3 < |η| < 5` with `pT > 0.4 GeV`; tags gen muons via recursive
  Z-ancestor lookup before filling `trackMuTagged`/`trackMuDR`. Regenerate from
  retained HepMC if the threshold changes, to stay acceptance-matched to
  `OFFICIAL_MCGENINPUT_PP` (`mergedSample/pythia-gen-v11-Zpt0.root`).
- **Closure runners**: pp validation entrypoints are
  `MainAnalysis/20241102_ZhadronVsZPt/pp-madgraphclosure.sh` (gen-mode `nmix=10`) and
  `Plots/20260404_ppClosure/{plot-comparison.sh,plot-central.sh}`. Apply both
  `EventWeight` and `TrackWeight` for official and private pp MC. Gen-level official
  comparison defaults to `OFFICIAL_MCGENINPUT_PP`, not the reco-style
  `OFFICIAL_DATAINPUT_PP`.
- **Private MC paths**: surfaced from the dictionary as `OFFICIAL_PRIVATE_PP_{5020,8160}_INPUT`
  / consumed via `PRIVATE_PP_{5020,8160}_INPUT`. The NNLO skims are LO/unweighted with
  one constant positive event weight; the MC-driven EE weight derived from them is
  `20260417_EE_nnlo.root`.
- **Private-production plot convention**: `Plots/20260404_ppClosure/plot_production.cpp`
  must treat `hZPtEtaPhi_*` in `*-nosub.root` inputs as already `1/N_Z` normalized
  (in `makeProjection.C`); divide only by bin width.

### Main analysis (20241102_ZhadronVsZPt)
- **Canonical runners**: maintained entrypoints are `closure-VZ.sh`, `closure-Z.sh`,
  `closure-trk.sh`, `central.sh`, `central-signed.sh`. Fix bugs in these directly
  rather than creating `*-newVZFix.sh` variants.
- **Signed common-CM convention** (`central-signed.sh`, `systematics-signed.sh`):
  the signed convention expresses pPb and Pbp in a common signed longitudinal frame
  before combination. Key parameters: `--FillSigned true`, `--DEtaRange 3.87`,
  `--ResultDEtaBins 12 --ResultDPhiBins 12`, `--MaxMixDeltaVZ 1.0`, `--UseJackknife true`,
  `--nMix 10`, `--yBoost 0`. Pbp uses `--FlipDeltaEta true --IsPPb false`. Fiducial
  acceptance: `|eta_cm| < 1.935` (pPb lab: `[-1.470, 2.400]`, Pbp lab:
  `[-2.400, 1.470]`, pp: `[-1.935, 1.935]`). Output 2D histograms span
  `[-3.87, 3.87]` in DeltaEta (12 equal bins) and `[-π/2, 3π/2]` in DeltaPhi
  (12 equal bins). Projection windows: DeltaPhi → x bins 7..12 (DeltaEta ≥ 0),
  DeltaEta → y bins 4..6 (DeltaPhi ∈ [0, π/2]). Tags use existing official tags
  (no extra suffix): `EEV6_ZV10_trkV29_nmix10` (pp), `ZV10_trkV29_nmix10` (pPb/PbP).
  Inclusive only: `ZPT 0_500`, `trkPT 0.5_15`.
- **Signed convention symmetrization**: after combining pPb + Pbp (N_Z-weighted sum
  of 2D nosub histograms, then background subtraction), apply fourfold 2D
  symmetrization before projecting to 1D. Mirror indices (1-indexed, 12×12):
  DeltaEta: `mi = 13 - i`; DeltaPhi near-side: `mj = 7 - j` for `j = 1..6`,
  away-side: `mj = 19 - j` for `j = 7..12`. Formula:
  `R_sym(i,j) = 0.25 * (R(i,j) + R(mi,j) + R(i,mj) + R(mi,mj))`. After projecting
  to 1D, the projected histograms are already symmetric. 1D symmetrization helper
  functions (`Symmetrize1DEta`, `Symmetrize1DPhi`) are applied to individual system
  results (pp, single pPb, single Pbp) that don't go through the 2D combine path.
  1D DeltaEta mirror (0-indexed): `mirror(i) = n-1-i`. 1D DeltaPhi mirror
  (0-indexed, 12 bins): near-side `mirror(j) = 5-j` for `j=0..2`, away-side
  `mirror(j) = 17-j` for `j=6..8`. 1D formula:
  `h_sym[k] = 0.5*(h[k] + h[mirror(k)])`,
  `err = 0.5*sqrt(e[k]^2 + e[mirror(k)]^2)`.
- **Signed convention statistical uncertainties**: jackknife resampling is computed
  per-system (pPb and Pbp independently). For display on result and overlay plots,
  only the diagonal variance is used (no covariance). Full covariance is retained
  for the chi-squared compatibility tests in `20260506_Jackknife/`. Combined pPb
  stat errors propagated via:
  `σ²_comb(i) = w²_pPb σ²_pPb(i) + w²_Pbp σ²_Pbp(i)` where
  `w_sys = N_Z,sys / (N_Z,pPb + N_Z,Pbp)`. Through symmetrization:
  `σ²_sym(i) = 0.25*(σ²_comb(i) + σ²_comb(mirror(i)))`. This diagonal
  approximation is conservative: it overestimates the symmetrized error because the
  neglected partner-bin covariance is positive (shared N_Z normalization). pp uses
  the same diagonal JK → symmetrize pipeline independently.
- **Signed convention end-to-end pipeline** (to reproduce all note-facing signed
  products from scratch):
  ```
  # 1. Production (corrected data + systematics)
  cd MainAnalysis/20241102_ZhadronVsZPt
  NTHREAD=25 ./central-signed.sh          # nominal pp, pPb, PbP
  NTHREAD=25 ./systematics-signed.sh 1 1 1 # all systematic variations

  # 2. TrackCorrection fix (MUST use nominal residual, not dedicated TC residual)
  bash rerun-trackcorrection-signed.sh     # pp, pPb, PbP × TC0p976/TC1p024

  # 3. Systematics harvesting
  cd ../../Systematics/20260329_pPbSystematics
  SYSTEMS=pp,pPb,PbP,pPbPbp ZPT_RANGES=0_500 TRACK_RANGES=0.5_15 bash run.sh
  # Also run standalone comparison plots:
  SYSTEMS=pp,pPb,PbP,pPbPbp ZPT_RANGES=0_500 TRACK_RANGES=0.5_15 bash run-track-correction.sh

  # 4. pPb-vs-Pbp overlay (Phase 1 verification)
  cd ../../Plots/20260213_Central
  bash plot-central-overlay-PPbPbP.sh      # → plots/central_overlay_PPbPbP/

  # 5. Result plots (Phase 3)
  USE_MODIFIED_12x12=true PLOT_INCLUDE_MC=false USE_SYSTEMATICS=true \
    bash plot-central-combined.sh          # → plots/central_combined/

  # 6. Copy to Overleaf
  # overlay → ~/OverleafZHadronInPPb/figures/analysis/combining/
  # result  → ~/OverleafZHadronInPPb/figures/result/
  # sys     → ~/OverleafZHadronInPPb/figures/sys/
  ```
- **DEtaRange CLI parameter**: `CorrelationAnalysis.cpp` accepts `--DEtaRange <double>`
  (default 4.0). Creates 12 equal-width DeltaEta bins from `[-range, +range]`.
  `makeProjection.C` validates both 3.87 and 4.0 as acceptable DeltaEta edges.
- **Systematics runner**: `systematics.sh` enumerates corrected-data variations. pp:
  `Loose`, `Tight`, `IsMuTaggedFalse`, `IsPURejectTrue`, `MuVar0..3`, inclusive
  `TrackCorrection0p976/1p024`, `_EEPrivate`. pPb/PbP: same minus `_EEPrivate`.
  Loose/Tight/`IsMuTaggedFalse` consume their dedicated residual corrections from the
  dictionary; `IsPURejectTrue` consumes the dedicated PU-stack VZ/Z/track residuals.
  Narrowed `CONFIG_FILE` blocks must still include `TrackCorrection0p976/1p024` for
  all three collisions, else reruns leave stale track-correction variations behind.
  pp PU family: `NVertex == 1` is a corrected-data selection only — do not impose on
  pp MC legs of VZ/Z/residual derivations.
- **Signed systematics runner**: `systematics-signed.sh` mirrors `systematics.sh` but
  adds `FillSigned`, `FlipDeltaEta` (Pbp), acceptance cuts, `DEtaRange 3.87`. Same
  variation families as the unsigned runner. Usage:
  `NTHREAD=25 ./systematics-signed.sh <DOPP> <DOPPB> <DOPBP>`.
- **V0.3 muon-rejection residual**: corrected-data muon-rejection systematic uses the
  dedicated `IsMuTaggedFalse` residual/data branches for pp/pPb/PbP. Nominal stays at
  skim default `--IsMuTagged true` (`trackMuDR = 0.0025`). The `TrackMuDR0p004` family
  is only a study surface, not the note-facing variation.
- **PU-selection in `CorrelationAnalysis.cpp`**: across main analysis, Z correction,
  and residual correction, the PU veto must be
  `if (par.isPUReject && par.isData && b->NVertex != 1) return 0;` — data-only, no
  effect on MC derivation legs.
- **Narrowed-test convention**: when `CONFIG_FILE` is supplied externally,
  `central.sh` / `systematics.sh` honor it and skip their built-in second range block.
- **|vz| window**: VZ reweighting production runs **without** the analysis `|vz| < 15`
  cut; all subsequent correction/analysis workflows default to enforcing `|vz| < 15`
  unless a VZ-study runner sets `--UseVZWindow false`.
- **MaxMixDeltaVZ default**: `CorrelationAnalysis.cpp` defaults to `MaxMixDeltaVZ=1.0`,
  applying a `|ΔVZ| < 1.0 cm` requirement between signal and mixed events in the mixing
  loop. This cut applies to ALL analysis curves — Gen, RECO, and every correction level.
  Never pass `--MaxMixDeltaVZ 0` to disable it unless explicitly studying the effect.
  (Changed from 0.5 cm: the 0.5 cm cut non-monotonically amplified pPb/PbP DeltaEta
  tension due to opposite-beam Vz asymmetry; 1.0 cm restores good agreement.)
- **Energy-extrapolation systematic**: `systematics.sh` is the maintained pp
  corrected-data entrypoint for `_EEPrivate` — swap only
  `EEWeightFile_PP_PRIVATE` for the nominal EE weight while keeping the official pp
  tag family unchanged.
- **20251211 residual systematics runner**: in
  `MainAnalysis/20251211_ResidualCorrection/workflow/`, common wrappers are
  `run-pp-systematics.sh` and `run-pPb-systematics.sh`, covering `Loose`, `Tight`,
  `IsMuTaggedFalse`, `TrackCorrection{0p976,1p024}`. PU residual family stays in
  `run-pp-pu.sh` / `run-pPb-pu.sh`; the pp wrapper calls `run-pp-pu.sh` explicitly.
- **20251211 capped-pt residual**: propagate the cap via `TRACK_PT_MAX` /
  `RESIDUAL_HISTOGRAM_MAX_TRACK_PT` in `workflow/pythia-analysis.sh` so RECO, GEN,
  iterative correction, and final closure share the range. Upper bound is exclusive
  (`trackPt >= MaxTrackPT` rejected) so clipped values don't leak into x-overflow;
  correction/closure pt plot macros read range from the histogram axis, not hardcoded
  legacy limits.
- **20251211 residual systematic policy**: in
  `workflow/{pythia-analysis.sh,pPb-DY-analysis.sh}`, route MC CLI extras through
  explicit buckets — shared RECO+GEN: `IsMuTagged`, `TrackMuDR`, `TrackMuClosest`;
  RECO-only: `TrackSelectionMode`, `TrackExtraWeight`. `IsPUReject` stays off MC
  residual derivation: PU residual family is defined by alternate VZ/Z weights while
  the direct PU veto remains data-only.
- **20251211 residual muon-rejection**:
  `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp` mirrors the
  main-analysis `rejectMuonMatchedTrack(...)` semantics — explicit `TrackMuDR` first,
  then `TrackMuClosest`, then boolean `IsMuTagged`. `TrackMuDR`/`TrackMuClosest`
  apply to MC RECO and MC GEN.

### 2D residual correction (20260518_ResidualCorrection2D)
- **Overview**: replaces the triple-1D (`pT × η × φ`) track residual correction with a
  `pT × (η,φ)` correction — one 1D ratio for `pT` and one simultaneous 2D ratio for
  `(η, φ)`, preserving angular correlations. Three iterations, same as 1D. Promoted at
  `trkV29`; the 1D workflow at `20251211_ResidualCorrection` is kept as a fallback.
- **Header**: `CommonCode/include/TrackResidualCorrector.h` exports both
  `TrackResidualCorrector` (1D, still used by `EnergyCorrector`/`Zcorrector`) and
  `TrackResidualCorrector2D` (reads `hPtCorrTotal` TH1D + `hEtaPhiCorrTotal` TH2D,
  returns product correction). Main-analysis `CorrelationAnalysis.cpp` dispatches to
  `TrackResidualCorrector2D` when the weight file contains `hEtaPhiCorrTotal`.
- **Correction ROOT layout**: each file contains `hPtCorrTotal` (TH1D, 30 bins
  0.5–15 GeV) and `hEtaPhiCorrTotal` (TH2D, 48 η × 48 φ bins). Iteration-level
  histograms `hPtCorr_iter{0,1,2}` and `hEtaPhiCorr_iter{0,1,2}` are also stored for
  diagnostic plots.
- **Workflow**: `MainAnalysis/20260518_ResidualCorrection2D/workflow_2D/` mirrors the
  1D workflow structure. Runners: `run-pp.sh`, `run-pPb.sh`, `run-pp-systematics.sh`,
  `run-pPb-systematics.sh`, `run-pp-pu.sh`, `run-pPb-pu.sh`. Iteration scripts:
  `correction_2D.C` (per-iteration 2D correction), `merge_corrections_2D.C`
  (element-wise product across iterations). Closure inputs stored in
  `output/closure_inputs/`.
- **Closure plots**: `Plots/20251202_trackResidualClosure/` generates 1D
  `pt`/`eta`/`phi` closure PDFs plus 2D `eta-phi-{corrected,gen,ratio}` heat maps from
  `workflow_2D/output/closure_inputs/`. The correction convergence PDFs
  (`corrections_2D_{system}_{tag}_{ZPT}.pdf`) are produced automatically by each run
  script via `plot_corrections_2D.C`; to refresh standalone, run
  `INPUT_TAG=<tag> bash Plots/20251202_trackResidualClosure/plot-track.sh`.
- **ZCorrector in 2D residual workflow**: `MainAnalysis/20260518_ResidualCorrection2D/CorrelationAnalysis.cpp`
  uses `ZCorrector *Zcorrector` with the 2-arg `GetCorrectionFactor(zPt, zY)` form.
  ZV10 Z weight files are 2D (pT × η) and contain only `hPtCorrTotal` + `hEtaCorrTotal`
  — no `hPhiCorrTotal`. The old 3-arg `TrackResidualCorrector` would segfault on these
  files. Do not revert to 3-arg without re-introducing `hPhiCorrTotal` in the Z weight files.
- **Official weight paths**: 72 files at
  `workflow_2D/output/20260601_ZV10_trkV29_TrackResidualCorrection_*` (24 per system:
  4 nominal + 20 systematic). Keyed in `OfficialWeightDictionary.sh` as
  `OFFICIAL_R_WEIGHT_FILE_{PP,PPB,PBP}[_variant]`. PU-reject R weights are not computed
  for ZV10; the `IsPURejectTrue` block is commented out in `run-pp-systematics.sh` and
  `run-pPb-systematics.sh`.
- **Inclusive ZPT0-500 closure inputs**: produced by hadd-ing the 4 ZPT bin closure
  inputs from `output/closure_inputs/` for each system × type (gen/reco/corrected).
  Required to generate the `*_ZPT0_500_*-nosub-closure-pt-{eta,phi}-*.pdf` 2D heat maps
  used in the pt–η/φ correlation appendix. Run `ExecuteClosureTest --zPtRange 0_500`
  after creating these combined files.

### TnP weights
- **20250929 skimmer TnP**: pp / PA MC keep `MZHadron.ZWeight = 1` and `ExtraZWeight = 1`.
  pp data nominal `ZWeight` comes from a dedicated local pp dimuon TnP helper
  (nominal pp TightID × TightID × double-trigger, not `CommonFunctions` Z-shape
  helpers). PA data uses the local pPb dimuon TnP helper with `ExtraZWeight[0..3]`
  reserved for ID up/down and trigger up/down. PA TnP helpers apply only when both
  muons stay in the documented high-pT window (`20 ≤ pT < 200 GeV`, `|η| < 2.4`);
  otherwise keep weights at 1.

### Systematics workspace (20260329_pPbSystematics)
- **Harvesting**: compiled workspace reading final corrected
  `*_trkResidual_<official-tag>_ZPT*-result.root`. Applies the quoted `0.5`
  normalization consistently for note-facing central values and systematic
  magnitudes. Keeps only `TrackSelection`, `TrackCorrection`, `MuonRejection`,
  `PUpp`, `PUpPb`, `ScaleFactor`, `EnergyExtrapolation`, `Total`. `MuonRejection`
  uses corrected-data `IsMuTaggedFalse`; pileup is collision-specific (`PUpp` for pp,
  `PUpPb` for pPb/PbP); tracking-correction is built bin-by-bin from
  `TrackCorrection0p976`/`1p024`, not a flat constant.
- **Combined `pPbPbp`**: keep separate `pPb` and `PbP` outputs and also produce a
  combined `pPbPbp`. Combined nominal/variation histograms are formed from
  `hData_*`, `hMixData_*`, `hNZData_*`, `hNZMixData_*` by undoing per-system `N_Z`
  normalization, summing pPb + Pbp, renormalizing by summed `N_Z`, projecting, then
  applying the extra `0.5` before bin-by-bin deviations. Combined runners (`run.sh`,
  `run-pu.sh`, `run-track-selection.sh`, `run-track-correction.sh`) feed the helper
  paired `*-nosub.root` for `pPbPbp` — using `*-result.root` inflates the
  reconstructed combined central values. Note-facing combined label is `pPb`, not
  `pPb + Pbp`. `run.sh` passes `--UseModified12x12 true` for the `pPbPbp` system so
  `BuildCombinedResultHistogram` projects the 2D nosub histograms using the 12×12
  projection windows. `ValidateModified12x12Histogram` in
  `ResultCombinationUtilities.h` accepts both DEtaRange=4.0 and DEtaRange=3.87.
- **pp/heavy-ion difference systematics**: keep `pPb` and `PbP` fully correlated inside
  the `pPbPbp` builder, but treat combined heavy-ion and pp as uncorrelated when
  building `Difference*` families. The `Difference<Family>_*` path takes bin-by-bin
  combined HI magnitude, matching pp magnitude, and combines as
  `sqrt(Δcombined^2 + Δpp^2)` with missing sides treated as zero. Do not rebuild a
  varied `(pPbPbp - pp)` and compare to nominal difference.
- **`run.sh` overlays**: `INCLUDE_FAMILIES` controls which families enter the ROOT
  output; `PLOT_FAMILIES` controls only the overlay PDFs. Defaults omit
  `MuonRejection` from the harvested total and overlay legends while keeping the
  `IsMuTaggedFalse` input hooks available for explicit re-enable.
- **EnergyExtrapolation family**: pp defaults harvest+plot from the nominal vs
  `_EEPrivate` corrected-data difference. Standalone note-facing comparison is
  `run-energy-extrapolation.sh` with the variation labeled `MC-driven`.
- **Per-observable plotter**: per observable, emit the absolute overlay
  (`-Delta{Phi,Eta}-absolute.pdf`), the relative overlay
  (`-Delta{Phi,Eta}-relative.pdf`, absolute uncertainty ÷ absolute nominal bin
  content), and the central-value plot (`-Delta{Phi,Eta}-central.pdf`) with total
  systematic uncertainties as bin errors.
- **Standalone comparison runners**: `run-pu.sh`, `run-track-selection.sh`,
  `run-track-correction.sh`, `run-muon-track.sh` are maintained standalone runners
  for PU, loose/nominal/tight, track-correction, and muon-track figures. They read
  corrected-data nominal+variation `*_trkResidual_<official-tag>_*_ZPT*-result.root`
  from `MainAnalysis/20241102_ZhadronVsZPt/plots/`, write PDFs to
  `plots/{pu,trackSelection,trackCorrection,muonTrack}/`, and emit separate `pPb` /
  `PbP` plus combined `pPbPbp` by default. Both standalone comparisons and the
  compiled systematic path apply the quoted `0.5` normalization to
  `Delta{Phi,Eta}_Result<trkPT>`. Label the extrapolated pp reference as
  `pp (8.16 TeV)` and the combined `pPbPbp` overlay as `pPb (8.16 TeV)`. PU and
  track-selection use overlay+difference (`variation - nominal`); track-correction
  uses nominal/0.976/1.024. Muon-track workflow uses `systematics-trackMuDR.sh`
  (`TrackMuDR0p001`, `0p0025`, `0p0035`, `0p004`, `TrackMuClosestTrue`) with
  `run-muon-track.sh` driven by `SYSTEMS`, `ZPT_RANGES`, `TRACK_RANGES`,
  `VARIATION_TAGS`, `VARIATION_LABELS`. With skim overlap, set `SKIP_CLEAN=1`
  upstream and avoid `clean.sh`/`CommonCode` rebuilds. Muon-track lower panels fixed
  at `[-0.2, 0.2]`; `DeltaPhi` top panels need extra headroom for note figures.
- **20260407 non-pp TrackMuDR equivalence**: in V0.2 pPb/PbP data, `trackMuTagged`
  matches `trackMuDR >= 0 && trackMuDR < 0.0025` exactly. If a non-pp nominal vs
  `TrackMuDR0p0025` disagrees, first check if nominal corrected-data outputs are
  stale relative to the variation; refresh narrowed nominals before suspecting a
  downstream code bug.

### Result / paper plots (20260213_Central)
- **Combined-result plot**: `plot-central-combined.sh` sources the dictionary for
  `OFFICIAL_TAG_PP`/`OFFICIAL_TAG_PPB`, accepts `CONFIG_FILE` for arbitrary Z/track
  grids, honors `PLOT_OUTPUT_BASE` for the `<OFFICIAL_TAG_PP>/` PDF staging dir, and
  passes `PLOT_INCLUDE_MC={true,false}` to `plot_central_combined.cpp`. Always reads
  corrected `pp_trkResidual_*`, `pPb_trkResidual_*`, `PbP_trkResidual_*` central
  outputs from `MainAnalysis/20241102_ZhadronVsZPt/plots/`. When `PLOT_INCLUDE_MC=true`,
  also keeps the Powheg+EPOS MC legs from `pPbMC_Gen_nominal_*` and
  `PbPMC_Gen_nominal_*`. `PLOT_INCLUDE_MC=false` is the data-only fallback for
  note/presentation scans.
- **Pre-subtraction overlay**: `plot-central-sb.sh` + `plot_central_sb.cpp` follow the
  `Plots/20260120_CentralClosure/plot_closure.cpp` style, read `pPb_trkResidual_*` /
  `PbP_trkResidual_*` `-nosub.root` inputs, use inclusive defaults `ZPT0_500`,
  `trkPT0.5_15`, and the signed `#Delta y_{ch,Z}` / `#Delta#phi_{ch,Z}` axes from
  `Delta{Eta,Phi}_Result<trkPT>` with the mixed projections from `hMixData_<trkPT>`,
  writing four data-only `Delta{Eta,Phi}-{all,bkg}.pdf` overlays to
  `plots/central_sb/`.
- **Result-style**: `KylesPlotting.h` `PlotCMSDiffResult(...)` lower pads default to
  point + statistical + systematic only — no base `HIST` line before lower-panel
  markers. Note-facing reruns of inclusive/scan result plots show points with stat
  bars + systematic band, no extra vertical histogram-bar look.
- **Paper plots**: `plot_paper.cpp` + `plot-paper.sh` produce four draft-paper PDFs
  in `plots/paper/<pp_tag>/` (`paper-Delta{Phi,Eta}-{inclusive,scan}.pdf`). Inclusive
  uses `PlotCMSPaperDiffResult` (pixel-based fonts, no auto CMS header, `labelScale`).
  Scan uses a manual 12-pad 2 × 3 grid (rows ZPT {0_30, 30_500}; cols trkPT
  {0.5_2, 2_4, 4_15}). Header: bold "CMS" (no "Preliminary") top-left,
  `pPb (pp) 8.16 TeV 174 nb^{-1} (301 pb^{-1})` top-right. Data loads via the same
  `LoadBin()` combine helper as `plot_central_combined.cpp`.
- **Note-facing bins**: maintained scan defaults for result, systematics, and pp
  energy-extrapolation are `ZPT={0_30,30_500}` × `trkPT={0.5_2,2_4,4_15}`, plus the
  inclusive `ZPT0_500`, `trkPT0.5_15`. Older `5_30`, `0.5_4`, `4_500`, `0.5_500`
  defaults are stale.
- **Modified-bin surfaces**: a shifted `10 × 10` surface remains as an older
  semi-official study (tags `*_bin10x10shifted_20260506`) — keep its runners and
  exploratory presentation but prefer the newer `12 × 12` set for note Results. The
  maintained `12 × 12` surface uses tags
  `EEV6_ZV10_trkV29_nmix10_bin12x12_20260602` (pp) and
  `ZV10_trkV29_nmix10_bin12x12_20260603` (pPb/PbP; `_20260602` for pp since pp was not
  rerun in the 2026-06-03 MaxMixDeltaVZ=1 cm campaign). Refresh the inclusive
  `ZPT0_500`/`trkPT0.5_15` plus six scan bins through
  `MainAnalysis/20241102_ZhadronVsZPt/{result-study-bin12x12-20260507.sh,
  systematics-bin12x12-20260507.sh}`,
  `Systematics/20260329_pPbSystematics/run-bin12x12-20260507.sh`,
  `Plots/20260213_Central/plot-central-combined-bin12x12-20260507.sh`, then stage the
  seven-slide presentation as
  `Plots/20260213_Central/presentations/central_combined_bin12x12_20260507.pdf`.
- **Modified-bin Overleaf**: `12 × 12` updates copy the inclusive PDFs
  `all_ZPT0_500_trkPT0.5_15_*_bin12x12_20260602-{DeltaPhi,DeltaEta}-result.pdf` (pp) /
  `20260603` (pPb/PbP) and the scan composites
  `overleaf_pdf_composites/overleaf_result_scan_bin12x12_{deltaphi,deltaeta}_combined.pdf`
  into `~/OverleafZHadronInPPb/figures/result/`, preserving source basenames and
  replacing older `10 × 10` additions in `src/results.tex`.
- **Projection windows**: result-stage projections are hard-coded by surface, not
  auto-detected. Official `20 × 20` keeps `DeltaPhi: x bins 0..10`,
  `DeltaEta: y bins 6..10`. Shifted `10 × 10`: `DeltaEta` bins `-4..4` and `DeltaPhi`
  bins `-3π/5..7π/5`, with exact axis edges at `DeltaEta = 0, 4` and `DeltaPhi = 0, π`;
  project with `DeltaPhi: x bins 6..10` (`0..4` in η) and `DeltaEta: y bins 4..8`
  (`0..π` in φ). `12 × 12`: `DeltaEta` bins `-4..4` and `DeltaPhi` bins `-π/2..3π/2`,
  with exact edges at `DeltaEta = 0, 4` and `DeltaPhi = 0, π/2, π`; project with
  `DeltaPhi: x bins 7..12` (`0..4`) and `DeltaEta: y bins 4..6` (`0..π/2`).
- **Jackknife note plots**: maintained note-facing result and pPb-vs-PbP comparison
  plots use jackknife statistical uncertainties by default. `central.sh` and the
  `10 × 10` result-study runner pass `--UseJackknife true` on corrected-data result
  legs; the pPb-vs-PbP compatibility recalculation lives in
  `MainAnalysis/20260506_Jackknife/`; `plot-central-overlay-PPbPbP.sh` defaults to the
  finalized note bins (`0_30`, `30_500`, `0_500` × `0.5_2`, `2_4`, `4_15`, `0.5_15`).
- **pPb/PbP compatibility tests**: `MainAnalysis/20260506_Jackknife/` provides two
  complementary tests via `run-pPbPbp-compatibility.sh` and `run-pPbPbp-ks.sh`.
  Both read the 12×12 surface result files tagged by `TAG12` (default from
  `OfficialProductDictionary.sh`).
  - *Chi-squared test* (`ExecuteCompatibility`): reads `JackknifeProjection0.5_15`
    TTree from each `*-result.root`; builds N×N covariance matrix from
    leave-one-Z-event-out resampling; inverts via SVD Moore-Penrose pseudoinverse;
    reports p-values for three error models: `Sumw2` (diagonal stat), `Diagonal`
    (jackknife diagonal only), `FullCovariance` (full N×N matrix). DeltaEta ndf=6
    (symmetry halves the SVD rank). Use `--EtaFirstBin 6 --EtaLastBin 11
    --PhiFirstBin 3 --PhiLastBin 8` for the 12×12 projection window. Outputs
    `output_12x12/pPbPbp_compatibility.tsv` and two correlation-matrix PDFs.
  - *KS test* (`ExecuteKS`): computes D_KS = max|CDF_pPb − CDF_PbP| between
    normalized cumulative distributions. The p-value is derived from the asymptotic
    Kolmogorov distribution using weighted Z-event counts as effective sample sizes.
    This is a heuristic for weighted, background-subtracted observables (not a
    calibrated KS p-value), but it is sensitive to systematic offsets that persist
    across bins — complementary to the chi-squared shape test. Outputs
    `output_12x12/pPbPbp_ks.tsv`.
  - Note: the 0.5 cm `MaxMixDeltaVZ` cut non-monotonically worsened pPb/PbP
    DeltaEta FullCov agreement (p=0.017) relative to no cut (p=0.28) or 1 cm (p=0.35)
    due to opposite-beam Vz asymmetry; the default was changed to 1.0 cm.
- **pp energy-extrapolation study**: `MainAnalysis/20241102_ZhadronVsZPt/pp-EE.sh` /
  `Plots/20260213_Central/plot_energyExtrapolation.cpp` run on the finalized
  note-facing bins, include the maintained pp VZ weighting in all curves, apply the
  energy weight multiplicatively to the pp corrected-data leg, and label the four
  overlays as baseline, `+ Z correction`, `+ Z and track correction`,
  `+ Z, track, energy correction`.
- **Dimuon-mass ratio**: `plot_zmass.cpp` uses the standard CMS ratio helper so the
  default PDF includes a bottom ratio panel. Top panel keeps the data/simulation
  overlay, ratio baseline is data; show at least the scaled `MC Reco / data` ratio
  in `0.5–1.5` y-range (scaled `MC Gen / data` may appear alongside).

### Diagnostic / temp study (20260216_temp)
- **Setup**: `MainAnalysis/20260216_temp/` sources the dictionary, uses official pp
  merged inputs plus direct-label V0.1 PA inputs. ROOT outputs in `output/`, PDFs in
  `plots/`. For note-facing track-muon refreshes use
  `CIRCLE_RADII=0.0025,0.004 ./plot-MuTrk.sh ...` (nominal + variation circles
  without changing the default code path).
- **Muon-map interpretation**: `hDeltaRMuTrk` shares the maintained muon-rejection
  choice (`IsMuTagged`, `TrackMuDR`, `TrackMuClosest`) and track pt/η acceptance, but
  is **only** an occupancy diagnostic — no residual weights, event/Z/VZ/energy
  weights, mixed-event subtraction, or full note normalization, so it is not a
  predictor of the final `IsMuTaggedFalse` systematic magnitude.
- **pp `IsMuTaggedFalse` forensic**: for pp `ZPT30_500`, `trkPT4_15`, the pp data skim
  and current temp map both give zero tracks inside `DeltaR < 0.0025`; the surviving
  nominal vs `IsMuTaggedFalse` difference comes from the dedicated `IsMuTaggedFalse`
  residual-weight family, not direct near-muon occupancy. Always run a narrow
  no-residual diagnostic before calling a remaining pp effect a genuine occupancy
  signal.
- **Raw-forest auto-detect**: `scan_muon_track_rectangle.cpp` raw-forest reco/reco mode
  auto-detects `ppTrack/trackTree` vs `PbPbTracks/trackTree`. Optional
  `--RawTriggerAND=<prefix1>,<prefix2>,...` requires each trigger family via nonzero
  `TriggerTreeMessenger::CheckTriggerStartWith(...)`.
- **Old-skims replay**: replay alternate pp / pPb / PbP data, reco-MC, gen-MC skim
  bundles via `*_INPUT_OVERRIDE` hooks (plus `PPB_EPOS_INPUT_OVERRIDE` /
  `PBP_EPOS_INPUT_OVERRIDE` for PA gen). Route replayed ROOT/PDFs through
  `output/<subdir>/` and `plots/<subdir>/`. The old-`v6` replay reads
  `mergedSample/{pp,pythia,pythia-gen,HISingleMuon,DY,DY-gen}-v6.root` with outputs
  isolated under `output/oldskims/` / `plots/oldskims/`. Missing old-PA UE
  `trackMuTagged` / `trackMuDR` get `false` / `-1`.

### pPb/PbP combining sandbox (20260414_pPbPbpCombining)
- **Pooled JK combination + 2D symmetrization**:
  `plot_12x12_pooled_symmetrized.cpp` + `run-pooled-symmetrized.sh` implement the
  signed common-CM combination pipeline. Pools pPb + Pbp event-level jackknife
  entries into a single leave-one-out estimator, applies fourfold 2D symmetrization
  (DeltaEta mirror: `i ↔ 13-i`; DeltaPhi mirror on `[-π/2, 3π/2]`: near-side
  `j ↔ 7-j` for `j ≤ 6`, away-side `j ↔ 19-j` for `j > 6`), then projects to 1D
  and computes jackknife covariance. Reads nosub + raw (Jackknife2DData) files from
  `20241102_ZhadronVsZPt/output/`. Outputs comparison plots and TeX p-value table to
  `plots/pooled_symmetrized/`.
- **Sandbox scope**: self-contained pPb/PbP comparison. Observables: trkPt, trkEta,
  ZPt, Zy, Mult, DEta, DPhi, DEtaDPhi 2D ratio, presentation-only ZEtaPhi 2D ratio. No
  DR. `trackCharge` is empty in V0.2/V0.3 skims (`ReduceForest` never fills it).
  Drivers: `runner.sh` (6 correction levels × 2 PU = 24 runs), `run_z_correction.sh`
  (direct 2D pPb → Pbp Z correction), `runner_with_zcorr.sh`, `_chunk${N}.root`
  naming for hadd safety. Presentations in `presentations/`.
- **Z correction**: maintained Z-data correction is the direct 2D pPb → Pbp event
  weight from `hZYPhiData` with `+VZ+Z+ZSF` already applied. `run_z_correction.sh`
  reads `output/pPb_VZ_Z_ZSF.root` and `output/PbP_VZ_Z_ZSF.root`, stores both the
  raw ratio and the multiplicative `hWeightToApply = Pbp/pPb` in
  `output/z_correction_final.root`. Followup uses 24 `y_CM` bins (× 12 φ bins) in
  `CorrelationAnalysis.cpp` for `hZYData`, `hZYPhiData`, and the plotted `Zy`
  closure. Prompts saying `(Z η, Z φ)` should be resolved to `(Z y_CM, Z φ)` —
  document the wording resolution in the zcorr presentation/summary.
- **Run averages**: `tabulate-run-average-deta.sh` + `tabulate_run_average_deta.cpp`
  produce the run-by-run `|DeltaEta|` tabulation; default selection is inclusive
  `ZPT0_500`, `trkPT0.5_15` data with `Event`+`VZ`+`Z`+`ZSF`+`Track`+`Residual` weights
  but no sandbox `ZCorrectionFile`; output table at
  `output/run_average_abs_deta_no_zcorr.tsv`.
- **Run-consistency**: `analyze-run-average-deta.sh` refreshes the table, writes the
  markdown chi-square assessment to
  `output/run_average_abs_deta_no_zcorr_consistency.md`, and writes
  `plots/run_average_abs_deta_no_zcorr_by_run.pdf` (equally spaced run order, Pbp
  first then pPb, actual run numbers as tick labels). Central value is the weighted
  mean over tracks; per-run σ treats each selected Z event as independent
  (`X_e = Σ_t w_et`, `Y_e = Σ_t w_et x_et`,
  `σ_run = sqrt(Σ_e (Y_e - x̄ X_e)²) / Σ_e X_e`). `effective_tracks` may be reported
  as a track-level diagnostic; runs with < 2 selected events with accepted tracks
  are excluded from `χ²/ν`.
- **2D map**: `|ΔEta|` and `|ΔPhi|` 1D histos stay on the original 12-bin
  presentation binning; `hDEtaDPhiData` uses 120 × 120 bins (10 × the 12-bin map per
  axis); keep the Pbp/pPb `(|ΔEta|, |ΔPhi|)` ratio panels for all 6 correction levels
  and PUReject variants; `ZEtaPhi_ratio_*` / `zcorr_ZEtaPhi_ratio_*` panels are
  separate presentation panels and do not replace the maintained `(Z y_CM, Z φ)`
  correction/closure surface.
- **MC companion**: `runner_mc.sh` + `plot-mc.sh` +
  `presentations/pPbPbp_mc_sandbox_presentation.tex` use
  `OFFICIAL_MCRECOINPUT_{PPB,PBP}` with official `VZ`/`Z`/`R` weight files. MCReco
  skims carry `ZWeight = 1`, so the `+ZSF` stage is a kept no-op only to match naming;
  no PUReject branch here, and `plot_comparison.cpp` skips missing correction levels.
- **MC run-average**: `analyze-run-average-deta-mc.sh` reads V0.3
  `OFFICIAL_MCRECOINPUT_{PPB,PBP}`, writes
  `output/run_average_abs_deta_mc_no_zcorr.{tsv,_consistency.md}` and
  `plots/pPbPbp_run_average_abs_deta_mc_by_run.pdf`. Both systems currently have
  `run = 1`, so the plot uses system-prefixed tick labels (`Pbp:1`, `pPb:1`) rather
  than implying a multi-run spread.
- **pp run-average**: `analyze-run-average-deta-pp.sh` reads `OFFICIAL_DATAINPUT_PP`,
  uses official pp `VZ`/`Z`/`R` weights, writes
  `output/run_average_abs_deta_pp.{tsv,_consistency.md}` and
  `plots/run_average_abs_deta_pp_by_run.pdf`. Same uncertainty + inverse-variance
  `χ²/ν` definitions as pPb/Pbp; pp summary reports the single `pp only` grouping.
- **Run half-splits**: pPb half-split keeps PbP at full stats and splits the V0.3 pPb
  skim at `Run < 286302` vs `≥ 286302`. PbP half-split symmetrically splits at
  `Run < 285549` vs `≥ 285549` via `runner_PbP_{first,second}_half.sh` /
  `plot_PbP_{first,second}_half.sh`. Outputs and presentations staged under matching
  subdirectories.
- **Extremal-run followup**: driven from `output/run_average_abs_deta_no_zcorr.tsv`
  with singled-run selections pPb max/min `Run 286327` / `286441` and Pbp max/min
  `Run 285718` / `285537` via `runner_extremal_runs.sh`
  (`--MinRun run --MaxRun run+1` on the relevant collision) +
  `plot-extremal-runs.sh` + `plot_extremal_runs.cpp`. Outputs under
  `output/<sys>_run<N>/` and `plots/extremal_runs/`; presentation
  `presentations/pPbPbp_extremal_runs_presentation.pdf`.

## Jewel MC samples (`SampleGeneration/20260521_Jewel`)

- **Generator**: `~/Jewel/jewel-2.4.0/jewel-2.4.0-vac` (vacuum, pp) and `jewel-2.4.0-2D` (medium, pPb).
  Requires `export LD_LIBRARY_PATH=/home/kdeverea/Jewel/lhapdf/lib` and
  `export LHAPATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current`.
- **Process**: `PROCESS PPZJ` with `PTMIN 15`, `PTMAX 1200`, `SQRTS {5020,8160}`, `NPROTON 1`,
  `WEXPO 4.5`, `HADRO T`, `MASS 1.`, `KEEPRECOILS T`, `WRITESCATCEN T`, `WRITEDUMMIES T`.
- **Canonical skims** (do not delete without user confirmation):
  - `output/jewel_pp_8160.root` — 1.89M events, pp 8.16 TeV vacuum (pre-existing)
  - `output/jewel_pp_5020.root` — 1.42M events, pp 5.02 TeV vacuum (NEVENT 1500000)
  - `output/jewel_pPb_8160.root` — 1.03M events, pPb 8.16 TeV 2D medium (100 Ncoll bins × 15K events)
- **HepMC format**: Jewel writes flat HepMC2 E/P-line format. `ConvertHepMC.cpp` uses a direct text
  parser (no HepMC3 library). pPb bins are merged with `cat` before conversion since the converter
  reads a single file.
- **Fortran path limit**: `MEDIUMPARAMS` path must be ≤ ~73 chars. Medium files for pPb are written to
  `~/Jewel/jewel-2.4.0/params/pPb/` (not the repo path). `setup-pPb.sh` handles this automatically.
- **2D hydro profiles**: 100 Ncoll bins in `~/Jewel/hydro/pPb/sample/` (68 visible + 32 hidden
  `.`-prefix dirs). `setup-pPb.sh` uses `shopt -s dotglob` to include all.
- **Analysis flags**: `--IsPP {true,false} --IsGenZ true --IsData false --IsJewel true
  --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false
  --yBoost 0 --nMix 10`. Run from `MainAnalysis/20241102_ZhadronVsZPt/` with `SKIP_CLEAN=1 NTHREAD=25`.
- **Cross-section weighting**: PPZJ events carry per-event cross-section weights. Weighted N_Z
  denominators (~1e-3) differ by dataset — per-Z distributions differ by dataset for this reason
  alone (not physics). Normalization offsets between pp and pPb are expected.
- **Analysis entrypoints**: `jewel-pp-energy.sh` (pp 8.16 + 5.02 comparison),
  `jewel-pPb.sh` (pPb 8.16). Output plots in `Plots/20260212_pPbJewel/plots/{pp_energy,pPb}/`.
- **Plot scripts**: `plot-jewel-pp-diagnostics.sh`, `plot-jewel-pp-energy.sh`,
  `plot-jewel-pPb-diagnostics.sh`, `plot-jewel-pPb.sh` — all in `Plots/20260212_pPbJewel/`.

## Production workflow style (official outputs)

- Favor modifying existing scripts or adding new dedicated scripts in-repo. Drive
  official plot/correction production via scripted `system-analysis.sh` calls — not
  one-off terminal command chains (those are fine for debugging only).
- For `20241102_ZhadronVsZPt`, avoid parallel fixup runner families for official
  reruns — update the canonical script in place (comment / replace superseded calls)
  so there's one authoritative entrypoint per stage.

## Runtime and filesystem norms

- Use `python3` (not `python`).
- Files intended for `/tmp` should be written under `/tmp/kdeverea`.
- Default behavior: write plots/products to analysis repo paths unless explicitly told
  otherwise.

## User-defined folder meanings

- `MainAnalysis/20241102_ZhadronVsZPt/`: main correlation analysis workflow.
- `Plots/20251001_pPbVZReweighting/summary/`: current Vz reweighting summary outputs;
  latest note-ready pp/pPb/PbP plots are `20260311_*`.
- `MainAnalysis/20260115_ZCorrection/`: Z reweighting correction derivation. Closure is
  demonstrated by running through `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`;
  plots in `Plots/20260115_ZResidualClosure/`.
- `Plots/20260115_ZResidualClosure/`: Z-correction closure outputs (e.g.
  `ZPT0_500_Zclosure-closure-*.pdf` under `plots/{pp,pPb,PbP}`).
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/`: 1D track residual
  correction-iteration outputs (fallback); superseded by 2D for `trkV29`.
- `MainAnalysis/20260518_ResidualCorrection2D/workflow_2D/`: 2D track residual
  correction derivation. Official `trkV29` weight files in `output/`, correction
  convergence PDFs (`corrections_2D_*.pdf`) and 1D closure PDFs in `plots/`, closure
  inputs in `output/closure_inputs/`.
- `Plots/20251202_trackResidualClosure/plots/`: track-level no-subtraction closure
  outputs (`*-nosub-closure-*`) including `trkV29` 2D eta-phi heat maps
  (`*-nosub-closure-eta-phi-{corrected,gen,ratio}.pdf`).
- `Plots/20260120_CentralClosure/plots/`: background-subtracted and pre-subtraction
  closure outputs (`*-closure-Delta{Eta,Phi}-{all,bkg,result}.pdf`), including
  `trkV29` updated figures. For current-tag closure refreshes, run
  `ExecuteClosureTest` directly for each `collisionType` over
  `ZPT={0_10,10_20,20_40,40_500}`, `trkPtRange=0.5_15`, `tag=$OFFICIAL_TAG_PP` (or
  `$OFFICIAL_TAG_PPB`); `plot-central.sh` runs all three systems.
  `plot_closure.cpp` shows exactly **4 curves**: MC DY-GEN, MC DY-RECO, + Z correction,
  + Z + track correction. The former 5th `|ΔVZ| < 0.5 cm` comparison curve and
  `tagDeltaVZ` parameter have been removed since `MaxMixDeltaVZ=1.0` is now the universal
  default applied to all curves including Gen.
- `SampleGeneration/20250929_ReducedTreePA/`: PA reduced-tree skimming that produced
  the V0.2 skim inputs. `ReduceForest.cpp` converts forest ROOT inputs into skim
  trees; `make Prepare` creates `Samples/{PAMC,APMC,PAData}` symlinks. Forest-side
  orientation labels are the correct physical ones even where downstream skim naming
  later became swapped.
- `SampleGeneration/20260403_PythiaMadgraph/`: private pp generator workflow for
  `Z → μμ`. Generates MG5 LHE at 5.02 / 8.16 TeV, showers with Pythia8/`CP5`,
  converts HepMC to single-`Tree` `ZHadronMessenger` skim. Includes
  `run_validation.sh` / `QuickDndEta.cpp` for quick `dN/dη` validation in `output/`.
- `MainAnalysis/20260222_EnergyExtrapolation/`: pp energy-extrapolation workflow
  (pp data only). `workflow/private-closure.sh` is the maintained private pp
  8.16/5.02 closure entrypoint — compares `ZMuMu_{5020,8160}_validation.root`, writes
  unique `output/20260405_EnergyExtrapolation_private8160over5020_closure*` products
  without touching the promoted `EEV*` files; correction basename ends in `_closure`.
- `Plots/20260213_Central/plot_dataMCComparison.cpp` + `plot-dataMCComparison.sh`:
  data/MC comparison entrypoint; reads ROOT inputs from
  `MainAnalysis/20241102_ZhadronVsZPt/plots/`, writes to
  `Plots/20260213_Central/plots/dataMCComparison/`.
- `Plots/20260213_Central/plot_pp.cpp` + `plot-pp.sh`: pp closure/data-vs-GEN
  entrypoint; expects
  `plots/{pp_nominal,pp_ZResidual,pp_trkResidual,pythiaMC_Gen_nominal}_<tag>_ZPT*-result.root`
  with all requested track-pt histograms merged into each ZPT result file.
- `Plots/20260213_Central/plot-zmass.sh`: also requires current-tag
  `pythiaMC_Gen_nominal_${OFFICIAL_TAG_PP}_ZPT0_500-nosub.root` and
  `pythiaMC_nominal_${OFFICIAL_TAG_PP}_ZPT0_500-nosub.root`; regenerate those before
  trusting the emitted PDF if missing.
- `MainAnalysis/20260414_pPbPbpCombining/`: see "pPb/PbP combining sandbox" above.
- `~/OverleafZHadronInPPb/`: analysis-note source tree; main file is `AN-23-ABC.tex`
  with sections in `src/*.tex`; figure assets organized by topic under `figures/`
  (`event`, `z_reco/eff`, `tracking`, `tracking/appendix`, `analysis/closure`, etc.).

## PA skimming workflow (`SampleGeneration/20250929_ReducedTreePA`)

- Skim producer turns PA forest files into the reduced ROOT datasets later consumed
  as `pPbSample/V0.2/*`.
- Executable built from `ReduceForest.cpp`; reads forest files via `--Input`, writes
  via `--Output`, with toggles `--DoGenLevel`, `--IsData`, `--RunStart/RunEnd`,
  `--TrackEfficiencyPath`, `--IgnoreEventWeight`.
- `make Prepare` creates local `Samples/` symlinks on `/eos`:
  - `Samples/PAMC` → correctly labeled pPb MC forest dir
  - `Samples/APMC` → correctly labeled PbP MC forest dir
  - `Samples/PAData` → PA single-muon data forests
  - `Samples/PPData` → 2017 pp single-muon data forests
  - `Samples/PPMC` → 2017 pp DY MC forests
- Maintained local pp skim runner branches are `PPData5TeV` and `PPMC5TeV`. For local
  validation, keep subset outputs under repository `output/`, not `/eos`.
- Forest paths on `/eos` require the user to run `kinit -5` first — ask before
  inspecting forest files directly. The user-owned EOS area
  `/eos/cms/store/group/phys_heavyions/kdeverea/` may be written for sanctioned skim
  / production tasks when explicitly authorized, but never run `hadd` there unless
  the prompt explicitly allows merging.
- `PrepareCondor.sh` writes `SubmitPA.condor`; submit with `condor_submit SubmitPA.condor`.
  `Condor.sh` bootstraps CMSSW on lxplus, copies the local `Execute` binary into the
  sandbox, runs reco+gen for MC inputs, or two split reco passes for data
  (`_AP.root` / `_PA.root` via embedded run ranges).
- **pp tracking-efficiency**: PA/non-pp tracking stays on `TrackEfficiencyCorrector.h`
  with `rTotalEff3D_0`; pp reco skims (`Tree`, `TreeLoose`, `TreeTight`) route through
  `TrkEff2017pp` from `CommonCode/include/trackingEfficiency2017pp.h` inside
  `ReduceForest.cpp`. Maintained pp runner `local_skim.sh` passes the tracking
  correction as a full ROOT path; pp skimmer wiring converts that to the directory
  prefix `TrkEff2017pp` expects.
- **V0.3 pp skim production**: for the unmerged V0.3 pp campaign, run `PPData` and
  `PPMC` sequentially (not overlapped). Use `NTHREAD=15` for pp data, `NTHREAD=5` for
  pp MC unless a documented buffer issue forces a reduction. Validate every expected
  output file for existence/openability/required keys. Rerun only the bad subset via
  direct `local_skim.sh` calls; never merge with `hadd`.

## Overleaf workflow and structure

- Repo path: `~/OverleafZHadronInPPb/`. Main entry: `AN-23-ABC.tex`. Sections in
  `src/*.tex`. Figures by topic under `figures/` (`analysis/energy/`,
  `analysis/closure/`, `result/`, `tracking/`, `z_reco/eff/`, etc.).
- Copy generated PDFs to Overleaf **only when explicitly called for** by user or
  reviewer plan. In note text/captions, avoid internal jargon (skims, versions,
  workflow details), do not say `PA`, and use `Pbp` capitalization exactly.

Figure-update standards:
- derive an **explicit manifest** of figures to update from
  `~/OverleafZHadronInPPb/src/*.tex` — no broad basename matching or global sync;
- preserve the **source basename exactly** when copying — do not rename a new
  `ZV6/trkV24` file to an older `ZV5/trkV23` or other stale filename;
- if a new filename is wanted, update the corresponding `\includegraphics{...}` path
  in `src/*.tex` to that exact copied filename;
- if multiple figures collide in the same Overleaf directory, preserve the source
  basename and resolve the collision via directory structure or a deliberate TeX
  path update — never via silent renaming to an old filename;
- after updating references, remove stale superseded copies only if no `src/*.tex`
  file still references them.

Concrete method:
1. Extract current note references from `src/*.tex` and identify exact figures.
2. Build a source-to-destination manifest for only those figures.
3. Copy each refreshed PDF to Overleaf using the source basename
   (`shutil.copy2(source, destdir / source.name)` style).
4. Update `src/*.tex` so every changed `\includegraphics{...}` points to the copied
   source basename and directory.
5. Post-copy audit: verify byte-identical copies (sha256), no missing
   `\includegraphics{...}` targets among the changed figures, distinguish unrelated
   pre-existing missing references from those changed in the current task, and
   confirm `git status --short` in `~/OverleafZHadronInPPb` matches the manifest.

## Correction-stack order and closure

Order: VZ → Z correction → track residual correction → energy extrapolation (pp only).
VZ feeds Z; VZ and Z both feed track residual. Central result uses all of them. Total
closure is demonstrated on MC by `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`;
plots in `Plots/20260120_ZhadronVsZPtClosure/`. Main results from
`MainAnalysis/20241102_ZhadronVsZPt/central.sh`; plots in `Plots/20260213_Central/`.

## Known issue context to keep in mind

- A mixed-event `UseEventWeight` bug in `CorrelationAnalysis.cpp` was identified and
  fixed; it affected mixed-event normalization. Historical VZ/event-weight outputs
  generated pre-fix may be biased — label pre-fix vs post-fix in planning/review docs.
- Closure-only regressions don't automatically imply downstream central/result plots
  are stale. Propagate reruns only when promoted correction ROOTs or central ROOTs
  actually change — pp correction changes can affect energy extrapolation and final
  combined plots; heavy-ion closure-label fixes alone do not.
