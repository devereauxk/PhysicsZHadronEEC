# Task: Compute a Covariance-Aware KS-like Cumulative-Shape Test for pPb vs Pbp

## Goal

Compute a **covariance-aware KS-like cumulative-shape statistic** for the pPb
and Pbp projected observables. The statistic must account for:

- the statistical uncertainty of each dataset,
- the bin-to-bin covariance within each dataset,
- the fact that the observables may be signed / background-subtracted.

This statistic is intended as a **secondary shape diagnostic**. It is not a
replacement for the standard covariance chi-square compatibility test.

Do **not** interpret the result. Just return the numerical values and the
supporting covariance-derived quantities.

---

## Scope

Apply this to the final projected 1D observables used in the analysis, e.g.

- `DeltaEta_Result`
- `DeltaPhi_Result`

for the pPb and Pbp datasets.

If both observables are available, compute the statistic for each.

---

## Inputs

For each observable, obtain:

- the pPb bin values: `R_i^pPb`
- the Pbp bin values: `R_i^Pbp`
- the pPb statistical covariance matrix: `C_ij^pPb`
- the Pbp statistical covariance matrix: `C_ij^Pbp`

These covariance matrices should come from the **jackknife replicas** of the
projected 1D observables.

Use the same binning and bin ordering for pPb and Pbp.

### Requirements on inputs
- The covariance matrices must be square and match the number of bins.
- The pPb and Pbp observable vectors must have the same length.
- Use only regular histogram bins (exclude underflow/overflow).
- Preserve the natural bin order along the axis.

---

## Definitions

### Step 1: Difference vector

Define the bin-by-bin difference vector:

```text
Delta_i = R_i^pPb - R_i^Pbp
```

### Step 2: Total covariance

Because pPb and Pbp are statistically independent, the covariance of the
difference is:

```text
C_ij^tot = C_ij^pPb + C_ij^Pbp
```

### Step 3: Cumulative difference vector

Define the cumulative difference up to bin `k`:

```text
S_k = sum_{i <= k} Delta_i
```

### Step 4: Cumulative covariance matrix

Define the covariance matrix of the cumulative differences:

```text
V_kl = sum_{i <= k} sum_{j <= l} C_ij^tot
```

In particular, the variance of the cumulative difference at bin `k` is:

```text
V_kk
```

### Step 5: Covariance-aware KS-like statistic

Define the standardized cumulative difference at bin `k`:

```text
Z_k = S_k / sqrt(V_kk)
```

provided `V_kk > 0`.

Then define the KS-like test statistic as the maximum absolute standardized
cumulative excursion:

```text
T_max = max_k |Z_k|
```

This is the requested covariance-aware KS-like statistic.

---

## p-value calculation by toy Monte Carlo

Because the bins are correlated, the null distribution of `T_max` is **not**
the standard Kolmogorov distribution. Therefore compute the p-value using
toy replicas drawn from the full covariance matrix.

### Null hypothesis

Under the null hypothesis, pPb and Pbp measure the same underlying truth, so:

```text
Delta ~ N(0, C^tot)
```

### Toy procedure

Generate `N_toys` multivariate Gaussian toy difference vectors:

```text
Delta_i^(t) ~ N(0, C^tot)
```

For each toy `t`:

1. Compute the cumulative toy difference:
   ```text
   S_k^(t) = sum_{i <= k} Delta_i^(t)
   ```
2. Compute the standardized cumulative difference:
   ```text
   Z_k^(t) = S_k^(t) / sqrt(V_kk)
   ```
3. Compute the toy test statistic:
   ```text
   T_max^(t) = max_k |Z_k^(t)|
   ```

Then compute the p-value as the fraction of toys with test statistic at
least as large as observed:

```text
p = (# toys with T_max^(t) >= T_max^obs) / N_toys
```

### Number of toys

Use at least:

```text
N_toys = 100000
```

If computationally cheap, use more. Report the number used.

---

## Numerical stability and validation

### Covariance validation
Before generating toys:

