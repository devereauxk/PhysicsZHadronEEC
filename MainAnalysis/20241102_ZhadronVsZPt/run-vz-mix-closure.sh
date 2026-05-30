#!/bin/bash

# Closure test for MaxMixDeltaVZ=1 requirement.
# Runs Gen MC and fully corrected reco MC (no background subtraction).
# ZPT 0-10, trkPT 0.5-15 for pp, pPb, PbP.

DOPP=${1:-1}
DOPPB=${2:-1}
DOPBP=${3:-1}

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cat > config.sh <<CFG
ZPT_RANGES=("0_30" "30_500")
PT_RANGES=("0.5_15")
CFG

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

MAX_MIX_DVZ=0.5

if [ "$DOPP" == "1" ]; then
    nMix=10
    TAG="_${OFFICIAL_TAG_PP}_vzmatch${MAX_MIX_DVZ}cm"

    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input ${OFFICIAL_MCGENINPUT_PP} \
        --MixFile ${OFFICIAL_MCGENINPUT_PP} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PP} \
        --MaxMixDeltaVZ $MAX_MIX_DVZ

    ./system-analysis.sh "pythiaMC_trkResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input ${OFFICIAL_MCRECOINPUT_PP} \
        --MixFile ${OFFICIAL_MCRECOINPUT_PP} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PP} \
        --ZWeightFile ${ZWeightFile_PP} \
        --ResidualWeightFile ${RWeightFile_PP} \
        --MaxMixDeltaVZ $MAX_MIX_DVZ
fi

if [ "$DOPPB" == "1" ]; then
    nMix=10
    TAG="_${OFFICIAL_TAG_PPB}_vzmatch${MAX_MIX_DVZ}cm"

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input ${OFFICIAL_MCGENINPUT_PPB} \
        --MixFile ${OFFICIAL_MCGENINPUT_PPB} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile ${OFFICIAL_EPOSINPUT_PPB} --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb} \
        --MaxMixDeltaVZ $MAX_MIX_DVZ

    ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input ${OFFICIAL_MCRECOINPUT_PPB} \
        --MixFile ${OFFICIAL_MCRECOINPUT_PPB} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb} \
        --ZWeightFile ${ZWeightFile_PPb} \
        --ResidualWeightFile ${RWeightFile_PPb} \
        --MaxMixDeltaVZ $MAX_MIX_DVZ
fi

if [ "$DOPBP" == "1" ]; then
    nMix=10
    TAG="_${OFFICIAL_TAG_PPB}_vzmatch${MAX_MIX_DVZ}cm"

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input ${OFFICIAL_MCGENINPUT_PBP} \
        --MixFile ${OFFICIAL_MCGENINPUT_PBP} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile ${OFFICIAL_EPOSINPUT_PBP} --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP} \
        --MaxMixDeltaVZ $MAX_MIX_DVZ

    ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input ${OFFICIAL_MCRECOINPUT_PBP} \
        --MixFile ${OFFICIAL_MCRECOINPUT_PBP} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP} \
        --ZWeightFile ${ZWeightFile_PbP} \
        --ResidualWeightFile ${RWeightFile_PbP} \
        --MaxMixDeltaVZ $MAX_MIX_DVZ
fi
