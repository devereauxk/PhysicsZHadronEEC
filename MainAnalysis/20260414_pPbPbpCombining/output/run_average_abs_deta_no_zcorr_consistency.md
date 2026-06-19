## Inputs

- Input table: `output/run_average_abs_deta_no_zcorr.tsv`
- Per-run scalar: `avg |DeltaEta| = sum_i (w_i |DeltaEta|_i) / sum_i w_i`, using the same per-track weights as the maintained sandbox run-average tabulation.
- Event-clustered uncertainty: for each selected Z event `e`, define `X_e = sum_t w_et` and `Y_e = sum_t (w_et |DeltaEta|_et)` over accepted tracks `t` in that event, then `sigma_run = sqrt(sum_e (Y_e - avg |DeltaEta| X_e)^2) / sum_e X_e`.
- `effective_tracks = (sum_i w_i)^2 / sum_i (w_i^2)` is reported as a diagnostic only; it is **not** used to set the plotted error bars.
- Runs with fewer than two selected Z events containing accepted tracks do not have a meaningful event-cluster variance and are excluded from the `chi2/nu` calculation.
- `chi2/nu` steps:
  1. Keep runs with positive finite `sigma_run`.
  2. Compute inverse-variance run weights `1 / sigma_run^2`.
  3. Form the weighted mean over runs, `xbar = sum_r (x_r / sigma_r^2) / sum_r (1 / sigma_r^2)`.
  4. Compute `chi2 = sum_r ((x_r - xbar)^2 / sigma_r^2)`.
  5. Set `nu = N_runs - 1` and report `chi2/nu`.
- Run-order plot: `plots/run_average_abs_deta_no_zcorr_by_run.pdf`

## Per-run values

