#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
export ProjectBase=/home/kdeverea/PhysicsZHadronEEC
ANALYSIS_DIR=$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt
mkdir -p plots/pPb

make ExecuteJewelPPDiagnostics

./ExecuteJewelPPDiagnostics \
    --Input8160 "$ANALYSIS_DIR/plots/jewelPPb_ZPT0_500-nosub.root" \
    --Input5020 "$ANALYSIS_DIR/plots/jewelPP8160_ZPT0_500-nosub.root" \
    --TrackPTRange 0.5_15 \
    --OutputBase plots/pPb/jewel_pPb_diag \
    --Label8160 JEWEL_pPb_8.16_TeV \
    --Label5020 JEWEL_pp_8.16_TeV \
    --RatioLabel pPb_/_pp

echo "=== Diagnostic plots ==="
ls plots/pPb/jewel_pPb_diag*.pdf
