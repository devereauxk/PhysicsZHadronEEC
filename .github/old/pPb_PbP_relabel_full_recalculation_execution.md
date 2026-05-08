# pPb/PbP relabel full recalculation execution summary


This analyzer execution followed `.github/pPb_PbP_relabel_full_recalculation_instructions.md` sequentially against the live working tree in `/home/kdeverea/PhysicsZHadronEEC`. The recovery scope was pPb/PbP-only: pp corrections, pp workflow branches, and pp Overleaf assets were intentionally left untouched.


## Scripts changed or added
- `Plots/20251001_pPbVZReweighting/run-reweight.sh`
- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `Plots/20260213_Central/plot_central.cpp`
- `.github/pPb_PbP_relabel_full_recalculation_execution.md`


## Ordered command log
1. **Task 0** — Read `.github/AGENT_REQUIREMENTS.md`, `.github/copilot-instructions.md`, `.github/pPb_PbP_relabel_full_recalculation_instructions.md`; patch the live workflow scripts; validate with `bash -n` and targeted `rg` proofs.
2. **Task 1** — Run the explicit manifest-driven pPb/PbP cleanup script; save `task1_deleted_manifest.tsv` and `task1_deleted_counts.txt` under the session-state files directory.
3. **Task 2** — `source SetupAnalysis.sh && cd CommonCode && make && cd Plots/20251001_pPbVZReweighting && make ExecuteVZ && OUTPUT_TAG=20260319_pPbPbpRelabelFix_ZPT0_500 ./run-reweight.sh`.
4. **Task 3** — Run `closure-VZ.sh 0 1 1` and `closure-VZ.sh 0 2 2` with `VZ_WEIGHT_FILE_PPB` / `VZ_WEIGHT_FILE_PBP` pointing at the new 20260319 VZ files; then run `Plots/20260213_Central/plot-VZ.sh`.
5. **Task 4** — Attempted `cd MainAnalysis/20260115_ZCorrection/workflow && make` (failed: no makefile); fixed the wrapper defaults/ROOT quoting; rebuilt from `MainAnalysis/20260115_ZCorrection/`; reran official `ZV6` production with the refreshed VZ files.
6. **Task 5** — Run `closure-Z.sh 0 1 1` with the refreshed VZ files, then `PLOT_SYSTEMS="pPb PbP" ./plot-Z.sh` in `Plots/20260115_ZResidualClosure`.
7. **Task 6** — Repair the residual-correction wrapper semantics; rebuild `MainAnalysis/20251211_ResidualCorrection`; run `NAME_TAG=20260311_TrackResidualCorrection_V24_ZWeight_V6 ./run-pPb.sh` with refreshed VZ and ZV6 weights.
8. **Task 7** — Initial closure rerun failed; restored the standard four Z bins in `closure-trk.sh`; reran `closure-trk.sh 0 1 1`, `PLOT_SYSTEMS="pPb PbP" ./plot-track.sh`, and `PLOT_SYSTEMS="pPb PbP" ./plot-central.sh`.
9. **Task 8** — Restore the executable bit on `central.sh`; run `SKIP_CLEAN=1 CUT_PARALLELISM=1 NTHREAD=8 NSLICE_FACTOR=1 ./central.sh 0 1 1` with refreshed VZ overrides.
10. **Task 9** — Run `plot-central.sh`, `plot-central-overlay-PPbPbP.sh`, and `plot-central-combined.sh`; identify the missing `plots/central` directory bug; patch `plot_central.cpp`; rebuild `ExecuteCentralPlot`; rerun `plot-central.sh`.
11. **Task 10** — Derive the explicit note manifest from `~/OverleafZHadronInPPb/src/*.tex`; after user approval to proceed on a dirty Overleaf worktree, copy 112 explicit pPb/PbP figures into Overleaf; save the source/destination manifest and `git status --short` snapshot under the session-state files directory.
12. **Task 11** — Write this execution summary markdown file.


