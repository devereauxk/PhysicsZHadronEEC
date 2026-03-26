#!/bin/bash

set -euo pipefail

cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260326_HLTCurves

make ExecuteHLTCurvePlot

OUTPUTBASE=${OUTPUTBASE:-$PWD/output/hlt_efficiency_study}
NTHREAD=${NTHREAD:-5}
MAX_FILES=${MAX_FILES:-20}
INPUT_FILE=${INPUT_FILE:-}
SKIMDIR=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA
SKIMOUT="$OUTPUTBASE/skims/PAData"

rm -rf "$OUTPUTBASE"
mkdir -p "$SKIMOUT"

wait_for_slot() {
   while [[ "$(jobs -rp | wc -l)" -ge "$NTHREAD" ]]
   do
      sleep 1
   done
}

if [[ -n "$INPUT_FILE" ]]; then
   FILES=("$INPUT_FILE")
else
   FILES=("$SKIMDIR"/Samples/PAData/000*/HiForestAOD_*.root)
fi

COUNT=0
for i in "${FILES[@]}"
do
   [[ -e "$i" ]] || continue
   if [[ "$MAX_FILES" -gt 0 && "$COUNT" -ge "$MAX_FILES" ]]; then
      break
   fi
   wait_for_slot
   (
      cd "$SKIMDIR"
      ./local_skim.sh PAData8TeV "$i" "$SKIMOUT/Reco$(basename "$i")" Dummy
   ) &
   COUNT=$((COUNT + 1))
done

wait

hadd -f "$OUTPUTBASE/merged_AP.root" "$SKIMOUT"/*_AP.root
hadd -f "$OUTPUTBASE/merged_PA.root" "$SKIMOUT"/*_PA.root
hadd -f "$OUTPUTBASE/merged_all.root" "$SKIMOUT"/*_AP.root "$SKIMOUT"/*_PA.root

./ExecuteHLTCurvePlot --Input "$OUTPUTBASE/merged_all.root" --Output "$OUTPUTBASE/HLTEfficiency.pdf"
./ExecuteHLTCurvePlot --Input "$OUTPUTBASE/merged_AP.root" --Output "$OUTPUTBASE/HLTEfficiency_AP.pdf"
./ExecuteHLTCurvePlot --Input "$OUTPUTBASE/merged_PA.root" --Output "$OUTPUTBASE/HLTEfficiency_PA.pdf"

echo "Wrote skim outputs and plots under $OUTPUTBASE"
