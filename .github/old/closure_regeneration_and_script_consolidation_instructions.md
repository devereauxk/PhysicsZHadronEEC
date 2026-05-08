# Analyzer Instructions: closure regeneration, pp diagnosis, and script consolidation after the 20260320 propagation

## Reviewer role and scope

Use the current working tree in `/home/kdeverea/PhysicsZHadronEEC` as the source of truth.

This is an analyzer production-and-repair task. Execute it sequentially unless a step explicitly says otherwise, and return a completion summary in:

- `.github/closure_regeneration_and_script_consolidation_execution.md`

Do **not** touch:

- `Plots/20260213_Central/plot-pp.sh`
- `Plots/20260213_Central/plot-dataMCComparison.sh`

unless you discover a blocker that makes this unavoidable, in which case stop and document it clearly in the summary.

---

## Reviewer findings you should treat as ground truth

### 1. The pPb / PbP correction producers already encode the switched V0.2 mapping

The current correction-derivation workflows are already using the switched physical mapping for the heavy-ion systems.

Evidence:

- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`

In those files, the branch labeled `PPb` is built from:

- `pPbSample/V0.2/PbPMC_Reco.root`
- `pPbSample/V0.2/PbPMC_Gen.root`
- `pPbSample/V0.2/PbPData_Reco.root`
- `mergedEPOS/PPbMC_Gen.root`

and the branch labeled `PbP` is built from:

- `pPbSample/V0.2/PPbMC_Reco.root`
- `pPbSample/V0.2/PPbMC_Gen.root`
- `pPbSample/V0.2/PPbData_Reco.root`
- `mergedEPOS/PbPMC_Gen.root`

Treat that switched-label convention as the canonical heavy-ion policy for V0.2 inputs in this task.

### 2. The canonical 20241102 closure entrypoints are no longer the single source of truth

The repository currently has duplicated rerun wrappers such as:

- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`
- `Plots/20260115_ZResidualClosure/plot-Z-newVZFix.sh`
- `Plots/20251202_trackResidualClosure/plot-track-newVZFix.sh`

This violates the new workflow standard. The canonical scripts that must become the maintained entrypoints are:

- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`

Fold any needed fixes directly into those scripts rather than preserving a parallel `*-newVZFix.sh` branch.

### 3. The pp Z closure is suspect and must be diagnosed first

The user reports that the pp Z-weight closure is not flat enough. That can indicate either:

- a partial job failure,
- a tag/input mismatch,
- or incorrect VZ usage during closure production.

Treat pp as the first diagnosis target, because if the pp Z correction root changes, the pp energy-extrapolation chain and combined final-result plots may also need to be rerun.

### 4. Downstream impact is conditional

Based on the live scripts, the heavy-ion central/result/overlay workflows already consume the switched heavy-ion mapping. Therefore:

- if the pPb / PbP correction ROOTs are unchanged after your audit, this task is primarily a closure-plot regeneration and labeling repair for the heavy-ion systems,
- but if any heavy-ion correction ROOT changes, you must propagate that change into the affected downstream pPb / PbP central, overlay, and combined-result plots.

Similarly for pp:

- if the pp Z correction root changes, rerun the dependent pp track-residual and energy-extrapolation chains and then refresh any affected final combined plots.

---

## Mandatory standards to enforce in this task

### A. Canonical-script standard

For official reruns in `MainAnalysis/20241102_ZhadronVsZPt`, maintain only one canonical script per stage:

- `closure-VZ.sh`
- `closure-Z.sh`
- `closure-trk.sh`
- `central.sh`

Required action:

1. Move any needed fixes from duplicate rerun scripts into the canonical script.
2. Stop using the duplicate `*-newVZFix.sh` scripts for official production.
3. Either delete those duplicate scripts or leave a short comment-only stub that points back to the canonical entrypoint. Do not leave two live production variants.

### B. Official naming/versioning standard

Do **not** promote ad hoc descriptor tags such as:

- `newVZFix`
- `skimVZOff`

into the official closure/correction/result naming used after this task.

Use the following rule:

1. If you are reproducing the same promoted correction family and the underlying correction ROOT is unchanged, overwrite the existing official tag family.
2. If the underlying correction materially changes, increment the canonical version family instead:
   - Z correction: `ZV*`
   - track correction: `trkV*`

Do not solve this by inventing another descriptive suffix.

### C. Heavy-ion V0.2 label rule

Whenever a V0.2 PA file is used as an input in correction derivation, closure production, or central production, apply the same switched physical-system convention already used in the correction producers.

In practical terms, the physical heavy-ion system label in the output must follow the correction-producer convention, not the raw basename of the V0.2 input file.

---

## Execution plan

### Stage 1. Audit whether the current correction ROOTs themselves are already correct

Before rerunning anything broad, audit these files and determine whether the existing promoted correction ROOTs are already consistent with the intended inputs:

- `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh`
- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pythia-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`

Check:

1. Which VZ roots they consume.
2. Which physical inputs they consume for pp, PPb, and PbP.
3. Whether the promoted pp / PPb / PbP correction ROOTs they produce are already the intended ones.

Minimum deliverable for this stage in your summary:

