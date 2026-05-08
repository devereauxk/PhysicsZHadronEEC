#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
TEX_ETA="$THISDIR/overleaf_result_scan_deltaeta_combined.tex"
TEX_PHI="$THISDIR/overleaf_result_scan_deltaphi_combined.tex"
PDF_ETA="$THISDIR/overleaf_result_scan_deltaeta_combined.pdf"
PDF_PHI="$THISDIR/overleaf_result_scan_deltaphi_combined.pdf"

if ! command -v tectonic >/dev/null 2>&1; then
    echo "tectonic is required but was not found in PATH." >&2
    exit 1
fi

tectonic --outdir "$THISDIR" "$TEX_ETA" >/dev/null
tectonic --outdir "$THISDIR" "$TEX_PHI" >/dev/null

rm -f \
    "$THISDIR/overleaf_result_scan_deltaeta_combined.aux" \
    "$THISDIR/overleaf_result_scan_deltaeta_combined.log" \
    "$THISDIR/overleaf_result_scan_deltaphi_combined.aux" \
    "$THISDIR/overleaf_result_scan_deltaphi_combined.log"

if [ ! -f "$PDF_ETA" ] || [ ! -f "$PDF_PHI" ]; then
    echo "Failed to build combined Overleaf scan PDFs." >&2
    exit 1
fi
