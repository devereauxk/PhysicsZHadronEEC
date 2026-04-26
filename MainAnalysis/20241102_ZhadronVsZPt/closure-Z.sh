#!/bin/bash

DOPP=$1
DOPPB=$2
DOPBP=$3

shift 3
COMMON_EXTRA_ARGS=("$@")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cat > config.sh <<CFG
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_500")
CFG

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

nMix=0
PP_TAG_SUFFIX="${TAG_SUFFIX:-_${OFFICIAL_TAG_PP/nmix10/nmix0}}"
PA_TAG_SUFFIX="${TAG_SUFFIX:-_${OFFICIAL_TAG_PPB/nmix10/nmix0}}"
PP_MCGENINPUT=${PP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}
PP_MCRECOINPUT=${PP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PP}}
PPB_MCGENINPUT=${PPB_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PPB}}
PPB_MCRECOINPUT=${PPB_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PPB}}
PPB_EPOSINPUT=${PPB_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PPB}}
PBP_MCGENINPUT=${PBP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PBP}}
PBP_MCRECOINPUT=${PBP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PBP}}
PBP_EPOSINPUT=${PBP_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PBP}}

if [ "$DOPP" == "1" ]; then
    ./system-analysis.sh "pythiaMC_Gen_nominal${PP_TAG_SUFFIX}"         --IsPP true --IsGenZ true --IsData false         --Input ${PP_MCGENINPUT}          --MixFile ${PP_MCGENINPUT}          --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PP}         "${COMMON_EXTRA_ARGS[@]}"
    ./system-analysis.sh "pythiaMC_nominal${PP_TAG_SUFFIX}"         --IsPP true --IsGenZ false --IsData false         --Input ${PP_MCRECOINPUT}         --MixFile ${PP_MCRECOINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PP}         "${COMMON_EXTRA_ARGS[@]}"
    ./system-analysis.sh "pythiaMC_ZResidual${PP_TAG_SUFFIX}"         --IsPP true --IsGenZ false --IsData false         --Input ${PP_MCRECOINPUT}         --MixFile ${PP_MCRECOINPUT}         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PP}         --UseVZWeight true --VZWeightFile ${VZWeightFile_PP}         "${COMMON_EXTRA_ARGS[@]}"
fi

if [ "$DOPPB" == "1" ]; then
    ./system-analysis.sh "pPbMC_Gen_nominal${PA_TAG_SUFFIX}"         --IsPP false --IsGenZ true --IsData false --IsPPb true         --Input ${PPB_MCGENINPUT}         --MixFile ${PPB_MCGENINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile ${PPB_EPOSINPUT} --Fraction 1         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb}         "${COMMON_EXTRA_ARGS[@]}"
    ./system-analysis.sh "pPbMC_nominal${PA_TAG_SUFFIX}"         --IsPP false --IsGenZ false --IsData false --IsPPb true         --Input ${PPB_MCRECOINPUT}         --MixFile ${PPB_MCRECOINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb}         "${COMMON_EXTRA_ARGS[@]}"
    ./system-analysis.sh "pPbMC_ZResidual${PA_TAG_SUFFIX}"         --IsPP false --IsGenZ false --IsData false --IsPPb true         --Input ${PPB_MCRECOINPUT}         --MixFile ${PPB_MCRECOINPUT}         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PPb}         --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb}         "${COMMON_EXTRA_ARGS[@]}"
fi

if [ "$DOPBP" == "1" ]; then
    ./system-analysis.sh "PbPMC_Gen_nominal${PA_TAG_SUFFIX}"         --IsPP false --IsGenZ true --IsData false --IsPPb false         --Input ${PBP_MCGENINPUT}         --MixFile ${PBP_MCGENINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile ${PBP_EPOSINPUT} --Fraction 1         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP}         "${COMMON_EXTRA_ARGS[@]}"
    ./system-analysis.sh "PbPMC_nominal${PA_TAG_SUFFIX}"         --IsPP false --IsGenZ false --IsData false --IsPPb false         --Input ${PBP_MCRECOINPUT}         --MixFile ${PBP_MCRECOINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP}         "${COMMON_EXTRA_ARGS[@]}"
    ./system-analysis.sh "PbPMC_ZResidual${PA_TAG_SUFFIX}"         --IsPP false --IsGenZ false --IsData false --IsPPb false         --Input ${PBP_MCRECOINPUT}         --MixFile ${PBP_MCRECOINPUT}         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PbP}         --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP}         "${COMMON_EXTRA_ARGS[@]}"
fi
