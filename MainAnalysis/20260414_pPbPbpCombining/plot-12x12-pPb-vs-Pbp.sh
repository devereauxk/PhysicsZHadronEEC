#!/bin/bash
# Study 19: Plot pPb vs Pbp closure under matched CM acceptance (|eta_cm|<1.935).
# Uses CMA nosub files (same as Option A / Study 17).

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PbvsPbp

OUTDIR="plots/12x12_pPb_vs_Pbp"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PbvsPbp \
    --pPbFile "${OFFICIAL_PPB_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --OutputDir "$OUTDIR" \
    --Tag "pPbvsPbp"

echo "=== Done: ${OUTDIR}/ ==="
