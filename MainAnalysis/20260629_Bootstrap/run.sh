#!/bin/bash
set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteBootstrap

NBOOTSTRAP="${NBOOTSTRAP:-2000}"
SEED="${SEED:-42}"
OUTPUT_DIR="${OUTPUT_DIR:-$THISDIR/output}"
SYST_DIR="/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output"

mkdir -p "${OUTPUT_DIR}"

# Clear TSV files for fresh header
rm -f "${OUTPUT_DIR}/test1_allbins.tsv" "${OUTPUT_DIR}/test2_allbins.tsv"

# Bin definitions: "label|zpt_suffix|trkRange|syst_tag_suffix"
# Inclusive uses no _scan suffix; scan bins use _scan suffix
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

  # Extract ZPT from label (e.g., "ZPT0_500 trkPT0.5_15" -> "0_500")
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
  echo "pPb:  ${PPB_FILE}"
  echo "PbP:  ${PBP_FILE}"
  echo "pp:   ${PP_FILE}"
  echo "Syst: ${SYST_FILE}"

  APPEND_FLAG="false"
  if [ "$FIRST" = true ]; then
    FIRST=false
  else
    APPEND_FLAG="true"
  fi

  ./ExecuteBootstrap \
      --pPbFile "${PPB_FILE}" \
      --PbPFile "${PBP_FILE}" \
      --ppFile "${PP_FILE}" \
      --SystematicsFile "${SYST_FILE}" \
      --TrkRange "${TRK_RANGE}" \
      --OutputDir "${OUTPUT_DIR}" \
      --BinLabel "${LABEL}" \
      --NBootstrap "${NBOOTSTRAP}" \
      --Seed "${SEED}" \
      --AppendTSV "${APPEND_FLAG}"
done

echo ""
echo "=== All bins complete ==="
echo ""

# Assemble consolidated markdown tables
python3 - "${OUTPUT_DIR}" <<'PYEOF'
import sys, os

outdir = sys.argv[1]

def read_tsv(path):
    rows = []
    with open(path) as f:
        header = f.readline().strip().split('\t')
        for line in f:
            rows.append(line.strip().split('\t'))
    return header, rows

# Test 1 markdown
h1, r1 = read_tsv(os.path.join(outdir, 'test1_allbins.tsv'))
with open(os.path.join(outdir, 'test1_allbins.md'), 'w') as md:
    md.write("# Test 1: pPb vs PbP compatibility — JK vs Bootstrap (all bins)\n\n")
    md.write("| Bin | Observable | Method | chi2 | ndf | p-value |\n")
    md.write("| --- | --- | --- | ---: | ---: | ---: |\n")
    for r in r1:
        # r = [BinLabel, Observable, Method, chi2, ndf, p_value]
        md.write(f"| {r[0]} | {r[1]} | {r[2]} | {float(r[3]):.4f} | {r[4]} | {float(r[5]):.4f} |\n")

# Test 2 markdown
h2, r2 = read_tsv(os.path.join(outdir, 'test2_allbins.tsv'))
with open(os.path.join(outdir, 'test2_allbins.md'), 'w') as md:
    md.write("# Test 2: Combined pPb vs pp — JK vs Bootstrap (all bins)\n\n")
    md.write("| Bin | Observable | Method | chi2 | ndf | p-value |\n")
    md.write("| --- | --- | --- | ---: | ---: | ---: |\n")
    for r in r2:
        md.write(f"| {r[0]} | {r[1]} | {r[2]} | {float(r[3]):.4f} | {r[4]} | {float(r[5]):.4f} |\n")

print("Wrote test1_allbins.md and test2_allbins.md")
PYEOF

echo ""
echo "=== Done. Outputs: ==="
ls -lh "${OUTPUT_DIR}/"*.md "${OUTPUT_DIR}/"*.tsv 2>/dev/null
