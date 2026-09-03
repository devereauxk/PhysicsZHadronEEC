#!/bin/bash
# Inclusive-row DeltaPhi: Pythia8+MG vs JEWEL pp (Z pT reweighted ptmin=0)
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

RESULT_DIR="$OFFICIAL_RESULT_DIR"
BASELINE="${OFFICIAL_PP_THEORY_PREFIX_SCAN}"
JEWEL_PP_ZRW="${RESULT_DIR}/jewelPP8160signed_MOD_zrw_scan"

for f in "${BASELINE}_ZPT0_500-result.root" "${JEWEL_PP_ZRW}_ZPT0_500-result.root"; do
    [ -f "$f" ] || { echo "ERROR: missing $f"; exit 1; }
done

cd "$(dirname "$0")"
make ExecuteJewelComposite

mkdir -p plots

./ExecuteJewelComposite \
    --baselinePrefix "$BASELINE" \
    --jewelPrefixes "${JEWEL_PP_ZRW}" \
    --labels "Pythia8+MG,JEWEL pp (Z p_{T} rw)" \
    --jewelColors "2" \
    --zPtBins "0_500" \
    --doEta false \
    --output plots/jewel_pp_deltaphi.pdf

echo "Done: plots/jewel_pp_deltaphi.pdf"
