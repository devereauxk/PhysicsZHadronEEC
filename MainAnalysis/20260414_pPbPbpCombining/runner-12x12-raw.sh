#!/bin/bash
# Study 7: pPb and Pbp raw uncorrected data — all corrections turned off.
# Keep jackknife for statistical errors; keep MaxMixDeltaVZ 0.5.
# Produces 2 result files in MainAnalysis/20241102_ZhadronVsZPt/plots/.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-20}
export CUT_PARALLELISM=1

CONFIG=$(mktemp /tmp/kdeverea/12x12_config_XXXXXX.sh)
cat > "$CONFIG" <<'EOF'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
EOF
export CONFIG_FILE="$CONFIG"

COMMON=(
    --IsPP false --IsGenZ false --IsData true
    --UseEventWeight false --UseZWeight false
    --UseTrackWeight false --UseResidualWeight false
    --UseVZWeight false --UseJackknife true
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --MaxMixDeltaVZ 0.5
)

echo "=== pPb raw (all corrections off) ==="
./system-analysis.sh "pPb_trkResidual_${TAG}_12x12_raw" \
    "${COMMON[@]}" \
    --IsPPb true \
    --Input "$OFFICIAL_DATAINPUT_PPB" --MixFile "$OFFICIAL_DATAINPUT_PPB"

echo "=== Pbp raw (all corrections off) ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_12x12_raw" \
    "${COMMON[@]}" \
    --IsPPb false \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP"

rm -f "$CONFIG"
echo "=== Done: raw analysis complete ==="
