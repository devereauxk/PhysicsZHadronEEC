#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
(
    cd "$SCRIPT_DIR"
    VARIANT_TAG=Loose ./run-pp.sh --TrackSelectionMode Loose "$@"
)
(
    cd "$SCRIPT_DIR"
    VARIANT_TAG=Tight ./run-pp.sh --TrackSelectionMode Tight "$@"
)
(
    cd "$SCRIPT_DIR"
    VARIANT_TAG=IsMuTaggedFalse ./run-pp.sh --IsMuTagged false "$@"
)
(
    cd "$SCRIPT_DIR"
    NAME_TAG="${NAME_TAG:-20260415_ZV9_trkV27_TrackResidualCorrection}" TRACK_EXTRA_WEIGHT=0.976 VARIANT_TAG=TrackCorrection0p976 ./run-pp.sh "$@"
)
(
    cd "$SCRIPT_DIR"
    NAME_TAG="${NAME_TAG:-20260415_ZV9_trkV27_TrackResidualCorrection}" TRACK_EXTRA_WEIGHT=1.024 VARIANT_TAG=TrackCorrection1p024 ./run-pp.sh "$@"
)
(
    cd "$SCRIPT_DIR"
    VARIANT_TAG=IsPURejectTrue ./run-pp-pu.sh "$@"
)
