#!/usr/bin/env python3
"""
Covariance-aware KS-like cumulative-shape statistic for pPb vs Pbp.
Per plan: .claude/handoff_ks_test_20260604.md

Reads JackknifeProjection TTree from pPb and Pbp result ROOT files,
builds full N×N jackknife covariance matrices, then computes:
  T_max = max_k |Z_k|   where Z_k = S_k / sqrt(V_kk)
and a toy-MC p-value under the null hypothesis Delta ~ N(0, C_tot).
"""

import argparse
import os
import sys
import numpy as np
import uproot

# ── Covariance builder ──────────────────────────────────────────────────────


def read_jackknife_events(root_file, tree_name, n_bins):
    """Read per-event jackknife branches from TTree; return arrays."""
    t = root_file[tree_name]
    sig_nz = t["SignalNZ"].array(library="np").astype(np.float64)
    mix_nz = t["MixNZ"].array(library="np").astype(np.float64)
    sig_eta = t["SignalEta"].array(library="np")
    mix_eta = t["MixEta"].array(library="np")
    sig_phi = t["SignalPhi"].array(library="np")
    mix_phi = t["MixPhi"].array(library="np")
    n_events = len(sig_nz)

    # Pad/truncate jagged arrays to fixed (n_events, n_bins) matrices
    def to_matrix(arr):
        m = np.zeros((n_events, n_bins), dtype=np.float64)
        for k in range(n_events):
            v = arr[k].astype(np.float64)
            nb = min(n_bins, len(v))
            m[k, :nb] = v[:nb]
        return m

    return (sig_nz, mix_nz,
            to_matrix(sig_eta), to_matrix(mix_eta),
            to_matrix(sig_phi), to_matrix(mix_phi))


def build_covariance(sig_nz, mix_nz, sig_bins, mix_bins, bin_widths):
    """
    Build N×N jackknife LOO covariance matrix in result-histogram units.
    Mirrors buildCovMatrix() in compute_pPbPbp_compatibility.cpp exactly.
    """
    N = len(bin_widths)
    n_events = len(sig_nz)

    total_sig_nz = np.sum(sig_nz)
    total_mix_nz = np.sum(mix_nz)

    total_sig = np.sum(sig_bins, axis=0)
    total_mix = np.sum(mix_bins, axis=0)

    if total_sig_nz <= 0 or total_mix_nz <= 0:
        return np.zeros((N, N))

    # theta: full estimate before bin-width division
    theta = total_sig / total_sig_nz - total_mix / total_mix_nz

    C = np.zeros((N, N))
    valid_n = 0

    for k in range(n_events):
        left_sig_nz = total_sig_nz - sig_nz[k]
        left_mix_nz = total_mix_nz - mix_nz[k]
        if left_sig_nz <= 0 or left_mix_nz <= 0:
            continue
        valid_n += 1

        theta_loo = (total_sig - sig_bins[k]) / left_sig_nz \
                  - (total_mix - mix_bins[k]) / left_mix_nz
        d = (theta_loo - theta) / bin_widths
        C += np.outer(d, d)

    if valid_n > 1:
        C *= (valid_n - 1) / valid_n

    return C


# ── KS-like statistic ───────────────────────────────────────────────────────


def compute_cov_ks(delta, C_tot, n_toys=100_000, seed=42):
    """
    Covariance-aware KS-like cumulative-shape statistic.

    Parameters
    ----------
    delta  : shape (N,), bin-by-bin difference R_pPb - R_Pbp
    C_tot  : shape (N,N), total covariance C_pPb + C_Pbp
    n_toys : number of toy MC replicas

    Returns
    -------
    dict with keys: T_max, p_value, S_k, V_kk, Z_k, excluded_bins,
                    regularized, min_eig_before, toy_mean, toy_rms, n_toys
    """
    N = len(delta)

    # Lower-triangular summation matrix: L[k,i] = 1 if i <= k else 0
    L = np.tril(np.ones((N, N)))

    # Cumulative difference and covariance
    S = L @ delta
    V = L @ C_tot @ L.T
    V_diag = np.diag(V).copy()

    # Standardized cumulative differences
    valid = V_diag > 0
    excluded = list(np.where(~valid)[0])
    sqrt_V = np.where(valid, np.sqrt(np.where(valid, V_diag, 1.0)), np.nan)
    Z = np.where(valid, S / sqrt_V, np.nan)
    T_max_obs = float(np.nanmax(np.abs(Z)))

    # ── Numerical validation and regularization ─────────────────────────────
    C_sym = 0.5 * (C_tot + C_tot.T)
    eigvals = np.linalg.eigvalsh(C_sym)
    min_eig_before = float(eigvals.min())
    regularized = False

    if min_eig_before < 0:
        regularized = True
        C_sym += (-min_eig_before * 1.001) * np.eye(N)

    # ── Toy MC ──────────────────────────────────────────────────────────────
    rng = np.random.default_rng(seed)
    toys = rng.multivariate_normal(np.zeros(N), C_sym, size=n_toys)  # (n_toys, N)
    S_toys = toys @ L.T                                               # (n_toys, N)
    Z_toys = S_toys / sqrt_V[np.newaxis, :]
    Z_toys[:, ~valid] = np.nan
    T_max_toys = np.nanmax(np.abs(Z_toys), axis=1)

    p_value = float(np.mean(T_max_toys >= T_max_obs))
    toy_mean = float(np.mean(T_max_toys))
    toy_rms = float(np.std(T_max_toys))

    return dict(T_max=T_max_obs, p_value=p_value, S_k=S, V_kk=V_diag, Z_k=Z,
                excluded_bins=excluded, regularized=regularized,
                min_eig_before=min_eig_before,
                toy_mean=toy_mean, toy_rms=toy_rms, n_toys=n_toys)


