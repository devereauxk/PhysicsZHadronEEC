#!/bin/bash
# Plot pPb vs Pbp MC reco, Z-count-matched to data (Study 4).
# Outputs to plots/12x12_mc/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_mc"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${MAIN}/plots/pPbMC_trkResidual_${TAG}_12x12_matched_ZPT0_500-result.root" \
    --PbPFile "${MAIN}/plots/PbPMC_trkResidual_${TAG}_12x12_matched_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "mc_matched" \
    --pPbLabel "pPb MC reco" \
    --PbPLabel "Pbp MC reco"

echo "=== Done: ${OUTDIR}/ ==="
