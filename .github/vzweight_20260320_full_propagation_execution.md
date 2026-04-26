# Analyzer execution: `vzweight_20260320_full_propagation_plan`

## Outcome

Completed the requested propagation of the fresh `20260320` skim-VZ-off VZ weights through the downstream pp / pPb / PbP chain, hardened the touched analyzers so skim-embedded `VZWeight` can no longer act as a silent fallback, rebuilt the affected modules, reran the correction / closure / support / central / energy / plotting stack, and validated the new products in-repo. Overleaf was not touched.

## 1. Old vs new VZ-root table

| System | Previously used root | Refreshed root used in this task |
| --- | --- | --- |
| `pp` | `Plots/20251001_pPbVZReweighting/summary/20260317_ZV6_ZPT0_350_VzReweightFits_pp.root` | `Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root` |
| `pPb` | `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_pPb.root` | `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root` |
| `PbP` | `Plots/20251001_pPbVZReweighting/summary/20260319_pPbPbpRelabelFix_ZPT0_500_VzReweightFits_PbP.root` | `Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root` |

Source: `.github/vzweight_20260320_full_propagation_plan.md:20-22,34-36,48-50`.

## 2. Files modified

This list covers the source and script files intentionally changed for this propagation task. It excludes generated artifacts such as rebuilt executables and transient `config.sh` output, and it also excludes unrelated pre-existing worktree changes.

### Analyzer hardening

- `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`
- `MainAnalysis/20260115_ZCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20260115_ZCorrection/include/parameter.h`
- `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp`
- `MainAnalysis/20251211_ResidualCorrection/include/parameter.h`
- `MainAnalysis/20260222_EnergyExtrapolation/CorrelationAnalysis.cpp`
- `MainAnalysis/20260222_EnergyExtrapolation/include/parameter.h`

### Workflow / production scripts

- `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh`
- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pythia-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/run-pp.sh`
- `MainAnalysis/20251211_ResidualCorrection/workflow/run-pPb.sh`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-newVZFix.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/check-dataMCComparison.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-zstep-audit.sh`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ-20260320.sh`

### Plot / summary wrappers and plotter fixes

- `Plots/20260115_ZResidualClosure/plot-Z.sh`
- `Plots/20260115_ZResidualClosure/plot-Z-newVZFix.sh`
- `Plots/20251202_trackResidualClosure/plot-track.sh`
- `Plots/20251202_trackResidualClosure/plot-track-newVZFix.sh`
- `Plots/20260120_CentralClosure/plot-central.sh`
- `Plots/20260120_CentralClosure/plot-central-newVZFix.sh`
- `Plots/20260213_Central/plot-pp.sh`
- `Plots/20260213_Central/plot-dataMCComparison.sh`
- `Plots/20260213_Central/plot-central.sh`
- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
- `Plots/20260213_Central/plot-central-combined.sh`
- `Plots/20260213_Central/plot-zmass.sh`
- `Plots/20260213_Central/plot-energyExtrapolation.sh`
- `Plots/20260213_Central/plot-VZ.sh`
- `Plots/20260213_Central/plot-zspectrum.sh`
- `Plots/20260213_Central/plot_zmass.cpp`
- `Plots/20260213_Central/plot_zspectrum.cpp`

## 3. Exact commands run

### Rebuild

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd CommonCode && make
cd ../MainAnalysis/20241102_ZhadronVsZPt && make
cd ../20260115_ZCorrection && make
cd ../20251211_ResidualCorrection && make
cd ../20260222_EnergyExtrapolation && make
cd ../../Plots/20251001_pPbVZReweighting && make
cd ../20260213_Central && make
```

### Stage 3

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20260115_ZCorrection/workflow
./pythia-analysis.sh
./pPb-DY-analysis.sh
```

### Stage 4

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20251211_ResidualCorrection/workflow
./run-pp.sh
./run-pPb.sh
```

### Stage 5

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20241102_ZhadronVsZPt
./closure-Z.sh D 1 1
./closure-trk.sh 1 1 1
./closure-Z-newVZFix.sh 1 1 1
./closure-trk-newVZFix.sh 1 1 1
```

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd Plots/20260115_ZResidualClosure
./plot-Z.sh
./plot-Z-newVZFix.sh
cd ../20251202_trackResidualClosure
./plot-track.sh
./plot-track-newVZFix.sh
cd ../20260120_CentralClosure
./plot-central.sh
./plot-central-newVZFix.sh
```

