#!/bin/bash
# Study 13: 20-bin production for sideband study.
# Default 20x20 result binning (no --ResultDEtaBins/--ResultDPhiBins override).
# pPb and Pbp run in parallel (40+40=80 total threads, within caps).

set -euo pipefail

MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=40
export CUT_PARALLELISM=1

CONFIG=$(mktemp /tmp/kdeverea/20bin_config_XXXXXX.sh)
cat > "$CONFIG" <<'EOF'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
EOF
export CONFIG_FILE="$CONFIG"

COMMON=(
    --IsPP false --IsGenZ false --IsData true
    --UseEventWeight true --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --UseVZWeight true --UseJackknife true
    --yBoost 0 --nMix 10
    --MaxMixDeltaVZ 1.0
)

echo "=== pPb 20-bin (background) ==="
./system-analysis.sh "pPb_trkResidual_${TAG}_20bin" \
    "${COMMON[@]}" \
    --IsPPb true \
    --Input "$OFFICIAL_DATAINPUT_PPB" --MixFile "$OFFICIAL_DATAINPUT_PPB" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb" &
PPB_PID=$!

echo "=== Pbp 20-bin (background) ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_20bin" \
    "${COMMON[@]}" \
    --IsPPb false \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP" &
PBP_PID=$!

wait $PPB_PID && echo "=== pPb done ===" || { echo "ERROR: pPb failed" >&2; kill $PBP_PID 2>/dev/null; exit 1; }
wait $PBP_PID && echo "=== Pbp done ===" || { echo "ERROR: Pbp failed" >&2; exit 1; }

rm -f "$CONFIG"
echo "=== Done: sideband 20-bin analysis complete ==="
