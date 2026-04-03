#!/bin/bash

set -euo pipefail

WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd "$WORKINGDIR"
sleep 1

name="${NAME_TAG:-20260331_ZV7_trkV25_TrackResidualCorrection}"
echo "[run-pp] NAME_TAG=${name} VARIANT_TAG=${VARIANT_TAG:-Nominal}"
./pythia-analysis.sh 0 10 "$name" "$@"
./pythia-analysis.sh 10 20 "$name" "$@"
./pythia-analysis.sh 20 40 "$name" "$@"
./pythia-analysis.sh 40 500 "$name" "$@"
