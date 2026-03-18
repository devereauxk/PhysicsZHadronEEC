# Follow-up execution summary: pPb/PbP VZ-weight fix

## Ordered command log

- `cd Plots/20260213_Central && bash ./plot-central-combined.sh`
- `python3` extract `all_*` refs from `~/OverleafZHadronInPPb/src/results.tex`, snapshot before/after timestamps, and copy refreshed files to `~/OverleafZHadronInPPb/figures/result/`
- `cd Plots/20260120_CentralClosure && bash ./plot-central-newVZFix.sh`
- `python3` generate Task-2 validations: generated/overridden file list, exploratory integral table, worsened-bin report
- `python3` /tmp hygiene pass (move `/tmp/copilot-tool-output-*` and `/tmp/config_*` under `/tmp/kdeverea`, create before/after/moved/removed logs)

## Task 1: combined pPb+PbP Overleaf plots (current weights)

References in `src/results.tex` (all matched):
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

Source ROOT inputs used by `plot_central_combined.cpp` (all exist):
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10_ZPT5_30-nosub.root` (size=1440845)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV5_trkV23_nmix10_ZPT5_30-nosub.root` (size=773744)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT5_30-nosub.root` (size=1924142)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_ZV5_trkV23_nmix10_ZPT5_30-nosub.root` (size=1333150)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT5_30-nosub.root` (size=1928514)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10_ZPT30_500-nosub.root` (size=1069192)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV5_trkV23_nmix10_ZPT30_500-nosub.root` (size=614190)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT30_500-nosub.root` (size=1668381)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_ZV5_trkV23_nmix10_ZPT30_500-nosub.root` (size=1034987)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT30_500-nosub.root` (size=1669068)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10_ZPT5_500-nosub.root` (size=1403731)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV5_trkV23_nmix10_ZPT5_500-nosub.root` (size=761009)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT5_500-nosub.root` (size=1704874)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_ZV5_trkV23_nmix10_ZPT5_500-nosub.root` (size=700395)
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT5_500-nosub.root` (size=1703962)

Generated PDFs (refreshed source set):
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`

Timestamp validation for Overleaf targets (`changed=1` for all 10):

| ref | before mtime | after mtime | changed |
|---|---:|---:|---:|
| `figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf` | 1773803447.711181 | 1773811042.1507382 | 1 |
| `figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf` | 1773803437.2541645 | 1773811031.5497239 | 1 |
| `figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf` | 1773803439.8761685 | 1773811034.1837273 | 1 |
| `figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf` | 1773803442.5171728 | 1773811036.829731 | 1 |
| `figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf` | 1773803445.1051767 | 1773811039.5227346 | 1 |
| `figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf` | 1773803447.4091804 | 1773811041.848738 | 1 |
| `figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf` | 1773803436.971164 | 1773811031.2647233 | 1 |
| `figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf` | 1773803439.599168 | 1773811033.905727 | 1 |
| `figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf` | 1773803442.2301722 | 1773811036.5427306 | 1 |
| `figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf` | 1773803444.8331764 | 1773811039.2507343 | 1 |

## Task 2: exploratory newVZFix central-closure plots (no Overleaf copy)

