# Main-analysis `DeltaEta` / `DeltaPhi` uncertainty propagation review

## Question being answered

This note assumes the intended question is:

> keeping the present Z-track-pair filling and correction weights unchanged, should the **statistical uncertainty propagation** in the main analysis be changed so that the selected Z event is treated as the independent unit, rather than the individual Z-track pair?

## Short answer

**Yes, in principle the uncertainty propagation should be moved to an event/Z-level treatment if the goal is statistically rigorous error bars.**

**No, this does _not_ imply changing the histogram filling scheme or the central-value definition.**

So the right split is:

- **central observable:** keep exactly the current pair-based filling and all existing correction weights,
- **statistical uncertainty propagation:** likely upgrade to an event/Z-clustered treatment.

## Why the central-value definition should stay unchanged

The maintained main-analysis observable is a **per-Z associated-track yield**. In the code, every accepted track contributes to the correlation histogram with the full event, track, and residual weight:

```cpp
float weight = this_eventWeight * this_trackWeight * this_residualWeight;

h->Fill(trackDeta, trackDphi, weight);
h->Fill(-trackDeta, trackDphi, weight);
h->Fill(trackDeta, trackDphi2, weight);
h->Fill(-trackDeta, trackDphi2, weight);
```

`MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp`, lines 537-545

Then the projection step normalizes by `N_Z`, subtracts the mixed-event term, and projects:

```cpp
hData->Scale(1. / hNZData->GetBinContent(1));
hMixData->Scale(1. / hNZMixData->GetBinContent(1));
if (doSub) hData->Add(hMixData, -1);
TH1D *hProjY = (TH1D*) hData->ProjectionY(...);
TH1D *hProjX = (TH1D*) hData->ProjectionX(...);
```

`MainAnalysis/20241102_ZhadronVsZPt/makeProjection.C`, lines 70-103

So the central value is intentionally a pair-yield observable of the form

```text
theta_b = (sum_e y_e,b) / (sum_e a_e)
```

where:

- `b` is a `DeltaEta` or `DeltaPhi` bin,
- `a_e` is the Z-level normalization weight,
- `y_e,b` is the sum of weighted accepted tracks from event `e` that land in bin `b`.

This is the correct physics definition for the current measurement. Changing the fill unit from track pairs to one fill per event would change the observable and would be wrong.

## What is questionable in the current statistical treatment

The current ROOT `Sumw2` bookkeeping effectively treats the filled weighted pairs as the statistical atoms contributing to the histogram variance.

That is not the same as treating the **selected Z event** as the independent observation.

Within one selected Z event:

- many accepted tracks share the same sampled Z kinematics,
- many accepted tracks share the same event/VZ/Z-side weights,
- multiplicity fluctuations move many track entries together,
- and in the main analysis the same Z event also seeds multiple mixed-event combinations.

So the track-Z pairs are **not independent draws** in the statistical sense, even though they are the correct building blocks for the central value.

This is exactly the issue that showed up in the run-by-run `avg |DeltaEta|` study: pair-level propagation gave very small bars, while event-clustered propagation gave realistic uncertainties and brought `chi2/nu` back to about 1.

## Signal-only answer: should the uncertainty propagation change?

For the **signal-only** `DeltaEta` / `DeltaPhi` numerator, the answer is **yes**.

For a given signal bin `b`, define

```text
a_e = Z/event normalization weight for selected event e
y_e,b = sum_{t in bin b} w_et
```

where `w_et` contains the same track-level factors already used in the current histogram filling.

The current bin content remains

```text
S_b = (sum_e y_e,b) / (sum_e a_e)
```

but an event-clustered variance estimator would be

```text
sigma^2(S_b) ≈ [sum_e (y_e,b - S_b a_e)^2] / (sum_e a_e)^2
```

or

```text
sigma(S_b) = sqrt(sum_e (y_e,b - S_b a_e)^2) / (sum_e a_e)
```

This is the direct event-level analogue of what was implemented for the run-average scalar study.

### Interpretation

For the signal term alone, this is more statistically faithful than pair-level `Sumw2`, because it respects the fact that the independent unit is the selected Z event, not each accepted track.

## Mixed-event and subtraction answer: should the uncertainty propagation change?

