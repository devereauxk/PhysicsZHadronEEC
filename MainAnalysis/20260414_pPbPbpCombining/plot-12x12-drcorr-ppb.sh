#!/bin/bash
# Study 23: Double-ratio ME correction applied to Pbp ME.
# sf[i,j] = (SE_Pbp[i,j] / SE_pPb[i,j]) / (ME_Pbp[i,j] / ME_pPb[i,j]).
# corrME_Pbp = ME_Pbp * sf; corrected Pbp result = SE_Pbp - corrME_Pbp.
# By construction: ME ratio (pPb / corrME_Pbp) = SE ratio (pPb / Pbp) bin-by-bin.
# pPb SE, ME, and result unchanged. Three curves: pPb, Pbp (DR corr), Pbp (nominal).
# Outputs to plots/12x12_drcorr_pbp/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_drcorr_pbp"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel "pPb data" \
    --PbPLabel "Pbp (DR corrected)" \
    --DoubleRatioCorrPbP true \
    --ShowNominalPbP true \
    --NominalPbPLabel "Pbp (nominal)" \
    --Tag "drcorr_pbp" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
