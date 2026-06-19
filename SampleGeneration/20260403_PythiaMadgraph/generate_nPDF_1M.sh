#!/bin/bash
# Generate 1M event nPDF study samples at 8.16 TeV
# CT18ANLO (free proton), EPPS21 (nuclear), NNPDF31 (baseline)
# MG5 steps run sequentially (shared work/ dir), shower+convert run in parallel
set -euo pipefail

cd "$(dirname "$0")"
source env.sh

OUTDIR=/data00/kdeverea/pythiaMG
PROC_BASE=/data00/kdeverea/pythiaMG/processes
LOG=output/nPDF_1M_generation.log
mkdir -p "$OUTDIR" "$PROC_BASE" output

log() { echo "[$(date '+%H:%M:%S')] $*" | tee -a "$LOG"; }

# ============================================================
# Phase 1: MG5 LHE generation (sequential — shared work/ dir)
# ============================================================

log "=== Phase 1: MG5 LHE generation (sequential) ==="

log "EPPS21: MadGraph LHE generation..."
MG5AMC_PYTHON=/usr/bin/python3.11 ./generate_madgraph.sh \
    --Energy 8160 --Events 1000000 --Seed 93001 \
    --LHAID 904400 --PDFLabel lhapdf \
    --ProcessDir "$PROC_BASE/ZMuMu_8160_epps21_1M" \
    --OutputLHE "$OUTDIR/ZMuMu_8160_epps21_1M.lhe" 2>&1 | tail -3 | tee -a "$LOG"

log "CT18ANLO: MadGraph LHE generation..."
MG5AMC_PYTHON=/usr/bin/python3.11 ./generate_madgraph.sh \
    --Energy 8160 --Events 1000000 --Seed 94001 \
    --LHAID 14600 --PDFLabel lhapdf \
    --ProcessDir "$PROC_BASE/ZMuMu_8160_ct18anlo_1M" \
    --OutputLHE "$OUTDIR/ZMuMu_8160_ct18anlo_1M.lhe" 2>&1 | tail -3 | tee -a "$LOG"

log "NNPDF31: MadGraph LHE generation..."
MG5AMC_PYTHON=/usr/bin/python3.11 ./generate_madgraph.sh \
    --Energy 8160 --Events 1000000 --Seed 95001 \
    --LHAID 303600 --PDFLabel lhapdf \
    --ProcessDir "$PROC_BASE/ZMuMu_8160_nnpdf31_1M" \
    --OutputLHE "$OUTDIR/ZMuMu_8160_nnpdf31_1M.lhe" 2>&1 | tail -3 | tee -a "$LOG"

log "=== Phase 1 complete: all 3 LHE files ready ==="

# ============================================================
# Phase 2: Pythia8 shower + HepMC→ROOT (parallel)
# ============================================================

log "=== Phase 2: Shower + conversion (parallel) ==="

shower_and_convert() {
    local label="$1"
    local config="$2"
    local seed="$3"

    log "$label: Pythia8 shower..."
    ./ExecuteShowerLHE \
        --Input "$OUTDIR/ZMuMu_8160_${label}_1M.lhe" \
        --Output "$OUTDIR/ZMuMu_8160_${label}_1M.hepmc" \
        --Config "$config" --Events 1000000 --Seed "$seed" 2>&1 | tail -3 | tee -a "$LOG"

    log "$label: HepMC → ROOT..."
    ./ExecuteConvertHepMC3 \
        --Input "$OUTDIR/ZMuMu_8160_${label}_1M.hepmc" \
        --Output "$OUTDIR/ZMuMu_8160_${label}_1M.root" \
        --MirrorGenToReco true 2>&1 | tail -3 | tee -a "$LOG"

    log "$label: done. Cleaning intermediates..."
    rm -f "$OUTDIR/ZMuMu_8160_${label}_1M.lhe" "$OUTDIR/ZMuMu_8160_${label}_1M.hepmc"
}

shower_and_convert epps21   CP5_CT18ANLO.cmnd 93002 &
PID_EPPS=$!
shower_and_convert ct18anlo CP5_CT18ANLO.cmnd 94002 &
PID_CT18=$!
shower_and_convert nnpdf31  CP5.cmnd          95002 &
PID_NNPDF=$!

log "Waiting for all 3 parallel jobs (PIDs: $PID_EPPS $PID_CT18 $PID_NNPDF)..."

FAIL=0
wait $PID_EPPS  || { log "ERROR: EPPS21 failed"; FAIL=1; }
wait $PID_CT18  || { log "ERROR: CT18ANLO failed"; FAIL=1; }
wait $PID_NNPDF || { log "ERROR: NNPDF31 failed"; FAIL=1; }

if [[ $FAIL -ne 0 ]]; then
    log "=== One or more samples FAILED ==="
    exit 1
fi

log "=== All 3 samples generated ==="
ls -lh "$OUTDIR"/ZMuMu_8160_*_1M.root | tee -a "$LOG"
