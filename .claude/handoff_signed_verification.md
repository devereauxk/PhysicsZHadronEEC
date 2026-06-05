```md
# Task: Validate Signed pPb/Pbp Treatment with Matched CM Acceptance and pp-Boost Closure

## Goal

Carry out two validation studies for the signed-analysis working point:

1. **Primary validation:** pPb vs Pbp closure under a matched center-of-mass (CM) acceptance.
2. **Secondary validation:** MC closure of the pp boost prescription.

These studies are intended to validate the use of the **signed** correlator as the nominal analysis, with symmetrization applied only after combining signed pPb and Pbp.

Do **not** modify the nominal analysis. Produce validation outputs only.

---

## Physics conventions and frame definitions

### Default pp convention
For pp, use:

```text
eta_lab = eta_cm
```

For Z bosons, when "eta" appears below, interpret it as **rapidity y**.

### Beam-orientation boosts

Use the following conventions:

- **pPb**: proton beam goes toward **positive eta**
  - lab and CM are related by:
    ```text
    eta_lab = eta_cm + 0.465
    ```
  - equivalently:
    ```text
    eta_cm = eta_lab - 0.465
    ```

- **Pbp**: proton beam goes toward **negative eta**
  - lab and CM are related by:
    ```text
    eta_lab = eta_cm - 0.465
    ```
  - equivalently:
    ```text
    eta_cm = eta_lab + 0.465
    ```

### Current nominal acceptance
The present lab-frame acceptance is:

```text
|eta_track_lab| < 2.4
|y_Z_lab|      < 2.4
```

---

## Part A: Primary validation — pPb vs Pbp closure under matched CM acceptance

## Objective

Recompute the signed pPb and Pbp observables after imposing a **common CM-frame acceptance**, so that both orientations are compared in the same CM fiducial region.

Then compare the resulting pPb and Pbp observables to test whether they are statistically compatible.

This is the **primary validation** of the signed-analysis and boost/acceptance treatment.

---

## A1. Derive the common CM acceptance window

Starting from the nominal lab acceptance `|eta_lab| < 2.4`, derive the CM acceptance implied by each orientation.

### For pPb
Using:

```text
eta_cm = eta_lab - 0.465
```

the lab acceptance:

```text
-2.4 < eta_lab < 2.4
```

maps to:

```text
-2.865 < eta_cm < 1.935
```

### For Pbp
Using:

```text
eta_cm = eta_lab + 0.465
```

the same lab acceptance maps to:

```text
-1.935 < eta_cm < 2.865
```

### Common CM acceptance
Take the overlap of the two CM acceptances:

```text
-1.935 < eta_cm < 1.935
```

This is the matched CM acceptance window to be imposed on both orientations.

### Apply to both tracks and Z bosons
Use this same common CM window for:

- track pseudorapidity in CM:
  ```text
  -1.935 < eta_track_cm < 1.935
  ```
- Z rapidity in CM:
  ```text
  -1.935 < y_Z_cm < 1.935
  ```

---

## A2. Required event/object transformations

For each selected object, compute CM-frame coordinates as follows.

### pPb
```text
eta_track_cm = eta_track_lab - 0.465
y_Z_cm       = y_Z_lab - 0.465
```

### Pbp
```text
eta_track_cm = eta_track_lab + 0.465
y_Z_cm       = y_Z_lab + 0.465
```

Use these transformed values **only for the acceptance decision** in this validation study, unless the signed observable itself is explicitly defined in CM coordinates in the current implementation.

### Important instruction
Do **not** silently change the definition of the correlator observable unless required by the maintained signed-analysis convention.

If the maintained signed observable is already constructed in the common orientation / CM convention, preserve that definition. The purpose of this study is to change the **acceptance selection**, not to invent a new observable definition.

---

## A3. Produce matched-acceptance signed observables

Re-run or re-evaluate the signed pPb and signed Pbp observables with the following additional requirement:

- track accepted only if:
  ```text
  -1.935 < eta_track_cm < 1.935
  ```
- Z accepted only if:
  ```text
  -1.935 < y_Z_cm < 1.935
  ```

Everything else should remain unchanged:
- same event selection,
- same weights,
- same same-event / mixed-event procedure,
- same signed observable definition,
- same jackknife statistical treatment.

Produce at least:

- signed `DeltaEta_Result` for pPb and Pbp under matched CM acceptance
- signed `DeltaPhi_Result` for pPb and Pbp under matched CM acceptance

If available, also produce:
- 2D signed correlator surfaces before projection
- covariance matrices / jackknife outputs for the projected observables

---

## A4. Quantify pPb vs Pbp closure

Using the matched-CM-acceptance signed observables, compare pPb and Pbp.

### Required comparison products
Produce:

1. **Overlay plots**
   - pPb and Pbp signed results on the same axes
   - for `DeltaEta_Result`
   - for `DeltaPhi_Result`

2. **Difference plots**
   - `pPb - Pbp` bin-by-bin

3. **Statistical compatibility metrics**
   - diagonal chi-square test:
     ```text
     chi2 = sum_i (R_i^pPb - R_i^Pbp)^2 / (sigma_i^pPb^2 + sigma_i^Pbp^2)
     ndf = number of included bins
     p-value = Prob(chi2, ndf)
     ```
   - if covariance matrices are readily available, also compute the full-covariance chi-square as an optional cross-check

4. **Pull distribution**
   For each bin:
   ```text
   pull_i = (R_i^pPb - R_i^Pbp) / sqrt(sigma_i^pPb^2 + sigma_i^Pbp^2)
   ```
   Return the pull values and summary statistics:
   - mean pull
   - RMS pull
   - maximum absolute pull

### Do not interpret
Do **not** make the final physics decision. Just return the compatibility metrics and plots.

---

## Part B: Secondary validation — MC closure of the pp boost prescription

## Objective

Test the boost prescription itself using pp MC.

Since pp is unboosted by default (`eta_lab = eta_cm`), artificially apply the pPb/Pbp boosts to pp MC and study how the acceptance changes under the same CM-window logic.

This is a **closure / robustness study of the boost prescription**, not a data-vs-MC agreement test.

---

## B1. Construct boosted pp-MC variants

Starting from the default pp-MC sample with:

```text
eta_lab = eta_cm
y_lab   = y_cm
```

construct three analysis variants:

### Variant 0: nominal pp
No boost:
```text
eta_cm = eta_lab
y_cm   = y_lab
```

Acceptance:
```text
|eta_lab| < 2.4
|y_lab|   < 2.4
```

### Variant 1: pp boosted like pPb
Apply:
```text
eta_cm = eta_lab - 0.465
y_cm   = y_lab - 0.465
```

Then impose the matched CM acceptance:
```text
-1.935 < eta_cm < 1.935
-1.935 < y_cm   < 1.935
```

### Variant 2: pp boosted like Pbp
Apply:
```text
eta_cm = eta_lab + 0.465
y_cm   = y_lab + 0.465
```

Then impose the matched CM acceptance:
```text
-1.935 < eta_cm < 1.935
-1.935 < y_cm   < 1.935
```

---

## B2. Produce pp-MC closure outputs

For each pp-MC variant, produce:

- signed `DeltaEta_Result`
- signed `DeltaPhi_Result`

Also produce the following ratios / comparisons:

1. **Nominal vs pPb-boosted pp MC**
2. **Nominal vs Pbp-boosted pp MC**
3. **pPb-boosted vs Pbp-boosted pp MC**

### Required products
For each comparison:
- overlay plots
- ratio plots if meaningful
- difference plots
- diagonal chi-square / p-value if statistical bars are available
- bin-by-bin deviation table

### Main check of interest
The most important MC-closure check is:

```text
pp_MC_boosted_like_pPb  vs  pp_MC_boosted_like_Pbp
```

These should be consistent after applying the common CM acceptance if the boost prescription is behaving symmetrically.

---

## Implementation details and guards

### Signed observable convention
Preserve the maintained signed-analysis definition. Do not re-symmetrize before combining.

### Acceptance application
The matched CM acceptance should be implemented as an **additional selection requirement** on the transformed CM coordinates.

### Tracks vs Zs
Apply the CM acceptance consistently to:
- tracks using `eta`
- Z bosons using `y`

### Mixed-event treatment
Keep the existing mixed-event machinery unchanged apart from whatever object/event acceptance is affected by the CM-cut requirement.

### Statistical uncertainties
Use the maintained jackknife statistical uncertainties for the projected observables if available in this workflow.

### Binning
Use the same final plotting binning as the maintained signed-analysis outputs.

---

## Deliverables

Return results in markdown with the following sections.

```text
## Matched CM acceptance definition
- pPb CM window from nominal lab acceptance
- Pbp CM window from nominal lab acceptance
- common CM window used in the study

