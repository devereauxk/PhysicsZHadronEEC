#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make

TAG="${1:-${OFFICIAL_TAG_PPB}}"
PLOT_SYSTEMS=(${PLOT_SYSTEMS:-pp pPb PbP})

for zPtRange in "0_10" "10_20" "20_40" "40_500"
do
    echo "Processing zPtRange: $zPtRange"
    for system in "${PLOT_SYSTEMS[@]}"
    do
        ./ExecuteClosureTest --collisionType "$system" --zPtRange "$zPtRange" --trkPtRange 0.5_500 --tag "$TAG"
    done
done
