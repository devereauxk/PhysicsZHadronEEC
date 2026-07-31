#!/bin/bash
# Theory prediction gen-level curves for result plots.
# pp: Pythia8+MadGraph gen (5.02 TeV), EE-extrapolated to 8.16 TeV
# pPb/Pbp: Powheg+EPOS gen (8.16 TeV), combined at plot stage
#
# Usage: NTHREAD=25 ./theory.sh <DOPP> <DOPPB> <DOPBP>
#   DOSCAN=1 ./theory.sh <DOPP> <DOPPB> <DOPBP>  # scan track pT bins

set -euo pipefail

DOPP=$1
DOPPB=$2
DOPBP=$3
DOSCAN=${DOSCAN:-0}

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-25}

if [ "$DOSCAN" == "1" ]; then
    ZPT_OVERRIDE="${ZPT_OVERRIDE:-0_500 0_30 30_500}"
else
    ZPT_OVERRIDE="${ZPT_OVERRIDE:-0_500}"
fi
CONFIG=$(mktemp /tmp/kdeverea/theory_config_XXXXXX.sh)
if [ "$DOSCAN" == "1" ]; then
    echo "ZPT_RANGES=(${ZPT_OVERRIDE})" > "$CONFIG"
    echo 'PT_RANGES=("0.5_2" "2_4" "4_15")' >> "$CONFIG"
else
    echo "ZPT_RANGES=(${ZPT_OVERRIDE})" > "$CONFIG"
    echo 'PT_RANGES=("0.5_15")' >> "$CONFIG"
fi
export CONFIG_FILE="$CONFIG"

if [ "$DOSCAN" == "1" ]; then
    TAG_PP="_${OFFICIAL_TAG_PP}_scan"
    TAG_PPB="_${OFFICIAL_TAG_PPB}_scan"
else
    TAG_PP="_${OFFICIAL_TAG_PP}"
    TAG_PPB="_${OFFICIAL_TAG_PPB}"
fi

SIGNED_COMMON=(
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --DEtaRange 3.87
    --FillSigned true
    --MaxMixDeltaVZ 1.0
    --nMix 10
)

GEN_COMMON=(
    --IsGenZ true --IsData false
    --UseEventWeight true --UseZWeight false
    --UseTrackWeight true --UseResidualWeight false
    --yBoost 0
)

if [ "$DOPP" == "1" ]; then
    echo "=== pp Pythia8+MadGraph gen + EE extrapolation ==="
    ./system-analysis.sh "pythiaMC_Gen_EExtrapolation${TAG_PP}" \
        --IsPP true \
        --Input "${OFFICIAL_MCGENINPUT_PP}" \
        --MixFile "${OFFICIAL_MCGENINPUT_PP}" \
        --UseVZWeight true --VZWeightFile "${VZWeightFile_PP}" \
        --EnergyExtraFile "${EEWeightFile_PP}" \
        --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
        --ZYSignedMin -1.935 --ZYSignedMax 1.935 \
        "${GEN_COMMON[@]}" \
        "${SIGNED_COMMON[@]}"
fi

if [ "$DOPPB" == "1" ]; then
    echo "=== pPb Powheg+EPOS gen ==="
    ./system-analysis.sh "pPbMC_Gen_nominal${TAG_PPB}" \
        --IsPP false --IsPPb true \
        --Input "${OFFICIAL_MCGENINPUT_PPB}" \
        --MixFile "${OFFICIAL_MCGENINPUT_PPB}" \
        --EPOSFile "${OFFICIAL_EPOSINPUT_PPB}" --Fraction 1 \
        --UseVZWeight true --VZWeightFile "${VZWeightFile_PPb}" \
        --TrackEtaMin -1.470 --TrackEtaMax 2.400 \
        --ZYSignedMin -1.470 --ZYSignedMax 2.400 \
        "${GEN_COMMON[@]}" \
        "${SIGNED_COMMON[@]}"
fi

if [ "$DOPBP" == "1" ]; then
    echo "=== Pbp Powheg+EPOS gen (FlipDeltaEta) ==="
    ./system-analysis.sh "PbPMC_Gen_nominal${TAG_PPB}" \
        --IsPP false --IsPPb false \
        --FlipDeltaEta true \
        --Input "${OFFICIAL_MCGENINPUT_PBP}" \
        --MixFile "${OFFICIAL_MCGENINPUT_PBP}" \
        --EPOSFile "${OFFICIAL_EPOSINPUT_PBP}" --Fraction 1 \
        --UseVZWeight true --VZWeightFile "${VZWeightFile_PbP}" \
        --TrackEtaMin -2.400 --TrackEtaMax 1.470 \
        --ZYSignedMin -2.400 --ZYSignedMax 1.470 \
        "${GEN_COMMON[@]}" \
        "${SIGNED_COMMON[@]}"
fi

rm -f "$CONFIG"
echo "=== Done: theory gen-level production ==="
