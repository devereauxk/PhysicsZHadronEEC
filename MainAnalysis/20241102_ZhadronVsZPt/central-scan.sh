#!/bin/bash
# Signed common-CM convention: track-pT scan production.
# Same physics as central-signed.sh but with 3 track bins instead of inclusive.
# Tag suffix "_scan" prevents filename collision with inclusive products.
#
# Usage: ./central-signed-scan.sh <DOPP> <DOPPB> <DOPBP> [extra args...]
# Example: NTHREAD=30 ./central-signed-scan.sh 1 1 1

set -euo pipefail

DOPP=$1
DOPPB=$2
DOPBP=$3
shift 3
COMMON_EXTRA_ARGS=("$@")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

nMix=10
TAG_PP="${OFFICIAL_TAG_PP}_scan"
TAG_PPB="${OFFICIAL_TAG_PPB}_scan"

PP_DATAINPUT="${OFFICIAL_DATAINPUT_PP}"
PPB_DATAINPUT="${OFFICIAL_DATAINPUT_PPB}"
PBP_DATAINPUT="${OFFICIAL_DATAINPUT_PBP}"

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-30}

CONFIG=$(mktemp /tmp/kdeverea/signed_scan_config_XXXXXX.sh)
cat > "$CONFIG" <<EOF
ZPT_RANGES=("${SCAN_ZPT_RANGE:-0_500}")
PT_RANGES=("0.5_2" "2_4" "4_15")
EOF
export CONFIG_FILE="$CONFIG"

SIGNED_COMMON=(
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --DEtaRange 3.87
    --FillSigned true
    --MaxMixDeltaVZ 1.0
    --nMix ${nMix}
    --UseJackknife true
)

if [ "$DOPPB" == "1" ]; then
    echo "=== pPb signed scan (|eta_cm|<1.935) ==="
    ./system-analysis.sh "pPb_trkResidual_${TAG_PPB}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb true \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true \
        --yBoost 0 \
        --TrackEtaMin -1.470 --TrackEtaMax 2.400 \
        --ZYSignedMin -1.470 --ZYSignedMax 2.400 \
        --Input "$PPB_DATAINPUT" --MixFile "$PPB_DATAINPUT" \
        --ZWeightFile "$ZWeightFile_PPb" \
        --ResidualWeightFile "$RWeightFile_PPb" \
        --VZWeightFile "$VZWeightFile_PPb" \
        "${SIGNED_COMMON[@]}" \
        "${COMMON_EXTRA_ARGS[@]}"
fi

if [ "$DOPBP" == "1" ]; then
    echo "=== Pbp signed scan (|eta_cm|<1.935, FlipDeltaEta) ==="
    ./system-analysis.sh "PbP_trkResidual_${TAG_PPB}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb false \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true \
        --yBoost 0 \
        --FlipDeltaEta true \
        --TrackEtaMin -2.400 --TrackEtaMax 1.470 \
        --ZYSignedMin -2.400 --ZYSignedMax 1.470 \
        --Input "$PBP_DATAINPUT" --MixFile "$PBP_DATAINPUT" \
        --ZWeightFile "$ZWeightFile_PbP" \
        --ResidualWeightFile "$RWeightFile_PbP" \
        --VZWeightFile "$VZWeightFile_PbP" \
        "${SIGNED_COMMON[@]}" \
        "${COMMON_EXTRA_ARGS[@]}"
fi

if [ "$DOPP" == "1" ]; then
    echo "=== pp signed scan (|eta|<1.935, EE-extrapolated) ==="
    ./system-analysis.sh "pp_trkResidual_${TAG_PP}" \
        --IsPP true --IsGenZ false --IsData true \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true \
        --IsPPb false \
        --yBoost 0 \
        --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
        --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
        --Input "$PP_DATAINPUT" --MixFile "$PP_DATAINPUT" \
        --ZWeightFile "$ZWeightFile_PP" \
        --ResidualWeightFile "$RWeightFile_PP" \
        --EnergyExtraFile "$EEWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP" \
        "${SIGNED_COMMON[@]}" \
        "${COMMON_EXTRA_ARGS[@]}"
fi

rm -f "$CONFIG"
echo "=== Done: signed scan production ==="
