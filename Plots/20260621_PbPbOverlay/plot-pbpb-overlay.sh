#!/bin/bash
cd "$(dirname "$0")"

source ../../SetupAnalysis.sh
source ../../OfficialWeightDictionary.sh
source ../../OfficialProductDictionary.sh

make ExecutePbPbOverlay || exit 1

mkdir -p plots

COMMON_ARGS="
    --PPResultFile ${OFFICIAL_PP_RESULT_PREFIX_SCAN}_ZPT30_500-result.root
    --PPbNosubFile ${OFFICIAL_PPB_RESULT_PREFIX_SCAN}_ZPT30_500-nosub.root
    --PbPNosubFile ${OFFICIAL_PBP_RESULT_PREFIX_SCAN}_ZPT30_500-nosub.root
    --PPSystematicsFile ../../Systematics/20260329_pPbSystematics/output/pp_${OFFICIAL_TAG_PP}_scan_ZPT30_500_trkPT0.5_2-systematics.root
    --PPbSystematicsFile ../../Systematics/20260329_pPbSystematics/output/pPbPbp_${OFFICIAL_TAG_PPB}_scan_ZPT30_500_trkPT0.5_2-systematics.root
"

./ExecutePbPbOverlay $COMMON_ARGS \
    --Mode DeltaPhi \
    --HEPDataPPFile "data/pp_5TeV_dphi_1_2.csv" \
    --HEPDataPbPbFile "data/PbPb_0-30_dphi_1_2.csv" \
    --Output "plots/pbpb_pPb_overlay_DeltaPhi.pdf"

./ExecutePbPbOverlay $COMMON_ARGS \
    --Mode DeltaEta \
    --HEPDataPPFile "data/pp_5TeV_deta_1_2.csv" \
    --HEPDataPbPbFile "data/PbPb_0-30_deta_1_2.csv" \
    --Output "plots/pbpb_pPb_overlay_DeltaEta.pdf"

./ExecutePbPbOverlay $COMMON_ARGS \
    --Mode DeltaPhi --Difference true \
    --HEPDataPPFile "data/pp_5TeV_dphi_1_2.csv" \
    --HEPDataPbPbFile "data/PbPb_0-30_dphi_1_2.csv" \
    --Output "plots/pbpb_pPb_difference_DeltaPhi.pdf"

./ExecutePbPbOverlay $COMMON_ARGS \
    --Mode DeltaEta --Difference true \
    --HEPDataPPFile "data/pp_5TeV_deta_1_2.csv" \
    --HEPDataPbPbFile "data/PbPb_0-30_deta_1_2.csv" \
    --Output "plots/pbpb_pPb_difference_DeltaEta.pdf"
