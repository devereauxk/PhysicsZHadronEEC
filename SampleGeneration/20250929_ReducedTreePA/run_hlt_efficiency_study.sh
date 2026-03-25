#!/bin/bash

set -euo pipefail

cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA

make ExecuteTest PlotHLTEfficiency

OUTPUTBASE=${OUTPUTBASE:-$PWD/output/hlt_efficiency_study}
NTHREAD=${NTHREAD:-5}
MAX_FILES=${MAX_FILES:-20}

rm -rf "$OUTPUTBASE"
mkdir -p "$OUTPUTBASE"

OUTPUTDIR="$OUTPUTBASE/skims" WRITE_ALL_TRACK_SELECTION_TREES=true NTHREAD="$NTHREAD" MAX_FILES="$MAX_FILES" \
   ./run_local_skim_test.sh 1 0 0

hadd -f "$OUTPUTBASE/merged_AP.root" "$OUTPUTBASE"/skims/PAData/*_AP.root
hadd -f "$OUTPUTBASE/merged_PA.root" "$OUTPUTBASE"/skims/PAData/*_PA.root
hadd -f "$OUTPUTBASE/merged_all.root" "$OUTPUTBASE"/skims/PAData/*_AP.root "$OUTPUTBASE"/skims/PAData/*_PA.root

./PlotHLTEfficiency --Input "$OUTPUTBASE/merged_all.root" --Output "$OUTPUTBASE/HLTEfficiency.pdf"
./PlotHLTEfficiency --Input "$OUTPUTBASE/merged_AP.root" --Output "$OUTPUTBASE/HLTEfficiency_AP.pdf"
./PlotHLTEfficiency --Input "$OUTPUTBASE/merged_PA.root" --Output "$OUTPUTBASE/HLTEfficiency_PA.pdf"

echo "Wrote skim outputs and plots under $OUTPUTBASE"
