#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh

PLOT_DIR="${PLOT_DIR:-plots/extremal_runs}"
PPB_MAX_DIR="${PPB_MAX_DIR:-output/pPb_run286327}"
PPB_MIN_DIR="${PPB_MIN_DIR:-output/pPb_run286441}"
PBP_MAX_DIR="${PBP_MAX_DIR:-output/PbP_run285718}"
PBP_MIN_DIR="${PBP_MIN_DIR:-output/PbP_run285537}"

make ExecutePlotExtremalRuns
./ExecutePlotExtremalRuns \
    --PlotDir "${PLOT_DIR}" \
    --PPbMaxDir "${PPB_MAX_DIR}" \
    --PPbMinDir "${PPB_MIN_DIR}" \
    --PbPMaxDir "${PBP_MAX_DIR}" \
    --PbPMinDir "${PBP_MIN_DIR}"

echo "Extremal-run plots saved to ${PLOT_DIR}"
