#!/bin/bash
# run-pPbPbp-sym.sh
# Runs chi-square + KS tests and overlay plots for signed-then-symmetrized pPb vs Pbp.
# Reads signed convention result files (no _bin12x12_ suffix).

set -e
cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

BASEDIR="${OFFICIAL_RESULT_DIR}"
TAG="ZV10_trkV29_nmix10"
TRKRANGE="0.5_15"
OUTDIR="output_signed_sym"

PPB_FILE="${BASEDIR}/pPb_trkResidual_${TAG}_ZPT0_500-result.root"
PBP_FILE="${BASEDIR}/PbP_trkResidual_${TAG}_ZPT0_500-result.root"

echo "=== Signed+symmetrized pPb vs Pbp compatibility test ==="
echo "pPb: ${PPB_FILE}"
echo "PbP: ${PBP_FILE}"
echo ""

# Build if needed
make -j4 ExecuteCompatibilitySym ExecuteOverlaySym

mkdir -p "${OUTDIR}"

echo "--- Chi-square + KS tests ---"
./ExecuteCompatibilitySym \
    --pPbFile "${PPB_FILE}" \
    --PbPFile "${PBP_FILE}" \
    --TrkRange "${TRKRANGE}" \
    --OutputDir "${OUTDIR}" \
    --EtaFirstBin 6 --EtaLastBin 11 \
    --PhiFirstBin 3 --PhiLastBin 8

echo ""
echo "--- Overlay plots ---"
./ExecuteOverlaySym \
    --pPbFile "${PPB_FILE}" \
    --PbPFile "${PBP_FILE}" \
    --TrkRange "${TRKRANGE}" \
    --OutputDir "${OUTDIR}"

echo ""
echo "=== Done. Outputs in ${OUTDIR}/ ==="
ls -lh "${OUTDIR}/"
