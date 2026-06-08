#!/bin/bash
# Build 1×3 composite PDFs for signed track-pT scan result figures.
# Builds all ZPT variants: inclusive (ZPT0_500), ZPT30_500, ZPT0_30.
set -euo pipefail
cd "$(dirname "$0")"

VARIANTS=("" "_ZPT30_500" "_ZPT20_500" "_ZPT40_500" "_ZPT0_30")

for variant in "${VARIANTS[@]}"; do
    for obs in deltaphi deltaeta; do
        TEX="overleaf_result_scan_signed_${obs}${variant}_combined.tex"
        if [ -f "$TEX" ]; then
            echo "Building $TEX ..."
            pdflatex -interaction=nonstopmode "$TEX" > /dev/null 2>&1 || echo "  WARN: $TEX failed (input PDFs may not exist yet)"
        fi
    done
done

rm -f overleaf_result_scan_signed_*_combined.{aux,log}
echo "Done. Output:"
ls -lh overleaf_result_scan_signed_*_combined.pdf 2>/dev/null || echo "No composite PDFs found"
