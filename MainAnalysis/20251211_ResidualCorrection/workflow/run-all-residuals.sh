#!/bin/bash
set -euo pipefail

# Full residual recalculation pipeline — 20260412 campaign
# All runs must be serial (shared output dir)

LOGDIR="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20251211_ResidualCorrection/workflow/logs"
mkdir -p "$LOGDIR"

cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20251211_ResidualCorrection/workflow

export NTHREAD=25

echo "=== [$(date)] Starting full residual recalculation ==="

# 1. pp nominal
echo "=== [$(date)] Step 1/5: pp nominal ==="
./run-pp.sh 2>&1 | tee "$LOGDIR/01_pp_nominal.log"
echo "=== [$(date)] Step 1/5 DONE ==="

# 2. pp systematics (Loose, Tight, IsMuTaggedFalse, TrackCorrection, PU)
echo "=== [$(date)] Step 2/5: pp systematics ==="
./run-pp-systematics.sh 2>&1 | tee "$LOGDIR/02_pp_systematics.log"
echo "=== [$(date)] Step 2/5 DONE ==="

# 3. pPb+PbP nominal
echo "=== [$(date)] Step 3/5: pPb+PbP nominal ==="
./run-pPb.sh 2>&1 | tee "$LOGDIR/03_pPb_nominal.log"
echo "=== [$(date)] Step 3/5 DONE ==="

# 4. pPb+PbP systematics
echo "=== [$(date)] Step 4/5: pPb+PbP systematics ==="
./run-pPb-systematics.sh 2>&1 | tee "$LOGDIR/04_pPb_systematics.log"
echo "=== [$(date)] Step 4/5 DONE ==="

# 5. pPb+PbP PU
echo "=== [$(date)] Step 5/5: pPb+PbP PU ==="
./run-pPb-pu.sh 2>&1 | tee "$LOGDIR/05_pPb_pu.log"
echo "=== [$(date)] Step 5/5 DONE ==="

echo "=== [$(date)] ALL RESIDUAL RECALCULATIONS COMPLETE ==="
