#!/bin/bash
# Study 22: Symmetrized final result in the pPb-oriented frame (Option B, CMB).
# R_sym(x) = 0.5*(R(+x)+R(-x)); stat errors from symmetrized jackknife replicas.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

PLOTDIR="${OFFICIAL_RESULT_DIR}"

make ExecutePlot12x12Symmetrized

OUTDIR="plots/12x12_sym_cmb"
mkdir -p "$OUTDIR"

echo "=== Study 22: symmetrized result, pPb-oriented frame (CMB) ==="
./ExecutePlot12x12Symmetrized \
    --pPbFile "${OFFICIAL_PPB_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --ppFile  "${OFFICIAL_PP_BIN12_CMB_PREFIX}_ZPT0_500-nosub.root" \
    --pPbResultFile "${PLOTDIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cmb_ZPT0_500-result.root" \
    --PbPResultFile "${PLOTDIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}_12x12_cmb_ZPT0_500-result.root" \
    --ppResultFile  "${PLOTDIR}/pp_trkResidual_${OFFICIAL_TAG_PP}_12x12_cmb_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --Tag "cmb_sym"

echo "=== Done: ${OUTDIR}/ ==="
