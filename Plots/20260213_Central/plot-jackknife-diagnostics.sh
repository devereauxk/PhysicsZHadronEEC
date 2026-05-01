#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

NOMINAL_PP_TAG="${NOMINAL_PP_TAG:-$OFFICIAL_TAG_PP}"
NOMINAL_PPB_TAG="${NOMINAL_PPB_TAG:-$OFFICIAL_TAG_PPB}"
JACKKNIFE_PP_TAG="${JACKKNIFE_PP_TAG:-${NOMINAL_PP_TAG}_jackknife}"
JACKKNIFE_PPB_TAG="${JACKKNIFE_PPB_TAG:-${NOMINAL_PPB_TAG}_jackknife}"
OUTPUT_DIR="${OUTPUT_DIR:-$THISDIR/plots/jackknife_diagnostics}"

mkdir -p "$OUTPUT_DIR"

root -l -q -b "jackknife_diagnostics.C(\"$NOMINAL_PP_TAG\",\"$NOMINAL_PPB_TAG\",\"$JACKKNIFE_PP_TAG\",\"$JACKKNIFE_PPB_TAG\",\"$OUTPUT_DIR\")"
