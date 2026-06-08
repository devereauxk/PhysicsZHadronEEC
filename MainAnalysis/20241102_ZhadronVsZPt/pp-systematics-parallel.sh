#!/bin/bash
# Parallel pp systematics re-run with corrected EE weights.
# Calls threader.sh directly (not via system-analysis.sh) to avoid wait-n issues in subshells.
# 3 groups run simultaneously (30 threads each, 90 total):
#   Group A: muon rejection (IsMuTaggedFalse)
#   Group B: scale factor (MuVar0-3, sequential within group)
#   Group C: track correction (TrackCorrection0p976, TrackCorrection1p024, sequential within group)
# Then sequential (30 threads each):
#   Tight, IsPURejectTrue, EEPrivate

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

TAG_PP="${OFFICIAL_TAG_PP}"
PP_INPUT="${OFFICIAL_DATAINPUT_PP}"

export SKIP_CLEAN=1
export NTHREAD=30

SIGNED_COMMON=(
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --DEtaRange 3.87
    --FillSigned true
    --MaxMixDeltaVZ 1.0
    --nMix 10
    --UseJackknife true
)

PP_COMMON=(
    --IsPP true --IsGenZ false --IsData true
    --UseEventWeight false --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --UseVZWeight true --IsPPb false --yBoost 0
    --TrackEtaMin -1.935 --TrackEtaMax 1.935
    --ZYSignedMin -1.935 --ZYSignedMax 1.935
    --Input "$PP_INPUT" --MixFile "$PP_INPUT"
    "${SIGNED_COMMON[@]}"
)

run_one() {
    local TAG_SUFFIX=$1; shift
    local BASENAME="pp_trkResidual_${TAG_PP}_${TAG_SUFFIX}"
    local PREFIX="output/${BASENAME}_ZPT0_500"

    echo ">>> Starting $TAG_SUFFIX at $(date)"

    ./threader.sh "${PREFIX}" "0.5_15" \
        "${PP_COMMON[@]}" \
        --MinTrackPT 0.5 --MaxTrackPT 15 \
        --MinZPT 0 --MaxZPT 500 \
        "$@"

    cp -f "${PREFIX}-0.5_15-nosub.root" "plots/${BASENAME}_ZPT0_500-nosub.root"
    cp -f "${PREFIX}-0.5_15-result.root" "plots/${BASENAME}_ZPT0_500-result.root"

    echo ">>> Done $TAG_SUFFIX at $(date)"
}

echo "=== pp systematics parallel runner ==="
echo "Start time: $(date)"
echo ""

echo "=== Phase 1: Parallel batch (3 groups x 30 threads = 90 total) ==="

# Group A: Muon rejection (1 variation)
(
    run_one "IsMuTaggedFalse" \
        --ZWeightFile "$ZWeightFile_PP" \
        --ResidualWeightFile "$RWeightFile_PP_IsMuTaggedFalse" \
        --EnergyExtraFile "$EEWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP" \
        --IsMuTagged false
) > /tmp/kdeverea/pp_syst_groupA.log 2>&1 &
PID_A=$!

# Group B: Scale factor (4 variations, sequential)
(
    for v in 0 1 2 3; do
        run_one "MuVar${v}" \
            --ZWeightFile "$ZWeightFile_PP" \
            --ResidualWeightFile "$RWeightFile_PP" \
            --EnergyExtraFile "$EEWeightFile_PP" \
            --VZWeightFile "$VZWeightFile_PP" \
            --ExtraZWeight $v
    done
) > /tmp/kdeverea/pp_syst_groupB.log 2>&1 &
PID_B=$!

# Group C: Track correction (2 variations, sequential)
(
    run_one "TrackCorrection0p976" \
        --ZWeightFile "$ZWeightFile_PP" \
        --ResidualWeightFile "$RWeightFile_PP_TrackCorrection0p976" \
        --EnergyExtraFile "$EEWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP" \
        --TrackExtraWeight 0.976

    run_one "TrackCorrection1p024" \
        --ZWeightFile "$ZWeightFile_PP" \
        --ResidualWeightFile "$RWeightFile_PP_TrackCorrection1p024" \
        --EnergyExtraFile "$EEWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP" \
        --TrackExtraWeight 1.024
) > /tmp/kdeverea/pp_syst_groupC.log 2>&1 &
PID_C=$!

echo "Parallel batch launched: GroupA=$PID_A GroupB=$PID_B GroupC=$PID_C"
echo "Logs: /tmp/kdeverea/pp_syst_group{A,B,C}.log"
wait $PID_A $PID_B $PID_C
echo ""
echo "=== Parallel batch complete at $(date) ==="
echo ""

echo "=== Phase 2: Sequential batch (30 threads each) ==="

run_one "Tight" \
    --ZWeightFile "$ZWeightFile_PP" \
    --ResidualWeightFile "$RWeightFile_PP_Tight" \
    --EnergyExtraFile "$EEWeightFile_PP" \
    --VZWeightFile "$VZWeightFile_PP" \
    --TrackSelectionMode Tight

if [ -n "${RWeightFile_PP_IsPURejectTrue:-}" ]; then
    run_one "IsPURejectTrue" \
        --ZWeightFile "${ZWeightFile_PP_IsPURejectTrue}" \
        --ResidualWeightFile "$RWeightFile_PP_IsPURejectTrue" \
        --EnergyExtraFile "$EEWeightFile_PP" \
        --VZWeightFile "${VZWeightFile_PP_IsPURejectTrue}" \
        --IsPUReject true
else
    echo "SKIP pp IsPURejectTrue: PU-reject R weights not available"
fi

run_one "EEPrivate" \
    --ZWeightFile "$ZWeightFile_PP" \
    --ResidualWeightFile "$RWeightFile_PP" \
    --EnergyExtraFile "$EEWeightFile_PP_PRIVATE" \
    --VZWeightFile "$VZWeightFile_PP"

echo ""
echo "=== All pp systematics complete at $(date) ==="
