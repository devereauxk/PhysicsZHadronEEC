# Concurrent Agent Safety Assessment

## Summary

Running two CLI agents (Claude + Copilot) simultaneously against this repository
has **serious correctness and crash risks** under the current build system. The
root cause is a single shared resource — `CommonCode/library/Messenger.o` — that
every `clean.sh` in the project deletes and rebuilds, combined with the lack of
any file-level locking.

---

## Current State

### What happens when a workflow starts

Every major runner script (`central.sh`, `systematics.sh`, `closure-*.sh`,
`pp-EE.sh`, etc.) calls `source clean.sh` at the top unless `SKIP_CLEAN=1` is
set. Every `clean.sh` in the project does the same three things:

```bash
rm -f ExecuteCorrelationAnalysis     # local binary
rm -rf ../../CommonCode/binary/      # GLOBAL shared utilities (DHAdd, etc.)
rm -rf ../../CommonCode/library/     # GLOBAL shared Messenger.o
# ... then rebuilds CommonCode and the local binary
```

This pattern is identical across all 13 `clean.sh` files:
- `MainAnalysis/20241102_ZhadronVsZPt/`
- `MainAnalysis/20251211_ResidualCorrection/`
- `MainAnalysis/20260518_ResidualCorrection2D/`
- `MainAnalysis/20260115_ZCorrection/`
- `MainAnalysis/20260222_EnergyExtrapolation/`
- `MainAnalysis/20260414_pPbPbpCombining/`
- `MainAnalysis/20260511_PionTrackStudy/`
- `MainAnalysis/20260523_NmixStudy/`
- `MainAnalysis/20260216_temp/`
- `Plots/20250918_pPbCentralValues/`
- `Plots/20260213_Central/`
- `SampleGeneration/20240607_PredictionConversion/`
- `SampleGeneration/20250929_ReducedTreePA/`

### What is shared vs. what is local

| Resource | Scope | Risk |
|----------|-------|------|
| `CommonCode/library/Messenger.o` | **Global** — single file, linked by 23+ modules | **Critical**: any `clean.sh` deletes it for everyone |
| `CommonCode/binary/` (DHAdd, etc.) | **Global** — single directory | **High**: nuked by every `clean.sh` |
| `ExecuteCorrelationAnalysis` | **Local** — each dated dir has its own copy | **Low for cross-module**: module A's clean doesn't touch module B's binary. **But** if two agents run the same module simultaneously, one can delete the other's binary mid-chunking |
| `output/*.root` files | **Local per tag** — filenames include the tag string | **Medium**: safe if agents use different tags; **dangerous** if they share a tag |
| `makeProjection.C` etc. | **Local** — read-only macros | **None**: not modified at runtime |

### How `make` behaves (without `clean.sh`)

Plain `make` is dependency-aware and **safe**:
- `CommonCode/make`: only rebuilds `Messenger.o` if `Messenger.cpp` or `Messenger.h`
  changed. Currently a no-op when nothing changed.
- Module-level `make`: only rebuilds `ExecuteCorrelationAnalysis` if
  `CorrelationAnalysis.cpp` changed (checks timestamp).

The problem is exclusively `clean.sh`, which does `rm -rf` before `make`,
**forcing a full rebuild every time regardless of whether source changed**.

### The `SKIP_CLEAN` mechanism

Some scripts already support `SKIP_CLEAN=1`:
- **Supported**: `central.sh`, `systematics.sh`, `system-analysis.sh`, `closure-Z.sh`,
  `closure-trk.sh`, `pp-EE.sh`, `pp-madgraphclosure.sh`, `check-dataMCComparison.sh`,
  2D residual `run-pp.sh`/`run-pPb.sh`, pPbPbp `runner.sh`/`runner_mc.sh`
- **NOT supported** (always calls `clean.sh`): 1D residual `run-pp.sh`/`run-pPb.sh`,
  `jewel-*.sh`

### Linux inode safety (partial protection)

On Linux, `rm` of a running binary does **not** crash the running process — the
kernel keeps the inode alive until the last file descriptor closes. So a running
`ExecuteCorrelationAnalysis` process survives its binary being deleted. However:

- If `threader.sh` has not yet spawned all chunks, new chunk processes try to
  execute the **deleted** binary path → immediate failure.
- After rebuild, the new binary at the same path is a **different inode**. If
  source code changed between agents, this silently mixes output from two
  different code versions.

---

## Failure Scenarios

### Scenario 1: Corrupted production (silent, worst case)

1. Agent A starts `central.sh` for pPb systematics (tag `ZV9_trkV28_nmix10`).
   `threader.sh` spawns 20 chunks of `ExecuteCorrelationAnalysis`.
2. Agent B starts `systematics.sh` in the same directory. Its `clean.sh` runs:
   `rm -f ExecuteCorrelationAnalysis`.
3. Agent A's chunk 15–20 have not spawned yet. They fail because the binary is
   gone. `hadd` at the end merges only chunks 1–14. The resulting ROOT file has
   **partial statistics** with no error indication in the filename.
4. Downstream plots and systematics consume the incomplete file as if it were
   the full result.

### Scenario 2: Code-version mixing (silent, insidious)

1. Agent A is running `ExecuteCorrelationAnalysis` from module
   `20260518_ResidualCorrection2D` (has 2D residual correction logic).
2. Agent B runs `clean.sh` from `20241102_ZhadronVsZPt`, which:
   - Deletes `CommonCode/library/Messenger.o`
   - Rebuilds `Messenger.o` (same source, so identical — safe in this case)
