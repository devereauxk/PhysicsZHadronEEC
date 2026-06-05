```md
# Reference Note for Reviewer: Combining Signed pPb/Pbp and Symmetrizing Final Results

## Purpose

This note is intended as **review guidance only**. It is not an implementation
task list and does not instruct the agent to modify code. It summarizes the
statistically correct way to understand:

1. how the signed pPb and Pbp results should be **combined**, and
2. how the final combined result should be **symmetrized** together with its
   statistical and systematic uncertainties.

The note is organized in two frame conventions:

1. **pp / common CM frame**
2. **pPb-oriented CM-like frame**

These two formulations are intended to describe the same underlying physics
comparison in two coordinate conventions. The reviewer should expect them to be
equivalent if implemented consistently.

---

# Section 1: Combining and symmetrizing in the pp / common CM frame

## 1.1 Frame convention

In this convention, the comparison is performed in the **nucleon-nucleon
center-of-mass frame**, i.e. the frame in which pp is naturally defined.

Use:

- for **pp**:
  ```text
  eta_lab = eta_cm
  y_lab   = y_cm
  ```

- for **pPb**:
  ```text
  eta_cm = eta_lab - 0.465
  y_cm   = y_lab   - 0.465
  ```

- for **Pbp**:
  ```text
  eta_cm = eta_lab + 0.465
  y_cm   = y_lab   + 0.465
  ```

For Z bosons, "eta" should be understood as **rapidity y**.

---

## 1.2 Signed pPb and Pbp observables in the common CM frame

The correct starting point is that both pPb and Pbp are constructed as
**signed observables** in a common orientation convention.

That means the longitudinal sign convention must already be made consistent
between the two orientations at the object / observable-definition level, not
only by reflecting a final histogram for presentation.

### Review point
A reviewer should verify that pPb and Pbp are treated as the same signed
observable in the CM convention before any combination is performed.

---

## 1.3 How signed pPb and Pbp should be combined

The signed pPb and signed Pbp results should be combined at the level of the
underlying numerator and denominator ingredients, not by averaging already
projected final-result histograms bin-by-bin with ad hoc weights.

For each orientation `c in {pPb, Pbp}`, define:

```text
S_c^num = signal numerator histogram
B_c^num = mixed-event numerator histogram
N_c     = signal normalization denominator
M_c     = mixed-event normalization denominator
```

The combined heavy-ion signal and mixed terms are:

```text
S_HI = ( S_pPb^num + S_Pbp^num ) / ( N_pPb + N_Pbp )
B_HI = ( B_pPb^num + B_Pbp^num ) / ( M_pPb + M_Pbp )
```

and the combined signed heavy-ion result is:

```text
R_HI = S_HI - B_HI
```

Projection and final presentation normalization happen only **after** this
combined 2D result is formed.

### Review point
The reviewer should expect the maintained combination to be a
**pooled ratio-of-sums** construction, not a simple average of already-finished
projected `DeltaEta_Result` or `DeltaPhi_Result` histograms.

---

## 1.4 Statistical uncertainty on the combined signed result

The statistical uncertainty on the combined heavy-ion result should be computed
from a **pooled event-level jackknife** built from the union of the selected
pPb and Pbp event sets.

That is, if:

```text
E_HI = E_pPb union E_Pbp
```

then the leave-one-event-out replicas are formed by dropping one event at a
time from the pooled set and recomputing the combined estimator.

This means pPb and Pbp are treated as **statistically independent event
samples**, and the statistical uncertainty is computed directly for the pooled
estimator rather than by propagating two already-computed orientation-level
errors.

### Review point
A reviewer should regard this pooled jackknife as the correct statistical
treatment for the combined signed heavy-ion result.

---

## 1.5 What object is symmetrized

Only after the final **combined signed heavy-ion result** is formed may one
construct a symmetrized presentation version.

The object to symmetrize is therefore:

```text
R_HI_signed(x)
```

and not the individual pPb or Pbp results separately.

### Review point
A reviewer should verify that pPb and Pbp are **not** symmetrized separately
before being combined.

---

## 1.6 Central-value symmetrization

For a signed 1D observable with partner bins at `+x` and `-x`, the symmetrized
central value is:

```text
R_sym(x) = 0.5 * [ R_signed(x) + R_signed(-x) ]
```

for the retained side of the axis.

If only one side is displayed, each displayed bin is the average of the
positive and negative partner bins of the final combined signed result.

---

## 1.7 Statistical uncertainty on the symmetrized result

### Why covariance matters

The `+x` and `-x` bins are generally **not statistically independent** in this
analysis, because:

- the same selected Z event contributes to multiple bins,
- the reflection filling procedure couples opposite sides,
- the jackknife resampling unit is the selected Z event.

Therefore the exact variance of the symmetrized quantity is:

```text
Var[R_sym(x)] =
1/4 * [ Var(R(x)) + Var(R(-x)) + 2 Cov(R(x), R(-x)) ]
```

or:

```text
sigma_sym^2(x) =
1/4 * [ sigma^2(x) + sigma^2(-x) + 2 Cov(x, -x) ]
```

### Preferred statistical treatment

The preferred and most robust method is to use the **jackknife replicas
directly**.

For each jackknife replica `k`, form the symmetrized replica:

```text
R_sym^(k)(x) = 0.5 * [ R^(k)(x) + R^(k)(-x) ]
```

Then compute the jackknife variance from the ensemble of symmetrized replicas.

This is preferred because it automatically retains the **non-diagonal
contribution** `Cov(x, -x)`.

### Review point
The reviewer should expect the statistical uncertainty on the symmetrized result
to come from **symmetrized jackknife replicas**, or from an exactly equivalent
covariance-aware propagation. A purely diagonal propagation is only an
approximation.

---

## 1.8 Approximate fallback if covariance is unavailable

If only diagonal bin errors are available, an approximate propagation is:

```text
sigma_sym(x) ≈ 0.5 * sqrt[ sigma^2(x) + sigma^2(-x) ]
```

This assumes zero covariance between partner bins and is generally not exact in
this analysis.

### Review point
If this approximation is used, it should be identified as approximate and not
as the exact statistical treatment.

---

## 1.9 Systematic uncertainty on the symmetrized result

For each systematic variation `v`, start from the **signed** nominal and varied
results:

```text
R^(0)(x)
R^(v)(x)
```

Symmetrize the histograms themselves:

```text
R_sym^(0)(x) = 0.5 * [ R^(0)(x) + R^(0)(-x) ]
R_sym^(v)(x) = 0.5 * [ R^(v)(x) + R^(v)(-x) ]
```

Then define the symmetrized systematic shift for family `f` as:

```text
delta_f^sym(x) = max_v | R_sym^(v)(x) - R_sym^(0)(x) |
```

The total symmetrized systematic uncertainty is then:

```text
Delta_tot^sym(x) = sqrt[ sum_f ( delta_f^sym(x) )^2 ]
```

### Review point
The systematic variation should be symmetrized at the **histogram level**
before taking absolute deviations. One should not average already-absolute
positive-side and negative-side systematic magnitudes.

---

# Section 2: Combining and symmetrizing in the pPb-oriented CM-like frame

## 2.1 Frame convention

In this convention, all datasets are expressed in a common **pPb-oriented**
longitudinal convention.

Conceptually:

- **pPb** is kept in its native orientation,
- **Pbp** is mapped into the same proton-going sign convention,
- **pp** is re-expressed in the same oriented frame.

A typical oriented-coordinate definition is:

- for **pPb**:
  ```text
  eta_oriented = eta_lab
  y_oriented   = y_lab
  ```

- for **Pbp**:
  ```text
  eta_oriented = - eta_lab
  y_oriented   = - y_lab
  ```

- for **pp**:
  ```text
  eta_oriented = eta_lab + 0.465
  y_oriented   = y_lab   + 0.465
  ```

The exact algebraic mapping can differ by convention, but the essential review
criterion is that all datasets are expressed in the **same signed orientation
convention** before comparison or combination.

---

## 2.2 Signed pPb and Pbp observables in the oriented frame

In this frame, pPb and Pbp must again correspond to the same signed observable
definition.

For Pbp, the orientation mapping should be applied at the **object level**,
not merely by flipping the final signed `DeltaEta` histogram after construction.

### Review point
A reviewer should check that the Pbp orientation is aligned with pPb before
building the correlator, so that the signed observable is defined consistently
between the two beam orientations.

---

## 2.3 How signed pPb and Pbp should be combined in the oriented frame

The combination logic is the same as in the common CM frame: combine the
underlying numerator and denominator ingredients, not the already-finished
projected histograms.

Define for each orientation:

```text
S_c^num = signal numerator histogram in the oriented convention
B_c^num = mixed-event numerator histogram in the oriented convention
N_c     = signal normalization denominator
M_c     = mixed-event normalization denominator
```

Then:

```text
S_HI = ( S_pPb^num + S_Pbp^num ) / ( N_pPb + N_Pbp )
B_HI = ( B_pPb^num + B_Pbp^num ) / ( M_pPb + M_Pbp )
R_HI = S_HI - B_HI
```

This produces the combined signed heavy-ion result in the pPb-oriented frame.

### Review point
The reviewer should expect the same pooled ratio-of-sums construction here as
in the CM-frame formulation.

---

## 2.4 Statistical uncertainty on the combined signed result

As in the CM-frame formulation, the correct statistical treatment is a
**pooled event-level jackknife** over the union of the pPb and Pbp event sets,
now evaluated in the oriented-frame observable definition.

The statistical uncertainty is therefore not a post hoc propagation of two
separate orientation-level errors but a direct jackknife of the combined
estimator.

### Review point
A reviewer should regard this as the correct combination-statistics treatment
also in the pPb-oriented frame.

---

## 2.5 Symmetrization in the oriented frame

If a final symmetrized presentation result is desired in the pPb-oriented
frame, the same ordering applies:

1. form the final **combined signed** heavy-ion result,
2. only then symmetrize it.

The symmetrized central value is again:

```text
R_sym(x) = 0.5 * [ R_signed(x) + R_signed(-x) ]
```

for partner bins at `+x` and `-x`.

### Review point
Even in the oriented-frame convention, symmetrization should happen only after
combining the signed pPb and Pbp results.

---

## 2.6 Statistical uncertainty on the symmetrized result

The same covariance-aware logic applies:

```text
Var[R_sym(x)] =
1/4 * [ Var(R(x)) + Var(R(-x)) + 2 Cov(R(x), R(-x)) ]
```

The preferred treatment remains to use the **jackknife replicas directly**:

```text
R_sym^(k)(x) = 0.5 * [ R^(k)(x) + R^(k)(-x) ]
```

and then compute the jackknife variance from the symmetrized replicas.

### Review point
The reviewer should again expect the use of jackknife replicas so that
non-diagonal contributions are preserved.

---

## 2.7 Systematic uncertainty on the symmetrized result

As in the CM-frame formulation, each systematic variation should be
symmetrized at the histogram level before its deviation from nominal is
evaluated.

For each variation:

```text
R_sym^(0)(x) = 0.5 * [ R^(0)(x) + R^(0)(-x) ]
R_sym^(v)(x) = 0.5 * [ R^(v)(x) + R^(v)(-x) ]
delta_f^sym(x) = max_v | R_sym^(v)(x) - R_sym^(0)(x) |
```

and then:

```text
Delta_tot^sym(x) = sqrt[ sum_f ( delta_f^sym(x) )^2 ]
```

### Review point
The same systematic-ordering logic should hold regardless of frame convention.

---

# Reviewer checklist

A reviewer evaluating either frame-convention implementation should check:

- [ ] pPb and Pbp are both built as **signed** observables in a common
      convention before combination.
- [ ] Signed pPb and signed Pbp are combined through the underlying numerator /
      denominator ingredients, not by averaging already-finished projected
      result histograms.
- [ ] The statistical uncertainty on the combined heavy-ion result is computed
      with a **pooled jackknife** over the union of pPb and Pbp events.
- [ ] Symmetrization, if present, is applied only to the **final combined
      signed** result.
- [ ] The statistical uncertainty on the symmetrized result is derived from
      **symmetrized jackknife replicas**, or an exactly equivalent
      covariance-aware propagation.
- [ ] Any diagonal-only propagation is identified as approximate.
- [ ] For systematics, the nominal and varied **signed histograms** are
      symmetrized first, and only then are the systematic shifts computed.
- [ ] Total systematics are combined in quadrature across families only after
      the symmetrized per-family magnitudes are established.

---

# Bottom-line reviewer guidance

In both the **common CM frame** and the **pPb-oriented frame**, the proper
analysis logic is:

1. construct pPb and Pbp as **signed** observables in a common convention,
2. combine them through the underlying ratio-of-sums ingredients,
3. compute the combined statistical uncertainty with a **pooled jackknife**,
4. symmetrize only after the final combined signed result is formed,
5. use **jackknife replicas** to compute the statistical uncertainty on the
   symmetrized result so that non-diagonal contributions are retained,
6. symmetrize the nominal and varied systematic histograms before deriving the
   systematic shifts.

Any implementation that symmetrizes too early, combines already-projected final
results directly, or propagates only diagonal statistical errors after
symmetrization should be understood as approximate or potentially incorrect.
```