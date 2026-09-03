# Draft email: JEWEL-2D / Trajectum pPb quenching

Draft for Wilke (Trajectum) and Isobel (JEWEL-2D). Attachments referenced:
`R_pPb.png`, `Z_hadron_deltaPhi.png`, `hydroprofile_time_temp.png`,
`x_y_temp.png`.

---

Hi Wilke and Isobel,

Thank you again for sharing the Trajectum sample and modified Jewel codebase
for pPb. We are now working to compare Jewel to our pPb Z-hadron correlation
measurement. We have attached some plots that might be of interest to you, and
have a few questions maybe you can help answer.

We see the 2D Jewel + Trajectum profile predicts sizable jet quenching in pPb.
`R_pPb.png` shows a large charged hadron nuclear modification ratio
R_pPb ~ 0.7. Here we use Jewel pp as baseline.

We also compare Jewel to Pythia8+Madgraph simulation for the Z-hadron
correlation function. `Z_hadron_deltaPhi.png` shows this as a function of
azimuthal angle deltaPhi between Z boson and charged hadron, in three track pT
selections. Here we also see a very strong quenching signal in which the high
pT hadron yield on the jet-side (at DeltaPhi ~ pi) is diminished and hadrons
are pushed to low pT.

Looking closer at the hydro profiles, we see a median event max temperature
T_max ~ 0.316 GeV and median hydro lifetime τ_last ~ 3.2 fm/c, and that out of
the 100 events there are only 57 distinct (T_max, τ_last) pairs. So 43/100 of
the points in the (T_max, τ_last) scatter plot in
`hydroprofile_time_temp.png` are duplicated/overlap. Plotting the temperature
(x, y) distribution in `x_y_temp.png` we see the thermal medium goes out
pretty far to ~4 fm.

On the duplicates: the sample directories are named `<jobid>.<eventindex>`,
and every duplicate pairs the same event index under different job ids (e.g.
.84/2.84/3.84) — it looks like the batch jobs ran the same deterministic event
sequence with a shared random seed and uploaded overlapping subsets.

Our questions:

- In the production config we see the pre-hydro stage is free-streaming until
  tau = 0.606 fm/c, while Jewel's medium ramps up linearly from tau = 0 with
  scattering active during that window. Should the Jewel ramp be disabled for
  a faithful coupling to these profiles, and was it active in your published
  small-system results?
- The dumps run down to the 149.7 MeV freezeout while Jewel only uses
  T > Tc = 0.17 GeV. Over what window does energy loss act in the Trajectum
  pPb R_AA prediction (temperature range, tau range)?
- The sample is minimum bias (flat 5% acceptance) but with
  `allowzeroentropyacceptance = false`, which we think explains the deficit
  of peripheral events we see (10% at Ncoll <= 3 vs ~35-40% expected for MB
  pPb). How are these removed events treated in minimum-bias hard-probe
  averages, and what fraction of MB pPb do they represent at 8.16 TeV?
- Can you confirm the batch jobs shared a random seed? We would naturally
  defer regenerating the sample (varied seeds, the missing events, more
  statistics) to you — but we would also be happy to run it ourselves if you
  could share the production tarball (or point us to the matching public
  Trajectum version with the jewelonly output) and how to vary the seed. We
  have your .par, job recipe, and converter already, so either way works.
- Splitting the Jewel sample by hydro-profile Ncoll class, we find the
  modification nearly independent of droplet size: the away-side 4-15 GeV
  yield ratio to pp is 0.85 / 0.79 / 0.78 for Ncoll <= 3 / 8-12 / >= 17 —
  even events with T_max barely above Tc quench by ~15%, so reweighting the
  ensemble changes the prediction by <= 0.03. Is this expected/known behavior
  for Jewel on small droplets, and are there recommended settings for a
  faithful small-system configuration (coupling, MDFACTOR/MDSCALEFAC, recoil
  treatment)?
- How much of the gap between Jewel's quenching and the percent-level
  Trajectum pPb R_AA expectation could come from the coupling / q-hat
  normalization alone?

Best regards,
Kyle

---

## Context notes (not for the email)

- T_c cutoff verified identical between stock and the 2D fork
  (medium-simple.f:514, medium-2D.f:799); the sub-T_c-halo hypothesis is
  refuted — do not resurrect it.
- Duplicate resolution (2026-08-12): md5 groups always pair the same event
  index across job labels — same-seed job overlaps, i.e. packaging artifacts.
  Correct hard-probe allocation: each unique event once, proportional to
  Ncoll (original dedup scheme). The mult-weighted v4 over-counts; impact is
  within the <= 0.03 reweighting bound (quenching is Ncoll-flat), so existing
  results stand, but CHANGES.diff and genPPbv4ZJet.sh should be reverted at
  the next regeneration.
- Trajectum production config (jewel/hydro/0smllnooverjewelMB.{par,sh},
  extractTrajectumForWilke.sh): sigma_NN = 71.93 mb, 96x96 grid over +-6 fm,
  numevents = 100, freezeouttemp = 0.1497 GeV, flat 5% MB acceptance with
  allowzeroentropyacceptance = false, free-streaming pre-hydro to
  tau = 0.606 fm/c, nucleon width w = 0.72 fm with nc = 4.5 constituent
  substructure, EPPS21 nPDF (soft sector).
- Key numbers behind the email: R_pPb plateau 0.76-0.79 at 6-20 GeV
  (inclusive and pT^Z > 30); away-side 4-15 GeV suppression 21-28% with +37%
  soft enhancement at pT^Z > 30; per-class ratios 0.85/0.79/0.78; medians
  T_max = 0.316 GeV, tau_last = 3.24 fm/c (Ncoll-weighted: 0.323, 3.45).
