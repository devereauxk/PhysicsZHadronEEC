#!/bin/bash
# Signed common-CM convention: systematic variations (inclusive only).
# Mirrors systematics.sh but adds FillSigned, FlipDeltaEta, acceptance cuts, DEtaRange 3.87.
# Requires systematic R weight files to be populated in OfficialWeightDictionary.sh.
#
# Usage: ./systematics-signed.sh <DOPP> <DOPPB> <DOPBP> [extra args...]
# Example: NTHREAD=25 ./systematics-signed.sh 1 1 1

set -euo pipefail

DOPP=$1
DOPPB=$2
DOPBP=$3
shift 3
COMMON_EXTRA_ARGS=("$@")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

nMix=10
TAG_PP="${OFFICIAL_TAG_PP}"
TAG_PPB="${OFFICIAL_TAG_PPB}"

PP_INPUT="${OFFICIAL_DATAINPUT_PP}"
PPB_DATAINPUT="${OFFICIAL_DATAINPUT_PPB}"
PBP_DATAINPUT="${OFFICIAL_DATAINPUT_PBP}"

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-25}

CONFIG=$(mktemp /tmp/kdeverea/signed_syst_config_XXXXXX.sh)
cat > "$CONFIG" <<'EOF'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
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

run_pp() {
    local TAG_SUFFIX=$1; shift
    ./system-analysis.sh "pp_trkResidual_${TAG_PP}_${TAG_SUFFIX}" \
        --IsPP true --IsGenZ false --IsData true \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true --IsPPb false --yBoost 0 \
        --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
        --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
        --Input "$PP_INPUT" --MixFile "$PP_INPUT" \
        "${SIGNED_COMMON[@]}" "$@" "${COMMON_EXTRA_ARGS[@]}"
}

run_pPb() {
    local TAG_SUFFIX=$1; shift
    ./system-analysis.sh "pPb_trkResidual_${TAG_PPB}_${TAG_SUFFIX}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb true \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true --yBoost 0 \
        --TrackEtaMin -1.470 --TrackEtaMax 2.400 \
        --ZYSignedMin -1.470 --ZYSignedMax 2.400 \
        --Input "$PPB_DATAINPUT" --MixFile "$PPB_DATAINPUT" \
        "${SIGNED_COMMON[@]}" "$@" "${COMMON_EXTRA_ARGS[@]}"
}

run_PbP() {
    local TAG_SUFFIX=$1; shift
    ./system-analysis.sh "PbP_trkResidual_${TAG_PPB}_${TAG_SUFFIX}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb false \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true --yBoost 0 \
        --FlipDeltaEta true \
        --TrackEtaMin -2.400 --TrackEtaMax 1.470 \
        --ZYSignedMin -2.400 --ZYSignedMax 1.470 \
        --Input "$PBP_DATAINPUT" --MixFile "$PBP_DATAINPUT" \
        "${SIGNED_COMMON[@]}" "$@" "${COMMON_EXTRA_ARGS[@]}"
}

if [ "$DOPP" == "1" ]; then
    echo "=== pp signed systematics ==="
    run_pp "Loose" --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_Loose" --EnergyExtraFile "$EEWeightFile_PP" --VZWeightFile "$VZWeightFile_PP" --TrackSelectionMode Loose
    run_pp "Tight" --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_Tight" --EnergyExtraFile "$EEWeightFile_PP" --VZWeightFile "$VZWeightFile_PP" --TrackSelectionMode Tight
    run_pp "IsMuTaggedFalse" --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_IsMuTaggedFalse" --EnergyExtraFile "$EEWeightFile_PP" --VZWeightFile "$VZWeightFile_PP" --IsMuTagged false
    if [ -n "$RWeightFile_PP_IsPURejectTrue" ]; then
        run_pp "IsPURejectTrue" --ZWeightFile "$ZWeightFile_PP_IsPURejectTrue" --ResidualWeightFile "$RWeightFile_PP_IsPURejectTrue" --EnergyExtraFile "$EEWeightFile_PP" --VZWeightFile "$VZWeightFile_PP_IsPURejectTrue" --IsPUReject true
    else
        echo "SKIP pp IsPURejectTrue: PU-reject R weights not available"
    fi
    for v in 0 1 2 3; do
        run_pp "MuVar${v}" --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" --EnergyExtraFile "$EEWeightFile_PP" --VZWeightFile "$VZWeightFile_PP" --ExtraZWeight $v
    done
    run_pp "EEPrivate" --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" --EnergyExtraFile "$EEWeightFile_PP_PRIVATE" --VZWeightFile "$VZWeightFile_PP"
    run_pp "TrackCorrection0p976" --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_TrackCorrection0p976" --EnergyExtraFile "$EEWeightFile_PP" --VZWeightFile "$VZWeightFile_PP" --TrackExtraWeight 0.976
    run_pp "TrackCorrection1p024" --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_TrackCorrection1p024" --EnergyExtraFile "$EEWeightFile_PP" --VZWeightFile "$VZWeightFile_PP" --TrackExtraWeight 1.024
