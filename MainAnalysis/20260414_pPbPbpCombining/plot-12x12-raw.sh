#!/bin/bash
# Plot pPb vs Pbp raw uncorrected data (Study 7).
# Outputs to plots/12x12_raw/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_raw"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${OFFICIAL_PPB_BIN12_RAW_PREFIX}_ZPT0_500-result.root" \
    --PbPFile  "${OFFICIAL_PBP_BIN12_RAW_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel "pPb raw data" \
    --PbPLabel "Pbp raw data" \
    --pPbFile2 "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile2 "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel2 "pPb data" \
    --PbPLabel2 "Pbp data" \
    --OutputDir "$OUTDIR" \
    --Tag "raw"

echo "=== Done: ${OUTDIR}/ ==="
