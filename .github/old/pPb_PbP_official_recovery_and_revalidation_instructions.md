# Analyzer instructions: recover from incorrect Overleaf sync and revalidate official pPb/PbP VZ6 deployment

## Why this follow-up is required

The previous deployment run copied files to Overleaf using broad basename matching and unintentionally modified pp-facing plots.  
This follow-up is **pPb/PbP only** and must restore unintended changes before any new publish step.

---

## Hard scope boundary

- In scope: pPb, PbP products only.
- Out of scope: pp production and pp Overleaf assets.
- If a step would modify pp outputs, stop and fix the script/path filter first.

---

## Task 0 (first): rollback unintended Overleaf edits

1. Use the prior copy manifest from the last deployment (`/tmp/kdeverea/overleaf_copy_manifest.txt`) plus current `git status` in `~/OverleafZHadronInPPb` to identify files touched by that sync.
2. Revert all pp-targeted files to original state.
3. Also revert any non-pPb/PbP analysis-note files touched accidentally.
4. Keep this rollback separate from the new pPb/PbP deployment.

Validation required:
- explicit list of reverted Overleaf files,
- proof that pp sections are restored (git diff/status summary by path),
- statement that no pp file remains modified before pPb/PbP recopy begins.

---

## Task 1: provenance audit of official `ZV6/trkV24` correction inputs

Goal: verify whether current official pPb/PbP `ZV6` and `trkV24` correction files are truly derived from the corrected VZ mapping:
- pPb correction uses **PbP data + pPb MC**
- PbP correction uses **pPb data + PbP MC**

Required checks:
1. Confirm VZ reweighting script/config used the swapped data/MC pairing above.
2. Confirm official correction files in use by main analysis (`my_ZWeights`, `my_residualWeights`) are either:
   - directly produced with that mapping under official names, or
   - byte-identical copies of mapped NEW files.
3. If provenance is incomplete/ambiguous, mark as **FAIL** and rerun Task 2 end-to-end.

Evidence to provide:
- exact source files and destination files,
- checksum table (`sha256`) for mapping/copy proof,
- command/script references that establish the data/MC pairing.

---

## Task 2 (conditional rerun): regenerate pPb/PbP corrections with official naming if Task 1 fails

If Task 1 is not a clean PASS, rerun correction chain for pPb/PbP with official naming (no `newVZFix` token in deliverables):

1. Regenerate VZ weight files with corrected mapping.
2. Regenerate Z correction (`ZV6`) for pPb and PbP.
3. Regenerate track residual correction (`trkV24`) for pPb and PbP across required ZPT bins.
4. Promote outputs into official analysis input paths used by production scripts.

Rules:
- use script-driven flow only,
- keep tags official (`ZV6_trkV24_nmix10` family),
- no pp reruns.

---

## Task 3: full pPb/PbP plot re-production using most up-to-date corrections

Regenerate all required pPb/PbP plot families with official naming, overwriting stale official files as needed:

1. VZ plots (summary outputs used in note flow)
2. Z-correction plots
3. Z-closure plots (must include analysis-note required bins, including `ZPT0_500` where applicable)
4. track-correction plots
5. track-closure plots
6. central-value closure plots
7. pPb vs PbP separated comparison/overlay plots
8. pPb result plots (analysis-note binning)

Output naming policy:
- official names should carry `ZV6_trkV24` tag family.
- remove transitional naming from official products.

---

## Task 4: naming and completeness audit

Before any Overleaf copy:

1. Verify no required official deliverable is missing for pPb/PbP categories above.
2. Verify no transitional `newVZFix` naming remains among official deliverables.
3. Verify regenerated central closure, pPb/PbP comparison, and pPb results are using the latest official correction inputs (trace via file timestamps + input-file references in logs).
4. Verify pp outputs were not regenerated/modified in this run.

Produce a PASS/FAIL checklist with one line per category.

---

## Task 5 (gated): Overleaf copy for pPb/PbP only

Only execute this step if Tasks 0–4 all PASS.

Copy to Overleaf only the validated pPb/PbP figure set:
- no pp files,
- no broad basename/global sync,
- copy by explicit curated manifest.

Include:
- central closure,
- pPb/PbP separated comparison,
- pPb results,
- and the pPb/PbP correction-stack figures tied to VZ6/trkV24.

Validation required:
- source manifest and destination manifest,
- list of overwritten files,
- explicit “pp files copied: 0”.

---

## Required return summary

Write execution report to:

`.github/pPb_PbP_official_recovery_and_revalidation_execution.md`

Must include:
1. rollback evidence for incorrect Overleaf edits,
2. provenance verdict for official ZV6/trkV24 corrections (PASS/FAIL),
3. rerun details if triggered,
4. regenerated file manifests by category,
5. naming audit (`newVZFix` absence in official outputs),
6. final Overleaf copy manifest with pp exclusion proof.
