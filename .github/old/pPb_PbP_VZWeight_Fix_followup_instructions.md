# Follow-up Instructions for Analyzer (Post `pPb_PbP_VZWeight_Fix_execution.md`)

## Reviewer note
Your previous execution covered the main plan well, but this follow-up is required to close remaining gaps in deliverables.

Execute the tasks below **in order**.

---

## Task 1 (first, mandatory): Reproduce and refresh Overleaf plots for the pPb+PbP combined section using CURRENT weights

### Scope
Regenerate the combined pPb+PbP result plots that are used in the Overleaf “combined pPb/Pbp” section, using the **current in-place pPb weight configuration** (same setup/tag family as Part A from your last task).

### Requirements
- Use the **same underlying result ROOT inputs** that were used to make Part A result plots.
- Re-run the relevant plotting scripts (script-based flow, no ad hoc one-off command chains for official outputs).
- Ensure outputs correspond to combined `all_*` plots used in note results section.
- Copy refreshed PDFs to Overleaf in-place (same filenames).

### Validation to include
- Provide exact source ROOT files used.
- Provide exact generated PDF list.
- Confirm file timestamps changed for refreshed Overleaf targets.
- Confirm `src/results.tex` references match those filenames.

---

## Task 2: Produce exploratory central-value closure plots for the new VZ scheme in central-closure plotting directory

### Scope
Create actual central-value closure plots for exploratory `newVZFix` chain and place them in:
- `Plots/20260120_CentralClosure/plots/`

### Requirements
- Use exploratory new-weight products already produced (`newVZFix` VZ/Z/residual stack).
- It is acceptable to override existing `ZV6`/`trkV24` closure plots in this directory if needed.
- Produce the standard closure set (`DeltaPhi`/`DeltaEta`, `all`/`bkg`/`result`) for pPb and PbP bins used by closure scripts.
- Do **not** copy these exploratory closure plots to Overleaf.

### Validation to include
- List all generated/overridden files.
- Report closure-integral summary table for exploratory set (same format as prior summary).
- Briefly flag bins that worsen vs current baseline.

---

## Task 3: `/tmp` hygiene and relocation under `/tmp/kdeverea`

### Scope
Clean up temporary artifacts and ensure files related to this work are under `/tmp/kdeverea` only.

### Requirements
- Move/relocate any repo-task temp files under `/tmp` into `/tmp/kdeverea`.
- Remove stale temporary files no longer needed.
- Do not delete unrelated third-party/user files.

### Validation to include
- Before/after listing summary for relevant `/tmp` paths.
- Explicit list of files moved and files removed.

---

## Execution rules (apply to all tasks)
- Use `python3`, not `python`.
- For official production outputs, use/modify existing scripts or add new scripts in-repo.
- Keep command log complete and ordered.
- Return one summary markdown file after all 3 tasks are complete, including:
  - commands run,
  - outputs produced,
  - validations,
  - failures/retries and resolutions.
