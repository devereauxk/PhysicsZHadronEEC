#!/bin/bash
set -euo pipefail

ZCORR_DIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow

INPUT_TAG="${INPUT_TAG:-20260407_ZCorrection_V8}"

GEN_FILE="$ZCORR_DIR/output/closure_inputs/pp_${INPUT_TAG}_zPt0-500_gen.root"
RECO_FILE="$ZCORR_DIR/output/closure_inputs/pp_${INPUT_TAG}_zPt0-500_reco.root"

for f in "$GEN_FILE" "$RECO_FILE"; do
   if [ ! -f "$f" ]; then echo "Missing: $f" && exit 1; fi
done

mkdir -p plots/pp

./ExecuteZCorrections \
   --GenFile  "$GEN_FILE" \
   --RecoFile "$RECO_FILE" \
   --System pp \
   --Tag    "$INPUT_TAG" \
   --OutputDir plots/pp

echo "=== Done: plots/pp/ ==="
