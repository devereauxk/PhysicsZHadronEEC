# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 12 | 14.990697601631634 | 12 | 1.2492248001359696 | 0.24194556839845363 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 12 | 11.486030056224841 | 12 | 0.95716917135207014 | 0.4877876052760125 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 6 | 6.6721019708440821 | 6 | 1.1120169951406804 | 0.35223784084969945 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 12 | 7.9048172035899409 | 12 | 0.65873476696582844 | 0.79252366169341992 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 12 | 4.8089362170281289 | 12 | 0.40074468475234409 | 0.96405787516729213 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 7 | 4.5576242821571213 | 7 | 0.65108918316530306 | 0.71376768868583396 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 6 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 5 bins because the combined variance was zero.
