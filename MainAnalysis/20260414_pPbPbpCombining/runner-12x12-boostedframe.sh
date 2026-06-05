#!/bin/bash
# Study 18: Option B — pPb-oriented frame signed comparison.
# All systems use symmetric |eta_lab| < 1.935 acceptance (same physical overlap as Option A).
# pPb: keep lab frame, |eta_lab| < 1.935, FillSigned=true, FlipDeltaEta=false
# Pbp: flip orientation (FlipDeltaEta=true), |eta_lab| < 1.935 (|eta_oriented|=|-eta_lab|<1.935)
# pp:  shift into pPb-oriented frame: |eta_lab+0.465|<1.935 → -2.400<eta_lab<1.470
# Options A and B should give equivalent symmetrized results (cross-check).

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

echo "=== pPb pPb-oriented frame (|eta_lab|<1.935) ==="
./system-analysis.sh "pPb_trkResidual_${TAG}_12x12_cmb" \
    "${COMMON[@]}" \
    --IsPPb true \
    --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
    --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
    --Input "$OFFICIAL_DATAINPUT_PPB" --MixFile "$OFFICIAL_DATAINPUT_PPB" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb"

echo "=== Pbp pPb-oriented frame (|eta_lab|<1.935, FlipDeltaEta=true) ==="
./system-analysis.sh "PbP_trkResidual_${TAG}_12x12_cmb" \
    "${COMMON[@]}" \
    --IsPPb false \
    --FlipDeltaEta true \
    --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
    --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
    --Input "$OFFICIAL_DATAINPUT_PBP" --MixFile "$OFFICIAL_DATAINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP"

echo "=== pp pPb-oriented frame (|eta_lab|<1.935, symmetric, matching pPb/Pbp acceptance width) ==="
./system-analysis.sh "pp_trkResidual_${TAG_PP}_12x12_cmb" \
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
echo "=== Done: pPb-oriented frame (Option B) analysis complete ==="