3. But if Agent B has also modified `Messenger.h` or `Messenger.cpp`, the rebuilt
   `Messenger.o` may have a different ABI. If Agent A's next compilation links
   against it, the new binary could produce subtly different output.

### Scenario 3: Compilation failure under concurrent `rm -rf`

1. Agent A's `clean.sh` does `cd CommonCode && make`.
2. Agent B's `clean.sh` does `rm -rf ../../CommonCode/library/` while Agent A's
   `make` is mid-write of `Messenger.o`.
3. Agent A's `make` succeeds (wrote to a now-deleted inode). Agent A's subsequent
   link step looks for `library/Messenger.o` → file not found → build failure.

### Scenario 4: Output file collision

1. Both agents run `central.sh` with the same tag and same Z pT / track pT bins.
2. Both write to `output/pp_trkResidual_EEV5_ZV9_trkV28_nmix10_ZPT0_30-0.5_2.root`.
3. Whichever finishes last overwrites the other's file. If `hadd -f` runs while the
   other agent's chunks are still writing, ROOT may produce a corrupt file.

---

## Recommendations

### Immediate: Agent operating rules (no code changes)

1. **Always set `SKIP_CLEAN=1`** when running any workflow from a CLI agent.
   This skips `clean.sh` entirely and relies on `make`'s dependency checking.
   ```bash
   export SKIP_CLEAN=1
   ```
   Pre-compile once manually before starting agents:
   ```bash
   cd CommonCode && make && cd ../MainAnalysis/20241102_ZhadronVsZPt && make
   ```

2. **Do not modify source files while production is running.** If `Messenger.h`,
   `Messenger.cpp`, or any `CorrelationAnalysis.cpp` changes, `make` will trigger
   a rebuild — which replaces the binary for future chunk spawns.

3. **Use different output tags when agents work on the same module.** If Agent A
   runs nominal pp and Agent B runs nominal pPb, they naturally use different
   prefixes (`pp_trkResidual_...` vs `pPb_trkResidual_...`). But if both run pp
   nominal with the same tag, outputs collide.

4. **Separate agents by module when possible.** Agent A in
   `20241102_ZhadronVsZPt`, Agent B in `20260518_ResidualCorrection2D` — their
   `ExecuteCorrelationAnalysis` binaries are independent. The only shared resource
   is CommonCode, which is safe **as long as neither agent calls `clean.sh`**.

5. **Never run `clean.sh` from any module while another agent is active.**
   `clean.sh` is a global operation disguised as a local one — it nukes
   `CommonCode/library/` which affects every module.

### Short-term: Add SKIP_CLEAN support to remaining scripts

The 1D residual `run-pp.sh` and `run-pPb.sh` unconditionally call `clean.sh`.
Wrap them like the 2D versions:

```bash
if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    cd ..
    ./clean.sh
    cd "$WORKINGDIR"
fi
```

### Medium-term: Make `clean.sh` safe

Replace the destructive `rm -rf` + rebuild with incremental `make`:

```bash
# safe-clean.sh (proposed replacement)
THISFOLDER=$(pwd)
cd ../../
source SetupAnalysis.sh
cd CommonCode/
make              # incremental — only rebuilds if source changed
cd ..
cd ${THISFOLDER}
make              # incremental — only rebuilds if source changed
```

This removes the `rm -rf` lines entirely. `make` already handles staleness via
timestamps. The only reason to force-delete is if you suspect the build artifacts
are from a different compiler/ROOT version — rare enough to be a manual step, not
an automatic one.

### Long-term: Lockfile for CommonCode builds

For maximum safety with concurrent agents:

```bash
# In clean.sh or any script that builds CommonCode:
(
    flock -x 200
    cd "${ProjectBase}/CommonCode" && make
) 200>"${ProjectBase}/CommonCode/.build.lock"
```

This ensures only one agent rebuilds CommonCode at a time. Combined with dropping
the `rm -rf`, this makes concurrent builds fully safe.

---

## Files to be careful about

| File/Directory | Why | Protection |
|---|---|---|
| `CommonCode/library/Messenger.o` | Linked by every executable in the project (23+ modules). Deleted by every `clean.sh`. | `SKIP_CLEAN=1`; never modify `Messenger.{h,cpp}` during production |
| `CommonCode/binary/*` | DHAdd, DHQuery, etc. Used by some scripts. Deleted by every `clean.sh`. | Same |
| `*/ExecuteCorrelationAnalysis` | Per-module binary. Deleted by that module's `clean.sh`. While running processes survive deletion (Linux inode), unchunked threader spawns fail. | `SKIP_CLEAN=1`; don't run two agents in the same module simultaneously |
| `output/*_chunk*.root` | Temporary chunk files created by `threader.sh`, merged by `hadd`, then deleted. Two agents writing the same chunks collide. | Use different tags or different modules |
| `output/*-result.root` | Final merged outputs. Overwritten by `hadd -f`. | Use different tags |
| `OfficialWeightDictionary.sh` | Sourced by all runners. Not modified at runtime — safe. | Read-only during production |
| `makeProjection*.C` | ROOT macros executed by `threader.sh`. Not modified at runtime — safe. | Read-only during production |

## Quick-Reference Checklist for Concurrent Agents

```
✓  export SKIP_CLEAN=1 before every workflow
✓  Pre-compile all needed modules manually before starting agents
✓  Assign each agent to a different module OR different tag
✗  Never call clean.sh while any agent is running
✗  Never modify .cpp/.h files while production is running
✗  Never have two agents write to the same output tag in the same module
```