### Stage 6

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20241102_ZhadronVsZPt
./pp-plotpp.sh
cd ../../Plots/20260213_Central
./plot-pp.sh
cd ../../MainAnalysis/20241102_ZhadronVsZPt
./check-dataMCComparison.sh 0 1 1
cd ../../Plots/20260213_Central
./plot-dataMCComparison.sh
```

After fixing the `nmix0` / tag mismatch and retargeting the heavy-ion support comparison to the refreshed corrections:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20241102_ZhadronVsZPt
./check-dataMCComparison.sh 0 1 1
cd ../../Plots/20260213_Central
./plot-dataMCComparison.sh
```

### Stage 7

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20241102_ZhadronVsZPt
./central.sh 0 1 1
```

### Stage 8

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd MainAnalysis/20260222_EnergyExtrapolation/workflow
./pythia-analysis.sh
cd ../../../Plots/20260213_Central
./plot-energyExtrapolation.sh
```

### Stage 9

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
cd Plots/20260213_Central
./plot-central.sh
./plot-central-overlay-PPbPbP.sh
./plot-central-combined.sh
./plot-zmass.sh
./plot-VZ.sh
./plot-zspectrum.sh
```

The final stage needed targeted reruns after fixing:

- `plot-central.sh` executable bit
- `check-dataMCComparison.sh` heavy-ion support weights + `plot-dataMCComparison.sh` `nmix0` tag
- `plot-energyExtrapolation.sh` / central wrappers / `plot-zmass.sh` / `plot-zspectrum.sh` pp EEV3 tag alignment
- `plot_zmass.cpp` pp-MC fallback resolution
- `plot_zspectrum.cpp` `pp_EE_` -> `pp_EExtrapolation_` input prefix

## 4. Script-audit results

### Explicit `20260320` VZ-root usage

Representative updated scripts:

- `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh:11,14-31`
- `MainAnalysis/20260115_ZCorrection/workflow/pPb-DY-analysis.sh:6-7,19-59`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pythia-analysis.sh:5-27`
- `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh:5-58`
- `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh:10,47-92`
- `MainAnalysis/20241102_ZhadronVsZPt/central.sh:6-11,34-40,61-81`
- `MainAnalysis/20241102_ZhadronVsZPt/check-dataMCComparison.sh:19-25,72-134,145-207`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ.sh:8-9,26-28,44-46,54-72`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ-20260320.sh:12-14,101-140`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-Z.sh:7-8`
- `MainAnalysis/20241102_ZhadronVsZPt/closure-trk.sh:22-23,33`

### Explicit `UseVZWeight` policy in scripts

- MC stages now pass explicit `--UseVZWeight true --VZWeightFile ...` in the refreshed chain, e.g.
  - `MainAnalysis/20260115_ZCorrection/workflow/pythia-analysis.sh:14,16,19,23,31`
  - `MainAnalysis/20251211_ResidualCorrection/workflow/pPb-DY-analysis.sh:18,20,23,27,36,40,42,45,49,58`
  - `MainAnalysis/20241102_ZhadronVsZPt/closure-VZ-20260320.sh:38-48,77-87`
- Data stages now pass `--UseVZWeight false`, e.g.
  - `MainAnalysis/20241102_ZhadronVsZPt/pp-plotpp.sh:76-92`
  - `MainAnalysis/20241102_ZhadronVsZPt/central.sh:36-40`
  - `MainAnalysis/20241102_ZhadronVsZPt/check-dataMCComparison.sh:118-134,191-207`

### No stale VZ-root usage in the active rerun chain

- `rg '20260311|20260317|20260318|20260319' MainAnalysis/**/*.sh` returned no matches in the active rerun chain.
- The only remaining hits were historical VZ-derivation wrappers under `Plots/20251001_pPbVZReweighting/` (`run-reweight.sh`, `run-reweight-newVZFix.sh`), which are not part of the rerun chain executed here.

## 5. Produced output manifest

The directories below can contain older products from previous runs. The paths listed here are the refreshed `20260320`-propagation outputs or the naming families used to locate them.

### Z-correction roots

- `MainAnalysis/20260115_ZCorrection/workflow/output/20260320_ZCorrection_V6_skimVZOff_pp_zPt0-500.root`
- `MainAnalysis/20260115_ZCorrection/workflow/output/20260320_ZCorrection_V6_skimVZOff_PPb_zPt0-500.root`
- `MainAnalysis/20260115_ZCorrection/workflow/output/20260320_ZCorrection_V6_skimVZOff_PbP_zPt0-500.root`

### Track-residual roots

- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt0-10.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt10-20.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt20-40.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt40-500.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt0-10.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt10-20.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt20-40.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt40-500.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt0-10.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt10-20.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt20-40.root`
- `MainAnalysis/20251211_ResidualCorrection/workflow/output/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt40-500.root`

