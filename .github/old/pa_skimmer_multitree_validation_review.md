# PA skimmer multitree validation review

## Scope

- `ReduceForest_test.cpp` now hard-wires `Tree` to nominal track selection when `WriteAllTrackSelectionTrees=false`; there is no user-facing `TrackSelectionMode` flag in the test skimmer path.
- Checked-in validation and benchmarking tools now live in `SampleGeneration/20250929_ReducedTreePA/` via `AuditMultitree.cpp` and `run_multitree_validation.sh`.
- The review below uses the checked-in runners and audit executable.

## hiBinUp values (reported only, no action taken)

| Sample | Output | Original hiBinUp values | Test hiBinUp values |
| --- | --- | --- | --- |
| PAData0000 | AP | `[10174256]` | `[0]` |
| PAData0000 | PA | `[]` | `[]` |
| PAData0005 | AP | `[]` | `[]` |
| PAData0005 | PA | `[21872432]` | `[0]` |
| PAMC | Reco | `[18194224]` | `[0]` |
| APMC | Reco | `[35802928]` | `[0]` |

## Correctness summary

| Sample | Output | Original entries | Test entries | Multitree nominal == original (ignoring hiBinUp/down) | Event info equal across Tree/Loose/Tight | Track order Loose >= Nominal >= Tight | Single-tree nominal == original |
| --- | --- | ---: | ---: | --- | --- | --- | --- |
| PAData0000 | AP | 1956 | 1956 | yes | yes | yes | yes |
| PAData0000 | PA | 0 | 0 | yes | yes | yes | yes |
| PAData0005 | AP | 0 | 0 | yes | yes | yes | yes |
| PAData0005 | PA | 4679 | 4679 | yes | yes | yes | yes |
| PAMC | Reco | 3296 | 3296 | yes | yes | yes | yes |
| APMC | Reco | 21134 | 21134 | yes | yes | yes | yes |

## Benchmark setup

- Runner: `SampleGeneration/20250929_ReducedTreePA/run_local_skim_test.sh`
- Mode: `WRITE_ALL_TRACK_SELECTION_TREES=true`
- Dataset: first 20 lexicographically matched files from `Samples/PAData/0000/HiForestAOD_*.root`
- Files used:
  - `Samples/PAData/0000/HiForestAOD_100.root`
  - `Samples/PAData/0000/HiForestAOD_101.root`
  - `Samples/PAData/0000/HiForestAOD_102.root`
  - `Samples/PAData/0000/HiForestAOD_103.root`
  - `Samples/PAData/0000/HiForestAOD_104.root`
  - `Samples/PAData/0000/HiForestAOD_106.root`
  - `Samples/PAData/0000/HiForestAOD_107.root`
  - `Samples/PAData/0000/HiForestAOD_108.root`
  - `Samples/PAData/0000/HiForestAOD_109.root`
  - `Samples/PAData/0000/HiForestAOD_10.root`
  - `Samples/PAData/0000/HiForestAOD_110.root`
  - `Samples/PAData/0000/HiForestAOD_111.root`
  - `Samples/PAData/0000/HiForestAOD_112.root`
  - `Samples/PAData/0000/HiForestAOD_113.root`
  - `Samples/PAData/0000/HiForestAOD_114.root`
  - `Samples/PAData/0000/HiForestAOD_115.root`
  - `Samples/PAData/0000/HiForestAOD_116.root`
  - `Samples/PAData/0000/HiForestAOD_117.root`
  - `Samples/PAData/0000/HiForestAOD_118.root`
  - `Samples/PAData/0000/HiForestAOD_119.root`

## Benchmark results

| NTHREAD | Elapsed time (s) | Output files produced |
| ---: | ---: | ---: |
| 1 | 137.27 | 40 |
| 2 | 27.56 | 40 |
| 5 | 11.53 | 40 |
| 10 | 6.52 | 40 |
| 15 | 5.47 | 40 |
| 20 | 3.52 | 40 |
| 30 | 3.58 | 40 |
| 40 | 3.51 | 40 |

### Diminishing-return point

- The timing improvement clearly saturates around `NTHREAD=20` for this local file-level parallel workflow.
- `NTHREAD=20` gave the best observed time in this sweep (`3.52 s`), while `30` and `40` were statistically flat/slightly worse (`3.58 s`, `3.51 s`).
- For this machine and this 20-file PAData test, pushing beyond `20` workers does not yield a meaningful gain.

## Trigger-efficiency workflow

- Added skim-time histogram production in `ReduceForest_test.cpp`:
  - `HltTree/TriggerTurnOn/HLTEffNumerator`
  - `HltTree/TriggerTurnOn/HLTEffDenominator`
- Added checked-in plotter: `Plots/20260326_HLTCurves/PlotHLTEfficiency.cpp`
- Added checked-in workflow runner: `Plots/20260326_HLTCurves/run_hlt_efficiency_study.sh`
- Verified local workflow:
  - processed `~20` PAData files with `run_local_skim_test.sh`
  - merged all skim outputs into a single ROOT file: `output/hlt_efficiency_study/merged_all.root`
  - also kept orientation-specific merged files: `merged_AP.root`, `merged_PA.root`
  - produced plots:
    - `output/hlt_efficiency_study/HLTEfficiency.pdf`
    - `output/hlt_efficiency_study/HLTEfficiency_AP.pdf`
    - `output/hlt_efficiency_study/HLTEfficiency_PA.pdf`

### Histogram sanity check

| File | Numerator entries | Denominator entries | Numerator integral | Denominator integral |
| --- | ---: | ---: | ---: | ---: |
| `merged_all.root` | 43 | 43 | 12 | 12 |
| `merged_AP.root` | 43 | 43 | 12 | 12 |
| `merged_PA.root` | 0 | 0 | 0 | 0 |

- For this particular `~20`-file local sample, the populated output is the `AP` run-window split; the `PA` split is empty, which is expected for these inputs.
- The merged-all file therefore matches the populated `AP` histograms in this test.
- The numerator never exceeds the denominator, and the plotter successfully reads the skim histograms from the merged file.

## Notes

- `hiBinUp` remains unstable in the original skim and is intentionally only reported.
- Empty PA/AP splits occur when a file lies outside that run window; parity checks still pass when both original and test outputs are empty.
- Detailed multitree-validation logs are under `/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/output/multitree_review/logs`.
- HLT workflow outputs are under `/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/output/hlt_efficiency_study`.
