#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteCentralPlot

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteCentralPlot --collisionType pPb --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$OFFICIAL_TAG_PPB" --pptag "$OFFICIAL_TAG_PP"
    ./ExecuteCentralPlot --collisionType PbP --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$OFFICIAL_TAG_PPB" --pptag "$OFFICIAL_TAG_PP"
}

run_one "0_30" "0.5_2"
run_one "0_30" "2_4"
run_one "0_30" "4_15"
run_one "30_500" "0.5_2"
run_one "30_500" "2_4"
run_one "30_500" "4_15"
run_one "0_500" "0.5_15"

exit
