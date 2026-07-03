#!/bin/bash
cd "$(dirname "$0")"

source ../../SetupAnalysis.sh
source ../../OfficialWeightDictionary.sh
source ../../OfficialProductDictionary.sh

make ExecutePPEEMC || exit 1

mkdir -p plots/pp-ee-mc

./ExecutePPEEMC \
    --EEWeightFile "$EEWeightFile_PP_PRIVATE" \
    --PPNoEEFile "${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_noEE_ZPT0_500-nosub.root" \
    --PPEEFile "${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_EEPrivate_ZPT0_500-nosub.root" \
    --OutputDir "plots/pp-ee-mc"
