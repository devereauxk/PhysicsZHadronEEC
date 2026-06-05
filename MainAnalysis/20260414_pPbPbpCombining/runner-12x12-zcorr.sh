#!/bin/bash
# Study 12: Apply 2D Z (yCM, phi) correction to Pbp in 12x12 runner.
# Uses z_correction_final.root (hWeightToApply: Pbp/pPb ratio in Z yCM x phi).
# Produces only the Pbp corrected result; pPb nominal already exists from halfsplit runner.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"

ZCORR_FILE="${SCRIPT_DIR}/output/z_correction_final.root"
if [[ ! -f "$ZCORR_FILE" ]]; then
    echo "ERROR: Z correction file not found: $ZCORR_FILE" >&2
    echo "Run output/z_correction_final.root generation first (run_z_correction.sh)." >&2
    exit 1
fi

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-30}
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
    --ZCorrectionFile "$ZCORR_FILE"
)

echo "=== Pbp with Z correction ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_12x12_zcorr" \
    "${COMMON[@]}" \
    --IsPPb false \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

rm -f "$CONFIG"
echo "=== Done: zcorr Pbp analysis complete ==="
