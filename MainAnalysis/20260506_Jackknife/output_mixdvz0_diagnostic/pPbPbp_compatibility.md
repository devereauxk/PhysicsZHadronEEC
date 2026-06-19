# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 12 | 17.863077467917442 | 12 | 1.4885897889931201 | 0.11991165009636821 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 12 | 13.305873116337468 | 12 | 1.1088227596947891 | 0.34720608484521481 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 6 | 7.4306197122988724 | 6 | 1.2384366187164788 | 0.28285129324678721 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 12 | 13.762416561991087 | 12 | 1.1468680468325905 | 0.31613659739275646 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 12 | 8.4384885712442728 | 12 | 0.70320738093702273 | 0.74999427924425421 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 6 | 3.7991376766051346 | 6 | 0.63318961276752239 | 0.7038367534286214 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 6 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 6 bins because the combined variance was zero.
