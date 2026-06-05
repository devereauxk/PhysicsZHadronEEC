#!/bin/bash
# Plot DeltaVZ mixing sensitivity study (Study 5): 3 pairs.
# Pair 1 (solid):     pPb + Pbp, MaxMixDeltaVZ<1.0 cm (nominal, official bin12x12_20260603)
# Pair 2 (long-dash): pPb + Pbp, MaxMixDeltaVZ<0.5 cm (variation)
# Pair 3 (dotted):    pPb + Pbp, MaxMixDeltaVZ off (variation)
# Outputs to plots/12x12_deltavz/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_deltavz"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile  "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel "pPb |DeltaVZ|<1.0 cm (nominal)" \
    --PbPLabel "Pbp |DeltaVZ|<1.0 cm (nominal)" \
    --pPbFile2 "${OFFICIAL_PPB_BIN12_DVZ0P5_PREFIX}_ZPT0_500-result.root" \
    --PbPFile2 "${OFFICIAL_PBP_BIN12_DVZ0P5_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel2 "pPb |DeltaVZ|<0.5 cm" \
    --PbPLabel2 "Pbp |DeltaVZ|<0.5 cm" \
    --pPbFile3 "${OFFICIAL_PPB_BIN12_DVZOFF_PREFIX}_ZPT0_500-result.root" \
    --PbPFile3 "${OFFICIAL_PBP_BIN12_DVZOFF_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel3 "pPb |DeltaVZ| off" \
    --PbPLabel3 "Pbp |DeltaVZ| off" \
    --OutputDir "$OUTDIR" \
    --Tag "deltavz"

echo "=== Done: ${OUTDIR}/ ==="
