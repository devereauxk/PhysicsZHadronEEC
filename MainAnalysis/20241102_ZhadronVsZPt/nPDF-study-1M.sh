#!/bin/bash
# nPDF study 1M: inclusive + scan bins
# Signed convention, NTHREAD=15 max
set -euo pipefail

cd "$(dirname "$0")"
export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=${NTHREAD:-15}

GEN_DIR=/data00/kdeverea/pythiaMG
TAG="nPDF_study_1M"

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

run_sample() {
   local label="$1"
   local input="$2"
   local minZPT="$3"
   local maxZPT="$4"
   local minTrkPT="$5"
   local maxTrkPT="$6"
   local trkTag="${minTrkPT}_${maxTrkPT}"
   local isInclusive="$7"

   echo "=== $label ZPT${minZPT}_${maxZPT} trkPT${trkTag} ==="
   ./threader.sh "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}" "${trkTag}" \
      "${GEN_ARGS[@]}" "${SIGNED_ARGS[@]}" \
      --Input "$input" --MixFile "$input" \
      --MinZPT "$minZPT" --MaxZPT "$maxZPT" \
      --MinTrackPT "$minTrkPT" --MaxTrackPT "$maxTrkPT"

   if [[ "$isInclusive" == "true" ]]; then
      cp "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-${trkTag}-result.root" \
         "plots/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-result.root"
      cp "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-${trkTag}-nosub.root" \
         "plots/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-nosub.root"
   else
      cp "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-${trkTag}-result.root" \
         "plots/${label}_${TAG}_ZPT${minZPT}_${maxZPT}_trkPT${trkTag}-result.root"
      cp "output/${label}_${TAG}_ZPT${minZPT}_${maxZPT}-${trkTag}-nosub.root" \
         "plots/${label}_${TAG}_ZPT${minZPT}_${maxZPT}_trkPT${trkTag}-nosub.root"
   fi
}

mkdir -p output plots

PDFS=("nPDF_epps21" "nPDF_ct18anlo" "nPDF_nnpdf31")
INPUTS=(
   "$GEN_DIR/ZMuMu_8160_epps21_1M.root"
   "$GEN_DIR/ZMuMu_8160_ct18anlo_1M.root"
   "$GEN_DIR/ZMuMu_8160_nnpdf31_1M.root"
)

# Inclusive: ZPT 0_500, trkPT 0.5_15
for i in 0 1 2; do
   run_sample "${PDFS[$i]}" "${INPUTS[$i]}" 0 500 0.5 15 true
done

# Scan: ZPT {0-30, 30-500} x trkPT {0.5-2, 2-4, 4-15}
ZPT_BINS=("0 30" "30 500")
TRK_BINS=("0.5 2" "2 4" "4 15")

for zpt in "${ZPT_BINS[@]}"; do
   read -r minZ maxZ <<< "$zpt"
   for trk in "${TRK_BINS[@]}"; do
      read -r minT maxT <<< "$trk"
      for i in 0 1 2; do
         run_sample "${PDFS[$i]}" "${INPUTS[$i]}" "$minZ" "$maxZ" "$minT" "$maxT" false
      done
   done
done

echo "=== All nPDF 1M analysis done ==="
