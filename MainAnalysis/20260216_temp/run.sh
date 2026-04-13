#!/bin/bash

set -e

nMix=1
ISMUTAGGED=${ISMUTAGGED:-false}
TRACKMUDR=${TRACKMUDR:--1}
TRACKMUCLOSEST=${TRACKMUCLOSEST:-false}
OUTPUT_SUBDIR=${OUTPUT_SUBDIR:-}
CONFIG_OVERRIDE=${CONFIG_FILE:-}

DOPP=${1:-1}
DOPPB=${2:-1}
DOPBP=${3:-1}
MODE=${4:-All}
TAG=${5:-MCStudies_V01Remake}

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

OUTPUT_SUBDIR="${OUTPUT_SUBDIR#/}"
OUTPUT_SUBDIR="${OUTPUT_SUBDIR%/}"

PP_DATA_INPUT="${PP_DATA_INPUT_OVERRIDE:-${OFFICIAL_DATAINPUT_PP}}"
PP_MCRECO_INPUT="${PP_MCRECO_INPUT_OVERRIDE:-${OFFICIAL_MCRECOINPUT_PP}}"
PP_MCGEN_INPUT="${PP_MCGEN_INPUT_OVERRIDE:-${OFFICIAL_MCGENINPUT_PP}}"

PPB_DATA_INPUT="${PPB_DATA_INPUT_OVERRIDE:-${OFFICIAL_DATAINPUT_PPB}}"
PPB_MCRECO_INPUT="${PPB_MCRECO_INPUT_OVERRIDE:-${OFFICIAL_MCRECOINPUT_PPB}}"
PPB_MCGEN_INPUT="${PPB_MCGEN_INPUT_OVERRIDE:-${OFFICIAL_MCGENINPUT_PPB}}"
PPB_EPOS_INPUT="${PPB_EPOS_INPUT_OVERRIDE:-${OFFICIAL_EPOSINPUT_PPB}}"

PBP_DATA_INPUT="${PBP_DATA_INPUT_OVERRIDE:-${OFFICIAL_DATAINPUT_PBP}}"
PBP_MCRECO_INPUT="${PBP_MCRECO_INPUT_OVERRIDE:-${OFFICIAL_MCRECOINPUT_PBP}}"
PBP_MCGEN_INPUT="${PBP_MCGEN_INPUT_OVERRIDE:-${OFFICIAL_MCGENINPUT_PBP}}"
PBP_EPOS_INPUT="${PBP_EPOS_INPUT_OVERRIDE:-${OFFICIAL_EPOSINPUT_PBP}}"

if [ -n "${OUTPUT_SUBDIR}" ]; then
    mkdir -p "output/${OUTPUT_SUBDIR}"
fi

if [ -z "${CONFIG_OVERRIDE}" ]; then
cat > config.sh <<EOF
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500")
EOF
else
    export CONFIG_FILE="${CONFIG_OVERRIDE}"
fi

run_case() {
    local prefix=$1
    local isPP=$2
    local isData=$3
    local isGenZ=$4
    local isPPb=$5
    local inputFile=$6
    local mixFile=$7
    local eposFile=$8
    local useEventWeight=$9
    local outputPrefix="${prefix}_${TAG}"

    if [ -n "${OUTPUT_SUBDIR}" ]; then
        outputPrefix="${OUTPUT_SUBDIR}/${outputPrefix}"
    fi

    local args=(
        ./system-analysis.sh "${outputPrefix}"
        --IsPP "${isPP}" --IsGenZ "${isGenZ}" --IsData "${isData}"
        --Input "${inputFile}" --MixFile "${mixFile}"
        --UseEventWeight "${useEventWeight}" --UseZWeight false
        --UseTrackWeight true --UseResidualWeight false
        --yBoost 0 --nMix "${nMix}" --IsMuTagged "${ISMUTAGGED}"
        --TrackMuDR "${TRACKMUDR}" --TrackMuClosest "${TRACKMUCLOSEST}"
    )

    if [ "${isPP}" = "false" ]; then
        args+=(--IsPPb "${isPPb}")
    fi
    if [ -n "${eposFile}" ]; then
        args+=(--EPOSFile "${eposFile}" --Fraction 1)
    fi

    "${args[@]}"
}

run_pp() {
    case "${MODE}" in
        Data)
            run_case "ppData" true true false false \
                "${PP_DATA_INPUT}" "${PP_DATA_INPUT}" "" false
            ;;
        Reco)
            run_case "ppMC_Reco" true false false false \
                "${PP_MCRECO_INPUT}" "${PP_MCRECO_INPUT}" "" true
            ;;
        Gen)
            run_case "ppMC_Gen" true false true false \
                "${PP_MCGEN_INPUT}" "${PP_MCGEN_INPUT}" "" true
            ;;
        All)
            run_case "ppData" true true false false \
                "${PP_DATA_INPUT}" "${PP_DATA_INPUT}" "" false
            run_case "ppMC_Reco" true false false false \
                "${PP_MCRECO_INPUT}" "${PP_MCRECO_INPUT}" "" true
            run_case "ppMC_Gen" true false true false \
                "${PP_MCGEN_INPUT}" "${PP_MCGEN_INPUT}" "" true
            ;;
        *)
            echo "Unsupported mode '${MODE}'. Use Data, Reco, Gen, or All." >&2
            exit 1
            ;;
    esac
}

run_pa() {
    local dataPrefix=$1
    local recoPrefix=$2
    local genPrefix=$3
    local isPPb=$4
    local dataInput=$5
    local recoInput=$6
    local genInput=$7
    local eposInput=$8

    case "${MODE}" in
        Data)
            run_case "${dataPrefix}" false true false "${isPPb}" \
                "${dataInput}" "${dataInput}" "" false
            ;;
        Reco)
            run_case "${recoPrefix}" false false false "${isPPb}" \
                "${recoInput}" "${recoInput}" "" true
            ;;
        Gen)
            run_case "${genPrefix}" false false true "${isPPb}" \
                "${genInput}" "${genInput}" "${eposInput}" true
            ;;
        All)
            run_case "${dataPrefix}" false true false "${isPPb}" \
                "${dataInput}" "${dataInput}" "" false
            run_case "${recoPrefix}" false false false "${isPPb}" \
                "${recoInput}" "${recoInput}" "" true
            run_case "${genPrefix}" false false true "${isPPb}" \
                "${genInput}" "${genInput}" "${eposInput}" true
            ;;
        *)
            echo "Unsupported mode '${MODE}'. Use Data, Reco, Gen, or All." >&2
            exit 1
            ;;
    esac
}

if [ "${DOPP}" = "1" ]; then
    run_pp
fi

if [ "${DOPPB}" = "1" ]; then
    run_pa "pPbData" "pPbMC_Reco" "pPbMC_Gen" true \
        "${PPB_DATA_INPUT}" \
        "${PPB_MCRECO_INPUT}" \
        "${PPB_MCGEN_INPUT}" \
        "${PPB_EPOS_INPUT}"
fi

if [ "${DOPBP}" = "1" ]; then
    run_pa "PbPData" "PbPMC_Reco" "PbPMC_Gen" false \
        "${PBP_DATA_INPUT}" \
        "${PBP_MCRECO_INPUT}" \
        "${PBP_MCGEN_INPUT}" \
        "${PBP_EPOS_INPUT}"
fi
