#!/bin/bash

# Produce pp inputs consumed by Plots/20260213_Central/plot_pp.cpp
# for ZPT40_350 and trkPT {1_2,2_4,4_10}.

set -euo pipefail

nMix=10
VZ_FILE="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260317_ZV6_ZPT0_350_VzReweightFits_pp.root"
Z_FILE="my_ZWeights/20260317_ZCorrection_V6_pp_zPt0-500.root"
R_FILE="my_residualWeights/20260317_TrackResidualCorrection_V24_ZWeight_V6_pp_zPt"

CONFIG_FILE_LOCAL="$(mktemp -p . config_plotpp_XXXX.sh)"
trap 'rm -f "${CONFIG_FILE_LOCAL}"' EXIT
cat > "${CONFIG_FILE_LOCAL}" <<EOF
ZPT_RANGES=("40_350")
PT_RANGES=("1_2" "2_4" "4_10")
EOF

if [ "${SKIP_CLEAN:-1}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM="${CUT_PARALLELISM:-1}"
export NTHREAD="${NTHREAD:-8}"
export NSLICE_FACTOR="${NSLICE_FACTOR:-1}"
export CONFIG_FILE="${CONFIG_FILE_LOCAL}"

run_one_tag() {
    local TAG="$1"
    local USE_EVENT_WEIGHT="$2"

    ./system-analysis.sh "pythiaMC_Gen_nominal_${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --VZWeightFile "${VZ_FILE}" \
        --yBoost 0 --nMix "${nMix}"

    ./system-analysis.sh "pp_nominal_${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --VZWeightFile "${VZ_FILE}" \
        --yBoost 0 --nMix "${nMix}"

    ./system-analysis.sh "pp_ZResidual_${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --VZWeightFile "${VZ_FILE}" \
        --ZWeightFile "${Z_FILE}" \
        --yBoost 0 --nMix "${nMix}"

    ./system-analysis.sh "pp_trkResidual_${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight "${USE_EVENT_WEIGHT}" --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --VZWeightFile "${VZ_FILE}" \
        --ZWeightFile "${Z_FILE}" \
        --ResidualWeightFile "${R_FILE}" \
        --yBoost 0 --nMix "${nMix}"
}

run_one_tag "evtWeightOn_ZV6_trkV24_nmix10" true
run_one_tag "evtWeightOff_ZV6_trkV24_nmix10" false
