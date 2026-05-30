# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_15` | **Sumw2** | 20 | 49.408235475176703 | 20 | 2.4704117737588351 | 0.00026902400536332771 |
| `DeltaEta_Result0.5_15` | **Jackknife** | 20 | 29.232679412389601 | 20 | 1.4616339706194801 | 0.083277913025042699 |
| `DeltaEta_Result0.5_15` | **FullCovariance** | 10 | 13.286690898609288 | 10 | 1.3286690898609288 | 0.20808140210797108 |
| `DeltaPhi_Result0.5_15` | **Sumw2** | 20 | 21.181062287497284 | 20 | 1.0590531143748643 | 0.38654510477421911 |
| `DeltaPhi_Result0.5_15` | **Jackknife** | 20 | 10.097855675140677 | 20 | 0.50489278375703384 | 0.96636262305224951 |
| `DeltaPhi_Result0.5_15` | **FullCovariance** | 12 | 9.0378425857240927 | 12 | 0.75315354881034102 | 0.69969481735445549 |

- `DeltaEta_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_15` / **FullCovariance** excluded 10 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_15` / **FullCovariance** excluded 8 bins because the combined variance was zero.