Generated/overridden closure files in `Plots/20260120_CentralClosure/plots/` (48 total):
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-result.pdf`

Exploratory closure integral summary (`DeltaPhi_Result0.5_500`, same format as prior summary):

| collision | ZPT | DeltaPhi integral | pass `<0.15` | file |
|---|---|---:|---:|---|
| pPbMC | 0_10 | -0.031074 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root` |
| pPbMC | 10_20 | -0.0559799 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root` |
| pPbMC | 20_40 | -0.0663023 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root` |
| pPbMC | 40_500 | 0.0290629 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root` |
| PbPMC | 0_10 | -0.0111509 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT0_10-result.root` |
| PbPMC | 10_20 | -0.0144956 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT10_20-result.root` |
| PbPMC | 20_40 | -0.015367 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT20_40-result.root` |
| PbPMC | 40_500 | 0.0332732 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_newVZFix_ZV6_trkV24_nmix10_ZPT40_500-result.root` |

Bins that worsen vs current baseline (`abs_delta_improvement < 0`):

| collision | ZPT | observable | current Δ | new Δ | improvement |
|---|---|---|---:|---:|---:|
| pPbMC | 0_10 | DeltaEta | 0.191548 | 0.248517 | -0.0569691 |
| pPbMC | 10_20 | DeltaPhi | 0.0567196 | 0.0604084 | -0.00368881 |
| pPbMC | 20_40 | DeltaPhi | -0.0591647 | 0.098061 | -0.0388963 |
| pPbMC | 40_500 | DeltaEta | -0.35435 | -0.40903 | -0.0546799 |
| PbPMC | 10_20 | DeltaEta | 0.0258262 | 0.0658853 | -0.0400592 |
| PbPMC | 20_40 | DeltaEta | -0.165045 | -0.242387 | -0.0773422 |

Per instructions, exploratory Task-2 outputs were **not** copied to Overleaf.

## Task 3: /tmp hygiene (`/tmp/kdeverea`)

Before listing summary: `/tmp/kdeverea/tmp_hygiene_before.csv`

After listing summary: `/tmp/kdeverea/tmp_hygiene_after.csv` (no matching task temp files left under `/tmp` root).

Files moved: **45**
- `/tmp/config_ppnorm_40350.sh -> /tmp/kdeverea/config_ppnorm_40350.sh`
- `/tmp/config_ppnorm_fix_extended.sh -> /tmp/kdeverea/config_ppnorm_fix_extended.sh`
- `/tmp/config_ppnorm_fix_gate.sh -> /tmp/kdeverea/config_ppnorm_fix_gate.sh`
- `/tmp/copilot-tool-output-1773711539496-r61ba9.txt -> /tmp/kdeverea/copilot-tool-output-1773711539496-r61ba9.txt`
- `/tmp/copilot-tool-output-1773711600676-qgqq4a.txt -> /tmp/kdeverea/copilot-tool-output-1773711600676-qgqq4a.txt`
- `/tmp/copilot-tool-output-1773716528634-y6713u.txt -> /tmp/kdeverea/copilot-tool-output-1773716528634-y6713u.txt`
- `/tmp/copilot-tool-output-1773717018059-qzmwjv.txt -> /tmp/kdeverea/copilot-tool-output-1773717018059-qzmwjv.txt`
- `/tmp/copilot-tool-output-1773717693346-22t9p0.txt -> /tmp/kdeverea/copilot-tool-output-1773717693346-22t9p0.txt`
- `/tmp/copilot-tool-output-1773717916654-dshwcd.txt -> /tmp/kdeverea/copilot-tool-output-1773717916654-dshwcd.txt`
- `/tmp/copilot-tool-output-1773717988944-h3wnwu.txt -> /tmp/kdeverea/copilot-tool-output-1773717988944-h3wnwu.txt`
- `/tmp/copilot-tool-output-1773718914496-m7xi5c.txt -> /tmp/kdeverea/copilot-tool-output-1773718914496-m7xi5c.txt`
- `/tmp/copilot-tool-output-1773718937586-8bh1mz.txt -> /tmp/kdeverea/copilot-tool-output-1773718937586-8bh1mz.txt`
- `/tmp/copilot-tool-output-1773718990070-i9ynls.txt -> /tmp/kdeverea/copilot-tool-output-1773718990070-i9ynls.txt`
- `/tmp/copilot-tool-output-1773720458703-hjdin6.txt -> /tmp/kdeverea/copilot-tool-output-1773720458703-hjdin6.txt`
- `/tmp/copilot-tool-output-1773722414206-57ead7.txt -> /tmp/kdeverea/copilot-tool-output-1773722414206-57ead7.txt`
- `/tmp/copilot-tool-output-1773722637725-jy7w8u.txt -> /tmp/kdeverea/copilot-tool-output-1773722637725-jy7w8u.txt`
- `/tmp/copilot-tool-output-1773724100462-ikpff3.txt -> /tmp/kdeverea/copilot-tool-output-1773724100462-ikpff3.txt`
- `/tmp/copilot-tool-output-1773724267058-gfm0vb.txt -> /tmp/kdeverea/copilot-tool-output-1773724267058-gfm0vb.txt`
- `/tmp/copilot-tool-output-1773724608293-c1rep9.txt -> /tmp/kdeverea/copilot-tool-output-1773724608293-c1rep9.txt`
- `/tmp/copilot-tool-output-1773725913842-nfhqpp.txt -> /tmp/kdeverea/copilot-tool-output-1773725913842-nfhqpp.txt`
- `/tmp/copilot-tool-output-1773769441131-hxf174.txt -> /tmp/kdeverea/copilot-tool-output-1773769441131-hxf174.txt`
- `/tmp/copilot-tool-output-1773771152605-aspaca.txt -> /tmp/kdeverea/copilot-tool-output-1773771152605-aspaca.txt`
- `/tmp/copilot-tool-output-1773773421613-gs4wjr.txt -> /tmp/kdeverea/copilot-tool-output-1773773421613-gs4wjr.txt`
- `/tmp/copilot-tool-output-1773773483880-7mj3zp.txt -> /tmp/kdeverea/copilot-tool-output-1773773483880-7mj3zp.txt`
- `/tmp/copilot-tool-output-1773776447078-109t33.txt -> /tmp/kdeverea/copilot-tool-output-1773776447078-109t33.txt`
- `/tmp/copilot-tool-output-1773778361454-bdwuh4.txt -> /tmp/kdeverea/copilot-tool-output-1773778361454-bdwuh4.txt`
- `/tmp/copilot-tool-output-1773783541390-ddgrds.txt -> /tmp/kdeverea/copilot-tool-output-1773783541390-ddgrds.txt`
- `/tmp/copilot-tool-output-1773784487139-noa9mu.txt -> /tmp/kdeverea/copilot-tool-output-1773784487139-noa9mu.txt`
- `/tmp/copilot-tool-output-1773785808906-diyud7.txt -> /tmp/kdeverea/copilot-tool-output-1773785808906-diyud7.txt`
- `/tmp/copilot-tool-output-1773788684373-6azjfw.txt -> /tmp/kdeverea/copilot-tool-output-1773788684373-6azjfw.txt`
- `/tmp/copilot-tool-output-1773794066045-qzco9p.txt -> /tmp/kdeverea/copilot-tool-output-1773794066045-qzco9p.txt`
- `/tmp/copilot-tool-output-1773794082493-bgqn4i.txt -> /tmp/kdeverea/copilot-tool-output-1773794082493-bgqn4i.txt`
- `/tmp/copilot-tool-output-1773794082493-cnhiz9.txt -> /tmp/kdeverea/copilot-tool-output-1773794082493-cnhiz9.txt`
- `/tmp/copilot-tool-output-1773794792446-awdat9.txt -> /tmp/kdeverea/copilot-tool-output-1773794792446-awdat9.txt`
- `/tmp/copilot-tool-output-1773794975491-k2ih3m.txt -> /tmp/kdeverea/copilot-tool-output-1773794975491-k2ih3m.txt`
- `/tmp/copilot-tool-output-1773800945505-nkort0.txt -> /tmp/kdeverea/copilot-tool-output-1773800945505-nkort0.txt`
- `/tmp/copilot-tool-output-1773801017506-hwexd3.txt -> /tmp/kdeverea/copilot-tool-output-1773801017506-hwexd3.txt`
- `/tmp/copilot-tool-output-1773803446805-ug9eg6.txt -> /tmp/kdeverea/copilot-tool-output-1773803446805-ug9eg6.txt`
- `/tmp/copilot-tool-output-1773803514579-2a4i57.txt -> /tmp/kdeverea/copilot-tool-output-1773803514579-2a4i57.txt`
- `/tmp/copilot-tool-output-1773803624061-duqxuk.txt -> /tmp/kdeverea/copilot-tool-output-1773803624061-duqxuk.txt`
- `/tmp/copilot-tool-output-1773803662682-rvc2bk.txt -> /tmp/kdeverea/copilot-tool-output-1773803662682-rvc2bk.txt`
- `/tmp/copilot-tool-output-1773803791708-9e7gke.txt -> /tmp/kdeverea/copilot-tool-output-1773803791708-9e7gke.txt`
- `/tmp/copilot-tool-output-1773804362908-svmyn0.txt -> /tmp/kdeverea/copilot-tool-output-1773804362908-svmyn0.txt`
- `/tmp/copilot-tool-output-1773811043223-apjeat.txt -> /tmp/kdeverea/copilot-tool-output-1773811043223-apjeat.txt`
- `/tmp/copilot-tool-output-1773811110239-uaadbu.txt -> /tmp/kdeverea/copilot-tool-output-1773811110239-uaadbu.txt`

Files removed: **0**
- none

## Failures / retries and resolutions

- Initial `/tmp` hygiene attempt hit a permission error while scanning unrelated `/tmp` entries. Resolved by restricting discovery to explicit glob patterns (`/tmp/copilot-tool-output-*`, `/tmp/config_*`).
