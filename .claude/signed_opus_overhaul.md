# Plan: Migrate the Nominal Analysis to the Signed Common-CM Convention and Regenerate the Inclusive Stack

## Goal

Bring the full analysis repository up to date with the **signed-computation convention** and regenerate the full inclusive result stack for the nominal comparison:

- **pp**: energy-extrapolated pp reference
- **pPb**: combined signed pPb+Pbp result in the common CM convention

The deliverable is the fully corrected inclusive result set, including:

- final 1D and 2D central-value plots,
- jackknife statistical uncertainties,
- full systematic uncertainties,
- updated covariance / correlation products,
- updated note text and inclusive figures.

This plan applies **for the inclusive kinematic selection only**:

- `Z pT: 0–500 GeV`
- `track pT: 0.5–15 GeV`

The scan points are not yet required; leave scan placeholders blank where needed.

---

## 1. Core analysis convention to adopt

The nominal analysis should be migrated to the **signed** convention:

- fill each Z–hadron pair **once** into the 2D correlation histogram,
- do **not** fill both signs by construction,
- keep the 2D histogram signed until after pPb and Pbp are combined,
- only then apply the final symmetrization.

The 2D histogram binning remains the **standard 12 × 12**:
- 12 bins in `DeltaEta`
- 12 bins in `DeltaPhi`

However, the **DeltaEta axis range** must be updated to reflect the new common-CM acceptance:

```text
DeltaEta in [-3.87, +3.87]
```

Do **not** continue to use the old `[-4, +4]` plotting/range definition for the new official outputs.

---

## 2. Common-CM / common-signed coordinate convention

The longitudinal coordinates must be evaluated in the **common CM convention** before the analysis selection is applied.

### pPb
For pPb, boost track eta and Z rapidity into the CM frame as:

```text
eta_track_CM = eta_track_lab - 0.465
y_Z_CM       = y_Z_lab       - 0.465
```

### Pbp
For Pbp, directly express the event in the same signed convention that will later be used for combination. That is, apply the CM boost and the longitudinal sign flip together:

```text
eta_track_CM_signed = - (eta_track_lab + 0.465)
y_Z_CM_signed       = - (y_Z_lab       + 0.465)
```

This is equivalent to boosting to the CM frame and then reflecting to the pPb-like signed convention. The sign flip must be applied **consistently to both tracks and Z bosons**.

### pp
For pp, no longitudinal boost is applied:

```text
eta_track_CM = eta_track_lab
y_Z_CM       = y_Z_lab
```

### Important clarification
The boost affects the **longitudinal coordinates** (`eta`, `y`) and therefore the **accepted sample** and the definition of `DeltaEta`. It does **not** directly change the definition of `DeltaPhi`, which should continue to be constructed from azimuthal angles in the usual way.

---

## 3. Common fiducial acceptance

The acceptance cuts for the official common-CM comparison must be applied **after** the coordinate transformation above.

Apply the same acceptance to **both tracks and Z bosons**:

```text
|eta_track_CM| < 1.935
|y_Z_CM|      < 1.935
```

and similarly for the Pbp signed coordinates:

```text
|eta_track_CM_signed| < 1.935
|y_Z_CM_signed|      < 1.935
```

This is the common overlap acceptance across pp, pPb, and Pbp.

With these cuts applied to both tracks and Z bosons, the physical support of the 1D `DeltaEta` observable is:

```text
DeltaEta in [-3.87, +3.87]
```

All official plots, histogram ranges, and projections should be updated accordingly.

---

## 4. Observable definitions

The signed 2D histogram should be built from:

```text
DeltaEta = eta_track_common - y_Z_common
DeltaPhi = phi_track - phi_Z   (wrapped using the maintained convention)
```

where `eta_track_common` / `y_Z_common` are the coordinates in the common signed frame defined above.

Use the maintained signed azimuthal convention and ensure that the final `DeltaPhi` axis is handled consistently on:

```text
[-pi/2, 3pi/2)
```

