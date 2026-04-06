#!/bin/bash

set -e

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

input_has_track_mu_dr() {
    local input_file=$1
    local status
    status=$(root -l -b <<EOF
TFile f("${input_file}");
TTree *t = (TTree *)f.Get("Tree");
cout << (t != nullptr && t->GetBranch("trackMuDR") != nullptr ? "yes" : "no") << endl;
.q
EOF
)
    [ "$(echo "${status}" | tail -n 1)" = "yes" ]
}

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
    local VZWEIGHT=$6
    local TAG=$7
    shift 7

    ./system-analysis.sh "${DATAPREFIX}_trkResidual_${TAG}" \
        --IsPP true --IsGenZ false --IsData true --UseVZWeight true \
        --Input "${DATAINPUT}" \
        --MixFile "${DATAINPUT}" \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix ${nMix} \
        --ZWeightFile "${ZWEIGHT}" \
        --ResidualWeightFile "${RWEIGHT}" \
        --EnergyExtraFile "${EEWEIGHT}" \
        --VZWeightFile "${VZWEIGHT}" \
        "$@" "${COMMON_EXTRA_ARGS[@]}"
}

run_pa_data_chain() {
    local DATAPREFIX=$1
    local ISPPB=$2
    local DATAINPUT=$3
    local ZWEIGHT=$4
    local RWEIGHT=$5
    local VZWEIGHT=$6
    local TAG=$7
    shift 7

    ./system-analysis.sh "${DATAPREFIX}_trkResidual_${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb ${ISPPB} \
        --Input "${DATAINPUT}" \
        --MixFile "${DATAINPUT}" \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix ${nMix} \
        --ZWeightFile "${ZWEIGHT}" \
        --ResidualWeightFile "${RWEIGHT}" \
        --VZWeightFile "${VZWEIGHT}" \
        "$@" "${COMMON_EXTRA_ARGS[@]}"
}

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

if [ "$DOPP" == "1" ] && ! input_has_track_mu_dr "${PP_INPUT}"; then
    echo "PP_INPUT=${PP_INPUT} does not provide Tree/trackMuDR." >&2
    echo "Override PP_INPUT with a trackMuDR-enabled pp skim before running pp TrackMuDR/TrackMuClosest variations." >&2
    exit 1
fi

write_config '"5_30" "30_500"' '"0.5_4" "4_500"'
activate_config

if [ "$DOPP" == "1" ]; then
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_IsMuTaggedFalse" --IsMuTagged false --TrackMuDR -1 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuDR0p001" --IsMuTagged true --TrackMuDR 0.001 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuDR0p0025" --IsMuTagged true --TrackMuDR 0.0025 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuDR0p0035" --IsMuTagged true --TrackMuDR 0.0035 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuClosestTrue" --IsMuTagged true --TrackMuDR -1 --TrackMuClosest true
fi

if [ "$DOPPB" == "1" ]; then
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false --TrackMuDR -1 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p001" --IsMuTagged true --TrackMuDR 0.001 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0025" --IsMuTagged true --TrackMuDR 0.0025 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0035" --IsMuTagged true --TrackMuDR 0.0035 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuClosestTrue" --IsMuTagged true --TrackMuDR -1 --TrackMuClosest true
fi

if [ "$DOPBP" == "1" ]; then
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false --TrackMuDR -1 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p001" --IsMuTagged true --TrackMuDR 0.001 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0025" --IsMuTagged true --TrackMuDR 0.0025 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0035" --IsMuTagged true --TrackMuDR 0.0035 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuClosestTrue" --IsMuTagged true --TrackMuDR -1 --TrackMuClosest true
fi

if [ -n "${CONFIG_OVERRIDE}" ]; then
    exit 0
fi

write_config '"5_500"' '"0.5_500"'
activate_config

if [ "$DOPP" == "1" ]; then
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_IsMuTaggedFalse" --IsMuTagged false --TrackMuDR -1 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuDR0p001" --IsMuTagged true --TrackMuDR 0.001 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuDR0p0025" --IsMuTagged true --TrackMuDR 0.0025 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuDR0p0035" --IsMuTagged true --TrackMuDR 0.0035 --TrackMuClosest false
    run_pp_chain "pp" "${PP_INPUT}" "${ZWeightFile_PP}" "${RWeightFile_PP}" "${EEWeightFile_PP}" "${VZWeightFile_PP}" \
        "${OFFICIAL_TAG_PP}_TrackMuClosestTrue" --IsMuTagged true --TrackMuDR -1 --TrackMuClosest true
fi

if [ "$DOPPB" == "1" ]; then
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false --TrackMuDR -1 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p001" --IsMuTagged true --TrackMuDR 0.001 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0025" --IsMuTagged true --TrackMuDR 0.0025 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0035" --IsMuTagged true --TrackMuDR 0.0035 --TrackMuClosest false
    run_pa_data_chain "pPb" true "${PPB_DATAINPUT}" "${ZWeightFile_PPb}" "${RWeightFile_PPb}" "${VZWeightFile_PPb}" \
        "${OFFICIAL_TAG_PPB}_TrackMuClosestTrue" --IsMuTagged true --TrackMuDR -1 --TrackMuClosest true
fi

if [ "$DOPBP" == "1" ]; then
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_IsMuTaggedFalse" --IsMuTagged false --TrackMuDR -1 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p001" --IsMuTagged true --TrackMuDR 0.001 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0025" --IsMuTagged true --TrackMuDR 0.0025 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuDR0p0035" --IsMuTagged true --TrackMuDR 0.0035 --TrackMuClosest false
    run_pa_data_chain "PbP" false "${PBP_DATAINPUT}" "${ZWeightFile_PbP}" "${RWeightFile_PbP}" "${VZWeightFile_PbP}" \
        "${OFFICIAL_TAG_PPB}_TrackMuClosestTrue" --IsMuTagged true --TrackMuDR -1 --TrackMuClosest true
fi
