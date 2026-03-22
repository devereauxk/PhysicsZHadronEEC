#!/bin/bash

# Produce pp inputs consumed by Plots/20260213_Central/plot_pp.cpp
# for the requested ZPT40_350 x {1_2,2_4,4_10} and
# ZPT{20_40,40_60,60_500} x {2_500} grids.

set -euo pipefail

nMix=10
VZ_FILE="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root"
Z_FILE="my_ZWeights/20260320_ZCorrection_V6_skimVZOff_pp_zPt0-500.root"
R_FILE="my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt"
TEMP_CONFIGS=()
cleanup_configs() {
    rm -f "${TEMP_CONFIGS[@]}"
}
trap cleanup_configs EXIT

if [ "${SKIP_CLEAN:-1}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM="${CUT_PARALLELISM:-1}"
export NTHREAD="${NTHREAD:-8}"
export NSLICE_FACTOR="${NSLICE_FACTOR:-1}"

write_config() {
    local zpt_ranges="$1"
    local pt_ranges="$2"
    local config_file

    config_file="$(mktemp -p . config_plotpp_XXXX.sh)"
    TEMP_CONFIGS+=("${config_file}")

    cat > "${config_file}" <<EOF
ZPT_RANGES=(${zpt_ranges})
PT_RANGES=(${pt_ranges})
EOF

    echo "${config_file}"
}

run_one_tag() {
    local TAG="$1"
    local USE_EVENT_WEIGHT="$2"

    ./system-analysis.sh "pythiaMC_nominal_${TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input mergedSample/pythia-v11-Zpt0.root \
        --MixFile mergedSample/pythia-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --UseVZWeight true --VZWeightFile "${VZ_FILE}" \
        --yBoost 0 --nMix "${nMix}"

    ./system-analysis.sh "pythiaMC_trkResidual_${TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input mergedSample/pythia-v11-Zpt0.root \
        --MixFile mergedSample/pythia-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --UseVZWeight true --VZWeightFile "${VZ_FILE}" \
        --ZWeightFile "${Z_FILE}" \
        --ResidualWeightFile "${R_FILE}" \
        --yBoost 0 --nMix "${nMix}"

    ./system-analysis.sh "pythiaMC_Gen_nominal_${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --UseVZWeight true --VZWeightFile "${VZ_FILE}" \
        --yBoost 0 --nMix "${nMix}"

    ./system-analysis.sh "pp_nominal_${TAG}" \
        --IsPP true --IsGenZ false --IsData true --UseVZWeight false \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix "${nMix}"

    ./system-analysis.sh "pp_trkResidual_${TAG}" \
        --IsPP true --IsGenZ false --IsData true --UseVZWeight false \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --ZWeightFile "${Z_FILE}" \
        --ResidualWeightFile "${R_FILE}" \
        --yBoost 0 --nMix "${nMix}"
}

CONFIG_FILE="$(write_config '"40_350"' '"1_2" "2_4" "4_10"')"
export CONFIG_FILE
run_one_tag "ZV6_trkV24_vz20260320_nmix10" true

CONFIG_FILE="$(write_config '"20_40" "40_60" "60_500"' '"2_500"')"
export CONFIG_FILE
run_one_tag "ZV6_trkV24_vz20260320_nmix10" true

#run_one_tag "evtWeightOff_ZV6_trkV24_vz20260320_nmix10" false
