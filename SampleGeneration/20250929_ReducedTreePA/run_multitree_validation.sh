#!/bin/bash

set -euo pipefail

cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA

make Execute AuditMultitree

REPORT=${REPORT:-/home/kdeverea/PhysicsZHadronEEC/.github/pa_skimmer_multitree_validation_review.md}
REVIEW_ROOT=${REVIEW_ROOT:-$PWD/output/multitree_review}
LOGROOT="$REVIEW_ROOT/logs"
BENCHMARK_MAX_FILES=${BENCHMARK_MAX_FILES:-20}
BENCHMARK_THREADS=${BENCHMARK_THREADS:-"1 2 5 10 15 20 30 40"}

mkdir -p "$LOGROOT"
rm -rf "$REVIEW_ROOT/PAData0000" "$REVIEW_ROOT/PAData0005" "$REVIEW_ROOT/PAMC" "$REVIEW_ROOT/APMC" "$REVIEW_ROOT/benchmarks"

samples=(
  "PAData0000|data|$PWD/Samples/PAData/0000/HiForestAOD_100.root|1|0|0|PAData8TeV|PAData|RecoHiForestAOD_100"
  "PAData0005|data|$PWD/Samples/PAData/0005/HiForestAOD_5000.root|1|0|0|PAData8TeV|PAData|RecoHiForestAOD_5000"
  "PAMC|mc|$PWD/Samples/PAMC/HiForestAOD_10.root|0|1|0|PAMC8TeV|PAMC|RecoHiForestAOD_10"
  "APMC|mc|$PWD/Samples/APMC/HiForestAOD_10.root|0|0|1|PAMC8TeV|APMC|RecoHiForestAOD_10"
)

AUDIT_TMP=/tmp/kdeverea/multitree_audit_summary.txt
rm -f "$AUDIT_TMP"

for sample in "${samples[@]}"
do
   IFS="|" read -r NAME KIND INPUT DODATA DOPAMC DOAPMC ORIGINAL_TYPE SUBDIR BASENAME <<< "$sample"

   SAMPLE_ROOT="$REVIEW_ROOT/$NAME"
   mkdir -p "$SAMPLE_ROOT/original" "$SAMPLE_ROOT/test_true" "$SAMPLE_ROOT/test_false"

   INPUT_FILE="$INPUT" OUTPUTDIR="$SAMPLE_ROOT/test_true" WRITE_ALL_TRACK_SELECTION_TREES=true NTHREAD=1 MAX_FILES=1 \
      ./run_local_skim.sh "$DODATA" "$DOPAMC" "$DOAPMC" > "$LOGROOT/${NAME}_test_true.log" 2>&1

   INPUT_FILE="$INPUT" OUTPUTDIR="$SAMPLE_ROOT/test_false" WRITE_ALL_TRACK_SELECTION_TREES=false NTHREAD=1 MAX_FILES=1 \
      ./run_local_skim.sh "$DODATA" "$DOPAMC" "$DOAPMC" > "$LOGROOT/${NAME}_test_false.log" 2>&1

   if [[ "$KIND" == "data" ]]; then
      ./local_skim.sh "$ORIGINAL_TYPE" "$INPUT" "$SAMPLE_ROOT/original/${BASENAME}.root" Dummy > "$LOGROOT/${NAME}_original.log" 2>&1
      for split in AP PA
      do
         ./AuditMultitree \
            --Label "${NAME}_${split}_true" \
            --Original "$SAMPLE_ROOT/original/${BASENAME}_${split}.root" \
            --Test "$SAMPLE_ROOT/test_true/${SUBDIR}/${BASENAME}_${split}.root" >> "$AUDIT_TMP"
         ./AuditMultitree \
            --Label "${NAME}_${split}_false" \
            --Original "$SAMPLE_ROOT/original/${BASENAME}_${split}.root" \
            --Test "$SAMPLE_ROOT/test_false/${SUBDIR}/${BASENAME}_${split}.root" >> "$AUDIT_TMP"
      done
   else
      ./local_skim.sh "$ORIGINAL_TYPE" "$INPUT" "$SAMPLE_ROOT/original/${BASENAME}.root" "$SAMPLE_ROOT/original/GenHiForestAOD_10.root" > "$LOGROOT/${NAME}_original.log" 2>&1
      ./AuditMultitree \
         --Label "${NAME}_true" \
         --Original "$SAMPLE_ROOT/original/${BASENAME}.root" \
         --Test "$SAMPLE_ROOT/test_true/${SUBDIR}/${BASENAME}.root" >> "$AUDIT_TMP"
      ./AuditMultitree \
         --Label "${NAME}_false" \
         --Original "$SAMPLE_ROOT/original/${BASENAME}.root" \
         --Test "$SAMPLE_ROOT/test_false/${SUBDIR}/${BASENAME}.root" >> "$AUDIT_TMP"
   fi
