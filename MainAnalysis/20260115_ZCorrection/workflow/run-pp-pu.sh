#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
export VARIANT_TAG="${VARIANT_TAG:-IsPURejectTrue}"
export VZWeightFile_PP="${VZWeightFile_PP_IsPURejectTrue}"
exec "$(cd "$(dirname "$0")" && pwd)/pythia-analysis.sh" 0 500 "${NAME_TAG:-20260401_ZCorrection_V7}" --IsPUReject true "$@"
