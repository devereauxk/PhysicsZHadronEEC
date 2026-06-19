# Jewel pp 5.02 TeV Skim Comparison Report

## Signed convention, inclusive ZPT 0–500 GeV, trkPT 0.5–15 GeV

**Date**: 2026-06-09

## Samples

| Property | Local production | v9 (PbPb study) |
|----------|-----------------|-----------------|
| File | `SampleGeneration/20260521_Jewel/output/jewel_pp_5020.root` | `mergedSample/jewel_pp-v9.root` |
| Owner | kdeverea | yjlee |
| Events | 1,417,320 | 2,000,000 |
| **Jewel version** | **2.4.0** (`~/Jewel/jewel-2.4.0/jewel-2.4.0-vac`) | **2.2.0** (from `RunAll.sh`: `Input/jewel/jewel-2.2.0/eventfilesZJet/`) |
| Converter | Our `ConvertHepMC.cpp` (pT>0.5, \|η\|<2.4 pre-cut) | yjlee's `ConvertHepMC.cpp` at `/data/yjlee/.../20240607_PredictionConversion/` (no pre-cuts, weight-based tagging) |
| Process | PPZJ vacuum | PPZJ vacuum |
| Energy | 5.02 TeV | 5.02 TeV |
| Weighted N_Z (signed acceptance) | 0.00268 | 0.00390 |

## Analysis configuration (signed convention)

Both samples processed with identical flags:
```
--FillSigned true --DEtaRange 3.87
--ResultDEtaBins 12 --ResultDPhiBins 12
--TrackEtaMin -1.935 --TrackEtaMax 1.935
--ZYSignedMin -1.935 --ZYSignedMax 1.935
--MaxMixDeltaVZ 1.0 --nMix 10
--IsJewel true --IsGenZ true --UseEventWeight true
```

## Results: significant differences observed

### DeltaEta
The v9 sample produces systematically deeper (more negative) DeltaEta correlation
values across all 12 bins. The difference is ~0.03–0.07 in the central bins
(|Δy| < 2) and smaller at the edges. The lower panel of `pp_skims-DeltaEta.pdf`
shows a nearly flat offset of ~−0.05.

### DeltaPhi
The v9 away-side peak (Δφ ≈ π) is ~30% larger than local (0.79 vs 0.61).
Near-side bins are also more negative in v9 by ~0.05–0.10. The difference panel
shows the largest deviations at the away-side (Δφ ∈ [2, 4]), reaching +0.15.

## Root cause analysis

### 1. Z kinematics agree
Both samples produce the same Z physics:
- Unweighted mean Z pT: local = 227 GeV, v9 = 225 GeV
- Weighted mean Z pT: local = 31.9 GeV, v9 = 31.7 GeV
- Z rapidity distributions: consistent shape
- Z yield per event: ~65–66% in both

### 2. Track multiplicity per Z event differs by 1.87×
Within the signed acceptance (|η| < 1.935, pT ∈ [0.5, 15] GeV, weight > 0):
- Local: **20.8 tracks/Z event**
- v9: **38.9 tracks/Z event**

This is the dominant source of all observed differences.

### 3. Track pT and η spectra have different shapes
- **pT**: Local has ~19% more tracks at low pT (0.5–1 GeV) per unit area;
  v9 has ~18% more at intermediate pT (3–8 GeV). v9 mean pT = 3.04 GeV
  vs local 2.79 GeV.
- **η**: v9 is more centrally concentrated. Normalized shape ratio
  (local/v9) ranges from 1.27 at |η| = 1.8 to 0.90 at η = 0.

### 4. Particle composition is the same
Both samples contain only charged pions (±211), kaons (±321), muons (±13),
and electrons (±11). No neutral particles. The higher v9 multiplicity comes
from ~1.8× more pions and ~1.6× more kaons per Z event.

### 5. Converter differences (secondary)
| Feature | Local (`ConvertHepMC.cpp`) | v9 (`/data/yjlee/.../ConvertHepMC.cpp`) |
|---------|-------|-----|
| `trackMuDR` branch | Present (real ΔR float) | **Missing** (`push_back(false)` — boolean, not DR) |
| Track pre-cut | pT > 0.5, \|η\| < 2.4 | None (stores all particles, tags via weight) |
| Neutral handling | Not stored | Stored with weight=0 via `ZeroNeutrals()` |
| `hiBin` | 0 (set explicitly) | 32766 (uninitialized) |
| Track η range in file | ±2.4 | ±5 |
| Tracks with weight=0 | None | ~39% of all tracks |
| `NegativeID` | Not used | `--NegativeID 3` (status-3 particles get weight=-1) |

These converter differences do not explain the multiplicity discrepancy, since
the analysis applies its own acceptance cuts regardless of what the converter
stored.

## Conclusion

The two pp Jewel samples at 5.02 TeV produce the same Z boson physics
(matching pT, rapidity, and cross-section weight distributions) but have
**fundamentally different underlying event / fragmentation models**. The v9
sample (from yjlee's PbPb analysis) generates 1.87× more charged hadrons per
Z event within the same acceptance window, with a harder pT spectrum and more
centrally concentrated η distribution.

**Root cause confirmed**: the v9 sample was generated with **Jewel 2.2.0**
(found in `/data/yjlee/PhysicsZHadronEEC/SampleGeneration/20240607_PredictionConversion/RunAll.sh`,
which references `Input/jewel/jewel-2.2.0/eventfilesZJet/`), while our local
production uses **Jewel 2.4.0** (`~/Jewel/jewel-2.4.0/jewel-2.4.0-vac`).

The Jewel 2.2.0 → 2.4.0 changelog includes changes to fragmentation,
underlying event modeling, and hadronization parameters that explain the 1.87×
track multiplicity difference and the harder pT spectrum in v9.

The original Jewel 2.2.0 installation and HepMC input files are no longer
available — only the merged ROOT skims survive. No Jewel generation code was
found in `/data/chenyi/`.

**The two samples should not be treated as interchangeable.** Any Jewel
comparison requiring a pp baseline should use a consistently generated sample
from the same Jewel installation and parameter set (our Jewel 2.4.0).

## Output files

Analysis ROOT files:
- `MainAnalysis/20241102_ZhadronVsZPt/plots/jewelPP5020signed_ZPT0_500-{result,nosub}.root`
- `MainAnalysis/20241102_ZhadronVsZPt/plots/jewelPPv9signed_ZPT0_500-{result,nosub}.root`

Comparison plots (this directory):
- `pp_skims-DeltaEta.pdf`, `pp_skims-DeltaPhi.pdf`
- `pp_skims-ZPt.pdf`, `pp_skims-ZY.pdf`, `pp_skims-ZPhi.pdf`
- `pp_skims-TrkPt.pdf`, `pp_skims-TrkEta.pdf`
