# Same-Vertex Study Plan: Track–Z Vertex Association

**Date:** 2026-05-24  
**Status:** For review — no implementation changes made

---

## Motivation

The current PU reject systematic variation (`isPUReject`) requires exactly one reconstructed vertex (`NVertex == 1`). This removes events with ≥2 reconstructed PVs, eliminating events where a second hard-scatter or pile-up interaction produces a second vertex cluster. However, it is **not** equivalent to requiring that all tracks and the Z originate from the same primary vertex:

- `NVertex == 1` is an **event-level** filter. Even in a single-vertex event, tracks from secondary hadronic decays (K⁰ₛ → π⁺π⁻, Λ → pπ, photon conversions, etc.) may have `trkAssocVtx == false` — they were not used in building the PV during vertex fitting — yet still pass the IP significance cut (`< 3σ`) if their displacement happens to be small or collinear with the PV.
- The **actual per-track gate** enforcing geometric PV compatibility is the IP significance cut, not `NVertex == 1`.

In other words, `NVertex == 1` enriches the sample with Z and tracks from the same hard-scatter vertex, but does not guarantee it track by track. There will still be secondary-decay products in the passing sample with `trkAssocVtx == false`.

The proposed same-vertex variation is a **track-level** filter: allow multiple vertices, but require each accepted track to have `trkAssocVtx == true` (associated with the best PV) and the Z muons to be compatible with the best PV. This retains the full multi-vertex event sample while vetoing only tracks assigned to PU vertices — a less statistically costly and more targeted cut than `NVertex == 1`.

---

## Technical Audit Summary

### What exists at the forest level (pp/PA format)

| Variable | Forest branch | Per-track? | Notes |
|---|---|---|---|
| Track dxy w.r.t. best PV | `trkDxy1` | Yes | Significance cut `< 3σ` applied at skim time |
| Track dz w.r.t. best PV | `trkDz1` | Yes | Significance cut `< 3σ` applied at skim time |
| dxy error | `trkDxyError1` | Yes | Used for significance denominator |
| dz error | `trkDzError1` | Yes | Used for significance denominator |
| Boolean PV association | `trkAssocVtx` | Yes | True if track is associated with the best PV; NOT an index |
| Number of associated vertices | `trkNVtx` | Yes (`short`) | Number of vertices the track is associated with |
| Muon dxy w.r.t. best PV | `DiDxy1`, `DiDxy2` | Per-muon | Absolute cut `< 0.2 cm` applied at skim time |
| Muon dz w.r.t. best PV | `DiDz1`, `DiDz2` | Per-muon | Absolute cut `< 0.5 cm` applied at skim time |

**`trkAssocVtx` is a flat 2D array, not a per-track boolean.** The CMSSW `TrackAnalyzer` fills it as:
```cpp
pev_.trkAssocVtx[trkCount * pev_.nVtx + i] = true/false;
```
where `trkCount` is the track index and `i` is the vertex index. The layout is track-major, vertex-minor. The total flat array size is `nTrk * nVtx`, stored in the companion branch `nTrkTimesnVtx`. In `TrackTreeMessenger` the array is declared as `bool trkAssocVtx[TRACKCOUNTMAX]` (size 10000), large enough for all `nTrk * nVtx` entries.

**The vertex index is therefore fully recoverable.** `nVtx` and `GetBestVertexIndex()` are both already present in `TrackTreeMessenger`. At skim time, the correct per-track best-PV association is:
```cpp
int bestVtxIdx = MTrk.GetBestVertexIndex();
bool onBestVtx = MTrk.trkAssocVtx[iTrack * MTrk.nVtx + bestVtxIdx];
```

**`trkAssocVtx` is currently read but never used.** `PassZHadron2022Cut` does not check it — only the IP significance cuts (`|trkDxy1|/trkDxyError1 < 3`, `|trkDz1|/trkDzError1 < 3` w.r.t. the best PV) are applied. The vertex-association boolean is available at forest level but discarded at skim time.

The available per-track vertex-association branches in `TrackTreeMessenger`:
- `trkAssocVtx[t * nVtx + v]` — is track `t` associated with vertex `v`?
- `trkNVtx` (short) — number of vertices track `t` is associated with
- `trkDxy1` / `trkDz1` / errors — IP w.r.t. best PV (significance already used in cuts)
- `nVtx` (int) — number of reconstructed vertices (needed for correct indexing)
- `nTrkTimesnVtx` (int) — total flat array size = `nTrk * nVtx`

### What exists at the skim level (V0.2 / V0.3)

The `ReduceForest.cpp` skimmer (`SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp`) writes:

- `NVertex` — event-level count of all reconstructed vertices  
- `VX`, `VY`, `VZ` — position of the single best PV (by max `sumPtVtx`)  
- `VXError`, `VYError`, `VZError` — uncertainty on the best PV

