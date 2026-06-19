# pPb/Pbp Compatibility Studies — Session Plan (2026-05-29)

## Working point

All studies share:
- Full correction stack: Event + VZ + Z + track + residual weights
- 12×12 result binning, `0 < pT^Z < 500 GeV`, `0.5 < pT^trk < 15 GeV`
- `nMix = 10`, 8.16 TeV pPb, `|vz| < 15 cm` (unless study varies it)
- **`MaxMixDeltaVZ = 0.5 cm`** (applied uniformly, confirmed in this session)
- Jackknife statistical uncertainties on corrected-data legs

Official tag: `OFFICIAL_TAG_PPB` from `OfficialWeightDictionary.sh`.

---

## DeltaPhi display range convention

- **All studies except Study 3**: DeltaPhi plots shown on `[0, π]`.
- **Study 3 (signed)**: DeltaPhi shown on full signed range `[-π, π]`. DeltaEta also shown on `[-4, 4]` (already handled by `--ShowSignedEta`).

Plotter flag: add `--ShowFullPhi {true|false}` (default false → `[0, π]`; true → `[-π, π]` for Study 3).

For SE/ME projections: when `ShowFullPhi=false`, project Y over all bins but display x-range `[0, π]` (ROOT clips the visible range). When `ShowFullPhi=true`, display `[-π, π]`.

---

## Current status (end of 2026-05-28 session)

All 4 original studies ran successfully with `MaxMixDeltaVZ = 0.5`. Output files:

| Study | Output prefix (in `20241102_ZhadronVsZPt/plots/`) |
|-------|---------------------------------------------------|
| 1 halfsplit | `pPb/PbP_trkResidual_${TAG}_12x12_{full,pPbFirst,pPbSecond,PbPFirst,PbPSecond}` |
| 2 vz10 | `pPb/PbP_trkResidual_${TAG}_12x12_vz10` |
| 3 signed | `pPb/PbP_trkResidual_${TAG}_12x12_signed` (phi-fold fix running now) |
| 4 MC matched | `pPbMC/PbPMC_trkResidual_${TAG}_12x12_matched` |

Plotter: `ExecutePlot12x12PPbPbP` (from `plot_12x12_ppbpbp.cpp`).
Presentation: `presentations/pPbPbp_12x12_compatibility_presentation.pdf`.

---

## Pending immediate task (on wakeup)

- Runner `runner-12x12-signed.sh` (phi-fold fix, 1 fill per track) is running (PID 2847470).
- On wakeup: check if done → re-plot Study 3 → replot all studies → recompile presentation.
- Update DeltaPhi range to `[0, π]` for all studies except Study 3.

---

## New studies to implement (after wakeup task)

### Study 5: ΔVZ mixing requirement sensitivity

**Physics goal**: Check stability of result with respect to the |ΔVZ| cut on mixed events.

**Analysis runs needed** (new):

| Prefix | MaxMixDeltaVZ | Notes |
|--------|--------------|-------|
| `pPb_trkResidual_${TAG}_12x12_dvzOff` | 0 (disabled) | No ΔVZ requirement |
| `PbP_trkResidual_${TAG}_12x12_dvzOff` | 0 (disabled) | |
| `pPb_trkResidual_${TAG}_12x12_dvz1` | 1.0 | |
| `PbP_trkResidual_${TAG}_12x12_dvz1` | 1.0 | |

**Reuse** for ΔVZ<0.5 nominal: `_12x12_full` files (already produced).

**Plot**: 6 lines per panel:
- Pair 1 (solid): pPb + Pbp, ΔVZ<0.5 (nominal)
- Pair 2 (long-dash): pPb + Pbp, ΔVZ off
- Pair 3 (dotted): pPb + Pbp, ΔVZ<1.0

Colors: pPb = cmsBlue, Pbp = kSpring-6 (same across all pairs).
DeltaPhi range: `[0, π]`.
Lower panel: 3 ratio (SE/ME) or 3 difference (result) curves, one per ΔVZ setting.

**Runner**: `runner-12x12-deltavz.sh`
**Plot script**: `plot-12x12-deltavz.sh`
**Output**: `plots/12x12_deltavz/`

---

### Study 6: MC reco full stats pPb vs Pbp

**Physics goal**: Compare pPb and Pbp at full MC reco statistics (no Z-count matching), cross-check of Study 4.

**Analysis runs needed** (new):

| Prefix | Input | Notes |
|--------|-------|-------|
| `pPbMC_trkResidual_${TAG}_12x12_fullstats` | `OFFICIAL_MCRECOINPUT_PPB` | No `--Fraction` |
| `PbPMC_trkResidual_${TAG}_12x12_fullstats` | `OFFICIAL_MCRECOINPUT_PBP` | No `--Fraction` |

**Plot**: Standard 2-line plot (pPb MC reco vs Pbp MC reco), same style as Study 4.
DeltaPhi range: `[0, π]`.

**Runner**: `runner-12x12-mc-fullstats.sh`
**Plot script**: `plot-12x12-mc-fullstats.sh`
**Output**: `plots/12x12_mc_fullstats/`

---

