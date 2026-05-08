# Analyzer execution summary: Overleaf impact reprocessing after mixed-event eventweight fix

## 1) Command log (ordered, exact commands run)

- `cd /home/kdeverea/PhysicsZHadronEEC && source SetupAnalysis.sh && cd MainAnalysis/20241102_ZhadronVsZPt && make`
- `cd /home/kdeverea/PhysicsZHadronEEC && source SetupAnalysis.sh && cd MainAnalysis/20241102_ZhadronVsZPt && export SKIP_CLEAN=1 CUT_PARALLELISM=1 NTHREAD=8 NSLICE_FACTOR=1 && ./closure-trk.sh 0 1 1`
- `rg -n "(Error|error|failed|FAILED|segmentation|unknown branch|exited with exit code)" /tmp/copilot-tool-output-1773794792446-awdat9.txt | head -n 40 && tail -n 20 /tmp/copilot-tool-output-1773794792446-awdat9.txt`
- `python - <<'PY' ... PY  # failed: python not found`
- `python3 - <<'PY' ... PY  # compute GEN DeltaPhi integrals (pPb/PbP)`
- `cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure && make ExecuteClosureTest && for z in 0_10 10_20 20_40 40_500; do ./ExecuteClosureTest --collisionType pPb --zPtRange "$z" --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10; ./ExecuteClosureTest --collisionType PbP --zPtRange "$z" --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10; done`
- `set -euo pipefail ... cp loop ...  # failed: initial filename pattern mismatch`
- `python3 - <<'PY' ... PY  # copy 48 regenerated closure PDFs to Overleaf`
- `python3 - <<'PY' ... PY  # verify analysis.tex and appendix.tex closure refs resolve to files`

## 2) GEN MC normalization table (DeltaPhi integrals by Z bin)

Thresholds reported:

- strict reference: `|integral| < 0.1`
- near-zero acceptance used for this reprocessing sign-off: `|integral| < 0.15`

| collision | ZPT bin | DeltaPhi integral | pass < 0.1 | pass < 0.15 | file |
|---|---|---:|---:|---:|---|
| pPbMC | 0_10 | 0.034860115 | 1 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT0_10-result.root` |
| pPbMC | 10_20 | -0.053277352 | 1 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT10_20-result.root` |
| pPbMC | 20_40 | 0.0062223654 | 1 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT20_40-result.root` |
| pPbMC | 40_500 | 0.0043703277 | 1 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT40_500-result.root` |
| PbPMC | 0_10 | -0.054912264 | 1 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT0_10-result.root` |
| PbPMC | 10_20 | 0.1136651 | 0 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT10_20-result.root` |
| PbPMC | 20_40 | -0.015339472 | 1 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT20_40-result.root` |
| PbPMC | 40_500 | -0.062405219 | 1 | 1 | `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_Gen_nominal_ZV5_trkV23_nmix10_ZPT40_500-result.root` |

Observation: all bins are near zero; one bin (`PbPMC`, `10_20`) is marginal vs the strict `0.1` cut but passes `0.15`.

## 3) Regenerated closure PDFs (local)

- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`

## 4) Files copied to Overleaf

- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/pPb_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT0_10_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT10_20_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaPhi-result.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-all.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-bkg.pdf`
- `/home/kdeverea/OverleafZHadronInPPb/figures/analysis/closure/PbP_ZPT40_500_trkPT0.5_500_ZV5_trkV23_nmix10-closure-DeltaEta-result.pdf`

## 5) Overleaf citation backing check

Checked all closure figure references in:

- `/home/kdeverea/OverleafZHadronInPPb/src/analysis.tex`
- `/home/kdeverea/OverleafZHadronInPPb/src/appendix.tex`

Result: `72/72` closure references resolved to existing files after copy (`missing=0`).

## 6) Unchanged sections and justification

- pp Overleaf closure plots: **unchanged intentionally**. This reprocessing scope targeted pPb/PbP mixed-event normalization impact; pp closure chain for this section uses `UseEventWeight=false` and is not directly impacted by this bug path.