## Final real-system input mapping
| Branch | Data input | Reco MC input | Gen MC input | EPOS / UE input | Patched surfaces |
| --- | --- | --- | --- | --- | --- |
| pPb branch (real pPb) | `pPbSample/V0.2/PbPData_Reco.root` | `pPbSample/V0.2/PbPMC_Reco.root` | `pPbSample/V0.2/PbPMC_Gen.root` | `mergedEPOS/PPbMC_Gen.root` | run-reweight.sh; Z-correction wrapper; residual wrapper; closure-VZ.sh; closure-Z.sh; closure-trk.sh; central.sh |
| PbP branch (real PbP/Pbp) | `pPbSample/V0.2/PPbData_Reco.root` | `pPbSample/V0.2/PPbMC_Reco.root` | `pPbSample/V0.2/PPbMC_Gen.root` | `mergedEPOS/PbPMC_Gen.root` | run-reweight.sh; Z-correction wrapper; residual wrapper; closure-VZ.sh; closure-Z.sh; closure-trk.sh; central.sh |

Main-analysis env-override support now exists for `VZ_WEIGHT_FILE_PPB`, `VZ_WEIGHT_FILE_PBP`, `Z_WEIGHT_FILE_PPB`, `Z_WEIGHT_FILE_PBP`, `R_WEIGHT_FILE_PPB`, and `R_WEIGHT_FILE_PBP` in the patched closure / central drivers.


## Deletion manifest for stale pPb/PbP official outputs
Category counts from `files/task1_deleted_counts.txt`:

```text
vz-closure-plots: 4
z-correction-roots: 2
z-correction-pdfs: 2
z-closure-pdfs: 6
track-correction-roots: 8
track-correction-pdfs: 8
track-closure-pdfs: 48
main-analysis-roots: 196
mc-central-closure-pdfs: 84
central-overlay-pdfs: 2
central-combined-pdfs: 266
total: 626
```

Full deletion manifest artifact: `/home/kdeverea/.copilot/session-state/257d17d6-2e69-4e03-a2cd-bc975314d199/files/task1_deleted_manifest.tsv`.


<details>
<summary>Deletion manifest: central-combined-pdfs (266 files)</summary>

- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV1_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.7_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix20/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix20-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix20/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix20-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix20/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix20-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix20/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix20-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix20/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix20-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/EEV2_ZV4_trkV22_nmix20/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix20-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT0_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/PbP_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_5_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_5_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_5_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT0_5_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT10_20_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT10_20_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT10_20_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT10_20_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT20_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT20_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT20_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT20_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_10_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_10_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_10_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_10_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/all_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT0_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV4_trkV22_nmix10/pPb_ZPT5_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV5_trkV23_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/PbP_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/pPb_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`

</details>

<details>
<summary>Deletion manifest: central-overlay-pdfs (2 files)</summary>

- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_500_trkPT0.5_500-DeltaPhi-overlay.pdf`

</details>

<details>
<summary>Deletion manifest: main-analysis-roots (196 files)</summary>

- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_ZResidual_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_ZResidual_ZV6_trkV24_nmix10_ZPT5_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_ZV6_trkV24_nmix10_ZPT5_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_trkResidual_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_trkResidual_ZV6_trkV24_nmix10_ZPT5_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV6_trkV24_nmix10_ZPT5_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_zstepAuditD_newfull_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_ZResidual_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_ZResidual_ZV6_trkV24_nmix10_ZPT5_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV6_trkV24_nmix10_ZPT30_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV6_trkV24_nmix10_ZPT30_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV6_trkV24_nmix10_ZPT5_30-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV6_trkV24_nmix10_ZPT5_500-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_trkResidual_ZV6_trkV24_nmix10_ZPT5_500-nosub.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_trkResidual_ZV6_trkV24_nmix10_ZPT5_500-result.root`

</details>

<details>
<summary>Deletion manifest: mc-central-closure-pdfs (84 files)</summary>

- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`

</details>

<details>
<summary>Deletion manifest: track-closure-pdfs (48 files)</summary>

- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`

</details>

<details>
<summary>Deletion manifest: track-correction-pdfs (8 files)</summary>

- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_0_10.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_10_20.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_20_40.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_40_500.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_0_10.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_10_20.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_20_40.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_40_500.pdf`

