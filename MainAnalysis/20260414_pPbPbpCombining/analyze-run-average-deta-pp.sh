#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh

TABLE_OUTPUT="${TABLE_OUTPUT:-output/run_average_abs_deta_pp.tsv}"
MARKDOWN_OUTPUT="${MARKDOWN_OUTPUT:-output/run_average_abs_deta_pp_consistency.md}"
PLOT_OUTPUT="${PLOT_OUTPUT:-plots/run_average_abs_deta_pp_by_run.pdf}"

./tabulate-run-average-deta-pp.sh "${TABLE_OUTPUT}"
python3 analyze_run_average_deta.py \
    --input "${TABLE_OUTPUT}" \
    --markdown-output "${MARKDOWN_OUTPUT}" \
    --plot-output "${PLOT_OUTPUT}" \
    --group "pp:pp data:pp only" \
    --shared-y-range-from output/run_average_abs_deta_no_zcorr.tsv \
    --plot-min-events-with-tracks 2

echo "PP run-by-run avg |DeltaEta| assessment written to ${MARKDOWN_OUTPUT}"
echo "PP run-by-run avg |DeltaEta| plot written to ${PLOT_OUTPUT}"
