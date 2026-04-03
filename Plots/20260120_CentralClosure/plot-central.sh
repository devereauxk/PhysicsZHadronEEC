#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteClosureTest

PPB_TAG="${OFFICIAL_TAG_PPB}"
PP_TAG="${OFFICIAL_TAG_PP}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteClosureTest --collisionType pPb --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG"
    ./ExecuteClosureTest --collisionType PbP --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG"
}

run_one "0_10" "0.5_500"
run_one "10_20" "0.5_500"
run_one "20_40" "0.5_500"
run_one "40_500" "0.5_500"

exit