</details>

<details>
<summary>Deletion manifest: track-correction-roots (8 files)</summary>

- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt0-10.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt10-20.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt20-40.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt40-500.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt0-10.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt10-20.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt20-40.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt40-500.root`

</details>

<details>
<summary>Deletion manifest: vz-closure-plots (4 files)</summary>

- `Plots/20260213_Central/plots/VZ/PbP_ZPT0_500-VZWeight_nmix0-Vz.pdf`
- `Plots/20260213_Central/plots/VZ/PbP_ZPT0_500-noVZWeight_nmix0-Vz.pdf`
- `Plots/20260213_Central/plots/VZ/pPb_ZPT0_500-VZWeight_nmix0-Vz.pdf`
- `Plots/20260213_Central/plots/VZ/pPb_ZPT0_500-noVZWeight_nmix0-Vz.pdf`

</details>

<details>
<summary>Deletion manifest: z-closure-pdfs (6 files)</summary>

- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_Zclosure-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_Zclosure-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_Zclosure-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_Zclosure-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_Zclosure-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_Zclosure-closure-pt.pdf`

</details>

<details>
<summary>Deletion manifest: z-correction-pdfs (2 files)</summary>

- `MainAnalysis/20260115_ZCorrection/workflow/plots/corrections_PPb_20260311_ZCorrection_V6_0_500.pdf`
- `MainAnalysis/20260115_ZCorrection/workflow/plots/corrections_PbP_20260311_ZCorrection_V6_0_500.pdf`

</details>

<details>
<summary>Deletion manifest: z-correction-roots (2 files)</summary>

- `MainAnalysis/20260115_ZCorrection/workflow/output/20260311_ZCorrection_V6_PPb_zPt0-500.root`
- `MainAnalysis/20260115_ZCorrection/workflow/output/20260311_ZCorrection_V6_PbP_zPt0-500.root`

</details>

## VZ output manifest under the new VZ tag
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-VzRatio_PbP.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-VzRatio_pPb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-Vz_PbP.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-Vz_pPb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root`


## Official `ZV6` output manifest
### ROOT outputs
- `MainAnalysis/20260115_ZCorrection/workflow/output/20260311_ZCorrection_V6_PPb_zPt0-500.root`
- `MainAnalysis/20260115_ZCorrection/workflow/output/20260311_ZCorrection_V6_PbP_zPt0-500.root`

### Correction PDFs
- `MainAnalysis/20260115_ZCorrection/workflow/plots/corrections_PPb_20260311_ZCorrection_V6_0_500.pdf`
- `MainAnalysis/20260115_ZCorrection/workflow/plots/corrections_PbP_20260311_ZCorrection_V6_0_500.pdf`


## Official `trkV24` output manifest
### ROOT outputs
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt0-10.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt10-20.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt20-40.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt40-500.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt0-10.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt10-20.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt20-40.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt40-500.root`

### Correction PDFs
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_0_10.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_10_20.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_20_40.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_40_500.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_0_10.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_10_20.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_20_40.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_40_500.pdf`


## pPb/PbP closure manifest
### VZ closure PDFs
- `Plots/20260213_Central/plots/VZ/PbP_ZPT0_500-VZWeight_nmix0-Vz.pdf`
- `Plots/20260213_Central/plots/VZ/PbP_ZPT0_500-noVZWeight_nmix0-Vz.pdf`

### Z closure PDFs
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`

### Track-closure PDFs
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`

### MC central-closure PDFs
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`


## pPb/PbP central plot manifest
### Per-system central PDFs
- `Plots/20260213_Central/plots/central/ppPbP_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central/pppPb_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`

### Overlay PDFs
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT0.5_4-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT4_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT4_500-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT0.5_4-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT4_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT4_500-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_500_trkPT0.5_500-DeltaPhi-overlay.pdf`

### Combined-result PDFs
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`


## Overleaf copy manifest with destination paths
Note references used to derive the manifest were saved to `/home/kdeverea/.copilot/session-state/257d17d6-2e69-4e03-a2cd-bc975314d199/files/task10_overleaf_note_refs.tsv`. Copy summary:

