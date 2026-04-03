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

run_one "5_30" "0.5_4"
run_one "5_30" "4_500"
run_one "30_500" "0.5_4"
run_one "30_500" "4_500"
run_one "5_500" "0.5_500"

exit
