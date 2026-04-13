#!/bin/bash

set -euo pipefail

minZpt="${1:-0}"
maxZpt="${2:-500}"
name="${3:-${NAME_TAG:-20260412_ZV8_trkV26_TrackResidualCorrection}}"
if [ "$#" -ge 3 ]; then
    shift 3
else
    shift "$#"
fi
EXTRA_ARGS=("$@")
COMMON_EXTRA_ARGS=()
SHARED_TRACK_ARGS=()
RECO_ONLY_EXTRA_ARGS=()
split_analysis_args() {
    local index=0
    while [ "$index" -lt "${#EXTRA_ARGS[@]}" ]; do
        local arg="${EXTRA_ARGS[$index]}"
        local value=""
        local step=1
        if [ $((index + 1)) -lt "${#EXTRA_ARGS[@]}" ] && [[ "${EXTRA_ARGS[$((index + 1))]}" != --* ]]; then
            value="${EXTRA_ARGS[$((index + 1))]}"
            step=2
        fi
        case "$arg" in
            --TrackSelectionMode|--TrackExtraWeight)
                RECO_ONLY_EXTRA_ARGS+=("$arg")
                if [ "$step" -eq 2 ]; then
                    RECO_ONLY_EXTRA_ARGS+=("$value")
                fi
                ;;
            --IsPUReject)
                ;;
            --IsMuTagged|--TrackMuDR|--TrackMuClosest)
                SHARED_TRACK_ARGS+=("$arg")
                if [ "$step" -eq 2 ]; then
                    SHARED_TRACK_ARGS+=("$value")
                fi
                ;;
            *)
                COMMON_EXTRA_ARGS+=("$arg")
                if [ "$step" -eq 2 ]; then
                    COMMON_EXTRA_ARGS+=("$value")
                fi
                ;;
        esac
        index=$((index + step))
    done
}
split_analysis_args
RECO_ANALYSIS_ARGS=("${COMMON_EXTRA_ARGS[@]}" "${SHARED_TRACK_ARGS[@]}" "${RECO_ONLY_EXTRA_ARGS[@]}")
GEN_ANALYSIS_ARGS=("${COMMON_EXTRA_ARGS[@]}" "${SHARED_TRACK_ARGS[@]}")
VARIANT_TAG="${VARIANT_TAG:-}"
VARIANT_SUFFIX=""
if [ -n "$VARIANT_TAG" ]; then
    VARIANT_SUFFIX="_${VARIANT_TAG}"
fi
TRACK_EXTRA_WEIGHT="${TRACK_EXTRA_WEIGHT:-1}"
TRACK_EXTRA_WEIGHT_ARGS=(--TrackExtraWeight "$TRACK_EXTRA_WEIGHT")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
ZWeightFile_pp="${ZWeightFile_PP}"
VZWeightFile="${VZWeightFile_PP}"
PP_MCRECOINPUT="${PP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PP}}"
PP_MCGENINPUT="${PP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}"
output_file="output/${name}_pp${VARIANT_SUFFIX}_zPt${minZpt}-${maxZpt}.root"
closure_dir="output/closure_inputs"

./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${RECO_ANALYSIS_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --Input "$PP_MCRECOINPUT" --IsGen false --IsPP true --IsGenZ false --ZWeightFile "$ZWeightFile_pp" --UseVZWeight true --VZWeightFile "$VZWeightFile"
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN "${GEN_ANALYSIS_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --Input "$PP_MCGENINPUT" --IsGen true --IsPP true --IsGenZ true --UseVZWeight true --VZWeightFile "$VZWeightFile"
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${RECO_ANALYSIS_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --Input "$PP_MCRECOINPUT" --residualFile output/correction_1.root --IsGen false --IsPP true --IsGenZ false --ZWeightFile "$ZWeightFile_pp" --UseVZWeight true --VZWeightFile "$VZWeightFile"
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${RECO_ANALYSIS_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --Input "$PP_MCRECOINPUT" --residualFile output/correction_12.root --IsGen false --IsPP true --IsGenZ false --ZWeightFile "$ZWeightFile_pp" --UseVZWeight true --VZWeightFile "$VZWeightFile"
root -l -q -b correction.C
mv correction.root output/correction_3.root
root -l -q -b plot_corrections.C
mv corrections.pdf "plots/corrections_pp_${name}${VARIANT_SUFFIX}_${minZpt}_${maxZpt}.pdf"
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"${output_file}\")"
./finalAnalysis.sh output/DY RECO "${TRACK_EXTRA_WEIGHT_ARGS[@]}" "${RECO_ANALYSIS_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --Input "$PP_MCRECOINPUT" --residualFile "$output_file" --IsGen false --IsPP true --IsGenZ false --ZWeightFile "$ZWeightFile_pp" --UseVZWeight true --VZWeightFile "$VZWeightFile"
mkdir -p "$closure_dir"
cp output/DY-GEN.root "$closure_dir/pp_${name}${VARIANT_SUFFIX}_zPt${minZpt}-${maxZpt}_gen.root"
cp output/DY-RECO-noResidual.root "$closure_dir/pp_${name}${VARIANT_SUFFIX}_zPt${minZpt}-${maxZpt}_reco.root"
cp output/DY-RECO.root "$closure_dir/pp_${name}${VARIANT_SUFFIX}_zPt${minZpt}-${maxZpt}_corrected.root"
root -l -q -b "plot_closure.C(\"plots/closure_pp_${name}${VARIANT_SUFFIX}_${minZpt}_${maxZpt}\")"
