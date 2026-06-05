#!/bin/bash
# Study 9 p-value: chi-squared compatibility between pPb (CM frame) and Pbp (CM frame).
# Reads pPb_signed and PbP_signed_boost result files.
# Outputs TSV/Markdown/TeX tables and correlation heatmaps to output_12x12_boost/.

set -euo pipefail

cd "$(dirname "$0")"

THISDIR=$(pwd)

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

COMPAT=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260506_Jackknife

cd /home/kdeverea/PhysicsZHadronEEC && source SetupAnalysis.sh
cd "$COMPAT" && make ExecuteCompatibility
cd "$THISDIR"

OUTDIR="$THISDIR/output_12x12_boost"
mkdir -p "$OUTDIR"

echo "=== Study 9 pPb vs Pbp (CM frame) compatibility ==="

"$COMPAT/ExecuteCompatibility" \
    --pPbFile "${OFFICIAL_PPB_BIN12_SIGNED_PREFIX}_ZPT0_500-result.root" \
    --PbPFile "${OFFICIAL_PBP_BIN12_SIGNED_BOOST_PREFIX}_ZPT0_500-result.root" \
    --OutputDir "$OUTDIR" \
    --EtaFirstBin 6 --EtaLastBin 11 \
    --PhiFirstBin 3 --PhiLastBin 8

echo "=== Done: p-value table in ${OUTDIR}/pPbPbp_compatibility.tsv ==="