- Check that `C^tot` is symmetric within numerical precision.
- Check that all diagonal entries are nonnegative.
- Check whether `C^tot` is positive semidefinite.

### If covariance is not numerically positive semidefinite
If small negative eigenvalues appear from numerical noise:

- symmetrize the matrix:
  ```text
  C^tot <- 0.5 * (C^tot + (C^tot)^T)
  ```
- then regularize by clipping tiny negative eigenvalues to zero.

Document any such regularization.

### Handling zero or tiny `V_kk`
If `V_kk <= 0` or numerically tiny for any cumulative bin:

- flag that bin,
- exclude it from the `T_max` scan,
- report which bins were excluded.

---

## Required outputs

For each observable, return:

### 1. Input summary
- number of bins
- confirmation that pPb/Pbp binning matches
- confirmation that covariance dimensions match
- whether covariance regularization was needed

### 2. Observed quantities
- the difference vector `Delta_i`
- the cumulative difference vector `S_k`
- the cumulative variances `V_kk`
- the standardized cumulative differences `Z_k`
- the observed statistic `T_max`

### 3. Toy-MC quantities
- number of toys used
- toy-estimated p-value
- optionally the mean and RMS of the toy `T_max` distribution

### 4. Per-bin cumulative table
Return a table with columns:

| Bin | `Delta_i` | `S_k` | `V_kk` | `sqrt(V_kk)` | `Z_k` |
|---|---:|---:|---:|---:|---:|

---

## Output format

Return results in markdown with one section per observable:

```text
## Covariance-aware KS-like statistic: <observable name>

### Input summary
- N_bins: ...
- Covariance regularized: yes/no
- Excluded cumulative bins: ...

### Observed statistic
- T_max: ...
- Toy p-value: ...
- N_toys: ...

### Cumulative table
| Bin | Delta_i | S_k | V_kk | sqrt(V_kk) | Z_k |
| --- | ---: | ---: | ---: | ---: | ---: |
| ... | ... | ... | ... | ... | ... |
```

If multiple observables are processed, provide one section for each.

---

## Pseudocode

### Build cumulative covariance

```python
import numpy as np

# Delta: shape (N,)
# Ctot:  shape (N,N)

N = len(Delta)

S = np.cumsum(Delta)

V = np.zeros((N, N))
for k in range(N):
    for l in range(N):
        V[k, l] = np.sum(Ctot[:k+1, :l+1])

Vdiag = np.diag(V)
Z = np.array([
    S[k] / np.sqrt(Vdiag[k]) if Vdiag[k] > 0 else np.nan
    for k in range(N)
])

Tmax_obs = np.nanmax(np.abs(Z))
```

### Toy generation

```python
# Ctot should be symmetrized / regularized if needed
toys = np.random.multivariate_normal(
    mean=np.zeros(N),
    cov=Ctot,
    size=N_toys
)

Tmax_toys = []
for toy in toys:
    S_toy = np.cumsum(toy)
    Z_toy = np.array([
        S_toy[k] / np.sqrt(Vdiag[k]) if Vdiag[k] > 0 else np.nan
        for k in range(N)
    ])
    Tmax_toys.append(np.nanmax(np.abs(Z_toy)))

Tmax_toys = np.array(Tmax_toys)
p_value = np.mean(Tmax_toys >= Tmax_obs)
```

---

## Alternative implementation using a cumulative matrix

You may also define a lower-triangular matrix `L` with:

```text
L_ki = 1 if i <= k else 0
```

Then:

```text
S = L * Delta
V = L * C^tot * L^T
```

This is equivalent and may be cleaner.

---

## Do not

- Do **not** use only diagonal bin errors; use the full covariance.
- Do **not** normalize the observable to unit area.
- Do **not** shift signed histograms to make them positive.
- Do **not** use the classical Kolmogorov distribution.
- Do **not** interpret whether the datasets are compatible.
- Do **not** replace the primary covariance chi-square test with this.

---

## Final instruction

Compute and return the covariance-aware KS-like cumulative-shape statistic
`T_max` and its toy-MC p-value for the available projected observables,
using the full jackknife covariance matrices for pPb and Pbp.