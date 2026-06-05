#!/bin/bash
# OfficialProductDictionary.sh — canonical paths for note-facing production ROOT files.
# Source after OfficialWeightDictionary.sh (depends on OFFICIAL_TAG_PP, OFFICIAL_TAG_PPB).
# Self-locating: does not depend on ProjectBase or PWD.

_OPDICT_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

# ── Canonical output directories ──────────────────────────────────────────────
OFFICIAL_RESULT_DIR="${_OPDICT_ROOT}/MainAnalysis/20241102_ZhadronVsZPt/plots"
OFFICIAL_ZCORR_CLOSURE_DIR="${_OPDICT_ROOT}/MainAnalysis/20260115_ZCorrection/workflow/output/closure_inputs"
OFFICIAL_TRKCORR_CLOSURE_DIR="${_OPDICT_ROOT}/MainAnalysis/20260518_ResidualCorrection2D/workflow_2D/output/closure_inputs"
export OFFICIAL_RESULT_DIR OFFICIAL_ZCORR_CLOSURE_DIR OFFICIAL_TRKCORR_CLOSURE_DIR

unset _OPDICT_ROOT

# ── MC closure file prefixes (populated; files exist for current tags) ────────
# Usage: ${PREFIX}_ZPT<ZPT>-nosub.root
# pp (pythiaMC) — four correction stages
OFFICIAL_PP_MC_GEN_PREFIX="${OFFICIAL_RESULT_DIR}/pythiaMC_Gen_nominal_${OFFICIAL_TAG_PP}"
OFFICIAL_PP_MC_RECO_PREFIX="${OFFICIAL_RESULT_DIR}/pythiaMC_nominal_${OFFICIAL_TAG_PP}"
OFFICIAL_PP_MC_ZRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/pythiaMC_ZResidual_${OFFICIAL_TAG_PP}"
OFFICIAL_PP_MC_TRKRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/pythiaMC_trkResidual_${OFFICIAL_TAG_PP}"
export OFFICIAL_PP_MC_GEN_PREFIX OFFICIAL_PP_MC_RECO_PREFIX
export OFFICIAL_PP_MC_ZRESIDUAL_PREFIX OFFICIAL_PP_MC_TRKRESIDUAL_PREFIX

# pPb MC (pPbMC) — four correction stages
OFFICIAL_PPB_MC_GEN_PREFIX="${OFFICIAL_RESULT_DIR}/pPbMC_Gen_nominal_${OFFICIAL_TAG_PPB}"
OFFICIAL_PPB_MC_RECO_PREFIX="${OFFICIAL_RESULT_DIR}/pPbMC_nominal_${OFFICIAL_TAG_PPB}"
OFFICIAL_PPB_MC_ZRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/pPbMC_ZResidual_${OFFICIAL_TAG_PPB}"
OFFICIAL_PPB_MC_TRKRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/pPbMC_trkResidual_${OFFICIAL_TAG_PPB}"
export OFFICIAL_PPB_MC_GEN_PREFIX OFFICIAL_PPB_MC_RECO_PREFIX
export OFFICIAL_PPB_MC_ZRESIDUAL_PREFIX OFFICIAL_PPB_MC_TRKRESIDUAL_PREFIX

# PbP MC (PbPMC) — four correction stages
OFFICIAL_PBP_MC_GEN_PREFIX="${OFFICIAL_RESULT_DIR}/PbPMC_Gen_nominal_${OFFICIAL_TAG_PPB}"
OFFICIAL_PBP_MC_RECO_PREFIX="${OFFICIAL_RESULT_DIR}/PbPMC_nominal_${OFFICIAL_TAG_PPB}"
OFFICIAL_PBP_MC_ZRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/PbPMC_ZResidual_${OFFICIAL_TAG_PPB}"
OFFICIAL_PBP_MC_TRKRESIDUAL_PREFIX="${OFFICIAL_RESULT_DIR}/PbPMC_trkResidual_${OFFICIAL_TAG_PPB}"
export OFFICIAL_PBP_MC_GEN_PREFIX OFFICIAL_PBP_MC_RECO_PREFIX
export OFFICIAL_PBP_MC_ZRESIDUAL_PREFIX OFFICIAL_PBP_MC_TRKRESIDUAL_PREFIX

