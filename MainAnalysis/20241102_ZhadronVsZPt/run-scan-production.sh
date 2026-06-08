#!/bin/bash
# Orchestrator for signed track-pT scan production.
# Three concurrent streams at 30 threads each = 90 total (hard cap).
# Each stream runs nominal then systematics sequentially on its own skim.
#
# Usage: bash run-scan-production.sh

set -euo pipefail

export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=30

cd "$(dirname "$0")"

echo "=== Starting scan production at $(date) ==="
echo "3 streams × 30 threads = 90 total"

# Stream 1: pPb nominal → pPb systematics (30 threads, reads pPb skim)
(
    echo "[Stream 1] pPb nominal + systematics"
    ./central-signed-scan.sh 0 1 0
    ./systematics-signed-scan.sh 0 1 0
    echo "[Stream 1] pPb DONE at $(date)"
) > /tmp/kdeverea/scan_pPb.log 2>&1 &
PID_PPB=$!

# Stream 2: PbP nominal → PbP systematics (30 threads, reads PbP skim)
(
    echo "[Stream 2] PbP nominal + systematics"
    ./central-signed-scan.sh 0 0 1
    ./systematics-signed-scan.sh 0 0 1
    echo "[Stream 2] PbP DONE at $(date)"
) > /tmp/kdeverea/scan_PbP.log 2>&1 &
PID_PBP=$!

# Stream 3: pp nominal → pp systematics (30 threads, reads pp skim sequentially)
(
    echo "[Stream 3] pp nominal + systematics"
    ./central-signed-scan.sh 1 0 0
    ./systematics-signed-scan.sh 1 0 0
    echo "[Stream 3] pp DONE at $(date)"
) > /tmp/kdeverea/scan_pp.log 2>&1 &
PID_PP=$!

echo "PIDs: pPb=$PID_PPB PbP=$PID_PBP pp=$PID_PP"

FAIL=0
wait $PID_PPB && echo "pPb OK"  || { echo "pPb FAILED"; FAIL=1; }
wait $PID_PBP && echo "PbP OK"  || { echo "PbP FAILED"; FAIL=1; }
wait $PID_PP  && echo "pp OK"   || { echo "pp FAILED"; FAIL=1; }

if [ "$FAIL" -eq 0 ]; then
    echo "=== All scan production completed successfully at $(date) ==="
else
    echo "=== Some jobs FAILED — check logs in /tmp/kdeverea/scan_*.log ==="
    exit 1
fi
