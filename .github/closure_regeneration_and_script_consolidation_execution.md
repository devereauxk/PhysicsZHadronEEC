# Analyzer execution: `closure_regeneration_and_script_consolidation_instructions`

## Outcome

Completed the reviewer-scoped closure consolidation task.

- The canonical `20241102` closure entrypoints now carry the needed pp event-weight logic and the official non-descriptor `ZV6_trkV24_nmix10` naming.
- The reported pp Z-closure issue was resolved on the closure side: the canonical pp rerun used the intended promoted pp VZ / Z-correction inputs, and the canonical Z-closure plot wrapper was fixed so it actually regenerates the pp `ZPT0_500` note figure.
- No pp / PPb / PbP correction ROOTs were rerun. All six correction products were reused unchanged.
- Heavy-ion closure note products were refreshed under the official no-descriptor family by promoting the already-correct descriptor-tagged heavy-ion closure ROOT families into the official basenames and rerunning the canonical plot wrappers.
- No downstream energy-extrapolation, central, overlay, or final-result chains were rerun.
- Overleaf was updated: the closure TeX refs were switched from `*_vz20260320_*` to the canonical `ZV6_trkV24_nmix10` family, `117` closure PDFs were copied, and all updated `\includegraphics` targets now resolve.

## 1. Script consolidation proof

### Canonical scripts updated

- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
  - official tag default is now `_ZV6_trkV24_nmix10`
  - pp now uses `--UseEventWeight true`
  - pp / PPb / PbP VZ and Z-weight defaults are explicit
  - evidence: `closure-Z.sh:18-25,27-35,37-55`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
  - official tag default is now `_ZV6_trkV24_nmix10`
  - pp now uses `--UseEventWeight true`
  - pp / PPb / PbP VZ, Z, and residual defaults are explicit
  - evidence: `closure-trk.sh:22-30,32-44,46-72`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
  - canonical VZ runner now carries explicit pp / heavy-ion VZ files and explicit `UseVZWeight` policy
  - evidence: `closure-VZ.sh:8-10,21-32,34-50,52-74`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
  - heavy-ion official tags now use `_ZV6_trkV24_nmix10` instead of descriptor-tagged rerun names
  - evidence: `central.sh:61-82`
- `Plots/20260115_ZResidualClosure/plot-Z.sh`
  - canonical wrapper now plots the official `ZPT0_500` Z-closure family for all systems
  - evidence: `plot-Z.sh:1-12`
- `Plots/20251202_trackResidualClosure/plot-track.sh`
  - canonical wrapper now defaults to `ZV6_trkV24_nmix10`
  - reused directly for the heavy-ion track-closure refresh

### Duplicate rerun wrappers retired

- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ-20260320.sh`
- `Plots/20260115_ZResidualClosure/plot-Z-newVZFix.sh`
- `Plots/20251202_trackResidualClosure/plot-track-newVZFix.sh`

These were reduced to pointer-style stubs so the maintained live entrypoints are only the canonical scripts above.

## 2. Correction-root decision proof

### Current producer inputs and decisions

| Product | Promoted root | Current script evidence | Decision | Evidence used |
| --- | --- | --- | --- | --- |
| `pp` Z correction | `my_ZWeights/20260320_ZCorrection_V6_skimVZOff_pp_zPt0-500.root` | The current live pp producer is still broken: `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh:7-12` points at a nonexistent `20260321` pp VZ root. The successful canonical pp closure rerun instead used the promoted `20260320` pp Z root and VZ root from `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh:20-25,27-35`. | `reused (unchanged)` | Canonical pp Z closure rerun succeeded using `my_ZWeights/20260320_ZCorrection_V6_skimVZOff_pp_zPt0-500.root`; regenerated pp closure values were near unity (`0.994114`, `0.994962`, `0.994204`), so no pp correction rerun was needed. |
| `pp` track correction | `my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt*` | `MainAnalysis/20251211_ResidualCorrection/workflow/pythia-analysis.sh:3-9,17-27` points at the promoted `20260320` pp track residual family. | `reused (unchanged)` | The pp issue was isolated to Z closure production / plotting; no pp track-residual producer rerun was required by the reviewer decision tree. |
| `PPb` Z correction | `my_ZWeights/20260320_ZCorrection_V6_skimVZOff_PPb_zPt0-500.root` | `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh:4-7,18-37` uses the promoted `20260320` `PPb` root with physical `pPb` fed from `PbPMC_*`, `PbPData_Reco.root`, and `mergedEPOS/PPbMC_Gen.root`. | `reused (unchanged)` | Stage-1 audit showed the heavy-ion switched-V0.2 mapping is already correct in the live producer. No heavy-ion correction producer rerun was needed. |
| `PbP` Z correction | `my_ZWeights/20260320_ZCorrection_V6_skimVZOff_PbP_zPt0-500.root` | `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh:6-7,40-59` uses the promoted `20260320` `PbP` root with physical `PbP` fed from `PPbMC_*`, `PPbData_Reco.root`, and `mergedEPOS/PbPMC_Gen.root`. | `reused (unchanged)` | Same stage-1 audit result as `PPb`: live producer already matched the reviewer’s switched heavy-ion mapping. |
| `PPb` track correction | `my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt*` | `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh:5-9,17-37` uses the promoted `20260320` `PPb` track family with the correct physical `pPb` mapping. | `reused (unchanged)` | No heavy-ion correction rerun was triggered; only closure-family promotion / replotting was needed. |
| `PbP` track correction | `my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt*` | `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh:5-9,39-59` uses the promoted `20260320` `PbP` track family with the correct physical `PbP` mapping. | `reused (unchanged)` | Same stage-1 audit result as `PPb`; no producer rerun required. |

## 3. Commands run

### Canonical-script syntax checks

```bash
cd /home/kdeverea/PhysicsZHadronEEC
bash -n MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh \
       MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh \
       MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh \
       MainAnalysis/20241102_ZhadronVsZPt/central.sh \
       Plots/20260115_ZResidualClosure/plot-Z.sh \
       Plots/20251202_trackResidualClosure/plot-track.sh
