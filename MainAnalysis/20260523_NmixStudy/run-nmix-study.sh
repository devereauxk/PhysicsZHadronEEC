#!/bin/bash
# Run corrected-data pp/pPb/PbP analysis and EPOS gen for nmix=1,5,15,20.
# nmix=10 is reused from the central analysis results already in plots/.

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
   source clean.sh
fi

export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-25}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}
export CONFIG_FILE="$THISDIR/config.sh"

for N in 1 5 15 20; do
   PP_NMIX_TAG="${OFFICIAL_TAG_PP/nmix10/nmix${N}}"
   PPB_NMIX_TAG="${OFFICIAL_TAG_PPB/nmix10/nmix${N}}"

   # pp corrected data
   ./system-analysis.sh "pp_trkResidual_${PP_NMIX_TAG}" \
      --IsPP true --IsGenZ false --IsData true --UseVZWeight true \
      --Input "${OFFICIAL_DATAINPUT_PP}" \
      --MixFile "${OFFICIAL_DATAINPUT_PP}" \
      --UseEventWeight false --UseZWeight true \
      --UseTrackWeight true --UseResidualWeight true \
      --yBoost 0 --nMix ${N} \
      --UseJackknife true \
      --ZWeightFile "${ZWeightFile_PP}" \
      --ResidualWeightFile "${RWeightFile_PP}" \
      --EnergyExtraFile "${EEWeightFile_PP}" \
      --VZWeightFile "${VZWeightFile_PP}"

   # pPb corrected data
   ./system-analysis.sh "pPb_trkResidual_${PPB_NMIX_TAG}" \
      --IsPP false --IsPPb true --IsGenZ false --IsData true --UseVZWeight true \
      --Input "${OFFICIAL_DATAINPUT_PPB}" \
      --MixFile "${OFFICIAL_DATAINPUT_PPB}" \
      --UseEventWeight true --UseZWeight true \
      --UseTrackWeight true --UseResidualWeight true \
      --yBoost 0 --nMix ${N} \
      --UseJackknife true \
      --ZWeightFile "${ZWeightFile_PPb}" \
      --ResidualWeightFile "${RWeightFile_PPb}" \
      --VZWeightFile "${VZWeightFile_PPb}"

   # PbP corrected data
   ./system-analysis.sh "PbP_trkResidual_${PPB_NMIX_TAG}" \
      --IsPP false --IsPPb false --IsGenZ false --IsData true --UseVZWeight true \
      --Input "${OFFICIAL_DATAINPUT_PBP}" \
      --MixFile "${OFFICIAL_DATAINPUT_PBP}" \
      --UseEventWeight true --UseZWeight true \
      --UseTrackWeight true --UseResidualWeight true \
      --yBoost 0 --nMix ${N} \
      --UseJackknife true \
      --ZWeightFile "${ZWeightFile_PbP}" \
      --ResidualWeightFile "${RWeightFile_PbP}" \
      --VZWeightFile "${VZWeightFile_PbP}"

   # pPbMC Gen (EPOS)
   ./system-analysis.sh "pPbMC_Gen_nominal_${PPB_NMIX_TAG}" \
      --IsPP false --IsPPb true --IsGenZ true --IsData false --UseVZWeight true \
      --Input "${OFFICIAL_MCGENINPUT_PPB}" \
      --MixFile "${OFFICIAL_MCGENINPUT_PPB}" \
      --UseEventWeight true --UseZWeight false \
      --UseTrackWeight true --UseResidualWeight false \
      --EPOSFile "${OFFICIAL_EPOSINPUT_PPB}" \
      --yBoost 0 --nMix ${N} \
      --VZWeightFile "${VZWeightFile_PPb}"

   # PbPMC Gen (EPOS)
   ./system-analysis.sh "PbPMC_Gen_nominal_${PPB_NMIX_TAG}" \
      --IsPP false --IsPPb false --IsGenZ true --IsData false --UseVZWeight true \
      --Input "${OFFICIAL_MCGENINPUT_PBP}" \
      --MixFile "${OFFICIAL_MCGENINPUT_PBP}" \
      --UseEventWeight true --UseZWeight false \
      --UseTrackWeight true --UseResidualWeight false \
      --EPOSFile "${OFFICIAL_EPOSINPUT_PBP}" \
      --yBoost 0 --nMix ${N} \
      --VZWeightFile "${VZWeightFile_PbP}"

done
