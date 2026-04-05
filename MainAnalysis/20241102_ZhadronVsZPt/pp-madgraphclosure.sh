#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$SCRIPT_DIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

CONFIG_FILE=${CONFIG_FILE:-$SCRIPT_DIR/config-pp-madgraphclosure.sh}
export CONFIG_FILE

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
   source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-1}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

TAG=${TAG:-madgraphClosure_nmix10}
NMIX=${NMIX:-10}

PP_OFFICIAL_MCGENINPUT=${PP_OFFICIAL_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}
PP_PRIVATE_MCGENINPUT=${PP_PRIVATE_MCGENINPUT:-/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20260403_PythiaMadgraph/output/ZMuMu_5020_validation.root}

COMMON_ARGS=(
   --IsPP true
   --IsGenZ true
   --IsData false
   --UseEventWeight true
   --UseZWeight false
   --UseTrackWeight true
   --UseResidualWeight false
   --UseVZWeight false
   --yBoost 0
   --nMix "${NMIX}"
)

./system-analysis.sh "pythiaMC_Gen_nominal_${TAG}" \
   --Input "${PP_OFFICIAL_MCGENINPUT}" \
   --MixFile "${PP_OFFICIAL_MCGENINPUT}" \
   "${COMMON_ARGS[@]}"

./system-analysis.sh "madgraphMC_Gen_nominal_${TAG}" \
   --Input "${PP_PRIVATE_MCGENINPUT}" \
   --MixFile "${PP_PRIVATE_MCGENINPUT}" \
   "${COMMON_ARGS[@]}"