---

## 5. Update and validate pp energy extrapolation

Before regenerating the final official inclusive plots:

- verify that the **official pp EE weight** is current,
- verify that the pp EE closure is good,
- update the EE section of the note with:
  - the corrected factor,
  - the inclusive closure plot.

No scan variations are required for EE at this stage.

---

## 6. Regenerate nominal inclusive central-value products

Rerun the inclusive nominal production for:

- pPb
- Pbp
- pp

using the new signed common-CM working point and the new `DeltaEta` range.

Replace the official inclusive products in the dictionary only after the new outputs are validated, and keep versioning / bookkeeping for the replaced products.

The Z, track, and EE weights are expected to already exist in the weight dictionary, but official outputs must be regenerated.

---

## 7. Combine pPb and Pbp using the maintained pooled prescription

Once pPb and Pbp have been converted into the same signed common convention, combine them using the maintained pooled heavy-ion prescription described in the pPb/Pbp combining section.

That means:

- combine the **signal numerators** across pPb and Pbp,
- combine the **mixed-event numerators** across pPb and Pbp,
- combine the corresponding normalizations,
- form the heavy-ion result only **after** pooling,
- do **not** combine already-finished projected histograms with ad hoc weights unless proven equivalent.

From that point onward, the combined heavy-ion result may be referred to collectively as the **“pPb”** curve for plotting, but internal bookkeeping should still make clear that it is the combined pPb+Pbp result.

---

## 8. Statistical uncertainty treatment

Use the statistical treatment described in:

```text
.claude/stat_unc_pPbPbp.md
```

### Clarification
The correct bookkeeping is a **pooled event-level jackknife** over the union of the pPb and Pbp event sets after they have already been transformed into the common signed convention.

That means:

- one leave-one-out replica per selected Z event,
- each replica removes exactly one event from either pPb or Pbp,
- the combined estimator is recomputed for the pooled sample,
- this yields the heavy-ion covariance for the actual combined estimator.

Do **not** compute separate pPb and Pbp jackknife uncertainties and then combine them afterward unless explicitly shown to be equivalent for the implemented estimator.

For pp, compute jackknife uncertainties in the same signed/common-CM setup.

---

## 9. pPb vs Pbp compatibility checks before combination

Update the pPb/Pbp combination section of the note with the new inclusive compatibility metrics using the **signed 12 × 12 results before combination and before symmetrization**.

Produce and update:

- chi-square compatibility metrics,
- KS-like / KS-test section as a secondary shape diagnostic,
- 1D overlays,
- any required covariance and correlation products for the inclusive selection.

### Clarification
The primary compatibility metric should be the **covariance-aware chi-square**.  
The KS-like test should be treated as a **secondary / shape-only diagnostic**, not as the primary decision metric.

---

## 10. Final symmetrization of the combined heavy-ion and EE-corrected pp results

After:

- combining signed pPb and Pbp,
- applying EE to pp,

perform the final symmetrization on the **combined pPb result** and the **energy-extrapolated pp result**.

At this stage the histograms are still stored as signed `12 × 12` histograms. After full 2D symmetrization there are only `6 × 6` independent bin values, although the histogram may still be stored in the original `12 × 12` layout.

### 2D symmetrization rule

For each orbit of four related bins:

```text
( DeltaEta,  DeltaPhi)
( DeltaEta, -DeltaPhi)
(-DeltaEta,  DeltaPhi)
(-DeltaEta, -DeltaPhi)
```

replace the four bin contents with their average:

```text
R_sym = 1/4 * (R1 + R2 + R3 + R4)
```

and assign that average back to all four bins.

### Important clarification about DeltaPhi
Because the `DeltaPhi` axis is defined on:

```text
[-pi/2, 3pi/2)
```

the “negative” `DeltaPhi` partner is not always the trivial array reflection. Use the correct maintained near-side / away-side mirrored-bin mapping.

---

## 11. Jackknife replica bookkeeping for the symmetrized result

