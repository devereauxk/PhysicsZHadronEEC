#!/bin/bash
# Study 4: pPb and Pbp MC reco fully corrected, Z-count-matched to data.
# Fractions computed dynamically from hNZData_0.5_15 in:
#   data: pPb/PbP_trkResidual_<TAG>_12x12_full_ZPT0_500-result.root  (from halfsplit runner)
#   MC:   pPbMC/PbPMC_trkResidual_<TAG>_12x12_fullstats_ZPT0_500-result.root  (from mc-fullstats runner)
# Run halfsplit and mc-fullstats runners first.
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

# Compute fractions dynamically from ZV10 result files
get_nz() {
    root -l -b -q -e "TFile f(\"$1\"); TH1D *h=(TH1D*)f.Get(\"hNZData_0.5_15\"); if(h) cout<<h->GetBinContent(1)<<endl; else cout<<0<<endl;" 2>/dev/null | tail -1
}

NZ_DATA_PPB=$(get_nz "${MAIN}/plots/pPb_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root")
NZ_MC_PPB=$(get_nz "${MAIN}/plots/pPbMC_trkResidual_${TAG}_12x12_fullstats_ZPT0_500-result.root")
NZ_DATA_PBP=$(get_nz "${MAIN}/plots/PbP_trkResidual_${TAG}_12x12_full_ZPT0_500-result.root")
NZ_MC_PBP=$(get_nz "${MAIN}/plots/PbPMC_trkResidual_${TAG}_12x12_fullstats_ZPT0_500-result.root")

echo "NZ data pPb=${NZ_DATA_PPB}  MC pPb=${NZ_MC_PPB}"
echo "NZ data PbP=${NZ_DATA_PBP}  MC PbP=${NZ_MC_PBP}"

FRAC_PPB=$(python3 -c "print('{:.6f}'.format(${NZ_DATA_PPB}/${NZ_MC_PPB}))")
FRAC_PBP=$(python3 -c "print('{:.6f}'.format(${NZ_DATA_PBP}/${NZ_MC_PBP}))")

echo "Fractions: pPb=${FRAC_PPB}  PbP=${FRAC_PBP}"

COMMON=(
    --IsPP false --IsGenZ false --IsData false
    --UseEventWeight true --UseZWeight true
    --UseTrackWeight true --UseResidualWeight true
    --UseVZWeight true
    --yBoost 0 --nMix 10
    --ResultDEtaBins 12 --ResultDPhiBins 12
    --MaxMixDeltaVZ 1.0
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
