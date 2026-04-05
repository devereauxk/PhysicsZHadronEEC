#source ./clean.sh

set -euo pipefail

echo "did you run kinit -5? [y/n]"
read -r KINIT
if [[ "$KINIT" != "y" ]]; then
    echo "Please run 'kinit -5' before proceeding."
    exit 1
fi

source clean.sh

OUTPUTDIR=${OUTPUTDIR:-$PWD/output/local_skim}

#runs right thorugh with D status with 10 threads - super tempermental though
#exact procedure: screen at ~, kinit, cd to skimmer, then skim with NTREAD=10
#slowed down a bit later though idk ...

INPUT_FILE=${INPUT_FILE:-}
NTHREAD=${NTHREAD:-10}
MAX_FILES=${MAX_FILES:-0}
mkdir -p $OUTPUTDIR
mkdir -p $OUTPUTDIR/PAData
mkdir -p $OUTPUTDIR/PAMC
mkdir -p $OUTPUTDIR/APMC
mkdir -p $OUTPUTDIR/PPData
mkdir -p $OUTPUTDIR/PPMC

DODATA=${1:-0}
DOPAMC=${2:-0}
DOAPMC=${3:-0}
DOPPDATA=${4:-0}
DOPPMC=${5:-0}
PIDS=()
JOBLABELS=()

wait_for_slot() {
   while [[ "$(jobs -rp | wc -l)" -ge "$NTHREAD" ]]
   do
      sleep 1
   done
}

launch_jobs() {
   local Type=$1
   local OutputSubdir=$2
   shift 2

   local Count=0
   for i in "$@"
   do
      [[ -e "$i" ]] || continue
      if [[ "$MAX_FILES" -gt 0 && "$Count" -ge "$MAX_FILES" ]]; then
         break
      fi
      wait_for_slot
      if [[ "$Type" == "PAData8TeV" || "$Type" == "PPData5TeV" ]]; then
         ./local_skim.sh "$Type" "$i" "$OUTPUTDIR/$OutputSubdir/Reco$(basename "$i")" Dummy &
      else
         ./local_skim.sh "$Type" "$i" "$OUTPUTDIR/$OutputSubdir/Reco$(basename "$i")" "$OUTPUTDIR/$OutputSubdir/Gen$(basename "$i")" &
      fi
      PIDS+=("$!")
      JOBLABELS+=("$Type ${i}")
      Count=$((Count + 1))
   done
}

wait_for_jobs() {
   local i
   local Status=0

   for i in "${!PIDS[@]}"
   do
      if ! wait "${PIDS[$i]}"; then
         echo "Skim job failed: ${JOBLABELS[$i]}" >&2
         Status=1
      fi
   done

   return $Status
}

if [[ "$DODATA" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=($PWD/Samples/PAData/000*/HiForestAOD_*.root)
   fi
   launch_jobs PAData8TeV PAData "${FILES[@]}"
fi

if [[ "$DOPAMC" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=($PWD/Samples/PAMC/*root)
   fi
   launch_jobs PAMC8TeV PAMC "${FILES[@]}"
fi

if [[ "$DOAPMC" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=($PWD/Samples/APMC/*root)
   fi
   launch_jobs PAMC8TeV APMC "${FILES[@]}"
fi

if [[ "$DOPPDATA" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=($PWD/Samples/PPData/000*/HiForestAOD_*.root)
   fi
   launch_jobs PPData5TeV PPData "${FILES[@]}"
fi

if [[ "$DOPPMC" == "1" ]]; then
   if [[ -n "$INPUT_FILE" ]]; then
      FILES=("$INPUT_FILE")
   else
      FILES=($PWD/Samples/PPMC/*root)
   fi
   launch_jobs PPMC5TeV PPMC "${FILES[@]}"
fi

wait_for_jobs
