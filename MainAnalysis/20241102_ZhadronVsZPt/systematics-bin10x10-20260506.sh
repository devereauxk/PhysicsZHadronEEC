#!/bin/bash

# Produces 10-bin shifted-geometry systematic variation result files.
#
# Workflow:
#   1. Run all systematic-variation analyses with shifted 10-bin geometry.
#   2. Re-project each raw output using the dedicated hard-coded shifted 10x10 macro.
#   3. Merge per-track files to per-ZPT result files in plots/.

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

make

export STUDY_TAG_SUFFIX="${STUDY_TAG_SUFFIX:-_bin10x10shifted_20260506}"
PP_TAG="${OFFICIAL_TAG_PP}${STUDY_TAG_SUFFIX}"
PPB_TAG="${OFFICIAL_TAG_PPB}${STUDY_TAG_SUFFIX}"

export SKIP_CLEAN=1
export CUT_PARALLELISM="${CUT_PARALLELISM:-1}"
export NTHREAD="${NTHREAD:-25}"
export NSLICE_FACTOR="${NSLICE_FACTOR:-1}"

SHIFTED_RESULT_ARGS=(--ResultDEtaBins 10 --ResultDPhiBins 10)
SCAN_CONFIG="$THISDIR/config_note_bin10x10_scan_20260506.sh"
INCLUSIVE_CONFIG="$THISDIR/config_note_bin10x10_inclusive_20260506.sh"

# ---- Step 1: Run systematic variation analyses (shifted geometry, shifted tag) ----
# STUDY_TAG_SUFFIX is read by systematics.sh after its internal OfficialWeightDictionary.sh source.
CONFIG_FILE="$SCAN_CONFIG" ./systematics.sh 1 1 1 "${SHIFTED_RESULT_ARGS[@]}"
CONFIG_FILE="$INCLUSIVE_CONFIG" ./systematics.sh 1 1 1 "${SHIFTED_RESULT_ARGS[@]}"

# ---- Step 2: Re-project with hard-coded 10-bin shifted window ----
# Overwrites the wrong-window projections that system-analysis.sh emits internally.

NOTE_BINS=(
    "0_500:0.5_15"
    "0_30:0.5_2"
    "0_30:2_4"
    "0_30:4_15"
    "30_500:0.5_2"
    "30_500:2_4"
    "30_500:4_15"
)

project_one() {
    local prefix=$1
    local zpt=$2
    local track=$3

    local input="output/${prefix}_ZPT${zpt}-${track}.root"
    local tmpNoSub="output/${prefix}_ZPT${zpt}-${track}-nosub.root"
    local tmpResult="output/${prefix}_ZPT${zpt}-${track}-result.root"

    root -l -q -b "makeProjectionShifted10x10.C(\"${input}\",\"${tmpNoSub}\",\"${track}\",0)" >/dev/null
    root -l -q -b "makeProjectionShifted10x10.C(\"${input}\",\"${tmpResult}\",\"${track}\",1)" >/dev/null
}

merge_zpt() {
    local prefix=$1
    local zpt=$2
    shift 2
    local tracks=("$@")
    local nosubInputs=()
    local resultInputs=()
    local track

    for track in "${tracks[@]}"; do
        nosubInputs+=("output/${prefix}_ZPT${zpt}-${track}-nosub.root")
        resultInputs+=("output/${prefix}_ZPT${zpt}-${track}-result.root")
    done

    hadd -f "plots/${prefix}_ZPT${zpt}-nosub.root" "${nosubInputs[@]}" >/dev/null
    hadd -f "plots/${prefix}_ZPT${zpt}-result.root" "${resultInputs[@]}" >/dev/null
    rm -f "${nosubInputs[@]}" "${resultInputs[@]}"
}

process_variation() {
    local prefix=$1
    local zpt
    local track

    for entry in "${NOTE_BINS[@]}"; do
        IFS=':' read -r zpt track <<< "$entry"
        project_one "$prefix" "$zpt" "$track"
    done

    merge_zpt "$prefix" "0_500" "0.5_15"
    merge_zpt "$prefix" "0_30" "0.5_2" "2_4" "4_15"
    merge_zpt "$prefix" "30_500" "0.5_2" "2_4" "4_15"
}

PP_VARIATIONS=(
    "Loose" "Tight" "IsMuTaggedFalse" "IsPURejectTrue"
    "MuVar0" "MuVar1" "MuVar2" "MuVar3"
    "EEPrivate"
    "TrackCorrection0p976" "TrackCorrection1p024"
)

PA_VARIATIONS=(
    "Loose" "Tight" "IsMuTaggedFalse" "IsPURejectTrue"
    "MuVar0" "MuVar1" "MuVar2" "MuVar3"
    "TrackCorrection0p976" "TrackCorrection1p024"
)

for variation in "${PP_VARIATIONS[@]}"; do
    process_variation "pp_trkResidual_${PP_TAG}_${variation}"
done

for variation in "${PA_VARIATIONS[@]}"; do
    process_variation "pPb_trkResidual_${PPB_TAG}_${variation}"
    process_variation "PbP_trkResidual_${PPB_TAG}_${variation}"
done
