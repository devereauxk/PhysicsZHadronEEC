#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh

NTHREAD="${NTHREAD:-25}"
export SKIP_CLEAN=1

run_single() {
    local output_dir="$1"
    local ppb_extra="$2"
    local pbp_extra="$3"

    echo "=== Running ${output_dir} ==="
    OUTPUT_DIR="${output_dir}" \
    PPB_EXTRA_ARGS="${ppb_extra}" \
    PBP_EXTRA_ARGS="${pbp_extra}" \
    NTHREAD="${NTHREAD}" \
    ./runner.sh
}

run_single "output/pPb_run286327" "--MinRun 286327 --MaxRun 286328" ""
run_single "output/pPb_run286441" "--MinRun 286441 --MaxRun 286442" ""
run_single "output/PbP_run285718" "" "--MinRun 285718 --MaxRun 285719"
run_single "output/PbP_run285537" "" "--MinRun 285537 --MaxRun 285538"

echo "All extremal single-run productions complete."
