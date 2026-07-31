#!/bin/bash
# 3-curve comparison: official Pythia+MG MC vs JEWEL pp MOD vs JEWEL pPb MOD
# Inclusive selection: ZPT 0_500, trkPT 0.5_15
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

RESULT_DIR="$OFFICIAL_RESULT_DIR"
PYTHIA_MG="${OFFICIAL_PP_THEORY_PREFIX}_ZPT0_500"
JEWEL_PP="${RESULT_DIR}/jewelPP8160signed_MOD_ZPT0_500"
JEWEL_PPB="${RESULT_DIR}/jewelPPb8160signed_MOD_ZPT0_500"

for f in "${PYTHIA_MG}-result.root" "${JEWEL_PP}-result.root" "${JEWEL_PPB}-result.root"; do
    if [ ! -f "$f" ]; then
        echo "ERROR: missing $f"
        exit 1
    fi
done

cd "$(dirname "$0")"
make ExecuteJewelComparison

mkdir -p plots

FILES="${PYTHIA_MG},${JEWEL_PP},${JEWEL_PPB}"
LABELS="Pythia8+MG (pp 8.16 TeV),JEWEL pp MOD,JEWEL pPb MOD"

./ExecuteJewelComparison \
    --files "$FILES" --labels "$LABELS" \
    --refLabel "Pythia8+MG" \
    --doEta true \
    --output plots/jewel_MOD_comparison_deltaeta.pdf

./ExecuteJewelComparison \
    --files "$FILES" --labels "$LABELS" \
    --refLabel "Pythia8+MG" \
    --doEta false \
    --output plots/jewel_MOD_comparison_deltaphi.pdf

echo "Done: plots/jewel_MOD_comparison_delta{eta,phi}.pdf"
