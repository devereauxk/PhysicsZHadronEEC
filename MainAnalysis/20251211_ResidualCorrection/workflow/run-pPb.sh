#!/bin/bash

set -euo pipefail

WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd "$WORKINGDIR"
sleep 1

name="${NAME_TAG:-20260331_ZV7_trkV25_TrackResidualCorrection}"
echo "[run-pPb] NAME_TAG=${name} VARIANT_TAG=${VARIANT_TAG:-Nominal}"
./pPb-DY-analysis.sh 0 10 "$name" "$@"
./pPb-DY-analysis.sh 10 20 "$name" "$@"
./pPb-DY-analysis.sh 20 40 "$name" "$@"
./pPb-DY-analysis.sh 40 500 "$name" "$@"
