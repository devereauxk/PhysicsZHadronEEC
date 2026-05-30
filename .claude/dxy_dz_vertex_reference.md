# Dxy / Dz Vertex Reference in CmsHI Forest Analyzers

**Commit**: `beda10b3d36f74c3a7ef19916e3e7554b21b52e8`  
**Base URL**: `https://github.com/CmsHI/cmssw/blob/<commit>/`

---

## Summary

Both track and dimuon impact parameters are computed relative to the **primary vertex** (best vertex by ΣpT), not the closest vertex. The two analyzers differ in how they identify that vertex:

| Variable | Vertex Reference | File | Lines |
|---|---|---|---|
| `trkDxy1`, `trkDz1` | max-ΣpT vertex (`maxPtVtx`) | TrackAnalyzer.cc | 691–695 |
| `trkDxy2`, `trkDz2` | max-multiplicity vertex (`maxMultVtx`) | TrackAnalyzer.cc | 697–701 |
| `trkDxyBS` | beam spot | TrackAnalyzer.cc | 703 |
| `Di_dxy1`, `Di_dz1` | first vertex in `hiSelectedVertex` (`vertex->begin()`) | HLTMuTree.cc | 372–373 |
| `Di_dxy2`, `Di_dz2` | same — second muon, same PV | HLTMuTree.cc | 378–379 |
| `Glb_dxy`, `Glb_dz` | first vertex in `hiSelectedVertex` | HLTMuTree.cc | 285–286 |
| `Sta_dxy`, `Sta_dz` | first vertex in `hiSelectedVertex` | HLTMuTree.cc | 305–306 |

---

## Track Impact Parameters — `TrackAnalysis/src/TrackAnalyzer.cc`

### Vertex selection loop (lines 495–565)

The analyzer iterates over all vertices and computes a per-vertex ΣpT and multiplicity
by checking each track's IP significance against that vertex:

```cpp
// line 508-509
float vtxSumPt=0.;
int vtxMult=0;

// line 521-524: per-vertex IP for association test
float Dz=etrk.dz(vtx_temp);
float Dxy=etrk.dxy(vtx_temp);

// line 530-532: accumulate if track passes IP significance cut
if(fabs(Dz/DzError) < trackVtxMaxDistance_ && fabs(Dxy/DxyError) < trackVtxMaxDistance_ ...){
    vtxSumPt+=etrk.pt();
    vtxMult++;
}

// line 560-561: running-max tracking
if( vtxMult > pev_.nTrkVtx[greatestNtrkVtx]) greatestNtrkVtx = i;
if( vtxSumPt > pev_.sumPtVtx[greatestPtVtx]) greatestPtVtx = i;

// line 564-565: stored globally
pev_.maxMultVtx = greatestNtrkVtx;
pev_.maxPtVtx = greatestPtVtx;
```

`maxPtVtx` is the index of the vertex with the highest ΣpT (sum of pT of tracks that pass
IP significance cuts). `maxMultVtx` is the index of the vertex with the highest such track count.
These are recomputed from scratch — the analyzer does not simply trust the ordering of
`offlinePrimaryVertices`, but the result effectively identifies the same PV under normal
pPb/pp conditions.

### Side effect: `trkAssocVtx` 2D array (line 517, 533)

```cpp
pev_.trkAssocVtx[it*pev_.nVtx+i]=false;   // line 517: default false for track it, vertex i
...
pev_.trkAssocVtx[trkCount*pev_.nVtx+i]=true;  // line 533: set true if IP significance passes
```

This is the flat 2D boolean `trkAssocVtx[track * nVtx + vtxIdx]` discussed for same-vertex studies.
It is filled with respect to **every vertex** using IP significance cuts.

### IP assignments (lines 691–703)

```cpp
// line 691-695: relative to maxPtVtx (primary by ΣpT)
math::XYZPoint v1(pev_.xVtx[pev_.maxPtVtx], pev_.yVtx[pev_.maxPtVtx], pev_.zVtx[pev_.maxPtVtx]);
pev_.trkDz1[pev_.nTrk]  = rndSF(etrk.dz(v1), 4);
pev_.trkDxy1[pev_.nTrk] = rndSF(etrk.dxy(v1), 4);

// line 697-701: relative to maxMultVtx (max multiplicity vertex)
math::XYZPoint v2(pev_.xVtx[pev_.maxMultVtx], pev_.yVtx[pev_.maxMultVtx], pev_.zVtx[pev_.maxMultVtx]);
pev_.trkDz2[pev_.nTrk]  = rndSF(etrk.dz(v2), 4);
pev_.trkDxy2[pev_.nTrk] = rndSF(etrk.dxy(v2), 4);

// line 703: relative to beam spot
pev_.trkDxyBS[pev_.nTrk] = etrk.dxy(beamSpot.position());
```

