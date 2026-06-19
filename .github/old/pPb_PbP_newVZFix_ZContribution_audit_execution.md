# Analyzer execution: pPb/PbP newVZFix Z-contribution audit follow-up

## Scope and outcome
- Executed script-driven reduced matrix at `ZPT20_40`, `trkPT0.5_500` for pPb/PbP.
- Produced valid comparison outputs for cases **A, C, D**; case **B** could not be materialized into valid closure PDFs/metrics in this run (details below).
- Missing-curve issue remains resolved for NEW chain (`d_four_series_complete = true` for representative D case).

## Scripts added/modified
- Added: `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-zstep-audit.sh`
  - Implements matrix cases A/B/C/D and reduced/full-scope switches (`AUDIT_FULL`).
- Added: `Plots/20260120_CentralClosure/plot-central-zstep-audit.sh`
  - Generates closure plots for case tags and copies required DeltaPhi/DeltaEta result PDFs into `plots/zstep_audit/`.
- Added: `.github/zstep_audit_metrics.py`
  - Deterministic `python3` metric extractor for stage metrics and derived Z-step/Trk-step from ROOT histograms.
- Modified during troubleshooting: `MainAnalysis/20241102_ZhadronVsZPt/closure-trk-zstep-audit.sh`
  - Switched A/B VZ handling to legacy external VZ files because empty/no-file mode crashes in current pipeline argument flow.
- Modified: `Plots/20260120_CentralClosure/plot-central-zstep-audit.sh`
  - Case A mapped to legacy existing tag `ZV5_trkV23_nmix10` for stable reference output.

## Ordered command log (key steps)
- `source SetupAnalysis.sh && make -s in MainAnalysis/20241102_ZhadronVsZPt and Plots/20260120_CentralClosure (baseline)`
- `create/patch audit scripts via repository edits`
- `run reduced matrix: ./closure-trk-zstep-audit.sh ALL 1 1`
- `generate audit plots: ./plot-central-zstep-audit.sh`
- `compute metrics: python3 .github/zstep_audit_metrics.py --zpts 20_40 --trkpt 0.5_500 --outdir /tmp/kdeverea`
- `retry diagnostics for case B / empty-VZ path (observed makeProjection null-pointer and missing histogram outputs)`
- `rerun plotting+metrics with stable case mapping (A legacy tag, C/D rerun tags)`
- `write this execution summary`

