#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
NAME_TAG="${NAME_TAG:-20260519_ZV9_trkV28_TrackResidualCorrection}"
export NAME_TAG

(
    cd "$SCRIPT_DIR"
    VARIANT_TAG=Loose ./run-pPb.sh --TrackSelectionMode Loose "$@"
)
(
    cd "$SCRIPT_DIR"
    VARIANT_TAG=Tight ./run-pPb.sh --TrackSelectionMode Tight "$@"
)
(
    cd "$SCRIPT_DIR"
    VARIANT_TAG=IsMuTaggedFalse ./run-pPb.sh --IsMuTagged false "$@"
)
(
    cd "$SCRIPT_DIR"
    TRACK_EXTRA_WEIGHT=0.976 VARIANT_TAG=TrackCorrection0p976 ./run-pPb.sh "$@"
)
(
    cd "$SCRIPT_DIR"
    TRACK_EXTRA_WEIGHT=1.024 VARIANT_TAG=TrackCorrection1p024 ./run-pPb.sh "$@"
)
# IsPURejectTrue not computed for ZV10 (PU-reject Z/R weights not in dictionary)
# (
#     cd "$SCRIPT_DIR"
#     VARIANT_TAG=IsPURejectTrue ./run-pPb-pu.sh "$@"
# )
