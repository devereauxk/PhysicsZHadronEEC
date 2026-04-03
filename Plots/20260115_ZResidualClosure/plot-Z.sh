#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make

TAG="${1:-${OFFICIAL_TAG_PPB/nmix10/nmix0}}"
PLOT_SYSTEMS=(${PLOT_SYSTEMS:-pp pPb PbP})

for zPtRange in "0_500"
do
    for trkPtRange in "0.5_500"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"
        for system in "${PLOT_SYSTEMS[@]}"
        do
            ./ExecuteClosureTest --collisionType "$system" --zPtRange "$zPtRange" --trkPtRange "$trkPtRange" --tag "$TAG"
        done
    done
done

exit
