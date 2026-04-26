#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
export VARIANT_TAG="${VARIANT_TAG:-IsPURejectTrue}"
export VZWeightFile_PPb="${VZWeightFile_PPb_IsPURejectTrue}"
export VZWeightFile_PbP="${VZWeightFile_PbP_IsPURejectTrue}"
./pPb-DY-analysis.sh 0 500 "${NAME_TAG:-20260331_ZCorrection_V7}" --IsPUReject true "$@"
