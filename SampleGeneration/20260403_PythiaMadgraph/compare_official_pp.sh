#!/bin/bash
set -euo pipefail

ScriptDir=$(cd "$(dirname "$0")" && pwd)
RepoBase=$(cd "$ScriptDir/../.." && pwd)

cd "$RepoBase"
source SetupAnalysis.sh

cd "$ScriptDir"
make ExecuteCompareOfficialPP

PRIVATE_INPUT=${PRIVATE_INPUT:-$ScriptDir/output/ZMuMu_5020_validation.root}
OFFICIAL_INPUT=${OFFICIAL_INPUT:-/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pp-v11-Zpt0.root}
OUTPUT_DIR=${OUTPUT_DIR:-$ScriptDir/plots/officialPP}

mkdir -p "$OUTPUT_DIR"

./ExecuteCompareOfficialPP \
   --PrivateInput "$PRIVATE_INPUT" \
   --OfficialInput "$OFFICIAL_INPUT" \
   --OutputDir "$OUTPUT_DIR"
