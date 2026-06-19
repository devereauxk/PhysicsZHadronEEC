#!/bin/bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd /home/kdeverea/PhysicsZHadronEEC && source SetupAnalysis.sh
cd "$SCRIPT_DIR"

ANALYSIS_DIR=$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt
TAG="nPDF_study"

mkdir -p plots/nPDF
make ExecuteNPDF

./ExecuteNPDF \
    --InputEPPS21Nosub  "$ANALYSIS_DIR/plots/nPDF_epps21_${TAG}_ZPT0_500-nosub.root" \
    --InputCT18Nosub    "$ANALYSIS_DIR/plots/nPDF_ct18anlo_${TAG}_ZPT0_500-nosub.root" \
    --InputNNPDF31Nosub "$ANALYSIS_DIR/plots/nPDF_nnpdf31_${TAG}_ZPT0_500-nosub.root" \
    --InputEPPS21Result  "$ANALYSIS_DIR/plots/nPDF_epps21_${TAG}_ZPT0_500-result.root" \
    --InputCT18Result    "$ANALYSIS_DIR/plots/nPDF_ct18anlo_${TAG}_ZPT0_500-result.root" \
    --InputNNPDF31Result "$ANALYSIS_DIR/plots/nPDF_nnpdf31_${TAG}_ZPT0_500-result.root" \
    --TrackPTRange 0.5_15 \
    --OutputBase plots/nPDF/nPDF

echo "=== nPDF comparison plots ==="
ls plots/nPDF/*.pdf
