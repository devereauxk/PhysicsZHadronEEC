# pp energy-extrapolation systematic plan

This note is a **planning document only**. It describes a concrete path to produce a note-facing systematic uncertainty for the pp energy-extrapolation correction using the data and MC ingredients that already exist in this repository. Nothing in this file implies that the production has been run yet.

## Note anchor

The current Overleaf systematic section only contains a stub:

- `~/OverleafZHadronInPPb/src/systematics.tex`
  - `\subsection{Energy extrapolation}`
  - `energy extrapolation in pp, using fluctuation of correction factor, use toy model to vary and compare to refence`

The nominal correction itself is documented in:

- `~/OverleafZHadronInPPb/src/analysis.tex`
  - Section `Energy Extrapolation`
  - current nominal figure inputs `20260321_EnergyExtrapolation_EEV3.*`

## Existing ingredients we can reuse

### Nominal energy-extrapolation derivation

Current promoted pp energy-extrapolation weight file:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260321_EnergyExtrapolation_EEV3.root`
- exported through `OfficialWeightDictionary.sh` as:
  - `OFFICIAL_EE_WEIGHT_FILE_PP`
  - `EEWeightFile_PP`

Current derivation workflow:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/finalAnalysis.sh`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/correction.C`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/plot_corrections.C`
- `MainAnalysis/20260222_EnergyExtrapolation/workflow/plot_closure.C`

### Data ingredients

Inputs used by the current nominal derivation workflow:

- pp 5.02 TeV reco data:
  - `MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pp-v11-Zpt0.root`
- pPb 8.16 TeV reco data:
  - `MainAnalysis/20260222_EnergyExtrapolation/workflow/pythia-analysis.sh` currently uses
    - `pPbSample/V0.2/PPbData_Reco.root`
    - `pPbSample/V0.2/PbPData_Reco.root`
  - and merges them into:
    - `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/pPb-PbP-816.root`

### MC ingredients

Available pp MC inputs from `OfficialWeightDictionary.sh`:

- reco MC:
  - `OFFICIAL_MCRECOINPUT_PP=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-v11-Zpt0.root`
- gen MC:
  - `OFFICIAL_MCGENINPUT_PP=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-gen-v11-Zpt0.root`

Use note:

- per `analysis.tex`, the energy weight applies to **reco-level pp data and reco-level pp MC**.
- gen-level MC keeps energy weight equal to `1` and is only useful as a closure/reference ingredient.

### Nominal pp corrected outputs

The current pp nominal corrected chain already consumes `EEWeightFile_PP` in:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`

with final outputs written as:

- `MainAnalysis/20241102_ZhadronVsZPt/plots/pp_trkResidual_<OFFICIAL_TAG_PP>_ZPT*-result.root`

## Recommended systematic definition

Because the note text explicitly calls for a **toy-model fluctuation of the correction factor**, the cleanest uncertainty definition is:

1. fluctuate the nominal pp energy-extrapolation correction histogram in toy replicas using the statistical uncertainty of the input spectra used to build it;
2. rerun the pp corrected-data chain with those fluctuated energy-weight files;
3. quote the pp energy-extrapolation systematic as the **bin-by-bin spread of the toy-varied corrected results relative to the nominal corrected pp result**.

Recommended quoted observable:

- for each `DeltaPhi_Result<trkPT>` and `DeltaEta_Result<trkPT>` bin, use the **RMS of `(toy - nominal)`** across the accepted toy ensemble.

Why RMS is the preferred default here:

- the source variation is continuous and toy-based rather than a small discrete set of named up/down alternatives;
- RMS directly summarizes the propagated width of the correction uncertainty;
- it can still be converted into a note-facing absolute systematic histogram in the same style as the other harvested families.

If the note later wants a more conservative prescription, the alternative would be the half-width of the central 68% toy interval or the maximum absolute toy excursion. That choice should be made explicitly before implementation.

## Exact RMS definition to use for the toy family

For a fixed observable/bin, define:

- `N_i` = the **nominal** pp corrected bin content in bin `i`
- `T_i^{(k)}` = the propagated pp corrected bin content in the same bin from toy `k`
- `N_toy` = the number of accepted toys

Then the recommended quoted uncertainty for bin `i` is the **nominal-centered RMS**:

```text
sigma_i^(EE) = sqrt( (1 / N_toy) * Sum_{k=1..N_toy} ( T_i^(k) - N_i )^2 )
```

This is the quantity that should be written into:

- `EnergyExtrapolation_DeltaPhi`
- `EnergyExtrapolation_DeltaEta`

### Why center on the nominal histogram

The toy ensemble is meant to represent uncertainty **around the promoted nominal correction**. The note-facing central value remains the nominal corrected pp histogram, so the uncertainty should be measured with respect to that nominal result rather than with respect to the toy mean.

That means the family is interpreted exactly like the existing harvested uncertainties:

- central value = nominal pp corrected histogram
- systematic band = spread of toy-varied pp corrected histograms around that nominal

### Small concrete example

Suppose one `DeltaPhi_Result2_500` bin has:

- nominal bin content:
  - `N_i = 0.80`
- three toy-propagated values:
  - `T_i^(1) = 0.82`
  - `T_i^(2) = 0.77`
  - `T_i^(3) = 0.81`

Then:

```text
sigma_i^(EE)
  = sqrt((1/3) * [ (0.82 - 0.80)^2 + (0.77 - 0.80)^2 + (0.81 - 0.80)^2 ])
  = sqrt((1/3) * [ 0.0004 + 0.0009 + 0.0001 ])
  = sqrt(0.0004666667)
  = 0.0216
