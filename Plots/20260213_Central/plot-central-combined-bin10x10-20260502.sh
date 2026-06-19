#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh

make ExecuteCentralCombinedPlot

PP_TAG="${PP_TAG_OVERRIDE:-EEV5_ZV9_trkV27_nmix10_bin10x10shifted_20260506}"
PPB_TAG="${PPB_TAG_OVERRIDE:-ZV9_trkV27_nmix10_bin10x10shifted_20260506}"
PLOT_OUTPUT_BASE="${PLOT_OUTPUT_BASE:-plots/central_combined_bin10x10shifted_20260506}"
SCAN_CONFIG="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/config_note_bin10x10_scan_20260506.sh"
INCLUSIVE_CONFIG="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/config_note_bin10x10_inclusive_20260506.sh"

for config in "$SCAN_CONFIG" "$INCLUSIVE_CONFIG"; do
    source "$config"
    for ZPT in "${ZPT_RANGES[@]}"; do
        for TRKPT in "${PT_RANGES[@]}"; do
            ./ExecuteCentralCombinedPlot \
                --zPtRange "$ZPT" \
                --trkPtRange "$TRKPT" \
                --pPbtag "$PPB_TAG" \
                --pPbMCTag "$PPB_TAG" \
                --pPbSystematicsTag "$PPB_TAG" \
                --pptag "$PP_TAG" \
                --ppSystematicsTag "$PP_TAG" \
                --doCombine true \
                --includeMC true \
                --UseSystematics true \
                --UseShifted10x10 true \
                --outputBase "$PLOT_OUTPUT_BASE"
        done
    done
done
