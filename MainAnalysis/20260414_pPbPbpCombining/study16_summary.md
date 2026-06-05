# Study 16: Inverse SE scale factor applied to ME_Pbp

## What is done

A bin-by-bin inverse scale factor is derived from the 2D same-event (SE) ratio and applied to the Pbp mixed-event (ME) histogram. The corrected Pbp result is then formed from the original (unmodified) Pbp SE.

### Step-by-step

1. **Derive inverse scale factor** from the 2D SE distributions:

   ```
   sf[ix, iy] = SE_Pbp[ix, iy] / SE_pPb[ix, iy]
   ```

   Falls back to sf = 1 where SE_pPb ≤ 0.

2. **Scale ME_Pbp** bin-by-bin:

   ```
   corrME_Pbp[ix, iy] = ME_Pbp[ix, iy] * sf[ix, iy]
   ```

3. **Form corrected result** using the original SE_Pbp (unchanged):

   ```
   corrResult_Pbp = SE_Pbp - corrME_Pbp
   ```

## What is plotted

| Panel | pPb | Pbp |
|-------|-----|-----|
| SE | SE_pPb (unchanged) | SE_Pbp (unchanged) |
| ME | ME_pPb (unchanged) | corrME_Pbp = ME_Pbp × sf |
| Result | SE_pPb − ME_pPb (precomputed) | SE_Pbp − corrME_Pbp |

The nominal Pbp curve (dotted) is overlaid on result panels for reference.

## Expected behavior

No closure is expected in SE, ME, or result. The SE panel shows the actual pPb/Pbp same-event mismatch (ratio not forced to 1). The ME panel reflects the Pbp mixed-event after absorbing the inverse of that SE shape. The result panel shows the net effect on the background-subtracted observable after this ME modification.

## Motivation

The previous version (forward scale factor, sf = SE_pPb/SE_Pbp) scaled ME_Pbp toward the pPb SE shape. Applying the inverse (sf = SE_Pbp/SE_pPb) instead scales ME_Pbp away from the pPb SE shape — amplifying rather than canceling the SE mismatch in the ME. This provides a complementary stress test: if the result is insensitive to this amplified ME distortion, the SE mismatch is not the dominant source of the pPb/Pbp result difference.
