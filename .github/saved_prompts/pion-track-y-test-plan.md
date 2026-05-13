# Pion-mass track-y test recommendation

## Recommendation

For this check, the safest and cleanest approach is to create a **new dated study folder under `MainAnalysis/`** and test there, rather than modifying the maintained `MainAnalysis/20241102_ZhadronVsZPt/` workflow in place.

**Recommended path**

- `MainAnalysis/20260511_PionTrackYStudy/`

Do **not** branch the current maintained working point for this test.

## Why this is the better choice

This repository is organized around **standalone, date-stamped analysis snapshots**. A test that changes the physics definition of the horizontal observable from

- current: `DeltaEta = Z rapidity - track eta`

to

- test: `DeltaY = Z rapidity - track rapidity(track p, pion mass)`

is not just a cosmetic plotting variation. It changes the core analysis observable and therefore touches the central analysis logic directly. Keeping that change in a separate dated study folder is the lowest-risk way to:

1. avoid accidental impact on frozen note production,
2. preserve the exact maintained 20241102 scripts and tags,
3. keep the test outputs easy to compare side-by-side against the frozen baseline,
4. allow the study to evolve without polluting the official path with temporary flags or half-maintained branches.

Using the existing `MainAnalysis/20260216_temp/` area is also **not** the best fit. That folder is already serving a different temporary-study purpose. Reusing it would mix unrelated diagnostics and make later auditing harder.

## Suggested folder/script structure

Start from a copy of the maintained 20241102 workflow, but only keep the pieces needed for a controlled central-value study.

Suggested contents of `MainAnalysis/20260511_PionTrackYStudy/`:

- `CorrelationAnalysis.cpp`
- `makeProjection.C`
- `include/`
- `makefile`
- `threader.sh`
- `system-analysis.sh`
- `clean.sh`
- a small dedicated runner such as:
  - `run-central-pion-tracky.sh`
  - or `run-check.sh`

## Suggested code strategy

Inside this new study folder:

1. Leave the maintained 20241102 code untouched.
2. In the copied `CorrelationAnalysis.cpp`, add a **study-local** helper that computes track rapidity from:
   - measured track `pT`,
   - measured track `eta`,
   - assumed pion mass.
3. Use that derived track rapidity only in the study folder when filling the Z-track longitudinal-difference observable.
4. Keep the result histogram names and output tags clearly study-specific so they cannot be confused with frozen products.

Concretely, the study folder should produce outputs with a dedicated suffix such as:

- `_pionTrackYTest`

or a dedicated study tag embedded in the output basename.

## Suggested run scope

Since this is explicitly a **check** and not a production replacement, do not start by cloning the full official production surface.

Best first scope:

1. inclusive central-value outputs only,
2. one or two representative scan bins if the inclusive result looks interesting,
3. no systematic rerun at first,
4. no note or Overleaf updates,
5. no reuse of official tags or frozen output basenames.

That means the first dedicated runner should be narrow, for example:

- inclusive `ZPT0_500`, `trkPT0.5_15`
- optionally `ZPT30_500`, `trkPT4_15` as a high-`pT` stress test

## Output hygiene

Within the study folder, keep outputs isolated:

- ROOT outputs in `output/`
- PDFs in `plots/`

and preferably under a study-specific subdirectory or basename, for example:

- `output/pionTrackY/`
- `plots/pionTrackY/`

This makes cleanup and comparison with the frozen baseline straightforward.

## Why not modify the maintained 20241102 workflow directly

Doing this test in the maintained folder would force one of two bad options:

1. temporarily hard-code the altered observable into official code, or
2. add a study-only runtime flag into the maintained production path.

Both are worse than a dated copy for this use case. They increase the chance that a temporary physics test leaks into future official reruns, especially because this test changes the definition of the plotted observable itself rather than only adding a harmless side diagnostic.

## Recommended execution sequence

1. Create `MainAnalysis/20260511_PionTrackYStudy/` from the maintained 20241102 analysis snapshot.
2. Modify only the copied study code there.
3. Add one narrow central runner for inclusive output.
4. Compare the study output directly against the frozen 20241102 baseline.
5. Only if the effect is significant, extend the study to a few scan bins.

## Bottom line

**Best method:** make a **new dated analysis-study folder** under `MainAnalysis/` and test the pion-mass track-rapidity idea there.

**Not recommended:** modifying the current maintained 20241102 workflow in place, or overloading the existing `20260216_temp` workspace for this unrelated physics test.