- a table listing the current promoted pp / PPb / PbP Z and track correction ROOTs,
- the exact script lines that feed them,
- and your conclusion for each system: `reuse` or `rerun`.

Reviewer expectation from current live-code inspection:

- pPb / PbP derivation scripts are likely already correct,
- pp may need a rerun because of the reported non-flat closure.

Do not assume that expectation is sufficient; verify it.

### Stage 2. Consolidate the canonical 20241102 scripts

Patch the canonical scripts in:

- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`

Tasks:

1. Move any required logic from the duplicate rerun scripts into the canonical scripts.
2. Ensure the canonical scripts are the only live official rerun entrypoints after your patch.
3. Update the canonical scripts so the heavy-ion labeling policy is applied consistently.
4. Replace descriptive tag fragments with canonical promoted naming.

Also align the touched companion plot wrappers so they follow the canonical script choice:

- `Plots/20260115_ZResidualClosure/plot-Z.sh`
- `Plots/20251202_trackResidualClosure/plot-track.sh`

If you retire the duplicate plot wrappers, do it in the same spirit: one maintained live entrypoint per task.

### Stage 3. Diagnose and fix the pp Z-closure problem first

Run the pp-only Z closure first.

Required path:

1. Verify that the canonical closure script is using the intended pp VZ root and pp Z correction root.
2. Rerun the pp Z closure production.
3. Regenerate the pp Z closure plots.
4. Decide whether the closure issue came from:
   - stale closure inputs only,
   - or a genuinely stale/incorrect pp Z correction root.

If the pp Z correction root must be regenerated:

1. rerun the pp Z correction producer,
2. compare the newly produced pp Z correction ROOT against the currently promoted one,
3. if it changed, rerun the dependent pp track-residual producer,
4. and then rerun the dependent pp energy-extrapolation products.

### Stage 4. Regenerate the heavy-ion Z and track closure plots with the canonical scripts

After the canonical-script consolidation, regenerate:

- the Z closure plots for the heavy-ion systems,
- the track closure plots for the heavy-ion systems,
- and any pp closure plots that were touched in Stage 3.

Requirements:

1. The output labels and plot labeling must follow the switched V0.2 heavy-ion convention consistently.
2. The closure plots must come from the canonical scripts, not the retired duplicates.
3. The promoted naming must not include ad hoc descriptors.

### Stage 5. Decide downstream rerun scope from the actual correction-root outcome

Use the following decision rule:

#### If only closure plots changed, but correction ROOTs did not

Then:

- do **not** rerun heavy-ion central/results/overlay plots,
- do **not** rerun pp energy-extrapolation plots,
- update only the closure-plot products and the Overleaf closure figures.

#### If the pp Z or pp track correction changed

Then rerun the dependent pp downstream chain:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `Plots/20260213_Central/plot-energyExtrapolation.sh`
- any final combined-result plotting that consumes the updated pp energy-extrapolated tag

#### If any PPb / PbP Z or track correction changed

Then rerun the affected heavy-ion downstream chain:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `Plots/20260213_Central/plot-central.sh`
- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
- `Plots/20260213_Central/plot-central-combined.sh`
- `Plots/20260213_Central/plot-zmass.sh`

Only rerun what is actually impacted by changed correction inputs. Document your logic.

### Stage 6. Update Overleaf for the changed closure/downstream figures

Once the corrected outputs are validated, copy the changed note-facing PDFs to:

- `~/OverleafZHadronInPPb/`

Follow the established reviewer standards:

1. derive the manifest from the touched `src/*.tex` references,
2. preserve the source basename exactly when copying,
3. update TeX references only if the promoted basename changed,
4. verify all changed `\includegraphics` paths resolve after the copy.

Minimum Overleaf scope:

- the refreshed Z-closure figures,
- the refreshed track-closure figures,
- and any downstream energy/result/overlay figures that actually changed in Stage 5.

---

## Validation requirements

Your summary must include:

### Script consolidation proof

- which canonical scripts were updated,
- which duplicate rerun wrappers were retired,
- and how you ensured the canonical scripts now carry the needed logic.

### Correction-root decision proof

For each of:

- pp Z correction,
- pp track correction,
- PPb Z correction,
- PbP Z correction,
- PPb track correction,
- PbP track correction,

state one of:

- `reused (unchanged)`
- `rerun (changed)`

and provide the evidence you used.

### Closure validation

- list the regenerated closure PDFs,
- state which systems were regenerated,
- and summarize whether the pp closure issue and the PPb/PbP labeling issue were resolved.

### Downstream impact decision

- state explicitly whether energy-extrapolation, heavy-ion overlays, and final results were rerun,
- and explain why.

### Overleaf validation

- list the copied Overleaf figures,
- list the touched TeX files if any,
- and confirm that the updated `\includegraphics` targets exist locally in the Overleaf tree.

---

## Stop conditions

Stop and report rather than guessing if:

1. the pp closure defect cannot be traced to either stale closure inputs or a stale pp correction root,
2. the heavy-ion switched-label policy is ambiguous in a touched code path,
3. or promoting a corrected output would require inventing another ad hoc descriptive tag instead of using the canonical versioning rule.
