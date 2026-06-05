#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteCompatibility

TAG="${TAG:-$OFFICIAL_TAG_PPB}"
TAG12="${TAG12:-${TAG}_bin12x12_20260603}"
OUTPUT_DIR="${OUTPUT_DIR:-$THISDIR/output}"

# No 20-bin result files produced; skip 20-bin block.
# echo "=== 20-bin surface (tag: $TAG) ==="
# ./ExecuteCompatibility --Tag "$TAG" --OutputDir "$OUTPUT_DIR" --BaseDir "${OFFICIAL_RESULT_DIR}"

echo "=== 12-bin surface (tag: $TAG12) ==="
./ExecuteCompatibility --Tag "$TAG12" --OutputDir "$THISDIR/output_12x12" --BaseDir "${OFFICIAL_RESULT_DIR}" \
    --EtaFirstBin 6 --EtaLastBin 11 --PhiFirstBin 3 --PhiLastBin 8
