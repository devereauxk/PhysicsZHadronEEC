#!/bin/bash
# Study 0: nominal pPb vs Pbp comparison, fully corrected, no special cuts.
# Reads official ZV10/trkV29 12x12 result from OfficialProductDictionary.sh.
# Outputs to plots/12x12_nominal/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_nominal"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "nominal" \
    --pPbLabel "pPb data" \
    --PbPLabel "Pbp data"

echo "=== Done: ${OUTDIR}/ ==="
