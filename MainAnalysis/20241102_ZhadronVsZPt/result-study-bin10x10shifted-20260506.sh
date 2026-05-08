#!/bin/bash

# Runs the maintained 10-bin shifted geometry analysis (note-facing bins only).
# DeltaEta: 10 bins [-4, 4]; DeltaPhi: 10 bins [-3pi/5, 7pi/5].
# Outputs are tagged _bin10x10shifted_20260506.

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

make

STUDY_SUFFIX="${STUDY_SUFFIX:-_bin10x10shifted_20260506}"
PP_TAG="${PP_TAG_OVERRIDE:-EEV5_ZV9_trkV27_nmix10${STUDY_SUFFIX}}"
PPB_TAG="${PPB_TAG_OVERRIDE:-ZV9_trkV27_nmix10${STUDY_SUFFIX}}"
NMIX="${NMIX:-10}"

export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD="${NTHREAD:-25}"
export NSLICE_FACTOR=1

SHIFTED_RESULT_ARGS=(--ResultDEtaBins 10 --ResultDPhiBins 10)
SCAN_CONFIG="$THISDIR/config_note_bin10x10_scan_20260506.sh"
INCLUSIVE_CONFIG="$THISDIR/config_note_bin10x10_inclusive_20260506.sh"

run_with_configs() {
    local prefix=$1
    shift
    for config in "$SCAN_CONFIG" "$INCLUSIVE_CONFIG"; do
        CONFIG_FILE="$config" ./system-analysis.sh "$prefix" "$@" "${SHIFTED_RESULT_ARGS[@]}"
    done
}

run_with_configs "pp_trkResidual_${PP_TAG}" \
    --IsPP true --IsGenZ false --IsData true --UseVZWeight true \
    --Input "$OFFICIAL_DATAINPUT_PP" \
    --MixFile "$OFFICIAL_DATAINPUT_PP" \
    --UseEventWeight false --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseJackknife true \
    --yBoost 0 --nMix "$NMIX" \
    --ZWeightFile "$ZWeightFile_PP" \
    --ResidualWeightFile "$RWeightFile_PP" \
    --EnergyExtraFile "$EEWeightFile_PP" \
    --VZWeightFile "$VZWeightFile_PP"

run_with_configs "pPb_trkResidual_${PPB_TAG}" \
    --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb true \
    --Input "$OFFICIAL_DATAINPUT_PPB" \
    --MixFile "$OFFICIAL_DATAINPUT_PPB" \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseJackknife true \
    --yBoost 0 --nMix "$NMIX" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb"

run_with_configs "PbP_trkResidual_${PPB_TAG}" \
    --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb false \
    --Input "$OFFICIAL_DATAINPUT_PBP" \
    --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseJackknife true \
    --yBoost 0 --nMix "$NMIX" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

run_with_configs "pPbMC_Gen_nominal_${PPB_TAG}" \
    --IsPP false --IsGenZ true --IsData false --IsPPb true \
    --Input "$OFFICIAL_MCGENINPUT_PPB" \
    --MixFile "$OFFICIAL_MCGENINPUT_PPB" \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile "$OFFICIAL_EPOSINPUT_PPB" \
    --yBoost 0 --nMix "$NMIX" \
    --UseVZWeight true --VZWeightFile "$VZWeightFile_PPb"

run_with_configs "PbPMC_Gen_nominal_${PPB_TAG}" \
    --IsPP false --IsGenZ true --IsData false --IsPPb false \
    --Input "$OFFICIAL_MCGENINPUT_PBP" \
    --MixFile "$OFFICIAL_MCGENINPUT_PBP" \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile "$OFFICIAL_EPOSINPUT_PBP" \
    --yBoost 0 --nMix "$NMIX" \
    --UseVZWeight true --VZWeightFile "$VZWeightFile_PbP"

# ---- Step 2: Re-project all output hadd files with the maintained shifted 10x10 path ----
# threader.sh always writes the official 20x20 projection; override it here with the maintained shifted macro.
SCAN_ZPT=("0_30" "30_500")
SCAN_PT=("0.5_2" "2_4" "4_15")
INC_ZPT=("0_500")
INC_PT=("0.5_15")
ALL_PREFIXES=(
    "pp_trkResidual_${PP_TAG}"
    "pPb_trkResidual_${PPB_TAG}"
    "PbP_trkResidual_${PPB_TAG}"
    "pPbMC_Gen_nominal_${PPB_TAG}"
    "PbPMC_Gen_nominal_${PPB_TAG}"
)
for pfx in "${ALL_PREFIXES[@]}"; do
    for zpt in "${SCAN_ZPT[@]}"; do
        for pt in "${SCAN_PT[@]}"; do
            base="output/${pfx}_ZPT${zpt}-${pt}"
            root -l -q -b "makeProjectionShifted10x10.C(\"${base}.root\",\"${base}-nosub.root\",\"${pt}\",0)"
            root -l -q -b "makeProjectionShifted10x10.C(\"${base}.root\",\"${base}-result.root\",\"${pt}\",1)"
        done
    done
    for zpt in "${INC_ZPT[@]}"; do
        for pt in "${INC_PT[@]}"; do
            base="output/${pfx}_ZPT${zpt}-${pt}"
            root -l -q -b "makeProjectionShifted10x10.C(\"${base}.root\",\"${base}-nosub.root\",\"${pt}\",0)"
            root -l -q -b "makeProjectionShifted10x10.C(\"${base}.root\",\"${base}-result.root\",\"${pt}\",1)"
        done
    done
done

# ---- Step 3: Re-hadd into ZPT-grouped plots/ files ----
for pfx in "${ALL_PREFIXES[@]}"; do
    for zpt in "${SCAN_ZPT[@]}"; do
        hadd -f "plots/${pfx}_ZPT${zpt}-nosub.root" $(for pt in "${SCAN_PT[@]}"; do echo "output/${pfx}_ZPT${zpt}-${pt}-nosub.root"; done)
        hadd -f "plots/${pfx}_ZPT${zpt}-result.root" $(for pt in "${SCAN_PT[@]}"; do echo "output/${pfx}_ZPT${zpt}-${pt}-result.root"; done)
    done
    for zpt in "${INC_ZPT[@]}"; do
        hadd -f "plots/${pfx}_ZPT${zpt}-nosub.root" $(for pt in "${INC_PT[@]}"; do echo "output/${pfx}_ZPT${zpt}-${pt}-nosub.root"; done)
        hadd -f "plots/${pfx}_ZPT${zpt}-result.root" $(for pt in "${INC_PT[@]}"; do echo "output/${pfx}_ZPT${zpt}-${pt}-result.root"; done)
    done
done
