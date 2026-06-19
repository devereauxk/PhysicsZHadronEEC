#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
PRESENTATION_DIR="${1:?presentation directory is required}"
PP_TAG="${PP_TAG_OVERRIDE:-EEV5_ZV9_trkV27_nmix10_bin10x10_20260502}"
PPB_TAG="${PPB_TAG_OVERRIDE:-ZV9_trkV27_nmix10_bin10x10_20260502}"
TEX_OUT="$PRESENTATION_DIR/central_combined_bin10x10_20260502_integrals.tex"
TSV_OUT="$PRESENTATION_DIR/central_combined_bin10x10_20260502_integrals.tsv"

root -l -b -q "$THISDIR/quantify_deltaphi_integrals_bin10x10_20260502.C(\"$PP_TAG\",\"$PPB_TAG\",\"$TEX_OUT\",\"$TSV_OUT\")"

if [ ! -f "$TEX_OUT" ] || [ ! -f "$TSV_OUT" ]; then
    echo "Failed to generate DeltaPhi integral audit outputs." >&2
    exit 1
fi
