#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
REPOROOT=/home/kdeverea/PhysicsZHadronEEC
OFFICIAL_ANALYSIS_DIR=${REPOROOT}/MainAnalysis/20241102_ZhadronVsZPt/plots

source ${REPOROOT}/OfficialWeightDictionary.sh

cd "$REPOROOT"
source SetupAnalysis.sh
cd "$THISDIR"

make Execute2DResidualComparisonPlot

PP_TAG=${PP_TAG_OVERRIDE:-${OFFICIAL_TAG_PP}_2DResidual_20260518}
PPB_TAG=${PPB_TAG_OVERRIDE:-${OFFICIAL_TAG_PPB}_2DResidual_20260518}
ZPT=0_500
TRACK=0.5_15

mkdir -p "$THISDIR/plots/2DResidualComparison"

# pp (energy extrapolated)
./Execute2DResidualComparisonPlot \
   --Nominal "$OFFICIAL_ANALYSIS_DIR/pp_trkResidual_${OFFICIAL_TAG_PP}_ZPT${ZPT}-result.root" \
   --Variation "$THISDIR/plots/pp_trkResidual_${PP_TAG}_ZPT${ZPT}-result.root" \
   --OutputBase "$THISDIR/plots/2DResidualComparison/pp_ZPT${ZPT}_trkPT${TRACK}" \
   --Collision pp \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"

# pPb
./Execute2DResidualComparisonPlot \
   --Nominal "$OFFICIAL_ANALYSIS_DIR/pPb_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-result.root" \
   --Variation "$THISDIR/plots/pPb_trkResidual_${PPB_TAG}_ZPT${ZPT}-result.root" \
   --OutputBase "$THISDIR/plots/2DResidualComparison/pPb_ZPT${ZPT}_trkPT${TRACK}" \
   --Collision pPb \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"

# PbP
./Execute2DResidualComparisonPlot \
   --Nominal "$OFFICIAL_ANALYSIS_DIR/PbP_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-result.root" \
   --Variation "$THISDIR/plots/PbP_trkResidual_${PPB_TAG}_ZPT${ZPT}-result.root" \
   --OutputBase "$THISDIR/plots/2DResidualComparison/PbP_ZPT${ZPT}_trkPT${TRACK}" \
   --Collision PbP \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"

# pPbPbp combined
./Execute2DResidualComparisonPlot \
   --NominalPPb "$OFFICIAL_ANALYSIS_DIR/pPb_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-nosub.root" \
   --NominalPBP "$OFFICIAL_ANALYSIS_DIR/PbP_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-nosub.root" \
   --VariationPPb "$THISDIR/plots/pPb_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
   --VariationPBP "$THISDIR/plots/PbP_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
   --OutputBase "$THISDIR/plots/2DResidualComparison/pPbPbp_ZPT${ZPT}_trkPT${TRACK}" \
   --Collision pPbPbp \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"

echo "Comparison plots written to $THISDIR/plots/2DResidualComparison/"
