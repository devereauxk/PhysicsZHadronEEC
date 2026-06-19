#!/bin/bash
# Study 13: Sideband study — 1 < |DeltaPhi| < 2 AND |DeltaEta| > 2.
# 1D projections + 2D heatmaps (SE and SE-ME) for pPb, Pbp, and ratio.
# Outputs to plots/12x12_sideband/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12Sideband

OUTDIR="plots/12x12_sideband"
mkdir -p "$OUTDIR"

./ExecutePlot12x12Sideband \
    --pPbFile "${OFFICIAL_PPB_BIN20_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PBP_BIN20_PREFIX}_ZPT0_500-nosub.root" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
