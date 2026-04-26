#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
REPO_ROOT=$(cd "$SCRIPT_DIR/../.." && pwd)

source "$REPO_ROOT/SetupAnalysis.sh"
source "$REPO_ROOT/OfficialWeightDictionary.sh"

cd "$SCRIPT_DIR"
make

OUTPUTBASE="${OUTPUTBASE:-$SCRIPT_DIR/output}"
SYSTEMS=(${SYSTEMS:-pp pPb PbP})

mkdir -p "$OUTPUTBASE"

declare -A DATA_FILES=(
   [pp]="$OFFICIAL_DATAINPUT_PP"
   [pPb]="$OFFICIAL_DATAINPUT_PPB"
   [PbP]="$OFFICIAL_DATAINPUT_PBP"
)
declare -A MC_FILES=(
   [pp]="$OFFICIAL_MCRECOINPUT_PP"
   [pPb]="$OFFICIAL_MCRECOINPUT_PPB"
   [PbP]="$OFFICIAL_MCRECOINPUT_PBP"
)

for system in "${SYSTEMS[@]}"; do
   case "$system" in
      pp|pPb|PbP)
         normalized_system="$system"
         ;;
      Pbp)
         normalized_system="PbP"
         ;;
      *)
         echo "Unsupported system: $system" >&2
         exit 1
         ;;
   esac

   data_file="${DATA_FILES[$normalized_system]}"
   mc_file="${MC_FILES[$normalized_system]}"

   for input_file in "$data_file" "$mc_file"; do
      if [ ! -f "$input_file" ]; then
         echo "Required merged input file is missing: $input_file" >&2
         exit 1
      fi
      echo "Using input file $input_file"
   done

   ./ExecuteHLTCurvePlot --Data "$data_file" --MC "$mc_file" \
      --Orientation "$normalized_system" --Output "$OUTPUTBASE/HLTEfficiency_${normalized_system}.pdf"
done

echo "Wrote HLT plots under $OUTPUTBASE"
