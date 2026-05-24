#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
export VZ_WEIGHT_FILE_PPB="${VZWeightFile_PPb_IsPURejectTrue}"
export VZ_WEIGHT_FILE_PBP="${VZWeightFile_PbP_IsPURejectTrue}"
export Z_WEIGHT_FILE_PPB="${ZWeightFile_PPb_IsPURejectTrue}"
export Z_WEIGHT_FILE_PBP="${ZWeightFile_PbP_IsPURejectTrue}"
export VARIANT_TAG="${VARIANT_TAG:-IsPURejectTrue}"
cd "$SCRIPT_DIR"
./run-pPb.sh --IsPUReject true "$@"
