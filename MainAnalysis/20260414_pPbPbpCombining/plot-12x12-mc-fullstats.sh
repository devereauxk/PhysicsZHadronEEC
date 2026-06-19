#!/bin/bash
# Plot pPb vs Pbp MC reco at full statistics (Study 6).
# Outputs to plots/12x12_mc_fullstats/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_mc_fullstats"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_MC_FULLSTATS_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_MC_FULLSTATS_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "mc_fullstats" \
    --pPbLabel "pPb MC reco" \
    --PbPLabel "Pbp MC reco"

echo "=== Done: ${OUTDIR}/ ==="
