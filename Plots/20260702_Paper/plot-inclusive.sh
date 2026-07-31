#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteInclusive

PP_TAG="${PP_TAG:-$OFFICIAL_TAG_PP}"
PP_SYSTEMATICS_TAG="${PP_SYSTEMATICS_TAG:-$PP_TAG}"
PPB_TAG="${PPB_TAG:-$OFFICIAL_TAG_PPB}"
PPB_MC_TAG="${PPB_MC_TAG:-$PPB_TAG}"
PPB_SYSTEMATICS_TAG="${PPB_SYSTEMATICS_TAG:-$PPB_TAG}"
PLOT_OUTPUT_BASE="${PLOT_OUTPUT_BASE:-plots/inclusive}"
RUN_COMBINED="${RUN_COMBINED:-1}"
RUN_SINGLE="${RUN_SINGLE:-1}"
PLOT_INCLUDE_MC="${PLOT_INCLUDE_MC:-true}"
USE_SYSTEMATICS="${USE_SYSTEMATICS:-true}"
INCLUDE_JEWEL="${INCLUDE_JEWEL:-0}"

run_one() {
    local ZPT=$1
    local TRKPT=$2

    local JEWEL_ARGS=""
    if [ "$INCLUDE_JEWEL" = "1" ]; then
        JEWEL_ARGS="--ppJewelFile ${OFFICIAL_RESULT_DIR}/jewelPP8160signed_ZPT${ZPT}"
    fi

    if [ "$RUN_COMBINED" != "0" ]; then
        ./ExecuteInclusive --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_MC_TAG" --pPbSystematicsTag "$PPB_SYSTEMATICS_TAG" --pptag "$PP_TAG" --ppSystematicsTag "$PP_SYSTEMATICS_TAG" --doCombine true --includeMC "$PLOT_INCLUDE_MC" --UseSystematics "$USE_SYSTEMATICS" --outputBase "$PLOT_OUTPUT_BASE" --BaseDir "${OFFICIAL_RESULT_DIR}" $JEWEL_ARGS
    fi
    if [ "$RUN_SINGLE" != "0" ]; then
        ./ExecuteInclusive --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_MC_TAG" --pPbSystematicsTag "$PPB_SYSTEMATICS_TAG" --pptag "$PP_TAG" --ppSystematicsTag "$PP_SYSTEMATICS_TAG" --doCombine false --collisionType pPb --includeMC "$PLOT_INCLUDE_MC" --UseSystematics "$USE_SYSTEMATICS" --outputBase "$PLOT_OUTPUT_BASE" --BaseDir "${OFFICIAL_RESULT_DIR}" $JEWEL_ARGS
        ./ExecuteInclusive --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_MC_TAG" --pPbSystematicsTag "$PPB_SYSTEMATICS_TAG" --pptag "$PP_TAG" --ppSystematicsTag "$PP_SYSTEMATICS_TAG" --doCombine false --collisionType PbP --includeMC "$PLOT_INCLUDE_MC" --UseSystematics "$USE_SYSTEMATICS" --outputBase "$PLOT_OUTPUT_BASE" --BaseDir "${OFFICIAL_RESULT_DIR}" $JEWEL_ARGS
    fi
}

run_one 0_500 0.5_15

exit
