#!/bin/bash
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

ZCORR_FILE=$1
OUTPUT=$2
NTHREAD=${NTHREAD:-25}

PPB_INPUT="${OFFICIAL_DATAINPUT_PPB}"
YBOOST="${YBOOST:-0.465}"

echo "=== Running pPb with Z correction: ${ZCORR_FILE} ==="
pids=()
for (( chunk=1; chunk<=NTHREAD; chunk++ )); do
    ./ExecuteCorrelationAnalysis \
        --IsPP false --IsGenZ false --IsData true --IsPPb true \
        --Input "${PPB_INPUT}" --yBoost "${YBOOST}" --nMix 0 \
        --UseEventWeight false --UseTrackWeight false --UseZWeight false \
        --UseResidualWeight false --UseVZWeight false \
        --ZCorrectionFile "${ZCORR_FILE}" \
        --nThread $NTHREAD --nChunk $chunk \
        --Output "output/zcorr_chunk${chunk}.root" &
    pids+=($!)
    while [ "$(jobs -rp | wc -l)" -ge "$NTHREAD" ]; do
        wait -n
    done
done
wait

hadd -f "${OUTPUT}" output/zcorr_chunk*.root
rm -f output/zcorr_chunk*.root
echo "=== Done: ${OUTPUT} ==="
