#!/bin/bash
# Study 6: pPb and Pbp MC reco at full statistics (no --Fraction, no Z-count matching).
# Cross-check of Study 4 without the Z-count matching.
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
    --IsPP false --IsGenZ false --IsData false
    --UseEventWeight true --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --UseVZWeight true
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --MaxMixDeltaVZ 0.5
)

echo "=== pPb MC reco full stats ==="
./system-analysis.sh "pPbMC_trkResidual_${TAG}_12x12_fullstats" \
    "${COMMON[@]}" \
    --IsPPb true \
    --Input "$OFFICIAL_MCRECOINPUT_PPB" --MixFile "$OFFICIAL_MCRECOINPUT_PPB" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb"

echo "=== Pbp MC reco full stats ==="
./system-analysis.sh "PbPMC_trkResidual_${TAG}_12x12_fullstats" \
    "${COMMON[@]}" \
    --IsPPb false \
    --Input "$OFFICIAL_MCRECOINPUT_PBP" --MixFile "$OFFICIAL_MCRECOINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

rm -f "$CONFIG"
echo "=== Done: MC fullstats analysis complete ==="