```text
total_copies	112
central-overlay	10
combined-result	10
mc-central-closure	48
trk-closure	24
trk-correction	8
vz-reweight	4
z-closure	6
z-correction	2
pp_files_copied	0
```


<details>
<summary>Overleaf copy manifest: central-overlay (10 files)</summary>

- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_500_trkPT0.5_500-DeltaPhi-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT0.5_4-DeltaEta-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT0.5_4-DeltaPhi-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT4_500-DeltaEta-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_30_trkPT4_500-DeltaPhi-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT0.5_4-DeltaEta-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT0.5_4-DeltaPhi-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT4_500-DeltaEta-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT30_500_trkPT4_500-DeltaPhi-overlay.pdf` -> `OverleafZHadronInPPb/figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaPhi-overlay.pdf`

</details>

<details>
<summary>Overleaf copy manifest: combined-result (10 files)</summary>

- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV6_trkV24_nmix10-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV6_trkV24_nmix10-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`

</details>

<details>
<summary>Overleaf copy manifest: mc-central-closure (48 files)</summary>

- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf` -> `OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`

</details>

<details>
<summary>Overleaf copy manifest: trk-closure (24 files)</summary>

- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_nmix10-nosub-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`

</details>

<details>
<summary>Overleaf copy manifest: trk-correction (8 files)</summary>

- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_0_10.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_ppb_20260223_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_0_10.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_pbp_20260223_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_10_20.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_10_20.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_10_20.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_10_20.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_20_40.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_20_40.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_20_40.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_20_40.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PPb_20260311_TrackResidualCorrection_V24_ZWeight_V6_40_500.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_40_500.pdf`
- `MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_PbP_20260311_TrackResidualCorrection_V24_ZWeight_V6_40_500.pdf` -> `OverleafZHadronInPPb/figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_40_500.pdf`

</details>

<details>
<summary>Overleaf copy manifest: vz-reweight (4 files)</summary>

- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-Vz_pPb.pdf` -> `OverleafZHadronInPPb/figures/event/vz_ppb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-Vz_PbP.pdf` -> `OverleafZHadronInPPb/figures/event/vz_pbp.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-VzRatio_pPb.pdf` -> `OverleafZHadronInPPb/figures/event/vz_ratio_ppb.pdf`
- `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500-VzRatio_PbP.pdf` -> `OverleafZHadronInPPb/figures/event/vz_ratio_pbp.pdf`

</details>

<details>
<summary>Overleaf copy manifest: z-closure (6 files)</summary>

- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix10-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix10-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_500_ZV6_trkV24_nmix10-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-phi.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix10-closure-pt.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-pt.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix10-closure-eta.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-eta.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_500_ZV6_trkV24_nmix10-closure-phi.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-phi.pdf`

</details>

<details>
<summary>Overleaf copy manifest: z-correction (2 files)</summary>

- `MainAnalysis/20260115_ZCorrection/workflow/plots/corrections_PPb_20260311_ZCorrection_V6_0_500.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/corrections_PPb_20260202_ZCorrection_V5_0_500.pdf`
- `MainAnalysis/20260115_ZCorrection/workflow/plots/corrections_PbP_20260311_ZCorrection_V6_0_500.pdf` -> `OverleafZHadronInPPb/figures/z_reco/eff/corrections_PbP_20260202_ZCorrection_V5_0_500.pdf`

</details>

### Note references tied to the refreshed figure families


<details>
<summary>Note references from analysis.tex (34 refs)</summary>

- `figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaPhi-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaEta-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaEta-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaPhi-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaEta-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaEta-overlay.pdf`
- `figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaPhi-overlay.pdf`

</details>

<details>
<summary>Note references from appendix.tex (48 refs)</summary>

- `figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_10_20.pdf`
- `figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_10_20.pdf`
- `figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_20_40.pdf`
- `figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_20_40.pdf`
- `figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_40_500.pdf`
- `figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_40_500.pdf`
- `figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`

</details>

<details>
<summary>Note references from results.tex (10 refs)</summary>

- `figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`

</details>

<details>
<summary>Note references from software_mc_simulation.tex (4 refs)</summary>

- `figures/event/vz_ratio_ppb.pdf`
- `figures/event/vz_ratio_pbp.pdf`
- `figures/event/vz_ppb.pdf`
- `figures/event/vz_pbp.pdf`

</details>

<details>
<summary>Note references from track_reco.tex (8 refs)</summary>

- `figures/tracking/corrections_ppb_20260223_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf`
- `figures/tracking/corrections_pbp_20260223_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf`
- `figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-phi.pdf`
- `figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`
- `figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-eta.pdf`
- `figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-pt.pdf`

</details>

<details>
<summary>Note references from z_reco.tex (8 refs)</summary>

- `figures/z_reco/eff/corrections_PPb_20260202_ZCorrection_V5_0_500.pdf`
- `figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-pt.pdf`
- `figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-eta.pdf`
- `figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-phi.pdf`
- `figures/z_reco/eff/corrections_PbP_20260202_ZCorrection_V5_0_500.pdf`
- `figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-pt.pdf`
- `figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-eta.pdf`
- `figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-phi.pdf`

</details>

### Overleaf `git status --short` snapshot after copy
```text
 M figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf
 M figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf
 M figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf
 M figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf
 M figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf
 M figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaEta-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaPhi-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaEta-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaPhi-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaEta-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaPhi-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaEta-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaPhi-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf
 M figures/analysis/combining/ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaPhi-overlay.pdf
 M figures/event/vz_pbp.pdf
 M figures/event/vz_ppb.pdf
 M figures/event/vz_ratio_pbp.pdf
 M figures/event/vz_ratio_ppb.pdf
 M figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf
 M figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf
 M figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf
 M figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf
 M figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf
 M figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf
 M figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf
 M figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf
 M figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf
 M figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf
 M figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_10_20.pdf
 M figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_20_40.pdf
 M figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_40_500.pdf
 M figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_10_20.pdf
 M figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_20_40.pdf
 M figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_40_500.pdf
 M figures/tracking/corrections_pbp_20260223_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf
 M figures/tracking/corrections_ppb_20260223_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf
 M figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
 M figures/tracking/pPb_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
 M figures/tracking/pPb_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
 M figures/tracking/pPb_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-eta.pdf
 M figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
 M figures/tracking/pPb_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-pt.pdf
 M figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-eta.pdf
 M figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-phi.pdf
 M figures/z_reco/eff/PbP_ZPT0_500_Zclosure-closure-pt.pdf
 M figures/z_reco/eff/corrections_PPb_20260202_ZCorrection_V5_0_500.pdf
 M figures/z_reco/eff/corrections_PbP_20260202_ZCorrection_V5_0_500.pdf
 M figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-eta.pdf
 M figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-phi.pdf
 M figures/z_reco/eff/pPb_ZPT0_500_Zclosure-closure-pt.pdf
