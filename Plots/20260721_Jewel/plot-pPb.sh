#!/bin/bash
# Inclusive-row DeltaPhi: Pythia8+MG vs JEWEL pp and JEWEL pPb v4 hydro
# (both Z pT reweighted; pPb with hole x0.67, charged-only)
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

RESULT_DIR="$OFFICIAL_RESULT_DIR"
BASELINE="${OFFICIAL_PP_THEORY_PREFIX_SCAN}"
JEWEL_PP_ZRW="${RESULT_DIR}/jewelPP8160signed_MOD_zrw_scan"
JEWEL_PPB_ZRW="${RESULT_DIR}/jewelPPb8160signed_MODv4_zrw_scan"

for f in "${BASELINE}_ZPT0_500-result.root" "${JEWEL_PP_ZRW}_ZPT0_500-result.root" "${JEWEL_PPB_ZRW}_ZPT0_500-result.root"; do
    [ -f "$f" ] || { echo "ERROR: missing $f"; exit 1; }
done

cd "$(dirname "$0")"
make ExecuteJewelComposite

mkdir -p plots

./ExecuteJewelComposite \
    --baselinePrefix "$BASELINE" \
    --jewelPrefixes "${JEWEL_PP_ZRW},${JEWEL_PPB_ZRW}" \
    --labels "Pythia8+MG,JEWEL pp (Z p_{T} rw),JEWEL pPb hydro (Z p_{T} rw)" \
    --jewelColors "2,1" \
    --zPtBins "0_500" \
    --doEta false \
    --output plots/jewel_pPb_deltaphi.pdf

echo "Done: plots/jewel_pPb_deltaphi.pdf"
