#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
source clean.sh

PLOT_SYSTEMS=(${PLOT_SYSTEMS:-pPb PbP})
for system in "${PLOT_SYSTEMS[@]}"; do
    case "$system" in
        pPb|PbP)
            ./ExecuteZMassPlot --collisionType "$system" --zPtRange 0_500 --trkPtRange 0.5_15 --tag "$OFFICIAL_TAG_PPB" --BaseDir "${OFFICIAL_RESULT_DIR}"
            ;;
        pp)
            ./ExecuteZMassPlot --collisionType pp --zPtRange 0_500 --trkPtRange 0.5_15 --tag "$OFFICIAL_TAG_PP" --BaseDir "${OFFICIAL_RESULT_DIR}"
            ;;
    esac
done

exit
