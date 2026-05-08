# Analyzer Instructions: full pPb/PbP correction-stack recalculation after relabeling bug

## Reviewer scope and source of truth

Use the **current working tree** in `/home/kdeverea/PhysicsZHadronEEC` as the source of truth.

This is a **pPb/PbP-only** recovery. Do **not** modify pp corrections, pp plots, or pp Overleaf assets.

Execute the tasks below **sequentially** unless a step explicitly allows internal workflow parallelism.

---

## Hard physics mapping to enforce everywhere

The currently live scripts still encode the previous pPb/PbP assignment. Before rerunning anything, update the workflow scripts so they use the following **real** sample definitions:

### Real pPb
- data: `pPbSample/V0.2/PbPData_Reco.root`
- reco MC: `pPbSample/V0.2/PbPMC_Reco.root`
- gen MC: `pPbSample/V0.2/PbPMC_Gen.root`
- EPOS / UE input: use the live workflow convention `mergedEPOS/PPbMC_Gen.root`

### Real PbP / Pbp
- data: `pPbSample/V0.2/PPbData_Reco.root`
- reco MC: `pPbSample/V0.2/PPbMC_Reco.root`
- gen MC: `pPbSample/V0.2/PPbMC_Gen.root`
- EPOS / UE input: use the live workflow convention `mergedEPOS/PbPMC_Gen.root`

Important:
- The user’s statement about the physical datasets is authoritative.
- The repository’s EPOS helpers currently use `mergedEPOS/...`; keep that directory convention in script code while applying the corrected real-system mapping above.
- Do not change any pp branch, pp input, pp tag, or pp plotting script unless a shared helper absolutely requires a safe no-op parameterization.

---

## Tagging policy

Use a **new VZ tag** for traceability:

- `20260319_pPbPbpRelabelFix_ZPT0_500`

Use that new VZ tag for the VZ-weight files and VZ summary PDFs.

For downstream official correction outputs, **do not mint a new version**:
- keep the official Z-correction version family as `ZV6`
- keep the official track-correction version family as `trkV24`

That means the pPb/PbP Z and track correction outputs should overwrite the current official deliverables already consumed by the live scripts, unless you add path parameterization and then promote the validated outputs back into those official filenames in one controlled step.

---

## Task 0: patch the live scripts before any cleanup or rerun

Update the script layer first so every subsequent production step uses the corrected mapping.

At minimum, review and patch these files:

- `Plots/20251001_pPbVZReweighting/run-reweight.sh`
- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`

Patch goals:

1. Everywhere a branch is labeled `pPb`, make it use the **real pPb** sample mapping above.
2. Everywhere a branch is labeled `PbP`, make it use the **real PbP/Pbp** sample mapping above.
3. Keep pp code paths untouched.
4. Make the main-analysis closure/central scripts accept env overrides for VZ weight paths if needed, mirroring the style already used in:
   - `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
   - `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`

Recommended env-variable pattern for the main-analysis scripts:
- `VZ_WEIGHT_FILE_PPB`
- `VZ_WEIGHT_FILE_PBP`
- if useful for consistency, also allow overrides for:
  - `Z_WEIGHT_FILE_PPB`
  - `Z_WEIGHT_FILE_PBP`
  - `R_WEIGHT_FILE_PPB`
  - `R_WEIGHT_FILE_PBP`

Validation required before moving on:

- `rg` proof that every `pPb` branch now points to `PbPData_Reco`, `PbPMC_Reco`, `PbPMC_Gen`
- `rg` proof that every `PbP` branch now points to `PPbData_Reco`, `PPbMC_Reco`, `PPbMC_Gen`
- proof that pp branches remain unchanged

If any script still mixes the old mapping, stop and fix it before continuing.

---

## Task 1: remove stale pPb/PbP official outputs before rerun

Before regenerating anything, remove the existing **pPb/PbP** official products so the rerun cannot be confused with stale outputs.

Do **not** remove pp outputs.

Clean only the pPb/PbP families tied to the official stack and the soon-to-be-regenerated VZ products.

Minimum cleanup scope:

### VZ
- stale pPb/PbP files for the new VZ tag if they already exist under:
  - `Plots/20251001_pPbVZReweighting/summary/`
- stale pPb/PbP VZ-closure plots under:
  - `Plots/20260213_Central/plots/VZ/`

### Z correction
- pPb/PbP official ROOT outputs under:
  - `MainAnalysis/20260115_ZCorrection/workflow/output/`
- pPb/PbP correction PDFs under:
  - `MainAnalysis/20260115_ZCorrection/workflow/plots/`
- pPb/PbP Z-closure PDFs under:
  - `Plots/20260115_ZResidualClosure/plots/`

### Track residual correction
- pPb/PbP official ROOT outputs under:
  - `MainAnalysis/20251211_ResidualCorrection/workflow/output/`
- pPb/PbP correction PDFs under:
  - `MainAnalysis/20251211_ResidualCorrection/workflow/plots/`
