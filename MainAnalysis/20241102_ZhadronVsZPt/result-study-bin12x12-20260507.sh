#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd /home/kdeverea/PhysicsZHadronEEC
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
cd "$THISDIR"

make

STUDY_SUFFIX="${STUDY_SUFFIX:-_bin12x12_20260507}"
PP_TAG="${PP_TAG_OVERRIDE:-EEV5_ZV9_trkV27_nmix10${STUDY_SUFFIX}}"
PPB_TAG="${PPB_TAG_OVERRIDE:-ZV9_trkV27_nmix10${STUDY_SUFFIX}}"
NMIX="${NMIX:-10}"

export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD="${NTHREAD:-25}"
export NSLICE_FACTOR=1

MODIFIED_RESULT_ARGS=(--ResultDEtaBins 12 --ResultDPhiBins 12)
SCAN_CONFIG="$THISDIR/config_note_bin12x12_scan_20260507.sh"
INCLUSIVE_CONFIG="$THISDIR/config_note_bin12x12_inclusive_20260507.sh"

run_with_configs() {
    local prefix=$1
    shift
    for config in "$SCAN_CONFIG" "$INCLUSIVE_CONFIG"; do
        CONFIG_FILE="$config" ./system-analysis.sh "$prefix" "$@" "${MODIFIED_RESULT_ARGS[@]}"
    done
}

run_with_configs "pp_trkResidual_${PP_TAG}" \
    --IsPP true --IsGenZ false --IsData true --UseVZWeight true \
    --Input "$OFFICIAL_DATAINPUT_PP" \
    --MixFile "$OFFICIAL_DATAINPUT_PP" \
    --UseEventWeight false --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseJackknife true \
    --yBoost 0 --nMix "$NMIX" \
    --ZWeightFile "$ZWeightFile_PP" \
    --ResidualWeightFile "$RWeightFile_PP" \
    --EnergyExtraFile "$EEWeightFile_PP" \
    --VZWeightFile "$VZWeightFile_PP"

run_with_configs "pPb_trkResidual_${PPB_TAG}" \
    --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb true \
    --Input "$OFFICIAL_DATAINPUT_PPB" \
    --MixFile "$OFFICIAL_DATAINPUT_PPB" \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseJackknife true \
    --yBoost 0 --nMix "$NMIX" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb"

run_with_configs "PbP_trkResidual_${PPB_TAG}" \
    --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb false \
    --Input "$OFFICIAL_DATAINPUT_PBP" \
    --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseJackknife true \
    --yBoost 0 --nMix "$NMIX" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

run_with_configs "pPbMC_Gen_nominal_${PPB_TAG}" \
    --IsPP false --IsGenZ true --IsData false --IsPPb true \
    --Input "$OFFICIAL_MCGENINPUT_PPB" \
    --MixFile "$OFFICIAL_MCGENINPUT_PPB" \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile "$OFFICIAL_EPOSINPUT_PPB" \
    --yBoost 0 --nMix "$NMIX" \
    --UseVZWeight true --VZWeightFile "$VZWeightFile_PPb"

run_with_configs "PbPMC_Gen_nominal_${PPB_TAG}" \
    --IsPP false --IsGenZ true --IsData false --IsPPb false \
    --Input "$OFFICIAL_MCGENINPUT_PBP" \
    --MixFile "$OFFICIAL_MCGENINPUT_PBP" \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile "$OFFICIAL_EPOSINPUT_PBP" \
    --yBoost 0 --nMix "$NMIX" \
    --UseVZWeight true --VZWeightFile "$VZWeightFile_PbP"