fi

if [ "$DOPPB" == "1" ]; then
    echo "=== pPb signed systematics ==="
    run_pPb "Loose" --ZWeightFile "$ZWeightFile_PPb" --ResidualWeightFile "$RWeightFile_PPb_Loose" --VZWeightFile "$VZWeightFile_PPb" --TrackSelectionMode Loose
    run_pPb "Tight" --ZWeightFile "$ZWeightFile_PPb" --ResidualWeightFile "$RWeightFile_PPb_Tight" --VZWeightFile "$VZWeightFile_PPb" --TrackSelectionMode Tight
    run_pPb "IsMuTaggedFalse" --ZWeightFile "$ZWeightFile_PPb" --ResidualWeightFile "$RWeightFile_PPb_IsMuTaggedFalse" --VZWeightFile "$VZWeightFile_PPb" --IsMuTagged false
    if [ -n "$RWeightFile_PPb_IsPURejectTrue" ]; then
        run_pPb "IsPURejectTrue" --ZWeightFile "$ZWeightFile_PPb_IsPURejectTrue" --ResidualWeightFile "$RWeightFile_PPb_IsPURejectTrue" --VZWeightFile "$VZWeightFile_PPb_IsPURejectTrue" --IsPUReject true
    else
        echo "SKIP pPb IsPURejectTrue: PU-reject R weights not available"
    fi
    for v in 0 1 2 3; do
        run_pPb "MuVar${v}" --ZWeightFile "$ZWeightFile_PPb" --ResidualWeightFile "$RWeightFile_PPb" --VZWeightFile "$VZWeightFile_PPb" --ExtraZWeight $v
    done
    run_pPb "TrackCorrection0p976" --ZWeightFile "$ZWeightFile_PPb" --ResidualWeightFile "$RWeightFile_PPb_TrackCorrection0p976" --VZWeightFile "$VZWeightFile_PPb" --TrackExtraWeight 0.976
    run_pPb "TrackCorrection1p024" --ZWeightFile "$ZWeightFile_PPb" --ResidualWeightFile "$RWeightFile_PPb_TrackCorrection1p024" --VZWeightFile "$VZWeightFile_PPb" --TrackExtraWeight 1.024
fi

if [ "$DOPBP" == "1" ]; then
    echo "=== Pbp signed systematics ==="
    run_PbP "Loose" --ZWeightFile "$ZWeightFile_PbP" --ResidualWeightFile "$RWeightFile_PbP_Loose" --VZWeightFile "$VZWeightFile_PbP" --TrackSelectionMode Loose
    run_PbP "Tight" --ZWeightFile "$ZWeightFile_PbP" --ResidualWeightFile "$RWeightFile_PbP_Tight" --VZWeightFile "$VZWeightFile_PbP" --TrackSelectionMode Tight
    run_PbP "IsMuTaggedFalse" --ZWeightFile "$ZWeightFile_PbP" --ResidualWeightFile "$RWeightFile_PbP_IsMuTaggedFalse" --VZWeightFile "$VZWeightFile_PbP" --IsMuTagged false
    if [ -n "$RWeightFile_PbP_IsPURejectTrue" ]; then
        run_PbP "IsPURejectTrue" --ZWeightFile "$ZWeightFile_PbP_IsPURejectTrue" --ResidualWeightFile "$RWeightFile_PbP_IsPURejectTrue" --VZWeightFile "$VZWeightFile_PbP_IsPURejectTrue" --IsPUReject true
    else
        echo "SKIP PbP IsPURejectTrue: PU-reject R weights not available"
    fi
    for v in 0 1 2 3; do
        run_PbP "MuVar${v}" --ZWeightFile "$ZWeightFile_PbP" --ResidualWeightFile "$RWeightFile_PbP" --VZWeightFile "$VZWeightFile_PbP" --ExtraZWeight $v
    done
    run_PbP "TrackCorrection0p976" --ZWeightFile "$ZWeightFile_PbP" --ResidualWeightFile "$RWeightFile_PbP_TrackCorrection0p976" --VZWeightFile "$VZWeightFile_PbP" --TrackExtraWeight 0.976
    run_PbP "TrackCorrection1p024" --ZWeightFile "$ZWeightFile_PbP" --ResidualWeightFile "$RWeightFile_PbP_TrackCorrection1p024" --VZWeightFile "$VZWeightFile_PbP" --TrackExtraWeight 1.024
fi

rm -f "$CONFIG"
echo "=== Done: signed common-CM systematic variations ==="
