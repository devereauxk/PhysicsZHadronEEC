#source ./clean.sh

set -euo pipefail

source clean.sh

OUTPUTDIR=${OUTPUTDIR:-/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.1}

INPUT_FILE=${INPUT_FILE:-}
NTHREAD=${NTHREAD:-20}
MAX_FILES=${MAX_FILES:-0}
mkdir -p $OUTPUTDIR/PAData
mkdir -p $OUTPUTDIR/PAMC
mkdir -p $OUTPUTDIR/APMC

DODATA=$1
DOPAMC=$2
DOAPMC=$3
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
      if [[ "$Type" == "PAData8TeV" ]]; then
         ./local_skim.sh $Type ${i} $OUTPUTDIR/$OutputSubdir/Reco$(basename ${i}) Dummy &
      else
         ./local_skim.sh $Type ${i} $OUTPUTDIR/$OutputSubdir/Reco$(basename ${i}) $OUTPUTDIR/$OutputSubdir/Gen$(basename ${i}) &
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

wait_for_jobs
