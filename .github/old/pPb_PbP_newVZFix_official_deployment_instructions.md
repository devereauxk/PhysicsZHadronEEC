# Analyzer Directions: Officially deploy NEW VZ weighting as `ZV6` / `trkV24`

## Reviewer decision from audit

Treat the NEW VZ-weight scheme as the **official** `ZV6` / `trkV24` configuration for pPb/PbP production.

Your job is to replace transitional `newVZFix_*` naming with official `ZV6_trkV24` naming and regenerate official outputs.

---

## Global requirements

- Use script-driven production (modify/add scripts; avoid ad-hoc one-off production command chains for official outputs).
- Use `python3` only.
- Use `/tmp/kdeverea` for temporary files.
- If an official `ZV6_trkV24` file already exists, **overwrite in place**.
- Remove `newVZFix` from official output names (ROOT + plot filenames).

---

## Task 1: Promote NEW VZ files to official `ZV6/trkV24` workflow

1. Update production/plot scripts so official `ZV6_trkV24` tags point to NEW VZ-weight inputs (the former `newVZFix` weight files).
2. Ensure official output tags are:
   - `ZV6_trkV24_nmix10` (or existing official variant used by current scripts)
3. Remove transitional naming from newly generated outputs:
   - do not leave new official products with `newVZFix` in filename.
4. Keep script behavior deterministic and documented.

---

## Task 2: Clean audit products from central-closure area

Remove audit-only products so official directories are clean:

- `Plots/20260120_CentralClosure/plots/zstep_audit/` (entire audit folder)
- any remaining `zstepAudit*`-named artifacts in central-closure plotting outputs/scripts that are no longer needed for official production

After cleanup, the central-closure directories should contain only official production outputs.

---

## Task 3: Reproduce official Z-closure plots in `ZPT 0_500`

Regenerate Z-closure using official NEW-as-`ZV6` setup:

- Collision systems: `pPb`, `PbP`
- Required bin: `ZPT 0_500`
- Tag family: official `ZV6_trkV24`

Outputs must be non-empty and named with official tags (no `newVZFix` token).

---

## Task 4: Regenerate separated pPb/PbP comparison plots for analysis-note binning

Regenerate the separated overlay/comparison plots used in the analysis note section that compares pPb vs PbP:

- Use official `ZV6_trkV24` tags
- Use analysis-note binning
- Overwrite existing files with same official names

Ensure any previously generated `newVZFix`-named versions are not treated as official outputs.

---

## Task 5: Regenerate final result plots for analysis-note binning

Regenerate final results with official `ZV6_trkV24` tags:

- pPb result plots required for note
- analysis-note binning
- overwrite existing official-name files if present

Do not keep parallel official results that still carry transitional `newVZFix` naming.

---

## Task 6: Copy official pPb VZ6 stack to Overleaf

Copy refreshed pPb official outputs to Overleaf (in-place replacement) for VZ6 weighting.  
This must include the full correction stack products:

1. VZ reweighting plots
2. Z-correction plots
3. Z-closure plots
4. track-correction plots
5. track-closure plots
6. central-closure plots
7. separated pPb/PbP comparison plots
8. final result plots

Use existing Overleaf destination structure and replace files referenced by current note `.tex` files.

---

## Validation and summary required

Write a single execution summary:

`.github/pPb_PbP_newVZFix_official_deployment_execution.md`

Include:

1. exact scripts modified/added,
2. ordered command log,
3. mapping table: `old transitional name -> new official name`,
4. list of deleted audit outputs (`zstep_audit`, etc.),
5. manifest of regenerated official ROOT/PDF outputs,
6. manifest of files copied to Overleaf (with destination paths),
7. explicit check that no newly produced official deliverable retains `newVZFix` in filename.
