#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

DOPP=${1:-1}
DOPPB=${2:-1}
DOPBP=${3:-1}

PP_TAG=${PP_TAG_OVERRIDE:-${OFFICIAL_TAG_PP}_2DResidual_20260518}
PPB_TAG=${PPB_TAG_OVERRIDE:-${OFFICIAL_TAG_PPB}_2DResidual_20260518}
nMix=10

RESIDUAL_DIR="${THISDIR}/workflow_2D/output"
RWeightFile_PP_2D="${RESIDUAL_DIR}/20260518_2DResidual_TrackResidualCorrection_pp_zPt"
RWeightFile_PPb_2D="${RESIDUAL_DIR}/20260518_2DResidual_TrackResidualCorrection_PPb_zPt"
RWeightFile_PbP_2D="${RESIDUAL_DIR}/20260518_2DResidual_TrackResidualCorrection_PbP_zPt"

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
   source clean.sh
fi

export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}
export CONFIG_FILE=${CONFIG_FILE:-"$THISDIR/config.sh"}

run_ppb_chain() {
   local PREFIX=$1
   local ISPPB=$2
   local DATAINPUT=$3
   local ZWEIGHT=$4
   local RWEIGHT=$5
   local VZWEIGHT=$6
   local TAG=$7

   ./system-analysis.sh "${PREFIX}_trkResidual_${TAG}" \
      --IsPP false --IsGenZ false --IsData true --UseVZWeight true --IsPPb "${ISPPB}" \
      --Input "${DATAINPUT}" \
      --MixFile "${DATAINPUT}" \
      --UseEventWeight true --UseZWeight true \
      --UseTrackWeight true --UseResidualWeight true \
      --yBoost 0 --nMix ${nMix} \
      --UseJackknife true \
      --ZWeightFile "${ZWEIGHT}" \
      --ResidualWeightFile "${RWEIGHT}" \
      --VZWeightFile "${VZWEIGHT}"
}

if [ "$DOPP" = "1" ]; then
   ./system-analysis.sh "pp_trkResidual_${PP_TAG}" \
      --IsPP true --IsGenZ false --IsData true --UseVZWeight true \
      --Input "${OFFICIAL_DATAINPUT_PP}" \
      --MixFile "${OFFICIAL_DATAINPUT_PP}" \
      --UseEventWeight false --UseZWeight true \
      --UseTrackWeight true --UseResidualWeight true \
      --yBoost 0 --nMix ${nMix} \
      --UseJackknife true \
      --ZWeightFile "${ZWeightFile_PP}" \
      --ResidualWeightFile "${RWeightFile_PP_2D}" \
      --EnergyExtraFile "${EEWeightFile_PP}" \
      --VZWeightFile "${VZWeightFile_PP}"
fi

if [ "$DOPPB" = "1" ]; then
   run_ppb_chain "pPb" true "${OFFICIAL_DATAINPUT_PPB}" "${ZWeightFile_PPb}" "${RWeightFile_PPb_2D}" "${VZWeightFile_PPb}" "${PPB_TAG}"
fi

if [ "$DOPBP" = "1" ]; then
   run_ppb_chain "PbP" false "${OFFICIAL_DATAINPUT_PBP}" "${ZWeightFile_PbP}" "${RWeightFile_PbP_2D}" "${VZWeightFile_PbP}" "${PPB_TAG}"
fi
