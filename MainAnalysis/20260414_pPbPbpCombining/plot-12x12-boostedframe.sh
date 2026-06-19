#!/bin/bash
# Study 18: Plot Option B (pPb-oriented frame) signed combined pPb+Pbp vs pp.
# Uses jackknife uncertainties and full covariance chi-square from result files.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

PLOTDIR="${OFFICIAL_RESULT_DIR}"

make ExecutePlot12x12CombinedSigned ExecutePlot12x12FramesXCheck

OUTDIR_B="plots/12x12_boostedframe"
mkdir -p "$OUTDIR_B"

echo "=== Option B: pPb-oriented frame signed combination ==="
./ExecutePlot12x12CombinedSigned \
    --pPbFile "${OFFICIAL_PPB_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --ppFile  "${OFFICIAL_PP_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --pPbResultFile "${PLOTDIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cmb_ZPT0_500-result.root" \
    --PbPResultFile "${PLOTDIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cmb_ZPT0_500-result.root" \
    --ppResultFile  "${PLOTDIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_cmb_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR_B" \
    --Tag "cmb"

echo "=== Done: ${OUTDIR_B}/ ==="
