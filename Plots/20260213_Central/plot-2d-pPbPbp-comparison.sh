#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteCorr2DMapPPbPbP

source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
TAG="${TAG:-ZV10_trkV29_nmix10_bin12x12_20260602}"
INDIR="${INDIR:-${OFFICIAL_RESULT_DIR}}"
OUTDIR="plots/2d_pPbPbp_comparison"

mkdir -p "$OUTDIR"

# Inclusive: ZPT 0-500, trkPT 0.5-15 (1 row × 3 cols: pPb | Pbp | ratio)
./ExecuteCorr2DMapPPbPbP \
    --Tag "$TAG" --InputDir "$INDIR" --OutputDir "$OUTDIR" \
    --ZPtRange 0_500 --TrkPtRanges 0.5_15 \
    --OutputName 2d_pPbPbp_inclusive

# ZPT 0-30 scan: 3 trkPT rows × 3 cols
./ExecuteCorr2DMapPPbPbP \
    --Tag "$TAG" --InputDir "$INDIR" --OutputDir "$OUTDIR" \
    --ZPtRange 0_30 --TrkPtRanges 0.5_2,2_4,4_15 \
    --OutputName 2d_pPbPbp_scan_0_30

# ZPT 30-500 scan: 3 trkPT rows × 3 cols
./ExecuteCorr2DMapPPbPbP \
    --Tag "$TAG" --InputDir "$INDIR" --OutputDir "$OUTDIR" \
    --ZPtRange 30_500 --TrkPtRanges 0.5_2,2_4,4_15 \
    --OutputName 2d_pPbPbp_scan_30_500

echo "Done. PDFs in $OUTDIR/"
