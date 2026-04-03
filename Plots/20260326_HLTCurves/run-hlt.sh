#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
REPO_ROOT=$(cd "$SCRIPT_DIR/../.." && pwd)

source "$REPO_ROOT/SetupAnalysis.sh"

cd "$SCRIPT_DIR"
make

INPUT_BASE="${INPUT_BASE:-/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.2}"
OUTPUTBASE="${OUTPUTBASE:-$SCRIPT_DIR/output/20260331_hlt_efficiency_study}"

mkdir -p "$OUTPUTBASE"

DATA_PPB_FILE="$INPUT_BASE/PPbData_Reco.root"
MC_PPB_FILE="$INPUT_BASE/PPbMC_Reco.root"
DATA_PBP_FILE="$INPUT_BASE/PbPData_Reco.root"
MC_PBP_FILE="$INPUT_BASE/PbPMC_Reco.root"

for input_file in "$DATA_PPB_FILE" "$MC_PPB_FILE" "$DATA_PBP_FILE" "$MC_PBP_FILE"; do
   if [ ! -f "$input_file" ]; then
      echo "Required merged input file is missing: $input_file" >&2
      exit 1
   fi
   echo "Using input file $input_file"
done

./ExecuteHLTCurvePlot --Data "$DATA_PPB_FILE" --MC "$MC_PPB_FILE" \
   --Orientation "pPb" --Output "$OUTPUTBASE/HLTEfficiency_pPb.pdf"
./ExecuteHLTCurvePlot --Data "$DATA_PBP_FILE" --MC "$MC_PBP_FILE" \
   --Orientation "Pbp" --Output "$OUTPUTBASE/HLTEfficiency_Pbp.pdf"

echo "Wrote HLT plots under $OUTPUTBASE"
