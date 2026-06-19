#!/bin/bash
# Study 20: pp MC Gen boost-prescription closure.
# Three variants of pp MC Gen with different eta acceptance:
#   Variant 0 (nominal): |eta_lab| < 2.4
#   Variant 1 (pPb-boosted): -1.935 < eta_lab < 2.865  (boost +0.465, then |eta_cm| < 2.4)
#   Variant 2 (Pbp-boosted): -2.865 < eta_lab < 1.935  (boost -0.465, then |eta_cm| < 2.4)
# All use FillSigned=true, IsGenZ=true, UseJackknife=false.
# Key closure: Variant 1 vs flipped Variant 2 should agree.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-40}
export CUT_PARALLELISM=1

CONFIG=$(mktemp /tmp/kdeverea/12x12_ppmc_config_XXXXXX.sh)
cat > "$CONFIG" <<'EOF'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
EOF

COMMON=(
    --IsPP true --IsGenZ true --IsData false
    --UseEventWeight true --UseZWeight false
    --UseTrackWeight true --UseResidualWeight false
    --UseVZWeight true --VZWeightFile "$VZWeightFile_PP"
    --IsPPb false
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --FillSigned true
    --MaxMixDeltaVZ 1.0
    --UseJackknife false
    --Input "$OFFICIAL_MCGENINPUT_PP"
    --MixFile "$OFFICIAL_MCGENINPUT_PP"
)

echo "=== Variant 0: nominal pp MC Gen (|eta|<2.4) ==="
CONFIG_FILE="$CONFIG" ./system-analysis.sh "ppmc_gen_boost_nom_12x12" \
    "${COMMON[@]}" \
    --TrackEtaMin -2.400 --TrackEtaMax 2.400 \
    --ZYSignedMin -2.400 --ZYSignedMax 2.400

echo "=== Variant 1: pPb-boosted pp MC Gen (-1.935<eta<2.865) ==="
CONFIG_FILE="$CONFIG" ./system-analysis.sh "ppmc_gen_boost_ppblike_12x12" \
    "${COMMON[@]}" \
    --TrackEtaMin -1.935 --TrackEtaMax 2.865 \
    --ZYSignedMin -1.935 --ZYSignedMax 2.865

echo "=== Variant 2: Pbp-boosted pp MC Gen (-2.865<eta<1.935) ==="
CONFIG_FILE="$CONFIG" ./system-analysis.sh "ppmc_gen_boost_pbplike_12x12" \
    "${COMMON[@]}" \
    --TrackEtaMin -2.865 --TrackEtaMax 1.935 \
    --ZYSignedMin -2.865 --ZYSignedMax 1.935

rm -f "$CONFIG"
echo "=== Done: pp MC Gen boost variants complete ==="
