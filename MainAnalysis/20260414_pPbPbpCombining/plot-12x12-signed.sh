#!/bin/bash
# Plot pPb vs Pbp with signed (unsymmetrized) DeltaEta (Study 3).
# Outputs to plots/12x12_signed/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_signed"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --ppFile  "${OFFICIAL_PP_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "signed" \
    --pPbLabel "pPb data" \
    --PbPLabel "Pbp data" \
    --ppLabel  "pp 8.16 TeV" \
    --ShowSignedEta true \
    --ShowFullPhi true

echo "=== Done: ${OUTDIR}/ ==="
