# Track-to-PV Vertex Association Study Results

**Date**: 2026-05-27  
**Code**: `MainAnalysis/20260216_temp/vtx_assoc_study.cpp`  
**Forest source**: `SampleGeneration/20250929_ReducedTreePA/Samples/{PAData,PPData}/`  
**Files used**: 19 forest files per system  

---

## Study design

**Question**: For tracks in pPb and pp data events passing our event selection, what are the
false-positive and false-negative rates when using the analysis 3σ IP significance cut
(`|trkDxy1/trkDxyError1| < 3` AND `|trkDz1/trkDzError1| < 3`) to assign tracks to the
primary vertex?

**Truth definition**: `|trkDxyOverDxyError[it*nVtx+0]| < 3σ` AND `|trkDzOverDzError[it*nVtx+0]| < 3σ`
— i.e., the unrounded per-vertex IP significance at vertex index 0, which is what
`trkAssocVtx[it*nVtx+0]` was supposed to encode before the TrackAnalyzer.cc off-by-one bug
zeroed every entry (see `dxy_dz_vertex_reference.md`).

**Predictor**: `|trkDxy1/trkDxyError1| < 3σ` AND `|trkDz1/trkDzError1| < 3σ`, where
`trkDxy1`/`trkDz1` are computed relative to `maxPtVtx` and rounded to 4 significant figures.

**Event selection**:
- Both systems: `pPAprimaryVertexFilter && pBeamScrapingFilter`
- pPb: `HLT_PAL2Mu12_v1 && HLT_PAL3Mu12_v1`
- pp: `HLT_HIL2Mu12_v1 && HLT_HIL3Mu12_v1`

---

## pPb results (19 PAData forest files, 62k events total, 19k passing selection)

**Overall** (3.34M tracks):

| Metric | Value |
|--------|-------|
| Truth+ (PV tracks) | 2,490,735 (74.6%) |
| Truth− (non-PV tracks) | 846,802 (25.4%) |
| **FPR** = FP/N | **4.54%** |
| **FNR** = FN/P | **0.85%** |
| Precision | 98.47% |
| Recall | 99.15% |

**Breakdown by nVtx**:

| nVtx | FPR | FNR | Comment |
|------|-----|-----|---------|
| 1 | 0.02% | 0.01% | Rounding-only effect; negligible |
| 2 | 7.98% | 2.16% | Single pileup vertex |
| ≥ 3 | 11.88% | 5.82% | Multi-vertex pileup |

---

## pp results (19 PPData forest files, 366k events total, 207k passing selection)

**Overall** (25M tracks):

| Metric | Value |
|--------|-------|
| Truth+ (PV tracks) | 11,059,847 (44.2%) |
| Truth− (non-PV tracks) | 13,972,523 (55.8%) |
| **FPR** = FP/N | **4.61%** |
| **FNR** = FN/P | **3.02%** |
| Precision | 94.33% |
| Recall | 96.98% |

**Breakdown by nVtx**:

| nVtx | FPR | FNR | Comment |
|------|-----|-----|---------|
| 1 | 0.02% | 0.01% | Rounding-only effect; negligible |
| 2 | 3.61% | 1.30% | Single pileup vertex |
| ≥ 3 | 4.88% | 4.03% | Multi-vertex pileup; dominates pp sample |

---

## Key conclusions

### 1. The 3σ cut is essentially exact for single-vertex events

In both systems, nVtx = 1 gives FPR ≈ FNR ≈ 0.01–0.02%. The only source of discrepancy
is the 4-significant-figure rounding applied to `trkDxy1`/`trkDz1` when they are stored,
vs. the unrounded significance in `trkDxyOverDxyError`. This is a sub-percent effect and
operationally irrelevant.

### 2. All meaningful FP/FN come from pileup vertices

The 4–5% overall FPR in both systems is driven entirely by multi-vertex events. In events
with nVtx ≥ 2, `maxPtVtx` (which `trkDxy1/trkDz1` are computed relative to) may differ
from vertex index 0 (which the truth check uses), so the IP value the cut sees is relative
to a different vertex than the truth check is testing. This is not a failure of the 3σ
criterion — it is a consequence of the vertex index mismatch between the rounded IP branches
and the unrounded per-vertex arrays.

### 3. pp has substantially higher FNR than pPb in multi-vertex events

- pPb nVtx ≥ 3: FNR = 5.8%, FPR = 11.9%
- pp  nVtx ≥ 3: FNR = 4.0%, FPR = 4.9%

The pp sample has many more multi-vertex events (truth− is 55.8% of pp tracks vs 25.4% for
pPb), so the nVtx ≥ 3 bin dominates the pp overall rates. The per-category rates are
actually lower in pp than pPb, consistent with lower average pileup per vertex in 5 TeV pp
vs 8.16 TeV pPb. The higher FNR overall for pp (3.0% vs 0.85%) reflects the larger fraction
of events with multiple vertices in the 2017 pp dataset.

### 4. Implication for the same-vertex study

A 3σ IP cut on `trkDxy1/trkDz1` is a reliable proxy for PV membership in single-vertex
events (> 99.98% correct for both systems). In pileup events the cut introduces ~4–12% FPR
and ~1–6% FNR per-category, but the overall analysis impact is mitigated by the fact that
the PU-reject selection (`NVertex == 1` in data) selects exactly the regime where the cut
is exact. For the main analysis without PU rejection, the error rate is bounded by the
pileup fraction of the sample.

---

## Caveats

- "Truth" here is itself an IP significance threshold (unrounded, at vertex index 0), not
  a generator-level particle origin. It tests whether the cut is self-consistent across
  the two representations (rounded vs. unrounded, maxPtVtx vs. vertex-0 indexing), not
  whether tracks are physically from the primary interaction.
- The vertex-index mismatch (maxPtVtx ≠ 0) is the dominant driver of disagreement in
  multi-vertex events. In practice for Z→μμ events, `maxPtVtx` is almost always vertex 0
  since the Z carries the highest ΣpT, so the mismatch is rare but nonzero.
- Study uses first 19 files per system; numbers are stable but not final-statistics.
