#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

make ExecuteCompatibility

TAG="${TAG:-$OFFICIAL_TAG_PPB}"
OUTPUT_DIR="${OUTPUT_DIR:-$THISDIR/output}"

./ExecuteCompatibility --Tag "$TAG" --OutputDir "$OUTPUT_DIR"
