#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteKS

TAG12="${OFFICIAL_TAG_PPB}_bin12x12_20260603"
NPERM="${NPERM:-100000}"

echo "=== 12-bin KS test with ${NPERM} permutations (tag: $TAG12) ==="
./ExecuteKS --Tag "$TAG12" --OutputDir output/ --BaseDir "${OFFICIAL_RESULT_DIR}" --NPerm "$NPERM"

echo ""
echo "Results:"
cat output/pPbPbp_ks.tsv