**Nothing per-track is written**: `trkDxy1`, `trkDz1`, their errors, `trkAssocVtx`, and `trkNVtx` are all consumed inside `PassZHadron2022Cut()` at skim time and then discarded. The `DiDxy1/2`, `DiDz1/2` muon impact parameters are read from forest but not written to skim.

**The `ZHadronMessenger` output tree** has no per-track IP or vertex-index field — this is confirmed in `CommonCode/include/Messenger.h`.

### Existing IP-based protection already in place

The three track selection modes already provide graded PV compatibility via dxy/dz significance w.r.t. the best PV:

| Mode | dxy significance | dz significance | pT uncertainty |
|---|---|---|---|
| Nominal | `< 3σ` | `< 3σ` | `< 10%` |
| Loose | `< 5σ` | `< 5σ` | `< 10%` |
| Tight | `< 2σ` | `< 2σ` | `< 5%` |

The nominal cut already rejects tracks displaced >3σ from the best PV. The tight cut tightens this further to 2σ. Neither is an explicit same-vertex requirement, but both preferentially select tracks compatible with the best PV.

**Muon quality cut** (`PassZHadronMuon`): `|DiDxy| < 0.2 cm`, `|DiDz| < 0.5 cm` absolute — not significance-based. These are looser than the track cuts in terms of PV compatibility enforcement.

---

## Feasibility Pathways

### Path A — Use tight track selection as a proxy (no skim changes needed)

**Concept:** Treat the existing `TrackSelectionMode=Tight` (sig < 2) as the closest available proxy for same-vertex track selection. For events with multiple vertices, tracks displaced from the best PV by >2σ are already rejected. Compare nominal vs. tight in the multi-vertex (`NVertex > 1`) subsample to isolate the PU-contamination effect.

**Effort:** Zero — uses existing infrastructure. Runners already exist in `MainAnalysis/20241102_ZhadronVsZPt/systematics.sh` and `MainAnalysis/20251211_ResidualCorrection/workflow/run-pp-systematics.sh`.

**Limitations:**
- Tight is not an exact same-vertex cut; it is a significance gate on the single best PV
- Cannot distinguish "track from a different reconstructed vertex" from "track genuinely displaced from the best PV"
- The tight selection already appears in the track-selection systematic family; reusing it as a PU proxy would conflate two systematics
- Muon cuts remain absolute-threshold (not significance-based), so the Z candidate vertex definition is not symmetrically tightened

**Verdict:** Viable as a cross-check or upper-bound estimate of the PU effect, but not a clean same-vertex measurement. Does not require skim changes.

---

### Path B — Skim reprocessing to add per-track IP and vertex-association branches

**Concept:** Modify `ReduceForest.cpp` to additionally write, for each accepted track:
- `trkDxy1`, `trkDz1` (float, w.r.t. best PV)
- `trkDxyError1`, `trkDzError1` (float)
- `trkAssocVtx` (bool — associated with best PV?)
- Optionally `trkNVtx` (short — number of vertices the track is associated with)

And for each muon:
- `DiDxy1`, `DiDxy2`, `DiDz1`, `DiDz2`

The same-vertex flag is defined at skim time using the 2D `trkAssocVtx` structure: a track is "on the best PV" if `trkAssocVtx[iTrack * nVtx + bestVtxIdx] == true`.

**Effort:** Moderate — simpler than originally estimated.

The only new skim branch needed is one `bool onBestVtx` per track, computed in `ReduceForest.cpp` as:
```cpp
int bestVtxIdx = MTrack.GetBestVertexIndex();
bool onBestVtx = MTrack.trkAssocVtx[iTrack * MTrack.nVtx + bestVtxIdx];
```
No new forest branches are required. `nVtx` and `trkAssocVtx` are already read into `TrackTreeMessenger`; `GetBestVertexIndex()` is already implemented.

Steps:
1. Add `bool trkOnBestVtx[NTRK]` to `ZHadronMessenger` in `CommonCode/include/Messenger.h`.
2. Populate it in `ReduceForest.cpp` using the 2D indexing above.
3. Add `--SameVertexMode` CLI flag to `CorrelationAnalysis.cpp`; when enabled, reject tracks where `trkOnBestVtx == false`.
4. Re-skim all pp / pPb / PbP samples (V0.2/V0.3).
5. Reproduce residual corrections and run the full analysis stack.

**Limitations:**
- The Z will nearly always be on the best PV (dimuon pT dominates `sumPtVtx`), so requiring `trkOnBestVtx == true` is effectively "track on the same PV as the Z." In the rare event where the Z is not on the best PV, this cut would be inverted in meaning — but such events are negligible.
- Muon-side vertex association is not separately stored (muons use absolute IP cuts, not `trkAssocVtx`). The implicit assumption is that the Z selects the best PV.
- Full systematic reprocessing required before producing a note-facing result.

