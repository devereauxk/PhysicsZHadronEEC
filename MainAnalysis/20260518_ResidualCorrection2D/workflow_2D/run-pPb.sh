#!/bin/bash

set -euo pipefail

WORKINGDIR=$(pwd)
if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    cd ..
    ./clean.sh
    cd "$WORKINGDIR"
    sleep 1
fi

name="${NAME_TAG:-20260519_ZV9_trkV28_TrackResidualCorrection}"
echo "[run-pPb] NAME_TAG=${name} VARIANT_TAG=${VARIANT_TAG:-Nominal}"
./pPb-DY-analysis.sh 0 10 "$name" "$@"
./pPb-DY-analysis.sh 10 20 "$name" "$@"
./pPb-DY-analysis.sh 20 40 "$name" "$@"
./pPb-DY-analysis.sh 40 500 "$name" "$@"