done

mapfile -t BENCHMARK_FILES < <(find "$PWD/Samples/PAData/0000" -maxdepth 1 -type f -name 'HiForestAOD_*.root' | sort | sed -n "1,${BENCHMARK_MAX_FILES}p")
BENCHMARK_LIST=/tmp/kdeverea/benchmark_file_list.txt
printf '%s\n' "${BENCHMARK_FILES[@]}" > "$BENCHMARK_LIST"

BENCHMARK_TMP=/tmp/kdeverea/multitree_benchmark_summary.txt
rm -f "$BENCHMARK_TMP"

for N in $BENCHMARK_THREADS
do
   OUTDIR="$REVIEW_ROOT/benchmarks/NTHREAD_${N}"
   rm -rf "$OUTDIR"
   START=$(date +%s.%N)
   OUTPUTDIR="$OUTDIR" INPUT_FILE="" WRITE_ALL_TRACK_SELECTION_TREES=true NTHREAD="$N" MAX_FILES="$BENCHMARK_MAX_FILES" \
      ./run_local_skim.sh 1 0 0 > "$LOGROOT/benchmark_NTHREAD_${N}.log" 2>&1
   END=$(date +%s.%N)
   ELAPSED=$(python3 - <<PY
start = float("$START")
end = float("$END")
print(f"{end-start:.2f}")
PY
)
   COUNT=$(find "$OUTDIR/PAData" -type f \( -name 'RecoHiForestAOD_*_AP.root' -o -name 'RecoHiForestAOD_*_PA.root' \) | wc -l)
   echo "$N|$ELAPSED|$COUNT" >> "$BENCHMARK_TMP"
done

export REPORT REVIEW_ROOT
python3 - <<'PY'
import os
from pathlib import Path

audit_path = Path('/tmp/kdeverea/multitree_audit_summary.txt')
benchmark_path = Path('/tmp/kdeverea/multitree_benchmark_summary.txt')
benchmark_file_list = Path('/tmp/kdeverea/benchmark_file_list.txt')
report_path = Path(os.environ['REPORT'])
logroot = Path(os.environ['REVIEW_ROOT']) / 'logs'

def parse_blocks(text):
    blocks = {}
    current = None
    for line in text.splitlines():
        if line.startswith('LABEL '):
            current = line.split(' ', 1)[1].strip()
            blocks[current] = {}
        elif current is not None and ' ' in line:
            key, value = line.split(' ', 1)
            blocks[current][key.strip()] = value.strip()
    return blocks

blocks = parse_blocks(audit_path.read_text())

mapping = [
    ('PAData0000', 'AP', 'PAData0000_AP_true', 'PAData0000_AP_false'),
    ('PAData0000', 'PA', 'PAData0000_PA_true', 'PAData0000_PA_false'),
    ('PAData0005', 'AP', 'PAData0005_AP_true', 'PAData0005_AP_false'),
    ('PAData0005', 'PA', 'PAData0005_PA_true', 'PAData0005_PA_false'),
    ('PAMC', 'Reco', 'PAMC_true', 'PAMC_false'),
    ('APMC', 'Reco', 'APMC_true', 'APMC_false'),
]

