#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd /home/kdeverea/PhysicsZHadronEEC
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
cd "$THISDIR"

make

export STUDY_TAG_SUFFIX="${STUDY_TAG_SUFFIX:-_bin12x12_20260507}"
export SKIP_CLEAN=1
export CUT_PARALLELISM="${CUT_PARALLELISM:-1}"
export NTHREAD="${NTHREAD:-25}"
export NSLICE_FACTOR="${NSLICE_FACTOR:-1}"

MODIFIED_RESULT_ARGS=(--ResultDEtaBins 12 --ResultDPhiBins 12)
SCAN_CONFIG="$THISDIR/config_note_bin12x12_scan_20260507.sh"
INCLUSIVE_CONFIG="$THISDIR/config_note_bin12x12_inclusive_20260507.sh"

CONFIG_FILE="$SCAN_CONFIG" ./systematics.sh 1 1 1 "${MODIFIED_RESULT_ARGS[@]}"
CONFIG_FILE="$INCLUSIVE_CONFIG" ./systematics.sh 1 1 1 "${MODIFIED_RESULT_ARGS[@]}"
