#!/bin/bash
# Study 3 supplement: pp signed DeltaEta/DeltaPhi.
# Produces 1 result file in MainAnalysis/20241102_ZhadronVsZPt/plots/
# which is overlaid as a third curve in the Study 3 signed plots.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
TAG_PP="${OFFICIAL_TAG_PP}"

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

echo "=== pp signed DeltaEta/DeltaPhi ==="
./system-analysis.sh "pp_trkResidual_${TAG_PP}_12x12_signed" \
    --IsPP true --IsGenZ false --IsData true \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseVZWeight true --UseJackknife true \
    --IsPPb false \
    --yBoost 0 --nMix 10 \
    --ResultDEtaBins 12 --ResultDPhiBins 12 \
    --FillSigned true \
    --MaxMixDeltaVZ 1.0 \
    --Input "$OFFICIAL_DATAINPUT_PP" --MixFile "$OFFICIAL_DATAINPUT_PP" \
    --ZWeightFile "$ZWeightFile_PP" \
    --ResidualWeightFile "$RWeightFile_PP" \
    --VZWeightFile "$VZWeightFile_PP"

rm -f "$CONFIG"
echo "=== Done: pp signed analysis complete ==="
