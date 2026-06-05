```md
# Task: Correct the Common-CM Symmetrized pPb+Pbp Statistical Study

## Goal

Review and, if necessary, correct the implementation of the statistical
uncertainty study for the **symmetrized combined pPb+Pbp vs pp result in the
common CM frame**.

Two corrections are required:

1. Use the **proper combined heavy-ion estimator**, consistent with the
   maintained analysis policy, rather than a simple weighted average of
   already-subtracted pPb and Pbp results.

2. Ensure that the **chi-square compatibility metrics are evaluated on the
   post-symmetrization observable with 6 effective degrees of freedom**, not
   12, since symmetrization reduces the independent dimensionality of the
   12-bin projected histogram to 6 mirrored bin pairs.

If this cannot be corrected from existing intermediate files, rerun the needed
production ROOT jobs.

---

## Background

The maintained heavy-ion combination policy is:

- build pPb and Pbp signal and mixed-event ingredients separately,
- combine their numerators,
- divide by the summed denominators separately for signal and mixed,
- subtract after combination,
- then project,
- then symmetrize if desired.

Therefore the heavy-ion combined central value is **not**, in general, to be
treated as:

```text
R_comb = w_pPb * R_pPb + w_Pbp * R_Pbp
```

unless that is shown to be algebraically equivalent for the exact estimator in
use.

The statistical treatment should match the actual estimator.

---

## Required corrections

# 1. Use the correct combined heavy-ion estimator

## 1.1 Replace weighted-average combination if present

Do **not** define the combined heavy-ion observable as a simple weighted average
of already-subtracted orientation-level histograms unless you explicitly verify
that this is equivalent to the maintained pooled ratio-of-sums construction.

The correct combined estimator should follow the maintained analysis structure.

For each orientation `c in {pPb, Pbp}`, define:

```text
S_c^num = signal numerator histogram
B_c^num = mixed-event numerator histogram
N_c^S   = signal denominator / normalization
N_c^B   = mixed denominator / normalization
```

Then construct:

```text
S_HI = ( S_pPb^num + S_Pbp^num ) / ( N_pPb^S + N_Pbp^S )
B_HI = ( B_pPb^num + B_Pbp^num ) / ( N_pPb^B + N_Pbp^B )
R_HI = S_HI - B_HI
```

Projection and symmetrization come **after** this.

## 1.2 Statistical uncertainty should match this estimator

If possible, compute the heavy-ion statistical covariance from the **pooled
jackknife over the union of pPb and Pbp events**, i.e. the correct jackknife
for the combined estimator itself.

Preferred method:

- build the combined estimator from pooled pPb ∪ Pbp event-level ingredients,
- form leave-one-event-out replicas of the pooled estimator,
- project,
- symmetrize,
- compute the jackknife covariance on the symmetrized result.

### If existing files already contain the required event-level ingredients
Use them directly.

### If existing files do not contain what is needed
Rerun the production ROOT steps as described in Section 5 below.

---

# 2. Symmetrization should be applied before the final compatibility test

## 2.1 Symmetrize the observable first

For a signed projected observable with mirrored bins `i <-> ni`, define the
symmetrized observable:

```text
R_sym(i) = 0.5 * [ R(i) + R(ni) ]
```

Apply this to:

- the combined heavy-ion result,
- the pp result,
- and the corresponding jackknife replicas / covariance.

## 2.2 Symmetrize the covariance correctly

If starting from an unsymmetrized covariance matrix `C`, construct the
symmetrized covariance:

```text
C_sym[i,j] = 0.25 * ( C[i,j] + C[i,nj] + C[ni,j] + C[ni,nj] )
```

This is equivalent to symmetrizing the jackknife replicas directly, and it
retains the non-diagonal contribution from the mirrored bin pairing.

### Preferred treatment
If jackknife replicas are available, symmetrize the replicas first and then
recompute the covariance from the symmetrized replicas.

This is preferred because it guarantees the covariance matches the exact
symmetrized estimator.

---

# 3. Chi-square metrics must use 6 effective degrees of freedom

## 3.1 Reason

The projected histograms have 12 bins, but after symmetrization the observable
contains only 6 independent mirrored bin pairs.

Therefore the post-symmetrization compatibility test must reflect that the
effective dimensionality is 6, not 12.

## 3.2 What to compute

For each observable (`DeltaEta`, `DeltaPhi`), define:

```text
delta = R_HI_sym - R_pp_sym
C_tot = C_HI_sym + C_pp_sym
```

Then compute the compatibility statistic on the **symmetrized result**.

### Preferred full-covariance test
Use:

```text
chi2 = delta^T * C_tot^+ * delta
```

where `C_tot^+` is the Moore-Penrose pseudoinverse or an equivalent stable
inverse restricted to the non-null subspace.

The effective degrees of freedom should be:

```text
ndf = rank(C_tot)
```

For a properly symmetrized 12-bin histogram, this should generally be 6.

## 3.3 Diagonal-only diagnostic

If you also report a diagonal-only chi-square as a secondary diagnostic, it
should still reflect the **6 independent mirrored pairs**, not 12 redundant
bins.

That means either:

### Option A: reduce to 6 symmetrized bins explicitly
Construct the 6 retained symmetrized bins and use:

```text
chi2_diag = sum_{a=1..6} [delta_a^2 / (sigma_HI,a^2 + sigma_pp,a^2)]
ndf = 6
```

### Option B: keep a 12-bin storage format but do not quote 12 dof
If the histogram is stored as 12 symmetrized bins, do **not** interpret the
diagonal chi-square as a 12-dof test.

Preferred approach: explicitly reduce to the 6 unique mirrored bins for
diagnostic reporting.

---

# 4. Required output corrections

Update the study output so that:

- the heavy-ion central value reflects the proper pooled combination formula,
- the heavy-ion statistical covariance matches that estimator,
- the symmetrized compatibility tests are evaluated with 6 effective dof,
- any tables or text that currently describe the heavy-ion result as a simple
  weighted average are corrected,
- any quoted 12-dof chi-square on the symmetrized observable is removed or
  relabeled as non-rigorous.

### Specifically update
- PDF plots if the central values or error bars change
- `cma_sym-pvalue.tex`
- any review/summary markdown or note text associated with this study

---

# 5. Rerun instructions if production ROOT needs to be regenerated

If the required pooled jackknife ingredients are not available from existing
ROOT files, rerun the production needed to regenerate the common-CM signed
outputs with the appropriate event-level information.

## Resource instruction

If rerunning is necessary:

- use `nthread=40` **per task**
- run at most **2 tasks concurrently**
- for a total of **80 threads maximum**

Examples:
- one production task: `nthread=40`
- two concurrent production tasks: each `nthread=40`, total `80`

Do **not** exceed 80 total threads.

## What to rerun

Rerun only the minimum needed set to regenerate the common-CM ingredients for:

- pPb
- Pbp
- pp

including the event-level jackknife content required for:

- pooled heavy-ion combination,
- projection,
- symmetrization,
- covariance construction.

Document exactly what was rerun and why.

---

# 6. Deliverables

Return a short report in markdown with the following sections:

```text
## Common-CM symmetrized study correction