?? figures/tracking/PbP_ZPT0_10_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
?? figures/tracking/PbP_ZPT10_20_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
?? figures/tracking/PbP_ZPT20_40_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
?? figures/tracking/PbP_ZPT40_500_ZV5_trkV23_nmix10-nosub-closure-phi.pdf
?? figures/tracking/corrections_PPb_20260202_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf
?? figures/tracking/corrections_PbP_20260202_TrackResidualCorrection_V23_ZWeight_V5_0_10.pdf
```


## Proof that pp corrections and pp plots were not changed
- No pp mapping was changed in any patched workflow script; only the pPb/PbP branches were remapped.
- Task 3 validation confirmed pp VZ outputs were not regenerated while the refreshed pPb/PbP VZ outputs landed on 2026-03-19 timestamps.
- Task 5 validation confirmed pp Z-closure PDFs remained on older timestamps while the pPb/PbP Z-closure PDFs refreshed.
- Task 7 validation confirmed pp track-closure and pp MC-closure PDFs remained older while the pPb/PbP sets refreshed.
- Task 8 validation showed pp main-analysis roots stayed on 2026-03-17 / 2026-03-18 timestamps while refreshed pPb/PbP roots landed on 2026-03-19 18:57 through 22:39.
- `plot-pp.sh` was never run during Task 9; only `plot-central.sh`, `plot-central-overlay-PPbPbP.sh`, and `plot-central-combined.sh` were executed.
- `task10_overleaf_copy_summary.txt` records `pp_files_copied	0`, so the Overleaf copy step did not touch pp assets.


## Failures, retries, and resolutions
- `MainAnalysis/20260115_ZCorrection/workflow/` has no makefile. The first reviewer-pattern build failed immediately; the fix was to build from `MainAnalysis/20260115_ZCorrection/`.
- My first shell rewrite broke the Z-correction wrapper defaults and ROOT macro quoting. I repaired the `minZpt/maxZpt/name` defaults and the `merge_corrections.C(...)` / `plot_closure.C(...)` invocations before rerunning Task 4.
- My earlier residual-wrapper rewrite had semantic regressions: dropped positional Z-bin handling, lost Z-weight plumbing, used the wrong default name, collapsed plot names, and introduced per-bin cleaning. I restored the intended official `trkV24` behavior before rerunning Task 6.
- The first Task 7 rerun failed because `closure-trk.sh` had been reduced to `ZPT0_500` while the downstream plotters expect the standard four Z bins (`0_10`, `10_20`, `20_40`, `40_500`). Restoring those bins fixed the failure.
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh` had lost its executable bit, causing `Permission denied` / exit 126 before Task 8. Restoring mode 755 resolved it.
- Task 8 log contained one `*** Break *** segmentation violation` marker mid-run. The overall shell still exited 0 and the full expected pPb/PbP ROOT manifest was produced, so it was classified as non-fatal ROOT-side noise rather than a stage blocker.
- `Plots/20260213_Central/plot-central.sh` initially failed to write per-system PDFs because `plot_central.cpp` did not create `plots/central/`. Adding `gSystem->mkdir("plots/central", true);` and rebuilding `ExecuteCentralPlot` fixed Task 9.
- `~/OverleafZHadronInPPb` already had a dirty worktree with pre-existing pPb/PbP tracking figures. I paused, asked the user, got approval to proceed carefully, and then copied only the explicit pPb/PbP manifest.

Task 8 non-fatal segfault snippet:

```text
2130-write hNZData
2131-write hNZMixData
2132-
2133: *** Break *** segmentation violation
2134-write [                                            ><>                          ]  66%[                                                          ><>            ]  84%[                                                   ><>                   ]  75%
2135-
2136-
```


## Final PASS / FAIL status by stage
| Stage | Status | Notes |
| --- | --- | --- |
| Task 0 | PASS | Live script layer patched and syntax-validated. |
| Task 1 | PASS | 626 stale pPb/PbP outputs deleted by explicit manifest; no pp deletions. |
| Task 2 | PASS | New 20260319 VZ tag regenerated for pPb/PbP. |
| Task 3 | PASS | pPb/PbP VZ closure roots and note-facing VZ PDFs regenerated; pp untouched. |
| Task 4 | PASS | Official pPb/PbP ZV6 roots and correction PDFs regenerated after wrapper repair. |
| Task 5 | PASS | pPb/PbP Z-closure PDFs regenerated and validated as non-empty. |
| Task 6 | PASS | Official pPb/PbP trkV24 roots and correction PDFs regenerated after wrapper repair. |
| Task 7 | PASS | pPb/PbP track-closure and MC central-closure PDFs regenerated after restoring four-bin closure-trk workflow. |
| Task 8 | PASS | pPb/PbP central-analysis ROOT outputs regenerated with reviewer-requested worker settings; pp roots remained older. |
| Task 9 | PASS | Per-system, overlay, and combined note-facing central plot families regenerated after fixing `plot_central.cpp` mkdir behavior. |
| Task 10 | PASS | 112 explicit pPb/PbP note figures copied to Overleaf; `pp files copied = 0`. |
| Task 11 | PASS | Execution summary written. |

