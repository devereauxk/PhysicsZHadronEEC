# Analyzer plan: PA local skim parallel I/O benchmark and script integration

## Reviewer scope

This task is only about **local skim throughput benchmarking** for:

- `SampleGeneration/20250929_ReducedTreePA/local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/run_local_skim.sh`

with a new runner:

- `SampleGeneration/20250929_ReducedTreePA/run_parallel_local_skim.sh`

Goal: test whether parallelizing over multiple PAData input files improves runtime (I/O may dominate).

Do not alter physics selection or correction content in this task. Keep behavior equivalent to the existing local skim path except for controlled parallel scheduling and output naming safety.

## Required analyzer work

### 1) Audit the current local skim interface

Read:

- `SampleGeneration/20250929_ReducedTreePA/local_skim.sh`
- `SampleGeneration/20250929_ReducedTreePA/run_local_skim.sh`

Identify current assumptions:

- required args
- output naming
- where data loops come from
- any early exits

Then make only the minimal compatibility edits needed so `local_skim.sh` can be safely called by both:

- existing serial `run_local_skim.sh`
- new parallel `run_parallel_local_skim.sh`

### 2) Add a new parallel runner script

Create:

- `SampleGeneration/20250929_ReducedTreePA/run_parallel_local_skim.sh`

Style must follow `run_local_skim.sh`:

- same high-level argument style (`DODATA`, `DOPAMC`, `DOAPMC` flags)
- same source preparation / `clean.sh` pattern where applicable
- same dataset discovery patterns under `Samples/*`

Parallel runner requirements:

1. Parallelize **across files** (not inside one file) for benchmarking.
2. Use a bounded worker pool controlled by env var:
   - `NTHREAD` (default a reasonable value, e.g. 4)
3. Ensure every process writes to unique output names.
   - No shared temporary output path collisions.
   - Preserve the `_AP` / `_PA` data split naming semantics currently produced by `local_skim.sh`.
4. Capture timing in a reproducible way per benchmark point.
5. Include a serial reference mode (or include `NTHREAD=1` in sweep) for comparison.

Do not use destructive process-kill patterns (`pkill`, `killall`).

### 3) Benchmark on a PAData subset

Run benchmark only on a **small subset** of PAData files (enough to show scaling trend, not a full production pass).

Suggested NTHREAD sweep:

- `1`, `2`, `4`, `8`

If machine constraints suggest a different upper bound, document why.

For each tested NTHREAD:

- run the same fixed input subset
- measure wall time
- record whether all expected outputs were produced
- verify no file clobbering/collision

### 4) Determine whether parallelization helps

From measured results, report:

- speedup (or slowdown) vs `NTHREAD=1`
- best-performing tested NTHREAD
- whether gains saturate (I/O limit)

If no gain:

- explicitly state that and keep the script usable anyway for optional parallel use.

### 5) Validation checklist

For changed scripts, validate:

1. `bash -n` syntax check on:
   - `local_skim.sh`
   - `run_local_skim.sh` (if edited)
   - `run_parallel_local_skim.sh`
2. Dry-run / real-run sanity on the selected PAData subset.
3. Output file uniqueness and expected AP/PA split products.

Do not run full dataset production in this task.

## Stop conditions / escalation

Stop and report back if:

1. `Samples/PAData` is unavailable in the environment.
2. The current local skim logic writes intermediates in a way that cannot be made parallel-safe without larger refactor.
3. Benchmark results are too noisy to compare because of unstable external I/O conditions.

In each case, provide concrete blocker details and the minimal next action.

## Required execution summary

Write summary to:

- `.github/pa_parallel_local_skim_execution.md`

Include:

1. Files changed.
2. Exact commands run.
3. Input subset definition.
4. Timing table by NTHREAD.
5. Best NTHREAD (or explicit “no improvement” result).
6. Validation evidence for unique outputs and AP/PA split integrity.
