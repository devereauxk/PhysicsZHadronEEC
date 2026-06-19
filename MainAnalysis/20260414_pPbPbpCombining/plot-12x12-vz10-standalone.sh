#!/bin/bash
# Study 2 standalone: pPb |vz|<10 cm vs Pbp |vz|<10 cm (single pair, no nominal overlay).
# Outputs to plots/12x12_vz10_standalone/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_vz10_standalone"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "${OFFICIAL_PPB_BIN12_VZ10_PREFIX}_ZPT0_500-result.root" \
    --PbPFile  "${OFFICIAL_PBP_BIN12_VZ10_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel "pPb |vz|<10 cm" \
    --PbPLabel "Pbp |vz|<10 cm" \
    --Tag "vz10standalone" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