- pPb/PbP track-closure PDFs under:
  - `Plots/20251202_trackResidualClosure/plots/`

### Main-analysis and central products
- pPb/PbP official result ROOTs under:
  - `MainAnalysis/20241102_ZhadronVsZPt/plots/`
- pPb/PbP MC central-closure PDFs under:
  - `Plots/20260120_CentralClosure/plots/pPb/`
  - `Plots/20260120_CentralClosure/plots/PbP/`
- pPb/PbP separated overlay PDFs under:
  - `Plots/20260213_Central/plots/central_overlay_PPbPbP/`
- combined pPb final-result PDFs under:
  - `Plots/20260213_Central/plots/central_combined/`

Use explicit path filters. Do not do broad repo-wide deletion. Do not touch pp.

Validation required:
- manifest of deleted files by category
- explicit proof that deleted files are pPb/PbP-only
- explicit proof that no pp file was deleted

---

## Task 2: rebuild and recalculate VZ weights with the corrected mapping

Environment/bootstrap:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd CommonCode && make
```

Then rebuild and run the VZ workflow:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting
make ExecuteVZ
OUTPUT_TAG=20260319_pPbPbpRelabelFix_ZPT0_500 ./run-reweight.sh
```

Requirements:

1. The patched script must produce the output label `pPb` from the **real pPb** mapping.
2. The patched script must produce the output label `PbP` from the **real PbP/Pbp** mapping.
3. Keep pp VZ files untouched.

Expected VZ outputs:
- `summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root`
- `summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root`
- matching pPb/PbP VZ PDFs from the same tag

---

## Task 3: regenerate VZ closure plots for note replacement

Use the main-analysis VZ closure workflow with the corrected sample mapping and the **new VZ tag**.

Requirements:
- regenerate both the `noVZWeight_nmix0` and `VZWeight_nmix0` inputs for pPb/PbP only
- use the new VZ weight files for the weighted branch
- use the note kinematics already encoded in the live scripts: `ZPT 0_500`, `trkPT 1_10`

Recommended command sequence after patching `closure-VZ.sh` for the corrected mapping and VZ env overrides:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root \
VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root \
./closure-VZ.sh 0 1 1

VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root \
VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root \
./closure-VZ.sh 0 2 2

cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central
./plot-VZ.sh
```

Validation required:
- list the regenerated pPb/PbP VZ closure ROOT inputs
- list the regenerated pPb/PbP VZ closure PDFs
- confirm pp VZ outputs were not regenerated

---

## Task 4: regenerate the official Z correction (`ZV6`) with the corrected mapping

Use the corrected VZ files from Task 2 while keeping the official Z-correction version family.

Recommended command pattern:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow
make
VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root \
VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root \
NAME_TAG=20260311_ZCorrection_V6 \
./pPb-DY-analysis.sh
```

Requirements:

1. The pPb branch must use the real pPb MC inputs.
2. The PbP branch must use the real PbP/Pbp MC inputs.
3. Output files must overwrite the current official pPb/PbP `V6` files, not create a new `V7`.

Expected official outputs:
- `output/20260311_ZCorrection_V6_PPb_zPt0-500.root`
- `output/20260311_ZCorrection_V6_PbP_zPt0-500.root`

---

## Task 5: regenerate Z-closure plots for note replacement

After Task 4 completes, regenerate the Z-closure products and plots for pPb/PbP only.

Recommended sequence:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root \
VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root \
./closure-Z.sh 0 1 1

cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure
PLOT_SYSTEMS="pPb PbP" ./plot-Z.sh
```

Required note-facing kinematics:
- `ZPT 0_500`
- `trkPT 0.5_500`

Validation required:
- regenerated pPb/PbP Z-closure PDF list
- proof that the PDFs are non-empty
- proof that pp Z-closure files were not regenerated

---

## Task 6: regenerate the official track residual correction (`trkV24`) with the corrected mapping

Use the corrected VZ files and the freshly regenerated official `ZV6` files.

Recommended sequence:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20251211_ResidualCorrection/workflow
make
VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root \
VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root \
Z_WEIGHT_FILE_PPB=my_ZWeights/20260311_ZCorrection_V6_PPb_zPt0-500.root \
Z_WEIGHT_FILE_PBP=my_ZWeights/20260311_ZCorrection_V6_PbP_zPt0-500.root \
NAME_TAG=20260311_TrackResidualCorrection_V24_ZWeight_V6 \
./run-pPb.sh
```

Requirements:

1. pPb branch uses the real pPb MC mapping.
2. PbP branch uses the real PbP/Pbp MC mapping.
3. Outputs overwrite the current official `trkV24` files; do not create `trkV25`.
4. Keep the standard four Z bins:
   - `0-10`
   - `10-20`
   - `20-40`
   - `40-500`

Expected official outputs:
- `output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt0-10.root`
- `output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt10-20.root`
- `output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt20-40.root`
- `output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt40-500.root`
- the corresponding `PbP` files

---

## Task 7: regenerate track-closure and MC central-closure plots

