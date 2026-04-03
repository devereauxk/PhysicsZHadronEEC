#!/bin/bash

set -euo pipefail

minZpt="${1:-0}"
maxZpt="${2:-500}"
name="${3:-${NAME_TAG:-20260331_ZV7_trkV25_TrackResidualCorrection}}"
if [ "$#" -ge 3 ]; then
    shift 3
else
    shift "$#"
fi
EXTRA_ARGS=("$@")
VARIANT_TAG="${VARIANT_TAG:-}"
VARIANT_SUFFIX=""
if [ -n "$VARIANT_TAG" ]; then
    VARIANT_SUFFIX="_${VARIANT_TAG}"
fi
TRACK_EXTRA_WEIGHT="${TRACK_EXTRA_WEIGHT:-1}"
TRACK_EXTRA_WEIGHT_ARGS=(--TrackExtraWeight "$TRACK_EXTRA_WEIGHT")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

echo "[pPb-DY-analysis] minZpt=${minZpt} maxZpt=${maxZpt} name=${name} variant=${VARIANT_TAG:-Nominal}"
PPB_MCRECOINPUT="${PPB_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PPB}}"
PPB_MCGENINPUT="${PPB_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PPB}}"
PPB_EPOSINPUT="${PPB_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PPB}}"
PBP_MCRECOINPUT="${PBP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PBP}}"
PBP_MCGENINPUT="${PBP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PBP}}"
PBP_EPOSINPUT="${PBP_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PBP}}"
ppb_output="output/${name}_PPb${VARIANT_SUFFIX}_zPt${minZpt}-${maxZpt}.root"
pbp_output="output/${name}_PbP${VARIANT_SUFFIX}_zPt${minZpt}-${maxZpt}.root"

# pPb
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PPB_MCRECOINPUT" --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PPb" --UseVZWeight true --VZWeightFile "$VZWeightFile_PPb"
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PPB_MCGENINPUT" --InputUE "$PPB_EPOSINPUT" --IsGen true --IsPP false --IsGenZ true --UseVZWeight true --VZWeightFile "$VZWeightFile_PPb"
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PPB_MCRECOINPUT" --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PPb" --UseVZWeight true --VZWeightFile "$VZWeightFile_PPb"
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b 'merge_corrections.C("output/correction_1.root","output/correction_2.root","output/correction_12.root")'
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PPB_MCRECOINPUT" --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PPb" --UseVZWeight true --VZWeightFile "$VZWeightFile_PPb"
root -l -q -b correction.C
mv correction.root output/correction_3.root
root -l -q -b plot_corrections.C
mv corrections.pdf "plots/corrections_PPb_${name}${VARIANT_SUFFIX}_${minZpt}_${maxZpt}.pdf"
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"${ppb_output}\")"
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PPB_MCRECOINPUT" --residualFile "$ppb_output" --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PPb" --UseVZWeight true --VZWeightFile "$VZWeightFile_PPb"
root -l -q -b "plot_closure.C(\"plots/isPPb_ZPT${minZpt}_${maxZpt}${VARIANT_SUFFIX}\")"

# PbP
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PBP_MCRECOINPUT" --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PbP" --UseVZWeight true --VZWeightFile "$VZWeightFile_PbP"
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PBP_MCGENINPUT" --InputUE "$PBP_EPOSINPUT" --IsGen true --IsPP false --IsGenZ true --UseVZWeight true --VZWeightFile "$VZWeightFile_PbP"
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PBP_MCRECOINPUT" --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PbP" --UseVZWeight true --VZWeightFile "$VZWeightFile_PbP"
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b 'merge_corrections.C("output/correction_1.root","output/correction_2.root","output/correction_12.root")'
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PBP_MCRECOINPUT" --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PbP" --UseVZWeight true --VZWeightFile "$VZWeightFile_PbP"
root -l -q -b correction.C
mv correction.root output/correction_3.root
root -l -q -b plot_corrections.C
mv corrections.pdf "plots/corrections_PbP_${name}${VARIANT_SUFFIX}_${minZpt}_${maxZpt}.pdf"
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"${pbp_output}\")"
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PBP_MCRECOINPUT" --residualFile "$pbp_output" --IsGen false --IsPP false --IsGenZ false --ZWeightFile "$ZWeightFile_PbP" --UseVZWeight true --VZWeightFile "$VZWeightFile_PbP"
root -l -q -b "plot_closure.C(\"plots/isPbP_ZPT${minZpt}_${maxZpt}${VARIANT_SUFFIX}\")"
