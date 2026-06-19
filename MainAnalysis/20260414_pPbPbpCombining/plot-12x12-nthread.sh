#!/bin/bash
# Study 11: nThread comparison — 20 vs 40 vs 60.
# 6 curves: pPb/Pbp x nthread=20/40/60.
# Outputs to plots/12x12_nthread/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_nthread"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${OFFICIAL_PPB_BIN12_NTHREAD20_PREFIX}_ZPT0_500-result.root" \
    --PbPFile  "${OFFICIAL_PBP_BIN12_NTHREAD20_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel  "pPb nThread=20" \
    --PbPLabel  "Pbp nThread=20" \
    --pPbFile2 "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile2 "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel2 "pPb nThread=40" \
    --PbPLabel2 "Pbp nThread=40" \
    --pPbFile3 "${OFFICIAL_PPB_BIN12_NTHREAD60_PREFIX}_ZPT0_500-result.root" \
    --PbPFile3 "${OFFICIAL_PBP_BIN12_NTHREAD60_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel3 "pPb nThread=60" \
    --PbPLabel3 "Pbp nThread=60" \
    --Tag "nthread" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
