#!/bin/bash
# Studies 17+18 cross-check: Option A (CM frame) vs Option B (pPb-oriented frame).
# Symmetrizes each option's combined result and compares them (should agree within stats).
# Also compares symmetrized combined HI vs pp for each option.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12FramesXCheck

OUTDIR="plots/12x12_frames_xcheck"
mkdir -p "$OUTDIR"

./ExecutePlot12x12FramesXCheck \
    --pPbFileA "${OFFICIAL_PPB_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFileA "${OFFICIAL_PBP_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --ppFileA  "${OFFICIAL_PP_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --pPbFileB "${OFFICIAL_PPB_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFileB "${OFFICIAL_PBP_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --ppFileB  "${OFFICIAL_PP_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --OutputDir "$OUTDIR" \
    --Tag "xcheck"

echo "=== Done: ${OUTDIR}/ ==="
