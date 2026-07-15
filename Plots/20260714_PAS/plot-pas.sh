#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteCentralCombinedPlot

PP_TAG="$OFFICIAL_TAG_PP"
PPB_TAG="$OFFICIAL_TAG_PPB"
PLOT_OUTPUT_BASE="plots/central_combined"
PLOT_OUTPUT_BASE_SCAN="plots/central_combined_scan"
PLOT_INCLUDE_MC="${PLOT_INCLUDE_MC:-false}"
USE_SYSTEMATICS="${USE_SYSTEMATICS:-true}"
USE_MODIFIED_12x12="${USE_MODIFIED_12x12:-true}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    local OUTPUT_BASE=$3
    local TAG_SUFFIX=${4:-}
    local PP_TAG_USE="${PP_TAG}${TAG_SUFFIX}"
    local PPB_TAG_USE="${PPB_TAG}${TAG_SUFFIX}"

    ./ExecuteCentralCombinedPlot \
        --zPtRange "$ZPT" --trkPtRange "$TRKPT" \
        --pPbtag "$PPB_TAG_USE" --pPbMCTag "$PPB_TAG" \
        --pPbSystematicsTag "${PPB_TAG}${TAG_SUFFIX}" \
        --pptag "$PP_TAG_USE" --ppSystematicsTag "${PP_TAG}${TAG_SUFFIX}" \
        --doCombine true --includeMC "$PLOT_INCLUDE_MC" \
        --UseSystematics "$USE_SYSTEMATICS" \
        --UseModified12x12 "$USE_MODIFIED_12x12" \
        --outputBase "$OUTPUT_BASE" \
        --BaseDir "${OFFICIAL_RESULT_DIR}"
}

echo "=== Inclusive ==="
run_one 0_500 0.5_15 "$PLOT_OUTPUT_BASE"

echo "=== Scan bins ==="
for ZPT in 0_500 0_30 30_500; do
    for TRKPT in 0.5_2 2_4 4_15; do
        run_one "$ZPT" "$TRKPT" "$PLOT_OUTPUT_BASE_SCAN" "_scan"
    done
done

echo "Done. Outputs in $THISDIR/plots/"
