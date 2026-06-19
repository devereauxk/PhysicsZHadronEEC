#!/bin/bash
# Study 14: Combined pPb+Pbp from signed distributions, folded to |eta|×|phi|.
# Inputs: pPb, Pbp, pp signed nosub files.
# Outputs to plots/12x12_combined_signed/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12CombinedSigned

OUTDIR="plots/12x12_combined_signed"
mkdir -p "$OUTDIR"

./ExecutePlot12x12CombinedSigned \
    --pPbFile "${OFFICIAL_PPB_BIN12_SIGNED_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_SIGNED_PREFIX}_ZPT0_500-nosub.root" \
    --ppFile  "${OFFICIAL_PP_BIN12_SIGNED_PREFIX}_ZPT0_500-nosub.root" \
    --OutputDir "$OUTDIR" \
    --Tag "combined"

echo "=== Done: ${OUTDIR}/ ==="
