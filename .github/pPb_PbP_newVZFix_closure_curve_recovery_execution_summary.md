# Analyzer Execution Summary: newVZFix closure curve recovery

## 1) Root-cause diagnosis
Missing central-closure curves were caused by using `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix-mc.sh`, which originally only produced `*_Gen_nominal_*` and `*_trkResidual_*` outputs and skipped `*_nominal_*` and `*_ZResidual_*` outputs required by `Plots/20260120_CentralClosure/plot_closure.cpp`.

File-level diagnosis for required 64 central inputs (`8 prefixes x 4 ZPT bins x {nosub,result}`):
- Before recovery: 32 existing, 32 missing (all `*_nominal_*` and `*_ZResidual_*` for pPb/PbP).
- After recovery: 64 existing, 0 missing.

## 2) Scripts changed/added
- Modified: `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix-mc.sh`
  - Now delegates to full-chain `closure-trk-newVZFix.sh` to prevent partial-production misuse.
- Added: `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh`
  - New newVZFix-aware Z-closure production script.
- Replaced/updated: `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`
  - Added stage toggles: `RUN_GEN`, `RUN_NOMINAL`, `RUN_ZRESIDUAL`, `RUN_TRKRESIDUAL` to support targeted reruns.
- Added: `Plots/20260115_ZResidualClosure/plot-Z-newVZFix.sh`
  - New newVZFix plotting wrapper (pPb/PbP, ZPT bins `0_10,10_20,20_40,40_500`).
- Added: `Plots/20251202_trackResidualClosure/plot-track-newVZFix.sh`
  - New newVZFix track-closure plotting wrapper (pPb/PbP, 4 ZPT bins).

## 3) Ordered command log (key steps)
- `source SetupAnalysis.sh && make -s in MainAnalysis/20241102_ZhadronVsZPt, Plots/20260120_CentralClosure, Plots/20260115_ZResidualClosure, Plots/20251202_trackResidualClosure (baseline)`
- `python3 manifest check for required central ROOT files (identified missing nominal/ZResidual matrix)`
- `apply script patches/additions listed above`
- `source SetupAnalysis.sh && RUN_GEN=0 RUN_NOMINAL=1 RUN_ZRESIDUAL=1 RUN_TRKRESIDUAL=0 ./closure-trk-newVZFix.sh 0 1 1 (targeted regeneration)`
- `source SetupAnalysis.sh && ./plot-central-newVZFix.sh (regenerate central closure PDFs)`
- `source SetupAnalysis.sh && ./plot-track-newVZFix.sh (regenerate track closure PDFs)`
- `source SetupAnalysis.sh && ./plot-Z-newVZFix.sh (generate newVZFix Z-closure PDFs)`
- `ROOT macro deterministic check: /tmp/kdeverea/check_central_curves.C`
- `pdftotext legend-presence assertion on representative central PDFs`
- `post-change make -s in same modules (validation)`

## 4) File manifests
- Central closure PDFs (newVZFix): **48 files**, all non-empty
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
- Z closure PDFs (newVZFix): **24 files**, all non-empty
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/PbP/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT0_10_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT10_20_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT20_40_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260115_ZResidualClosure/plots/pPb/ZPT40_500_newVZFix_ZV6_trkV24_nmix10-closure-pt.pdf`
- Track closure PDFs (newVZFix): **24 files**, all non-empty
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT0_10_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT10_20_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT20_40_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-eta.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-phi.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_newVZFix_ZV6_trkV24_nmix10-nosub-closure-pt.pdf`

## 5) Curve-visibility validation evidence
- Deterministic source-object check (ROOT): representative `ZPT20_40` for both pPb and PbP showed `SERIES_WITH_HIST 4` with non-zero integrals for: GEN, nominal, ZResidual, trkResidual.
```text
COLLISION pPb
SERIES pPbMC_Gen_nominal INTEGRAL 461.919
SERIES pPbMC_nominal INTEGRAL 463.876
SERIES pPbMC_ZResidual INTEGRAL 463.896
SERIES pPbMC_trkResidual INTEGRAL 462.56
SERIES_WITH_HIST 4
COLLISION PbP
SERIES PbPMC_Gen_nominal INTEGRAL 460.202
SERIES PbPMC_nominal INTEGRAL 464.971
SERIES PbPMC_ZResidual INTEGRAL 464.998
SERIES PbPMC_trkResidual INTEGRAL 460.731
SERIES_WITH_HIST 4
```
- Legend/series label assertion (representative central `-closure-DeltaPhi-all.pdf` for pPb and PbP):
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> {'MC DY-GEN': True, 'MC DY-RECO': True, '& Z correction': True, '& Z + track correction': True}
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT20_40_trkPT0.5_500_newVZFix_ZV6_trkV24_nmix10-closure-DeltaPhi-all.pdf` -> {'MC DY-GEN': True, 'MC DY-RECO': True, '& Z correction': True, '& Z + track correction': True}
- Visual spot-check notes: representative generated central PDFs include all expected legend labels (`MC DY-GEN`, `MC DY-RECO`, `& Z correction`, `& Z + track correction`) and were regenerated without file-open errors in plotting logs.

## 6) Remaining issues / recommended next fix
- `closure-Z-newVZFix.sh` full `ZPT0_500` production was started but is computationally expensive; this run was not carried to completion in this execution window.
- Current newVZFix Z-closure outputs are produced for `ZPT={0_10,10_20,20_40,40_500}` and are non-empty.
- Recommended next fix (if integrated `ZPT0_500` products are mandatory): run `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh 0 1 1` to completion (optionally with tuned `NTHREAD/CUT_PARALLELISM`) and then rerun `Plots/20260115_ZResidualClosure/plot-Z-newVZFix.sh` in `ZPT0_500` mode.
