#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source clean.sh

PLOT_SYSTEMS=(${PLOT_SYSTEMS:-pPb PbP})
for system in "${PLOT_SYSTEMS[@]}"; do
    case "$system" in
        pPb|PbP)
            ./ExecuteZMassPlot --collisionType "$system" --zPtRange 0_500 --trkPtRange 0.5_500 --tag "$OFFICIAL_TAG_PPB"
            ;;
        pp)
            ./ExecuteZMassPlot --collisionType pp --zPtRange 5_500 --trkPtRange 0.5_500 --tag "$OFFICIAL_TAG_PP"
            ;;
    esac
done

exit
