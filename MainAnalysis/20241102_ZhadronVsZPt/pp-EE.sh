#!/bin/bash

DOPP=$1
DOPPB=$2
DOPBP=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
PP_DATAINPUT=${PP_DATAINPUT:-${OFFICIAL_DATAINPUT_PP}}

nMix=10
CONFIG_OVERRIDE=${CONFIG_FILE:-}
CONFIG_TARGET=config.sh

write_config() {
    local zpt_list="$1"
    local pt_list="$2"
    if [ -n "${CONFIG_OVERRIDE}" ]; then
        return
    fi
    cat > "${CONFIG_TARGET}" <<EOF
ZPT_RANGES=(${zpt_list})
PT_RANGES=(${pt_list})
EOF
}

activate_config() {
    export CONFIG_FILE="${CONFIG_OVERRIDE:-${CONFIG_TARGET}}"
}

run_pp_energy_extrapolation() {
    local TAG="$1"

    ./system-analysis.sh "pp_nominal${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight true         --Input "${PP_DATAINPUT}"         --MixFile "${PP_DATAINPUT}"         --UseEventWeight false --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}         --VZWeightFile "${VZWeightFile_PP}"

    ./system-analysis.sh "pp_ZResidual${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight true         --Input "${PP_DATAINPUT}"         --MixFile "${PP_DATAINPUT}"         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWeightFile_PP}"         --VZWeightFile "${VZWeightFile_PP}"

    ./system-analysis.sh "pp_EEtrkResidual${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight true         --Input "${PP_DATAINPUT}"         --MixFile "${PP_DATAINPUT}"         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWeightFile_PP}"         --ResidualWeightFile "${RWeightFile_PP}"         --VZWeightFile "${VZWeightFile_PP}"

    ./system-analysis.sh "pp_EExtrapolation${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight true         --Input "${PP_DATAINPUT}"         --MixFile "${PP_DATAINPUT}"         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWeightFile_PP}"         --ResidualWeightFile "${RWeightFile_PP}"         --EnergyExtraFile "${EEWeightFile_PP}"         --VZWeightFile "${VZWeightFile_PP}"
}

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

TAG="${TAG_SUFFIX:-_${OFFICIAL_TAG_PP}}"

write_config '"0_30" "30_500"' '"0.5_2" "2_4" "4_15"'
activate_config
if [ "$DOPP" == "1" ]; then
    run_pp_energy_extrapolation "${TAG}"
fi

if [ -n "${CONFIG_OVERRIDE}" ]; then
    exit 0
fi

write_config '"0_500"' '"0.5_15"'
activate_config
if [ "$DOPP" == "1" ]; then
    run_pp_energy_extrapolation "${TAG}"
fi
