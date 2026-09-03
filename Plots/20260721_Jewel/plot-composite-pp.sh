#!/bin/bash
# Composite DeltaPhi: Pythia+MG vs JEWEL pp MOD (2x3 grid)
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

RESULT_DIR="$OFFICIAL_RESULT_DIR"
BASELINE="${OFFICIAL_PP_THEORY_PREFIX_SCAN}"
JEWEL_PP="${RESULT_DIR}/jewelPP8160signed_MOD_scan"
JEWEL_PP_ZRW="${RESULT_DIR}/jewelPP8160signed_MOD_zrw_scan"
JEWEL_PP_15="${RESULT_DIR}/jewelPP8160signed_MOD15_scan"

for zpt in 0_30 30_500; do
    for f in "${BASELINE}_ZPT${zpt}-result.root" "${JEWEL_PP}_ZPT${zpt}-result.root" "${JEWEL_PP_ZRW}_ZPT${zpt}-result.root" "${JEWEL_PP_15}_ZPT${zpt}-result.root"; do
        [ -f "$f" ] || { echo "ERROR: missing $f"; exit 1; }
    done
done
for f in "${BASELINE}_ZPT0_500-result.root" "${JEWEL_PP_ZRW}_ZPT0_500-result.root"; do
    [ -f "$f" ] || { echo "ERROR: missing $f"; exit 1; }
done

cd "$(dirname "$0")"
make ExecuteJewelComposite

mkdir -p plots

# Per-row: Z pT reweighted ptmin=0 (green) for ZPT<30, unweighted ptmin=15 (magenta) for ZPT>30
./ExecuteJewelComposite \
    --baselinePrefix "$BASELINE" \
    --baselineLabel "Pythia8+MG" \
    --jewelPrefixesLow "${JEWEL_PP_ZRW}" \
    --labelsLow "JEWEL pp (Z p_{T} rw)" \
    --jewelColorsLow "2" \
    --jewelPrefixesHigh "${JEWEL_PP_15}" \
    --labelsHigh "JEWEL pp (ptmin=15)" \
    --jewelColorsHigh "3" \
    --doEta false \
    --output plots/jewel_pp_composite_deltaphi.pdf

# Verification: all 4 curves on both rows (ptmin=15 vs ptmin=0 agreement at ZPT>30)
./ExecuteJewelComposite \
    --baselinePrefix "$BASELINE" \
    --jewelPrefixes "${JEWEL_PP},${JEWEL_PP_ZRW},${JEWEL_PP_15}" \
    --labels "Pythia8+MG,JEWEL pp (ptmin=0),JEWEL pp (Z p_{T} rw),JEWEL pp (ptmin=15)" \
    --legendScale 0.85 \
    --doEta false \
    --output plots/jewel_pp_composite_deltaphi_verification.pdf

# 3-row money layout: single sample/method (Z pT reweighted ptmin=0) in all rows
./ExecuteJewelComposite \
    --baselinePrefix "$BASELINE" \
    --jewelPrefixes "${JEWEL_PP_ZRW}" \
    --labels "Pythia8+MG,JEWEL pp (Z p_{T} rw)" \
    --jewelColors "2" \
    --zPtBins "0_500,0_30,30_500" \
    --legendY 0.52 \
    --doEta false \
    --output plots/jewel_pp_composite_deltaphi_3row.pdf

echo "Done: plots/jewel_pp_composite_deltaphi{,_verification,_3row}.pdf"
