#!/bin/bash
# Plot pPb vs Pbp with signed (unsymmetrized) DeltaEta (Study 3).
# Outputs to plots/12x12_signed/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_signed"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_signed_ZPT0_500-result.root" \
    --PbPFile "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_signed_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "signed" \
    --pPbLabel "pPb DATA" \
    --PbPLabel "Pbp DATA" \
    --ShowSignedEta true \
    --ShowFullPhi true

echo "=== Done: ${OUTDIR}/ ==="
