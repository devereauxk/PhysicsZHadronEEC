#!/bin/bash
set -euo pipefail

JEWEL_DIR=/home/kdeverea/Jewel/jewel-2.4.0
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
PARAMS=$WORK_DIR/params/pPb

export LD_LIBRARY_PATH=/home/kdeverea/Jewel/lhapdf/lib:${LD_LIBRARY_PATH:-}
export LHAPATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current

mkdir -p "$WORK_DIR/eventfiles/pPb" "$WORK_DIR/logs/pPb"

NJOBS=${NJOBS:-20}

echo "=== Generating pPb 8.16 TeV across Ncoll bins ==="
echo "=== Running $NJOBS bins in parallel ==="

total=$(ls "$PARAMS"/params.2D-pPb-*.dat | wc -l)
skipped=0
launched=0
job_count=0

for f in "$PARAMS"/params.2D-pPb-*.dat; do
    ncoll=$(basename "$f" | sed 's/params.2D-pPb-//;s/.dat//')
    outfile="$WORK_DIR/eventfiles/pPb/2D-pPb-${ncoll}.hepmc"

    if [ -f "$outfile" ] && [ -s "$outfile" ]; then
        skipped=$((skipped + 1))
        continue
    fi

    "$JEWEL_DIR/jewel-2.4.0-2D" "$f" &
    launched=$((launched + 1))
    job_count=$((job_count + 1))

    if [ "$job_count" -ge "$NJOBS" ]; then
        wait
        echo "  Batch done ($launched/$total launched, $skipped skipped)"
        job_count=0
    fi
done
wait
echo "=== Generation complete: $launched launched, $skipped skipped of $total total ==="

echo "=== Merging all pPb HepMC files ==="
cat "$WORK_DIR"/eventfiles/pPb/2D-pPb-*.hepmc > "$WORK_DIR/eventfiles/pPb/merged_pPb_8160.hepmc"

echo "=== Converting merged HepMC to ROOT skim ==="
"$WORK_DIR/ExecuteConvertHepMC" \
    --Input "$WORK_DIR/eventfiles/pPb/merged_pPb_8160.hepmc" \
    --Output "$WORK_DIR/output/jewel_pPb_8160.root"

echo "=== Done ==="
