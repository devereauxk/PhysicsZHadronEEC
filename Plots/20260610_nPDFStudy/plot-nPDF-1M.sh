#!/bin/bash
# Plot all nPDF 1M comparisons: inclusive + individual scan bins
set -euo pipefail

cd "$(dirname "$0")"
export ProjectBase=$(cd ../.. && pwd)
export PATH=$ProjectBase/CommonCode/binary/:$PATH

make ExecuteNPDFIndividual 2>&1

INPUT_DIR="$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt/plots"
TAG="nPDF_study_1M"
OUTBASE="plots/nPDF_1M"
mkdir -p "$OUTBASE"

RESULT_DIR="$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt/plots"
PP_TAG="EEV6_ZV10_trkV29_nmix10"
PPB_TAG="ZV10_trkV29_nmix10"

DATA_ARGS=(
    --PPFile  "$RESULT_DIR/pp_trkResidual_${PP_TAG}_ZPT0_500-result.root"
    --PPbFile "$RESULT_DIR/pPb_trkResidual_${PPB_TAG}_ZPT0_500-nosub.root"
    --PbPFile "$RESULT_DIR/PbP_trkResidual_${PPB_TAG}_ZPT0_500-nosub.root"
)

# Inclusive
./ExecuteNPDFIndividual \
    --InputDir "$INPUT_DIR" --Tag "$TAG" \
    --ZPTRange 0_500 --TrackPTRange 0.5_15 \
    --OutputBase "$OUTBASE/nPDF_ZPT0_500_trkPT0.5_15" \
    "${DATA_ARGS[@]}"

# Scan bins — data files use _scan_ZPT{range} with all trkPT bins inside
for ZPT in 0_30 30_500; do
    SCAN_DATA_ARGS=(
        --PPFile  "$RESULT_DIR/pp_trkResidual_${PP_TAG}_scan_ZPT${ZPT}-result.root"
        --PPbFile "$RESULT_DIR/pPb_trkResidual_${PPB_TAG}_scan_ZPT${ZPT}-nosub.root"
        --PbPFile "$RESULT_DIR/PbP_trkResidual_${PPB_TAG}_scan_ZPT${ZPT}-nosub.root"
    )
    for TRK in 0.5_2 2_4 4_15; do
        ./ExecuteNPDFIndividual \
            --InputDir "$INPUT_DIR" --Tag "$TAG" \
            --ZPTRange "$ZPT" --TrackPTRange "$TRK" \
            --OutputBase "$OUTBASE/nPDF_ZPT${ZPT}_trkPT${TRK}" \
            "${SCAN_DATA_ARGS[@]}"
    done
done

echo "=== All 1M nPDF plots done ==="
ls -la "$OUTBASE"/*.pdf
