#!/bin/bash
# Fill remaining pp 5 TeV scan systematic families.
# Adds: IsMuTaggedFalse, MuVar0-3, TrackCorrection0p976/1p024
# (Loose, Tight, IsPURejectTrue already produced by pp-5tev.sh)
#
# Stream 1 (original skim): IsMuTaggedFalse, MuVar0, MuVar1, MuVar2
# Stream 2 (copy skim):     MuVar3, TC0p976, TC1p024
#
# Usage: bash pp-5tev-scan-systematics.sh

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=30

cd "$(dirname "$0")"

TAG_5TEV="5tev_ZV10_trkV29_nmix10"

PP_INPUT="${OFFICIAL_DATAINPUT_PP}"
PP_COPY="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.3/PPData_Reco-copy.root"

SIGNED_COMMON=(
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --DEtaRange 3.87
    --FillSigned true
    --MaxMixDeltaVZ 1.0
    --nMix 10
    --UseJackknife true
)

run_pp() {
    local INPUT_FILE=$1
    local TAG_PREFIX=$2; shift 2
    ./system-analysis.sh "$TAG_PREFIX" \
        --IsPP true --IsGenZ false --IsData true \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true --IsPPb false --yBoost 0 \
        --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
        --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
        --Input "$INPUT_FILE" --MixFile "$INPUT_FILE" \
        "${SIGNED_COMMON[@]}" "$@"
}

run_family() {
    local INPUT_FILE=$1
    local FAMILY=$2; shift 2
    for ZPT in 0_500 0_30 30_500; do
        CONFIG=$(mktemp /tmp/kdeverea/pp5tev_scan_${FAMILY}_XXXXXX.sh)
        cat > "$CONFIG" <<EOFCFG
ZPT_RANGES=("${ZPT}")
PT_RANGES=("0.5_2" "2_4" "4_15")
EOFCFG
        CONFIG_FILE="$CONFIG" run_pp "$INPUT_FILE" \
            "pp_trkResidual_${TAG_5TEV}_scan_${FAMILY}" "$@"
        rm -f "$CONFIG"
    done
}

echo "=== pp 5 TeV scan systematics started at $(date) ==="

# --- Stream 1: original skim (4 families × 3 ZPT = 12 calls) ---
(
    echo "[Stream 1] IsMuTaggedFalse, MuVar0-2 — $(date)"

    run_family "$PP_INPUT" "IsMuTaggedFalse" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_IsMuTaggedFalse" \
        --VZWeightFile "$VZWeightFile_PP" --IsMuTagged false

    for v in 0 1 2; do
        run_family "$PP_INPUT" "MuVar${v}" \
            --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" \
            --VZWeightFile "$VZWeightFile_PP" --ExtraZWeight $v
    done

    echo "[Stream 1] DONE at $(date)"
) > /tmp/kdeverea/pp5tev_scan_syst_1.log 2>&1 &
PID1=$!

# --- Stream 2: copy skim (3 families × 3 ZPT = 9 calls) ---
(
    echo "[Stream 2] MuVar3, TC0p976, TC1p024 — $(date)"

    run_family "$PP_COPY" "MuVar3" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP" --ExtraZWeight 3

    run_family "$PP_COPY" "TrackCorrection0p976" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_TrackCorrection0p976" \
        --VZWeightFile "$VZWeightFile_PP" --TrackExtraWeight 0.976

    run_family "$PP_COPY" "TrackCorrection1p024" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_TrackCorrection1p024" \
        --VZWeightFile "$VZWeightFile_PP" --TrackExtraWeight 1.024

    echo "[Stream 2] DONE at $(date)"
) > /tmp/kdeverea/pp5tev_scan_syst_2.log 2>&1 &
PID2=$!

echo "Stream 1 (PID $PID1): IsMuTaggedFalse, MuVar0-2 → /tmp/kdeverea/pp5tev_scan_syst_1.log"
echo "Stream 2 (PID $PID2): MuVar3, TC0p976, TC1p024 → /tmp/kdeverea/pp5tev_scan_syst_2.log"

FAIL=0
wait $PID1 && echo "Stream 1 OK" || { echo "Stream 1 FAILED"; FAIL=1; }
wait $PID2 && echo "Stream 2 OK" || { echo "Stream 2 FAILED"; FAIL=1; }

if [ "$FAIL" -eq 0 ]; then
    echo "=== pp 5 TeV scan systematics complete at $(date) ==="
else
    echo "=== FAILED — check logs in /tmp/kdeverea/pp5tev_scan_syst_*.log ==="
    exit 1
fi
