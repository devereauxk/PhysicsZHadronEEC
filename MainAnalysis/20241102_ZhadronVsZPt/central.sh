DOPP=$1
DOPPB=$2
DOPBP=$3

shift 3
COMMON_EXTRA_ARGS=("$@")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

nMix=10
CONFIG_OVERRIDE=${CONFIG_FILE:-}
CONFIG_TARGET=config.sh

PP_DATAINPUT=${PP_DATAINPUT:-${OFFICIAL_DATAINPUT_PP}}
PPB_MCGENINPUT=${PPB_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PPB}}
PPB_EPOSINPUT=${PPB_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PPB}}
PPB_DATAINPUT=${PPB_DATAINPUT:-${OFFICIAL_DATAINPUT_PPB}}

PBP_MCGENINPUT=${PBP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PBP}}
PBP_EPOSINPUT=${PBP_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PBP}}
PBP_DATAINPUT=${PBP_DATAINPUT:-${OFFICIAL_DATAINPUT_PBP}}

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

run_ppb_chain() {
    local MCPREFIX=$1
    local DATAPREFIX=$2
    local ISPPB=$3
    local MCGENINPUT=$4
    local EPOSINPUT=$5
    local DATAINPUT=$6
    local ZWEIGHT=$7
    local RWEIGHT=$8
    local VZWEIGHT=$9
    local TAG=${10}

    ./system-analysis.sh "${MCPREFIX}_Gen_nominal_${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb ${ISPPB}         --Input "${MCGENINPUT}"         --MixFile "${MCGENINPUT}"         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile "${EPOSINPUT}"         "${COMMON_EXTRA_ARGS[@]}"         --Fraction 1         --yBoost 0 --nMix ${nMix}         --UseVZWeight true --VZWeightFile "${VZWEIGHT}"

    ./system-analysis.sh "${DATAPREFIX}_nominal_${TAG}"         --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB}         --Input "${DATAINPUT}"         --MixFile "${DATAINPUT}"         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}         "${COMMON_EXTRA_ARGS[@]}"

    ./system-analysis.sh "${DATAPREFIX}_ZResidual_${TAG}"         --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB}         --Input "${DATAINPUT}"         --MixFile "${DATAINPUT}"         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWEIGHT}"         "${COMMON_EXTRA_ARGS[@]}"

    ./system-analysis.sh "${DATAPREFIX}_trkResidual_${TAG}"         --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB}         --Input "${DATAINPUT}"         --MixFile "${DATAINPUT}"         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWEIGHT}"         --ResidualWeightFile "${RWEIGHT}"         "${COMMON_EXTRA_ARGS[@]}"
}

# Speedup path for repeated system-analysis calls:
# build once, then skip per-call clean/rebuild and allow optional cross-cut parallelism.
if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

# CENTRAL VALUES (requested): 4 selections
write_config '"5_30" "30_500"' '"0.5_4" "4_500"'
activate_config

if [ "$DOPP" == "1" ]; then
    TAG="${OFFICIAL_TAG_PP}"
    ./system-analysis.sh "pp_trkResidual_${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight false         --Input "${PP_DATAINPUT}"         --MixFile "${PP_DATAINPUT}"         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWeightFile_PP}"         --ResidualWeightFile "${RWeightFile_PP}"         --EnergyExtraFile "${EEWeightFile_PP}"         "${COMMON_EXTRA_ARGS[@]}"
fi

if [ "$DOPPB" == "1" ]; then
    TAG="${OFFICIAL_TAG_PPB}"
    run_ppb_chain "pPbMC" "pPb" true         "${PPB_MCGENINPUT}"         "${PPB_EPOSINPUT}"         "${PPB_DATAINPUT}"         "${ZWeightFile_PPb}"         "${RWeightFile_PPb}"         "${VZWeightFile_PPb}"         "${TAG}"
fi

if [ "$DOPBP" == "1" ]; then
    TAG="${OFFICIAL_TAG_PPB}"
    run_ppb_chain "PbPMC" "PbP" false         "${PBP_MCGENINPUT}"         "${PBP_EPOSINPUT}"         "${PBP_DATAINPUT}"         "${ZWeightFile_PbP}"         "${RWeightFile_PbP}"         "${VZWeightFile_PbP}"         "${TAG}"
fi

if [ -n "${CONFIG_OVERRIDE}" ]; then
    exit 0
fi

# INCLUSIVE selection
write_config '"5_500"' '"0.5_500"'
activate_config

if [ "$DOPPB" == "1" ]; then
    TAG="${OFFICIAL_TAG_PPB}"
    run_ppb_chain "pPbMC" "pPb" true         "${PPB_MCGENINPUT}"         "${PPB_EPOSINPUT}"         "${PPB_DATAINPUT}"         "${ZWeightFile_PPb}"         "${RWeightFile_PPb}"         "${VZWeightFile_PPb}"         "${TAG}"
fi

if [ "$DOPBP" == "1" ]; then
    TAG="${OFFICIAL_TAG_PPB}"
    run_ppb_chain "PbPMC" "PbP" false         "${PBP_MCGENINPUT}"         "${PBP_EPOSINPUT}"         "${PBP_DATAINPUT}"         "${ZWeightFile_PbP}"         "${RWeightFile_PbP}"         "${VZWeightFile_PbP}"         "${TAG}"
fi
