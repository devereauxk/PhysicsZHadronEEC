# pPb TnP provenance and trigger-plot update notes

## 1. Current pPb weight source

The current pPb Tag-and-Probe implementation comes from the high-`p_{T}` header on branch `80X_HI` of `CMS-HIN-dilepton/MuonAnalysis-TagAndProbe`:

- `macros/tnp_weight.h`

The local repository wrapper is:

- `CommonCode/include/tnp_weight_pPb.h`

That wrapper embeds the pPb `tnp_weight_trg_ppb(double eta, int idx)` and `tnp_weight_muid_ppb(double pt, double eta, int idx)` definitions from the high-`p_{T}` header and maps only the direct `+1 sigma` / `-1 sigma` variations into the pPb `ExtraZWeight` slots used by `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp:416-423`.

## 2. What the old “second muon typo” meant

The old dormant TnP block was supposed to evaluate the two muons independently, but the temporary variables for muon 2 were accidentally filled from the muon-1 branches again.

Historically, the buggy pattern looked like this:

```cpp
double Mu1Eta = MZHadron.muEta1->at(0);
double Mu1PT = MZHadron.muPt1->at(0);
double Mu2Eta = MZHadron.muEta1->at(0);   // wrong
double Mu2PT = MZHadron.muPt1->at(0);     // wrong
```

That made the event-level systematic weight behave like “muon 1 times muon 1 again” instead of “muon 1 times muon 2”.

## 3. What modification fixed that typo

The promoted skimmer now reads the second muon from the actual second-muon branches:

- `SampleGeneration/20250929_ReducedTreePA/ReduceForest.cpp:398-401`

```cpp
double Mu1Eta = MZHadron.muEta1->at(0);
double Mu1PT = MZHadron.muPt1->at(0);
double Mu2Eta = MZHadron.muEta2->at(0);
double Mu2PT = MZHadron.muPt2->at(0);
```

## 4. Current pPb validity behavior

There is no pPb clamping in `CommonCode/include/tnp_weight_pPb.h`.

Instead, the local wrapper explicitly returns `NaN` when either muon falls outside the intended high-`p_{T}` validity region:

- `p_{T} > 20 GeV`
- `p_{T} < 200 GeV`
- `|eta| < 2.4`

This is enforced in `CommonCode/include/tnp_weight_pPb.h`.

## 5. Current trigger turn-on plotter state

`Plots/20260326_HLTCurves/PlotHLTEfficiency.cpp` is currently the reverted ratio-only plotter.

The present implementation:

- reads `HltTree/TriggerTurnOn/HLTEffNumerator` and `HLTEffDenominator`,
- forms a single `TGraphAsymmErrors` ratio with `BayesDivide`,
- writes one PDF per invocation,
- does not emit standalone numerator or denominator PDFs.

Relevant code: `Plots/20260326_HLTCurves/PlotHLTEfficiency.cpp:20-49`.

## 6. Current shipped-skimmer validation note

The promoted skimmer was revalidated with outputs written under `SampleGeneration/20250929_ReducedTreePA/output/shipping_validation/`.

Observed counts:

- data AP split: `RecoHiForestAOD_100_AP.root` -> `tree=1956`, `loose=1956`, `tight=1956`, `num=1`, `den=1`
- data PA split: `RecoHiForestAOD_100_PA.root` -> empty, as expected for this run-window split
- PAMC reco: `RecoPAMC_10.root` -> `tree=2554`, `loose=2554`, `tight=2554`, `num=1176`, `den=1178`
- PAMC gen: `GenPAMC_10.root` -> `tree=2554`
- APMC reco: `RecoAPMC_10.root` -> `tree=16378`, `loose=16378`, `tight=16378`, `num=7467`, `den=7481`
- APMC gen: `GenAPMC_10.root` -> `tree=16378`