**Neither `trkDxy1/trkDz1` nor `trkDxy2/trkDz2` uses the closest vertex** — they use fixed
global vertex indices determined once per event.

---

## Dimuon (and Single-Muon) Impact Parameters — `MuonAnalysis/src/HLTMuTree.cc`

### Vertex loading (lines 237–242)

```cpp
edm::Handle< vector<reco::Vertex> > vertex;
iEvent.getByToken(tagVtx, vertex);    // line 238
if(vertex->size() > 0){
    vx = vertex->begin()->x();        // line 240
    vy = vertex->begin()->y();        // line 241
    vz = vertex->begin()->z();        // line 242
}
```

`vertex->begin()` is the **first element** of the `hiSelectedVertex` collection. The input tag
is defined in `MuonAnalysis/python/hltMuTree_cfi.py`, line 5:

```python
vertices = cms.InputTag("hiSelectedVertex"),
```

`hiSelectedVertex` (and its predecessor `offlinePrimaryVertices`) is ordered by the CMS
vertex reconstruction framework with the **highest-sumPt² vertex first**. `vertex->begin()` is
therefore always the primary vertex — not the closest to the muon.

### Single-muon IPs (lines 274–306)

```cpp
GlbMu.trkDxy[nGlb] = fabs(trk->dxy(vertex->begin()->position()));  // line 274
GlbMu.trkDz[nGlb]  = fabs(trk->dz(vertex->begin()->position()));   // line 275

GlbMu.dxy[nGlb] = glb->dxy(vertex->begin()->position());           // line 285
GlbMu.dz[nGlb]  = glb->dz(vertex->begin()->position());            // line 286

StaMu.dxy[nSta] = sta->dxy(vertex->begin()->position());            // line 305
StaMu.dz[nSta]  = sta->dz(vertex->begin()->position());             // line 306
```

### Dimuon IPs (lines 372–379)

Both muons in a pair use the same `vertex->begin()` position:

```cpp
// Muon 1 of pair
DiMu.dxy1[nDiMu] = glb->dxy(vertex->begin()->position());   // line 372
DiMu.dz1[nDiMu]  = glb->dz(vertex->begin()->position());    // line 373

// Muon 2 of pair
DiMu.dxy2[nDiMu] = glb2->dxy(vertex->begin()->position());  // line 378
DiMu.dz2[nDiMu]  = glb2->dz(vertex->begin()->position());   // line 379
```

These are written to the forest as branches `Di_dxy1`, `Di_dxy2`, `Di_dz1`, `Di_dz2`
(lines 512–515). The `1`/`2` suffix distinguishes the two muons in the pair, **not** two
different vertex choices.

### Branch declarations (lines 512–515)

```cpp
treeMu->Branch("Di_dxy1", DiMu.dxy1, "Di_dxy1[Di_npair]/F");
treeMu->Branch("Di_dxy2", DiMu.dxy2, "Di_dxy2[Di_npair]/F");
treeMu->Branch("Di_dz1",  DiMu.dz1,  "Di_dz1[Di_npair]/F");
treeMu->Branch("Di_dz2",  DiMu.dz2,  "Di_dz2[Di_npair]/F");
```

---

## Key Difference Between the Two Analyzers

| | TrackAnalyzer | HLTMuTree |
|---|---|---|
| Primary vertex identification | Recomputed per-event by iterating all vertices and selecting max ΣpT (line 561) | Trusts collection ordering: `vertex->begin()` (line 240) |
| Vertex collection | `offlinePrimaryVertices` (or configured source) | `hiSelectedVertex` (sorted by sumPt²) |
| Result | `maxPtVtx` index, not necessarily index 0 | Always index 0 of the ordered collection |
| IP relative to closest vertex? | No | No |

Under typical pPb/pp conditions (Z boson with pT > 15 GeV dominates ΣpT) both approaches
select the same vertex and converge to the same PV.

---

## Implication for Same-Vertex Study

The `Di_dxy1/2` and `Di_dz1/2` variables in the dimuon tree, and `trkDxy1/trkDz1` in the
track tree, are all computed relative to the **same PV** (best vertex). They are not
computed relative to each particle's closest vertex. Therefore:

- `Di_dxy`/`Di_dz` significance cannot distinguish whether a muon came from the PV or
  a secondary vertex — they are already anchored to PV.
- Track-vertex matching for the same-vertex study must use `trkAssocVtx[track*nVtx+vtxIdx]`
  (the 2D significance-based flag, lines 517/533) which is filled per vertex, not the
  IP values themselves.
