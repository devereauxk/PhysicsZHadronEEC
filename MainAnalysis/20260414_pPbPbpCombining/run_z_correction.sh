#!/bin/bash
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
cd "$(dirname "$0")"

DERIVATION_LEVEL="${DERIVATION_LEVEL:-VZ_Z_ZSF}"
OUTPUT_FILE="${1:-output/z_correction_final.root}"
PPB_INPUT="output/pPb_${DERIVATION_LEVEL}.root"
PBP_INPUT="output/PbP_${DERIVATION_LEVEL}.root"

echo "=== Deriving direct 2D Z correction (pPb→PbP) from ${DERIVATION_LEVEL} ==="
root -l -q -b "z_correction.C(\"${PPB_INPUT}\",\"${PBP_INPUT}\",\"${OUTPUT_FILE}\")"
echo "Final Z correction: ${OUTPUT_FILE}"
echo "=== Done ==="