### Combination formula
- what was previously used
- what was changed
- whether the old weighted-average form was found to be invalid

### Statistical covariance
- whether pooled jackknife was implemented directly
- or whether equivalent event-level reconstruction was used
- whether production ROOT had to be rerun

### Symmetrization
- how the symmetrized central value was formed
- how the symmetrized covariance was constructed

### Chi-square metrics
- updated DeltaEta chi2 / ndf / p-value
- updated DeltaPhi chi2 / ndf / p-value
- confirm that post-symmetrization tests use 6 effective dof

### Files updated
- list of plots/tables/ROOT outputs changed

### Production rerun details
- yes/no
- commands / scripts used
- thread settings
```

---

# 7. Key review criteria

The corrected result should satisfy all of the following:

- [ ] The heavy-ion combined result is not described as a simple weighted average
      unless explicitly proven equivalent.
- [ ] The statistical uncertainty is matched to the actual combined estimator.
- [ ] Symmetrization is applied before the final compatibility test.
- [ ] The full-covariance compatibility metric uses 6 effective dof after
      symmetrization.
- [ ] Any diagonal compatibility metric is also reported in a 6-independent-bin
      form, not as a 12-dof test.
- [ ] Reruns, if needed, use `nthread=40` per task with at most 2 tasks at once.

---

# Final instruction

Correct the common-CM symmetrized pPb+Pbp statistical study so that the
combination formula, covariance treatment, and post-symmetrization chi-square
metrics are consistent with the maintained pooled heavy-ion estimator and the
6-degree-of-freedom structure of the symmetrized observable.
```