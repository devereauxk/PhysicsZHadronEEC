# Analyzer Directions: Regenerate pPb/PbP Overlay Plots for Overleaf Combining Section

## Objective
Regenerate the **separated pPb and PbP overlay plots** used in the Overleaf **“Combining pPb and Pbp data”** section, using the **current in-place pPb weight configuration** (same tag family as current result plots), overwrite existing overlay outputs, and push updated files to Overleaf.

## Scope and tag
- Use current tag family: `ZV5_trkV23_nmix10`.
- Use the same inputs underlying current result production (Part A current-weight setup).
- This task is only for the combining overlay plots (not central pp-vs-pPb result plots).

## Production requirements
1. Rebuild and run the dedicated overlay plotting workflow:
   - `Plots/20260213_Central/plot-central-overlay-PPbPbP.sh`
2. Keep script-driven flow (no arbitrary one-off production command chains).
3. It is OK to overwrite existing files with same tag.

## Expected analysis output location
The regenerated files should be under:
- `Plots/20260213_Central/plots/central_overlay_PPbPbP/`

Expected filenames (10 total):
- `ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaEta-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT5_500_trkPT0.5_500-DeltaPhi-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaEta-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT5_30_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaEta-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT5_30_trkPT4_500-DeltaPhi-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaEta-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT30_500_trkPT0.5_4-DeltaPhi-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaEta-overlay.pdf`
- `ZV5_trkV23_nmix10_ZPT30_500_trkPT4_500-DeltaPhi-overlay.pdf`

## Overleaf push requirements
Copy these PDFs into:
- `~/OverleafZHadronInPPb/figures/analysis/combining/`

Use same filenames (in-place replacement) so `src/analysis.tex` references remain unchanged.

## Validation to report
- Exact commands run (ordered).
- List of regenerated files in analysis repo with timestamps.
- List of copied files in Overleaf with timestamps.
- Confirmation that these match `src/analysis.tex` combining-section includes.
- Note any failures/retries and resolutions.
