#!/bin/bash

set -euo pipefail

TAG=${TAG:-madgraphClosure_nmix10}

make ExecuteClosureTest

run_one() {
   local ZPT=$1
   local TRKPT=$2
   ./ExecuteClosureTest --zPtRange "$ZPT" --trkPtRange "$TRKPT" --tag "$TAG"
}

run_one "0_10" "0.5_500"
run_one "10_20" "0.5_500"
run_one "20_40" "0.5_500"
run_one "40_500" "0.5_500"
