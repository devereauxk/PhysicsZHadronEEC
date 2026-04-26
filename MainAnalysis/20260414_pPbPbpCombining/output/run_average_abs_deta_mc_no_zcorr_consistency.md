## Inputs

- Input table: `output/run_average_abs_deta_mc_no_zcorr.tsv`
- Per-run scalar: `avg |DeltaEta| = sum_i (w_i |DeltaEta|_i) / sum_i w_i`, using the same per-track weights as the maintained sandbox run-average tabulation.
- Error bar on each run point: `sigma_run = sqrt(sum_i (w_i^2 (|DeltaEta|_i - avg |DeltaEta|)^2)) / sum_i w_i`, i.e. the propagated statistical uncertainty of that same weighted mean.
- `effective_tracks = (sum_i w_i)^2 / sum_i (w_i^2)` is reported as a diagnostic only; it is **not** used to set the plotted error bars.
- `chi2/nu` steps:
  1. Keep runs with positive finite `sigma_run`.
  2. Compute inverse-variance run weights `1 / sigma_run^2`.
  3. Form the weighted mean over runs, `xbar = sum_r (x_r / sigma_r^2) / sum_r (1 / sigma_r^2)`.
  4. Compute `chi2 = sum_r ((x_r - xbar)^2 / sigma_r^2)`.
  5. Set `nu = N_runs - 1` and report `chi2/nu`.
- Run-order plot: `plots/pPbPbp_run_average_abs_deta_mc_by_run.pdf`

## Per-run values

| Run | Orientation | avg \|DeltaEta\| | Stat. unc. | Effective tracks | Selected tracks | Selected events |
| ---: | :--- | ---: | ---: | ---: | ---: | ---: |
| 1 | Pbp MC reco | 2.688859 | 0.000390 | 19749678.81 | 20689816 | 352427 |
| 1 | pPb MC reco | 2.687453 | 0.000390 | 19785839.48 | 20659660 | 351394 |

## Run-by-run consistency results

| Grouping | Runs used | Weighted mean avg \|DeltaEta\| | chi2 | nu | chi2/nu | Assessment |
| :--- | ---: | ---: | ---: | ---: | ---: | :--- |
| pPb only | 1 | n/a | n/a | n/a | n/a | insufficient runs |
| Pbp only | 1 | n/a | n/a | n/a | n/a | insufficient runs |
| combined pPb+Pbp | 2 | 2.688155 | 6.493996 | 1 | 6.493996 | inconsistent with statistical fluctuations |

## Assessment

- pPb only: n/a (insufficient runs).
- Pbp only: n/a (insufficient runs).
- combined pPb+Pbp: 6.493996 (inconsistent with statistical fluctuations).
