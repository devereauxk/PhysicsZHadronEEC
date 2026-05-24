#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
export VARIANT_TAG="${VARIANT_TAG:-IsPURejectTrue}"
export VZ_WEIGHT_FILE_PP="${VZWeightFile_PP_IsPURejectTrue}"
export Z_WEIGHT_FILE_PP="${ZWeightFile_PP_IsPURejectTrue}"
export NAME_TAG="${NAME_TAG:-20260415_ZV9_trkV27_TrackResidualCorrection}"
exec "$(cd "$(dirname "$0")" && pwd)/run-pp.sh" --IsPUReject true "$@"
