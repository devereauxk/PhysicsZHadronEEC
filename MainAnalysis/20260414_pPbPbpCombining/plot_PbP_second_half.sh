#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
RUN_THRESHOLD="${RUN_THRESHOLD:-285549}"
INPUT_DIR="${INPUT_DIR:-output/PbP_runge${RUN_THRESHOLD}}"
PLOT_DIR="${PLOT_DIR:-plots/PbP_runge${RUN_THRESHOLD}}"
make ExecutePlotComparison
./ExecutePlotComparison --InputDir "${INPUT_DIR}" --PlotDir "${PLOT_DIR}"
echo "Plots saved to ${PLOT_DIR}/"
