Use a **pooled event-level jackknife** over the union of the pPb and Pbp Z-event sets.

## Core bookkeeping

Assume pPb and Pbp have already been:
- boosted into the common frame,
- Pbp flipped into the common signed convention,
- subjected to the same acceptance,
- and expressed with per-event signal/mix contributions.

Then define one combined event list:

\[
\mathcal E_{\rm HI} = \mathcal E_{\rm pPb} \sqcup \mathcal E_{\rm Pbp}
\]

Each event record should carry, for every event \(e\):

- signal normalization contribution: \(n^S_e\)
- mixed normalization contribution: \(n^B_e\)
- per-bin signal contributions: \(s_{e,b}\)
- per-bin mixed contributions: \(b_{e,b}\)

where \(b\) can be:
- a 2D bin \((\Delta\eta,\Delta\phi)\), or
- a projected 1D bin if you store projected contributions directly.

## Full-sample estimator

For each bin \(b\),

\[
R_b = \frac{\sum_{e\in\mathcal E} s_{e,b}}{\sum_{e\in\mathcal E} n^S_e}
-
\frac{\sum_{e\in\mathcal E} b_{e,b}}{\sum_{e\in\mathcal E} n^B_e}
\]

This is the combined pPb+Pbp estimator.

## Leave-one-out replica bookkeeping

For each event \(e \in \mathcal E\), form one replica by removing that event from the **pooled** set:

\[
R_b^{(-e)}=
\frac{\sum_{e'\neq e} s_{e',b}}{\sum_{e'\neq e} n^S_{e'}}
-
\frac{\sum_{e'\neq e} b_{e',b}}{\sum_{e'\neq e} n^B_{e'}}
\]

So:
- if \(e\) is a pPb event, drop only that pPb event;
- if \(e\) is a Pbp event, drop only that Pbp event.

You do **not** build separate pPb and Pbp jackknives and combine them afterward if the target estimator is the pooled combined result.

## Variance / covariance

With \(N = |\mathcal E|\),

\[
\mathrm{Cov}_{bc}^{\rm JK}
=
\frac{N-1}{N}
\sum_{e=1}^{N}
\left(R_b^{(-e)}-R_b\right)
\left(R_c^{(-e)}-R_c\right)
\]

and the per-bin statistical error is

\[
\sigma_b = \sqrt{\mathrm{Cov}_{bb}^{\rm JK}}.
\]

## Best practice for symmetrization

If you later symmetrize, do it at the replica level.

For each replica, first form the signed combined histogram, then symmetrize:

- 1D:
  \[
  R_{\rm sym}^{(-e)}(x)=\frac12\left(R^{(-e)}(x)+R^{(-e)}(-x)\right)
  \]

- 2D fourfold:
  \[
  R_{\rm sym}^{(-e)}=\frac14(R_1^{(-e)}+R_2^{(-e)}+R_3^{(-e)}+R_4^{(-e)})
  \]

Then compute the jackknife covariance from the **symmetrized replicas**.

That automatically keeps the non-diagonal contributions.

## Practical bookkeeping recommendation

Store one flat replica table keyed by:
- `dataset` = pPb or Pbp
- `event_id`
- `signalNZ`
- `mixNZ`
- bin contributions

Then:
1. concatenate pPb and Pbp rows,
2. compute global totals once,
3. for replica \(e\), subtract that row from the totals,
4. evaluate the combined estimator.

This is the cleanest and least error-prone approach.

## What not to do

Do not:
- compute separate pPb and Pbp jackknife errors and then combine them with weights, unless your estimator is explicitly a weighted average of finished results;
- symmetrize central values first and then propagate only diagonal errors;
- drop one pPb event and one Pbp event together in the same replica.

## Bottom line

\[
\boxed{
\text{Use one leave-one-Z-event-out jackknife over the pooled pPb+Pbp event list, with each replica dropping exactly one event from either orientation.}
}
\]

That is the proper bookkeeping if pPb and Pbp have already been transformed into the common convention and are being combined as one pooled heavy-ion sample.