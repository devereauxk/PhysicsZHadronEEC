#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
TEX_ETA="$THISDIR/overleaf_result_scan_bin10x10_deltaeta_combined.tex"
TEX_PHI="$THISDIR/overleaf_result_scan_bin10x10_deltaphi_combined.tex"
PDF_ETA="$THISDIR/overleaf_result_scan_bin10x10_deltaeta_combined.pdf"
PDF_PHI="$THISDIR/overleaf_result_scan_bin10x10_deltaphi_combined.pdf"

cd "$THISDIR"

if command -v pdflatex >/dev/null 2>&1; then
    pdflatex -interaction=nonstopmode -halt-on-error "$(basename "$TEX_ETA")" >/dev/null
    pdflatex -interaction=nonstopmode -halt-on-error "$(basename "$TEX_PHI")" >/dev/null
elif command -v tectonic >/dev/null 2>&1; then
    tectonic --outdir "$THISDIR" "$TEX_ETA" >/dev/null
    tectonic --outdir "$THISDIR" "$TEX_PHI" >/dev/null
else
    echo "No TeX engine found (checked pdflatex and tectonic)." >&2
    exit 127
fi

rm -f \
    "$THISDIR/overleaf_result_scan_bin10x10_deltaeta_combined.aux" \
    "$THISDIR/overleaf_result_scan_bin10x10_deltaeta_combined.log" \
    "$THISDIR/overleaf_result_scan_bin10x10_deltaeta_combined.out" \
    "$THISDIR/overleaf_result_scan_bin10x10_deltaphi_combined.aux" \
    "$THISDIR/overleaf_result_scan_bin10x10_deltaphi_combined.log" \
    "$THISDIR/overleaf_result_scan_bin10x10_deltaphi_combined.out"

if [ ! -f "$PDF_ETA" ] || [ ! -f "$PDF_PHI" ]; then
    echo "Failed to build ten-bin combined Overleaf scan PDFs." >&2
    exit 1
fi
