#!/bin/bash
# Study 9: CM-frame signed DeltaEta/DeltaPhi.
# pPb in CM frame reuses _12x12_signed (yBoost cancels in deltaEta).
# Produces 1 new result file: PbP with yBoost=0.465 + FlipDeltaEta.
# This converts Pbp from lab frame to CM frame, making it comparable to pPb.
# pp: use pp_trkResidual_..._12x12_signed (no boost needed; lab = cm for pp).

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

echo "=== Pbp signed boost (yBoost=0.465 + FlipDeltaEta = CM frame) ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_12x12_signed_boost" \
    --IsPP false --IsGenZ false --IsData true \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseVZWeight true --UseJackknife true \
    --IsPPb false \
    --yBoost 0.465 --nMix 10 \
    --ResultDEtaBins 12 --ResultDPhiBins 12 \
    --FillSigned true \
    --FlipDeltaEta true \
    --MaxMixDeltaVZ 1.0 \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

rm -f "$CONFIG"
echo "=== Done: Pbp boost analysis complete ==="