```

So the harvested absolute systematic uncertainty for that bin would be:

- `EnergyExtrapolation(bin i) = 0.0216`

### Recommended implementation details

- use the accepted toy count in the denominator exactly as written above
- do **not** subtract statistical bin errors from the propagated toy spread
- do **not** apply Bessel's correction (`N_toy - 1`) unless there is an explicit note-level decision to reinterpret the toy set as a small-sample estimator rather than the full propagated ensemble
- skip any toy output that is non-finite in a given bin and record the accepted toy count used for that bin if such filtering is ever needed

### Optional diagnostic outputs

Even if the final note-facing family uses only the RMS, it is useful to save or print:

- the toy mean:
  - `mu_i = (1 / N_toy) * Sum_k T_i^(k)`
- the largest absolute excursion:
  - `max_k |T_i^(k) - N_i|`

These are diagnostics only. The recommended quoted family remains the nominal-centered RMS.

## Proposed production steps

### 1. Freeze the nominal reference

Before making a systematic:

- keep the current promoted nominal correction file
  - `20260321_EnergyExtrapolation_EEV3.root`
- record the exact derivation inputs used to make it
- record the exact pp nominal corrected-data outputs that consume it

This avoids mixing a toy uncertainty derived from one correction version with a different promoted nominal.

### 2. Produce toy energy-extrapolation correction files

Add a dedicated toy-producing step under:

- `MainAnalysis/20260222_EnergyExtrapolation/workflow/`

Recommended new artifact:

- a ROOT macro or compiled helper that reads:
  - `output/pp-502.root`
  - `output/pPb-PbP-816.root`
  - and the nominal correction histogram from `correction.C`

Recommended toy procedure:

1. project the pp and merged pPb/PbP `Z p_{T}` spectra exactly as the nominal `correction.C` already does;
2. normalize by `N_Z` in the same way as the nominal derivation;
3. fluctuate the input bin contents toy-by-toy using their statistical uncertainties;
4. rebuild the toy correction ratio
   - `pPb(8.16 TeV) / pp(5.02 TeV)`
5. save all toy correction histograms into a single ROOT file, for example:
   - `output/20260321_EnergyExtrapolation_EEV3_toys.root`

Recommended practical choices:

- use enough toys that the RMS is stable; `O(100)` is a reasonable starting point
- enforce positive toy weights
- keep the exact same `Z p_{T}` binning as the nominal correction file

### 3. Validate the toy correction ensemble at the correction level

Before propagating to the full pp correlation:

- plot the nominal correction together with the toy band
- verify the nominal closure plot sits near the center of the toy ensemble
- identify any pathological high-`p_{T}` bins where toys become unstable because of very low statistics

If the highest-`p_{T}` bins fluctuate wildly, decide whether to:

- merge those bins before toy production, or
- smooth/freeze the tail in a documented way

That decision must be made once and then used consistently for the nominal plus toys.

### 4. Propagate toys through the pp corrected-data chain

Use the existing pp central corrected-data chain in:

- `MainAnalysis/20241102_ZhadronVsZPt/central.sh`

but replace the nominal `EEWeightFile_PP` with each toy correction file while keeping:

- the same pp input
- the same nominal Z correction
- the same nominal residual correction
- the same official tag structure otherwise

Recommended naming pattern for propagated toy outputs:

- `pp_trkResidual_<OFFICIAL_TAG_PP>_EEVar000_ZPT*-result.root`
- `pp_trkResidual_<OFFICIAL_TAG_PP>_EEVar001_ZPT*-result.root`
- ...

Do **not** fold the toy index into the promoted official tag itself. Keep the official tag fixed and append a dedicated variation suffix.

### 5. Use pp reco MC as a sanity/closure ingredient

We do have pp MC ingredients, and they should be used as a validation layer, not as the primary uncertainty source.

Recommended MC use:

- run the same toy energy-weight ensemble on **pp reco MC**
- compare toy-varied reco-MC outputs against the nominal reco-MC output
- confirm the propagated spread is smooth and statistically sensible

Use gen MC only as a reference/closure input:

- gen-level pp MC should keep energy weight equal to `1`
- it can be used to confirm that the systematic is defined entirely on the reco-weighted side and not accidentally propagated into a place where the note says it should be absent

### 6. Harvest the uncertainty in `Systematics/20260329_pPbSystematics`

After the toy-varied pp outputs exist, add a new pp-only family to the compiled workspace:

- family name:
  - `EnergyExtrapolation`

Implementation concept:

1. extend `CalculateSystematics.cpp` so it can accept a list of pp energy-toy files
2. for each observable/bin:
   - load the nominal pp corrected histogram
   - compute `(toy - nominal)` for every toy
   - fill the output uncertainty histogram with the nominal-centered toy RMS
3. write:
   - `EnergyExtrapolation_DeltaPhi`
   - `EnergyExtrapolation_DeltaEta`

Collision applicability:

- `pp`: include `EnergyExtrapolation`
- `pPb`, `PbP`: do not include it

### 7. Add plotting support

Once harvested, add `EnergyExtrapolation` to the pp plotting family list in:

- `Systematics/20260329_pPbSystematics/run.sh`
- `PlotSystematics.cpp`

This should make the family appear in:

- absolute systematic overlays
- relative systematic overlays
- total systematic quadrature for pp

No standalone direct-comparison runner is strictly required at first, because the toy set is large and the compiled family histogram is the note-facing product.

### 8. Validation checks before promoting the result

The implementation should be considered valid only if all of the following are checked:

1. the toy correction ensemble reproduces the nominal correction as its center;
2. the pp toy-varied corrected outputs remain finite for all requested `ZPT` / `trkPT` bins;
3. the reco-MC propagation looks smooth and physically sensible;
4. the harvested `EnergyExtrapolation` family is non-zero in pp and exactly absent from pPb/PbP;
5. the pp total systematic changes only through the addition of the new family;
6. the note-facing absolute/relative/central pp plots render correctly with the new family included.

## Recommended implementation order

1. add a toy-production helper in `MainAnalysis/20260222_EnergyExtrapolation/workflow/`
2. make a small wrapper script that propagates the toy files through the pp corrected-data chain
3. test on one narrowed kinematic point first:
   - `ZPT40_350`
   - `trkPT2_500`
4. extend `Systematics/20260329_pPbSystematics/CalculateSystematics.cpp` and `run.sh`
5. only after the narrowed validation succeeds, scale to the full official pp bin set

## Open decisions to settle before implementation

### Which heavy-ion data input should define the nominal toy source?

The current nominal `EEV3` derivation workflow still uses:

- `pPbSample/V0.2/PPbData_Reco.root`
- `pPbSample/V0.2/PbPData_Reco.root`

If the official heavy-ion reco inputs are later refreshed and the energy-extrapolation nominal is rederived, the toys must be regenerated from the same refreshed nominal inputs. Do not mix a toy ensemble from one derivation version with a different promoted nominal correction file.

### Exact toy fluctuation model

The note only says “use toy model to vary.”

Before coding, explicitly decide:

- Gaussian fluctuation per normalized bin using histogram errors, or
- Poisson fluctuation at the unnormalized count level followed by renormalization

The second option is usually more faithful if the underlying inputs are event counts.

### Final summary metric

Recommended default:

- RMS of propagated `(toy - nominal)` per bin

Alternative if the note wants a conservative band:

- 68% central interval half-width, or
- maximum absolute excursion

## Expected outputs after implementation

When this is eventually implemented and run, the expected durable products are:

- toy correction ROOT file(s) under
  - `MainAnalysis/20260222_EnergyExtrapolation/workflow/output/`
- pp toy-varied corrected result ROOT files under
  - `MainAnalysis/20241102_ZhadronVsZPt/plots/`
- harvested pp `EnergyExtrapolation_{DeltaPhi,DeltaEta}` histograms under
  - `Systematics/20260329_pPbSystematics/output/`
- updated pp systematic overlay PDFs under
  - `Systematics/20260329_pPbSystematics/plots/`

## Minimal first implementation target

If this is handed to an analyzer later, the smallest safe first target is:

1. generate toys for the current promoted `20260321_EnergyExtrapolation_EEV3.root`
2. propagate them only to narrowed pp outputs at `ZPT40_350`, `trkPT2_500`
3. harvest a pp-only `EnergyExtrapolation` family for that narrowed point
4. inspect the resulting pp absolute/relative systematic plots

Only after that narrow test looks stable should the family be promoted to the full official pp systematic workflow.

## Audit of the current harvested-systematics method

The current compiled harvester in:

- `Systematics/20260329_pPbSystematics/CalculateSystematics.cpp`

is **not** RMS-based.

### What it does today

For the discrete variation families (`TrackSelection`, `MuonRejection`, `PUpp`, `PUpPb`, `ScaleFactor`), the current code does:

```text
family_i = max over variation files v of | V_i^(v) - N_i |
```

where:

- `N_i` is the nominal bin content
- `V_i^(v)` is the bin content from one variation file in that family

This is implemented in `BuildFamilyHistogram(...)` through:

- looping over the variation ROOT files
- taking `fabs(nominal - variation)` bin-by-bin
- retaining the **maximum** absolute deviation

Then the total systematic is built by quadrature:

```text
Total_i = sqrt( Sum_f family_i(f)^2 )
```

The tracking-correction family is a special case:

```text
TrackCorrection_i = | trackingFraction * N_i |
```

### Is the current method an RMS approach?

No.

It is an **envelope / maximum-deviation** approach for the discrete families, followed by quadrature across families.

### Would I recommend changing the existing discrete families to RMS?

No, not by default.

For the currently implemented families, the existing method is sensible because they are based on a **small discrete set of named alternatives**:

- loose vs nominal vs tight
- nominal vs `IsMuTaggedFalse`
- nominal vs `IsPURejectFalse`
- nominal vs `MuVar0..3`

For that style of systematic, the usual choices are:

- maximum absolute excursion, or
- specifically defined up/down envelope

Using RMS across such a small and non-random set of discrete alternatives would usually understate the intended systematic unless the note explicitly redefines those families probabilistically.

### What I recommend

- keep the **current envelope method** for the existing discrete families
- use the **nominal-centered toy RMS** only for the new pp energy-extrapolation family, because that family is naturally generated from a large stochastic toy ensemble

So the mixed prescription would be:

- `TrackSelection`, `MuonRejection`, `PU`, `ScaleFactor`:
  - keep max-absolute-deviation family construction
- `EnergyExtrapolation`:
  - use toy RMS around nominal
- `Total`:
  - continue combining the family magnitudes in quadrature

This keeps the new family matched to its natural uncertainty model without forcing an unnecessary redesign of the current systematics framework.
