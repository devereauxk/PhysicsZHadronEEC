# JEWEL pPb nuclear modification ratio (R_pPb)

Z-tagged charged-particle nuclear modification ratio from the JEWEL v4 samples,
for comparison against Trajectum pPb R_AA predictions:

    R_pPb(pT) = [1/N_Z dN_ch/dpT]_pPb-hydro / [1/N_Z dN_ch/dpT]_pp-vacuum

Standalone study: reads the skims directly, does not touch the main analysis
directories or outputs.

## Method

- **Inputs**: `~/jewel/jewel_pp8160v3_MOD_500k.root` (pp vacuum, ptmin=0) and
  `~/jewel/jewel_pPb_v4_MOD_500k.root` (pPb Trajectum 2D hydro, all fixes).
- **Z pT reweighting**: both samples reweighted to the Pythia8+MG spectrum with
  their own derived weights (`../output/jewel_{pp,pPb}_zptweight.root`), so
  numerator and denominator share identical Z kinematics.
- **Event selection** (mirrors the main-analysis JEWEL defaults): gen Z with
  mass 60-120 GeV, signed y_Z in [-1.935, 1.935], pT^Z in [0, 500] GeV.
  Event weight = EventWeight (cross section) x Z pT weight.
  Faithfulness check: weighted N_Z = 3686.20 for pPb, exactly matching the
  main-analysis zrw run.
- **Track selection**: muon-tagged rejection, charged-only
  (trackCharge != 0; hole sentinel 999 passes), pT > 0.5 GeV, |eta| < 1.965
  (per study spec; note the main correlation analysis uses 1.935).
  pPb holes enter with weight -0.67 (charged-share scaling).
- **Binning**: 22 log-uniform pT bins, 0.5-80 GeV.
- **Curves**: inclusive pT^Z and pT^Z > 30 GeV (both numerator and denominator
  carry the same Z pT selection).

## How to run

```bash
bash run-raa.sh   # -> output/jewel_pPb_raa.{root,tsv}, plots/jewel_pPb_raa.pdf
```

## Results

| pT region | R_pPb (incl.) | R_pPb (ZPT>30) | structure |
|-----------|-------|-------|-----------|
| 0.5-0.63 GeV | 1.17 | 1.23 | recoil (medium-response) enhancement |
| ~1-1.6 GeV | 0.59-0.63 | 0.86-0.93 | hole-subtraction depletion (thermal momenta, <pT> ~ 0.8 GeV) |
| 2.5-4 GeV | ~0.95 | 1.0-1.07 | crossover |
| 6-20 GeV | 0.76-0.78 | 0.77-0.79 | parton energy-loss plateau |
| > 40 GeV | 0.95-0.99 | 0.95-0.99 | recovery toward unity |

The 6-20 GeV plateau matches the away-side correlation suppression (0.77
inclusive at 4-15 GeV) — consistent pictures of the same energy loss — and is
independent of the Z pT selection. The low-pT structure (enhancement below
0.6 GeV, depletion at ~1 GeV) is entirely the JEWEL medium-response model:
recoils add soft particles, holes subtract the thermal momenta they displaced;
at high Z pT the dip is shallower because the same thermal subtraction sits
under a much larger jet yield.

## Caveats for comparison with Trajectum R_AA predictions

- **Z-tagged, not minimum-bias inclusive**: per-Z-event yields select a recoil
  parton of known energy with no surface bias; suppression is generically
  2-3x stronger than inclusive-hadron R_pPb for the same medium. Trajectum
  percent-level MB predictions and this ~22-24% plateau are different
  observables through different lenses.
- **Ensemble**: the profile ensemble is centrality-biased relative to MB
  (<Ncoll> = 10.8 vs ~7; see `../Solution2/`), though the per-class study
  shows the modification is nearly independent of droplet size, so this
  shifts R_pPb by only ~0.01-0.03.
- The medium-response region (pT < 2.5 GeV) depends directly on the
  KEEPRECOILS + hole x0.67 treatment; the energy-loss plateau does not.
