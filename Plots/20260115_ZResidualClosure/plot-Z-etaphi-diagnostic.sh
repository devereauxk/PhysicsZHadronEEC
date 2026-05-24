#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$(dirname "$0")"
make ExecuteZEtaPhiDiagnostic

INPUT_TAG="${INPUT_TAG:-20260407_ZCorrection_V8}"

for system in pp pPb PbP
do
    if [ "$system" = "pp" ]; then
        DATA_TAG="${OFFICIAL_TAG_PP}"
    else
        DATA_TAG="${OFFICIAL_TAG_PPB}"
    fi

    mkdir -p "plots/${system}"

    echo "=== ${system} ==="
    ./ExecuteZEtaPhiDiagnostic \
        --collisionType "$system" \
        --inputTag "$INPUT_TAG" \
        --dataTag "$DATA_TAG" \
        --outputDir plots
done
