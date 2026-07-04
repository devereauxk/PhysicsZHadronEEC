#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteResultPlot

PP_TAG="${OFFICIAL_TAG_PP}_scan"
PP_MC_TAG="${OFFICIAL_TAG_PP}"
PP_SYSTEMATICS_TAG="${OFFICIAL_TAG_PP}_scan"
PPB_TAG="${OFFICIAL_TAG_PPB}_scan"
PPB_MC_TAG="${OFFICIAL_TAG_PPB}"
PPB_SYSTEMATICS_TAG="${OFFICIAL_TAG_PPB}_scan"
INCLUDE_MC="${INCLUDE_MC:-false}"

PANEL_MODE="${PANEL_MODE:-0}"

PANEL_IDX=0
for TRKPT in 0.5_2 2_4 4_15; do
    if [[ "$PANEL_MODE" == "true" ]]; then
        PANEL_IDX=$((PANEL_IDX + 1))
        THIS_PANEL=$PANEL_IDX
    else
        THIS_PANEL=0
    fi
    echo "=== Running trkPT $TRKPT (panelMode=$THIS_PANEL) ==="
    ./ExecuteResultPlot \
        --zPtRange 0_500 --trkPtRange "$TRKPT" \
        --pPbtag "$PPB_TAG" \
        --pPbMCTag "$PPB_MC_TAG" \
        --pPbSystematicsTag "$PPB_SYSTEMATICS_TAG" \
        --pptag "$PP_TAG" \
        --ppMCTag "$PP_MC_TAG" \
        --ppSystematicsTag "$PP_SYSTEMATICS_TAG" \
        --doCombine true --includeMC "$INCLUDE_MC" \
        --UseSystematics true \
        --panelMode "$THIS_PANEL" \
        --outputBase plots/scan \
        --BaseDir "${OFFICIAL_RESULT_DIR}"
done
