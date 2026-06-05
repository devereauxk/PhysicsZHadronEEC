#!/bin/bash
# Study 10: ME scale ±0.3% (Pbp ME only).
# 3 pairs: nominal, Pbp ME x1.003, Pbp ME x0.997.
# Outputs to plots/12x12_mescale/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_mescale"
mkdir -p "$OUTDIR"

NOM_PPB="${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root"
NOM_PBP="${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root"

./ExecutePlot12x12PPbPbP \
    --pPbFile  "$NOM_PPB" \
    --PbPFile  "$NOM_PBP" \
    --pPbLabel  "pPb (nominal)"    --PbPLabel  "Pbp (nominal)" \
    --pPbFile2 "$NOM_PPB" \
    --PbPFile2 "$NOM_PBP" \
    --pPbLabel2 "pPb"              --PbPLabel2 "Pbp ME +0.3%" \
    --pPbFile3 "$NOM_PPB" \
    --PbPFile3 "$NOM_PBP" \
    --pPbLabel3 "pPb"              --PbPLabel3 "Pbp ME -0.3%" \
    --MEScalePair2 1.003 \
    --MEScalePair3 0.997 \
    --Tag "mescale" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
