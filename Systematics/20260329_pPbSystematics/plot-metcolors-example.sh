#!/bin/bash
set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"

mkdir -p plots

INPUT="output/pp_${OFFICIAL_TAG_PP}_ZPT0_500_trkPT0.5_15-systematics.root"
OUTPUT="plots/metcolors_austria_example.pdf"

./ExecutePlotMetColorsExample \
   --Input  "$INPUT" \
   --Output "$OUTPUT" \
   --Collision pp \
   --ZPT 0_500 \
   --Trk 0.5_15

echo "Done: $OUTPUT"
