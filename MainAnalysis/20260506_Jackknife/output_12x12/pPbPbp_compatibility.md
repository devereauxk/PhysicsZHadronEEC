# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 12 | 34.693056828284817 | 12 | 2.8910880690237346 | 0.00052401169185226472 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 12 | 21.155736794515562 | 12 | 1.7629780662096302 | 0.048145912523474783 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 6 | 11.859931539716817 | 6 | 1.9766552566194695 | 0.065167506850658113 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 12 | 14.584633002629685 | 12 | 1.2153860835524737 | 0.26494098025905771 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 12 | 6.7068435272045299 | 12 | 0.55890362726704412 | 0.8763630822125158 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 6 | 4.1296021464241184 | 6 | 0.68826702440401977 | 0.65914271129356861 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 6 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 6 bins because the combined variance was zero.