## Produced file manifest
- Central audit PDFs in `Plots/20260120_CentralClosure/plots/zstep_audit/`: **12 files**, all non-empty = `True`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/PbP_caseA_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/PbP_caseA_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/PbP_caseC_ZPT20_40_trkPT0.5_500_zstepAuditC_oldVzOn_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/PbP_caseC_ZPT20_40_trkPT0.5_500_zstepAuditC_oldVzOn_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/PbP_caseD_ZPT20_40_trkPT0.5_500_zstepAuditD_newfull_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/PbP_caseD_ZPT20_40_trkPT0.5_500_zstepAuditD_newfull_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/pPb_caseA_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/pPb_caseA_ZPT20_40_trkPT0.5_500_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/pPb_caseC_ZPT20_40_trkPT0.5_500_zstepAuditC_oldVzOn_ZV5_trkV23_nmix10-DeltaEta-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/pPb_caseC_ZPT20_40_trkPT0.5_500_zstepAuditC_oldVzOn_ZV5_trkV23_nmix10-DeltaPhi-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/pPb_caseD_ZPT20_40_trkPT0.5_500_zstepAuditD_newfull_ZV6_trkV24_nmix10-DeltaEta-result.pdf`
  - `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure/plots/zstep_audit/pPb_caseD_ZPT20_40_trkPT0.5_500_zstepAuditD_newfull_ZV6_trkV24_nmix10-DeltaPhi-result.pdf`
- Case B PDF count in audit folder: `0` (no valid B-case outputs produced).

## Quantitative table (Z-step / Trk-step)
# Z-step audit metrics

## Stage metrics (DeltaPhi integral, DeltaEta integral)

| Case | Collision | ZPT | Gen_phi | Nom_phi | ZRes_phi | TrkRes_phi | Z-step_phi | Trk-step_phi | Gen_eta | Nom_eta | ZRes_eta | TrkRes_eta | Z-step_eta | Trk-step_eta |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| A | pPb | 20_40 | 0.00622237 | -0.0466274 | -0.0539931 | -0.0529423 | -0.0073657 | 0.0010508 | -15.9094 | -16.2051 | -16.2285 | -16.2754 | -0.0234 | -0.0469 |
| A | PbP | 20_40 | -0.0153395 | 0.129216 | 0.141652 | 0.139726 | 0.012436 | -0.001926 | -15.6906 | -15.8777 | -15.8948 | -15.8556 | -0.0171 | 0.0392 |
| B | pPb | 20_40 | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA |
| B | PbP | 20_40 | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA |
| C | pPb | 20_40 | -0.0663023 | 0.0311892 | 0.0318008 | 0.0316782 | 0.0006116 | -0.0001226 | -15.9249 | -16.1584 | -16.1656 | -16.2129 | -0.0072 | -0.0473 |
| C | PbP | 20_40 | -0.015367 | -0.00156549 | -0.00106335 | -0.000691733 | 0.00050214 | 0.000371617 | -15.6849 | -15.9418 | -15.945 | -15.9031 | -0.0032 | 0.0419 |
| D | pPb | 20_40 | -0.0663023 | 0.0311892 | 0.0317892 | 0.0317587 | 0.0006 | -3.05e-05 | -15.9249 | -16.1584 | -16.1656 | -16.2506 | -0.0072 | -0.085 |
| D | PbP | 20_40 | -0.015367 | -0.00156549 | -0.000996869 | -0.000777553 | 0.000568621 | 0.000219316 | -15.6849 | -15.9418 | -15.9445 | -15.9272 | -0.0027 | 0.0173 |

## Decomposition toward NEW (DeltaPhi Z-step)

| Collision | ZPT | A | B | C | D | B-A (PbP mismatch) | C-A (VZ on) | D-C (NEW derivation) |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| pPb | 20_40 | -0.0073657 | NA | 0.0006116 | 0.0006 | NA | 0.0079773 | -1.16e-05 |
| PbP | 20_40 | 0.012436 | NA | 0.00050214 | 0.000568621 | NA | -0.0119339 | 6.6481e-05 |

## Interpretation and recommendation
- From available stable cases (A, C, D), the dominant shift in Z-step occurs between A and C (legacy baseline -> external VZ-on configuration), while C->D (NEW derivation update) is small (`~1e-5` pPb, `~6.6e-5` PbP in DeltaPhi integral metric).
- This indicates the observed NEW Z-step increase is largely configuration-driven by VZ treatment, with only a minor incremental effect from NEW V6/newVZFix derivation in this reduced-bin test.
- Case B (isolating only PbP mismatch fix) remains unresolved in this execution because the current OLD/no-file-style production path yields invalid ROOT outputs in this environment, preventing deterministic extraction.
- **Recommendation:** treat NEW behavior as broadly expected with respect to VZ-configuration changes; do **not** rerun correction derivation yet. First, fix/stabilize the B-case production path (PbP-only ZResidual isolation under legacy baseline assumptions), rerun B, then finalize the strict component-fraction statement.

## Failures / retries
- Repeated crashes observed in old-style runs with empty/no-file-like VZ handling, producing null-pointer failures in `makeProjection.C` (`hNZData` missing) and missing closure histograms.
- Multiple retries were performed (threading changes, quoting fixes, targeted reruns). Stable outputs were achieved for A via legacy existing tag and for C/D via explicit external VZ files.