lines = []
lines.append('# PA skimmer multitree validation review')
lines.append('')
lines.append('## Scope')
lines.append('')
lines.append('- `ReduceForest.cpp` can write `Tree`, `TreeLoose`, and `TreeTight` when `WriteAllTrackSelectionTrees=true`; otherwise it keeps the single-tree path.')
lines.append('- Checked-in validation and benchmarking tools now live in `SampleGeneration/20250929_ReducedTreePA/` via `AuditMultitree.cpp` and `run_multitree_validation.sh`.')
lines.append('- The review below uses the checked-in runners and audit executable.')
lines.append('')
lines.append('## hiBinUp values (reported only, no action taken)')
lines.append('')
lines.append('| Sample | Output | Original hiBinUp values | Test hiBinUp values |')
lines.append('| --- | --- | --- | --- |')
for sample, output, true_key, false_key in mapping:
    block = blocks[true_key]
    lines.append(f"| {sample} | {output} | `{block.get('original_hiBinUp_values', 'missing')}` | `{block.get('test_hiBinUp_values', 'missing')}` |")

lines.append('')
lines.append('## Correctness summary')
lines.append('')
lines.append('| Sample | Output | Original entries | Test entries | Multitree nominal == original (ignoring hiBinUp/down) | Event info equal across Tree/Loose/Tight | Track order Loose >= Nominal >= Tight | Single-tree nominal == original |')
lines.append('| --- | --- | ---: | ---: | --- | --- | --- | --- |')
for sample, output, true_key, false_key in mapping:
    true_block = blocks[true_key]
    false_block = blocks[false_key]
    lines.append(
        f"| {sample} | {output} | {true_block.get('original_entries', 'missing')} | {true_block.get('test_entries', 'missing')} | "
        f"{true_block.get('nominal_equal_except_hiBinUpDown', 'missing')} | "
        f"{true_block.get('event_info_equal_across_trees', 'missing')} | "
        f"{true_block.get('track_order_loose_ge_nominal_ge_tight', 'missing')} | "
        f"{false_block.get('nominal_equal_except_hiBinUpDown', 'missing')} |"
    )

lines.append('')
lines.append('## Benchmark setup')
lines.append('')
lines.append('- Runner: `SampleGeneration/20250929_ReducedTreePA/run_local_skim.sh`')
lines.append('- Mode: `WRITE_ALL_TRACK_SELECTION_TREES=true`')
lines.append('- Dataset: first 20 lexicographically matched files from `Samples/PAData/0000/HiForestAOD_*.root`')
lines.append('- Files used:')
for file_name in benchmark_file_list.read_text().splitlines():
    relative = Path(file_name).relative_to(Path('/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA'))
    lines.append(f'  - `{relative}`')

lines.append('')
lines.append('## Benchmark results')
lines.append('')
lines.append('| NTHREAD | Elapsed time (s) | Output files produced |')
lines.append('| ---: | ---: | ---: |')
for line in benchmark_path.read_text().splitlines():
    nthread, elapsed, count = line.split('|')
    lines.append(f'| {nthread} | {elapsed} | {count} |')

lines.append('')
lines.append('## Notes')
lines.append('')
lines.append('- `hiBinUp` remains unstable in the original skim and is intentionally only reported.')
lines.append('- Empty PA/AP splits occur when a file lies outside that run window; parity checks still pass when both original and test outputs are empty.')
lines.append(f'- Detailed logs are under `{logroot}`.')

report_path.write_text('\n'.join(lines) + '\n')
print(f'Wrote {report_path}')
PY

echo "Wrote report to $REPORT"
