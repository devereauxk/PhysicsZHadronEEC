#!/bin/bash

set -e

source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh

cd /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA
make ExecuteTest

OUTPUTDIR=${OUTPUTDIR:-$PWD/output}
INPUT_FILE=${INPUT_FILE:-}
NTHREAD=${NTHREAD:-20}
MAX_FILES=${MAX_FILES:-0}
mkdir -p "$OUTPUTDIR/PAData"
mkdir -p "$OUTPUTDIR/PAMC"
mkdir -p "$OUTPUTDIR/APMC"

DODATA=$1
DOPAMC=$2
DOAPMC=$3

wait_for_slot() {
   while [[ "$(jobs -rp | wc -l)" -ge "$NTHREAD" ]]
   do
      sleep 1
   done
}

launch_jobs() {
   local type=$1
   local output_subdir=$2
   shift 2

   local files=("$@")
   local count=0
   for i in "${files[@]}"
   do
      [[ -e "$i" ]] || continue
      if [[ "$MAX_FILES" -gt 0 && "$count" -ge "$MAX_FILES" ]]; then
         break
      fi
      wait_for_slot
      if [[ "$type" == "PAData8TeV" ]]; then
         ./local_skim_test.sh "$type" "${i}" "$OUTPUTDIR/$output_subdir/Reco$(basename "${i}")" Dummy &
      else
         ./local_skim_test.sh "$type" "${i}" "$OUTPUTDIR/$output_subdir/Reco$(basename "${i}")" "$OUTPUTDIR/$output_subdir/Gen$(basename "${i}")" &
      fi
      count=$((count + 1))
   done
}

echo "NTHREAD = $NTHREAD"
echo "MAX_FILES = $MAX_FILES"

if [[ "$DODATA" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=("$PWD"/Samples/PAData/000*/HiForestAOD_*.root)
   fi
   launch_jobs PAData8TeV PAData "${FILES[@]}"
fi

if [[ "$DOPAMC" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=("$PWD"/Samples/PAMC/*root)
   fi
   launch_jobs PAMC8TeV PAMC "${FILES[@]}"
fi

if [[ "$DOAPMC" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=("$PWD"/Samples/APMC/*root)
   fi
   launch_jobs PAMC8TeV APMC "${FILES[@]}"
fi

wait
