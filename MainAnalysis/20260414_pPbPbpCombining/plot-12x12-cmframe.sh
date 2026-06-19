#!/bin/bash
# Study 17: Plot Option A (CM-frame) signed combined pPb+Pbp vs pp.
# Uses jackknife uncertainties and full covariance chi-square from result files.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

PLOTDIR="${OFFICIAL_RESULT_DIR}"

make ExecutePlot12x12CombinedSigned ExecutePlot12x12FramesXCheck

OUTDIR_A="plots/12x12_cmframe"
mkdir -p "$OUTDIR_A"

echo "=== Option A: CM-frame signed combination ==="
./ExecutePlot12x12CombinedSigned \
    --pPbFile "${OFFICIAL_PPB_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --ppFile  "${OFFICIAL_PP_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --pPbResultFile "${PLOTDIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma_ZPT0_500-result.root" \
    --PbPResultFile "${PLOTDIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma_ZPT0_500-result.root" \
    --ppResultFile  "${PLOTDIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_cma_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR_A" \
    --Tag "cma"

echo "=== Done: ${OUTDIR_A}/ ==="
