# 20260321 working-point paper propagation execution

## Final status

Completed. The promoted `20260321` working point has been propagated through the requested upstream note figures, downstream rerun chain, and Overleaf note references/files.

## Commands run

### Bootstrap / inspection

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh
```

Read and audited:

- `.github/AGENT_REQUIREMENTS.md`
- `.github/working_point_20260321_paper_propagation_instructions.md`
- `.github/for-reviewer.md`
- live Overleaf sources under `/home/kdeverea/OverleafZHadronInPPb/src`

### Upstream artifact audit

Verified the hand-made upstream families exist:

```bash
glob Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500-*.pdf
glob MainAnalysis/20260115_ZCorrection/workflow/plots/corrections_*20260321_ZCorrection_V6*.pdf
glob Plots/20260115_ZResidualClosure/plots/{pp,pPb,PbP}/ZPT0_500_ZV6_trkV24_nmix0-closure-*.pdf
glob MainAnalysis/20251211_ResidualCorrection/workflow/plots/corrections_*20260321_ZV6_trkV24_TrackResidualCorrection_*.pdf
```

Built the upstream Overleaf manifest from live refs:

```bash
python3 /tmp/...  # session-local manifest generator
```

Manifest result:

- `rows=66`
- `source_missing=0`
- `dest_missing_before=30`

### Upstream Overleaf propagation

Patched and copied using the live-ref manifest:

- `src/software_mc_simulation.tex`
- `src/z_reco.tex`
- `src/track_reco.tex`
- `src/appendix.tex`

Copied all `66` referenced upstream figures into Overleaf with exact source basenames.

### Downstream rerun chain

Patched the maintained runners first so they point at the promoted working point and official output naming:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`
- `Plots/20260120_CentralClosure/plot-central.sh`
- `Plots/20260213_Central/plot-central.sh`
- `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
- `Plots/20260213_Central/plot-central-combined.sh`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`

Then ran:

```bash
cd /home/kdeverea/PhysicsZHadronEEC
source SetupAnalysis.sh

cd MainAnalysis/20260222_EnergyExtrapolation/workflow
./pythia-analysis.sh
```

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
export SKIP_CLEAN=1
export CUT_PARALLELISM=1
export NTHREAD=8
export NSLICE_FACTOR=1
./central.sh 0 1 1
```

Explicit pp EEV3 rerun (to refresh the `pp_{nominal,ZResidual,trkResidual,EExtrapolation}_ZV6_trkV24_EEV3_nmix10` families used by the note-facing plotters):

```bash
cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
ENERGY_ROOT=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260321_EnergyExtrapolation_EEV3.root

# differential bins
cat > config.sh <<'EOF'
ZPT_RANGES=("5_30" "30_500")
PT_RANGES=("0.5_4" "4_500")
EOF

./system-analysis.sh "pp_nominal_ZV6_trkV24_EEV3_nmix10" ...
./system-analysis.sh "pp_ZResidual_ZV6_trkV24_EEV3_nmix10" --ZWeightFile my_ZWeights/20260321_ZCorrection_V6_pp_zPt0-500.root ...
./system-analysis.sh "pp_trkResidual_ZV6_trkV24_EEV3_nmix10" --ZWeightFile my_ZWeights/20260321_ZCorrection_V6_pp_zPt0-500.root --ResidualWeightFile my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_pp_zPt ...
./system-analysis.sh "pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10" --ZWeightFile my_ZWeights/20260321_ZCorrection_V6_pp_zPt0-500.root --ResidualWeightFile my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_pp_zPt --EnergyExtraFile "$ENERGY_ROOT" ...

# inclusive bin
cat > config.sh <<'EOF'
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500")
EOF

./system-analysis.sh "pp_nominal_ZV6_trkV24_EEV3_nmix10" ...
./system-analysis.sh "pp_ZResidual_ZV6_trkV24_EEV3_nmix10" ...
./system-analysis.sh "pp_trkResidual_ZV6_trkV24_EEV3_nmix10" ...
./system-analysis.sh "pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10" ...
```

Downstream plot wrappers:

```bash
cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure
./plot-central.sh

