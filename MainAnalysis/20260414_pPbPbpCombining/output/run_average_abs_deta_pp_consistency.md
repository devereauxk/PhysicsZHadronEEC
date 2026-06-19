## Inputs

- Input table: `output/run_average_abs_deta_pp.tsv`
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
- Run-order plot: `plots/run_average_abs_deta_pp_by_run.pdf`

## Per-run values

| Run | Orientation | avg \|DeltaEta\| | Stat. unc. | Effective tracks | Selected tracks | Selected events | Events with tracks |
| ---: | :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| 306553 | pp data | 1.630108 | 0.000000 | 40.96 | 41 | 1 | 1 |
| 306563 | pp data | 2.728041 | 0.056233 | 14197.88 | 14266 | 716 | 709 |
| 306572 | pp data | 2.704523 | 0.017308 | 136051.51 | 136718 | 6689 | 6658 |
| 306580 | pp data | 2.718224 | 0.014923 | 179361.44 | 180240 | 9007 | 8973 |
| 306584 | pp data | 2.788042 | 0.071883 | 8566.32 | 8610 | 426 | 425 |
| 306595 | pp data | 2.712383 | 0.066800 | 8935.94 | 8986 | 463 | 462 |
| 306598 | pp data | 2.717842 | 0.032213 | 39404.86 | 39622 | 1970 | 1959 |
| 306604 | pp data | 2.748396 | 0.020561 | 99318.52 | 99868 | 5018 | 4992 |
| 306629 | pp data | 2.722825 | 0.020737 | 95940.60 | 96382 | 4806 | 4784 |
| 306630 | pp data | 2.642676 | 0.054479 | 13465.77 | 13538 | 666 | 660 |
| 306631 | pp data | 2.692758 | 0.019300 | 107558.18 | 108119 | 5467 | 5445 |
| 306636 | pp data | 3.224532 | 0.404402 | 280.30 | 282 | 18 | 18 |
| 306645 | pp data | 2.631306 | 0.143177 | 1440.58 | 1448 | 73 | 72 |
| 306646 | pp data | 2.723979 | 0.206162 | 1041.55 | 1046 | 52 | 52 |
| 306647 | pp data | 2.971727 | 0.328205 | 201.94 | 203 | 11 | 11 |
| 306651 | pp data | 2.659567 | 0.162932 | 1515.07 | 1521 | 75 | 74 |
| 306652 | pp data | 2.750212 | 0.213168 | 967.72 | 973 | 50 | 49 |
| 306653 | pp data | 2.638212 | 0.057185 | 12585.58 | 12644 | 609 | 608 |
| 306654 | pp data | 2.751482 | 0.054933 | 14529.61 | 14596 | 735 | 730 |
| 306656 | pp data | 2.778441 | 0.050309 | 16152.72 | 16233 | 823 | 819 |
| 306657 | pp data | 2.683165 | 0.022434 | 82295.31 | 82724 | 4163 | 4144 |
| 306705 | pp data | 2.707991 | 0.015945 | 158458.94 | 159232 | 7917 | 7874 |
| 306709 | pp data | 2.710829 | 0.016052 | 164392.32 | 165206 | 8236 | 8199 |
| 306772 | pp data | 2.680029 | 0.018005 | 125888.79 | 126497 | 6320 | 6287 |
| 306773 | pp data | 2.702685 | 0.026178 | 62647.55 | 62980 | 3109 | 3097 |
| 306776 | pp data | 2.587089 | 0.213259 | 773.39 | 778 | 42 | 42 |
| 306777 | pp data | 2.633657 | 0.029503 | 46606.18 | 46865 | 2320 | 2306 |
| 306793 | pp data | 2.695072 | 0.013219 | 232481.45 | 233652 | 11592 | 11532 |
| 306794 | pp data | 2.686757 | 0.031227 | 42763.66 | 42989 | 2141 | 2126 |
| 306801 | pp data | 2.709245 | 0.016579 | 148854.34 | 149596 | 7369 | 7338 |
| 306826 | pp data | 2.701706 | 0.028784 | 49721.35 | 49977 | 2477 | 2461 |

## Run-by-run consistency results

| Grouping | Runs used | Weighted mean avg \|DeltaEta\| | chi2 | nu | chi2/nu | Assessment |
| :--- | ---: | ---: | ---: | ---: | ---: | :--- |
| pp only | 30 | 2.704411 | 26.449482 | 29 | 0.912051 | consistent with statistical fluctuations |

## Assessment

- pp only: 0.912051 (consistent with statistical fluctuations).
