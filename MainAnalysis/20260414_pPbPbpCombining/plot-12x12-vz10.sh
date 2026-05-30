#!/bin/bash
# Plot pPb vs Pbp: |vz|<10 (solid) vs |vz|<15 (dashed) overlay (Study 2).
# Pair 1 (solid): |vz|<10
# Pair 2 (dashed): |vz|<15 (reuses _12x12_full outputs)
# Outputs to plots/12x12_vz10/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_vz10"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_vz10_ZPT0_500-result.root" \
    --PbPFile  "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_vz10_ZPT0_500-result.root" \
    --pPbLabel2 "pPb |vz|<15 cm" \
    --PbPLabel2 "Pbp |vz|<15 cm" \
    --pPbFile2  "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --PbPFile2  "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "vz10" \
    --pPbLabel "pPb |vz|<10 cm" \
    --PbPLabel "Pbp |vz|<10 cm"

echo "=== Done: ${OUTDIR}/ ==="
