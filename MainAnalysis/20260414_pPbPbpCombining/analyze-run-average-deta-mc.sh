#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh

TABLE_OUTPUT="${TABLE_OUTPUT:-output/run_average_abs_deta_mc_no_zcorr.tsv}"
MARKDOWN_OUTPUT="${MARKDOWN_OUTPUT:-output/run_average_abs_deta_mc_no_zcorr_consistency.md}"
PLOT_OUTPUT="${PLOT_OUTPUT:-plots/pPbPbp_run_average_abs_deta_mc_by_run.pdf}"

./tabulate-run-average-deta-mc.sh "${TABLE_OUTPUT}"
python3 analyze_run_average_deta.py \
    --input "${TABLE_OUTPUT}" \
    --markdown-output "${MARKDOWN_OUTPUT}" \
    --plot-output "${PLOT_OUTPUT}" \
    --ppb-label "pPb MC reco" \
    --pbp-label "Pbp MC reco" \
    --prefix-system-in-ticks

echo "MC run-by-run avg |DeltaEta| assessment written to ${MARKDOWN_OUTPUT}"
echo "MC run-by-run avg |DeltaEta| plot written to ${PLOT_OUTPUT}"