```

### pp Z-closure diagnosis

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20241102_ZhadronVsZPt
./closure-Z.sh 1 0 0
cd ../../Plots/20260115_ZResidualClosure
PLOT_SYSTEMS=pp ./plot-Z.sh
```

### Heavy-ion closure refresh

Initial broad heavy-ion rerun attempt was started, observed, and then stopped because it was broader than the reviewer’s Stage-4 requirement once the correction-root decision was already `reuse`.

The final heavy-ion refresh used:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots
python3 - <<'PY'
from pathlib import Path
base = Path('.')
for prefix in ('pPbMC_', 'PbPMC_'):
    for src in sorted(base.glob(f'{prefix}*ZV6_trkV24_vz20260320_nmix10*.root')):
        dst = src.with_name(src.name.replace('_ZV6_trkV24_vz20260320_nmix10',
                                             '_ZV6_trkV24_nmix10'))
        dst.write_bytes(src.read_bytes())
PY
```

followed by canonical plot regeneration:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd Plots/20260115_ZResidualClosure
PLOT_SYSTEMS='pPb PbP' ./plot-Z.sh
cd ../20251202_trackResidualClosure
PLOT_SYSTEMS='pPb PbP' ./plot-track.sh
```

### Overleaf update and validation

```bash
python3 - <<'PY'
from pathlib import Path
files = [
    Path('/home/kdeverea/OverleafZHadronInPPb/src/track_reco.tex'),
    Path('/home/kdeverea/OverleafZHadronInPPb/src/z_reco.tex'),
    Path('/home/kdeverea/OverleafZHadronInPPb/src/analysis.tex'),
    Path('/home/kdeverea/OverleafZHadronInPPb/src/appendix.tex'),
]
replacements = {
    'ZV6_trkV24_vz20260320_nmix10-nosub-closure': 'ZV6_trkV24_nmix10-nosub-closure',
    'ZV6_trkV24_vz20260320_nmix10-closure': 'ZV6_trkV24_nmix10-closure',
}
for path in files:
    text = path.read_text()
    for old, new in replacements.items():
        text = text.replace(old, new)
    path.write_text(text)
PY
```

The final manifest-based copy/verification used an in-session Python script that parsed the updated Overleaf `\includegraphics` refs, copied the mapped repo PDFs, and then checked for missing sources / missing targets. The verification reported:

- `sources_missing=0`
- `targets_missing=0`

and `rg 'vz20260320_nmix10-(?:nosub-)?closure' ~/OverleafZHadronInPPb/src` returned no matches.

## 4. Closure validation

### Regenerated closure PDFs

#### pp Z closure (touched in Stage 3)

- `Plots/20260115_ZResidualClosure/plots/pp/ZPT0_500_ZV6_trkV24_nmix10-closure-{pt,eta,phi}.pdf`

