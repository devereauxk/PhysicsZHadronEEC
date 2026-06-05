#!/bin/bash
# Study 20: Plot pp MC Gen boost-prescription closure.
# Compares nominal vs pPb-boosted vs Pbp-boosted (flipped) variants.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPMCBoost

OUTDIR="plots/12x12_ppmc_boost"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPMCBoost \
    --NomFile "${OFFICIAL_PP_BIN12_BOOST_NOM_PREFIX}_ZPT0_500-nosub.root" \
    --PPbFile "${OFFICIAL_PP_BIN12_BOOST_PPB_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PP_BIN12_BOOST_PBP_PREFIX}_ZPT0_500-nosub.root" \
    --OutputDir "$OUTDIR" \
    --Tag "boost"

echo "=== Done: ${OUTDIR}/ ==="
