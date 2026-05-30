#!/bin/bash
# Plot pPb vs Pbp MC reco at full statistics (Study 6).
# Outputs to plots/12x12_mc_fullstats/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_mc_fullstats"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${MAIN}/plots/pPbMC_trkResidual_${TAG}_12x12_fullstats_ZPT0_500-result.root" \
    --PbPFile "${MAIN}/plots/PbPMC_trkResidual_${TAG}_12x12_fullstats_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "mc_fullstats" \
    --pPbLabel "pPb MC reco" \
    --PbPLabel "Pbp MC reco"

echo "=== Done: ${OUTDIR}/ ==="
