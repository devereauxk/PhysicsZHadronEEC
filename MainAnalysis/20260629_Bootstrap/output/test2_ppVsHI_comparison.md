# Test 2: Combined pPb vs pp — JK vs Bootstrap

- NBootstrap: 2000, Seed: 42
- pPb events: 10524, PbP events: 6046, pp events: 88745

| Observable | Error model | chi2 | ndf | p-value |
| --- | --- | ---: | ---: | ---: |
| DeltaEta | Diagonal (JK) | 5.5765 | 6 | 0.4723 |
| DeltaEta | FullCovariance (JK) | 5.9768 | 6 | 0.4258 |
| DeltaEta | Diagonal (Bootstrap) | 5.5247 | 6 | 0.4785 |
| DeltaEta | FullCovariance (Bootstrap) | 5.8803 | 6 | 0.4367 |
| DeltaPhi | Diagonal (JK) | 0.2214 | 6 | 0.9998 |
| DeltaPhi | FullCovariance (JK) | 0.2794 | 6 | 0.9996 |
| DeltaPhi | Diagonal (Bootstrap) | 0.2251 | 6 | 0.9998 |
| DeltaPhi | FullCovariance (Bootstrap) | 0.2706 | 6 | 0.9996 |
