#!/bin/bash
# Run trkResidual corrected-data legs for pPb and PbP with sub-binned track pT
# (0.5_1, 1_2) for ZPT ranges 0_30, 30_500, 0_500.
# New histogram keys are hadd-appended to the existing combined result files
# so the overlay plotter can find DeltaEta/Phi_Result0.5_1 etc.

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

TAG="${OFFICIAL_TAG_PPB}"
nMix=10
export NTHREAD=${NTHREAD:-20}

ZPT_RANGES=(0_30 30_500 0_500)
PT_RANGES=(0.5_1 1_2)

COMMON=(
    --IsGenZ false --IsData true --UseVZWeight true
    --UseEventWeight true --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --yBoost 0 --nMix ${nMix} --UseJackknife true
    --IsPP false
)

run_new_bins() {
    local PREFIX=$1
    local ISPPB=$2
    local INPUT=$3
    local ZWEIGHT=$4
    local RWEIGHT=$5
    local VZWEIGHT=$6

    for zpt in "${ZPT_RANGES[@]}"; do
        local min_zpt=${zpt/_*/}
        local max_zpt=${zpt/*_/}
        for pt in "${PT_RANGES[@]}"; do
            local min_pt=${pt/_*/}
            local max_pt=${pt/*_/}
            echo "=== ${PREFIX} ZPT${zpt} trkPT${pt} ==="
            ./threader.sh "output/${PREFIX}_${TAG}_ZPT${min_zpt}_${max_zpt}" "${pt}" \
                "${COMMON[@]}" \
                --IsPPb ${ISPPB} \
                --Input "${INPUT}" \
                --MixFile "${INPUT}" \
                --ZWeightFile "${ZWEIGHT}" \
                --ResidualWeightFile "${RWEIGHT}" \
                --VZWeightFile "${VZWEIGHT}" \
                --MinTrackPT "${min_pt}" --MaxTrackPT "${max_pt}" \
                --MinZPT "${min_zpt}" --MaxZPT "${max_zpt}"
        done
    done
}

# Run analysis legs (sequential to avoid resource overload)
run_new_bins "pPb_trkResidual"  true  \
    "${OFFICIAL_DATAINPUT_PPB}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}"

run_new_bins "PbP_trkResidual"  false \
    "${OFFICIAL_DATAINPUT_PBP}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}"

# Append new per-pt result files into existing combined result files.
# hadd -a only appends TTrees; use a ROOT macro to copy missing keys.
echo "=== Appending new histogram keys to combined result files ==="
for PREFIX in pPb_trkResidual PbP_trkResidual; do
    for zpt in "${ZPT_RANGES[@]}"; do
        min_zpt=${zpt/_*/}
        max_zpt=${zpt/*_/}
        COMBINED="plots/${PREFIX}_${TAG}_ZPT${min_zpt}_${max_zpt}-result.root"
        if [ ! -f "${COMBINED}" ]; then
            echo "Warning: combined result file not found: ${COMBINED}" >&2
            continue
        fi
        for pt in "${PT_RANGES[@]}"; do
            f="output/${PREFIX}_${TAG}_ZPT${min_zpt}_${max_zpt}-${pt}-result.root"
            if [ -f "$f" ]; then
                echo "  appending ${f} -> ${COMBINED}"
                root -l -b -q "appendSubbinKeys.C(\"${COMBINED}\",\"${f}\")"
            fi
        done
    done
done

echo "=== Done. New track pT sub-bins appended to result files. ==="
