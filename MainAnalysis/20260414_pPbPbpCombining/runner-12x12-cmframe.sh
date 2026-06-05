#!/bin/bash
# Study 17: Option A — CM-frame signed comparison.
# Acceptance: |eta_cm| < 1.935 for all systems (yBoost = 0.465).
# pPb lab-frame cut: -1.470 < eta_lab < 2.400  (eta_cm = eta_lab - 0.465)
# Pbp lab-frame cut: -2.400 < eta_lab < 1.470  (eta_cm = eta_lab + 0.465)
# pp  lab-frame cut: -1.935 < eta_lab < 1.935  (no boost)
# DeltaEta is boost-invariant: lab-frame values equal CM-frame values.
# Both pPb and Pbp use FillSigned=true, FlipDeltaEta=false.
# Combine signed pPb + Pbp, then symmetrize for presentation.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"
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

COMMON=(
    --IsPP false --IsGenZ false --IsData true
    --UseEventWeight true --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --UseVZWeight true --UseJackknife true
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --FillSigned true
    --MaxMixDeltaVZ 1.0
)

echo "=== pPb CM-frame (-1.470 < eta_lab < 2.400) ==="
./system-analysis.sh "pPb_trkResidual_${TAG}_12x12_cma" \
    "${COMMON[@]}" \
    --IsPPb true \
    --TrackEtaMin -1.470 --TrackEtaMax 2.400 \
    --ZYSignedMin -1.470 --ZYSignedMax 2.400 \
    --Input "$OFFICIAL_DATAINPUT_PPB" --MixFile "$OFFICIAL_DATAINPUT_PPB" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb"

echo "=== Pbp CM-frame (-2.400 < eta_lab < 1.470, no flip) ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_12x12_cma" \
    "${COMMON[@]}" \
    --IsPPb false \
    --TrackEtaMin -2.400 --TrackEtaMax 1.470 \
    --ZYSignedMin -2.400 --ZYSignedMax 1.470 \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

echo "=== pp CM-frame (-1.935 < eta_lab < 1.935) ==="
./system-analysis.sh "pp_trkResidual_${TAG_PP}_12x12_cma" \
    --IsPP true --IsGenZ false --IsData true \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseVZWeight true --UseJackknife true \
    --IsPPb false \
    --yBoost 0 --nMix 10 \
    --ResultDEtaBins 12 --ResultDPhiBins 12 \
    --FillSigned true \
    --MaxMixDeltaVZ 1.0 \
    --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
    --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
    --Input "$OFFICIAL_DATAINPUT_PP" --MixFile "$OFFICIAL_DATAINPUT_PP" \
    --ZWeightFile "$ZWeightFile_PP" \
    --ResidualWeightFile "$RWeightFile_PP" \
    --VZWeightFile "$VZWeightFile_PP"

rm -f "$CONFIG"
echo "=== Done: CM-frame (Option A) analysis complete ==="
