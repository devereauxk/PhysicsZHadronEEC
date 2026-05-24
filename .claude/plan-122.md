# Plan 122: Reprocess 2D Residual Corrections and Redo All Plots (trkV28 bug fix)

## Context
A bug was discovered in the 20260519 2D residual correction production: `CorrelationAnalysis.cpp`
at `MainAnalysis/20260518_ResidualCorrection2D/` had wrong `MinTrackPT=1, MaxTrackPT=2` defaults
and wrong histogram binning (`nbinsX=25, xMax=10` with `binEdgesX[25]=200` hack). This caused a
hard track pT cut selecting only tracks in ~0.9-2.1 GeV, which propagated through the entire
analysis chain. All downstream plots from the previous task (`.claude/saved_prompts/20260519.md`)
must be redone.

Additionally, the user requested that the 2D module match the 1D module
(`20251211_ResidualCorrection/CorrelationAnalysis.cpp`) in track pT/eta/phi binning and
weighting. Key alignment changes were made (see "Completed" below).

## Reference for full task scope
`.claude/saved_prompts/20260519.md` — the original trkV28 promotion instructions

## Completed

### 1. Bug diagnosis
- Traced to `MinTrackPT=1, MaxTrackPT=2` defaults in `CorrelationAnalysis.cpp` lines 929-930
- Workflow scripts (`pythia-analysis.sh`, `pPb-DY-analysis.sh`) did not pass explicit `--MinTrackPT`/`--MaxTrackPT`
- 20260518 production used an older binary that happened to work; 20260519 used the recompiled one with wrong defaults

### 2. Code fixes applied (all in `MainAnalysis/20260518_ResidualCorrection2D/`)

**CorrelationAnalysis.cpp — complete rewrite (20260523):**
- Replaced full main-analysis code (with event mixing, DeltaEta/DeltaPhi correlations, etc.)
  with a lean standalone version copied from the 1D reference (`20251211_ResidualCorrection/CorrelationAnalysis.cpp`)
- Uses `TrackResidualCorrector2D` (reads `hPtCorrTotal` TH1D + `hEtaPhiCorrTotal` TH2D) for iterative corrections
- Output histograms: `hTrkPtEtaPhiData` (TH3D, 26×50×50 bins) + `hNZData` (TH1D) — compatible with `correction_2D.C` and `plot_closure.C`
- No event mixing (was never needed for residual correction, previously wasted ~60+ minutes per run)
- Hardcoded track pT range 0.5-15 GeV, eta -2.4 to 2.4, phi 0 to 2pi (matching 1D)
- EventWeight always included; TrackExtraWeight included in fill
- Z weight applied automatically when `--ZWeightFile` is non-empty (no `--UseZWeight` flag)
- EPOS UE loaded via `--InputUE` parameter (matching 1D)

**include/parameter.h — replaced with 1D version:**
- Now has `residualFile`, `inputUE`, `isGen`, `isAddUE`, `isOO` fields matching the 1D reference

**workflow_2D/pythia-analysis.sh — rewritten to match 1D calling convention:**
- Removed `--MixFile`, `--UseEventWeight true`, `--UseZWeight true`, `--MinTrackPT`, `--MaxTrackPT`
- Uses `--residualFile` instead of `--UseResidualWeight true --ResidualWeightFile`
- Z weight applied via `--ZWeightFile` alone

**workflow_2D/pPb-DY-analysis.sh — same changes as pythia-analysis.sh**

### 3. All nominal corrections reprocessed (tag: 20260523)
- pp: 4 files, 1m7s total
- pPb: 4 files
- PbP: 4 files, 1m30s total for pPb+PbP combined
- Total: 12 files at `workflow_2D/output/20260523_ZV9_trkV28_TrackResidualCorrection_{pp,PPb,PbP}_zPt{0-10,10-20,20-40,40-500}.root`
- All 26 bins, 0.5-15 GeV, corrections near 1.0 (physically sensible)
- Closure plots generated for all 12 bins

