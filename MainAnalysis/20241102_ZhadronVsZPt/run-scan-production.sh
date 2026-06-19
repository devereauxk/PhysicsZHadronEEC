#!/bin/bash
# Orchestrator for signed track-pT scan production.
# Three concurrent streams at 30 threads each = 90 total (hard cap).
# Each stream runs nominal then systematics sequentially on its own skim.
# When pPb or PbP finishes before pp, pp EEPrivate runs on the copy skim
# in the freed slot (still ≤90 total).
#
# Usage: bash run-scan-production.sh

set -euo pipefail

export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=30

PP_COPY="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.3/PPData_Reco-copy.root"

cd "$(dirname "$0")"

echo "=== Starting scan production at $(date) ==="
echo "3 streams × 30 threads = 90 total"

# Stream 1: pPb nominal → pPb systematics (30 threads, reads pPb skim)
(
    echo "[Stream 1] pPb nominal + systematics"
    ./central-scan.sh 0 1 0
    ./systematics-scan.sh 0 1 0
    echo "[Stream 1] pPb DONE at $(date)"
) > /tmp/kdeverea/scan_pPb.log 2>&1 &
PID_PPB=$!

# Stream 2: PbP nominal → PbP systematics (30 threads, reads PbP skim)
(
    echo "[Stream 2] PbP nominal + systematics"
    ./central-scan.sh 0 0 1
    ./systematics-scan.sh 0 0 1
    echo "[Stream 2] PbP DONE at $(date)"
) > /tmp/kdeverea/scan_PbP.log 2>&1 &
PID_PBP=$!

# Stream 3: pp nominal → pp systematics except EEPrivate (30 threads, reads pp skim)
(
    echo "[Stream 3] pp nominal + systematics (Loose, Tight, PU)"
    ./central-scan.sh 1 0 0
    DOPP_SKIP_EE=1 ./systematics-scan.sh 1 0 0
    echo "[Stream 3] pp main DONE at $(date)"
) > /tmp/kdeverea/scan_pp.log 2>&1 &
PID_PP=$!

echo "PIDs: pPb=$PID_PPB PbP=$PID_PBP pp=$PID_PP"

# Wait for all 3 streams, then run pp EEPrivate on the freed slot.
# EEPrivate runs after everything else to avoid exceeding 90 threads.
FAIL=0
wait $PID_PPB && echo "pPb OK"  || { echo "pPb FAILED"; FAIL=1; }
wait $PID_PBP && echo "PbP OK"  || { echo "PbP FAILED"; FAIL=1; }
wait $PID_PP  && echo "pp OK"   || { echo "pp FAILED"; FAIL=1; }

# pp EEPrivate on copy skim (30 threads, single run)
echo "[Stream 4] pp EEPrivate on copy skim at $(date)"
PP_INPUT_OVERRIDE="$PP_COPY" DOPP_ONLY_EE=1 ./systematics-scan.sh 1 0 0 \
    > /tmp/kdeverea/scan_pp_ee.log 2>&1 \
    && echo "pp EE OK" || { echo "pp EE FAILED"; FAIL=1; }

if [ "$FAIL" -eq 0 ]; then
    echo "=== All scan production completed successfully at $(date) ==="
else
    echo "=== Some jobs FAILED — check logs in /tmp/kdeverea/scan_*.log ==="
    exit 1
fi
