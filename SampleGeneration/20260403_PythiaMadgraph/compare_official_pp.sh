#!/bin/bash
set -euo pipefail

ScriptDir=$(cd "$(dirname "$0")" && pwd)
RepoBase=$(cd "$ScriptDir/../.." && pwd)

cd "$RepoBase"
source SetupAnalysis.sh
source OfficialWeightDictionary.sh

cd "$ScriptDir"
make ExecuteCompareOfficialPP

PRIVATE_INPUT=${PRIVATE_INPUT:-$ScriptDir/output/ZMuMu_5020_validation.root}
OFFICIAL_INPUT=${OFFICIAL_INPUT:-${OFFICIAL_MCGENINPUT_PP}}
OUTPUT_DIR=${OUTPUT_DIR:-$ScriptDir/plots/officialPP}

mkdir -p "$OUTPUT_DIR"

./ExecuteCompareOfficialPP \
   --PrivateInput "$PRIVATE_INPUT" \
   --OfficialInput "$OFFICIAL_INPUT" \
   --OutputDir "$OUTPUT_DIR"
