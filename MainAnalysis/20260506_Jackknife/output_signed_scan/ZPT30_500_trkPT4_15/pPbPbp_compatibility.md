# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result4_15` | **Sumw2** | 12 | 5.9164300743287308 | 12 | 0.49303583952739422 | 0.92023596186879453 |
| `DeltaEta_Result4_15` | **Jackknife** | 12 | 5.476828915591736 | 12 | 0.45640240963264467 | 0.94013079546135214 |
| `DeltaEta_Result4_15` | **FullCovariance** | 12 | 5.187955340182131 | 12 | 0.43232961168184425 | 0.95140449560229301 |
| `DeltaPhi_Result4_15` | **Sumw2** | 12 | 11.700968406957404 | 12 | 0.97508070057978369 | 0.46998425004172095 |
| `DeltaPhi_Result4_15` | **Jackknife** | 12 | 11.425182102350075 | 12 | 0.95209850852917288 | 0.49287411148886273 |
| `DeltaPhi_Result4_15` | **FullCovariance** | 12 | 11.963474360550039 | 12 | 0.99695619671250324 | 0.44861751902631475 |

- `DeltaEta_Result4_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result4_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result4_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result4_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result4_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result4_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
