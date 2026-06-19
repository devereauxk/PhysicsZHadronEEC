# Analyzer execution summary: official pPb/PbP recovery and revalidation

## Scope and verdict

Executed the recovery plan in strict `pPb`/`PbP` scope.

Final verdict:
- rollback of unintended `pp` Overleaf edits: **PASS**
- official `ZV6/trkV24` correction provenance: **PASS**
- pPb/PbP-only revalidation reruns and plot regeneration: **PASS**
- naming audit on filtered official deliverables: **PASS**
- curated Overleaf copy: **PASS**

No `pp` files were recopied to Overleaf.

---

## 1) Rollback evidence for incorrect Overleaf edits

### What was reverted

The previous broad Overleaf sync had touched `pp` closure/track/Z-closure note assets.  
Using `/tmp/kdeverea/overleaf_copy_manifest.txt` plus `git status` in `~/OverleafZHadronInPPb`, I restored all accidental `pp` targets before any new pPb/PbP recopy.

Artifacts:
- status before rollback: `/tmp/kdeverea/overleaf_status_before.txt`
- pp targets identified from previous manifest: `/tmp/kdeverea/overleaf_manifest_pp_targets.txt`
- pp files restored with `git restore`: `/tmp/kdeverea/overleaf_pp_restore_candidates.txt`
- status after rollback: `/tmp/kdeverea/overleaf_status_after_rollback.txt`
- pp remaining after rollback: `/tmp/kdeverea/overleaf_pp_remaining_after_rollback.txt`

Counts:
- restored pp files: `39`
- pp files still modified after rollback: `0`

Validation statement:
- No `pp` file remained modified in Overleaf before the new pPb/PbP recopy began.

---

## 2) Provenance verdict for official `ZV6/trkV24` corrections

### Verdict

**PASS**

### Mapping evidence

The corrected swapped VZ pairing is explicitly encoded in:
- `Plots/20251001_pPbVZReweighting/run-reweight-newVZFix.sh`
- `Plots/20251001_pPbVZReweighting/run-reweight.sh` (updated in this task to match official workflow)

Relevant lines:
- pPb weight uses `--mcCollisionType pPb --dataCollisionType PbP`
- PbP weight uses `--mcCollisionType PbP --dataCollisionType pPb`

This matches the reviewer requirement:
- pPb correction uses **PbP data + pPb MC**
- PbP correction uses **pPb data + PbP MC**

### Byte-identity proof

Official files in use by main analysis are byte-identical to the corrected NEW-derived chain.

Full checksum table:
- `/tmp/kdeverea/provenance_sha256.tsv`

Compact checksum proof:

| Group | Corrected source | Official workflow/output | Main-analysis input | sha256 |
|---|---|---|---|---|
| VZ pPb | `20260318_newVZFix_ZPT0_500_VzReweightFits_pPb.root` | `20260311_ZPT0_500_VzReweightFits_pPb.root` | same file used directly | `50d5170c399e807488a0992390213ec697286f40ffab9f6f25245aecccfffbe1` |
| VZ PbP | `20260318_newVZFix_ZPT0_500_VzReweightFits_PbP.root` | `20260311_ZPT0_500_VzReweightFits_PbP.root` | same file used directly | `cf1c3ad03ba11ec8f38b71dd62202f7196e674cc9c3b278d169d94c94de2dd77` |
| Z corr pPb | `20260318_ZCorrection_V6_newVZFix_PPb_zPt0-500.root` | `20260311_ZCorrection_V6_PPb_zPt0-500.root` | `my_ZWeights/20260311_ZCorrection_V6_PPb_zPt0-500.root` | `5d757ed291299ca5b1cf89565ce5f20a7419859f1e1526fd8968b080f6a8976b` |
| Z corr PbP | `20260318_ZCorrection_V6_newVZFix_PbP_zPt0-500.root` | `20260311_ZCorrection_V6_PbP_zPt0-500.root` | `my_ZWeights/20260311_ZCorrection_V6_PbP_zPt0-500.root` | `b36305f7f3b073d11400992b601f00475484c682b182b35b90196bae05f3b916` |
| Trk corr pPb zPt0-10 | `20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PPb_zPt0-10.root` | same promoted official file | `my_residualWeights/...PPb_zPt0-10.root` | `a75319943125df3a2a930aca748b75c732c475844d3f5380c1f6146b503c91f6` |
| Trk corr pPb zPt10-20 | `...newVZFix_PPb_zPt10-20.root` | same promoted official file | `my_residualWeights/...PPb_zPt10-20.root` | `7362e22418ac6f38c416ab1c158334a1c7475dab412fa7ca2727d4a5ba12e39d` |
| Trk corr PbP zPt20-40 | `...newVZFix_PbP_zPt20-40.root` | same promoted official file | `my_residualWeights/...PbP_zPt20-40.root` | `f74a3041e9d457b28be94d0c5cd6c29065798aab7b3f44b086edb24bc0a780dc` |
| Trk corr PbP zPt40-500 | `...newVZFix_PbP_zPt40-500.root` | same promoted official file | `my_residualWeights/...PbP_zPt40-500.root` | `9e358aa0b4232fdb50817c7bedcfc8f6551a54f565f72baff097c91610ec20b9` |

