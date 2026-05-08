# Analyzer summary: pPb/PbP VZ-weight fix plan execution

## 1) Command / script log (ordered)

- `cd MainAnalysis/20241102_ZhadronVsZPt && bash ./central.sh 0 1 1` (first attempt used `./central.sh` and failed with permission denied; rerun via `bash`)
- `cd Plots/20260213_Central && bash ./plot-central.sh && bash ./plot-central-combined.sh && bash ./plot-central-overlay-PPbPbP.sh`
- `python3` checks for Phase A central/mc integrals and Overleaf figure references from `~/OverleafZHadronInPPb/src/results.tex`
- `python3` copy of 10 current-weight result PDFs to `~/OverleafZHadronInPPb/figures/result/`
- `cd MainAnalysis/20241102_ZhadronVsZPt && bash ./closure-VZ.sh 0 1 1` (regenerate noVZWeight inputs)
- `cd Plots/20251001_pPbVZReweighting && bash ./run-reweight-newVZFix.sh` (new corrected mapping, with runtime source logging)
- `cd MainAnalysis/20260115_ZCorrection/workflow && NAME_TAG=20260318_ZCorrection_V6_newVZFix ... bash ./pPb-DY-analysis.sh`
- `cd MainAnalysis/20251211_ResidualCorrection/workflow && NAME_TAG=20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix ... bash ./run-pPb.sh`
- `cd MainAnalysis/20241102_ZhadronVsZPt && bash ./closure-trk-newVZFix.sh 0 1 1` (stopped after runtime concerns, replaced with MC-only closure script)
- `cd MainAnalysis/20241102_ZhadronVsZPt && bash ./closure-trk-newVZFix-mc.sh 1 1` (completed exploratory MC closure endpoint)
- `python3` comparison table generation: current (`ZV5_trkV23_nmix10`) vs new (`newVZFix_ZV6_trkV24_nmix10`).

Failed attempts and resolution:

- `./central.sh 0 1 1` failed (permission denied); resolved by running `bash ./central.sh 0 1 1`.
- Copy command failed when source and destination files were identical for some correction outputs; resolved by skipping redundant copy and running closure directly against existing paths.
- Full exploratory closure script was runtime-heavy; resolved by adding/using MC-focused script `closure-trk-newVZFix-mc.sh` that executes only GEN and trkResidual chains (the required Phase B endpoint).

## 2) Phase A summary (current weights, note-facing)

- Current-weight note-facing result plots were regenerated with maintained plotting scripts in `Plots/20260213_Central/`.
- Copied 10 referenced result PDFs to Overleaf `figures/result/` (filenames unchanged).
- MC subtraction sanity cross-check (`DeltaPhi_Result0.5_500`, threshold `|I| < 0.15`) for current tag `ZV5_trkV23_nmix10`:

| collision | ZPT | DeltaPhi integral | pass |
|---|---|---:|---:|
| pPbMC | 0_10 | 0.0348601 | 1 |
| pPbMC | 10_20 | -0.0532774 | 1 |
| pPbMC | 20_40 | 0.00622237 | 1 |
| pPbMC | 40_500 | 0.00437033 | 1 |
| PbPMC | 0_10 | -0.0549123 | 1 |
| PbPMC | 10_20 | 0.113665 | 1 |
| PbPMC | 20_40 | -0.0153395 | 1 |
| PbPMC | 40_500 | -0.0624052 | 1 |

Overleaf copies (Phase A):

- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT5_500_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT5_30_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT0.5_4_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/result/all_ZPT30_500_trkPT4_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`

## 3) Phase B summary (exploratory new VZ mapping, no Overleaf push)

New VZ derivation mapping applied and logged explicitly:

- pPb correction: MC=`pPbMC`, Data=`PbP`
- PbP correction: MC=`PbPMC`, Data=`pPb`

New exploratory VZ files:

- `/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_pPb.root`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_PbP.root`

Propagated correction products:

- Z corrections:
  - `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow/output/20260318_ZCorrection_V6_newVZFix_PPb_zPt0-500.root`
  - `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow/output/20260318_ZCorrection_V6_newVZFix_PbP_zPt0-500.root`
- Track residuals:
  - `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20251211_ResidualCorrection/workflow/output/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PPb_zPt{0-10,10-20,20-40,40-500}.root`
  - `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20251211_ResidualCorrection/workflow/output/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PbP_zPt{0-10,10-20,20-40,40-500}.root`

MC central-closure comparison table (current vs new):

| collision | ZPT | observable | current Δ(trk-Gen) | new Δ(trk-Gen) | |current|-|new| |
|---|---|---|---:|---:|---:|
| pPbMC | 0_10 | DeltaPhi | -0.0618845 | 0.0244158 | 0.0374687 |
| pPbMC | 0_10 | DeltaEta | 0.191548 | 0.248517 | -0.0569691 |
| pPbMC | 10_20 | DeltaPhi | 0.0567196 | 0.0604084 | -0.00368881 |
| pPbMC | 10_20 | DeltaEta | 0.14715 | 0.0468913 | 0.100258 |
| pPbMC | 20_40 | DeltaPhi | -0.0591647 | 0.098061 | -0.0388963 |
| pPbMC | 20_40 | DeltaEta | -0.366038 | -0.325744 | 0.040294 |
| pPbMC | 40_500 | DeltaPhi | -0.0614955 | -0.0478625 | 0.013633 |
| pPbMC | 40_500 | DeltaEta | -0.35435 | -0.40903 | -0.0546799 |
| PbPMC | 0_10 | DeltaPhi | 0.092922 | -0.00132458 | 0.0915974 |
| PbPMC | 0_10 | DeltaEta | 0.0474137 | 0.0422593 | 0.00515448 |
| PbPMC | 10_20 | DeltaPhi | -0.114063 | 0.00983174 | 0.104232 |
| PbPMC | 10_20 | DeltaEta | 0.0258262 | 0.0658853 | -0.0400592 |
| PbPMC | 20_40 | DeltaPhi | 0.155065 | 0.0145894 | 0.140476 |
| PbPMC | 20_40 | DeltaEta | -0.165045 | -0.242387 | -0.0773422 |
| PbPMC | 40_500 | DeltaPhi | 0.102367 | -0.00986583 | 0.0925013 |
| PbPMC | 40_500 | DeltaEta | -0.687241 | -0.511351 | 0.17589 |

Interpretation:

- `DeltaPhi` closure residuals improve in most PbPMC bins and mixed behavior in pPbMC bins.
- `DeltaEta` improvements are mixed; some bins improve while others degrade modestly.
- Overall this is a valid exploratory branch, but not uniformly better in all metrics yet.

Recommendation:

- Keep this as exploratory (`newVZFix`) and perform one focused follow-up on `DeltaEta` behavior (especially PbPMC `20_40`) before promoting to note-level central plotting.
- Per plan, no exploratory outputs were copied to Overleaf.
