#!/bin/bash
set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
REPOROOT=/home/kdeverea/PhysicsZHadronEEC
CENTRAL_DIR=${REPOROOT}/MainAnalysis/20241102_ZhadronVsZPt/plots

source "${REPOROOT}/OfficialWeightDictionary.sh"

cd "$REPOROOT"
source SetupAnalysis.sh
cd "$THISDIR"

make ExecuteNmixComparisonPlot

ZPT=0_500
TRACK=0.5_15

mkdir -p "$THISDIR/plots/nmixStudy"

./ExecuteNmixComparisonPlot \
   --StudyDir  "$THISDIR/plots" \
   --CentralDir "$CENTRAL_DIR" \
   --OutputDir  "$THISDIR/plots/nmixStudy" \
   --ZPTRange   "$ZPT" \
   --TrackPTRange "$TRACK" \
   --PPTagBase  "${OFFICIAL_TAG_PP/nmix10/nmix}" \
   --PPBTagBase "${OFFICIAL_TAG_PPB/nmix10/nmix}"
