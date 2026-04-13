#!/bin/bash

set -euo pipefail

ZPT_RANGE="${ZPT_RANGE:-0_500}"
TRKPT_RANGE="${TRKPT_RANGE:-1_10}"
INPUT_TAG="${INPUT_TAG:-noVZWeight_nmix0}"
OUTPUT_DATE="${OUTPUT_DATE:-20260407}"
OUTPUT_TAG="${OUTPUT_TAG:-${OUTPUT_DATE}_ZPT${ZPT_RANGE}}"
SYSTEMS=(${SYSTEMS:-pPb PbP})

make ExecuteVZ

run_one() {
    local system="$1"
    ./ExecuteVZ \
        --collisionType "$system" \
        --mcCollisionType "$system" \
        --dataCollisionType "$system" \
        --zPtRange "$ZPT_RANGE" \
        --trkPtRange "$TRKPT_RANGE" \
        --tag "$INPUT_TAG" \
        --output "$OUTPUT_TAG"
}

for system in "${SYSTEMS[@]}"; do
    run_one "$system"
done
