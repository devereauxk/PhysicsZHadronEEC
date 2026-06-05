# Proposal: OfficialProductDictionary.sh — Centralized Production ROOT Registry

## Motivation

The `MainAnalysis/20241102_ZhadronVsZPt/plots/` directory currently accumulates result
ROOT files from every analysis campaign since EEV1 (2024 through the present EEV6
campaign). There is **no centralized declaration** of which files are the current
note-facing production roots. Every downstream consumer — plotters in `Plots/20260213_Central/`,
systematics harvesters in `Systematics/20260329_pPbSystematics/`, and jackknife tools in
`MainAnalysis/20260506_Jackknife/` — independently assembles paths by combining a hardcoded
directory string with a tag from the dictionary. Risks:

- A plotter uses a stale tag (or `OFFICIAL_TAG` is not sourced) and silently reads old data.
- Running a new production campaign doesn't invalidate old files; the old files sit alongside
  the new ones with no deprecation signal.
- The absolute path `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots`
  is **hardcoded in 15+ C++ source files** — fragile if the working directory ever moves.
- The `ANALYSISDIR` variable used in shell scripts is defined locally in each script,
  not sourced from a shared location.

---

## Current State (Audit, June 2026)

### Where production roots are written

| Producer script | Output directory | Files produced |
|---|---|---|
| `MainAnalysis/20241102_ZhadronVsZPt/central.sh` | `…/20241102_ZhadronVsZPt/plots/` | `{pp,pPb,PbP}_trkResidual_${TAG}_ZPT*-{result,nosub}.root` |
| `MainAnalysis/20241102_ZhadronVsZPt/systematics.sh` | same | variation roots (`_Loose`, `_Tight`, `_IsMuTaggedFalse`, `_IsPURejectTrue`, `_MuVar0..3`, `_TrackCorrection{0p976,1p024}`, `_EEPrivate`) |
| `MainAnalysis/20241102_ZhadronVsZPt/result-study-bin12x12-20260507.sh` | same | 12×12 roots (tag suffix `_bin12x12_20260602`) |
| `MainAnalysis/20241102_ZhadronVsZPt/pp-EE.sh` | same | `pp_{nominal,ZResidual,EEtrkResidual,EExtrapolation}_${TAG}_ZPT*-result.root` |
| `MainAnalysis/20241102_ZhadronVsZPt/central.sh` (MC legs) | same | `{pythiaMC,pPbMC,PbPMC}_{Gen_nominal,nominal,ZResidual,trkResidual}_${TAG}_ZPT*-{result,nosub}.root` |

All outputs land in one flat directory: `MainAnalysis/20241102_ZhadronVsZPt/plots/`.

### Where production roots are consumed (hardcoded paths found)

| Consumer | Path mechanism | Problem |
|---|---|---|
| `Plots/20260213_Central/plot_central_combined.cpp` | Hardcoded absolute path in `Form(...)` | Breaks if dir moves; ignores dict changes |
| `Plots/20260213_Central/plot_central_overlay_PPbPbP.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_pp.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_central.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_paper.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_energyExtrapolation.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_dataMCComparison.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_zmass.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_zspectrum.cpp` | Hardcoded absolute path | Same |
| `Plots/20260213_Central/plot_2d_corr_bin12x12.cpp` | `inputDir` CLI arg | OK; but caller must know the path |
| `Plots/20260213_Central/plot_central_sb.cpp` | Hardcoded absolute path | Same |
| `Plots/20260115_ZResidualClosure/plot_closure.cpp` | Hardcoded absolute path | Same |
| `Plots/20260120_CentralClosure/plot_closure.cpp` | Hardcoded absolute path | Same |
| `Systematics/20260329_pPbSystematics/run.sh` | Local `ANALYSISDIR` var | Not centralized, defined per-script |
| `Systematics/20260329_pPbSystematics/run-bin12x12-*.sh` | Local `ANALYSISDIR` var | Same |
| `Systematics/20260329_pPbSystematics/run-*.sh` (all) | Local `ANALYSISDIR` var | Same |
| `MainAnalysis/20260506_Jackknife/compute_pPbPbp_ks.cpp` | Hardcoded absolute path | Same as C++ plotters |
| `MainAnalysis/20260506_Jackknife/compute_pPbPbp_compatibility.cpp` | Hardcoded absolute path | Same |

---

## Proposed Implementation: `OfficialProductDictionary.sh`

Create a new file `OfficialProductDictionary.sh` at the project root, sourced **after**
`OfficialWeightDictionary.sh`. Both can be sourced from runner scripts in sequence:

```bash
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
```

Alternatively, source it from within `OfficialWeightDictionary.sh` at the bottom as a
one-liner — the simpler option since all runners already source that file.

### Contents of `OfficialProductDictionary.sh`