### Closure ROOT families

Representative refreshed roots:

- `MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_nominal_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_ZResidual_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_trkResidual_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_nominal_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_ZResidual_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPbMC_trkResidual_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_nominal_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_ZResidual_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbPMC_trkResidual_ZV6_trkV24_vz20260320_nmix10_ZPT0_500-{nosub,result}.root`

### Closure PDFs

Representative refreshed PDFs:

- `Plots/20260115_ZResidualClosure/plots/pp/ZPT0_500_ZV6_trkV24_vz20260320_nmix10-closure-{eta,phi,pt}.pdf`
- `Plots/20260115_ZResidualClosure/plots/pPb/ZPT40_500_ZV6_trkV24_vz20260320_nmix10-closure-{eta,phi,pt}.pdf`
- `Plots/20260115_ZResidualClosure/plots/PbP/ZPT40_500_ZV6_trkV24_vz20260320_nmix10-closure-{eta,phi,pt}.pdf`
- `Plots/20251202_trackResidualClosure/plots/pp/pp_ZPT40_500_ZV6_trkV24_vz20260320_nmix10-nosub-closure-{eta,phi,pt}.pdf`
- `Plots/20251202_trackResidualClosure/plots/pPb/pPb_ZPT40_500_ZV6_trkV24_vz20260320_nmix10-nosub-closure-{eta,phi,pt}.pdf`
- `Plots/20251202_trackResidualClosure/plots/PbP/PbP_ZPT40_500_ZV6_trkV24_vz20260320_nmix10-nosub-closure-{eta,phi,pt}.pdf`
- `Plots/20260120_CentralClosure/plots/pp/pp_ZPT40_500_trkPT0.5_500_ZV6_trkV24_vz20260320_nmix10-closure-Delta{Eta,Phi}-{all,bkg,result}.pdf`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT40_500_trkPT0.5_500_ZV6_trkV24_vz20260320_nmix10-closure-Delta{Eta,Phi}-{all,bkg,result}.pdf`
- `Plots/20260120_CentralClosure/plots/PbP/PbP_ZPT40_500_trkPT0.5_500_ZV6_trkV24_vz20260320_nmix10-closure-Delta{Eta,Phi}-{all,bkg,result}.pdf`

### Central-value / support ROOT families

Representative refreshed roots:

- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_nominal_ZV6_trkV24_vz20260320_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_ZResidual_ZV6_trkV24_vz20260320_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_trkResidual_ZV6_trkV24_vz20260320_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_nominal_ZV6_trkV24_vz20260320_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_ZResidual_ZV6_trkV24_vz20260320_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_trkResidual_ZV6_trkV24_vz20260320_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_ZV6_trkV24_EEV3_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_ZV6_trkV24_EEV3_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_ZV6_trkV24_EEV3_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10_ZPT{5_30,30_500,5_500}-{nosub,result}.root`