## Part A: pPb vs Pbp matched-CM closure
### Inputs
### Output files / plots
### Compatibility metrics
### Pull summary

## Part B: pp-MC boost closure
### Variants produced
### Output files / plots
### Comparison metrics

## Tables
- chi2 / ndf / p-value for all requested comparisons
- pull summaries
- any bin-by-bin ratio / difference summaries

## Notes
- implementation details
- any edge cases
- any deviations from the requested setup
```

---

## Minimum required metrics table

For each of the following comparisons:

- pPb vs Pbp under matched CM acceptance
- nominal pp MC vs pPb-boosted pp MC
- nominal pp MC vs Pbp-boosted pp MC
- pPb-boosted pp MC vs Pbp-boosted pp MC

and for each observable:

- `DeltaEta_Result`
- `DeltaPhi_Result`

return:

| Comparison | Observable | N bins | chi2 | ndf | chi2/ndf | p-value |
|---|---|---:|---:|---:|---:|---:|

---

## Do not

- Do **not** replace the maintained nominal analysis with these validation outputs.
- Do **not** symmetrize pPb or Pbp before comparing them in Part A.
- Do **not** interpret the results as a final ARC conclusion.
- Do **not** compare pp data directly to pPb data as a closure test in this task.
- Do **not** use pp-data / MC ratios as the primary validation metric here.

---

## Final instruction

Carry out:

1. the **matched CM acceptance pPb vs Pbp closure test**, and
2. the **pp-MC boost-prescription closure test**

using the signed-analysis convention and the frame transformations specified above, and return the requested plots, tables, and compatibility metrics.
```