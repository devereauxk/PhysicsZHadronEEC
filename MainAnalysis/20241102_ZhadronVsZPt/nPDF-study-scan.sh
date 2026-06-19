#!/bin/bash
# nPDF study scan bins: ZPT {0-30, 30-500} x trkPT {0.5-2, 2-4, 4-15}
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
)

ZPT_BINS=("0 30" "30 500")
TRK_BINS=("0.5 2" "2 4" "4 15")

run_sample() {
   local label="$1"
   local input="$2"
   local minZPT="$3"
   local maxZPT="$4"
   local minTrkPT="$5"
   local maxTrkPT="$6"
   local trkTag="${minTrkPT}_${maxTrkPT}"

   echo "=== $label ZPT${minZPT}_${maxZPT} trkPT${trkTag} ==="
   ./threader.sh "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}" "${trkTag}" \
      "${GEN_ARGS[@]}" "${SIGNED_ARGS[@]}" \
      --Input "$input" --MixFile "$input" \
      --MinZPT "$minZPT" --MaxZPT "$maxZPT" \
      --MinTrackPT "$minTrkPT" --MaxTrackPT "$maxTrkPT"

   cp "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-${trkTag}-result.root" \
      "plots/${label}_${TAG}_ZPT${minZPT}_${maxZPT}_trkPT${trkTag}-result.root"
   cp "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-${trkTag}-nosub.root" \
      "plots/${label}_${TAG}_ZPT${minZPT}_${maxZPT}_trkPT${trkTag}-nosub.root"
}

mkdir -p output plots

for zpt in "${ZPT_BINS[@]}"; do
   read -r minZ maxZ <<< "$zpt"
   for trk in "${TRK_BINS[@]}"; do
      read -r minT maxT <<< "$trk"
      run_sample "nPDF_epps21"   "$GEN_DIR/ZMuMu_8160_epps21.root"   "$minZ" "$maxZ" "$minT" "$maxT"
      run_sample "nPDF_ct18anlo" "$GEN_DIR/ZMuMu_8160_ct18anlo.root" "$minZ" "$maxZ" "$minT" "$maxT"
      run_sample "nPDF_nnpdf31"  "$GEN_DIR/ZMuMu_8160_nnlo.root"     "$minZ" "$maxZ" "$minT" "$maxT"
   done
done

echo "=== All scan bins done ==="
