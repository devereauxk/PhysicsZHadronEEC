#!/bin/bash
set -euo pipefail

JEWEL_DIR=/home/kdeverea/Jewel/jewel-2.4.0
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

export LD_LIBRARY_PATH=/home/kdeverea/Jewel/lhapdf/lib:${LD_LIBRARY_PATH:-}
export LHAPATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current

mkdir -p "$WORK_DIR/eventfiles" "$WORK_DIR/logs"

echo "=== Generating pp 8.16 TeV ==="
"$JEWEL_DIR/jewel-2.4.0-vac" "$WORK_DIR/params/pp-8160.dat"

echo "=== Generating pp 5.02 TeV ==="
"$JEWEL_DIR/jewel-2.4.0-vac" "$WORK_DIR/params/pp-5020.dat"

echo "=== Converting HepMC to ROOT skim ==="
"$WORK_DIR/ExecuteConvertHepMC" \
    --Input "$WORK_DIR/eventfiles/pp-8160.hepmc" \
    --Output "$WORK_DIR/output/jewel_pp_8160.root"

"$WORK_DIR/ExecuteConvertHepMC" \
    --Input "$WORK_DIR/eventfiles/pp-5020.hepmc" \
    --Output "$WORK_DIR/output/jewel_pp_5020.root"

echo "=== Done ==="
