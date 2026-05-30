#!/bin/bash
set -euo pipefail

JEWEL_DIR=/home/kdeverea/Jewel/jewel-2.4.0
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

export LD_LIBRARY_PATH=/home/kdeverea/Jewel/lhapdf/lib:${LD_LIBRARY_PATH:-}
export LHAPATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current

NJOBS=${NJOBS:-20}
total=$(ls "$WORK_DIR/params/lo/"*.dat "$WORK_DIR/params/mi/"*.dat "$WORK_DIR/params/hi/"*.dat | wc -l)
skipped=0; launched=0; job_count=0

echo "=== Generating pPb 8.16 TeV sliced (3 pt-hat slices × 100 Ncoll bins) ==="
echo "=== Running $NJOBS jobs in parallel, $total total ==="

for SLICE in lo mi hi; do
for f in "$WORK_DIR/params/${SLICE}"/params.2D-pPb-*.dat; do
    ncoll=$(basename "$f" | sed 's/params.2D-pPb-//;s/.dat//')
    outfile="$WORK_DIR/eventfiles/${SLICE}/2D-pPb-${ncoll}.hepmc"

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
done
wait
echo "=== Generation complete: $launched launched, $skipped skipped of $total total ==="

echo "=== Merging all sliced pPb HepMC files ==="
cat "$WORK_DIR"/eventfiles/lo/2D-pPb-*.hepmc \
    "$WORK_DIR"/eventfiles/mi/2D-pPb-*.hepmc \
    "$WORK_DIR"/eventfiles/hi/2D-pPb-*.hepmc \
    > "$WORK_DIR/eventfiles/merged_pPb_8160_sliced.hepmc"

echo "=== Converting merged HepMC to ROOT skim ==="
"$WORK_DIR/ExecuteConvertHepMC" \
    --Input  "$WORK_DIR/eventfiles/merged_pPb_8160_sliced.hepmc" \
    --Output "$WORK_DIR/output/jewel_pPb_8160_sliced.root"

echo "=== Done: output/jewel_pPb_8160_sliced.root ==="
