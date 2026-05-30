#!/bin/bash
# Study 0: nominal pPb vs Pbp comparison, fully corrected, no special cuts.
# Reuses existing _12x12_full result files from runner-12x12-halfsplit.sh.
# Outputs to plots/12x12_nominal/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_nominal"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --PbPFile "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "nominal" \
    --pPbLabel "pPb data" \
    --PbPLabel "Pbp data"

echo "=== Done: ${OUTDIR}/ ==="
