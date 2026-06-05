#!/bin/bash
# Study 11: nThread=60 comparison run (full pPb+Pbp inclusive).

set -euo pipefail

MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=60
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
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --MaxMixDeltaVZ 1.0
)

echo "=== pPb nThread=60 ==="
./system-analysis.sh "pPb_trkResidual_${TAG}_12x12_nthread60" \
    "${COMMON[@]}" \
    --IsPPb true \
    --Input "$OFFICIAL_DATAINPUT_PPB" --MixFile "$OFFICIAL_DATAINPUT_PPB" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb"

echo "=== Pbp nThread=60 ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_12x12_nthread60" \
    "${COMMON[@]}" \
    --IsPPb false \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

rm -f "$CONFIG"
echo "=== Done: nthread60 analysis complete ==="
