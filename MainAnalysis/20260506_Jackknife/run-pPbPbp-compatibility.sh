#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteCompatibility

TAG="${TAG:-$OFFICIAL_TAG_PPB}"
OUTPUT_DIR="${OUTPUT_DIR:-$THISDIR/output}"

echo "=== Base tag unsymmetrized (tag: $TAG) ==="
./ExecuteCompatibility --Tag "$TAG" --OutputDir "$THISDIR/output_12x12" --BaseDir "${OFFICIAL_RESULT_DIR}" \
    --EtaFirstBin 0 --EtaLastBin 11 --PhiFirstBin 0 --PhiLastBin 11
