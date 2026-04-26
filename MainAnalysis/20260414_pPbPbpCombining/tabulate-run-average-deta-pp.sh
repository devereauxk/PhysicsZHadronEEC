#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

OUTPUT="${1:-output/run_average_abs_deta_pp.tsv}"

make ExecuteRunAverageDEta

./ExecuteRunAverageDEta \
    --Label pp \
    --Input "${OFFICIAL_DATAINPUT_PP}" \
    --IsPP true --IsData true --IsPPb false \
    --MinZPT 0 --MaxZPT 500 --MinTrackPT 0.5 --MaxTrackPT 15 \
    --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true \
    --UseVZWeight true --UseVZWindow true --UseZScaleFactor true \
    --VZWeightFile "${VZWeightFile_PP}" \
    --ZWeightFile "${ZWeightFile_PP}" \
    --ResidualWeightFile "${RWeightFile_PP}" \
    --Output "${OUTPUT}" \
    --Append false

echo "Run-by-run average |DeltaEta| table written to ${OUTPUT}"
