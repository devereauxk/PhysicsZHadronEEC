# Test 1: pPb vs PbP compatibility — JK vs Bootstrap

- NBootstrap: 2000, Seed: 42
- pPb events: 10524, PbP events: 6046

| Observable | Error model | chi2 | ndf | p-value |
| --- | --- | ---: | ---: | ---: |
| DeltaEta_Result0.5_15 | Sumw2 | 12.5916 | 12 | 0.3994 |
| DeltaEta_Result0.5_15 | Diagonal (JK) | 9.5451 | 12 | 0.6558 |
| DeltaEta_Result0.5_15 | Diagonal (Bootstrap) | 9.6468 | 12 | 0.6469 |
| DeltaEta_Result0.5_15 | FullCovariance (JK) | 10.4015 | 12 | 0.5808 |
| DeltaEta_Result0.5_15 | FullCovariance (Bootstrap) | 10.6106 | 12 | 0.5626 |
| DeltaPhi_Result0.5_15 | Sumw2 | 9.9149 | 12 | 0.6234 |
| DeltaPhi_Result0.5_15 | Diagonal (JK) | 6.7966 | 12 | 0.8708 |
| DeltaPhi_Result0.5_15 | Diagonal (Bootstrap) | 6.8076 | 12 | 0.8701 |
| DeltaPhi_Result0.5_15 | FullCovariance (JK) | 8.2771 | 12 | 0.7631 |
| DeltaPhi_Result0.5_15 | FullCovariance (Bootstrap) | 8.4308 | 12 | 0.7506 |