# ── Output writers ──────────────────────────────────────────────────────────


def write_tex(path, results_eta, results_phi, trk_range="0.5_15"):
    """Write a TeX tabular (no float wrapper) for embedding in beamer."""
    obs_eta = f"DeltaEta_Result{trk_range}".replace("_", "\\_")
    obs_phi = f"DeltaPhi_Result{trk_range}".replace("_", "\\_")
    with open(path, "w") as f:
        f.write("\\begin{tabular}{|l|r|r|r|}\n\\hline\n")
        f.write("Observable & $T_{\\rm max}$ & $p$-value & $N_{\\rm toys}$ \\\\\n\\hline\n")
        for obs, res in [(obs_eta, results_eta), (obs_phi, results_phi)]:
            f.write(
                f"\\texttt{{{obs}}} & "
                f"{res['T_max']:.4f} & "
                f"{res['p_value']:.4f} & "
                f"{res['n_toys']:d} \\\\\n"
            )
        f.write("\\hline\n\\end{tabular}\n")


def write_markdown(path, results_eta, results_phi, delta_eta, delta_phi,
                   bin_edges_eta, bin_edges_phi, trk_range="0.5_15"):
    """Write full markdown report with per-bin cumulative table."""
    with open(path, "w") as f:
        f.write("# Covariance-aware KS-like statistic: pPb vs Pbp (nominal 12×12)\n\n")
        f.write(f"Track range: {trk_range}  |  N_toys = {results_eta['n_toys']}\n\n")

        for obs_name, res, delta, edges in [
            (f"DeltaEta_Result{trk_range}", results_eta, delta_eta, bin_edges_eta),
            (f"DeltaPhi_Result{trk_range}", results_phi, delta_phi, bin_edges_phi),
        ]:
            f.write(f"## Observable: {obs_name}\n\n")
            f.write(f"### Input summary\n\n")
            f.write(f"- N_bins: {len(delta)}\n")
            f.write(f"- Covariance regularized: {'yes' if res['regularized'] else 'no'}\n")
            f.write(f"- Min eigenvalue before regularization: {res['min_eig_before']:.3e}\n")
            if res["excluded_bins"]:
                f.write(f"- Excluded cumulative bins: {res['excluded_bins']}\n")
            else:
                f.write(f"- Excluded cumulative bins: none\n")
            f.write(f"\n### Observed statistic\n\n")
            f.write(f"- T_max: {res['T_max']:.6f}\n")
            f.write(f"- Toy p-value: {res['p_value']:.6f}\n")
            f.write(f"- N_toys: {res['n_toys']}\n")
            f.write(f"- Toy T_max mean: {res['toy_mean']:.4f}\n")
            f.write(f"- Toy T_max RMS:  {res['toy_rms']:.4f}\n")
            f.write(f"\n### Per-bin cumulative table\n\n")
            f.write("| Bin | center | Delta_i | S_k | V_kk | sqrt(V_kk) | Z_k |\n")
            f.write("| --- | ---: | ---: | ---: | ---: | ---: | ---: |\n")
            N = len(delta)
            centers = 0.5 * (edges[:-1] + edges[1:])
            for k in range(N):
                sqV = np.sqrt(res["V_kk"][k]) if res["V_kk"][k] > 0 else float("nan")
                Z_k = res["Z_k"][k] if res["V_kk"][k] > 0 else float("nan")
                f.write(f"| {k} | {centers[k]:.4f} | {delta[k]:.6e} | "
                        f"{res['S_k'][k]:.6e} | {res['V_kk'][k]:.6e} | "
                        f"{sqV:.6e} | {Z_k:.4f} |\n")
            f.write("\n")


# ── Main ────────────────────────────────────────────────────────────────────


