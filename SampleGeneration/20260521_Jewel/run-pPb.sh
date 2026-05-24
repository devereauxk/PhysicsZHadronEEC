#!/bin/bash
set -euo pipefail

JEWEL_DIR=/home/kdeverea/Jewel/jewel-2.4.0
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
PARAMS=$WORK_DIR/params/pPb

export LD_LIBRARY_PATH=/home/kdeverea/Jewel/lhapdf/lib:${LD_LIBRARY_PATH:-}
export LHAPATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current

mkdir -p "$WORK_DIR/eventfiles/pPb" "$WORK_DIR/logs/pPb"

NJOBS=${NJOBS:-4}
SKIP_GENERATE=${SKIP_GENERATE:-0}

if [ "$SKIP_GENERATE" -eq 0 ]; then
    echo "=== Generating pPb 8.16 TeV across $(ls "$PARAMS"/params.*.dat | wc -l) Ncoll bins ==="
    echo "=== Running $NJOBS bins in parallel ==="

    job_count=0
    total=$(ls "$PARAMS"/params.*.dat | wc -l)
    done_count=0

    for f in "$PARAMS"/params.2D-pPb-*.dat; do
        ncoll=$(basename "$f" | sed 's/params.2D-pPb-//;s/.dat//')
        outfile="$WORK_DIR/eventfiles/pPb/2D-pPb-${ncoll}.hepmc"

        if [ -f "$outfile" ] && [ -s "$outfile" ]; then
            done_count=$((done_count + 1))
            continue
        fi

        "$JEWEL_DIR/jewel-2.4.0-2D" "$f" &
        job_count=$((job_count + 1))

        if [ "$job_count" -ge "$NJOBS" ]; then
            wait
            job_count=0
        fi
    done
    wait
    echo "=== Generation complete (skipped $done_count already-done bins) ==="
fi

echo "=== Converting all pPb HepMC to single ROOT skim ==="
# Build space-separated list of all HepMC files
HEPMC_FILES=""
for f in "$WORK_DIR"/eventfiles/pPb/2D-pPb-*.hepmc; do
    if [ -f "$f" ] && [ -s "$f" ]; then
        HEPMC_FILES="$HEPMC_FILES $f"
    fi
done

"$WORK_DIR/ExecuteConvertHepMC" \
    --Input "$WORK_DIR/eventfiles/pPb/2D-pPb-*.hepmc" \
    --Output "$WORK_DIR/output/jewel_pPb_8160.root"

echo "=== Done ==="
