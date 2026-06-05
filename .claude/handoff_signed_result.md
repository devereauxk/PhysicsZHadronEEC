```md
# Task: Produce pp vs Combined pPb+Pbp Comparison Results in Two Frame Conventions (Option A and Option B)

## Goal

Produce the pp vs combined pPb+Pbp comparison using **two alternative but intended-to-be-equivalent frame conventions**:

- **Option A:** comparison in the **common CM frame**
- **Option B:** comparison in a **common pPb-oriented lab-like frame**

The purpose of this task is to generate both results and compare them as a methodological cross-check.

Do **not** replace the maintained nominal analysis. Produce validation / comparison outputs only.

---

## Physics conventions

## Beam rapidity shift

Use the heavy-ion beam rapidity shift:

```text
Delta_y = 0.465
```

## Dataset conventions

### pp
Default:
```text
eta_lab = eta_cm
y_lab   = y_cm
```

### pPb
Proton-going direction is toward **positive eta**:

```text
eta_lab = eta_cm + 0.465
y_lab   = y_cm + 0.465
```

equivalently:

```text
eta_cm = eta_lab - 0.465
y_cm   = y_lab   - 0.465
```

### Pbp
Proton-going direction is toward **negative eta**:

```text
eta_lab = eta_cm - 0.465
y_lab   = y_cm   - 0.465
```

equivalently:

```text
eta_cm = eta_lab + 0.465
y_cm   = y_lab   + 0.465
```

## Acceptance convention

The current nominal object acceptance is:

```text
|eta_track_lab| < 2.4
|y_Z_lab|      < 2.4
```

For Z bosons, whenever "eta" would normally appear, use **rapidity y**.

---

# Common requirements for both Option A and Option B

## Observable convention

Use the **signed** correlator as the analysis basis:

- compute signed pPb and signed Pbp,
- combine signed pPb and Pbp,
- only symmetrize afterward if needed for presentation.

Do **not** symmetrize before combining.

## Statistical treatment

Use the maintained **jackknife** statistical uncertainty propagation.

## Mixed-event treatment

Keep the mixed-event procedure unchanged except for any object/event acceptance changes implied by the frame convention and matched fiducial selection.

## Outputs required for both options

Produce at least:

- combined signed heavy-ion `DeltaEta_Result`
- combined signed heavy-ion `DeltaPhi_Result`
- pp `DeltaEta_Result`
- pp `DeltaPhi_Result`

Also produce, if available:

- symmetrized presentation versions
- difference histograms `(combined pPb+Pbp) - pp`
- statistical uncertainties on all plotted observables
- compatibility metrics between combined HI and pp

---

# Option A: Common CM-frame comparison

## Objective

Compare pp and combined pPb+Pbp in a **common center-of-mass frame**.

This is the cleaner and preferred physics formulation.

---

## A1. Define CM-frame object coordinates

### pPb
Transform objects to CM:

```text
eta_track_cm = eta_track_lab - 0.465
y_Z_cm       = y_Z_lab - 0.465
```

### Pbp
Transform objects to CM:

```text
eta_track_cm = eta_track_lab + 0.465
y_Z_cm       = y_Z_lab + 0.465
```

### pp
No shift needed:

```text
eta_track_cm = eta_track_lab
y_Z_cm       = y_Z_lab
```

---

## A2. Define the common CM acceptance

From the nominal lab acceptance, the overlap CM acceptance for pPb and Pbp is:

```text
-1.935 < eta_track_cm < 1.935
-1.935 < y_Z_cm       < 1.935
```

Apply this same CM acceptance to:

- pPb
- Pbp
- pp

That is:

```text
|eta_track_cm| < 1.935
|y_Z_cm|       < 1.935
```

---

## A3. Produce Option A observables

Using the CM-frame object coordinates and common CM acceptance:

1. compute signed pPb
2. compute signed Pbp
3. combine signed pPb + Pbp into the heavy-ion result
4. compute pp under the same CM acceptance
5. produce:
   - `DeltaEta_Result`
   - `DeltaPhi_Result`
   - optionally symmetrized presentation versions
   - `(combined HI) - pp`

Use the same central-value construction, weights, subtraction, projection, and jackknife procedure as in the maintained workflow.

---

# Option B: Common pPb-oriented lab-like frame comparison

## Objective

Compare pp and combined pPb+Pbp in a **common pPb-oriented frame**, where:

- pPb is kept in its native orientation,
- Pbp is mapped into the same forward/backward convention,
- pp is shifted by `+0.465` to share the same coordinate convention.

This is an alternative parameterization of the comparison.

---

## B1. Define the pPb-oriented frame

Use the pPb-oriented frame coordinates:

### pPb
Keep as-is:

```text
eta_track_oriented = eta_track_lab
y_Z_oriented       = y_Z_lab
```

### Pbp
Map into the same orientation as pPb by flipping the longitudinal sign at the **object level**:

```text
eta_track_oriented = - eta_track_lab
y_Z_oriented       = - y_Z_lab
```

Do **not** only flip the final `DeltaEta` histogram. The transformation must be applied at the object level before building the observable.

### pp
Shift pp into the pPb-oriented frame:

```text
eta_track_oriented = eta_track_lab + 0.465
y_Z_oriented       = y_Z_lab + 0.465
```

---

## B2. Define the common acceptance in the pPb-oriented frame

The goal is to impose the same physical overlap region as in Option A, but expressed in the pPb-oriented coordinates.

Use the acceptance corresponding to the CM-overlap region mapped into the pPb-oriented frame:

```text
-1.935 < eta_track_oriented < 1.935
-1.935 < y_Z_oriented       < 1.935
```

Apply this acceptance consistently to:

- pPb in oriented coordinates
- Pbp after orientation flip
- pp after the `+0.465` shift

### Important note
If, during implementation, it becomes clear that the exact overlap region in the pPb-oriented frame should instead be expressed with a different interval due to the coordinate definition, document that explicitly and produce both:
- the direct implementation above, and
- the corrected equivalent window if needed.

The key requirement is that Option B cover the **same physical fiducial region** as Option A.

---

## B3. Produce Option B observables

Using the oriented-frame coordinates and matched acceptance:

1. compute signed pPb
2. compute signed Pbp in the common pPb-oriented convention
3. combine signed pPb + Pbp into the heavy-ion result
4. compute pp in the same oriented frame
5. produce:
   - `DeltaEta_Result`
   - `DeltaPhi_Result`
   - optionally symmetrized presentation versions
   - `(combined HI) - pp`

Use the same central-value construction, weights, subtraction, projection, and jackknife procedure as in the maintained workflow.

---

# Required cross-comparison between Option A and Option B

## Objective

Check whether Option A and Option B give equivalent results within numerical/statistical precision.

## Required comparisons

For each of the following observables:

- combined HI `DeltaEta_Result`
- combined HI `DeltaPhi_Result`
- pp `DeltaEta_Result`
- pp `DeltaPhi_Result`
- `(combined HI) - pp` for `DeltaEta`
- `(combined HI) - pp` for `DeltaPhi`

compare:

```text
Option A result  vs  Option B result
```

## Metrics

For each comparison, return:

1. bin-by-bin difference
2. maximum absolute bin difference
3. chi-square using the statistical errors if meaningful
4. visual overlay

If statistical errors differ between Option A and B only because of tiny implementation details, document that.

---

# Step-by-step algorithm

## Step 1: Prepare transformed coordinates

For each dataset:
- compute the coordinates needed for Option A
- compute the coordinates needed for Option B

## Step 2: Apply acceptance for each option

For each option separately:
- apply the corresponding matched fiducial acceptance to tracks and Z bosons
- rebuild the observables under that selection

## Step 3: Build signed orientation-level observables

For pPb and Pbp separately:
- keep the signed-observable logic
- do not symmetrize before combining

## Step 4: Combine heavy-ion orientations

For each option:
- combine signed pPb and signed Pbp using the maintained combined heavy-ion construction
- use the same jackknife treatment over the pooled event set if available in this workflow

## Step 5: Build pp reference

For each option:
- compute the pp observable in the corresponding frame convention and acceptance

## Step 6: Form heavy-ion minus pp difference

For each option:
- compute `(combined HI) - pp`
- propagate statistical uncertainty using the maintained method

## Step 7: Compare Option A vs Option B

Return the requested cross-check tables and plots.

---

# Deliverables

Return results in markdown with the following structure:

```text
## Frame definitions
- Option A coordinates
- Option B coordinates

