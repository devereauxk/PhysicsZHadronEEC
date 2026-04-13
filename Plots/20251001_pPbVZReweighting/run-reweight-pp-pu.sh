#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
export INPUT_TAG="${INPUT_TAG:-IsPURejectTrue_noVZWeight_nmix0}"
export OUTPUT_TAG="${OUTPUT_TAG:-20260407_IsPURejectTrue_ZPT0_500}"
export SYSTEMS="${SYSTEMS:-pp}"
exec "$SCRIPT_DIR/run-reweight.sh"
