#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
REPOROOT=/home/kdeverea/PhysicsZHadronEEC
OFFICIAL_ANALYSIS_DIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$REPOROOT"
source SetupAnalysis.sh
cd "$THISDIR"

make ExecutePionComparisonPlot

PP_TAG=${PP_TAG_OVERRIDE:-${OFFICIAL_TAG_PP}_pionTrackY_20260511}
PPB_TAG=${PPB_TAG_OVERRIDE:-${OFFICIAL_TAG_PPB}_pionTrackY_20260511}
ZPT=0_500
TRACK=0.5_15

mkdir -p "$THISDIR/plots/pionTrackStudy"

./ExecutePionComparisonPlot \
   --Nominal "$OFFICIAL_ANALYSIS_DIR/pp_trkResidual_${OFFICIAL_TAG_PP}_ZPT${ZPT}-result.root" \
   --Variation "$THISDIR/plots/pp_trkResidual_${PP_TAG}_ZPT${ZPT}-result.root" \
   --OutputBase "$THISDIR/plots/pionTrackStudy/pp_${PP_TAG}_ZPT${ZPT}_trkPT${TRACK}-PionTrackY" \
   --Collision pp \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"

./ExecutePionComparisonPlot \
   --Nominal "$OFFICIAL_ANALYSIS_DIR/pPb_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-result.root" \
   --Variation "$THISDIR/plots/pPb_trkResidual_${PPB_TAG}_ZPT${ZPT}-result.root" \
   --OutputBase "$THISDIR/plots/pionTrackStudy/pPb_${PPB_TAG}_ZPT${ZPT}_trkPT${TRACK}-PionTrackY" \
   --Collision pPb \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"

./ExecutePionComparisonPlot \
   --Nominal "$OFFICIAL_ANALYSIS_DIR/PbP_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-result.root" \
   --Variation "$THISDIR/plots/PbP_trkResidual_${PPB_TAG}_ZPT${ZPT}-result.root" \
   --OutputBase "$THISDIR/plots/pionTrackStudy/PbP_${PPB_TAG}_ZPT${ZPT}_trkPT${TRACK}-PionTrackY" \
   --Collision PbP \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"

./ExecutePionComparisonPlot \
   --NominalPPb "$OFFICIAL_ANALYSIS_DIR/pPb_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-nosub.root" \
   --NominalPBP "$OFFICIAL_ANALYSIS_DIR/PbP_trkResidual_${OFFICIAL_TAG_PPB}_ZPT${ZPT}-nosub.root" \
   --VariationPPb "$THISDIR/plots/pPb_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
   --VariationPBP "$THISDIR/plots/PbP_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
   --OutputBase "$THISDIR/plots/pionTrackStudy/pPbPbp_${PPB_TAG}_ZPT${ZPT}_trkPT${TRACK}-PionTrackY" \
   --Collision pPbPbp \
   --ZPTRange "$ZPT" \
   --TrackPTRange "$TRACK"
