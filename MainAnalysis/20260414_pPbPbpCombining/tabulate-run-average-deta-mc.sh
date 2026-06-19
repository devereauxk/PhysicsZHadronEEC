#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

OUTPUT="${1:-output/run_average_abs_deta_mc_no_zcorr.tsv}"

make ExecuteRunAverageDEta

./ExecuteRunAverageDEta \
    --Label pPb \
    --Input "${OFFICIAL_MCRECOINPUT_PPB}" \
    --IsPP false --IsData false --IsPPb true \
    --MinZPT 0 --MaxZPT 500 --MinTrackPT 0.5 --MaxTrackPT 15 \
    --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true \
    --UseVZWeight true --UseVZWindow true --UseZScaleFactor true \
    --VZWeightFile "${VZWeightFile_PPb}" \
    --ZWeightFile "${ZWeightFile_PPb}" \
    --ResidualWeightFile "${RWeightFile_PPb}" \
    --yBoost 0.465 \
    --Output "${OUTPUT}" \
    --Append false

./ExecuteRunAverageDEta \
    --Label PbP \
    --Input "${OFFICIAL_MCRECOINPUT_PBP}" \
    --IsPP false --IsData false --IsPPb false \
    --MinZPT 0 --MaxZPT 500 --MinTrackPT 0.5 --MaxTrackPT 15 \
    --UseEventWeight true --UseTrackWeight true --UseZWeight true --UseResidualWeight true \
    --UseVZWeight true --UseVZWindow true --UseZScaleFactor true \
    --VZWeightFile "${VZWeightFile_PbP}" \
    --ZWeightFile "${ZWeightFile_PbP}" \
    --ResidualWeightFile "${RWeightFile_PbP}" \
    --yBoost 0.465 \
    --Output "${OUTPUT}" \
    --Append true

echo "MC run-by-run average |DeltaEta| table written to ${OUTPUT}"
