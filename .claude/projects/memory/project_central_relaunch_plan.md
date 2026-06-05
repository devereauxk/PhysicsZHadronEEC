---
name: central-relaunch-plan
description: Plan to kill b09pulodp after pp section completes and relaunch pPb+PbP with NTHREAD=50
metadata:
  type: project
---

After pp section of central.sh 1 1 1 completes, kill the job and relaunch only pPb+PbP with NTHREAD=50 to use more of the 112-core machine (was using 20, now 50).

**Why:** Machine has 112 cores; NTHREAD=20 used only 40 cores when both jobs ran together (load ~44). With scan pp-EE finishing its pp stages around the same time, pPb+PbP can be relaunched alone at NTHREAD=50 (50/112 cores). This cuts ~18 hrs off the pPb+PbP wall time.

**How to apply:** When `plots/pp_trkResidual_EEV6_ZV10_trkV29_nmix10_ZPT0_500-result.root` appears, execute kill+relaunch. Full procedure saved at `/home/kdeverea/.claude/saved_prompts/kill_relaunch_ppb_pbp.md`.

Key identifiers (stable across all bins):
- b09pulodp process group: PGID 2606345 (bash central.sh 1 1 1)
- b09pulodp root wrapper: PID 2605902
- btucj0lwp (scan pp-EE): PGID 2605525 — DO NOT KILL

Kill command: `kill -TERM -2606345 2>/dev/null; kill -TERM 2605902 2>/dev/null`

Relaunch: `cd MainAnalysis/20241102_ZhadronVsZPt && SKIP_CLEAN=1 NTHREAD=50 nohup bash central.sh 0 1 1 > /tmp/kdeverea/central_ppb_pbp_<timestamp>.log 2>&1 &`

Expected pp completion: ~18:37 CEST Jun 2 2026 (inclusive ZPT0_500 bin last).
Expected pPb+PbP completion with NTHREAD=50: ~9-10 hrs after relaunch (~04:00-05:00 Jun 3).
