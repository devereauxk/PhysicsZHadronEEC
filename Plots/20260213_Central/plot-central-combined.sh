#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

#source ./clean.sh
make ExecuteCentralCombinedPlot


PP_TAG="${PP_TAG:-$OFFICIAL_TAG_PP}"
PP_SYSTEMATICS_TAG="${PP_SYSTEMATICS_TAG:-$PP_TAG}"
PPB_TAG="${PPB_TAG:-$OFFICIAL_TAG_PPB}"
PPB_MC_TAG="${PPB_MC_TAG:-$PPB_TAG}"
PPB_SYSTEMATICS_TAG="${PPB_SYSTEMATICS_TAG:-$PPB_TAG}"
PLOT_OUTPUT_BASE="${PLOT_OUTPUT_BASE:-plots/central_combined}"
RUN_COMBINED="${RUN_COMBINED:-1}"
RUN_SINGLE="${RUN_SINGLE:-1}"
PLOT_INCLUDE_MC="${PLOT_INCLUDE_MC:-true}"
USE_SYSTEMATICS="${USE_SYSTEMATICS:-true}"
USE_MODIFIED_12x12="${USE_MODIFIED_12x12:-false}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    if [ "$RUN_COMBINED" != "0" ]; then
        ./ExecuteCentralCombinedPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_MC_TAG" --pPbSystematicsTag "$PPB_SYSTEMATICS_TAG" --pptag "$PP_TAG" --ppSystematicsTag "$PP_SYSTEMATICS_TAG" --doCombine true --includeMC "$PLOT_INCLUDE_MC" --UseSystematics "$USE_SYSTEMATICS" --UseModified12x12 "$USE_MODIFIED_12x12" --outputBase "$PLOT_OUTPUT_BASE" --BaseDir "${OFFICIAL_RESULT_DIR}"
    fi
    if [ "$RUN_SINGLE" != "0" ]; then
        ./ExecuteCentralCombinedPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_MC_TAG" --pPbSystematicsTag "$PPB_SYSTEMATICS_TAG" --pptag "$PP_TAG" --ppSystematicsTag "$PP_SYSTEMATICS_TAG" --doCombine false --collisionType pPb --includeMC "$PLOT_INCLUDE_MC" --UseSystematics "$USE_SYSTEMATICS" --UseModified12x12 "$USE_MODIFIED_12x12" --outputBase "$PLOT_OUTPUT_BASE" --BaseDir "${OFFICIAL_RESULT_DIR}"
        ./ExecuteCentralCombinedPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_MC_TAG" --pPbSystematicsTag "$PPB_SYSTEMATICS_TAG" --pptag "$PP_TAG" --ppSystematicsTag "$PP_SYSTEMATICS_TAG" --doCombine false --collisionType PbP --includeMC "$PLOT_INCLUDE_MC" --UseSystematics "$USE_SYSTEMATICS" --UseModified12x12 "$USE_MODIFIED_12x12" --outputBase "$PLOT_OUTPUT_BASE" --BaseDir "${OFFICIAL_RESULT_DIR}"
    fi
}

if [ -n "${CONFIG_FILE:-}" ]; then
    source "$CONFIG_FILE"
    for ZPT in "${ZPT_RANGES[@]}"; do
        for TRKPT in "${PT_RANGES[@]}"; do
            run_one "$ZPT" "$TRKPT"
        done
    done
else
    run_one 0_30 0.5_2
    run_one 0_30 2_4
    run_one 0_30 4_15
    run_one 30_500 0.5_2
    run_one 30_500 2_4
    run_one 30_500 4_15
    run_one 0_500 0.5_15
fi

exit
