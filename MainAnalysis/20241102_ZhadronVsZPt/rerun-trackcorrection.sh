#!/bin/bash
# Re-run TrackCorrection systematic with DEDICATED TC residual weights.
# Each TC variation uses a residual correction derived with the same
# TrackExtraWeight applied to MC RECO (but not MC Gen), so the residual
# absorbs the track-shape change and the systematic reflects only the
# flat ±2.4% scaling effect on the per-Z normalized yield.
#
# Runs pp, pPb, PbP in parallel (30 threads each = 90 total).
# Within each system, 0.976 and 1.024 run sequentially (same skim file).

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=30

TAG_PP="${OFFICIAL_TAG_PP}"
TAG_PPB="${OFFICIAL_TAG_PPB}"

PP_INPUT="${OFFICIAL_DATAINPUT_PP}"
PPB_INPUT="${OFFICIAL_DATAINPUT_PPB}"
PBP_INPUT="${OFFICIAL_DATAINPUT_PBP}"

CONFIG=$(mktemp /tmp/kdeverea/signed_tc_config_XXXXXX.sh)
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
    --nMix 10
    --UseJackknife true
)

run_system() {
    local SYSTEM=$1
    local TAG=$2
    local INPUT=$3
    local IS_PP=$4
    local IS_PPB=$5
    local FLIP=$6
    local ETA_MIN=$7
    local ETA_MAX=$8
    local USE_EVENT_WEIGHT=$9
    local R_WEIGHT_0p976=${10}
    local R_WEIGHT_1p024=${11}
    local Z_WEIGHT=${12}
    local VZ_WEIGHT=${13}
    local EE_EXTRA="${14}"

    local COMMON_ARGS=(
        --IsPP "$IS_PP" --IsGenZ false --IsData true --IsPPb "$IS_PPB"
        --UseEventWeight "$USE_EVENT_WEIGHT" --UseZWeight true
        --UseTrackWeight true --UseResidualWeight true
        --UseVZWeight true --yBoost 0
        --TrackEtaMin "$ETA_MIN" --TrackEtaMax "$ETA_MAX"
        --ZYSignedMin "$ETA_MIN" --ZYSignedMax "$ETA_MAX"
        --Input "$INPUT" --MixFile "$INPUT"
        --ZWeightFile "$Z_WEIGHT"
        --VZWeightFile "$VZ_WEIGHT"
        "${SIGNED_COMMON[@]}"
    )

    if [ "$FLIP" = "true" ]; then
        COMMON_ARGS+=(--FlipDeltaEta true)
    fi
    if [ -n "$EE_EXTRA" ]; then
        COMMON_ARGS+=(--EnergyExtraFile "$EE_EXTRA")
    fi

    echo "=== ${SYSTEM} TrackCorrection0p976 ==="
    ./system-analysis.sh "${SYSTEM}_trkResidual_${TAG}_TrackCorrection0p976" \
        "${COMMON_ARGS[@]}" --ResidualWeightFile "$R_WEIGHT_0p976" --TrackExtraWeight 0.976

    echo "=== ${SYSTEM} TrackCorrection1p024 ==="
    ./system-analysis.sh "${SYSTEM}_trkResidual_${TAG}_TrackCorrection1p024" \
        "${COMMON_ARGS[@]}" --ResidualWeightFile "$R_WEIGHT_1p024" --TrackExtraWeight 1.024
}

echo "Starting TrackCorrection rerun with DEDICATED TC residual weights at $(date)"
echo "NTHREAD=$NTHREAD per system (3 systems in parallel = 90 total)"

# pp (30 threads)
run_system "pp" "$TAG_PP" "$PP_INPUT" \
    true false false \
    -1.935 1.935 \
    false \
    "$RWeightFile_PP_TrackCorrection0p976" "$RWeightFile_PP_TrackCorrection1p024" \
    "$ZWeightFile_PP" "$VZWeightFile_PP" "$EEWeightFile_PP" &
PID_PP=$!

# pPb (30 threads)
run_system "pPb" "$TAG_PPB" "$PPB_INPUT" \
    false true false \
    -1.470 2.400 \
    true \
    "$RWeightFile_PPb_TrackCorrection0p976" "$RWeightFile_PPb_TrackCorrection1p024" \
    "$ZWeightFile_PPb" "$VZWeightFile_PPb" "" &
PID_PPB=$!

# PbP (30 threads)
run_system "PbP" "$TAG_PPB" "$PBP_INPUT" \
    false false true \
    -2.400 1.470 \
    true \
    "$RWeightFile_PbP_TrackCorrection0p976" "$RWeightFile_PbP_TrackCorrection1p024" \
    "$ZWeightFile_PbP" "$VZWeightFile_PbP" "" &
PID_PBP=$!

echo "PIDs: pp=$PID_PP pPb=$PID_PPB PbP=$PID_PBP"

wait $PID_PP && echo "pp done" || echo "pp FAILED"
wait $PID_PPB && echo "pPb done" || echo "pPb FAILED"
wait $PID_PBP && echo "PbP done" || echo "PbP FAILED"

rm -f "$CONFIG"
echo "All TrackCorrection reruns completed at $(date)"
