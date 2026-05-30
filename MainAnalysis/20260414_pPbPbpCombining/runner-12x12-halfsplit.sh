#!/bin/bash
# Study 1: run half-splits for pPb and Pbp using main analysis 12x12 binning.
# Produces 6 result files in MainAnalysis/20241102_ZhadronVsZPt/plots/:
#   pPb full, Pbp full, pPb first/second half, Pbp first/second half.
# Run from the sandbox directory: cd 20260414_pPbPbpCombining && ./runner-12x12-halfsplit.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-20}
export CUT_PARALLELISM=1

# Write temp config: inclusive ZPT and track pT
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
    --MaxMixDeltaVZ 0.5
)

run_pPb() {
    local PREFIX=$1
    shift
    ./system-analysis.sh "pPb_trkResidual_${TAG}_${PREFIX}" \
        "${COMMON[@]}" \
        --IsPPb true \
        --Input "$OFFICIAL_DATAINPUT_PPB" --MixFile "$OFFICIAL_DATAINPUT_PPB" \
        --ZWeightFile "$ZWeightFile_PPb" \
        --ResidualWeightFile "$RWeightFile_PPb" \
        --VZWeightFile "$VZWeightFile_PPb" \
        "$@"
}

run_PbP() {
    local PREFIX=$1
    shift
    ./system-analysis.sh "PbP_trkResidual_${TAG}_${PREFIX}" \
        "${COMMON[@]}" \
        --IsPPb false \
        --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
        --ZWeightFile "$ZWeightFile_PbP" \
        --ResidualWeightFile "$RWeightFile_PbP" \
        --VZWeightFile "$VZWeightFile_PbP" \
        "$@"
}

echo "=== pPb full ==="
run_pPb "12x12_full"

echo "=== Pbp full ==="
run_PbP "12x12_full"

echo "=== pPb first half (Run < 286302) ==="
run_pPb "12x12_pPbFirst" --MaxRun 286302

echo "=== pPb second half (Run >= 286302) ==="
run_pPb "12x12_pPbSecond" --MinRun 286302

echo "=== Pbp first half (Run < 285549) ==="
run_PbP "12x12_PbPFirst" --MaxRun 285549

echo "=== Pbp second half (Run >= 285549) ==="
run_PbP "12x12_PbPSecond" --MinRun 285549

rm -f "$CONFIG"
echo "=== Done: halfsplit analysis complete ==="
