#!/bin/bash

set -euo pipefail

#source ./clean.sh
make ExecuteCentralOverlayPPbPbPPlot

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
PPB_TAG="${PPB_TAG:-$OFFICIAL_TAG_PPB}"
PPB_MC_TAG="${PPB_MC_TAG:-$PPB_TAG}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteCentralOverlayPPbPbPPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_MC_TAG" --BaseDir "${OFFICIAL_RESULT_DIR}"
}

if [ -n "${CONFIG_FILE:-}" ]; then
    source "$CONFIG_FILE"
    for ZPT in "${ZPT_RANGES[@]}"; do
        for TRKPT in "${PT_RANGES[@]}"; do
            run_one "$ZPT" "$TRKPT"
        done
    done
else
    run_one 0_30 0.5_2
    run_one 0_30 2_4
    run_one 0_30 4_15
    run_one 30_500 0.5_2
    run_one 30_500 2_4
    run_one 30_500 4_15
    run_one 0_500 0.5_15
fi

exit
