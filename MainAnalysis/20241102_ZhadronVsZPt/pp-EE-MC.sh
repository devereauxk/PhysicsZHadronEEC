#!/bin/bash
# Run pp MC gen and reco with energy extrapolation weight.
# SAFE: reads PPMC_{Gen,Reco}.root, no conflict with ongoing PPData_Reco runs.
# DOES NOT recompile anything.

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=${NTHREAD:-30}

TAG="_${OFFICIAL_TAG_PP}"

CONFIG=$(mktemp /tmp/kdeverea/ee_mc_config_XXXXXX.sh)
cat > "$CONFIG" <<'EOF'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
EOF
export CONFIG_FILE="$CONFIG"

SIGNED_FLAGS=(
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --DEtaRange 3.87
    --FillSigned true
    --MaxMixDeltaVZ 1.0
    --TrackEtaMin -1.935 --TrackEtaMax 1.935
    --ZYSignedMin -1.935 --ZYSignedMax 1.935
)

echo "=== Gen MC with EE weight ==="
./system-analysis.sh "pythiaMC_Gen_EExtrapolation${TAG}" \
    --IsPP true --IsGenZ true --IsData false \
    --Input "${OFFICIAL_MCGENINPUT_PP}" \
    --MixFile "${OFFICIAL_MCGENINPUT_PP}" \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --UseVZWeight true --VZWeightFile "${VZWeightFile_PP}" \
    --EnergyExtraFile "${EEWeightFile_PP}" \
    --yBoost 0 --nMix 10 \
    "${SIGNED_FLAGS[@]}"

echo "=== Reco MC fully corrected with EE weight ==="
./system-analysis.sh "pythiaMC_trkResidual_EExtrapolation${TAG}" \
    --IsPP true --IsGenZ false --IsData false \
    --Input "${OFFICIAL_MCRECOINPUT_PP}" \
    --MixFile "${OFFICIAL_MCRECOINPUT_PP}" \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --UseVZWeight true --VZWeightFile "${VZWeightFile_PP}" \
    --ZWeightFile "${ZWeightFile_PP}" \
    --ResidualWeightFile "${RWeightFile_PP}" \
    --EnergyExtraFile "${EEWeightFile_PP}" \
    --yBoost 0 --nMix 10 \
    "${SIGNED_FLAGS[@]}"

rm -f "$CONFIG"
echo "=== Done: pp MC EE variants ==="
