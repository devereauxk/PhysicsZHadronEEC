#!/bin/bash
# Plot pPb vs Pbp half-split compatibility (Study 1).
# Reads 12x12 result files from MainAnalysis/20241102_ZhadronVsZPt/plots/.
# Outputs 6 PDFs per comparison (4 comparisons × 6 = 24 PDFs) to plots/12x12_halfsplit/.

set -euo pipefail

cd "$(dirname "$0")"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

make ExecutePlot12x12PPbPbP

OUTDIR="plots/12x12_halfsplit"
mkdir -p "$OUTDIR"

run_plot() {
    local ppb_file=$1
    local pbp_file=$2
    local tag=$3
    local ppb_label=$4
    local pbp_label=$5
    ./ExecutePlot12x12PPbPbP \
        --pPbFile "$ppb_file" \
        --PbPFile "$pbp_file" \
        --OutputDir "$OUTDIR" \
        --Tag "$tag" \
        --pPbLabel "$ppb_label" \
        --PbPLabel "$pbp_label"
}

FULL_PPB="${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root"
FULL_PBP="${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root"
FIRST_PPB="${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_pPbFirst_ZPT0_500-result.root"
SECOND_PPB="${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_pPbSecond_ZPT0_500-result.root"
FIRST_PBP="${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_PbPFirst_ZPT0_500-result.root"
SECOND_PBP="${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_PbPSecond_ZPT0_500-result.root"

echo "=== pPb first half vs Pbp full ==="
run_plot "$FIRST_PPB"  "$FULL_PBP"  "halfsplit_pPbFirst"  "pPb first half"  "Pbp full"

echo "=== pPb second half vs Pbp full ==="
run_plot "$SECOND_PPB" "$FULL_PBP"  "halfsplit_pPbSecond" "pPb second half" "Pbp full"

echo "=== Pbp first half vs pPb full ==="
run_plot "$FULL_PPB"   "$FIRST_PBP"  "halfsplit_PbPFirst"  "pPb full"        "Pbp first half"

echo "=== Pbp second half vs pPb full ==="
run_plot "$FULL_PPB"   "$SECOND_PBP" "halfsplit_PbPSecond" "pPb full"        "Pbp second half"

echo "=== Done: ${OUTDIR}/ ==="
