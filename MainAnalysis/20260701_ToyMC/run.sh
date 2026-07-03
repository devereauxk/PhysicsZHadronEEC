#!/bin/bash
set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteToyMC

NTOYS1="${NTOYS1:-500}"
NTOYS2="${NTOYS2:-10000}"
SEED="${SEED:-42}"
OUTPUT_DIR="${OUTPUT_DIR:-$THISDIR/output}"
SYST_DIR="/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output"

mkdir -p "${OUTPUT_DIR}"

# Clear output files for fresh run
rm -f "${OUTPUT_DIR}/test1_toymc.tsv" "${OUTPUT_DIR}/test2_toymc.tsv"
rm -f "${OUTPUT_DIR}/toymc_histograms.root"

# Bin definitions: "label|scan_suffix|trkRange|syst_scan_suffix"
BINS=(
  "ZPT0_500 trkPT0.5_15||0.5_15|"
  "ZPT0_500 trkPT0.5_2|_scan|0.5_2|_scan"
  "ZPT0_500 trkPT2_4|_scan|2_4|_scan"
  "ZPT0_500 trkPT4_15|_scan|4_15|_scan"
  "ZPT0_30 trkPT0.5_2|_scan|0.5_2|_scan"
  "ZPT0_30 trkPT2_4|_scan|2_4|_scan"
  "ZPT0_30 trkPT4_15|_scan|4_15|_scan"
  "ZPT30_500 trkPT0.5_2|_scan|0.5_2|_scan"
  "ZPT30_500 trkPT2_4|_scan|2_4|_scan"
  "ZPT30_500 trkPT4_15|_scan|4_15|_scan"
)

FIRST=true
for entry in "${BINS[@]}"; do
  IFS='|' read -r LABEL SCAN_SUFFIX TRK_RANGE SYST_SCAN_SUFFIX <<< "$entry"

  ZPT=$(echo "$LABEL" | sed 's/ZPT\([^ ]*\) .*/\1/')
  TRK_LABEL=$(echo "$LABEL" | sed 's/.* trkPT//')

  PPB_FILE="${OFFICIAL_RESULT_DIR}/pPb_trkResidual_${OFFICIAL_TAG_PPB}${SCAN_SUFFIX}_ZPT${ZPT}-result.root"
  PBP_FILE="${OFFICIAL_RESULT_DIR}/PbP_trkResidual_${OFFICIAL_TAG_PPB}${SCAN_SUFFIX}_ZPT${ZPT}-result.root"
  PP_FILE="${OFFICIAL_RESULT_DIR}/pp_trkResidual_${OFFICIAL_TAG_PP}${SCAN_SUFFIX}_ZPT${ZPT}-result.root"
  SYST_FILE="${SYST_DIR}/pPbPbp_${OFFICIAL_TAG_PPB}${SYST_SCAN_SUFFIX}_ZPT${ZPT}_trkPT${TRK_LABEL}-systematics.root"

  echo ""
  echo "=========================================="
  echo "  ${LABEL}"
  echo "=========================================="

  APPEND_FLAG="false"
  if [ "$FIRST" = true ]; then
    FIRST=false
  else
    APPEND_FLAG="true"
  fi

  ./ExecuteToyMC \
      --pPbFile "${PPB_FILE}" \
      --PbPFile "${PBP_FILE}" \
      --ppFile "${PP_FILE}" \
      --SystematicsFile "${SYST_FILE}" \
      --TrkRange "${TRK_RANGE}" \
      --OutputDir "${OUTPUT_DIR}" \
      --BinLabel "${LABEL}" \
      --NToys1 "${NTOYS1}" \
      --NToys2 "${NTOYS2}" \
      --Seed "${SEED}" \
      --AppendTSV "${APPEND_FLAG}"
done

echo ""
echo "=== All bins complete ==="
echo ""

# Meta-analysis and summary markdown
python3 - "${OUTPUT_DIR}" <<'PYEOF'
import sys, os
import numpy as np
from scipy.stats import kstest

