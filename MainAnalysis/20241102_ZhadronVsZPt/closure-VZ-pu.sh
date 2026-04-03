#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
DOPPB=${1:-1}
DOPBP=${2:-1}
shift $(( $# >= 2 ? 2 : $# ))
export TAG_PREFIX="${TAG_PREFIX:-IsPURejectTrue}"
exec "$SCRIPT_DIR/closure-VZ.sh" 0 "$DOPPB" "$DOPBP" --IsPUReject true "$@"
