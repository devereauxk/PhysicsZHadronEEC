#!/bin/bash
# Plot pPb vs Pbp raw uncorrected data (Study 7).
# Outputs to plots/12x12_raw/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_raw"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_raw_ZPT0_500-result.root" \
    --PbPFile  "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_raw_ZPT0_500-result.root" \
    --pPbLabel "pPb raw data" \
    --PbPLabel "Pbp raw data" \
    --pPbFile2 "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --PbPFile2 "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --pPbLabel2 "pPb data" \
    --PbPLabel2 "Pbp data" \
    --OutputDir "$OUTDIR" \
    --Tag "raw"

echo "=== Done: ${OUTDIR}/ ==="
