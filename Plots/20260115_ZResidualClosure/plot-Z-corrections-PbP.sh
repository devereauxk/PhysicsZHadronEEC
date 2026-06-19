#!/bin/bash
set -euo pipefail

ZCORR_DIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow

INPUT_TAG="${INPUT_TAG:-20260407_ZCorrection_V8}"

GEN_FILE="$ZCORR_DIR/output/closure_inputs/PbP_${INPUT_TAG}_zPt0-500_gen.root"
RECO_FILE="$ZCORR_DIR/output/closure_inputs/PbP_${INPUT_TAG}_zPt0-500_reco.root"

for f in "$GEN_FILE" "$RECO_FILE"; do
   if [ ! -f "$f" ]; then echo "Missing: $f" && exit 1; fi
done

mkdir -p plots/PbP

./ExecuteZCorrections \
   --GenFile  "$GEN_FILE" \
   --RecoFile "$RECO_FILE" \
   --System PbP \
   --Tag    "$INPUT_TAG" \
   --OutputDir plots/PbP

echo "=== Done: plots/PbP/ ==="
