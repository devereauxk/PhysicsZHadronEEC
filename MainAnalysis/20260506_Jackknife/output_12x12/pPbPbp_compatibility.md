# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 12 | 29.768380333284156 | 12 | 2.480698361107013 | 0.0030254820828005389 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 12 | 18.911891336513101 | 12 | 1.5759909447094251 | 0.090677066473972265 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 6 | 10.450083100615753 | 6 | 1.7416805167692921 | 0.10693328400086907 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 12 | 11.437747040540552 | 12 | 0.95314558671171268 | 0.49182215586378242 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 12 | 5.3839742362239322 | 12 | 0.44866451968532767 | 0.94391005531662597 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 6 | 5.8043139524785001 | 6 | 0.96738565874641669 | 0.44546430036299811 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 6 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 6 bins because the combined variance was zero.