```bash
#!/bin/bash
# OfficialProductDictionary.sh — canonical paths for note-facing production ROOT files.
# Source after OfficialWeightDictionary.sh (depends on OFFICIAL_TAG_PP, OFFICIAL_TAG_PPB).

# Canonical result directory
OFFICIAL_RESULT_DIR="${ProjectBase}/MainAnalysis/20241102_ZhadronVsZPt/plots"
export OFFICIAL_RESULT_DIR

# ──────────────────────────────────────────────────────────────────────────────
# 20-bin surface (standard note-facing)
# ──────────────────────────────────────────────────────────────────────────────
# Note-facing ZPT ranges for scan + inclusive
OFFICIAL_ZPT_RANGES_SCAN=("0_30" "30_500")
OFFICIAL_ZPT_RANGES_INCLUSIVE=("0_500")
OFFICIAL_TRKPT_RANGES_SCAN=("0.5_2" "2_4" "4_15")
OFFICIAL_TRKPT_RANGE_INCLUSIVE="0.5_15"

# Nominal corrected data roots (pp: -result.root, pPb/PbP: -nosub.root)
#   Usage: ${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_ZPT${ZPT}-result.root
OFFICIAL_PP_RESULT_PREFIX="${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}"
OFFICIAL_PPB_NOSUB_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}"
OFFICIAL_PBP_NOSUB_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}"
export OFFICIAL_PP_RESULT_PREFIX OFFICIAL_PPB_NOSUB_PREFIX OFFICIAL_PBP_NOSUB_PREFIX

# MC companion roots (Gen-level)
OFFICIAL_PP_MC_GEN_PREFIX="${OFFICIAL_RESULT_DIR}/pythiaMC_Gen_nominal_${OFFICIAL_TAG_PP}"
OFFICIAL_PPB_MC_GEN_PREFIX="${OFFICIAL_RESULT_DIR}/pPbMC_Gen_nominal_${OFFICIAL_TAG_PPB}"
OFFICIAL_PBP_MC_GEN_PREFIX="${OFFICIAL_RESULT_DIR}/PbPMC_Gen_nominal_${OFFICIAL_TAG_PPB}"
export OFFICIAL_PP_MC_GEN_PREFIX OFFICIAL_PPB_MC_GEN_PREFIX OFFICIAL_PBP_MC_GEN_PREFIX

# ──────────────────────────────────────────────────────────────────────────────
# 12×12 binning surface (note-facing for Figs 26–28, KS, systematics)
# ──────────────────────────────────────────────────────────────────────────────
OFFICIAL_BIN12_DATE="20260602"
OFFICIAL_TAG_PP_BIN12="${OFFICIAL_TAG_PP}_bin12x12_${OFFICIAL_BIN12_DATE}"
OFFICIAL_TAG_PPB_BIN12="${OFFICIAL_TAG_PPB}_bin12x12_${OFFICIAL_BIN12_DATE}"
export OFFICIAL_TAG_PP_BIN12 OFFICIAL_TAG_PPB_BIN12

OFFICIAL_PP_RESULT_PREFIX_BIN12="${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP_BIN12}"
OFFICIAL_PPB_NOSUB_PREFIX_BIN12="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB_BIN12}"
OFFICIAL_PBP_NOSUB_PREFIX_BIN12="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB_BIN12}"
export OFFICIAL_PP_RESULT_PREFIX_BIN12 OFFICIAL_PPB_NOSUB_PREFIX_BIN12 OFFICIAL_PBP_NOSUB_PREFIX_BIN12

# ──────────────────────────────────────────────────────────────────────────────
# Energy extrapolation study roots (4-curve pp EE comparison)
# ──────────────────────────────────────────────────────────────────────────────
OFFICIAL_PP_EE_NOMINAL_PREFIX="${OFFICIAL_RESULT_DIR}/pp_nominal_${OFFICIAL_TAG_PP}"
OFFICIAL_PP_EE_ZRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/pp_ZResidual_${OFFICIAL_TAG_PP}"
OFFICIAL_PP_EE_TRKRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/pp_EEtrkResidual_${OFFICIAL_TAG_PP}"
OFFICIAL_PP_EE_EXTRAPOLATION_PREFIX="${OFFICIAL_RESULT_DIR}/pp_EExtrapolation_${OFFICIAL_TAG_PP}"
export OFFICIAL_PP_EE_NOMINAL_PREFIX OFFICIAL_PP_EE_ZRESIDUAL_PREFIX
export OFFICIAL_PP_EE_TRKRESIDUAL_PREFIX OFFICIAL_PP_EE_EXTRAPOLATION_PREFIX

# ──────────────────────────────────────────────────────────────────────────────
# Helper: assert production file exists before use
# Usage: assert_product_exists "${OFFICIAL_PP_RESULT_PREFIX}_ZPT0_30-result.root"
# ──────────────────────────────────────────────────────────────────────────────
assert_product_exists() {
    local f="$1"
    if [[ ! -f "$f" ]]; then
        echo "ERROR: production ROOT not found: $f" >&2
        echo "  Run central.sh (or result-study-bin12x12-*.sh for bin12x12 surface)" >&2
        return 1
    fi
}
export -f assert_product_exists
```

