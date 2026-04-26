#!/bin/bash

set -euo pipefail

#source ./clean.sh
make ExecuteCentralOverlayPPbPbPPlot

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
PPB_TAG="${PPB_TAG:-$OFFICIAL_TAG_PPB}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteCentralOverlayPPbPbPPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG"
}

if [ -n "${CONFIG_FILE:-}" ]; then
    source "$CONFIG_FILE"
    for ZPT in "${ZPT_RANGES[@]}"; do
        for TRKPT in "${PT_RANGES[@]}"; do
            run_one "$ZPT" "$TRKPT"
        done
    done
else
    run_one 5_30 0.5_4
    run_one 5_30 4_500
    run_one 30_500 0.5_4
    run_one 30_500 4_500
    run_one 5_500 0.5_500
fi

exit
