#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
export ProjectBase=/home/kdeverea/PhysicsZHadronEEC
ANALYSIS_DIR=$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt
mkdir -p plots/pPb

make ExecuteJewelPPb

./ExecuteJewelPPb \
    --InputPPb "$ANALYSIS_DIR/plots/jewelPPb_ZPT0_500-result.root" \
    --InputPP "$ANALYSIS_DIR/plots/jewelPP8160_ZPT0_500-result.root" \
    --TrackPTRange 0.5_15 \
    --OutputBase plots/pPb/jewel_pPb

echo "=== Result plots ==="
ls plots/pPb/jewel_pPb*.pdf
