
DOPP=$1
DOPPB=$2
DOPBP=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

nMixPP=0
nMixPA=0

PP_MCRECOINPUT=${PP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PP}}
PP_MCGENINPUT=${PP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}
PP_DATAINPUT=${PP_DATAINPUT:-${OFFICIAL_DATAINPUT_PP}}
PPB_MCRECOINPUT=${PPB_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PPB}}
PPB_MCGENINPUT=${PPB_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PPB}}
PPB_EPOSINPUT=${PPB_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PPB}}
PPB_DATAINPUT=${PPB_DATAINPUT:-${OFFICIAL_DATAINPUT_PPB}}
PBP_MCRECOINPUT=${PBP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PBP}}
PBP_MCGENINPUT=${PBP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PBP}}
PBP_EPOSINPUT=${PBP_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PBP}}
PBP_DATAINPUT=${PBP_DATAINPUT:-${OFFICIAL_DATAINPUT_PBP}}

write_config() {
    local zpt_list="$1"
    local pt_list="$2"
    cat > config.sh <<EOF
ZPT_RANGES=(${zpt_list})
PT_RANGES=(${pt_list})
EOF
}

run_pp_chain() {
    local TAG=$1
    local USE_EVENT_WEIGHT=$2
    local USE_TRACK_WEIGHT=$3
    local VZ_FILE=$4

    ./system-analysis.sh "pythiaMC_nominal${TAG}"         --IsPP true --IsGenZ false --IsData false         --Input ${PP_MCRECOINPUT}         --MixFile ${PP_MCRECOINPUT}         --UseEventWeight ${USE_EVENT_WEIGHT} --UseZWeight false         --UseTrackWeight ${USE_TRACK_WEIGHT} --UseResidualWeight false         --yBoost 0 --nMix ${nMixPP}         ${VZ_FILE:+--UseVZWeight true --VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG}"         --IsPP true --IsGenZ true --IsData false         --Input ${PP_MCGENINPUT}         --MixFile ${PP_MCGENINPUT}         --UseEventWeight ${USE_EVENT_WEIGHT} --UseZWeight false         --UseTrackWeight ${USE_TRACK_WEIGHT} --UseResidualWeight false         --yBoost 0 --nMix ${nMixPP}         ${VZ_FILE:+--UseVZWeight true --VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "pp_nominal${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight false         --Input ${PP_DATAINPUT}         --MixFile ${PP_DATAINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMixPP}
}

run_pap_chain() {
    local MCPREFIX=$1
    local DATAPREFIX=$2
    local ISPPB=$3
    local MCRECOINPUT=$4
    local MCGENINPUT=$5
    local EPOSINPUT=$6
    local DATAINPUT=$7
    local TAG=$8
    local VZ_FILE=$9

    ./system-analysis.sh "${MCPREFIX}_nominal${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb ${ISPPB}         --Input ${MCRECOINPUT}         --MixFile ${MCRECOINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMixPA}         ${VZ_FILE:+--UseVZWeight true --VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "${MCPREFIX}_Gen_nominal${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb ${ISPPB}         --Input ${MCGENINPUT}         --MixFile ${MCGENINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile ${EPOSINPUT} --Fraction 1         --yBoost 0 --nMix ${nMixPA}         ${VZ_FILE:+--UseVZWeight true --VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "${DATAPREFIX}_nominal${TAG}"         --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB}         --Input ${DATAINPUT}         --MixFile ${DATAINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMixPA}
}

write_config '"0_500"' '"1_10"'

if [ "${DOPP}" == "1" ]; then
    run_pp_chain "_noVZWeight_nmix0" true true   ""
    run_pp_chain "_VZWeight_nmix0" true true     "${VZWeightFile_PP}"
fi

if [ "${DOPPB}" == "1" ]; then
    run_pap_chain "pPbMC" "pPb" true         "${PPB_MCRECOINPUT}"         "${PPB_MCGENINPUT}"         "${PPB_EPOSINPUT}"         "${PPB_DATAINPUT}"         "_noVZWeight_nmix0" ""
fi

if [ "${DOPPB}" == "2" ]; then
    run_pap_chain "pPbMC" "pPb" true         "${PPB_MCRECOINPUT}"         "${PPB_MCGENINPUT}"         "${PPB_EPOSINPUT}"         "${PPB_DATAINPUT}"         "_VZWeight_nmix0"         "${VZWeightFile_PPb}"
fi

if [ "${DOPBP}" == "1" ]; then
    run_pap_chain "PbPMC" "PbP" false         "${PBP_MCRECOINPUT}"         "${PBP_MCGENINPUT}"         "${PBP_EPOSINPUT}"         "${PBP_DATAINPUT}"         "_noVZWeight_nmix0" ""
fi

if [ "${DOPBP}" == "2" ]; then
    run_pap_chain "PbPMC" "PbP" false         "${PBP_MCRECOINPUT}"         "${PBP_MCGENINPUT}"         "${PBP_EPOSINPUT}"         "${PBP_DATAINPUT}"         "_VZWeight_nmix0"         "${VZWeightFile_PbP}"
fi
