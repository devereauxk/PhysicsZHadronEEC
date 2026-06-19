# pPb vs Pbp compatibility: Sumw2 vs jackknife vs full-covariance

| Observable | Error model | N_bins | chi2 | ndf | chi2/ndf | p-value |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| `DeltaEta_Result0.5_2` | **Sumw2** | 12 | 8.3530931215896764 | 12 | 0.69609109346580633 | 0.75696456513060284 |
| `DeltaEta_Result0.5_2` | **Jackknife** | 12 | 6.3100958223037598 | 12 | 0.52584131852531335 | 0.89965043931960453 |
| `DeltaEta_Result0.5_2` | **FullCovariance** | 12 | 6.1921156522559606 | 12 | 0.51600963768799668 | 0.90608934522415097 |
| `DeltaPhi_Result0.5_2` | **Sumw2** | 12 | 13.665599224743081 | 12 | 1.1387999353952567 | 0.32257018936388482 |
| `DeltaPhi_Result0.5_2` | **Jackknife** | 12 | 9.7355068783144301 | 12 | 0.8112922398595358 | 0.63915190870818073 |
| `DeltaPhi_Result0.5_2` | **FullCovariance** | 12 | 14.929807579499514 | 12 | 1.2441506316249595 | 0.24529755531064068 |

- `DeltaEta_Result0.5_2` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_2` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaEta_Result0.5_2` / **FullCovariance** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_2` / **Sumw2** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_2` / **Jackknife** excluded 0 bins because the combined variance was zero.
- `DeltaPhi_Result0.5_2` / **FullCovariance** excluded 0 bins because the combined variance was zero.
