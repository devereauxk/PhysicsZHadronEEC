# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 12 | 12.591648956453033 | 12 | 1.0493040797044195 | 0.39940613123477647 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 12 | 9.5451404598830134 | 12 | 0.79542837165691782 | 0.65579685725177161 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 12 | 10.401511876493339 | 12 | 0.86679265637444491 | 0.58078087979036352 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 12 | 9.9148524883237918 | 12 | 0.82623770736031599 | 0.62343050654743282 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 12 | 6.7965673563364088 | 12 | 0.56638061302803411 | 0.87075915794402481 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 12 | 8.2771164751859558 | 12 | 0.68975970626549632 | 0.76311702782439406 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 0 bins because the combined variance was zero.
