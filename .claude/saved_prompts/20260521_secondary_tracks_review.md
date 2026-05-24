# Reviewer Assessment: Secondary Track Contamination

**What the current framework does**

The analysis applies three layers of secondary rejection at the skimming stage. First, the CMS `highPurity` flag is required for all selections — this is the tracking group's primary tool against fakes and secondaries, encoding hit pattern, track fit quality, and algorithmic compatibility. Second, impact parameter significance cuts are applied in both the transverse ($|d_{xy}|/\sigma < 3$) and longitudinal ($|d_z|/\sigma < 3$) directions relative to the primary vertex, which suppresses tracks from weak decays (K$_S^0$, $\Lambda$), photon conversions, and nuclear interactions with detector material. Third, the relative $p_\mathrm{T}$ uncertainty is required below 10%, which rejects poorly measured tracks that tend to be disproportionately secondary.

The Loose/Tight systematic variation changes the impact parameter significance thresholds to 5$\sigma$ and 2$\sigma$ respectively — this directly modifies the secondary acceptance window and is therefore the primary quantitative handle in the current framework.

**Where the framework falls short**

The Loose/Tight variation is a useful proxy but cannot cleanly isolate the secondary contribution, for two reasons. First, tightening the impact parameter cut also removes real primary tracks with poorly measured vertex association — the variation conflates secondary rejection with primary efficiency loss, and the two effects cannot be disentangled from the skim alone. Second, and more fundamentally, the impact parameter values ($d_{xy}$, $d_z$) are **not stored in the ZHadronMessenger skim output** after skimming. All track quality information is used at the skimmer stage to make the selection, then discarded. This means a post-hoc impact parameter distribution study — the most direct way to compare data and MC secondary background levels — cannot be done without going back to the raw forest files.

There is also no generator-level track matching stored in the skim for reco tracks. The analysis cannot identify which reco tracks are secondaries in the MC at the analysis stage. This rules out the cleanest possible check: running the correlation on MC with an explicit secondary veto (requiring generator-level match) and comparing to nominal.

**An important mitigating physics argument**

Secondary tracks from K$_S^0$ and $\Lambda$ decays, photon conversions, and nuclear interactions are kinematically isotropic relative to the Z boson direction at leading order. They are not expected to carry a genuine Z-correlated signal. In the mixed-event subtraction, an isotropic secondary component is present in both the signal and background distributions and largely cancels. The residual concern is a differential secondary fraction between data and MC — if the MC incorrectly models the secondary rate, the track efficiency correction absorbs a wrong secondary component and the corrected result is biased.

The residual correction closure (MC reco vs. MC gen) provides indirect sensitivity to this: if secondary tracks in reco MC are not present in gen MC and are not modeled correctly by the efficiency correction, they would show up as non-closure. The fact that closure is at the sub-percent level is a positive sign, but it is not a direct measurement of the secondary fraction.

**Best quantitative check within the current framework**

The most direct test that requires no skimmer changes exploits the fact that the `subevent` branch **is already stored in the skim for generator-level MC tracks** (it is stored at skimming time but is never actually read in the main analysis or plots — grep over the `MainAnalysis` and `Plots` directories returns nothing). For generator-level pPb MC, the sub-event index distinguishes the hard-scatter sub-event from the soft/background sub-events in the HIJING/EPOS generator structure. Running the analysis at gen level twice — once with all gen tracks, once requiring the hard-scatter sub-event only — would directly quantify the contribution of soft-background gen tracks to the correlation. This is not exactly the same as reco-level secondaries, but it gives a physics-motivated upper bound on the underlying event's contamination of the measured correlation without any infrastructure changes.

For a more rigorous reco-level check, the most actionable path would be to add a dedicated skimmer run that stores the generator-match index for each reco track (already computed during the efficiency weighting step, so the information is in scope at skim time), enabling a primary-vs-all comparison in MC. This is a targeted skimmer extension, not a full re-analysis.

**Summary**

The analysis is adequately protected against secondary tracks for the purposes of a measurement, but it lacks a dedicated secondary fraction study. The Loose/Tight systematic provides only indirect sensitivity, and the `subevent` information already in the skim is unexploited. The most informative quantitative check available without any code changes is a comparison of the residual correction closure computed with Tight vs. Nominal track selection — if the correction map and resulting closure are compatible between the two, secondaries are not driving the residual. If the collaboration wishes a more direct statement, the gen-level sub-event comparison described above can be done with existing infrastructure in a single analysis pass.
