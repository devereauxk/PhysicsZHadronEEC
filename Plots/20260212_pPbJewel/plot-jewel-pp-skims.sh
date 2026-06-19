#!/bin/bash
set -euo pipefail

ANALYSIS_DIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

INPUT_PPB="$ANALYSIS_DIR/plots/jewelPP5020_ZPT0_500-result.root"
INPUT_PPBB="$ANALYSIS_DIR/plots/jewelPPv9_ZPT0_500-result.root"

if [ ! -f "$INPUT_PPB" ]; then
   echo "Missing $INPUT_PPB — run jewel-pp-energy.sh first"
   exit 1
fi
if [ ! -f "$INPUT_PPBB" ]; then
   echo "Missing $INPUT_PPBB — run jewel-pp-v9.sh first"
   exit 1
fi

mkdir -p plots/pp_skims

./ExecuteJewelPPSkims \
   --InputPPb  "$INPUT_PPB" \
   --InputPPbB "$INPUT_PPBB" \
   --TrackPTRange 0.5_15 \
   --OutputDir plots/pp_skims

echo "=== Output: plots/pp_skims/ ==="