**Verdict:** Cleanest implementation and more achievable than initially assessed. The 2D structure of `trkAssocVtx` in the forest provides exact track-to-vertex association — no approximation. One new bool branch per track at skim time; no new forest reads required.

---

### Path C — Forest re-reading in a standalone module

**Concept:** Write a new standalone analysis that reads the original forest ROOT files directly (not through the skim), applies the same Z and track selections, and evaluates the same-vertex condition with access to all forest-level vertex information. This is essentially re-implementing the skim selection inside an analysis macro.

**Effort:** High.
- Requires access to forest files, which live on `/eos/` and require `kinit -5`
- Forest files are large and not kept locally; re-reading them for a systematic study is operationally expensive
- Adds a new analysis path outside the validated skim framework, making cross-checks against nominal harder
- No existing infrastructure in this repo for direct forest analysis (all workflows use skims)

**Verdict:** Not recommended for a systematic variation that would appear in the note. More appropriate for a one-time feasibility check if specific forest-level questions arise.

---

## Key Open Questions for the User

1. **Exact vertex-index matching is available.** *(Resolved — no longer a question.)* `trkAssocVtx` in the forest is a flat 2D array indexed as `[track * nVtx + vtxIdx]`, so `trkAssocVtx[iTrack * nVtx + GetBestVertexIndex()]` gives the exact association of a track with the best PV. A strict vertex-index match between track and Z is achievable at skim time with no new forest branches.

2. **Should the muon vertex requirement be added symmetrically?**  
   The Z vertex is implicitly defined by the best PV (chosen largely by the dimuon system). Requiring explicit muon-to-best-PV dxy/dz significance cuts (rather than the current absolute cut) would make the requirement symmetric but is a meaningful change to the muon selection.

3. **Is Path A (tight selection comparison) sufficient to bound the effect?**  
   If the goal is a conservative upper bound on the PU contribution before committing to a skim reprocess, Path A produces something today. If the conclusion from Path A is that the effect is negligible, no skim changes may be needed.

4. **Is this intended to replace or supplement `isPUReject`?**  
   - *Replace*: the same-vertex cut should be derived for data and for MC, and the MC closure (gen-level has no PU) should be verified. The residual corrections would need to be derived with the same-vertex cut applied.
   - *Supplement*: run both and take the envelope; no change to residual derivation needed.

5. **Which collision systems are priority?**  
   The PU systematic is `PUpp` (pp) and `PUpPb` (pPb/PbP) in `Systematics/20260329_pPbSystematics/`. If one system dominates the uncertainty, it may not be worth reprocessing all three.

---

## Recommendation

**Near-term (no skim changes):** Run Path A — produce a narrow diagnostic comparison of `nominal` vs. `NVertex==1 (isPUReject)` vs. `Tight track selection (sig<2)` in data and MC for a representative ZPT/trkPT bin. This establishes the size of the PU effect and whether the tight selection already captures most of it. This is actionable today with existing infrastructure.

**If Path A shows residual effect beyond tight selection:** Proceed with Path B. The skim reprocessing is well-defined and scoped — add `trkDxy1/Dz1/DxyError1/DzError1` + `trkAssocVtx` to the track branches, add `DiDxy1/2/DiDz1/2` to the muon branches, produce a V0.4 or dedicated same-vertex skim variant, and define the cut at analysis time.

**Do not pursue Path C** unless a one-time forest-level check is needed to validate that `trkAssocVtx` is tracking the correct thing.

---

## Files that Would Need Changes (Path B)

| File | Change |
|---|---|
| `CommonCode/include/Messenger.h` | Add `std::vector<bool> *trkOnBestVtx` to `ZHadronMessenger` |
| `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp` | Compute `trkAssocVtx[iT * MTrack.nVtx + MTrack.GetBestVertexIndex()]` for each accepted track and write to the new branch |
| `MainAnalysis/20241102_ZhadronVsZPt/CorrelationAnalysis.cpp` | Add `--SameVertexMode` CLI flag; when enabled, skip tracks where `trkOnBestVtx == false` |
| `MainAnalysis/20251211_ResidualCorrection/CorrelationAnalysis.cpp` | Same flag for residual derivation |
| `MainAnalysis/20241102_ZhadronVsZPt/systematics.sh` | Add `SameVertex` to the systematic family |
| `OfficialWeightDictionary.sh` | Add dedicated residual correction files for same-vertex variant |

**No muon-side branch is needed.** `MuTreeMessenger` has no vertex index for muons — only absolute IP values (`DiDxy1/2`, `DiDz1/2`). This is not a problem: the best PV (from `GetBestVertexIndex()`) is always the Z's vertex by construction because the dimuon pT dominates `sumPtVtx` relative to any soft PU vertex. Requiring `trkOnBestVtx == true` is therefore already equivalent to matching tracks to the Z's vertex.

*This list is for planning purposes only. No changes have been made.*
