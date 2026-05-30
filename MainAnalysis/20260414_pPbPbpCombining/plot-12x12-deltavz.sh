#!/bin/bash
# Plot ΔVZ sensitivity study (Study 5): 3 pairs.
# Pair 1 (solid):     pPb + Pbp, MaxMixDeltaVZ<0.5 (nominal, reuses _12x12_full)
# Pair 2 (long-dash): pPb + Pbp, MaxMixDeltaVZ off
# Pair 3 (dotted):    pPb + Pbp, MaxMixDeltaVZ<1.0
# Outputs to plots/12x12_deltavz/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_deltavz"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --PbPFile  "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root" \
    --pPbLabel "pPb |DeltaVZ|<0.5 cm" \
    --PbPLabel "Pbp |DeltaVZ|<0.5 cm" \
    --pPbFile2 "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_dvzOff_ZPT0_500-result.root" \
    --PbPFile2 "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_dvzOff_ZPT0_500-result.root" \
    --pPbLabel2 "pPb |DeltaVZ| off" \
    --PbPLabel2 "Pbp |DeltaVZ| off" \
    --pPbFile3 "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_dvz1_ZPT0_500-result.root" \
    --PbPFile3 "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_dvz1_ZPT0_500-result.root" \
    --pPbLabel3 "pPb |DeltaVZ|<1.0 cm" \
    --PbPLabel3 "Pbp |DeltaVZ|<1.0 cm" \
    --OutputDir "$OUTDIR" \
    --Tag "deltavz"

echo "=== Done: ${OUTDIR}/ ==="
