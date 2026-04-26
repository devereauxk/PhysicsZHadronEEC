#!/bin/bash
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

ZCORR_FILE="${1:-output/z_correction_final.root}"
PPB_INPUT="${OFFICIAL_DATAINPUT_PPB}"
PBP_INPUT="${OFFICIAL_DATAINPUT_PBP}"
YBOOST="${YBOOST:-0.465}"
NTHREAD=${NTHREAD:-25}

export SKIP_CLEAN=1

run_analysis() {
    local NAME=$1
    shift
    echo "=== Running $NAME ==="
    local pids=()
    for (( chunk=1; chunk<=NTHREAD; chunk++ )); do
        ./ExecuteCorrelationAnalysis "$@" \
            --nThread $NTHREAD --nChunk $chunk \
            --Output "output/${NAME}_chunk${chunk}.root" &
        pids+=($!)
        while [ "$(jobs -rp | wc -l)" -ge "$NTHREAD" ]; do
            wait -n
        done
    done
    wait
    hadd -f "output/${NAME}.root" output/${NAME}_chunk*.root
    rm -f output/${NAME}_chunk*.root
    echo "=== Done: output/${NAME}.root ==="
}

ZPTCUT="--MinZPT 5 --MaxZPT 100 --ZCorrectionFile ${ZCORR_FILE}"
COMMON_PPB="--IsPP false --IsGenZ false --IsData true --IsPPb true --Input ${PPB_INPUT} --yBoost ${YBOOST} --nMix 0 ${ZPTCUT}"
COMMON_PBP="--IsPP false --IsGenZ false --IsData true --IsPPb false --Input ${PBP_INPUT} --yBoost ${YBOOST} --nMix 0 ${ZPTCUT}"

run_family() {
    local SUFFIX=$1
    local PUEXTRA=$2

    run_analysis "pPb_raw_zptcut${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight false --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight false
    run_analysis "PbP_raw_zptcut${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight false --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight false

    run_analysis "pPb_VZ_zptcut${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}"
    run_analysis "PbP_VZ_zptcut${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}"

    run_analysis "pPb_VZ_Z_zptcut${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}"
    run_analysis "PbP_VZ_Z_zptcut${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}"

    run_analysis "pPb_VZ_Z_ZSF_zptcut${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --UseZScaleFactor true
    run_analysis "PbP_VZ_Z_ZSF_zptcut${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --UseZScaleFactor true

    run_analysis "pPb_VZ_Z_ZSF_trk_zptcut${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --UseZScaleFactor true
    run_analysis "PbP_VZ_Z_ZSF_trk_zptcut${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --UseZScaleFactor true

    run_analysis "pPb_VZ_Z_ZSF_trk_res_zptcut${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --ResidualWeightFile "${RWeightFile_PPb}" --UseZScaleFactor true
    run_analysis "PbP_VZ_Z_ZSF_trk_res_zptcut${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --ResidualWeightFile "${RWeightFile_PbP}" --UseZScaleFactor true
}

run_family "" ""
run_family "_PUReject" "--IsPUReject true"

echo "All Z pT cut analysis runs complete."
