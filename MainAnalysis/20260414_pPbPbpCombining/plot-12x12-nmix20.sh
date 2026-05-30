#!/bin/bash
# Plot pPb vs Pbp: Nmix=20 (solid) vs Nmix=10 nominal (dashed) overlay (Study 8).
# Pair 1 (solid):  Nmix=20
# Pair 2 (dashed): Nmix=10 (reuses _12x12_full outputs)
# Outputs to plots/12x12_nmix20/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_nmix20"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_nmix20_ZPT0_500-result.root" \
    --PbPFile  "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_nmix20_ZPT0_500-result.root" \
    --pPbLabel "pPb Nmix=20" \
    --PbPLabel "Pbp Nmix=20" \
    --pPbFile2 "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --PbPFile2 "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --pPbLabel2 "pPb Nmix=10" \
    --PbPLabel2 "Pbp Nmix=10" \
    --OutputDir "$OUTDIR" \
    --Tag "nmix20"

echo "=== Done: ${OUTDIR}/ ==="
