#!/bin/bash

set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

ZPT_RANGE="${ZPT_RANGE:-0_500}"
TRKPT_RANGE="${TRKPT_RANGE:-0.5_15}"
PLOT_DIR="${PLOT_DIR:-plots/central_sb}"

make ExecuteCentralSBPlot
./ExecuteCentralSBPlot \
    --zPtRange "${ZPT_RANGE}" \
    --trkPtRange "${TRKPT_RANGE}" \
    --tag "${OFFICIAL_TAG_PPB}" \
    --PlotDir "${PLOT_DIR}" \
    --InputDir "${OFFICIAL_RESULT_DIR}"

echo "Pre-subtraction pPb/Pbp data overlays saved to ${PLOT_DIR}"
