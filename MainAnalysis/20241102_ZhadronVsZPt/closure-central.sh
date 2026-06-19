#!/bin/bash
# Signed common-CM convention: MC closure for note-facing Section 3.10.
# Runs 4 correction levels × 3 systems (pp, pPb, PbP) in the signed frame.
#
# Correction levels:
#   Gen             — gen-level, VZ weight only (no Z, no track residual)
#   Reco nominal    — reco, VZ weight only
#   Reco ZResidual  — reco, VZ + Z weight
#   Reco trkResidual— reco, VZ + Z + track residual weight
#
# Signed convention flags (matching central.sh):
#   --FillSigned true --DEtaRange 3.87 --ResultDEtaBins 12 --ResultDPhiBins 12
#   --MaxMixDeltaVZ 1.0 --nMix 10 --yBoost 0 --UseJackknife false
#
# Parallelism: Stream A (pp) runs in background, Stream B (pPb → PbP) in foreground.
# Usage: NTHREAD=30 ./closure-central.sh <DOPP> <DOPPB> <DOPBP> [extra args...]

set -euo pipefail

DOPP=$1
DOPPB=$2
DOPBP=$3
shift 3
COMMON_EXTRA_ARGS=("$@")

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

nMix=10
TAG_PP="_${OFFICIAL_TAG_PP}"
TAG_PPB="_${OFFICIAL_TAG_PPB}"

if [ -z "${CONFIG_FILE:-}" ]; then
    CONFIG=$(mktemp /tmp/kdeverea/closure_central_config_XXXXXX.sh)
    cat > "$CONFIG" <<'CFG'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
CFG
    export CONFIG_FILE="$CONFIG"
    CREATED_CONFIG=1
else
    CREATED_CONFIG=0
fi

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-30}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

SIGNED_COMMON=(
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --DEtaRange 3.87
    --FillSigned true
    --MaxMixDeltaVZ 1.0
    --nMix ${nMix}
    --UseJackknife false
)

run_pp() {
    local PP_MCGENINPUT=${PP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}
    local PP_MCRECOINPUT=${PP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PP}}

    local PP_COMMON=(
        --IsPP true --IsPPb false --IsData false
        --UseEventWeight true --UseTrackWeight true
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PP}
        --yBoost 0
        --TrackEtaMin -1.935 --TrackEtaMax 1.935
        --ZYSignedMin -1.935 --ZYSignedMax 1.935
        "${SIGNED_COMMON[@]}"
        "${COMMON_EXTRA_ARGS[@]}"
    )

    echo "=== pp Gen ==="
    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG_PP}" \
        --IsGenZ true \
        --Input ${PP_MCGENINPUT} --MixFile ${PP_MCGENINPUT} \
        --UseZWeight false --UseResidualWeight false \
        "${PP_COMMON[@]}"

    echo "=== pp Reco nominal ==="
    ./system-analysis.sh "pythiaMC_nominal${TAG_PP}" \
        --IsGenZ false \
        --Input ${PP_MCRECOINPUT} --MixFile ${PP_MCRECOINPUT} \
        --UseZWeight false --UseResidualWeight false \
        "${PP_COMMON[@]}"

    echo "=== pp Reco +Z ==="
    ./system-analysis.sh "pythiaMC_ZResidual${TAG_PP}" \
        --IsGenZ false \
        --Input ${PP_MCRECOINPUT} --MixFile ${PP_MCRECOINPUT} \
        --UseZWeight true --UseResidualWeight false \
        --ZWeightFile ${ZWeightFile_PP} \
        "${PP_COMMON[@]}"

    echo "=== pp Reco +Z+trk ==="
    ./system-analysis.sh "pythiaMC_trkResidual${TAG_PP}" \
        --IsGenZ false \
        --Input ${PP_MCRECOINPUT} --MixFile ${PP_MCRECOINPUT} \
        --UseZWeight true --UseResidualWeight true \
        --ZWeightFile ${ZWeightFile_PP} \
        --ResidualWeightFile ${RWeightFile_PP} \
        "${PP_COMMON[@]}"
}

