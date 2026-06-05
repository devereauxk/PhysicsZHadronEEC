#!/bin/bash
# Study 0: chi-squared p-value + covariance KS-like test for nominal pPb vs Pbp.
# Inputs: local nthread=40 result files (same stack as nominal study).

set -euo pipefail

cd "$(dirname "$0")"
THISDIR=$(pwd)

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

COMPAT=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260506_Jackknife

cd /home/kdeverea/PhysicsZHadronEEC && source SetupAnalysis.sh
cd "$COMPAT" && make ExecuteCompatibility ExecuteKS
cd "$THISDIR"

PPB_FILE="${OFFICIAL_PPB_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root"
PBP_FILE="${OFFICIAL_PBP_BIN12_NTHREAD40_PREFIX}_ZPT0_500-result.root"

echo "=== Study 0: chi-squared compatibility (nthread=40 local files) ==="
mkdir -p "$THISDIR/output_12x12_nominal"
"$COMPAT/ExecuteCompatibility" \
    --pPbFile "$PPB_FILE" \
    --PbPFile "$PBP_FILE" \
    --OutputDir "$THISDIR/output_12x12_nominal" \
    --EtaFirstBin 6 --EtaLastBin 11 \
    --PhiFirstBin 3 --PhiLastBin 8

echo ""
echo "=== Study 0: covariance-aware KS-like test ==="
python3 "$THISDIR/compute_covKS.py" \
    --pPbFile "$PPB_FILE" \
    --PbPFile "$PBP_FILE" \
    --OutputDir "$THISDIR/output_12x12_nominal"

echo ""
echo "=== Study 0: standard KS test ==="
"$COMPAT/ExecuteKS" \
    --Tag "${OFFICIAL_TAG_PPB}_12x12_nthread40" \
    --BaseDir "$(dirname "$PPB_FILE")" \
    --OutputDir "$THISDIR/output_12x12_nominal"

echo ""
echo "=== Done: output_12x12_nominal/ ==="
