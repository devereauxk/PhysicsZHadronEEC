#!/bin/bash

set -euo pipefail

TAG=${TAG:-madgraphClosure_nmix10}
TRKPT=${TRKPT:-0.5_500}
CONFIG_FILE=${CONFIG_FILE:-/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/config-pp-madgraphclosure.sh}

source "${CONFIG_FILE}"

make ExecuteCompareProduction

run_one() {
   local ZPT=$1
   ./ExecuteCompareProduction --zPtRange "${ZPT}" --trkPtRange "${TRKPT}" --tag "${TAG}"
}

for ZPT in "${ZPT_RANGES[@]}"; do
   run_one "${ZPT}"
done
