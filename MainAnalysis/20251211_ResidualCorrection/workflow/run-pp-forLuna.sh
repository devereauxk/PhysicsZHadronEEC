#!/bin/bash

set -euo pipefail

WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd "$WORKINGDIR"
sleep 1

name="20260413_TrackResidualCorrection_V2_forLuna"
./pythia-analysis-forLuna.sh 0 10 "$name"
./pythia-analysis-forLuna.sh 10 20 "$name"
./pythia-analysis-forLuna.sh 20 40 "$name"
./pythia-analysis-forLuna.sh 40 500 "$name"
