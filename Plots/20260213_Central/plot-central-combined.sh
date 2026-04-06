#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

#source ./clean.sh
make ExecuteCentralCombinedPlot


PP_TAG="$OFFICIAL_TAG_PP"
PPB_TAG="$OFFICIAL_TAG_PPB"
PLOT_OUTPUT_BASE="${PLOT_OUTPUT_BASE:-plots/central_combined}"
RUN_COMBINED="${RUN_COMBINED:-1}"
RUN_SINGLE="${RUN_SINGLE:-1}"
PLOT_INCLUDE_MC="${PLOT_INCLUDE_MC:-true}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    if [ "$RUN_COMBINED" != "0" ]; then
        ./ExecuteCentralCombinedPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pptag "$PP_TAG" --doCombine true --includeMC "$PLOT_INCLUDE_MC" --outputBase "$PLOT_OUTPUT_BASE"
    fi
    if [ "$RUN_SINGLE" != "0" ]; then
        ./ExecuteCentralCombinedPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pptag "$PP_TAG" --doCombine false --collisionType pPb --includeMC "$PLOT_INCLUDE_MC" --outputBase "$PLOT_OUTPUT_BASE"
        ./ExecuteCentralCombinedPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pptag "$PP_TAG" --doCombine false --collisionType PbP --includeMC "$PLOT_INCLUDE_MC" --outputBase "$PLOT_OUTPUT_BASE"
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
    run_one 5_30 0.5_4
    run_one 5_30 4_500
    run_one 30_500 0.5_4
    run_one 30_500 4_500
    run_one 5_500 0.5_500
fi

exit
