#!/bin/bash
set -euo pipefail

ANALYSIS_DIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

INPUT_ORIG="$ANALYSIS_DIR/plots/jewelPPb_ZPT0_500-result.root"
INPUT_SLICED="$ANALYSIS_DIR/plots/jewelPPbSliced_ZPT0_500-result.root"

if [ ! -f "$INPUT_ORIG" ]; then
   echo "Missing $INPUT_ORIG — run jewel-pPb.sh first"
   exit 1
fi
if [ ! -f "$INPUT_SLICED" ]; then
   echo "Missing $INPUT_SLICED — run jewel-pPb-sliced.sh first"
   exit 1
fi

mkdir -p plots/pPb_sliced

./ExecuteJewelPPbSlicedComparison \
   --InputOriginal "$INPUT_ORIG" \
   --InputSliced   "$INPUT_SLICED" \
   --TrackPTRange  0.5_15 \
   --OutputDir     plots/pPb_sliced

echo "=== Output: plots/pPb_sliced/ ==="
