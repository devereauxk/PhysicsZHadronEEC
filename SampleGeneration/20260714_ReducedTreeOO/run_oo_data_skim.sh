#!/bin/bash
set -euo pipefail

NPARALLEL=${NPARALLEL:-10}
OUTDIR=/eos/cms/store/group/phys_heavyions/kdeverea/Run3_2025_OO_Skim/V0.0
FILELIST="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/oo_data_filelist.txt"
SCRIPTDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOGDIR=/tmp/kdeverea/oo_data_skim_logs
mkdir -p "$LOGDIR"

if [ ! -f "$FILELIST" ]; then
   echo "Error: file list not found at $FILELIST"
   exit 1
fi

TOTAL=$(wc -l < "$FILELIST")
echo "=== OO Data Skim Production ==="
echo "Input files:  $TOTAL"
echo "Output dir:   $OUTDIR"
echo "Parallelism:  $NPARALLEL"
echo "Log dir:      $LOGDIR"
echo ""

DONE=0
FAIL=0
SKIP=0

process_file() {
   local INPUT="$1"
   local BASENAME=$(basename "$INPUT" .root)

   # Extract IonPhysicsN and subdirectory index for unique output naming
   # Path: .../IonPhysicsN/.../0000/HiForestMiniAOD_K.root
   local IONPHYS=$(echo "$INPUT" | grep -oP 'IonPhysics\d+' | head -1)
   local SUBDIR=$(echo "$INPUT" | grep -oP '/000\d/' | tr -d '/')
   local OUTFILE="${OUTDIR}/${IONPHYS}_${SUBDIR}_${BASENAME}.root"

   if [ -f "$OUTFILE" ]; then
      echo "[SKIP] $OUTFILE already exists"
      return 0
   fi

   local LOGFILE="${LOGDIR}/${IONPHYS}_${SUBDIR}_${BASENAME}.log"

   ${SCRIPTDIR}/local_skim.sh OOData "$INPUT" "$OUTFILE" > "$LOGFILE" 2>&1
   local RC=$?
   if [ $RC -ne 0 ]; then
      echo "[FAIL] $INPUT (rc=$RC, see $LOGFILE)"
      return 1
   fi
   return 0
}

export -f process_file
export OUTDIR SCRIPTDIR LOGDIR

echo "Starting at $(date)"
cat "$FILELIST" | xargs -P "$NPARALLEL" -I {} bash -c 'process_file "$@"' _ {}
echo ""
echo "Finished at $(date)"

# Count results
NSKIMMED=$(find "$OUTDIR" -name "*.root" 2>/dev/null | wc -l)
echo "Output files on EOS: $NSKIMMED / $TOTAL"
