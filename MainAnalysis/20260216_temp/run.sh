#!/bin/bash

set -e

nMix=1
ISMUTAGGED=${ISMUTAGGED:-false}

DOPP=${1:-1}
DOPPB=${2:-1}
DOPBP=${3:-1}
MODE=${4:-All}
TAG=${5:-MCStudies_V01Remake}

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

PPB_MCRECO_INPUT="${PPB_MCRECO_INPUT_OVERRIDE:-${OFFICIAL_MCRECOINPUT_PPB}}"
PPB_MCGEN_INPUT="${PPB_MCGEN_INPUT_OVERRIDE:-${OFFICIAL_MCGENINPUT_PPB}}"

cat > config.sh <<EOF
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500")
EOF

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

    local args=(
        ./system-analysis.sh "${prefix}_${TAG}"
        --IsPP "${isPP}" --IsGenZ "${isGenZ}" --IsData "${isData}"
        --Input "${inputFile}" --MixFile "${mixFile}"
        --UseEventWeight "${useEventWeight}" --UseZWeight false
        --UseTrackWeight true --UseResidualWeight false
        --yBoost 0 --nMix "${nMix}" --IsMuTagged "${ISMUTAGGED}"
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
                "${OFFICIAL_DATAINPUT_PP}" "${OFFICIAL_DATAINPUT_PP}" "" false
            ;;
        Reco)
            run_case "ppMC_Reco" true false false false \
                "${OFFICIAL_MCRECOINPUT_PP}" "${OFFICIAL_MCRECOINPUT_PP}" "" true
            ;;
        Gen)
            run_case "ppMC_Gen" true false true false \
                "${OFFICIAL_MCGENINPUT_PP}" "${OFFICIAL_MCGENINPUT_PP}" "" true
            ;;
        All)
            run_case "ppData" true true false false \
                "${OFFICIAL_DATAINPUT_PP}" "${OFFICIAL_DATAINPUT_PP}" "" false
            run_case "ppMC_Reco" true false false false \
                "${OFFICIAL_MCRECOINPUT_PP}" "${OFFICIAL_MCRECOINPUT_PP}" "" true
            run_case "ppMC_Gen" true false true false \
                "${OFFICIAL_MCGENINPUT_PP}" "${OFFICIAL_MCGENINPUT_PP}" "" true
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
        "${OFFICIAL_DATAINPUT_PPB}" \
        "${PPB_MCRECO_INPUT}" \
        "${PPB_MCGEN_INPUT}" \
        "${OFFICIAL_EPOSINPUT_PPB}"
fi

if [ "${DOPBP}" = "1" ]; then
    run_pa "PbPData" "PbPMC_Reco" "PbPMC_Gen" false \
        "${OFFICIAL_DATAINPUT_PBP}" \
        "${OFFICIAL_MCRECOINPUT_PBP}" \
        "${OFFICIAL_MCGENINPUT_PBP}" \
        "${OFFICIAL_EPOSINPUT_PBP}"
fi