cd /home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central
./plot-central.sh
./plot-energyExtrapolation.sh
./plot-central-overlay-PPbPbP.sh
./plot-central-combined.sh
```

### Downstream Overleaf propagation

Built the downstream manifest from live refs in:

- `src/analysis.tex`
- `src/results.tex`

Manifest result:

- `rows=68`
- `source_missing=0`
- `dest_missing_before=2`

Patched those TeX refs, copied all `68` referenced downstream figures, then removed unreferenced stale superseded Overleaf files from the old downstream family.

## Output changes

### Upstream note refs/files

Updated note refs from stale basenames to exact source basenames for:

- VZ ratio / VZ distributions:
  - `20260320_*skimVZOff*` -> `20260321_ZPT0_500-*`
- Z correction:
  - `corrections_*_20260320_ZCorrection_V6_skimVZOff_0_500.pdf`
  - -> `corrections_*_20260321_ZCorrection_V6_0_500.pdf`
- Z closure:
  - `ZPT0_500_ZV6_trkV24_nmix10-closure-*`
  - -> `ZPT0_500_ZV6_trkV24_nmix0-closure-*`
- Track correction:
  - `corrections_*_20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_*`
  - -> `corrections_*_20260321_ZV6_trkV24_TrackResidualCorrection_*`

### Downstream note refs/files

Updated note refs from stale basenames to official rerun basenames for:

- central closure:
  - `*_ZV6_trkV24_vz20260320_nmix10-closure-*`
  - -> `*_ZV6_trkV24_nmix10-closure-*`
- energy correction factor:
  - `20260320_EnergyExtrapolation_EEV3_skimVZOff{,-closure}.pdf`
  - -> `20260321_EnergyExtrapolation_EEV3{,-closure}.pdf`
- pp energy panels:
  - `ZV6_trkV24_EEV3_vz20260320_nmix10_*`
  - -> `ZV6_trkV24_EEV3_nmix10_*`
- pPb/PbP overlay:
  - `ZV6_trkV24_vz20260320_nmix10_*`
  - -> `ZV6_trkV24_nmix10_*`
- final combined results:
  - `all_*_ZV6_trkV24_vz20260320_nmix10-*`
  - -> `all_*_ZV6_trkV24_nmix10-*`

Removed `32` now-unreferenced stale downstream Overleaf files from the superseded `vz20260320` families after confirming they were no longer referenced anywhere in `src/*.tex`.

## Validation

### Upstream

- live-ref upstream manifest: `66` rows
- `source_missing=0`
- after copy: `targets_missing=0`
- no remaining stale upstream refs in:
  - `src/software_mc_simulation.tex`
  - `src/z_reco.tex`
  - `src/track_reco.tex`
  - `src/appendix.tex`

### Downstream

- live-ref downstream manifest: `68` rows
- `source_missing=0`
- after copy: `targets_missing=0`
- no remaining stale downstream refs matching `vz20260320` or `20260320_EnergyExtrapolation_EEV3_skimVZOff` in:
  - `src/analysis.tex`
  - `src/results.tex`

Representative refreshed timestamps:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260321_EnergyExtrapolation_EEV3.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pPb_trkResidual_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/PbP_trkResidual_ZV6_trkV24_nmix10_ZPT5_30-result.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_EExtrapolation_ZV6_trkV24_EEV3_nmix10_ZPT5_500-result.root`
- `Plots/20260120_CentralClosure/plots/pPb/pPb_ZPT0_10_trkPT0.5_500_ZV6_trkV24_nmix10-closure-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/energyExtrapolation/ZV6_trkV24_EEV3_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-result.pdf`
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/ZV6_trkV24_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf`
- `Plots/20260213_Central/plots/central_combined/ZV6_trkV24_EEV3_nmix10/all_ZPT5_500_trkPT0.5_500_ZV6_trkV24_nmix10-DeltaEta-result.pdf`

Long downstream shell log scan:

- `rg 'Error:|No such file|cannot open|missing-file warning|warning'` over the completed shell log returned no matches.

## Failures / retries / discrepancies

- The reviewer brief’s literal Z-correction-root location (`workflow/my_ZWeights/...`) did not match the first physical location found on disk. This was a location mismatch only; the promoted `20260321` basenames existed and were used successfully.
- `z_reco.tex` referenced nonexistent `nmix10` Z-closure PDFs; the actual hand-made note-facing family present for this task was `nmix0`. The note was updated to the existing `nmix0` family.
- The first downstream rerun attempt followed `central.sh` too broadly and spent time in the pp `noEvtWeight` branch, which was not needed for the note-facing propagation. I stopped that run and restarted with the focused sequence:
  - `central.sh 0 1 1` for heavy-ion central products
  - explicit pp EEV3 `system-analysis.sh` commands for the note-facing pp energy family
  - downstream plot wrappers afterward
- One intermediate timestamp check asked for a nonexistent differential central-closure filename (`pPb_ZPT5_30...`) in `Plots/20260120_CentralClosure`; that plotter’s note-facing closure family is the expected `0_10`, `10_20`, `20_40`, `40_500` set. Validation was rerun against the actual produced filenames.

## Git status snapshots

At the end of this task:

- `PhysicsZHadronEEC`: still a dirty tree with both this task’s script changes and unrelated pre-existing modifications in other files. I did not revert unrelated changes.
- `OverleafZHadronInPPb`: updated TeX refs plus copied/removed figure files matching the manifest-driven propagation above.

## Final result

The paper note now consistently references and contains the promoted `20260321` upstream and downstream figure families, with stale `vz20260320` downstream note refs removed and superseded unreferenced Overleaf files cleaned up.