After Task 6 completes, regenerate the pPb/PbP closure plots used to validate the full correction stack.

### Track closure

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root \
VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root \
./closure-trk.sh 0 1 1

cd /home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure
PLOT_SYSTEMS="pPb PbP" ./plot-track.sh
```

### MC central closure

```bash
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure
PLOT_SYSTEMS="pPb PbP" ./plot-central.sh
```

Requirements:
- use the official `ZV6_trkV24_nmix10` tag family for the closure stack
- regenerate pPb and PbP only
- keep the standard closure kinematics already encoded in the scripts

Validation required:
- list of regenerated pPb/PbP track-closure PDFs
- list of regenerated pPb/PbP MC central-closure PDFs
- proof that pp closure PDFs were not regenerated

---

## Task 8: rerun the main pPb/PbP central analysis with the corrected inputs

Now regenerate the official pPb/PbP result ROOT files consumed by the note plotting layer.

Recommended sequence:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=8
export NSLICE_FACTOR=1
VZ_WEIGHT_FILE_PPB=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root \
VZ_WEIGHT_FILE_PBP=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root \
./central.sh 0 1 1
```

Requirements:
- regenerate pPb/PbP data and MC official ROOT outputs with the corrected physical mapping
- do not regenerate pp
- keep the official `ZV6_trkV24_nmix10` downstream tag family

Validation required:
- manifest of regenerated `MainAnalysis/20241102_ZhadronVsZPt/plots/` pPb/PbP ROOT outputs
- explicit proof that no pp ROOT outputs were regenerated

---

## Task 9: regenerate note-facing central plots

After Task 8 completes, regenerate the pPb/PbP note-facing plot families in this order:

1. per-system central plots
2. pPb vs PbP overlay/comparison plots for the combining section
3. combined pPb final-result plots

Recommended sequence:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central
./plot-central.sh
./plot-central-overlay-PPbPbP.sh
./plot-central-combined.sh
```

Requirements:
- do not touch `plot-pp.sh`
- do not regenerate pp correction products
- it is fine for these plotters to read existing pp reference files if needed; the prohibition is on changing pp outputs

Deliverables to check:
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/`
- `Plots/20260213_Central/plots/central_combined/`
- any refreshed pPb/PbP PDFs under `Plots/20260213_Central/plots/pp/`-free directories that correspond to the note

Validation required:
- list of regenerated overlay PDFs for the combining section
- list of regenerated combined-result PDFs
- timestamps proving they are newer than the deleted stale versions

---

## Task 10: copy updated pPb/PbP correction and result plots to Overleaf

Only do this after Tasks 0-9 pass.

Copy only the refreshed pPb/PbP figure set. Do **not** perform a broad global sync and do **not** copy pp files.

Use an explicit manifest derived from the note references in `~/OverleafZHadronInPPb/src/*.tex`.

The updated plot families must cover:

1. VZ reweighting / VZ closure figures
2. Z-correction figures
3. Z-closure figures
4. track-correction figures
5. track-closure figures
6. MC central-closure figures
7. pPb vs PbP overlay/comparison figures for the combining section
8. combined pPb final-result figures

Likely Overleaf target areas include:
- `~/OverleafZHadronInPPb/figures/analysis/closure/`
- `~/OverleafZHadronInPPb/figures/analysis/combining/`
- `~/OverleafZHadronInPPb/figures/result/`
- `~/OverleafZHadronInPPb/figures/tracking/`
- `~/OverleafZHadronInPPb/figures/z_reco/eff/`
- `~/OverleafZHadronInPPb/figures/event/`

Use the actual `\includegraphics{...}` references in the Overleaf source to decide the final manifest.

Validation required:
- source manifest
- destination manifest
- `git status --short` summary in `~/OverleafZHadronInPPb`
- explicit confirmation that pp files copied = `0`
- explicit confirmation that every note figure tied to these corrections now points to a refreshed file

---

## Required completion summary

Write the execution summary to:

- `.github/pPb_PbP_relabel_full_recalculation_execution.md`

The summary must include:

1. exact scripts changed or added
2. ordered command log
3. final file-level mapping table showing real pPb and real PbP/Pbp inputs used
4. deletion manifest for stale pPb/PbP official outputs
5. VZ output manifest under the new VZ tag
6. official `ZV6` output manifest
7. official `trkV24` output manifest
8. pPb/PbP closure manifest:
   - VZ closure
   - Z closure
   - track closure
   - MC central closure
9. pPb/PbP central-overlay and combined-result PDF manifests
10. Overleaf copy manifest with destination paths
11. proof that pp corrections and pp plots were not changed
12. failures/retries and how they were resolved
13. final PASS/FAIL status for each major stage

---

## Stop conditions

Stop and report back immediately if any of the following happens:

- a required step would modify pp outputs
- the corrected pPb/PbP mapping cannot be applied consistently in one of the official scripts
- the Overleaf copy manifest would require broad basename matching rather than an explicit curated list
- a stale output cannot be unambiguously distinguished from the new pPb/PbP rerun products
