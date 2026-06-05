#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
make ExecuteClosureTest

PPB_TAG="${OFFICIAL_TAG_PPB}"
PP_TAG="${OFFICIAL_TAG_PP}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteClosureTest --collisionType pp  --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PP_TAG" --BaseDir "${OFFICIAL_RESULT_DIR}"
    ./ExecuteClosureTest --collisionType pPb --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG" --BaseDir "${OFFICIAL_RESULT_DIR}"
    ./ExecuteClosureTest --collisionType PbP --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG" --BaseDir "${OFFICIAL_RESULT_DIR}"
}

if [ -n "${CONFIG_FILE:-}" ]; then
    source "$CONFIG_FILE"
    for ZPT in "${ZPT_RANGES[@]}"; do
        for TRKPT in "${PT_RANGES[@]}"; do
            run_one "$ZPT" "$TRKPT"
        done
    done
else
    run_one "0_30"   "0.5_15"
    run_one "30_500" "0.5_15"
fi

exit
