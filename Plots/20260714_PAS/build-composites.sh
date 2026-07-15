#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR/overleaf_pdf_composites"

TECTONIC=/home/kdeverea/opt/tectonic-0.15.0-musl/tectonic

for TEX in paper_scan_ZPT0_500_phieta.tex paper_scan_ZPT0_30_phieta.tex paper_scan_ZPT30_500_phieta.tex; do
    echo "Building $TEX ..."
    "$TECTONIC" "$TEX"
done

echo "Done. Composites in $THISDIR/overleaf_pdf_composites/"
