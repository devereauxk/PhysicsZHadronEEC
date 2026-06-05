#!/bin/bash
# Study 9: CM-frame signed DeltaEta/DeltaPhi comparison.
# pPb uses _12x12_signed (lab frame eta = cm frame eta for pPb orientation).
# Pbp uses _12x12_signed_boost (yBoost=0.465 + FlipDeltaEta → CM frame).
# pp uses _12x12_signed (no boost; lab = cm for pp).
# Outputs to plots/12x12_boost/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_boost"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_SIGNED_BOOST_PREFIX}_ZPT0_500-result.root" \
    --ppFile  "${OFFICIAL_PP_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "boost" \
    --pPbLabel "pPb (CM frame)" \
    --PbPLabel "Pbp (CM frame)" \
    --ppLabel  "pp (lab frame)" \
    --ShowSignedEta true \
    --ShowFullPhi true

echo "=== Done: ${OUTDIR}/ ==="