### In-use script references

Current main-analysis scripts point at official promoted inputs:
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`

These reference:
- `Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_{pPb,PbP}.root`
- `my_ZWeights/20260311_ZCorrection_V6_{PPb,PbP}_zPt0-500.root`
- `my_residualWeights/20260311_TrackResidualCorrection_V24_ZWeight_V6_{PPb,PbP}_zPt*.root`

---

## 3) Rerun details

Task-1 provenance was already a clean **PASS**, so a forced end-to-end correction rerun was **not required**.

However, for recovery/revalidation I still re-executed the pPb/PbP-only official chain to refresh outputs from the official entrypoints:

1. Updated `Plots/20251001_pPbVZReweighting/run-reweight.sh` so the official script uses the swapped mapping.
2. Updated these plotting scripts to accept `PLOT_SYSTEMS` and safely skip `pp`:
   - `Plots/20260120_CentralClosure/plot-central.sh`
   - `Plots/20260115_ZResidualClosure/plot-Z.sh`
   - `Plots/20251202_trackResidualClosure/plot-track.sh`
3. Ran upstream pPb/PbP-only workflows:
   - `Plots/20251001_pPbVZReweighting/run-reweight.sh`
   - `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
   - `MainAnalysis/20251211_ResidualCorrection/workflow/run-pPb.sh`
