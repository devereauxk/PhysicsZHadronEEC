# pPb vs Pbp compatibility: Sumw2 vs jackknife

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 20 | 41.485997202551296 | 20 | 2.0742998601275646 | 0.0032249273451230268 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 20 | 24.384789686396488 | 20 | 1.2192394843198244 | 0.22598773422237081 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 20 | 25.569766608083889 | 20 | 1.2784883304041945 | 0.18049580863171186 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 20 | 12.152323587923556 | 20 | 0.60761617939617785 | 0.91073330981108458 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
