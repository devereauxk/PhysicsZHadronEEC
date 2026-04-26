#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
export ZWeightFile_PPb="${ZWeightFile_PPb_IsPURejectTrue}"
export ZWeightFile_PbP="${ZWeightFile_PbP_IsPURejectTrue}"
export VZWeightFile_PPb="${VZWeightFile_PPb_IsPURejectTrue}"
export VZWeightFile_PbP="${VZWeightFile_PbP_IsPURejectTrue}"
export VARIANT_TAG="${VARIANT_TAG:-IsPURejectTrue}"
cd "$SCRIPT_DIR"
./run-pPb.sh --IsPUReject true "$@"
