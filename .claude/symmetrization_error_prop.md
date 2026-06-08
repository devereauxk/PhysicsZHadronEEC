Yes — if you adopt the **diagonal approximation everywhere for displayed statistical uncertainties**, then the symmetrized statistical error should also be propagated in the **same diagonal-only approximation**.

## Rule for combining signed pPb and Pbp

If the signed combined central value is formed as

\[
R_{\rm comb}(b)=w_{\rm pPb}R_{\rm pPb}(b)+w_{\rm Pbp}R_{\rm Pbp}(b),
\]

then the displayed statistical error on the **signed combined** bin \(b\) should be

\[
\boxed{
\sigma_{\rm comb}^2(b)
=
w_{\rm pPb}^2 \sigma_{\rm pPb}^2(b)
+
w_{\rm Pbp}^2 \sigma_{\rm Pbp}^2(b)
}
\]

assuming pPb and Pbp are statistically independent.

So it is “sum in quadrature,” but with the **same weights used in the central-value combination**.

If instead the combination is an unweighted sum or average, then use the corresponding coefficients.

---

# Then: how to propagate through symmetrization?

That depends on whether you are symmetrizing 1D or 2D.

---

## 1D symmetrization

If the symmetrized 1D result is

\[
R_{\rm sym}(x)=\frac12\left[R(+x)+R(-x)\right],
\]

then under the **diagonal approximation** you ignore the covariance between \(+x\) and \(-x\), so:

\[
\boxed{
\sigma_{\rm sym}(x)
=
\frac12\sqrt{\sigma^2(+x)+\sigma^2(-x)}
}
\]

where \(\sigma(+x)\) and \(\sigma(-x)\) are the already-combined signed-bin statistical errors.

So the sequence is:

1. compute signed pPb error per bin from jackknife diagonal,
2. compute signed Pbp error per bin from jackknife diagonal,
3. combine them in quadrature with weights,
4. then symmetrize with the formula above.

---

## 2D symmetrization

If the final 2D symmetrization is

\[
R_{\rm sym}=\frac14(R_1+R_2+R_3+R_4),
\]

for the four related bins:
- \((+\Delta\eta,+\Delta\phi)\)
- \((+\Delta\eta,-\Delta\phi)\)
- \((-\Delta\eta,+\Delta\phi)\)
- \((-\Delta\eta,-\Delta\phi)\)

then under the **diagonal approximation** the propagated error is

\[
\boxed{
\sigma_{\rm sym}
=
\frac14\sqrt{
\sigma_1^2+\sigma_2^2+\sigma_3^2+\sigma_4^2
}
}
\]

again using the already-combined signed-bin errors for those four bins.

---

# Important consequence

If you adopt this policy, then:

- jackknife is used only to get **per-bin diagonal variances** for signed pPb and signed Pbp separately,
- the displayed combined pPb+Pbp uncertainty is **not** from a pooled jackknife covariance anymore,
- and the displayed symmetrized uncertainty is just standard diagonal propagation from the combined signed errors.

So this is a **deliberate approximation** for plotting.

--- 

# Summary formulas

## Signed pPb/Pbp combination
\[
\sigma_{\rm comb}^2(b)=w_{\rm pPb}^2\sigma_{\rm pPb}^2(b)+w_{\rm Pbp}^2\sigma_{\rm Pbp}^2(b)
\]

## 2D four-bin symmetrization
\[
\sigma_{\rm sym}=\frac14\sqrt{\sigma_1^2+\sigma_2^2+\sigma_3^2+\sigma_4^2}
\]