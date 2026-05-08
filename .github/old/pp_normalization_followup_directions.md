# Follow-up Directions for Analyzer: pp Reproduction After EventWeight Fix

## Goal
Reproduce pp comparison plots with the mixed-event event-weight bug fixed, using a **new unique tag containing `fix`**, and only proceed to extended kinematics after confirming GEN MC subtraction integrals are near zero.

## Required tag convention
Use:
- `evtWeightOn_fixEWBug_ZV6_trkV24_nmix10`
- `evtWeightOff_fixEWBug_ZV6_trkV24_nmix10` (optional control set)

Do not overwrite old `evtWeightOn_ZV6_trkV24_nmix10` products.

## Step 1 — Reproduce baseline fixed set (gate step)
Kinematics:
- `ZPT40_350`
- `trkPT`: `1_2`, `2_4`, `4_10`

Run with fixed analyzer binary and the new `fix` tag(s), producing:
- `pythiaMC_Gen_nominal_<fix-tag>_ZPT40_350-result.root`
- `pp_nominal_<fix-tag>_ZPT40_350-result.root`
- `pp_ZResidual_<fix-tag>_ZPT40_350-result.root`
- `pp_trkResidual_<fix-tag>_ZPT40_350-result.root`

## Step 2 — Mandatory normalization gate before proceeding
For `pythiaMC_Gen_nominal_<fix-tag>_ZPT40_350-result.root`, check:
- `DeltaPhi_Result1_2`
- `DeltaPhi_Result2_4`
- `DeltaPhi_Result4_10`

Acceptance: each integral must be “near zero” (recommended `|integral| < 0.1`; target is similar to the post-fix study, O(1e-2)).

If gate fails, stop and report (do not continue to Step 3).

## Step 3 — Produce requested additional pp plots
After Step 2 passes, run new selections:
- `ZPT20_40`, `ZPT40_60`, `ZPT60_500`
- `trkPT2_500`

Use the same `fix` tag and produce full pp comparison plots in:
- `Plots/20260213_Central/plots/pp/`

Expected outputs (per Z range):
- `<fix-tag>_ZPT20_40_trkPT2_500-DeltaPhi-result.pdf`
- `<fix-tag>_ZPT20_40_trkPT2_500-DeltaEta-result.pdf`
- `<fix-tag>_ZPT40_60_trkPT2_500-DeltaPhi-result.pdf`
- `<fix-tag>_ZPT40_60_trkPT2_500-DeltaEta-result.pdf`
- `<fix-tag>_ZPT60_500_trkPT2_500-DeltaPhi-result.pdf`
- `<fix-tag>_ZPT60_500_trkPT2_500-DeltaEta-result.pdf`

## Step 4 — Deliverables to report back
- Table of GEN MC `DeltaPhi` integrals for Step 2 gate bins.
- Confirmation gate pass/fail.
- List of generated ROOT files and new PDFs (full paths).
- Brief comparison note vs old non-`fix` tag behavior.
