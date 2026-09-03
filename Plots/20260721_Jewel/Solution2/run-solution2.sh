#!/bin/bash
# Solution 2: per-Ncoll-class quenching study.
# Prerequisites: the three class analyses have been run from
# MainAnalysis/20241102_ZhadronVsZPt via
#   MODE=pPbMODv4_Ncoll_{le3,8to12,ge17}_zrw_scan NTHREAD=30 bash jewel-signed.sh
set -euo pipefail
cd "$(dirname "$0")"

RESULT_DIR="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots"
for c in le3 8to12 ge17; do
    for zpt in 0_500 0_30 30_500; do
        f="${RESULT_DIR}/jewelPPbv4Ncoll_${c}_zrw_scan_ZPT${zpt}-result.root"
        [ -f "$f" ] || { echo "ERROR: missing $f"; exit 1; }
    done
done

root -l -b -q glauber_pPb.C
root -l -b -q analyze_ncoll_classes.C

echo "Done: output/away_ratios_ncoll.tsv output/mb_reweight.tsv plots/away_ratio_vs_ncoll.pdf"
