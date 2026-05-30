#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteClosureTest

PPB_TAG="${OFFICIAL_TAG_PPB}"
PP_TAG="${OFFICIAL_TAG_PP}"

PP_DVZ_TAG="${PP_TAG}_vzmatch0.5cm"
PPB_DVZ_TAG="${PPB_TAG}_vzmatch0.5cm"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteClosureTest --collisionType pp  --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PP_TAG"  --tagDeltaVZ "$PP_DVZ_TAG"
    ./ExecuteClosureTest --collisionType pPb --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG" --tagDeltaVZ "$PPB_DVZ_TAG"
    ./ExecuteClosureTest --collisionType PbP --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG" --tagDeltaVZ "$PPB_DVZ_TAG"
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