outdir = sys.argv[1]

def read_tsv(path):
    rows = []
    with open(path) as f:
        header = f.readline().strip().split('\t')
        for line in f:
            vals = line.strip().split('\t')
            rows.append(dict(zip(header, vals)))
    return rows

rows1 = read_tsv(os.path.join(outdir, 'test1_toymc.tsv'))
rows2 = read_tsv(os.path.join(outdir, 'test2_toymc.tsv'))

# Independent scan bins only (exclude overlapping ZPT0_500 bins)
SCAN_ONLY = lambda r: ('ZPT0_500' not in r['BinLabel'])

with open(os.path.join(outdir, 'toymc_summary.md'), 'w') as md:
    md.write("# Toy MC p-value calibration summary\n\n")

    # ── Test 1 ──
    md.write("## Test 1: pPb vs PbP permutation toys (stat-only, 12 bins)\n\n")
    md.write("Under H0 (pPb = PbP), pool all JK events and randomly partition into pseudo-pPb/pseudo-PbP.\n")
    md.write("Rebuild JK covariance and FullCovariance chi-squared for each toy.\n\n")

    md.write("### Per-bin diagnostics\n\n")
    md.write("| Bin | Observable | NToys | Data chi2 | ndf | Data p | ")
    md.write("Toy mean chi2 | Toy RMS | Exp mean | Exp RMS | Emp. p | KS p (toys vs U) |\n")
    md.write("| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |\n")
    for r in rows1:
        md.write(f"| {r['BinLabel']} | {r['Observable']} | {r['NToys']} | "
                 f"{float(r['DataChi2']):.2f} | {r['DataNdf']} | {float(r['DataPval']):.4f} | "
                 f"{float(r['ToyMeanChi2']):.2f} | {float(r['ToyRMSChi2']):.2f} | "
                 f"{float(r['ExpectedMeanChi2']):.1f} | {float(r['ExpectedRMSChi2']):.2f} | "
                 f"{float(r['EmpiricalPval']):.4f} | {float(r['KS_pval_vs_Uniform']):.4f} |\n")

    md.write("\n### Meta-analysis: KS test of data p-values vs Uniform(0,1)\n\n")
    md.write("Uses 6 independent scan bins (ZPT{0_30,30_500} x trkPT{0.5_2,2_4,4_15}).\n")
    md.write("ZPT0_500 bins excluded due to overlap.\n\n")

    scan1 = [r for r in rows1 if SCAN_ONLY(r)]
    for obs in ['DeltaEta', 'DeltaPhi']:
        pvals = [float(r['DataPval']) for r in scan1 if r['Observable'] == obs]
        if len(pvals) >= 3:
            stat, pv = kstest(pvals, 'uniform')
            md.write(f"- **{obs}**: n={len(pvals)}, KS D={stat:.4f}, p={pv:.4f}, "
                     f"mean p-value={np.mean(pvals):.4f}\n")

    combined_pvals = [float(r['DataPval']) for r in scan1]
    if len(combined_pvals) >= 3:
        stat, pv = kstest(combined_pvals, 'uniform')
        md.write(f"- **Combined (Eta+Phi)**: n={len(combined_pvals)}, KS D={stat:.4f}, p={pv:.4f}, "
                 f"mean p-value={np.mean(combined_pvals):.4f}\n")

    # ── Test 2 ──
    md.write("\n## Test 2: combined pPb vs pp parametric toys (stat+syst, 6 unique bins)\n\n")
    md.write("Under H0 (pPb = pp), draw pseudo-data delta ~ N(0, C_total) where C_total = C_stat + C_syst.\n")
    md.write("Compute chi-squared via SVD pseudoinverse on unique bins.\n\n")

    md.write("### Per-bin diagnostics (FullCovariance method)\n\n")
    md.write("| Bin | Observable | NToys | Data chi2 | ndf | Data p | ")
    md.write("Toy mean chi2 | Toy RMS | Exp mean | Exp RMS | Emp. p | KS p (toys vs U) |\n")
    md.write("| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |\n")
    rows2_full = [r for r in rows2 if 'FullCovariance' in r['Method']]
    for r in rows2_full:
        md.write(f"| {r['BinLabel']} | {r['Observable']} | {r['NToys']} | "
                 f"{float(r['DataChi2']):.2f} | {r['DataNdf']} | {float(r['DataPval']):.4f} | "
                 f"{float(r['ToyMeanChi2']):.2f} | {float(r['ToyRMSChi2']):.2f} | "
                 f"{float(r['ExpectedMeanChi2']):.1f} | {float(r['ExpectedRMSChi2']):.2f} | "
                 f"{float(r['EmpiricalPval']):.4f} | {float(r['KS_pval_vs_Uniform']):.4f} |\n")

    md.write("\n### Meta-analysis: KS test of data p-values vs Uniform(0,1)\n\n")
    scan2 = [r for r in rows2_full if SCAN_ONLY(r)]
    for obs in ['DeltaEta', 'DeltaPhi']:
        pvals = [float(r['DataPval']) for r in scan2 if r['Observable'] == obs]
        if len(pvals) >= 3:
            stat, pv = kstest(pvals, 'uniform')
            md.write(f"- **{obs}**: n={len(pvals)}, KS D={stat:.4f}, p={pv:.4f}, "
                     f"mean p-value={np.mean(pvals):.4f}\n")

    combined_pvals2 = [float(r['DataPval']) for r in scan2]
    if len(combined_pvals2) >= 3:
        stat, pv = kstest(combined_pvals2, 'uniform')
        md.write(f"- **Combined (Eta+Phi)**: n={len(combined_pvals2)}, KS D={stat:.4f}, p={pv:.4f}, "
                 f"mean p-value={np.mean(combined_pvals2):.4f}\n")

    # ── Interpretation ──
    md.write("\n## Interpretation\n\n")
    md.write("### Test 1 (pPb vs PbP)\n\n")
    md.write("The permutation toys directly test chi-squared calibration under H0 by generating\n")
    md.write("pseudo-datasets from the pooled pPb+PbP event sample. If the JK covariance estimator\n")
    md.write("and SVD pseudoinverse are correctly calibrated:\n")
    md.write("- Toy mean chi2 should match ndf (expected mean of chi2(ndf))\n")
    md.write("- Toy RMS should match sqrt(2*ndf) (expected RMS of chi2(ndf))\n")
    md.write("- Per-bin KS(toys vs Uniform) should show no significant rejection\n")
    md.write("- Empirical p-value should be consistent with the analytic p-value\n\n")

    md.write("### Test 2 (combined pPb vs pp)\n\n")
    md.write("The parametric toys validate the SVD inversion and unique-bin selection machinery.\n")
    md.write("Since pseudo-data is drawn from N(0, C_total), the resulting chi-squared should be\n")
    md.write("chi2(ndf) by construction if the inversion is correct. High data p-values in Test 2\n")
    md.write("are expected when systematic uncertainties dominate the total covariance and the\n")
    md.write("actual pPb-pp difference is much smaller than the systematic uncertainty.\n\n")

    md.write("### Note on bin correlations\n\n")
    md.write("The 10 reported bins are not independent: ZPT0_500 overlaps with ZPT{0_30,30_500},\n")
    md.write("and trkPT0.5_15 overlaps with trkPT{0.5_2,2_4,4_15}. The meta-analysis KS test\n")
    md.write("uses only the 6 independent scan bins. Even so, the same data enters all bins,\n")
    md.write("so correlations between different kinematic selections are expected.\n")

print("Wrote toymc_summary.md")
PYEOF

echo ""
echo "=== Done. Outputs: ==="
ls -lh "${OUTPUT_DIR}/"*.md "${OUTPUT_DIR}/"*.tsv "${OUTPUT_DIR}/"*.root 2>/dev/null
