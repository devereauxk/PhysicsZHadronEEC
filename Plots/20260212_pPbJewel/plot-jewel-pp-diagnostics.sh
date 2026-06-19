#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
export ProjectBase=/home/kdeverea/PhysicsZHadronEEC
ANALYSIS_DIR=$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt
mkdir -p plots/pp_energy

make ExecuteJewelPPDiagnostics

./ExecuteJewelPPDiagnostics \
    --Input8160 "$ANALYSIS_DIR/plots/jewelPP8160_ZPT0_500-nosub.root" \
    --Input5020 "$ANALYSIS_DIR/plots/jewelPP5020_ZPT0_500-nosub.root" \
    --TrackPTRange 0.5_15 \
    --OutputBase plots/pp_energy/jewel_pp_diag

echo "=== Diagnostic plots ==="
ls plots/pp_energy/jewel_pp_diag*.pdf
