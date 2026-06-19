#!/bin/bash
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

PPB_INPUT="${OFFICIAL_MCRECOINPUT_PPB}"
PBP_INPUT="${OFFICIAL_MCRECOINPUT_PBP}"
YBOOST="${YBOOST:-0.465}"
OUTPUT_DIR="${OUTPUT_DIR:-output/mc}"
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

    for (( chunk=1; chunk<=NTHREAD; chunk++ )); do
        ./ExecuteCorrelationAnalysis "$@" \
            --nThread $NTHREAD --nChunk $chunk \
            --Output "${OUTPUT_DIR}/${NAME}_chunk${chunk}.root" &
        while [ "$(jobs -rp | wc -l)" -ge "$NTHREAD" ]; do
            wait -n
        done
    done
    wait

    hadd -f "${OUTPUT_DIR}/${NAME}.root" "${OUTPUT_DIR}/${NAME}_chunk"*.root
    rm -f "${OUTPUT_DIR}/${NAME}_chunk"*.root
    echo "=== Done: ${OUTPUT_DIR}/${NAME}.root ==="
}

COMMON_KINEMATIC="--MinZPT ${MIN_ZPT} --MaxZPT ${MAX_ZPT} --MinTrackPT ${MIN_TRACK_PT} --MaxTrackPT ${MAX_TRACK_PT}"
COMMON_PPB="--IsPP false --IsGenZ false --IsData false --IsPPb true --Input ${PPB_INPUT} --yBoost ${YBOOST} --nMix 0 ${COMMON_KINEMATIC} ${PPB_EXTRA_ARGS}"
COMMON_PBP="--IsPP false --IsGenZ false --IsData false --IsPPb false --Input ${PBP_INPUT} --yBoost ${YBOOST} --nMix 0 ${COMMON_KINEMATIC} ${PBP_EXTRA_ARGS}"

run_analysis "pPbMC_raw" $COMMON_PPB --UseEventWeight false --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight false
run_analysis "PbPMC_raw" $COMMON_PBP --UseEventWeight false --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight false

run_analysis "pPbMC_VZ" $COMMON_PPB --UseEventWeight true --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}"
run_analysis "PbPMC_VZ" $COMMON_PBP --UseEventWeight true --UseTrackWeight false --UseZWeight false --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}"

run_analysis "pPbMC_VZ_Z" $COMMON_PPB --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}"
run_analysis "PbPMC_VZ_Z" $COMMON_PBP --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}"

# The MC skims carry ZWeight = 1, so this stage is intentionally identical to +VZ+Z.
run_analysis "pPbMC_VZ_Z_ZSF" $COMMON_PPB --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --UseZScaleFactor true
run_analysis "PbPMC_VZ_Z_ZSF" $COMMON_PBP --UseEventWeight true --UseTrackWeight false --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --UseZScaleFactor true

run_analysis "pPbMC_VZ_Z_ZSF_trk" $COMMON_PPB --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --UseZScaleFactor true
run_analysis "PbPMC_VZ_Z_ZSF_trk" $COMMON_PBP --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight false --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --UseZScaleFactor true

run_analysis "pPbMC_VZ_Z_ZSF_trk_res" $COMMON_PPB --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" --ZWeightFile "${ZWeightFile_PPb}" --ResidualWeightFile "${RWeightFile_PPb}" --UseZScaleFactor true
run_analysis "PbPMC_VZ_Z_ZSF_trk_res" $COMMON_PBP --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" --ZWeightFile "${ZWeightFile_PbP}" --ResidualWeightFile "${RWeightFile_PbP}" --UseZScaleFactor true

echo "All MC comparison runs complete."
