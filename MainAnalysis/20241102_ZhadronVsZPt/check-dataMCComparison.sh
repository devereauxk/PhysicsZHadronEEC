#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

DOPP=$1
DOPPB=$2
DOPBP=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

PP_MCGENINPUT=${PP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}
PPB_MCGENINPUT=${PPB_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PPB}}
PPB_MCRECOINPUT=${PPB_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PPB}}
PPB_DATAINPUT=${PPB_DATAINPUT:-${OFFICIAL_DATAINPUT_PPB}}
PPB_EPOSINPUT=${PPB_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PPB}}
PBP_MCGENINPUT=${PBP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PBP}}
PBP_MCRECOINPUT=${PBP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PBP}}
PBP_DATAINPUT=${PBP_DATAINPUT:-${OFFICIAL_DATAINPUT_PBP}}
PBP_EPOSINPUT=${PBP_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PBP}}

CONFIG_TARGET=${CONFIG_TARGET:-/tmp/kdeverea/check-dataMCComparison-config.sh}
mkdir -p /tmp/kdeverea
cat > "${CONFIG_TARGET}" <<EOF
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
EOF
export CONFIG_FILE="${CONFIG_TARGET}"

PP_TAG_BASE=${PP_TAG_BASE:-${OFFICIAL_TAG_PP/%_nmix10/_nmix0}}
PA_TAG_BASE=${PA_TAG_BASE:-${OFFICIAL_TAG_PPB/%_nmix10/_nmix0}}
PP_TAG=${PP_TAG:-_dataMCComparison_${PP_TAG_BASE/_nmix0/_noEE_nmix0}}
PA_TAG=${PA_TAG:-_dataMCComparison_${PA_TAG_BASE}}

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-25}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

# Use the official dictionary-backed inputs so the direct-label V0.1 PA convention stays consistent across comparison reruns.

# pp
if [ "$DOPP" == "1" ]; then

    nMix=0
    PP_MCGENINPUT=${PP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}
    PP_MCRECOINPUT=${PP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PP}}
    PP_DATAINPUT=${PP_DATAINPUT:-${OFFICIAL_DATAINPUT_PP}}

    ./system-analysis.sh "pythiaMC_Gen_nominal${PP_TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input ${PP_MCGENINPUT} \
        --MixFile ${PP_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "pythiaMC_nominal${PP_TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input ${PP_MCRECOINPUT} \
        --MixFile ${PP_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "pythiaMC_trkResidual${PP_TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input ${PP_MCRECOINPUT} \
        --MixFile ${PP_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP \
        --ZWeightFile $ZWeightFile_PP \
        --ResidualWeightFile $RWeightFile_PP

    ./system-analysis.sh "pp_nominal${PP_TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input ${PP_DATAINPUT} \
        --MixFile ${PP_DATAINPUT} \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "pp_EEtrkResidual${PP_TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input ${PP_DATAINPUT} \
        --MixFile ${PP_DATAINPUT} \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP \
        --ZWeightFile $ZWeightFile_PP \
        --ResidualWeightFile $RWeightFile_PP

fi

# pPb
if [ "$DOPPB" == "1" ]; then
    nMix=0

    ./system-analysis.sh "pythiapPbEPOSMC_Gen_nominal${PA_TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input ${PP_MCGENINPUT}  \
        --MixFile ${PP_MCGENINPUT}  \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile ${PPB_EPOSINPUT} --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "pPbNoEPOSMC_Gen_nominal${PA_TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input ${PPB_MCGENINPUT} \
        --MixFile ${PPB_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_Gen_nominal${PA_TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input ${PPB_MCGENINPUT} \
        --MixFile ${PPB_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile ${PPB_EPOSINPUT} --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_nominal${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input ${PPB_MCRECOINPUT} \
        --MixFile ${PPB_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_trkResidual${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input ${PPB_MCRECOINPUT} \
        --MixFile ${PPB_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb \
        --ZWeightFile $ZWeightFile_PPb \
        --ResidualWeightFile $RWeightFile_PPb

    ./system-analysis.sh "pPb_nominal${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb true \
        --Input ${PPB_DATAINPUT} \
        --MixFile ${PPB_DATAINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPb_trkResidual${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb true \
        --Input ${PPB_DATAINPUT} \
        --MixFile ${PPB_DATAINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile_PPb \
        --ZWeightFile $ZWeightFile_PPb \
        --ResidualWeightFile $RWeightFile_PPb

    
fi

# PbP
if [ "$DOPBP" == "1" ]; then
    nMix=0

    ./system-analysis.sh "pythiaPbPEPOSMC_Gen_nominal${PA_TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input ${PP_MCGENINPUT}  \
        --MixFile ${PP_MCGENINPUT}  \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile ${PBP_EPOSINPUT} --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "PbPNoEPOSMC_Gen_nominal${PA_TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input ${PBP_MCGENINPUT} \
        --MixFile ${PBP_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_Gen_nominal${PA_TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input ${PBP_MCGENINPUT} \
        --MixFile ${PBP_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile ${PBP_EPOSINPUT} --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_nominal${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input ${PBP_MCRECOINPUT} \
        --MixFile ${PBP_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_trkResidual${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input ${PBP_MCRECOINPUT} \
        --MixFile ${PBP_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $RWeightFile_PbP

    ./system-analysis.sh "PbP_nominal${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb false \
        --Input ${PBP_DATAINPUT} \
        --MixFile ${PBP_DATAINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbP_trkResidual${PA_TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb false \
        --Input ${PBP_DATAINPUT} \
        --MixFile ${PBP_DATAINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile_PbP \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $RWeightFile_PbP

fi