def main():
    ap = argparse.ArgumentParser(
        description="Covariance-aware KS-like statistic for pPb vs Pbp (Study 0 nominal)."
    )
    ap.add_argument("--pPbFile", required=True)
    ap.add_argument("--PbPFile", required=True)
    ap.add_argument("--TrkRange", default="0.5_15")
    ap.add_argument("--OutputDir", default="output_12x12_nominal")
    ap.add_argument("--NToys", type=int, default=100_000)
    ap.add_argument("--Seed", type=int, default=42)
    args = ap.parse_args()

    os.makedirs(args.OutputDir, exist_ok=True)
    trk = args.TrkRange
    tree_name = f"JackknifeProjection{trk}"
    eta_key = f"DeltaEta_Result{trk}"
    phi_key = f"DeltaPhi_Result{trk}"

    print(f"Opening pPb: {args.pPbFile}")
    print(f"Opening Pbp: {args.PbPFile}")

    with uproot.open(args.pPbFile) as fA, uproot.open(args.PbPFile) as fB:
        # Read projected histograms for bin values and edges
        hA_eta, eA_eta = fA[eta_key].to_numpy()
        hB_eta, eB_eta = fB[eta_key].to_numpy()
        hA_phi, eA_phi = fA[phi_key].to_numpy()
        hB_phi, eB_phi = fB[phi_key].to_numpy()

        assert np.allclose(eA_eta, eB_eta), "pPb/Pbp DeltaEta bin edges mismatch"
        assert np.allclose(eA_phi, eB_phi), "pPb/Pbp DeltaPhi bin edges mismatch"

        bw_eta = np.diff(eA_eta)
        bw_phi = np.diff(eA_phi)
        N_eta = len(bw_eta)
        N_phi = len(bw_phi)

        print(f"DeltaEta: {N_eta} bins   DeltaPhi: {N_phi} bins")

        # Read jackknife events
        print("Reading pPb jackknife tree...")
        (A_sig_nz, A_mix_nz,
         A_sig_eta, A_mix_eta,
         A_sig_phi, A_mix_phi) = read_jackknife_events(fA, tree_name, max(N_eta, N_phi))

        print("Reading Pbp jackknife tree...")
        (B_sig_nz, B_mix_nz,
         B_sig_eta, B_mix_eta,
         B_sig_phi, B_mix_phi) = read_jackknife_events(fB, tree_name, max(N_eta, N_phi))

    print(f"pPb events: {len(A_sig_nz)}   Pbp events: {len(B_sig_nz)}")

    # Build covariance matrices
    print("Building DeltaEta covariance matrices...")
    C_pPb_eta = build_covariance(A_sig_nz, A_mix_nz,
                                  A_sig_eta[:, :N_eta], A_mix_eta[:, :N_eta], bw_eta)
    C_Pbp_eta = build_covariance(B_sig_nz, B_mix_nz,
                                  B_sig_eta[:, :N_eta], B_mix_eta[:, :N_eta], bw_eta)

    print("Building DeltaPhi covariance matrices...")
    C_pPb_phi = build_covariance(A_sig_nz, A_mix_nz,
                                  A_sig_phi[:, :N_phi], A_mix_phi[:, :N_phi], bw_phi)
    C_Pbp_phi = build_covariance(B_sig_nz, B_mix_nz,
                                  B_sig_phi[:, :N_phi], B_mix_phi[:, :N_phi], bw_phi)

    C_tot_eta = C_pPb_eta + C_Pbp_eta
    C_tot_phi = C_pPb_phi + C_Pbp_phi

    # Covariance validation
    for name, C in [("C_tot_eta", C_tot_eta), ("C_tot_phi", C_tot_phi)]:
        sym_err = np.max(np.abs(C - C.T))
        neg_diag = np.sum(np.diag(C) < 0)
        print(f"  {name}: symmetry_err={sym_err:.2e}  neg_diagonal_entries={neg_diag}")

    # Difference vectors
    delta_eta = hA_eta - hB_eta
    delta_phi = hA_phi - hB_phi

    # Compute KS-like statistics
    print(f"Running toy MC (N_toys={args.NToys}) for DeltaEta...")
    res_eta = compute_cov_ks(delta_eta, C_tot_eta, n_toys=args.NToys, seed=args.Seed)
    print(f"  T_max={res_eta['T_max']:.4f}   p-value={res_eta['p_value']:.4f}"
          f"   regularized={res_eta['regularized']}")

    print(f"Running toy MC (N_toys={args.NToys}) for DeltaPhi...")
    res_phi = compute_cov_ks(delta_phi, C_tot_phi, n_toys=args.NToys, seed=args.Seed)
    print(f"  T_max={res_phi['T_max']:.4f}   p-value={res_phi['p_value']:.4f}"
          f"   regularized={res_phi['regularized']}")

    # Write outputs
    tex_path = os.path.join(args.OutputDir, "pPbPbp_covKS.tex")
    md_path  = os.path.join(args.OutputDir, "pPbPbp_covKS.md")
    write_tex(tex_path, res_eta, res_phi, trk)
    write_markdown(md_path, res_eta, res_phi,
                   delta_eta, delta_phi, eA_eta, eA_phi, trk)

    print(f"\nOutputs written to {args.OutputDir}/")
    print(f"  {tex_path}")
    print(f"  {md_path}")


if __name__ == "__main__":
    main()
