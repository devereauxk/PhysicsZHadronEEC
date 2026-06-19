#!/bin/bash
# nPDF study: compare EPPS21 vs CT18ANLO vs NNPDF31 at 8.16 TeV gen-level.
# Uses signed convention (12x12, DEtaRange 3.87, pp acceptance |eta|<1.935).
set -euo pipefail

cd "$(dirname "$0")"
export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=${NTHREAD:-20}

GEN_DIR=$ProjectBase/SampleGeneration/20260403_PythiaMadgraph/output
TAG="nPDF_study"

SIGNED_ARGS=(
   --ResultDEtaBins 12 --ResultDPhiBins 12
   --DEtaRange 3.87
   --FillSigned true
   --MaxMixDeltaVZ 1.0
   --UseJackknife true
)

GEN_ARGS=(
   --IsPP true --IsGenZ true --IsData false
   --UseEventWeight true --UseZWeight false
   --UseTrackWeight true --UseResidualWeight false
   --UseVZWeight false
   --yBoost 0 --nMix 10
   --TrackEtaMin -1.935 --TrackEtaMax 1.935
   --ZYSignedMin -1.935 --ZYSignedMax 1.935
   --MinZPT 0 --MaxZPT 500
   --MinTrackPT 0.5 --MaxTrackPT 15
)

run_sample() {
   local label="$1"
   local input="$2"

   echo "=== Running $label ==="
   ./threader.sh "output/${label}_${TAG}_ZPT0_500" "0.5_15" \
      "${GEN_ARGS[@]}" "${SIGNED_ARGS[@]}" \
      --Input "$input" --MixFile "$input"

   cp "output/${label}_${TAG}_ZPT0_500-0.5_15-result.root" \
      "plots/${label}_${TAG}_ZPT0_500-result.root"
   cp "output/${label}_${TAG}_ZPT0_500-0.5_15-nosub.root" \
      "plots/${label}_${TAG}_ZPT0_500-nosub.root"
   echo "=== $label done ==="
}

mkdir -p output plots

run_sample "nPDF_epps21"  "$GEN_DIR/ZMuMu_8160_epps21.root"
run_sample "nPDF_ct18anlo" "$GEN_DIR/ZMuMu_8160_ct18anlo.root"
run_sample "nPDF_nnpdf31" "$GEN_DIR/ZMuMu_8160_nnlo.root"

echo "=== All samples done ==="
echo "Output in plots/nPDF_{epps21,ct18anlo,nnpdf31}_${TAG}_ZPT0_500-{result,nosub}.root"