# ── Result file prefixes (TBD: EEV6/ZV10 production not yet complete) ────────
# File usage: ${PREFIX}_ZPT<ZPT>-result.root  or  -nosub.root
OFFICIAL_PP_RESULT_PREFIX=""       # ${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}
OFFICIAL_PPB_NOSUB_PREFIX=""       # ${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}
OFFICIAL_PBP_NOSUB_PREFIX=""       # ${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}
export OFFICIAL_PP_RESULT_PREFIX OFFICIAL_PPB_NOSUB_PREFIX OFFICIAL_PBP_NOSUB_PREFIX

# 12x12 surface result prefixes (ZV10/trkV29, MaxMixDeltaVZ=1cm, tag suffix _bin12x12_20260603; pp stays at _20260602)
OFFICIAL_PP_RESULT_PREFIX_BIN12="${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_bin12x12_20260603"
OFFICIAL_PPB_NOSUB_PREFIX_BIN12="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_bin12x12_20260603"
OFFICIAL_PBP_NOSUB_PREFIX_BIN12="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_bin12x12_20260603"
export OFFICIAL_PP_RESULT_PREFIX_BIN12 OFFICIAL_PPB_NOSUB_PREFIX_BIN12 OFFICIAL_PBP_NOSUB_PREFIX_BIN12

