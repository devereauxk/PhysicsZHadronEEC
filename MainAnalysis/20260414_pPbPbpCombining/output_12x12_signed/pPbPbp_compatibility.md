# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 12 | 11.365877343672405 | 12 | 0.94715644530603382 | 0.4978502317048058 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 12 | 8.4854387022966797 | 12 | 0.70711989185805668 | 0.74613828080169653 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 12 | 9.8645088529600731 | 12 | 0.82204240441333942 | 0.62784596174630125 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 12 | 9.5957414549538704 | 12 | 0.79964512124615583 | 0.65137850614455473 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 12 | 6.3821990954278247 | 12 | 0.53184992461898539 | 0.89560382083359236 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 12 | 6.7177022851060295 | 12 | 0.55980852375883583 | 0.87569132040650532 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
