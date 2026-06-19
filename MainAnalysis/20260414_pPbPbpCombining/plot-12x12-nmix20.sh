#!/bin/bash
# Study 8: Nmix sensitivity — Nmix=10 (solid), Nmix=20 (long-dash), Nmix=40 (dotted).
# Outputs to plots/12x12_nmix20/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_nmix20"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile  "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel  "pPb Nmix=10" \
    --PbPLabel  "Pbp Nmix=10" \
    --pPbFile2 "${OFFICIAL_PPB_BIN12_NMIX20_PREFIX}_ZPT0_500-result.root" \
    --PbPFile2 "${OFFICIAL_PBP_BIN12_NMIX20_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel2 "pPb Nmix=20" \
    --PbPLabel2 "Pbp Nmix=20" \
    --pPbFile3 "${OFFICIAL_PPB_BIN12_NMIX40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile3 "${OFFICIAL_PBP_BIN12_NMIX40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel3 "pPb Nmix=40" \
    --PbPLabel3 "Pbp Nmix=40" \
    --OutputDir "$OUTDIR" \
    --Tag "nmix20"

echo "=== Done: ${OUTDIR}/ ==="
