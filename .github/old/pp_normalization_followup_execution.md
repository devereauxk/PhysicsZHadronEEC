# pp normalization follow-up execution (fixEWBug tag)

## Scope executed

Executed all instructions from `.github/pp_normalization_followup_directions.md` using the required non-overwriting tag:

- `evtWeightOn_fixEWBug_ZV6_trkV24_nmix10`

Also kept the optional `evtWeightOff_fixEWBug_ZV6_trkV24_nmix10` control production in place from the follow-up run context.

## Step 1 baseline fixed set (gate input): completed

Generated required `ZPT40_350` ROOT products:

- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_Gen_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_350-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_350-result.root`

## Step 2 normalization gate: passed

GEN MC gate check from `pythiaMC_Gen_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_350-result.root`:

| trkPT | DeltaPhi integral | pass (`|I| < 0.1`) |
|---|---:|---:|
| `1_2` | `0.0262882` | 1 |
| `2_4` | `-0.00359735` | 1 |
| `4_10` | `0.00383951` | 1 |

Gate verdict: **PASS**.

## Step 3 extended kinematics plots: completed

Produced extended ROOT outputs for `ZPT20_40`, `ZPT40_60`, `ZPT60_500` with `trkPT2_500` (all four sets per Z range):

- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_Gen_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT20_40-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_Gen_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_60-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pythiaMC_Gen_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT60_500-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_nominal_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT60_500-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT60_500-result.root`
- `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT60_500-result.root`

Produced requested PDFs in `Plots/20260213_Central/plots/pp/`:

- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/pp/evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT20_40_trkPT2_500-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/pp/evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT20_40_trkPT2_500-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/pp/evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_60_trkPT2_500-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/pp/evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT40_60_trkPT2_500-DeltaPhi-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/pp/evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT60_500_trkPT2_500-DeltaEta-result.pdf`
- `/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/plots/pp/evtWeightOn_fixEWBug_ZV6_trkV24_nmix10_ZPT60_500_trkPT2_500-DeltaPhi-result.pdf`

## Comparison note vs old non-fix tag

Old non-fix `evtWeightOn_ZV6_trkV24_nmix10` showed large GEN `DeltaPhi` subtraction offsets in the baseline gate bins (from pre-fix baseline capture):

- `trkPT 1_2`: `-3.90324`
- `trkPT 2_4`: `-2.0673`
- `trkPT 4_10`: `-0.667895`

With fix-tag production, these become near-zero (`O(1e-2)` to `O(1e-3)`), satisfying the mandatory gate and matching expected post-fix behavior.
