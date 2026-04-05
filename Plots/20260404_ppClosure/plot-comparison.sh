#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$SCRIPT_DIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

make ExecuteComparePP

PRIVATE_INPUT=${PRIVATE_INPUT:-/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20260403_PythiaMadgraph/output/ZMuMu_5020_validation.root}
OFFICIAL_INPUT=${OFFICIAL_INPUT:-${OFFICIAL_MCGENINPUT_PP}}
OUTPUT_DIR=${OUTPUT_DIR:-$SCRIPT_DIR/plots/compare}
OFFICIAL_MAX_EVENTS=${OFFICIAL_MAX_EVENTS:--1}
PRIVATE_MAX_EVENTS=${PRIVATE_MAX_EVENTS:--1}

mkdir -p "$OUTPUT_DIR"

./ExecuteComparePP \
   --PrivateInput "$PRIVATE_INPUT" \
   --OfficialInput "$OFFICIAL_INPUT" \
   --OutputDir "$OUTPUT_DIR" \
   --OfficialMaxEvents "$OFFICIAL_MAX_EVENTS" \
   --PrivateMaxEvents "$PRIVATE_MAX_EVENTS"