### Study 7: Raw uncorrected data pPb vs Pbp

**Physics goal**: Compare pPb and Pbp with all corrections turned off — baseline cross-check of raw-level agreement.

**Analysis runs needed** (new):

| Prefix | Corrections | Notes |
|--------|-------------|-------|
| `pPb_trkResidual_${TAG}_12x12_raw` | All off | `--UseEventWeight false --UseZWeight false --UseTrackWeight false --UseResidualWeight false --UseVZWeight false` |
| `PbP_trkResidual_${TAG}_12x12_raw` | All off | Same |

Keep `--UseJackknife true` for statistical errors. Keep `MaxMixDeltaVZ 0.5`.

**Plot**: Standard 2-line plot (pPb raw vs Pbp raw).
DeltaPhi range: `[0, π]`.

**Runner**: `runner-12x12-raw.sh`
**Plot script**: `plot-12x12-raw.sh`
**Output**: `plots/12x12_raw/`

---

### Study 2 (modification): Add |vz|<15 overlay

**Physics goal**: Show VZ<10 alongside nominal VZ<15 to quantify sensitivity.

**No new runs needed**: VZ<15 comes from `_12x12_full` (already produced).

**Plot**: 4 lines per panel:
- Pair 1 (solid): pPb + Pbp, |vz|<10
- Pair 2 (dashed): pPb + Pbp, |vz|<15 (reuse `_12x12_full`)

DeltaPhi range: `[0, π]`.
Lower panel: 2 ratio/diff curves (one per VZ window).

Modified plot script: `plot-12x12-vz10.sh` updated to pass second file pair.

---

## Plotter changes required

### 1. Add `--ShowFullPhi` flag

Default `false` → DeltaPhi display range `[0, π]`.
When `true` (Study 3) → DeltaPhi display range `[-π, π]`.

### 2. Multi-pair mode (Studies 2, 5)

Add optional CLI flags:
```
--pPbFile2 <path>    (optional second pPb result file)
--PbPFile2 <path>
--pPbLabel2 <str>
--PbPLabel2 <str>
--pPbFile3 <path>    (optional third pPb result file)
--PbPFile3 <path>
--pPbLabel3 <str>
--PbPLabel3 <str>
```

When `pPbFile2` present → 4-line mode (Study 2).
When `pPbFile2` + `pPbFile3` present → 6-line mode (Study 5).

Line styles per pair: pair 1 = `kSolid`, pair 2 = `kDashed`, pair 3 = `kDotted`.

Lower panel in multi-pair mode: pre-compute per-pair ratio/diff histograms (clone + `TH1::Divide` or subtract), draw as separate curves with matching styles.

---

## Presentation update

Title: "pPb/Pbp compatibility studies (12×12)"
Subtitle: 7 studies.

Updated overview table:

| Study | Description | Slides |
|-------|-------------|--------|
| 1 | Run half-splits (4 comparisons) | ref |
| 2 | \|vz\|<10 cm with \|vz\|<15 overlay (4 lines) | ref |
| 3 | Signed Δη and Δφ (no folding, full φ range) | ref |
| 4 | MC reco Z-count-matched to data | ref |
| 5 | ΔVZ sensitivity: <0.5 vs off vs <1.0 (6 lines) | ref |
| 6 | MC reco full stats pPb vs Pbp | ref |
| 7 | Raw uncorrected data pPb vs Pbp | ref |

Each study: 2 slides (ΔEta row + ΔPhi row, 3 panels each: SE / ME / Result).

---

## File checklist

New scripts to create:
- [ ] `runner-12x12-deltavz.sh` (4 runs: pPb/Pbp × dvzOff/dvz1)
- [ ] `runner-12x12-mc-fullstats.sh` (2 runs)
- [ ] `runner-12x12-raw.sh` (2 runs)
- [ ] `plot-12x12-deltavz.sh`
- [ ] `plot-12x12-mc-fullstats.sh`
- [ ] `plot-12x12-raw.sh`

Scripts to modify:
- [ ] `plot_12x12_ppbpbp.cpp` — add `--ShowFullPhi`, multi-pair mode
- [ ] `plot-12x12-vz10.sh` — pass second file pair (VZ<15 = `_12x12_full`)
- [ ] `plot-12x12-signed.sh` — pass `--ShowFullPhi true`
- [ ] `presentations/pPbPbp_12x12_compatibility_presentation.tex` — 7 studies, updated overview, ΔPhi `[0,π]` for all but Study 3

---

## Execution order

1. **[Immediate, on wakeup]** Check signed runner → rebuild plotter with `--ShowFullPhi` → re-plot all 4 studies with corrected ΔPhi range → recompile 4-study presentation.
2. Extend plotter to multi-pair mode; rebuild.
3. Create + run `runner-12x12-deltavz.sh` (4 runs) and `runner-12x12-mc-fullstats.sh` (2 runs) and `runner-12x12-raw.sh` (2 runs) — launch in parallel.
4. While running: create all new plot scripts; update `plot-12x12-vz10.sh` and `plot-12x12-signed.sh`.
5. When runs finish: plot all new studies.
6. Update and recompile presentation with all 7 studies.
