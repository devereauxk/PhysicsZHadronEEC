#!/bin/bash
# Study 3: chi-squared p-value compatibility for signed DeltaEta/DeltaPhi.
# Three comparisons: pPb vs Pbp, pPb vs pp, Pbp vs pp (all signed).

set -euo pipefail

cd "$(dirname "$0")"

THISDIR=$(pwd)

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

COMPAT=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260506_Jackknife

cd /home/kdeverea/PhysicsZHadronEEC && source SetupAnalysis.sh
cd "$COMPAT" && make ExecuteCompatibility
cd "$THISDIR"

BIN_ARGS=(--EtaFirstBin 6 --EtaLastBin 11 --PhiFirstBin 6 --PhiLastBin 11)

echo "=== Study 3a: pPb vs Pbp signed ==="
mkdir -p "$THISDIR/output_12x12_signed"
"$COMPAT/ExecuteCompatibility" \
    --pPbFile "${OFFICIAL_PPB_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$THISDIR/output_12x12_signed" \
    "${BIN_ARGS[@]}"

echo "=== Study 3b: pPb vs pp signed ==="
mkdir -p "$THISDIR/output_12x12_signed_ppbvpp"
"$COMPAT/ExecuteCompatibility" \
    --pPbFile "${OFFICIAL_PPB_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PP_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$THISDIR/output_12x12_signed_ppbvpp" \
    "${BIN_ARGS[@]}"

echo "=== Study 3c: Pbp vs pp signed ==="
mkdir -p "$THISDIR/output_12x12_signed_pbpvpp"
"$COMPAT/ExecuteCompatibility" \
    --pPbFile "${OFFICIAL_PBP_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PP_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$THISDIR/output_12x12_signed_pbpvpp" \
    "${BIN_ARGS[@]}"

echo "=== Done: p-value tables in output_12x12_signed*/ ==="