### 4. All systematic variant corrections reprocessed (tag: 20260523)
- pp systematics: 6 variants × 4 ZPT bins = 24 files, 6m41s
- pPb/PbP systematics: 6 variants × 4 ZPT bins × 2 systems = 48 files, 9m0s
- Variants: Loose, Tight, IsMuTaggedFalse, TrackCorrection0p976, TrackCorrection1p024, IsPURejectTrue
- Total: 84 correction files (12 nominal + 72 systematic)
- All old files (20260518, 20260519, 20260521) removed from output/ and plots/

### 5. OfficialWeightDictionary.sh updated
- All 21 residual weight entries: `20260519` → `20260523`
- Header comment updated

## Completed (continued)

### 6. Corrected-data productions (MainAnalysis/20241102_ZhadronVsZPt/) ✓
- 6b: central.sh (scan+inclusive, pp/pPb/PbP) ✓
- 6c: systematics.sh (84 files, scan+inclusive) ✓
- 6d: pp-EE.sh (6 EE files) ✓
- Note: MC closure (6a) was already done; pythiaMC files don't use residual weights

### 7. All standard-binning plots regenerated ✓
- 7a: Track-level closure (36 1D + 2D heat maps) ✓
- 7b: Analysis-level closure (72 PDFs) ✓
- 7c: Systematics (1,614 PDFs: compiled + standalone) ✓
- 7d: Result plots (42 combined + EE + Z mass + dataMC + pre-sub + pp + overlay) ✓
- 7e: Combining overlay (168 PDFs: no-zcorr + zcorr + Z correction + closure) ✓
  - Fixed TrackResidualCorrector → TrackResidualCorrector2D in combining sandbox

### 8. Overleaf figure copy (standard binning) ✓
- 213 figures copied, 12 TeX references updated (20260519 → 20260523)
- 3 muon-track 2D diagnostics skipped (not affected by residual correction, lower priority)

### 7f. 12×12 bin pipeline ✓
- `result-study-bin12x12-20260507.sh` ✓ (15 result files: 5 prefixes × 3 ZPT configs)
- `systematics-bin12x12-20260507.sh` ✓ (93 systematic variant files)
- `run-bin12x12-20260507.sh` (Systematics/) ✓ (182 systematics PDFs)
- `plot-central-combined-bin12x12-20260507.sh` ✓ (14 result PDFs)
- Scan composite TeX updated trkV27→trkV28, rebuilt ✓
- Copy 12×12 result figures to Overleaf ✓ (4 PDFs: 2 inclusive + 2 scan composites)

## Remaining Work (lower priority)

### 9. Still remaining from prior task
- Pion track figures (appendix)
- Muon-track 2D diagnostics (needs ROOT files from temp module, not affected by residual correction)

## Key Technical Details

### Tags
- `OFFICIAL_TAG_PP=EEV5_ZV9_trkV28_nmix10`
- `OFFICIAL_TAG_PPB=ZV9_trkV28_nmix10`
- Residual correction name tag: `20260523_ZV9_trkV28_TrackResidualCorrection`

### Systematics split for run.sh
```bash
# Inclusive
ZPT_RANGES=0_500 TRACK_RANGES=0.5_15 ./run.sh
# Scan
ZPT_RANGES=0_30,30_500 TRACK_RANGES=0.5_2,2_4,4_15 ./run.sh
```

### Alignment with 1D reference (20251211)
The 2D module now matches the 1D module in:
- pT binning: 26 log-spaced bins from 0.5-15 GeV
- eta binning: 50 linear bins from -2.4 to 2.4
- phi binning: 50 linear bins from 0 to 2pi
- EventWeight: always included (hardcoded, not flag-gated)
- TrackExtraWeight: included in hTrkPtEtaPhi fill
- trackWeight from skim: included
- VZ weight: included via flag
- Z weight: applied via ZWeightFile (not UseZWeight flag)
- No event mixing (lean executable, ~17s per ZPT bin vs many minutes before)

### Performance comparison
- Old (fat executable with event mixing): ~60+ min for nominal pp+pPb+PbP
- New (lean executable, no mixing): ~2.5 min for nominal pp+pPb+PbP
- Systematics: ~16 min total for all 72 variant corrections