run_pPb() {
    local PPB_MCGENINPUT=${PPB_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PPB}}
    local PPB_MCRECOINPUT=${PPB_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PPB}}
    local PPB_EPOSINPUT=${PPB_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PPB}}

    local PPB_COMMON=(
        --IsPP false --IsPPb true --IsData false
        --UseEventWeight true --UseTrackWeight true
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb}
        --yBoost 0
        --TrackEtaMin -1.470 --TrackEtaMax 2.400
        --ZYSignedMin -1.470 --ZYSignedMax 2.400
        "${SIGNED_COMMON[@]}"
        "${COMMON_EXTRA_ARGS[@]}"
    )

    echo "=== pPb Gen ==="
    ./system-analysis.sh "pPbMC_Gen_nominal${TAG_PPB}" \
        --IsGenZ true \
        --Input ${PPB_MCGENINPUT} --MixFile ${PPB_MCGENINPUT} \
        --UseZWeight false --UseResidualWeight false \
        --EPOSFile ${PPB_EPOSINPUT} --Fraction 1 \
        "${PPB_COMMON[@]}"

    echo "=== pPb Reco nominal ==="
    ./system-analysis.sh "pPbMC_nominal${TAG_PPB}" \
        --IsGenZ false \
        --Input ${PPB_MCRECOINPUT} --MixFile ${PPB_MCRECOINPUT} \
        --UseZWeight false --UseResidualWeight false \
        "${PPB_COMMON[@]}"

    echo "=== pPb Reco +Z ==="
    ./system-analysis.sh "pPbMC_ZResidual${TAG_PPB}" \
        --IsGenZ false \
        --Input ${PPB_MCRECOINPUT} --MixFile ${PPB_MCRECOINPUT} \
        --UseZWeight true --UseResidualWeight false \
        --ZWeightFile ${ZWeightFile_PPb} \
        "${PPB_COMMON[@]}"

    echo "=== pPb Reco +Z+trk ==="
    ./system-analysis.sh "pPbMC_trkResidual${TAG_PPB}" \
        --IsGenZ false \
        --Input ${PPB_MCRECOINPUT} --MixFile ${PPB_MCRECOINPUT} \
        --UseZWeight true --UseResidualWeight true \
        --ZWeightFile ${ZWeightFile_PPb} \
        --ResidualWeightFile ${RWeightFile_PPb} \
        "${PPB_COMMON[@]}"
}

run_PbP() {
    local PBP_MCGENINPUT=${PBP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PBP}}
    local PBP_MCRECOINPUT=${PBP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PBP}}
    local PBP_EPOSINPUT=${PBP_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PBP}}

    local PBP_COMMON=(
        --IsPP false --IsPPb false --IsData false
        --UseEventWeight true --UseTrackWeight true
        --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP}
        --yBoost 0
        --FlipDeltaEta true
        --TrackEtaMin -2.400 --TrackEtaMax 1.470
        --ZYSignedMin -2.400 --ZYSignedMax 1.470
        "${SIGNED_COMMON[@]}"
        "${COMMON_EXTRA_ARGS[@]}"
    )

    echo "=== PbP Gen ==="
    ./system-analysis.sh "PbPMC_Gen_nominal${TAG_PPB}" \
        --IsGenZ true \
        --Input ${PBP_MCGENINPUT} --MixFile ${PBP_MCGENINPUT} \
        --UseZWeight false --UseResidualWeight false \
        --EPOSFile ${PBP_EPOSINPUT} --Fraction 1 \
        "${PBP_COMMON[@]}"

    echo "=== PbP Reco nominal ==="
    ./system-analysis.sh "PbPMC_nominal${TAG_PPB}" \
        --IsGenZ false \
        --Input ${PBP_MCRECOINPUT} --MixFile ${PBP_MCRECOINPUT} \
        --UseZWeight false --UseResidualWeight false \
        "${PBP_COMMON[@]}"

    echo "=== PbP Reco +Z ==="
    ./system-analysis.sh "PbPMC_ZResidual${TAG_PPB}" \
        --IsGenZ false \
        --Input ${PBP_MCRECOINPUT} --MixFile ${PBP_MCRECOINPUT} \
        --UseZWeight true --UseResidualWeight false \
        --ZWeightFile ${ZWeightFile_PbP} \
        "${PBP_COMMON[@]}"

    echo "=== PbP Reco +Z+trk ==="
    ./system-analysis.sh "PbPMC_trkResidual${TAG_PPB}" \
        --IsGenZ false \
        --Input ${PBP_MCRECOINPUT} --MixFile ${PBP_MCRECOINPUT} \
        --UseZWeight true --UseResidualWeight true \
        --ZWeightFile ${ZWeightFile_PbP} \
        --ResidualWeightFile ${RWeightFile_PbP} \
        "${PBP_COMMON[@]}"
}

# --- Execute with 30-30 thread parallelism ---
# Stream A (background): pp
# Stream B (foreground): pPb → PbP

PP_PID=""

if [ "$DOPP" == "1" ]; then
    run_pp &
    PP_PID=$!
    echo "pp stream launched (PID ${PP_PID})"
fi

if [ "$DOPPB" == "1" ]; then
    run_pPb
fi

if [ "$DOPBP" == "1" ]; then
    run_PbP
fi

if [ -n "$PP_PID" ]; then
    echo "Waiting for pp stream (PID ${PP_PID})..."
    wait $PP_PID
    echo "pp stream complete."
fi

if [ "$CREATED_CONFIG" == "1" ]; then
    rm -f "$CONFIG"
fi

echo "=== Done: signed CM-frame MC closure ==="