# ── pPbPbpCombining 12×12 compatibility study productions ────────────────────
# Nominal verification copy (same settings as BIN12 above; should be bit-identical)
export OFFICIAL_PPB_BIN12_FULL_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_full"
export OFFICIAL_PBP_BIN12_FULL_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_full"
# Study 1: Run half-splits
export OFFICIAL_PPB_BIN12_PPB_FIRST_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_pPbFirst"
export OFFICIAL_PPB_BIN12_PPB_SECOND_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_pPbSecond"
export OFFICIAL_PBP_BIN12_PBP_FIRST_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_PbPFirst"
export OFFICIAL_PBP_BIN12_PBP_SECOND_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_PbPSecond"
# Study 2: |vz|<10 cm window
export OFFICIAL_PPB_BIN12_VZ10_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_vz10"
export OFFICIAL_PBP_BIN12_VZ10_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_vz10"
# Study 3: Signed DeltaEta/DeltaPhi (Pbp with FlipDeltaEta) + pp signed curve
export OFFICIAL_PPB_BIN12_SIGNED_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_signed"
export OFFICIAL_PBP_BIN12_SIGNED_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_signed"
export OFFICIAL_PP_BIN12_SIGNED_PREFIX="${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_signed"
# Study 9: CM-frame signed DeltaEta (Pbp with yBoost=0.465+FlipDeltaEta; pPb reuses _signed; pp reuses _signed)
export OFFICIAL_PBP_BIN12_SIGNED_BOOST_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_signed_boost"
# Study 4: MC reco Z-count-matched to data
export OFFICIAL_PPB_BIN12_MC_MATCHED_PREFIX="${OFFICIAL_RESULT_DIR}/pPbMC_trkResidual_${OFFICIAL_TAG_PPB}_12x12_matched"
export OFFICIAL_PBP_BIN12_MC_MATCHED_PREFIX="${OFFICIAL_RESULT_DIR}/PbPMC_trkResidual_${OFFICIAL_TAG_PPB}_12x12_matched"
# Study 5: DeltaVZ sensitivity (nominal=1 cm; variations: 0.5 cm and off)
export OFFICIAL_PPB_BIN12_DVZ0P5_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_dvz0p5"
export OFFICIAL_PBP_BIN12_DVZ0P5_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_dvz0p5"
export OFFICIAL_PPB_BIN12_DVZOFF_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_dvzOff"
export OFFICIAL_PBP_BIN12_DVZOFF_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_dvzOff"
# Study 6: MC reco full statistics
export OFFICIAL_PPB_BIN12_MC_FULLSTATS_PREFIX="${OFFICIAL_RESULT_DIR}/pPbMC_trkResidual_${OFFICIAL_TAG_PPB}_12x12_fullstats"
export OFFICIAL_PBP_BIN12_MC_FULLSTATS_PREFIX="${OFFICIAL_RESULT_DIR}/PbPMC_trkResidual_${OFFICIAL_TAG_PPB}_12x12_fullstats"
# Study 7: Raw uncorrected data
export OFFICIAL_PPB_BIN12_RAW_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_raw"
export OFFICIAL_PBP_BIN12_RAW_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_raw"
# Study 8: Nmix sensitivity (20 and 40)
export OFFICIAL_PPB_BIN12_NMIX20_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nmix20"
export OFFICIAL_PBP_BIN12_NMIX20_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nmix20"
export OFFICIAL_PPB_BIN12_NMIX40_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nmix40"
export OFFICIAL_PBP_BIN12_NMIX40_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nmix40"
# Study 11: nThread comparison (20, 40, 60; 30 kept for archival)
export OFFICIAL_PPB_BIN12_NTHREAD20_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread20"
export OFFICIAL_PBP_BIN12_NTHREAD20_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread20"
export OFFICIAL_PPB_BIN12_NTHREAD30_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread30"
export OFFICIAL_PBP_BIN12_NTHREAD30_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread30"
export OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread40"
export OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread40"
export OFFICIAL_PPB_BIN12_NTHREAD60_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread60"
export OFFICIAL_PBP_BIN12_NTHREAD60_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_nthread60"
# Study 13: 20-bin (default 20x20) sideband production
export OFFICIAL_PPB_BIN20_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_20bin"
export OFFICIAL_PBP_BIN20_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_20bin"
# Study 12: Pbp with 2D Z (yCM, phi) correction applied
export OFFICIAL_PBP_BIN12_ZCORR_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_zcorr"
# Study 17: Option A — CM-frame signed comparison (asymmetric acceptance |eta_cm|<1.935)
export OFFICIAL_PPB_BIN12_CMA_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma"
export OFFICIAL_PBP_BIN12_CMA_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma"
export OFFICIAL_PP_BIN12_CMA_PREFIX="${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_cma"
# Study 18: Option B — pPb-oriented signed comparison (|eta_lab|<1.935 for HI; Pbp flipped)
export OFFICIAL_PPB_BIN12_CMB_PREFIX="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cmb"
export OFFICIAL_PBP_BIN12_CMB_PREFIX="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cmb"
export OFFICIAL_PP_BIN12_CMB_PREFIX="${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_cmb"
# Study 20: pp MC Gen boost variants (nominal, pPb-boosted, Pbp-boosted)
export OFFICIAL_PP_BIN12_BOOST_NOM_PREFIX="${OFFICIAL_RESULT_DIR}/ppmc_gen_boost_nom_12x12"
export OFFICIAL_PP_BIN12_BOOST_PPB_PREFIX="${OFFICIAL_RESULT_DIR}/ppmc_gen_boost_ppblike_12x12"
export OFFICIAL_PP_BIN12_BOOST_PBP_PREFIX="${OFFICIAL_RESULT_DIR}/ppmc_gen_boost_pbplike_12x12"

# ── Helper: abort if expected product file is missing ─────────────────────────
assert_product_exists() {
    local f="$1"
    if [[ ! -f "$f" ]]; then
        echo "ERROR: production ROOT not found: $f" >&2
        return 1
    fi
}
export -f assert_product_exists
