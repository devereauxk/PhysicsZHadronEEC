#!/bin/bash
# Study 21: Symmetrized final result in the pp/CM frame (Option A, CMA).
# R_sym(x) = 0.5*(R(+x)+R(-x)); stat errors from symmetrized jackknife replicas.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

PLOTDIR="${OFFICIAL_RESULT_DIR}"
# Raw intermediate files (contain Jackknife2DData for full-eta phi JK covariance)
RAWDIR="$(dirname "${OFFICIAL_RESULT_DIR}")/output"

make ExecutePlot12x12Symmetrized

OUTDIR="plots/12x12_sym_cma"
mkdir -p "$OUTDIR"

echo "=== Study 21: symmetrized result, CM frame (CMA) ==="
./ExecutePlot12x12Symmetrized \
    --pPbFile "${OFFICIAL_PPB_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --ppFile  "${OFFICIAL_PP_BIN12_CMA_PREFIX}_ZPT0_500-nosub.root" \
    --pPbResultFile "${PLOTDIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma_ZPT0_500-result.root" \
    --PbPResultFile "${PLOTDIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma_ZPT0_500-result.root" \
    --ppResultFile  "${PLOTDIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_cma_ZPT0_500-result.root" \
    --pPbRawFile "${RAWDIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma_ZPT0_500-0.5_15.root" \
    --PbPRawFile "${RAWDIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cma_ZPT0_500-0.5_15.root" \
    --ppRawFile  "${RAWDIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_cma_ZPT0_500-0.5_15.root" \
    --OutputDir "$OUTDIR" \
    --Tag "cma_sym"

echo "=== Done: ${OUTDIR}/ ==="
