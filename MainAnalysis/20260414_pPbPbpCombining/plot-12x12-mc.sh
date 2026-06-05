#!/bin/bash
# Plot pPb vs Pbp MC reco, Z-count-matched to data (Study 4).
# Outputs to plots/12x12_mc/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_mc"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_MC_MATCHED_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_MC_MATCHED_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "mc_matched" \
    --pPbLabel "pPb MC reco" \
    --PbPLabel "Pbp MC reco"

echo "=== Done: ${OUTDIR}/ ==="
