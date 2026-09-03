#!/bin/bash
# JEWEL pPb R_pPb study: standalone, reads skims directly.
# Does not touch the main analysis directories.
set -euo pipefail
cd "$(dirname "$0")"

for f in /home/kdeverea/jewel/jewel_pp8160v3_MOD_500k.root \
         /home/kdeverea/jewel/jewel_pPb_v4_MOD_500k.root \
         ../output/jewel_pp_zptweight.root \
         ../output/jewel_pPb_zptweight.root; do
    [ -f "$f" ] || { echo "ERROR: missing $f"; exit 1; }
done

root -l -b -q 'calc_raa.C+'
echo "Done: output/jewel_pPb_raa.{root,tsv} plots/jewel_pPb_raa.pdf"
