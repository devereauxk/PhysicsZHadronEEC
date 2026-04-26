# Analyzer execution summary: official NEW-VZ deployment as `ZV6/trkV24`

## Scope and outcome

Executed the reviewer deployment plan to treat NEW VZ weighting as official production for pPb/PbP under `ZV6_trkV24_nmix10`, regenerate official outputs, clean audit artifacts, and sync refreshed figures to Overleaf.

Key outcome:
- official script/tag migration completed,
- official production + plotting commands completed,
- audit-only central closure outputs removed,
- Overleaf refreshed in place with matched official PDFs,
- filtered official manifests are clean of `newVZFix`.

---

## 1) Scripts modified/added

Updated for official deployment:
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `Plots/20260120_CentralClosure/plot-central.sh`
- `Plots/20251202_trackResidualClosure/plot-track.sh`
- `Plots/20260115_ZResidualClosure/plot-Z.sh`
- `Plots/20260213_Central/plot-central.sh`
- `Plots/20260213_Central/plot-central-combined.sh`
- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`

Related cleanup script removal (already done during deployment prep):
- deleted `Plots/20260120_CentralClosure/plot-central-zstep-audit.sh`

---

## 2) Ordered command log

1. Safe promotion copy of NEW-derived inputs into official non-`newVZFix` filenames (with same-file guard) via `python3` copy map.
2. Official production chain:
   - `source SetupAnalysis.sh`
   - `cd MainAnalysis/20241102_ZhadronVsZPt`
   - `export SKIP_CLEAN=1 CUT_PARALLELISM=1 NTHREAD=8 NSLICE_FACTOR=1`
   - `./closure-trk.sh 0 1 1 && ./closure-Z.sh 0 1 1 && ./central.sh 0 1 1`
3. Official plotting chain:
   - `Plots/20260120_CentralClosure/plot-central.sh`
   - `Plots/20260115_ZResidualClosure/plot-Z.sh`
   - `Plots/20251202_trackResidualClosure/plot-track.sh`
   - `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
   - `Plots/20260213_Central/plot-central.sh`
   - `Plots/20260213_Central/plot-central-combined.sh`
4. Audit cleanup verification + removal:
   - remove any remaining `*zstepAudit*` / `*zstep_audit*` files in `Plots/20260120_CentralClosure`.
5. Overleaf in-place sync by basename match from refreshed source directories:
   - copied 134 files (manifest below).
6. Validation manifests + `newVZFix` exclusion checks.

---

## 3) Transitional -> official mapping table

| Transitional / NEW source | Official target |
|---|---|
| `Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_pPb.root` | `Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_pPb.root` |
| `Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_PbP.root` | `Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_PbP.root` |
| `MainAnalysis/20260115_ZCorrection/workflow/output/20260318_ZCorrection_V6_newVZFix_PPb_zPt0-500.root` | `MainAnalysis/20260115_ZCorrection/workflow/output/20260311_ZCorrection_V6_PPb_zPt0-500.root` |
| `MainAnalysis/20260115_ZCorrection/workflow/output/20260318_ZCorrection_V6_newVZFix_PbP_zPt0-500.root` | `MainAnalysis/20260115_ZCorrection/workflow/output/20260311_ZCorrection_V6_PbP_zPt0-500.root` |
| `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PPb_zPt{0-10,10-20,20-40,40-500}.root` | `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt{0-10,10-20,20-40,40-500}.root` |
| `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PbP_zPt{0-10,10-20,20-40,40-500}.root` | `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt{0-10,10-20,20-40,40-500}.root` |

Then promoted to main-analysis input stores:
- `MainAnalysis/20241102_ZhadronVsZPt/my_ZWeights/20260311_ZCorrection_V6_{PPb,PbP}_zPt0-500.root`
- `MainAnalysis/20241102_ZhadronVsZPt/my_residualWeights/20260311_TrackResidualCorrection_V24_ZWeight_V6_{PPb,PbP}_zPt*.root`

---

## 4) Deleted audit outputs

Deleted audit folder:
- `Plots/20260120_CentralClosure/plots/zstep_audit/`

Deleted remaining central-closure audit-named files:
- 24 files removed (`*zstepAudit*` in `Plots/20260120_CentralClosure/plots/{pPb,PbP}/...`).
- deletion log: `/tmp/kdeverea/deleted_zstep_audit_files.txt`

Post-check:
- `find Plots/20260120_CentralClosure -type f -name '*zstepAudit*'` -> `0` files.

---

## 5) Regenerated official output manifests

Filtered official ROOT manifest (no `newVZFix`, no `zstepAudit`):
- `/tmp/kdeverea/official_root_manifest_filtered.txt`
- count: `714`

Filtered official PDF manifest (no `newVZFix`, no `zstepAudit`):
- `/tmp/kdeverea/official_pdf_manifest_filtered.txt`
- count: `143`

Additional broad manifests:
- `/tmp/kdeverea/official_zv6_root_manifest.txt` (all `*ZV6_trkV24_nmix10*.root`)
- `/tmp/kdeverea/official_zv6_pdf_manifest.txt` (all `*ZV6_trkV24_nmix10*.pdf`)

---

## 6) Overleaf copy manifest

In-place Overleaf refresh executed by basename match against existing `~/OverleafZHadronInPPb/figures/**/*.pdf`:
- copied files: `134`
- manifest: `/tmp/kdeverea/overleaf_copy_manifest.txt`

This includes the correction stack figure categories requested in the plan where matching note filenames exist:
- VZ reweighting
- Z-correction
- Z-closure
- track-correction
- track-closure
- central-closure
- separated pPb/PbP comparison
- final result plots

---

## 7) Explicit no-`newVZFix` official-deliverable check

Check command:
- `grep -n 'newVZFix' /tmp/kdeverea/official_root_manifest_filtered.txt /tmp/kdeverea/official_pdf_manifest_filtered.txt`

Result:
- `CLEAN` (no matches)

Notes:
- Historical transitional files still exist elsewhere in the repo (expected for traceability), but filtered official deliverables generated for deployment are clean.