### Energy-extrapolation workflow outputs

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260320_EnergyExtrapolation_EEV3_skimVZOff.root`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260320_EnergyExtrapolation_EEV3_skimVZOff.pdf`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260320_EnergyExtrapolation_EEV3_skimVZOff-closure.pdf`

### Central / combination / final-result PDFs

Representative refreshed products:

- `Plots/20260213_Central/plots/pp/ZV6_trkV24_vz20260320_nmix10_ZPT40_350_trkPT{1_2,2_4,4_10}-Delta{Eta,Phi}-result.pdf`
- `Plots/20260213_Central/plots/pp/ZV6_trkV24_vz20260320_nmix10_ZPT{20_40,40_60,60_500}_trkPT2_500-Delta{Eta,Phi}-result.pdf`
- `Plots/20260213_Central/plots/dataMCComparison/pPb_ZV6_trkV24_vz20260320_nmix0_ZPT5_500-{Zmass,Zpt,Zeta,pt,eta}.pdf`
- `Plots/20260213_Central/plots/energyExtrapolation/ZV6_trkV24_EEV3_nmix10_ZPT{5_30,30_500,5_500}_trkPT{0.5_4,4_500,0.5_500}-Delta{Eta,Phi}-result.pdf`
- `Plots/20260213_Central/plots/central/ppPbP_ZPT{5_30,30_500,5_500}_trkPT{0.5_4,4_500,0.5_500}_ZV6_trkV24_vz20260320_nmix10-Delta{Eta,Phi}-result.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/{all,pPb,PbP}_ZPT{5_30,30_500,5_500}_trkPT{0.5_4,4_500,0.5_500}_ZV6_trkV24_vz20260320_nmix10-Delta{Eta,Phi}-result.pdf`
- `Plots/20260213_Central/plots/zmass/pPb_ZPT0_500-ZV6_trkV24_vz20260320_nmix10.pdf`
- `Plots/20260213_Central/plots/zmass/PbP_ZPT0_500-ZV6_trkV24_vz20260320_nmix10.pdf`
- `Plots/20260213_Central/plots/zmass/pp_ZPT5_500-ZV6_trkV24_EEV3_nmix10.pdf`
- `Plots/20260213_Central/plots/VZ/{pPb,PbP}_ZPT0_500-{noVZWeight_nmix0,VZWeight_nmix0}-Vz.pdf`
- `Plots/20260213_Central/plots/VZ/pp_ZPT0_350-{noEvtWeight_VZOnly_nmix1,myEvtWeight_VZOnly_nmix1}-Vz.pdf`
- `Plots/20260213_Central/plots/zspectrum/{pppPb,all}_ZPT5_500_trkPT0.5_500_ZV6_trkV24_EEV3_nmix10.pdf`

## 6. Validation notes on the new default VZ behavior

### No skim fallback remains in the touched binaries

- Main analyzer:
  - `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp:33-48` rejects data + VZ weight, data + `VZWeightFile`, and MC `UseVZWeight=true` without an explicit external file.
  - `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp:709` sets `UseVZWeight` default to `false`.
  - `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp:233-237,315-319,423-427` shows the only remaining VZ path is `VZCorrector(par.VZWeightFile)` with explicit `vzCorrector->GetCorrectionFactor(...)`.
- Downstream analyzers:
  - `MainAnalysis/20260115_ZCorrection/CorrelationAnalysis.cpp:24-40,272`
  - `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp:24-40,303`
  - `MainAnalysis/20260222_EnergyExtrapolation/CorrelationAnalysis.cpp:24-40,277`
  All three now validate the same explicit policy and default `UseVZWeight` to `false`.

### Repo search for skim-VZ fallback

- A repo-wide search for `MZSignal->VZWeight` / `MMix->VZWeight` no longer finds live fallback code in the touched analyzers.
- Remaining matches were outside the touched binaries:
  - `MainAnalysis/20260216_temp/CorrelationAnalysis.cpp` (temporary snapshot)
  - one explanatory comment in `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp`

### Collision-system impact confirmation

- `pp`: propagation was required because the new `20260320_skimVZOff_ZPT0_350` fit moved relative to the previously used `20260317` pp root; this changed the pp Z-correction, residual, support, and energy-extrapolation inputs.
- `pPb`: propagation was required because the relabel-fixed `20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500` fit replaced the previously used `20260319` pPb root; this changed heavy-ion Z corrections, residuals, closure, central, VZ, and support comparisons.
- `PbP`: same as `pPb`; the fresh `20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500` PbP fit replaced the previous `20260319` root and had to be propagated through the same downstream stack.

### Additional execution notes

- `my_ZWeights/` and `my_residualWeights/` handoffs were already symlinked to the new workflow outputs, so no manual copy step was required.
- `check-dataMCComparison.sh` required a post-run fix:
  - retarget heavy-ion Z / residual weights to the new `20260320` family
  - align `plot-dataMCComparison.sh` to the produced `nmix0` support tag
- `Plots/20260213_Central/plot-central.sh` and `Plots/20260120_CentralClosure/plot-central-newVZFix.sh` both needed executable-bit restoration after edits.
- The pp EEV3 plot side still uses the established pp tag family `ZV6_trkV24_EEV3_nmix10` because that is the actual refreshed pp extrapolation family on disk; the workflow-level correction artifact is separately date-tagged as `20260320_EnergyExtrapolation_EEV3_skimVZOff.*`.

## 7. Overleaf

Not touched.
