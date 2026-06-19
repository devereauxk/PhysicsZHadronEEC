# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 12 | 12.976419868982449 | 12 | 1.081368322415204 | 0.37075692100315488 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 12 | 9.4517799982129169 | 12 | 0.78764833318440974 | 0.66393442750015685 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 12 | 9.4698450051900327 | 12 | 0.78915375043250269 | 0.662361424922431 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 12 | 16.565735751180135 | 12 | 1.3804779792650113 | 0.16667578221706972 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 12 | 11.348254679935213 | 12 | 0.94568788999460107 | 0.49933235842608581 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 12 | 12.38485450202085 | 12 | 1.0320712085017376 | 0.41528718632041278 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
