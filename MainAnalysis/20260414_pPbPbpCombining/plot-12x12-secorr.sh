#!/bin/bash
# Study 16: SE-derived scale factor applied to ME_Pbp.
# sf[i,j] = SE_pPb[i,j] / SE_Pbp[i,j]; corrME_Pbp = ME_Pbp * sf; result = SE_Pbp - corrME_Pbp.
# No closure expected in SE, ME, or result. Outputs to plots/12x12_secorr/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_secorr"
mkdir -p "$OUTDIR"

./ExecutePlot12x12PPbPbP \
    --pPbFile "${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root" \
    --pPbLabel "pPb data" \
    --PbPLabel "Pbp (SE corrected)" \
    --SECorrPbP true \
    --ShowNominalPbP true \
    --NominalPbPLabel "Pbp (nominal)" \
    --Tag "secorr" \
    --OutputDir "$OUTDIR"

echo "=== Done: ${OUTDIR}/ ==="
