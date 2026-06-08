#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
make ExecuteEnergyExtrapolationPlot

mkdir -p plots/energyExtrapolation

./ExecuteEnergyExtrapolationPlot \
    --trkPtRange "0.5_15" \
    --pptag "$OFFICIAL_TAG_PP" \
    --BaseDir "${OFFICIAL_RESULT_DIR}" \
    --outputDir "plots/energyExtrapolation"

exit