Here the answer is also **probably yes**, but the implementation is more subtle.

The mixed-event piece is not just another simple event-sum histogram. In the current code:

- each selected signal Z event drives a loop over multiple mixed partners,
- the mixed-event contribution is normalized separately by `hNZMix`,
- and the final result is the difference of two ratio estimators:

```text
R_b = S_b - M_b
```

with

```text
S_b = (sum_e y_e,b) / (sum_e a_e)
M_b = (sum_k m_k,b) / (sum_k c_k)
```

where `k` labels the mixed-event construction units.

The key issue is that the current mixed-event procedure is **anchored on the top-level selected signal event loop**, so the same signal Z event contributes:

1. to the signal histogram,
2. to the mixed-event search,
3. and to multiple mixed-event pairings.

That means an exact event-level variance treatment should not just replace `Sumw2` independently for `hData` and `hMixData` and add errors in quadrature without thinking about the clustering and possible covariance.

## Best statistical framing for the main analysis

If this is upgraded, the natural independent unit is still the **selected signal Z event**, not the pair and not necessarily the individual mixed partner.

A practical conceptual decomposition would be:

### Signal contribution per selected signal event `e`

```text
Y_e,b^(sig) = total weighted signal-pair contribution from event e to bin b
A_e^(sig)   = signal normalization weight from event e
```

### Mixed contribution attached to the same selected signal event `e`

```text
Y_e,b^(mix) = total weighted mixed-pair contribution generated while processing signal event e
A_e^(mix)   = mixed normalization contribution generated while processing signal event e
```

Then one could define, still without changing the central observable,

```text
S_b = (sum_e Y_e,b^(sig)) / (sum_e A_e^(sig))
M_b = (sum_e Y_e,b^(mix)) / (sum_e A_e^(mix))
R_b = S_b - M_b
```

and estimate the variance using **event-level clustered contributions** associated with the top-level selected signal events.

That is much closer to the actual independence structure of the algorithm than pair-level `Sumw2`.

## Should this be changed in the main analysis?

### Scientific answer

**Yes, if the goal is the most statistically correct uncertainty propagation.**

The event/Z-level unit is the better-motivated independent unit for the variance because the final estimator is built from event-level ratios of correlated within-event track sums.

### Operational answer

**Yes, but not as a trivial one-line replacement.**

A correct implementation would need to address:

1. **signal term clustering** at the selected-Z-event level,
2. **mixed-event term clustering** in a way that matches how mixed events are actually generated,
3. **subtraction propagation** for `R_b = S_b - M_b`,
4. likely at least some treatment of **bin-to-bin covariance** if downstream fits or chi-square comparisons use these histograms quantitatively.

## What should *not* be done

The following would be the wrong takeaway:

- do **not** change the histogram fill definition from track pairs to one event entry,
- do **not** drop track-level weights or residual corrections from the central-value construction,
- do **not** assume the run-average scalar fix can be copied directly onto the full mixed-event-subtracted 2D analysis without redesigning the variance bookkeeping.

## Recommended conclusion

The best answer to the intended question is:

> **Yes, the main-analysis uncertainty propagation should probably move to an event/Z-level treatment if we want the statistical uncertainties to reflect the true independent unit.**
>
> **No, the pair-based filling and central-value definition should not change.**

In other words:

- **keep** the current Z-track-pair observable and correction scheme,
- **revisit** the statistical error propagation.

## Practical next step if this is pursued later

The cleanest future study would be to prototype an **event-clustered variance calculation** for one projected observable first, for example `DeltaEta_Result`, by:

1. accumulating signal and mixed contributions per selected signal Z event,
2. reproducing the existing central value exactly,
3. comparing event-clustered errors to the current `Sumw2` errors bin-by-bin,
4. then deciding whether the difference is large enough to justify a full propagated update of the maintained workflow.

## Bottom line

For the main analysis:

- **central value / histogram filling:** keep the present pair-based scheme,
- **statistical uncertainty propagation:** yes, there is a good statistical case that it should be upgraded to an event/Z-level treatment,
- **but** that upgrade has to be designed around the full signal + mixed-event + subtraction workflow, not copied blindly from the run-average scalar study.
