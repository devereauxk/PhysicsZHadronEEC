#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteKS

TAG="${TAG:-$OFFICIAL_TAG_PPB}"
TAG12="${TAG}_bin12x12_20260603"

# No 20-bin result files produced; skip 20-bin block.
# echo "=== 20-bin surface (tag: $TAG) ==="
# ./ExecuteKS --Tag "$TAG" --OutputDir output/ --BaseDir "${OFFICIAL_RESULT_DIR}"

echo "=== 12-bin surface (tag: $TAG12) ==="
./ExecuteKS --Tag "$TAG12" --OutputDir output_12x12/ --BaseDir "${OFFICIAL_RESULT_DIR}"

echo ""
echo "20-bin results:"
cat output/pPbPbp_ks.tsv

echo ""
echo "12-bin results:"
cat output_12x12/pPbPbp_ks.tsv
