#!/bin/bash
# Run pooled JK combination + 2D symmetrization for signed common-CM results.
# Reads pPb/Pbp/pp production outputs and raw files (with Jackknife2DData).
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

TAG_PP="${OFFICIAL_TAG_PP}"
TAG_PPB="${OFFICIAL_TAG_PPB}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANALYSIS_DIR="$(cd "${SCRIPT_DIR}/../20241102_ZhadronVsZPt" && pwd)"

PPB_NOSUB="${ANALYSIS_DIR}/output/pPb_trkResidual_${TAG_PPB}_ZPT0_500-0.5_15-nosub.root"
PBP_NOSUB="${ANALYSIS_DIR}/output/PbP_trkResidual_${TAG_PPB}_ZPT0_500-0.5_15-nosub.root"
PP_NOSUB="${ANALYSIS_DIR}/output/pp_trkResidual_${TAG_PP}_ZPT0_500-0.5_15-nosub.root"

PPB_RAW="${ANALYSIS_DIR}/output/pPb_trkResidual_${TAG_PPB}_ZPT0_500-0.5_15.root"
PBP_RAW="${ANALYSIS_DIR}/output/PbP_trkResidual_${TAG_PPB}_ZPT0_500-0.5_15.root"
PP_RAW="${ANALYSIS_DIR}/output/pp_trkResidual_${TAG_PP}_ZPT0_500-0.5_15.root"

OUT_DIR="plots/pooled_symmetrized"
TAG="pooled_signed_sym"

for f in "$PPB_NOSUB" "$PBP_NOSUB" "$PP_NOSUB" "$PPB_RAW" "$PBP_RAW" "$PP_RAW"; do
    [ -f "$f" ] || { echo "Missing: $f"; exit 1; }
done

./ExecutePlot12x12PooledSymmetrized \
    --pPbFile "$PPB_NOSUB" --PbPFile "$PBP_NOSUB" --ppFile "$PP_NOSUB" \
    --pPbRawFile "$PPB_RAW" --PbPRawFile "$PBP_RAW" --ppRawFile "$PP_RAW" \
    --OutputDir "$OUT_DIR" --Tag "$TAG"

echo "=== Pooled symmetrized outputs in ${OUT_DIR}/${TAG}-*.{pdf,root,tex} ==="
