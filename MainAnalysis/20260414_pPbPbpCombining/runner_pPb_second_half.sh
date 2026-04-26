#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

RUN_THRESHOLD="${RUN_THRESHOLD:-286302}"
export OUTPUT_DIR="${OUTPUT_DIR:-output/pPb_runge${RUN_THRESHOLD}}"
export MIN_ZPT="${MIN_ZPT:-0}"
export MAX_ZPT="${MAX_ZPT:-500}"
export MIN_TRACK_PT="${MIN_TRACK_PT:-0.5}"
export MAX_TRACK_PT="${MAX_TRACK_PT:-15}"
export PPB_EXTRA_ARGS="${PPB_EXTRA_ARGS:---MinRun ${RUN_THRESHOLD}}"
export PBP_EXTRA_ARGS="${PBP_EXTRA_ARGS:-}"
export NTHREAD="${NTHREAD:-25}"

./runner.sh
