#!/bin/bash
# pp 5.02 TeV native-energy side check (no EE weight).
# Runs nominal + systematics across inclusive and scan kinematic bins.
# Two parallel streams at 30 threads each = 60 total.
#
# Stream 1 (original skim): inclusive + scan ZPT0_500
# Stream 2 (copy skim):     scan ZPT0_30 + ZPT30_500
#
# Usage: bash pp-5tev.sh

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

run_inclusive_systematics() {
    local INPUT_FILE=$1
    echo "--- Inclusive systematics (9 families) ---"
    run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_Loose" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_Loose" \
        --VZWeightFile "$VZWeightFile_PP" --TrackSelectionMode Loose
    run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_Tight" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_Tight" \
        --VZWeightFile "$VZWeightFile_PP" --TrackSelectionMode Tight
    run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_IsMuTaggedFalse" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_IsMuTaggedFalse" \
        --VZWeightFile "$VZWeightFile_PP" --IsMuTagged false
    if [ -n "$RWeightFile_PP_IsPURejectTrue" ]; then
        run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_IsPURejectTrue" \
            --ZWeightFile "$ZWeightFile_PP_IsPURejectTrue" \
            --ResidualWeightFile "$RWeightFile_PP_IsPURejectTrue" \
            --VZWeightFile "$VZWeightFile_PP_IsPURejectTrue" --IsPUReject true
    else
        echo "SKIP pp IsPURejectTrue: PU-reject R weights not available"
    fi
    for v in 0 1 2 3; do
        run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_MuVar${v}" \
            --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" \
            --VZWeightFile "$VZWeightFile_PP" --ExtraZWeight $v
    done
    run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_TrackCorrection0p976" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_TrackCorrection0p976" \
        --VZWeightFile "$VZWeightFile_PP" --TrackExtraWeight 0.976
    run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_TrackCorrection1p024" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_TrackCorrection1p024" \
        --VZWeightFile "$VZWeightFile_PP" --TrackExtraWeight 1.024
}

run_scan_systematics() {
    local INPUT_FILE=$1
    echo "--- Scan systematics (Loose, Tight, IsPURejectTrue) ---"
    run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_scan_Loose" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_Loose" \
        --VZWeightFile "$VZWeightFile_PP" --TrackSelectionMode Loose
    run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_scan_Tight" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP_Tight" \
        --VZWeightFile "$VZWeightFile_PP" --TrackSelectionMode Tight
    if [ -n "$RWeightFile_PP_IsPURejectTrue" ]; then
        run_pp "$INPUT_FILE" "pp_trkResidual_${TAG_5TEV}_scan_IsPURejectTrue" \
            --ZWeightFile "$ZWeightFile_PP_IsPURejectTrue" \
            --ResidualWeightFile "$RWeightFile_PP_IsPURejectTrue" \
            --VZWeightFile "$VZWeightFile_PP_IsPURejectTrue" --IsPUReject true
    else
        echo "SKIP pp IsPURejectTrue: PU-reject R weights not available"
    fi
}

echo "=== pp 5 TeV production started at $(date) ==="

# --- Stream 1: original skim ---
# Inclusive (nominal + systematics) + scan ZPT0_500 (nominal + systematics)
(
    echo "[Stream 1] pp 5 TeV — original skim — $(date)"

    # Inclusive nominal
    CONFIG=$(mktemp /tmp/kdeverea/pp5tev_incl_XXXXXX.sh)
    cat > "$CONFIG" <<'EOFCFG'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
EOFCFG
    export CONFIG_FILE="$CONFIG"

    echo "--- Inclusive nominal ---"
    run_pp "$PP_INPUT" "pp_trkResidual_${TAG_5TEV}" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP"

    # Inclusive systematics
    run_inclusive_systematics "$PP_INPUT"
    rm -f "$CONFIG"

    # Scan ZPT0_500 nominal + systematics
    CONFIG=$(mktemp /tmp/kdeverea/pp5tev_scan0500_XXXXXX.sh)
    cat > "$CONFIG" <<'EOFCFG'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_2" "2_4" "4_15")
EOFCFG
    export CONFIG_FILE="$CONFIG"

    echo "--- Scan ZPT0_500 nominal ---"
    run_pp "$PP_INPUT" "pp_trkResidual_${TAG_5TEV}_scan" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP"

    run_scan_systematics "$PP_INPUT"
    rm -f "$CONFIG"

    echo "[Stream 1] DONE at $(date)"
) > /tmp/kdeverea/pp5tev_stream1.log 2>&1 &
PID1=$!

# --- Stream 2: copy skim ---
# Scan ZPT0_30 + ZPT30_500 (nominal + systematics each)
(
    echo "[Stream 2] pp 5 TeV — copy skim — $(date)"

    # Scan ZPT0_30
    CONFIG=$(mktemp /tmp/kdeverea/pp5tev_scan030_XXXXXX.sh)
    cat > "$CONFIG" <<'EOFCFG'
ZPT_RANGES=("0_30")
PT_RANGES=("0.5_2" "2_4" "4_15")
EOFCFG
    export CONFIG_FILE="$CONFIG"

    echo "--- Scan ZPT0_30 nominal ---"
    run_pp "$PP_COPY" "pp_trkResidual_${TAG_5TEV}_scan" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP"

    run_scan_systematics "$PP_COPY"
    rm -f "$CONFIG"

    # Scan ZPT30_500
    CONFIG=$(mktemp /tmp/kdeverea/pp5tev_scan30500_XXXXXX.sh)
    cat > "$CONFIG" <<'EOFCFG'
ZPT_RANGES=("30_500")
PT_RANGES=("0.5_2" "2_4" "4_15")
EOFCFG
    export CONFIG_FILE="$CONFIG"

    echo "--- Scan ZPT30_500 nominal ---"
    run_pp "$PP_COPY" "pp_trkResidual_${TAG_5TEV}_scan" \
        --ZWeightFile "$ZWeightFile_PP" --ResidualWeightFile "$RWeightFile_PP" \
        --VZWeightFile "$VZWeightFile_PP"

    run_scan_systematics "$PP_COPY"
    rm -f "$CONFIG"

    echo "[Stream 2] DONE at $(date)"
) > /tmp/kdeverea/pp5tev_stream2.log 2>&1 &
PID2=$!

echo "Stream 1 (PID $PID1): inclusive + ZPT0_500 scan → /tmp/kdeverea/pp5tev_stream1.log"
echo "Stream 2 (PID $PID2): ZPT0_30 + ZPT30_500 scan → /tmp/kdeverea/pp5tev_stream2.log"

FAIL=0
wait $PID1 && echo "Stream 1 OK" || { echo "Stream 1 FAILED"; FAIL=1; }
wait $PID2 && echo "Stream 2 OK" || { echo "Stream 2 FAILED"; FAIL=1; }

if [ "$FAIL" -eq 0 ]; then
    echo "=== pp 5 TeV production complete at $(date) ==="
else
    echo "=== FAILED — check logs in /tmp/kdeverea/pp5tev_*.log ==="
    exit 1
fi
