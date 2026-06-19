#!/bin/bash
# Compare local Jewel pp 5.02 TeV vs v9 (PbPb-study) in signed convention.
set -euo pipefail

ANALYSIS_DIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

INPUT_LOCAL="$ANALYSIS_DIR/plots/jewelPP5020signed_ZPT0_500-result.root"
INPUT_V9="$ANALYSIS_DIR/plots/jewelPPv9signed_ZPT0_500-result.root"

for f in "$INPUT_LOCAL" "$INPUT_V9"; do
   if [ ! -f "$f" ]; then
      echo "Missing $f — run jewel-pp-skims-signed.sh first"
      exit 1
   fi
done

mkdir -p plots/pp_skims_signed

./ExecuteJewelPPSkims \
   --InputPPb  "$INPUT_LOCAL" \
   --InputPPbB "$INPUT_V9" \
   --TrackPTRange 0.5_15 \
   --DEtaRange 3.87 \
   --LabelA "JEWEL pp 5.02 TeV (local)" \
   --LabelB "JEWEL pp 5.02 TeV (v9/PbPb study)" \
   --OutputDir plots/pp_skims_signed

echo "=== Output: plots/pp_skims_signed/ ==="
