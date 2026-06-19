#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteCorr2DMapBin12x12

PPBTAG="ZV9_trkV28_nmix10_bin12x12_20260507"
PPTAG="EEV5_ZV9_trkV28_nmix10_bin12x12_20260507"
INDIR="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots"
OUTDIR="plots/2d_corr_bin12x12"

mkdir -p "$OUTDIR"

# Inclusive: pp and pPb side-by-side in one combined PDF
./ExecuteCorr2DMapBin12x12 \
    --PbPbTag "$PPBTAG" --PPTag "$PPTAG" \
    --Mode inclusive \
    --ZPtRanges 0_500 --TrkPtRanges 0.5_15 \
    --InputDir "$INDIR" --OutputDir "$OUTDIR" \
    --OutputName 2d_corr_bin12x12_inclusive

# Scan: separate pp and pPb 2×3 composites
./ExecuteCorr2DMapBin12x12 \
    --PbPbTag "$PPBTAG" --PPTag "$PPTAG" \
    --Mode scan \
    --ZPtRanges 0_30,30_500 --TrkPtRanges 0.5_2,2_4,4_15 \
    --InputDir "$INDIR" --OutputDir "$OUTDIR" \
    --OutputName 2d_corr_bin12x12_scan

echo "Done. PDFs in $OUTDIR/"