---

## Migration Plan (do not implement until production runs complete)

### Phase 1 — Shell scripts (low risk, no rebuild needed)

For each runner script that defines a local `ANALYSISDIR`:
```bash
# Before (each script defines its own):
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots
# After:
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
ANALYSISDIR="${OFFICIAL_RESULT_DIR}"
```

Target scripts:
- `Systematics/20260329_pPbSystematics/run.sh`
- `Systematics/20260329_pPbSystematics/run-bin12x12-20260507.sh`
- `Systematics/20260329_pPbSystematics/run-pu.sh`
- `Systematics/20260329_pPbSystematics/run-track-selection.sh`
- `Systematics/20260329_pPbSystematics/run-track-correction.sh`
- `Systematics/20260329_pPbSystematics/run-energy-extrapolation.sh`
- `Systematics/20260329_pPbSystematics/run-muon-track.sh`
- `MainAnalysis/20260506_Jackknife/run-pPbPbp-ks.sh`
- `MainAnalysis/20260506_Jackknife/run-pPbPbp-compatibility.sh`

### Phase 2 — C++ plotters (requires rebuild of each plotter)

Add a `--inputDir` CLI argument to all plotters that currently hardcode the path.
The shell wrapper then passes `${OFFICIAL_RESULT_DIR}`. This is a pure addition — the
hardcoded path stays as a default, so old behavior is preserved when `--inputDir` is absent.

Priority (note-facing plotters first):
1. `Plots/20260213_Central/plot_central_combined.cpp` — used for Figs 26–28
2. `Plots/20260213_Central/plot_central_overlay_PPbPbP.cpp` — used for Figs 29–31
3. `Plots/20260213_Central/plot_paper.cpp` — paper figures
4. `Plots/20260213_Central/plot_energyExtrapolation.cpp` — Figs 22–24
5. `MainAnalysis/20260506_Jackknife/compute_pPbPbp_ks.cpp` — KS table
6. `MainAnalysis/20260506_Jackknife/compute_pPbPbp_compatibility.cpp` — Table 7
7. Remaining plotters (closure, diagnostic, etc.) — lower priority

### Phase 3 — Optional: subdirectory per campaign

Instead of a flat `plots/` directory, use `plots/${OFFICIAL_TAG_PP}/` as the output dir for
`central.sh` (and `plots/${OFFICIAL_TAG_PPB}/` for pPb/PbP). `OFFICIAL_RESULT_DIR` would
then point to the per-tag subdirectory. Old campaigns stay in their own subdirectories and
are clearly dated. This is the highest-impact change and requires updating all producers and
consumers simultaneously — defer until after the EEV6 campaign is stable.

---

## What NOT to centralize here

- Correction weight file paths (`VZWeightFile_*`, `ZWeightFile_*`, `RWeightFile_*`,
  `EEWeightFile_*`) — these stay in `OfficialWeightDictionary.sh`
- Input skim paths (`OFFICIAL_*INPUT_*`) — same
- Systematic variation file paths (constructed on-the-fly in each runner from the
  nominal prefix + variation suffix) — no centralization needed

---

## Immediate Action (for current EEV6 campaign)

While the above is implemented, add `OFFICIAL_RESULT_DIR` to `OfficialWeightDictionary.sh`
as a minimal first step — no migration of consumers needed, just gives shell scripts a
canonical variable to reference instead of defining their own:

```bash
# Add near the bottom of OfficialWeightDictionary.sh:
OFFICIAL_RESULT_DIR="${ProjectBase}/MainAnalysis/20241102_ZhadronVsZPt/plots"
export OFFICIAL_RESULT_DIR
```

---

## Current Production Status (as of audit, 2026-06-02 ~14:00)

Both background jobs are running:
- `btucj0lwp` (scan pp-EE 12×12): in `pp_nominal_EEV6_ZV10_trkV29_nmix10_ZPT0_30-0.5_2`
- `b09pulodp` (central.sh 1 1 1): in `pp_trkResidual_EEV6_ZV10_trkV29_nmix10_ZPT0_30-0.5_2`

Neither `pPb_trkResidual_ZV10_trkV29_nmix10_ZPT*-result.root` nor
`PbP_trkResidual_ZV10_trkV29_nmix10_ZPT*-result.root` exist yet — central.sh must
complete its pp section, then pPb, then PbP. Expected completion: several hours from now.

Existing nominal production files for current tags (ZV10_trkV29):
- pp: **NOT YET** (central.sh currently producing)
- pPb/PbP: **NOT YET** (central.sh will produce after pp section)
- pp 12×12: **NOT YET** (result-study-bin12x12 not yet run)
- pPb/PbP 12×12: **NOT YET** (same)

Note: MC companion roots for ZV10_trkV29 (pythiaMC, pPbMC, PbPMC) already exist from
prior runs completed on 2026-06-01.
