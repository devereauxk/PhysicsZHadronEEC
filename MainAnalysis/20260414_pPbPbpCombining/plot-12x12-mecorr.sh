#!/bin/bash
# Study 9: ME correction — Pbp ME corrected via (SE/ME) double ratio to match pPb ME.
# Outputs to plots/12x12_mecorr/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_mecorr"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel "pPb data" \
    --PbPLabel "Pbp (ME corrected)" \
    --MECorrPbP true \
    --ShowNominalPbP true \
    --NominalPbPLabel "Pbp (nominal)" \
    --Tag "mecorr" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
