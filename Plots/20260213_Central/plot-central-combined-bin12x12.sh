#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd /home/kdeverea/PhysicsZHadronEEC
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
cd "$THISDIR"
make ExecuteCentralCombinedPlot

PP_TAG="${PP_TAG_OVERRIDE:-${OFFICIAL_TAG_PP}_bin12x12_20260602}"
PPB_TAG="${PPB_TAG_OVERRIDE:-${OFFICIAL_TAG_PPB}_bin12x12_20260602}"
PLOT_OUTPUT_BASE="${PLOT_OUTPUT_BASE:-plots/central_combined_bin12x12}"
SCAN_CONFIG="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/config_note_bin12x12_scan.sh"
INCLUSIVE_CONFIG="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/config_note_bin12x12_inclusive.sh"

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
                --UseModified12x12 true \
                --outputBase "$PLOT_OUTPUT_BASE" \
                --BaseDir "${OFFICIAL_RESULT_DIR}"
        done
    done
done
