#!/bin/bash
set -e

cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

export SKIP_CLEAN=1
export NTHREAD=30

TAG_PP="${OFFICIAL_TAG_PP}"
PP_ORIGINAL="${OFFICIAL_DATAINPUT_PP}"
PP_COPY="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.3/PPData_Reco-copy.root"

SIGNED_COMMON=(
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --DEtaRange 3.87 --FillSigned true
    --MaxMixDeltaVZ 1.0 --nMix 10 --UseJackknife true
)

mk_common() {
    local INPUT=$1
    echo --IsPP true --IsGenZ false --IsData true \
         --UseEventWeight false --UseZWeight true \
         --UseTrackWeight true --UseResidualWeight true \
         --UseVZWeight true --IsPPb false --yBoost 0 \
         --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
         --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
         --Input "$INPUT" --MixFile "$INPUT" \
         "${SIGNED_COMMON[@]}"
}

run_one() {
    local INPUT=$1; local TAG_SUFFIX=$2; shift 2
    local BASENAME="pp_trkResidual_${TAG_PP}_${TAG_SUFFIX}"
    local PREFIX="output/${BASENAME}_ZPT0_500"
    echo ">>> Starting $TAG_SUFFIX at $(date)"
    ./threader.sh "${PREFIX}" "0.5_15" \
        $(mk_common "$INPUT") \
        --MinTrackPT 0.5 --MaxTrackPT 15 \
        --MinZPT 0 --MaxZPT 500 "$@"
    cp -f "${PREFIX}-0.5_15-nosub.root" "plots/${BASENAME}_ZPT0_500-nosub.root"
    cp -f "${PREFIX}-0.5_15-result.root" "plots/${BASENAME}_ZPT0_500-result.root"
    echo ">>> Done $TAG_SUFFIX at $(date)"
}

echo "=== pp systematics remaining ($(date)) ==="
echo "Original skim: $PP_ORIGINAL"
echo "Copy skim:     $PP_COPY"

# --- Group 1: original skim ---
# MuVar2 + MuVar3 parallel (60 threads on original), then EEPrivate (30 threads)
(
    (
        run_one "$PP_ORIGINAL" "MuVar2" \
            --ZWeightFile "$ZWeightFile_PP" \
            --ResidualWeightFile "$RWeightFile_PP" \
            --EnergyExtraFile "$EEWeightFile_PP" \
            --VZWeightFile "$VZWeightFile_PP" \
            --ExtraZWeight 2
    ) &
    PID_MV2=$!

    (
        run_one "$PP_ORIGINAL" "MuVar3" \
            --ZWeightFile "$ZWeightFile_PP" \
            --ResidualWeightFile "$RWeightFile_PP" \
            --EnergyExtraFile "$EEWeightFile_PP" \
            --VZWeightFile "$VZWeightFile_PP" \
            --ExtraZWeight 3
    ) &
    PID_MV3=$!

    wait $PID_MV2 $PID_MV3
    echo ">>> MuVar2+MuVar3 done, starting EEPrivate on original skim"

    run_one "$PP_ORIGINAL" "EEPrivate" \
        --ZWeightFile "$ZWeightFile_PP" \
        --ResidualWeightFile "$RWeightFile_PP" \
        --EnergyExtraFile "$EEWeightFile_PP_PRIVATE" \
        --VZWeightFile "$VZWeightFile_PP"
) > /tmp/kdeverea/pp_syst_group_original.log 2>&1 &
PID_ORIG=$!

# --- Group 2: copy skim ---
# Tight then IsPURejectTrue sequential (30 threads on copy)
(
    run_one "$PP_COPY" "Tight" \
        --ZWeightFile "$ZWeightFile_PP" \
        --ResidualWeightFile "$RWeightFile_PP_Tight" \
        --EnergyExtraFile "$EEWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP" \
        --TrackSelectionMode Tight

    run_one "$PP_COPY" "IsPURejectTrue" \
        --ZWeightFile "${ZWeightFile_PP_IsPURejectTrue}" \
        --ResidualWeightFile "$RWeightFile_PP_IsPURejectTrue" \
        --EnergyExtraFile "$EEWeightFile_PP" \
        --VZWeightFile "${VZWeightFile_PP_IsPURejectTrue}" \
        --IsPUReject true
) > /tmp/kdeverea/pp_syst_group_copy.log 2>&1 &
PID_COPY=$!

echo "Group original: PID=$PID_ORIG (MuVar2+MuVar3 parallel -> EEPrivate)"
echo "Group copy:     PID=$PID_COPY (Tight -> IsPURejectTrue)"
echo "Logs: /tmp/kdeverea/pp_syst_group_{original,copy}.log"

wait $PID_ORIG $PID_COPY
echo "=== All remaining pp systematics complete at $(date) ==="
