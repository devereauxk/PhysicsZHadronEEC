#!/bin/bash
# Study 5: ΔVZ mixing sensitivity — nominal is MaxMixDeltaVZ=1.0 (_12x12_full from halfsplit).
# Produces 4 result files in MainAnalysis/20241102_ZhadronVsZPt/plots/:
#   pPb/Pbp × dvz0p5 (MaxMixDeltaVZ=0.5, variation) and dvzOff (MaxMixDeltaVZ=0, variation).
# Nominal (1.0 cm) reuses existing _12x12_full files; dvz1 no longer produced separately.

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
    --UseEventWeight true --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --UseVZWeight true --UseJackknife true
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
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

echo "=== pPb dvz0p5 (MaxMixDeltaVZ=0.5, variation) ==="
run_pPb "12x12_dvz0p5" --MaxMixDeltaVZ 0.5

echo "=== Pbp dvz0p5 ==="
run_PbP "12x12_dvz0p5" --MaxMixDeltaVZ 0.5

echo "=== pPb dvzOff (MaxMixDeltaVZ=0, no requirement) ==="
run_pPb "12x12_dvzOff" --MaxMixDeltaVZ 0

echo "=== Pbp dvzOff ==="
run_PbP "12x12_dvzOff" --MaxMixDeltaVZ 0

rm -f "$CONFIG"
echo "=== Done: deltavz analysis complete ==="
