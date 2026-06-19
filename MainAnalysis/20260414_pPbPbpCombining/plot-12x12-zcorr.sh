#!/bin/bash
# Study 12: Z correction applied to Pbp.
# Pair 1 (solid): pPb nominal vs Pbp Z-corrected.
# Pair 2 (dashed): pPb nominal vs Pbp nominal (uncorrected reference).
# Outputs to plots/12x12_zcorr/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_zcorr"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile  "${OFFICIAL_PBP_BIN12_ZCORR_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel  "pPb data" \
    --PbPLabel  "Pbp (Z corrected)" \
    --pPbFile2 "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile2 "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel2 "pPb data" \
    --PbPLabel2 "Pbp (nominal)" \
    --Tag "zcorr" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
