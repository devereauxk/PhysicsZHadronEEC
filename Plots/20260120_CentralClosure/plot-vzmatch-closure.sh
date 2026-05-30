#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteVZMatchClosureTest

mkdir -p plots/vzmatch/pp plots/vzmatch/pPb plots/vzmatch/PbP

TRKPT="0.5_15"

PP_TAG="${OFFICIAL_TAG_PP}_vzmatch0.5cm"
PPB_TAG="${OFFICIAL_TAG_PPB}_vzmatch0.5cm"

for ZPT in "0_30" "30_500"; do
    ./ExecuteVZMatchClosureTest --collisionType pp  --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PP_TAG"
    ./ExecuteVZMatchClosureTest --collisionType pPb --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG"
    ./ExecuteVZMatchClosureTest --collisionType PbP --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$PPB_TAG"
done
