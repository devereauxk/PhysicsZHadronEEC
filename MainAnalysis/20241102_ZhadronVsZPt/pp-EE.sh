#!/bin/bash

DOPP=$1
DOPPB=$2
DOPBP=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

nMix=10

write_config() {
    local zpt_list="$1"
    local pt_list="$2"
    cat > config.sh <<EOF
ZPT_RANGES=(${zpt_list})
PT_RANGES=(${pt_list})
EOF
}

run_pp_energy_extrapolation() {
    local TAG="$1"

    ./system-analysis.sh "pp_nominal${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight false         --Input mergedSample/pp-v11-Zpt0.root         --MixFile mergedSample/pp-v11-Zpt0.root         --UseEventWeight false --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}

    ./system-analysis.sh "pp_ZResidual${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight false         --Input mergedSample/pp-v11-Zpt0.root         --MixFile mergedSample/pp-v11-Zpt0.root         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWeightFile_PP}"

    ./system-analysis.sh "pp_trkResidual${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight false         --Input mergedSample/pp-v11-Zpt0.root         --MixFile mergedSample/pp-v11-Zpt0.root         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWeightFile_PP}"         --ResidualWeightFile "${RWeightFile_PP}"

    ./system-analysis.sh "pp_EExtrapolation${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight false         --Input mergedSample/pp-v11-Zpt0.root         --MixFile mergedSample/pp-v11-Zpt0.root         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWeightFile_PP}"         --ResidualWeightFile "${RWeightFile_PP}"         --EnergyExtraFile "${EEWeightFile_PP}"
}

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

TAG="${TAG_SUFFIX:-_EEV3_ZV6_trkV24_nmix10}"

write_config '"5_30" "30_500"' '"0.5_4" "4_500"'
if [ "$DOPP" == "1" ]; then
    run_pp_energy_extrapolation "${TAG}"
fi

write_config '"5_500"' '"0.5_500"'
if [ "$DOPP" == "1" ]; then
    run_pp_energy_extrapolation "${TAG}"
fi
