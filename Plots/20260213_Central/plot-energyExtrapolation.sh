#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteEnergyExtrapolationPlot

run_one() {
    local zPtRange=$1
    local trkPtRange=$2
    echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"
    ./ExecuteEnergyExtrapolationPlot --zPtRange "$zPtRange" --trkPtRange "$trkPtRange" --pptag "$OFFICIAL_TAG_PP"
}

if [ -n "${CONFIG_FILE:-}" ]; then
    source "$CONFIG_FILE"
    for zPtRange in "${ZPT_RANGES[@]}"; do
        for trkPtRange in "${PT_RANGES[@]}"; do
            run_one "$zPtRange" "$trkPtRange"
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