## Acceptance windows
- Option A acceptance
- Option B acceptance
- note on whether they are physically equivalent

## Outputs produced
- file names / histogram names / plot names

## Option A results
### combined HI
### pp
### HI - pp

## Option B results
### combined HI
### pp
### HI - pp

## Option A vs Option B comparison
### DeltaEta
### DeltaPhi
### HI - pp DeltaEta
### HI - pp DeltaPhi

## Tables
- chi2 / ndf / p-value where applicable
- max absolute bin difference
- summary of statistical uncertainties
```

---

# Minimum required tables

## Table 1: Observable summary

| Option | Dataset | Observable | N bins | Integral / sum | Notes |
|---|---|---|---:|---:|---|

for:
- Option A combined HI
- Option A pp
- Option B combined HI
- Option B pp

## Table 2: Option A vs Option B comparison

| Observable | Dataset | Max abs diff | chi2 | ndf | chi2/ndf | p-value |
|---|---|---:|---:|---:|---:|---:|

for:
- `DeltaEta_Result`
- `DeltaPhi_Result`
- `HI - pp` DeltaEta
- `HI - pp` DeltaPhi

---

# Important implementation cautions

- Do **not** only flip the final Pbp `DeltaEta` histogram in Option B.
  The orientation mapping must be applied at the object level before building the correlator.
- Do **not** compare results from different acceptances and call them Option A/B equivalents.
  The fiducial regions must correspond to the same physical overlap region.
- Do **not** change the central-value definition or weighting scheme.
- Do **not** replace the maintained nominal outputs.
- Document any ambiguity encountered in mapping the exact matched acceptance between the two options.

---

# Final instruction

Produce both:

- **Option A:** pp vs combined pPb+Pbp in the common CM frame
- **Option B:** pp vs combined pPb+Pbp in the common pPb-oriented frame

using matched fiducial acceptances and the signed-analysis convention, then compare the two options quantitatively and return the requested outputs.
```