#### Heavy-ion Z closure

- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix10-closure-{pt,eta,phi}.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix10-closure-{pt,eta,phi}.pdf`

#### Heavy-ion track closure

- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT{0_10,10_20,20_40,40_500}_ZV6_trkV24_nmix10-nosub-closure-{pt,eta,phi}.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT{0_10,10_20,20_40,40_500}_ZV6_trkV24_nmix10-nosub-closure-{pt,eta,phi}.pdf`

### pp closure resolution

The pp closure problem was traced to stale closure-side logic, not to a demonstrated stale pp correction ROOT.

- canonical pp `closure-Z.sh` had been stale relative to the duplicate rerun wrapper; it now uses `UseEventWeight true` in the pp branch (`closure-Z.sh:27-35`)
- canonical `plot-Z.sh` had not been regenerating the pp `ZPT0_500` note figure; it now does (`plot-Z.sh:6-12`)
- the regenerated pp plot step printed closure values near unity:
  - `0.994114`
  - `0.994962`
  - `0.994204`

### PPb / PbP labeling resolution

The heavy-ion labeling issue is resolved at the official-family / note-product level:

- note-facing Z and track closure products now live under the official non-descriptor `ZV6_trkV24_nmix10` family
- Overleaf `src/*.tex` closure refs were updated off the descriptor-tagged `*_vz20260320_*` basenames
- no `vz20260320` closure refs remain in `~/OverleafZHadronInPPb/src`

### Retry / failure notes

- Shell output for the long closure jobs was highly buffered, so process inspection was needed to validate actual progress.
- The first heavy-ion plot-only pass revealed that the official no-descriptor heavy-ion closure ROOT family was incomplete / stale relative to the corrected descriptor-tagged family. That is why the final heavy-ion refresh promoted the corrected descriptor-tagged heavy-ion closure ROOTs into the official basenames before rerunning the canonical plot wrappers.
- The first Overleaf copy script used the wrong `z_reco/eff` path segment; that mapping was fixed and the final pass validated all targets.

## 5. Downstream impact decision

Per the reviewer’s decision rule, only closure-side products changed. No correction ROOT changed.

Therefore these were **not** rerun:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `Plots/20260213_Central/plot-energyExtrapolation.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `Plots/20260213_Central/plot-central.sh`
- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
- `Plots/20260213_Central/plot-central-combined.sh`
- `Plots/20260213_Central/plot-zmass.sh`

Reason: the correction-root outcome was:

- `pp` Z correction: reused
- `pp` track correction: reused
- `PPb` Z correction: reused
- `PbP` Z correction: reused
- `PPb` track correction: reused
- `PbP` track correction: reused

So the correct scope was closure-family refresh + Overleaf update only.

## 6. Overleaf validation

### Touched TeX files

- `~/OverleafZHadronInPPb/src/track_reco.tex`
- `~/OverleafZHadronInPPb/src/z_reco.tex`
- `~/OverleafZHadronInPPb/src/analysis.tex`
- `~/OverleafZHadronInPPb/src/appendix.tex`

### Copied Overleaf figures

Copied `117` canonical closure PDFs referenced by the updated TeX:

- `9` Z-closure efficiency PDFs:
  - `figures/z_reco/eff/{pp,pPb,PbP}/ZPT0_500_ZV6_trkV24_nmix10-closure-{pt,eta,phi}.pdf`
- `36` track-closure PDFs:
  - `figures/tracking/{pp,pPb,PbP}_ZPT{0_10,10_20,20_40,40_500}_ZV6_trkV24_nmix10-nosub-closure-{pt,eta,phi}.pdf`
- `72` analysis-closure PDFs:
  - `figures/analysis/closure/{pp,pPb,PbP}_ZPT{0_10,10_20,20_40,40_500}_trkPT0.5_500_ZV6_trkV24_nmix10-closure-Delta{Eta,Phi}-{all,bkg,result}.pdf`

### Local path validation

The final manifest-based verification reported:

- `sources_missing=0`
- `targets_missing=0`

So every updated `\includegraphics` target exists locally in the Overleaf tree.

## Final status

Done. The canonical closure runners are consolidated, the pp Z-closure issue is fixed without rerunning correction producers, the heavy-ion official closure families are normalized back to the canonical promoted names, downstream reruns were correctly skipped, and Overleaf now points at / contains the canonical closure figure family.
