#!/bin/bash
# Study 4: pPb and Pbp MC reco fully corrected, Z-count-matched to data.
# Fractions computed from:
#   data NZ  pPb=10323.5, PbP=5942.57  (from pPb/PbP_trkResidual_<TAG>_ZPT0_500-result.root)
#   MC NZ    pPb=302326,  PbP=303036   (summed over ZPT bins from existing MC reco result files)
# Produces 2 result files in MainAnalysis/20241102_ZhadronVsZPt/plots/.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAIN=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
TAG="${OFFICIAL_TAG_PPB}"

cd "$MAIN"
export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-20}
export CUT_PARALLELISM=1

CONFIG=$(mktemp /tmp/kdeverea/12x12_config_XXXXXX.sh)
cat > "$CONFIG" <<'EOF'
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")
EOF
export CONFIG_FILE="$CONFIG"

# Fractions to match data Z-event statistics
FRAC_PPB="0.034150"   # pPb  data NZ 10323.5 / MC NZ 302326
FRAC_PBP="0.019611"   # PbP  data NZ 5942.57 / MC NZ 303036

COMMON=(
    --IsPP false --IsGenZ false --IsData false
    --UseEventWeight true --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --UseVZWeight true
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --MaxMixDeltaVZ 0.5
)

echo "=== pPb MC reco (Fraction=${FRAC_PPB}) ==="
./system-analysis.sh "pPbMC_trkResidual_${TAG}_12x12_matched" \
    "${COMMON[@]}" \
    --IsPPb true \
    --Input "$OFFICIAL_MCRECOINPUT_PPB" --MixFile "$OFFICIAL_MCRECOINPUT_PPB" \
    --ZWeightFile "$ZWeightFile_PPb" \
    --ResidualWeightFile "$RWeightFile_PPb" \
    --VZWeightFile "$VZWeightFile_PPb" \
    --Fraction "$FRAC_PPB"

echo "=== Pbp MC reco (Fraction=${FRAC_PBP}) ==="
./system-analysis.sh "PbPMC_trkResidual_${TAG}_12x12_matched" \
    "${COMMON[@]}" \
    --IsPPb false \
    --Input "$OFFICIAL_MCRECOINPUT_PBP" --MixFile "$OFFICIAL_MCRECOINPUT_PBP" \
    --ZWeightFile "$ZWeightFile_PbP" \
    --ResidualWeightFile "$RWeightFile_PbP" \
    --VZWeightFile "$VZWeightFile_PbP" \
    --Fraction "$FRAC_PBP"

rm -f "$CONFIG"
echo "=== Done: MC matched analysis complete ==="
