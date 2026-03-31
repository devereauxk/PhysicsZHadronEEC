#!/bin/bash

DOPP=$1
DOPPB=$2
DOPBP=$3

shift 3
COMMON_EXTRA_ARGS=("$@")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

nMix=10
CONFIG_OVERRIDE=${CONFIG_FILE:-}
CONFIG_TARGET=config.sh

PP_INPUT=${PP_INPUT:-${OFFICIAL_DATAINPUT_PP}}
PPB_DATAINPUT=${PPB_DATAINPUT:-${OFFICIAL_DATAINPUT_PPB}}
PBP_DATAINPUT=${PBP_DATAINPUT:-${OFFICIAL_DATAINPUT_PBP}}

write_config() {
    local zpt_list="$1"
    local pt_list="$2"
    if [ -n "${CONFIG_OVERRIDE}" ]; then
        return
    fi
    cat > "${CONFIG_TARGET}" <<CFG
ZPT_RANGES=(${zpt_list})
PT_RANGES=(${pt_list})
CFG
}

activate_config() {
    export CONFIG_FILE="${CONFIG_OVERRIDE:-${CONFIG_TARGET}}"
}

run_pp_chain() {
    local DATAPREFIX=$1
    local DATAINPUT=$2
    local ZWEIGHT=$3
    local RWEIGHT=$4
    local EEWEIGHT=$5
    local TAG=$6
    shift 6

    ./system-analysis.sh "${DATAPREFIX}_trkResidual_${TAG}" \
        --IsPP true --IsGenZ false --IsData true --UseVZWeight false \
        --Input "${DATAINPUT}" \
        --MixFile "${DATAINPUT}" \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix ${nMix} \
        --ZWeightFile "${ZWEIGHT}" \
        --ResidualWeightFile "${RWEIGHT}" \
        --EnergyExtraFile "${EEWEIGHT}" \
        "$@" "${COMMON_EXTRA_ARGS[@]}"
}

run_pa_data_chain() {
    local DATAPREFIX=$1
    local ISPPB=$2
    local DATAINPUT=$3
    local ZWEIGHT=$4
    local RWEIGHT=$5
    local TAG=$6
    shift 6

    ./system-analysis.sh "${DATAPREFIX}_trkResidual_${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB} \
        --Input "${DATAINPUT}" \
        --MixFile "${DATAINPUT}" \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix ${nMix} \
        --ZWeightFile "${ZWEIGHT}" \
        --ResidualWeightFile "${RWEIGHT}" \
        "$@" "${COMMON_EXTRA_ARGS[@]}"
}

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

write_config '"5_30" "30_500"' '"0.5_4" "4_500"'
activate_config

if [ "$DOPP" == "1" ]; then
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_Loose" --TrackSelectionMode Loose
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_Tight" --TrackSelectionMode Tight
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_IsMuTaggedFalse" --IsMuTagged false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_IsPURejectFalse" --IsPUReject false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_MuVar0" --ExtraZWeight 0
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_MuVar1" --ExtraZWeight 1
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_MuVar2" --ExtraZWeight 2
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${OFFICIAL_TAG_PP}_MuVar3" --ExtraZWeight 3
fi

if [ "$DOPPB" == "1" ]; then
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_Loose" --TrackSelectionMode Loose
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_Tight" --TrackSelectionMode Tight
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_IsPURejectFalse" --IsPUReject false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar0" --ExtraZWeight 0
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar1" --ExtraZWeight 1
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar2" --ExtraZWeight 2
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar3" --ExtraZWeight 3
fi

if [ "$DOPBP" == "1" ]; then
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_Loose" --TrackSelectionMode Loose
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_Tight" --TrackSelectionMode Tight
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_IsPURejectFalse" --IsPUReject false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar0" --ExtraZWeight 0
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar1" --ExtraZWeight 1
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar2" --ExtraZWeight 2
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar3" --ExtraZWeight 3
fi

if [ -n "${CONFIG_OVERRIDE}" ]; then
    exit 0
fi

write_config '"5_500"' '"0.5_500"'
activate_config

if [ "$DOPPB" == "1" ]; then
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_Loose" --TrackSelectionMode Loose
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_Tight" --TrackSelectionMode Tight
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_IsPURejectFalse" --IsPUReject false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar0" --ExtraZWeight 0
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar1" --ExtraZWeight 1
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar2" --ExtraZWeight 2
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${OFFICIAL_TAG_PPB}_MuVar3" --ExtraZWeight 3
fi

if [ "$DOPBP" == "1" ]; then
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_Loose" --TrackSelectionMode Loose
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_Tight" --TrackSelectionMode Tight
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_IsPURejectFalse" --IsPUReject false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar0" --ExtraZWeight 0
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar1" --ExtraZWeight 1
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar2" --ExtraZWeight 2
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${OFFICIAL_TAG_PPB}_MuVar3" --ExtraZWeight 3
fi