| Run | Orientation | avg \|DeltaEta\| | Stat. unc. | Effective tracks | Selected tracks | Selected events | Events with tracks |
| ---: | :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| 285480 | Pbp | 2.745457 | 0.333609 | 228.90 | 266 | 4 | 4 |
| 285505 | Pbp | 2.754920 | 0.099580 | 10583.43 | 10802 | 147 | 147 |
| 285517 | Pbp | 2.724191 | 0.046701 | 65898.37 | 67778 | 945 | 942 |
| 285530 | Pbp | 2.747705 | 0.036701 | 85953.40 | 88634 | 1250 | 1248 |
| 285537 | Pbp | 2.674322 | 0.074300 | 21097.71 | 21759 | 306 | 305 |
| 285538 | Pbp | 2.737515 | 0.108538 | 8159.68 | 8568 | 122 | 122 |
| 285539 | Pbp | 2.814986 | 0.054823 | 40805.55 | 42116 | 612 | 608 |
| 285549 | Pbp | 2.679514 | 0.063385 | 28445.68 | 29422 | 415 | 414 |
| 285684 | Pbp | 2.692610 | 0.054159 | 37796.28 | 39008 | 545 | 541 |
| 285718 | Pbp | 2.936644 | 0.073945 | 19804.69 | 20547 | 287 | 287 |
| 285726 | Pbp | 2.778242 | 0.074548 | 23733.48 | 24387 | 354 | 354 |
| 285750 | Pbp | 2.792769 | 0.039427 | 82132.25 | 84429 | 1212 | 1211 |
| 285759 | Pbp | 2.832888 | 0.057220 | 32483.46 | 33574 | 470 | 469 |
| 285832 | Pbp | 2.767742 | 0.144666 | 5249.23 | 5359 | 73 | 73 |
| 285975 | pPb | 2.795998 | 0.122513 | 8162.60 | 8443 | 115 | 115 |
| 285993 | pPb | 2.731689 | 0.055615 | 41216.87 | 42287 | 612 | 612 |
| 286009 | pPb | 2.831504 | 0.089171 | 15892.39 | 16437 | 232 | 232 |
| 286010 | pPb | 2.752260 | 0.052944 | 44696.84 | 45908 | 627 | 627 |
| 286023 | pPb | 2.802286 | 0.051207 | 49223.10 | 50737 | 734 | 733 |
| 286031 | pPb | 2.662259 | 0.083384 | 18936.08 | 19371 | 268 | 268 |
| 286033 | pPb | 2.794408 | 0.167491 | 4759.69 | 4937 | 71 | 71 |
| 286034 | pPb | 2.777801 | 0.068066 | 29593.28 | 30354 | 424 | 424 |
| 286051 | pPb | 2.751994 | 0.067670 | 27508.35 | 28319 | 409 | 409 |
| 286054 | pPb | 2.653152 | 0.059115 | 29496.79 | 30346 | 421 | 421 |
| 286069 | pPb | 2.801924 | 0.069242 | 27042.28 | 27937 | 388 | 386 |
| 286070 | pPb | 2.734471 | 0.134945 | 5525.76 | 5681 | 81 | 81 |
| 286178 | pPb | 2.724898 | 0.058731 | 36487.87 | 37746 | 513 | 512 |
| 286200 | pPb | 2.711525 | 0.095576 | 12298.55 | 12597 | 176 | 176 |
| 286201 | pPb | 2.715796 | 0.089755 | 16209.22 | 16675 | 233 | 233 |
| 286288 | pPb | 2.753996 | 0.176484 | 4136.38 | 4274 | 67 | 67 |
| 286301 | pPb | 2.808245 | 0.053860 | 44951.67 | 46540 | 666 | 664 |
| 286302 | pPb | 2.645875 | 0.066748 | 23271.89 | 24087 | 336 | 336 |
| 286309 | pPb | 2.967541 | 0.585661 | 310.61 | 315 | 4 | 4 |
| 286314 | pPb | 3.123467 | 0.160157 | 4700.42 | 4834 | 70 | 70 |
| 286327 | pPb | 3.190278 | 0.316077 | 1981.56 | 2008 | 31 | 31 |
| 286329 | pPb | 2.740028 | 0.102765 | 12656.73 | 13114 | 196 | 196 |
| 286365 | pPb | 2.759287 | 0.050313 | 53736.14 | 55320 | 788 | 785 |
| 286420 | pPb | 2.473560 | 0.332582 | 1275.76 | 1293 | 20 | 20 |
| 286422 | pPb | 2.444340 | 0.298543 | 1849.90 | 2016 | 28 | 28 |
| 286425 | pPb | 2.784189 | 0.046883 | 59988.21 | 61973 | 880 | 880 |
| 286441 | pPb | 2.368330 | 0.210606 | 692.72 | 740 | 13 | 13 |
| 286442 | pPb | 2.701290 | 0.048792 | 49046.80 | 50507 | 706 | 705 |
| 286450 | pPb | 2.724994 | 0.038092 | 73317.63 | 75501 | 1112 | 1109 |
| 286471 | pPb | 2.756150 | 0.043574 | 70216.78 | 72015 | 1013 | 1011 |
| 286496 | pPb | 2.753991 | 0.062542 | 34224.25 | 35407 | 495 | 495 |

## Run-by-run consistency results

| Grouping | Runs used | Weighted mean avg \|DeltaEta\| | chi2 | nu | chi2/nu | Assessment |
| :--- | ---: | ---: | ---: | ---: | ---: | :--- |
| pPb only | 31 | 2.746418 | 25.310708 | 30 | 0.843690 | consistent with statistical fluctuations |
| Pbp only | 14 | 2.762843 | 14.295459 | 13 | 1.099651 | consistent with statistical fluctuations |
| combined pPb+Pbp | 45 | 2.752553 | 40.258327 | 44 | 0.914962 | consistent with statistical fluctuations |

## Assessment

- pPb only: 0.843690 (consistent with statistical fluctuations).
- Pbp only: 1.099651 (consistent with statistical fluctuations).
- combined pPb+Pbp: 0.914962 (consistent with statistical fluctuations).
