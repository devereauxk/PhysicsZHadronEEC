#!/bin/bash
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

PPB_INPUT="${OFFICIAL_DATAINPUT_PPB}"
PBP_INPUT="${OFFICIAL_DATAINPUT_PBP}"
YBOOST="${YBOOST:-0.465}"
OUTPUT_DIR="${OUTPUT_DIR:-output}"
MIN_ZPT="${MIN_ZPT:-0}"
MAX_ZPT="${MAX_ZPT:-500}"
MIN_TRACK_PT="${MIN_TRACK_PT:-0.5}"
MAX_TRACK_PT="${MAX_TRACK_PT:-15}"
PPB_EXTRA_ARGS="${PPB_EXTRA_ARGS:-}"
PBP_EXTRA_ARGS="${PBP_EXTRA_ARGS:-}"

NTHREAD=${NTHREAD:-25}

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
mkdir -p "${OUTPUT_DIR}"

run_analysis() {
    local NAME=$1
    shift
    echo "=== Running $NAME ==="

    # Launch chunks
    local pids=()
    for (( chunk=1; chunk<=NTHREAD; chunk++ )); do
        ./ExecuteCorrelationAnalysis "$@" \
            --nThread $NTHREAD --nChunk $chunk \
            --Output "${OUTPUT_DIR}/${NAME}_chunk${chunk}.root" &
        pids+=($!)
        # Limit concurrent workers
        while [ "$(jobs -rp | wc -l)" -ge "$NTHREAD" ]; do
            wait -n
        done
    done
    wait

    # Merge (use _chunk* pattern to avoid matching other merged outputs)
    hadd -f "${OUTPUT_DIR}/${NAME}.root" "${OUTPUT_DIR}/${NAME}_chunk"*.root
    rm -f "${OUTPUT_DIR}/${NAME}_chunk"*.root
    echo "=== Done: ${OUTPUT_DIR}/${NAME}.root ==="
}

COMMON_KINEMATIC="--MinZPT ${MIN_ZPT} --MaxZPT ${MAX_ZPT} --MinTrackPT ${MIN_TRACK_PT} --MaxTrackPT ${MAX_TRACK_PT}"
COMMON_PPB="--IsPP false --IsGenZ false --IsData true --IsPPb true --Input ${PPB_INPUT} --yBoost ${YBOOST} --nMix 0 ${COMMON_KINEMATIC} ${PPB_EXTRA_ARGS}"
COMMON_PBP="--IsPP false --IsGenZ false --IsData true --IsPPb false --Input ${PBP_INPUT} --yBoost ${YBOOST} --nMix 0 ${COMMON_KINEMATIC} ${PBP_EXTRA_ARGS}"

run_family() {
    local SUFFIX=$1
    local PUEXTRA=$2

    # 1. Raw (no EventWeight, no TrackWeight, no corrections)
    run_analysis "pPb_raw${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight false --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight false
    run_analysis "PbP_raw${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight false --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight false

    # 2. VZ correction only
    run_analysis "pPb_VZ${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}"
    run_analysis "PbP_VZ${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}"

    # 3. VZ + Z correction
    run_analysis "pPb_VZ_Z${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}"
    run_analysis "PbP_VZ_Z${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}"

    # 4. VZ + Z + Z scale factor (skim TnP ZWeight)
    run_analysis "pPb_VZ_Z_ZSF${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --UseZScaleFactor true
    run_analysis "PbP_VZ_Z_ZSF${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --UseZScaleFactor true

    # 5. VZ + Z + ZSF + TrackWeight
    run_analysis "pPb_VZ_Z_ZSF_trk${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --UseZScaleFactor true
    run_analysis "PbP_VZ_Z_ZSF_trk${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --UseZScaleFactor true

    # 6. VZ + Z + ZSF + TrackWeight + TrackResidual
    run_analysis "pPb_VZ_Z_ZSF_trk_res${SUFFIX}" $COMMON_PPB ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --ResidualWeightFile "${RWeightFile_PPb}" --UseZScaleFactor true
    run_analysis "PbP_VZ_Z_ZSF_trk_res${SUFFIX}" $COMMON_PBP ${PUEXTRA} --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --ResidualWeightFile "${RWeightFile_PbP}" --UseZScaleFactor true
}

run_family "" ""
run_family "_PUReject" "--IsPUReject true"

echo "All analysis runs complete."
