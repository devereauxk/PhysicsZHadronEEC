#!/bin/bash
# Study 8: Nmix sensitivity — Nmix=20 vs nominal Nmix=10.
# Produces 2 result files in MainAnalysis/20241102_ZhadronVsZPt/plots/.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=40
export CUT_PARALLELISM=1

CONFIG=$(mktemp /tmp/kdeverea/12x12_config_XXXXXX.sh)
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
    --yBoost 0 --nMix 20
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --MaxMixDeltaVZ 1.0
)

echo "=== pPb Nmix=20 (background) ==="
./system-analysis.sh "pPb_trkResidual_${TAG}_12x12_nmix20" \
    "${COMMON[@]}" \
    --IsPPb true \
    --Input "$OFFICIAL_DATAINPUT_PPB" --MixFile "$OFFICIAL_DATAINPUT_PPB" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb" &
PPB_PID=$!

echo "=== Pbp Nmix=20 (background) ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_12x12_nmix20" \
    "${COMMON[@]}" \
    --IsPPb false \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP" &
PBP_PID=$!

wait $PPB_PID && wait $PBP_PID

rm -f "$CONFIG"
echo "=== Done: nmix20 analysis complete ==="