4. Ran downstream pPb/PbP-only plots:
   - `Plots/20260120_CentralClosure/plot-central.sh` with `PLOT_SYSTEMS='pPb PbP'`
   - `Plots/20260115_ZResidualClosure/plot-Z.sh` with `PLOT_SYSTEMS='pPb PbP'`
   - `Plots/20251202_trackResidualClosure/plot-track.sh` with `PLOT_SYSTEMS='pPb PbP'`
   - `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
   - `Plots/20260213_Central/plot-central.sh`
   - `Plots/20260213_Central/plot-central-combined.sh`

### Log evidence for latest official inputs

Workflow log:
- `/tmp/copilot-tool-output-1773867319314-8jcfm4.txt`

Relevant lines:
- `[official VZ6] Building pPb weight from MC=PPbMC, Data=PbPData`
- `[official VZ6] Building PbP weight from MC=PbPMC, Data=PPbData`
- `[pPb-DY-analysis] name=20260311_ZCorrection_V6`
- `[pPb-DY-analysis] VZWeightFile_PPb=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_pPb.root`
- `[pPb-DY-analysis] VZWeightFile_PbP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_PbP.root`
- `[run-pPb] NAME_TAG=20260311_TrackResidualCorrection_V24_ZWeight_V6`
- `[pPb-DY-analysis] ZWeightFile_pPb=my_ZWeights/20260311_ZCorrection_V6_PPb_zPt0-500.root`
- `[pPb-DY-analysis] ZWeightFile_PbP=my_ZWeights/20260311_ZCorrection_V6_PbP_zPt0-500.root`

Timestamp anchor for this run:
- `/tmp/kdeverea/revalidation_start_utc.txt`

No `pp` output was newer than this timestamp in the audited output areas.

---

## 4) Regenerated file manifests by category

### Upstream correction-stack regeneration

Manifest:
- `/tmp/kdeverea/revalidation_upstream_manifest.txt`

Counts:
- VZ: `6`
- Z correction: `4`
- track correction: `16`

### Downstream pPb/PbP plots

Manifest:
- `/tmp/kdeverea/revalidation_downstream_manifest.txt`

Counts:
- Z closure: `6`
- track closure: `24`
- central closure: `48`
- central note outputs (results/overlay subset): `8`

### Filtered official pPb/PbP deliverable manifest

Manifest:
- `/tmp/kdeverea/official_pbp_manifest_final.txt`

Count:
- `152`

This filtered manifest excludes `newVZFix` transitional products and was used for the naming audit.

---

## 5) Naming and completeness audit

Checklist:
- `/tmp/kdeverea/revalidation_checklist.txt`

PASS/FAIL results:
- Rollback pp restore: `PASS`
- Official correction provenance: `PASS`
- Conditional correction rerun requirement: `PASS (not required; provenance clean)`
- pPb/PbP plot regeneration: `PASS`
- Official manifest `newVZFix` absence: `PASS`
- pp outputs modified in this run: `PASS`
- Curated Overleaf copy executed: `PASS`
- pp files copied: `PASS`

Specific proofs:
- `pp` outputs touched in this run: `/tmp/kdeverea/pp_touched_in_revalidation.txt` -> `0` lines
- filtered official manifest `newVZFix` hits: none in `/tmp/kdeverea/official_pbp_manifest_final.txt`

Note on naming:
- Repository official deliverables were audited against the filtered official manifest and are clean of `newVZFix`.
- Overleaf note destinations still use several legacy `ZV5_trkV23` filenames in `src/*.tex`, so the curated copy intentionally remapped validated `ZV6/trkV24` content onto those exact note-referenced destination paths.

---

## 6) Final Overleaf copy manifest with pp exclusion proof

### Why explicit remapping was required

The note source still references several pPb/PbP figures under legacy filenames, for example:
- `figures/analysis/closure/pPb_ZPT..._ZV5_trkV23_nmix10-...`
- `figures/tracking/pPb_ZPT..._ZV5_trkV23_nmix10-nosub-closure-...`
- `figures/result/all_ZPT..._ZV5_trkV23_nmix10-...`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT...-overlay.pdf`

Therefore the final Overleaf step used an explicit curated source->destination map instead of basename matching.

### Copy manifest

Manifest:
- `/tmp/kdeverea/overleaf_recovery_copy_manifest.txt`

Count:
- copied files: `112`

Categories included:
- VZ plots: `4`
- Z correction plots: `2`
- Z closure plots: `6`
- track correction plots: `8`
- track closure plots: `24`
- central closure plots: `48`
- combined result plots: `10`
- pPb/PbP overlay plots: `10`

### pp exclusion proof

- pp files copied: `0`
- proof file: `/tmp/kdeverea/overleaf_pp_status_after_copy.txt` -> `0` lines

### Overleaf status after copy

Overleaf now shows only pPb/PbP/event/Z-correction/tracking/result/combining files modified or added by the curated remap.  
No `pp` paths remain modified.

---

## 7) Files modified in this task

Repository files updated:
- `Plots/20251001_pPbVZReweighting/run-reweight.sh`
- `Plots/20260120_CentralClosure/plot-central.sh`
- `Plots/20260115_ZResidualClosure/plot-Z.sh`
- `Plots/20251202_trackResidualClosure/plot-track.sh`

Report written:
- `.github/pPb_PbP_official_recovery_and_revalidation_execution.md`