The same symmetrization operation must be applied to the **jackknife replicas**, not just the central values.

For each replica `k`, define:

```text
R_sym^(k) = 1/4 * (R1^(k) + R2^(k) + R3^(k) + R4^(k))
```

Then compute the final statistical covariance from the ensemble of symmetrized replicas.

This is required so that the statistical uncertainty retains the correct non-diagonal contributions from the symmetrization.

---

## 12. Systematic uncertainties: rerun the full inclusive stack in the new signed working point

Rerun the entire systematic workflow for the inclusive selection using the new signed common-CM convention.

This will likely require recalculating official weight variations for:

- W
- R
- EE

This is expected to take significant time.

### Important clarification on systematic treatment
Systematics should **not** be derived only from already-final symmetrized outputs.  
Instead, for each systematic variation:

1. build the varied result in the **signed** convention,
2. for heavy-ion: combine signed pPb and signed Pbp,
3. for pp: apply EE if relevant,
4. symmetrize the varied result,
5. compare the symmetrized varied result to the symmetrized nominal result,
6. build per-family systematic magnitudes,
7. combine families in quadrature.

This full propagation is required for a correct systematic treatment.

---

## 13. Inclusive plotting deliverables

Update the inclusive central-value products and note plots to show:

- final pPb curve = combined pPb+Pbp
- final pp curve = EE-extrapolated pp
- no MC required at this stage

Update:

- inclusive 1D projections,
- inclusive 2D heatmaps,
- covariance / correlation heatmaps,
- inclusive result comparison plots.

On the 1D projections, show both:

- jackknife statistical uncertainty,
- full systematic uncertainty.

For scan figures:
- leave the figures in place,
- blank out or placeholder the scan plots for now if the scan production is not yet rerun.

---

## 14. Note / Overleaf updates

Update the analysis section of the note with:

- the signed analysis convention,
- the common-CM coordinate definition,
- the common acceptance definition,
- the new `DeltaEta` range,
- the pooled pPb/Pbp combination prescription,
- the pooled jackknife statistical treatment,
- the final symmetrization procedure.

Update the pPb/Pbp combination section with:

- the new inclusive pPb vs Pbp compatibility metrics,
- the chi-square discussion,
- the KS-like / KS-test discussion,
- updated inclusive covariance and heatmap products.

Update the EE section with:

- the corrected EE factor,
- the inclusive closure plot.

Update the systematic section with:

- the new inclusive systematic plots,
- the current list of systematic productions rerun under the signed working point.

---

## 15. Compute and rerun policy

This task will require significant compute.

If there is doubt that a production is stale, rerun it.

### Resource limits
- Up to **30 threads for a single task**
  - where “single task” means threads reading / writing against the same input skim family
- Up to **90 threads total**

Do not exceed these limits.

Keep clear bookkeeping of:
- which productions were rerun,
- which dictionary products were replaced,
- which note plots were updated,
- and which systematic variations remain pending.

---

## 16. Final deliverable checklist

The final inclusive deliverable should include:

- [ ] signed nominal production rerun for pPb / Pbp / pp
- [ ] updated EE factor and closure plot
- [ ] pPb+Pbp combined heavy-ion result in the common signed convention
- [ ] pp EE-extrapolated result in the common signed convention
- [ ] final 2D symmetrized heavy-ion and pp histograms
- [ ] final 1D projections with stat + sys uncertainties
- [ ] pooled jackknife covariance / correlation products
- [ ] updated pPb vs Pbp compatibility metrics
- [ ] updated systematic inclusive products
- [ ] updated note text and inclusive figures
- [ ] scan figures left blank / placeholder if not yet rerun

---

## Questions / implementation notes

If there is any ambiguity in:
- the maintained `DeltaPhi` mirrored-bin mapping,
- the exact pooled-jackknife implementation path in the full repo,
- the official dictionary replacement procedure,
- or the current state of the weight variations,

ask implementation questions before proceeding.