#!/bin/bash
# Study 15: Compare locally-run (_12x12_full) vs official (_bin12x12_20260603) results.
# Pair 1 (solid): pPb local vs Pbp local.
# Pair 2 (dashed): pPb official vs Pbp official.
# Outputs to plots/12x12_local_vs_official/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_local_vs_official"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${OFFICIAL_PPB_BIN12_FULL_PREFIX}_ZPT0_500-result.root" \
    --PbPFile  "${OFFICIAL_PBP_BIN12_FULL_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel  "pPb local (_full)" \
    --PbPLabel  "Pbp local (_full)" \
    --pPbFile2 "${OFFICIAL_PPB_NOSUB_PREFIX_BIN12}_ZPT0_500-result.root" \
    --PbPFile2 "${OFFICIAL_PBP_NOSUB_PREFIX_BIN12}_ZPT0_500-result.root" \
    --pPbLabel2 "pPb official" \
    --PbPLabel2 "Pbp official" \
    --Tag "localvsofficial" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
