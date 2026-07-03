# Bootstrap cross-check of jackknife p-values (all kinematic bins)

B = 2000 bootstrap replicates, seed = 42

## Test 1: pPb vs PbP compatibility (FullCovariance, 12 bins)

| Bin | Observable | chi2 (JK) | p (JK) | chi2 (Boot) | p (Boot) |
| --- | --- | ---: | ---: | ---: | ---: |
| ZPT0_500 trkPT0.5_15 | DeltaEta | 10.40 | 0.5808 | 10.61 | 0.5626 |
| ZPT0_500 trkPT0.5_15 | DeltaPhi | 8.28 | 0.7631 | 8.43 | 0.7506 |
| ZPT0_500 trkPT0.5_2 | DeltaEta | 11.13 | 0.5175 | 11.43 | 0.4926 |
| ZPT0_500 trkPT0.5_2 | DeltaPhi | 4.97 | 0.9589 | 4.87 | 0.9622 |
| ZPT0_500 trkPT2_4 | DeltaEta | 10.70 | 0.5551 | 10.68 | 0.5562 |
| ZPT0_500 trkPT2_4 | DeltaPhi | 9.02 | 0.7014 | 9.05 | 0.6985 |
| ZPT0_500 trkPT4_15 | DeltaEta | 5.96 | 0.9179 | 5.89 | 0.9214 |
| ZPT0_500 trkPT4_15 | DeltaPhi | 10.02 | 0.6141 | 9.84 | 0.6301 |
| ZPT0_30 trkPT0.5_2 | DeltaEta | 15.29 | 0.2262 | 15.44 | 0.2181 |
| ZPT0_30 trkPT0.5_2 | DeltaPhi | 3.21 | 0.9939 | 3.33 | 0.9927 |
| ZPT0_30 trkPT2_4 | DeltaEta | 18.95 | 0.0897 | 18.77 | 0.0942 |
| ZPT0_30 trkPT2_4 | DeltaPhi | 5.43 | 0.9422 | 5.34 | 0.9456 |
| ZPT0_30 trkPT4_15 | DeltaEta | 9.26 | 0.6803 | 9.02 | 0.7011 |
| ZPT0_30 trkPT4_15 | DeltaPhi | 14.67 | 0.2599 | 15.06 | 0.2381 |
| ZPT30_500 trkPT0.5_2 | DeltaEta | 6.19 | 0.9061 | 6.21 | 0.9051 |
| ZPT30_500 trkPT0.5_2 | DeltaPhi | 14.93 | 0.2453 | 14.91 | 0.2465 |
| ZPT30_500 trkPT2_4 | DeltaEta | 6.51 | 0.8882 | 6.69 | 0.8776 |
| ZPT30_500 trkPT2_4 | DeltaPhi | 14.07 | 0.2962 | 14.49 | 0.2702 |
| ZPT30_500 trkPT4_15 | DeltaEta | 5.19 | 0.9514 | 5.31 | 0.9467 |
| ZPT30_500 trkPT4_15 | DeltaPhi | 11.96 | 0.4486 | 12.14 | 0.4341 |

## Test 2: Combined pPb vs pp (FullCovariance, stat+syst, 6 unique bins)

| Bin | Observable | chi2 (JK) | p (JK) | chi2 (Boot) | p (Boot) |
| --- | --- | ---: | ---: | ---: | ---: |
| ZPT0_500 trkPT0.5_15 | DeltaEta | 5.98 | 0.4258 | 5.88 | 0.4367 |
| ZPT0_500 trkPT0.5_15 | DeltaPhi | 0.28 | 0.9996 | 0.27 | 0.9996 |
| ZPT0_500 trkPT0.5_2 | DeltaEta | 6.04 | 0.4189 | 6.01 | 0.4216 |
| ZPT0_500 trkPT0.5_2 | DeltaPhi | 0.63 | 0.9959 | 0.60 | 0.9963 |
| ZPT0_500 trkPT2_4 | DeltaEta | 2.82 | 0.8310 | 2.84 | 0.8283 |
| ZPT0_500 trkPT2_4 | DeltaPhi | 0.16 | 0.9999 | 0.17 | 0.9999 |
| ZPT0_500 trkPT4_15 | DeltaEta | 1.86 | 0.9325 | 1.86 | 0.9319 |
| ZPT0_500 trkPT4_15 | DeltaPhi | 2.66 | 0.8504 | 2.69 | 0.8464 |
| ZPT0_30 trkPT0.5_2 | DeltaEta | 10.07 | 0.1216 | 9.78 | 0.1343 |
| ZPT0_30 trkPT0.5_2 | DeltaPhi | 1.83 | 0.9346 | 1.87 | 0.9316 |
| ZPT0_30 trkPT2_4 | DeltaEta | 1.76 | 0.9407 | 1.74 | 0.9421 |
| ZPT0_30 trkPT2_4 | DeltaPhi | 0.73 | 0.9939 | 0.71 | 0.9943 |
| ZPT0_30 trkPT4_15 | DeltaEta | 2.77 | 0.8375 | 2.86 | 0.8257 |
| ZPT0_30 trkPT4_15 | DeltaPhi | 2.95 | 0.8149 | 2.91 | 0.8195 |
| ZPT30_500 trkPT0.5_2 | DeltaEta | 1.26 | 0.9736 | 1.24 | 0.9750 |
| ZPT30_500 trkPT0.5_2 | DeltaPhi | 1.40 | 0.9660 | 1.43 | 0.9641 |
| ZPT30_500 trkPT2_4 | DeltaEta | 1.51 | 0.9591 | 1.52 | 0.9580 |
| ZPT30_500 trkPT2_4 | DeltaPhi | 0.68 | 0.9950 | 0.66 | 0.9953 |
| ZPT30_500 trkPT4_15 | DeltaEta | 1.04 | 0.9842 | 1.03 | 0.9845 |
| ZPT30_500 trkPT4_15 | DeltaPhi | 1.95 | 0.9239 | 1.96 | 0.9231 |
