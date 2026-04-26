#!/usr/bin/env python3

import argparse
import csv
import json
import math
import os
import subprocess
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser(description="Compute Z-step/Trk-step audit metrics from central closure ROOT files.")
    parser.add_argument(
        "--repo",
        default="/home/kdeverea/PhysicsZHadronEEC",
        help="Repository root path",
    )
    parser.add_argument(
        "--zpts",
        default="20_40",
        help="Comma-separated ZPT ranges (default: 20_40)",
    )
    parser.add_argument(
        "--trkpt",
        default="0.5_500",
        help="Track PT range key in histogram names (default: 0.5_500)",
    )
    parser.add_argument(
        "--outdir",
        default="/tmp/kdeverea",
        help="Output directory for csv/json/md artifacts",
    )
    return parser.parse_args()


def _run_root_metric_macro(macro_path: Path, file_path: str, hist_name: str):
    cmd = ["root", "-l", "-b", "-q", f'{macro_path}("{file_path}","{hist_name}")']
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True, check=False)
    integral = None
    mean = None
    error = None
    for line in result.stdout.splitlines():
        if line.startswith("__METRIC__"):
            parts = line.strip().split()
            if len(parts) >= 3:
                integral = float(parts[1])
                mean = float(parts[2])
        if line.startswith("__ERROR__"):
            error = line.strip()
    if result.returncode != 0 and error is None:
        error = f"root exit code {result.returncode}"
    return integral, mean, error


def read_hist_metric(file_path: str, hist_name: str, macro_path: Path):
    if not os.path.exists(file_path):
        return None, None, "missing file"
    integral, mean, error = _run_root_metric_macro(macro_path, file_path, hist_name)
    if error:
        return None, None, error
    return integral, mean, None


def safe_subtract(a, b):
    if a is None or b is None:
        return None
    return a - b


def format_num(x):
    if x is None or (isinstance(x, float) and (math.isnan(x) or math.isinf(x))):
        return "NA"
    return f"{x:.6g}"


def main():
    args = parse_args()
    repo = Path(args.repo)
    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    zpts = [z.strip() for z in args.zpts.split(",") if z.strip()]
    trkpt = args.trkpt

    cases = {
        "A": {"tag": "ZV5_trkV23_nmix10", "description": "OLD-reference reproduction (legacy existing tag)"},
        "B": {"tag": "zstepAuditB_oldfixPbP_ZV5_trkV23_nmix10", "description": "OLD + PbP ZResidual file fix only"},
        "C": {"tag": "zstepAuditC_oldVzOn_ZV5_trkV23_nmix10", "description": "OLD + VZ-file-on (OLD Z files)"},
        "D": {"tag": "zstepAuditD_newfull_ZV6_trkV24_nmix10", "description": "NEW full chain"},
    }
    collisions = {"pPb": "pPbMC", "PbP": "PbPMC"}
    stages = ["Gen_nominal", "nominal", "ZResidual", "trkResidual"]

    macro_path = outdir / "zstep_metric_reader.C"
    macro_path.write_text(
        "\n".join(
            [
                "#include <TFile.h>",
                "#include <TH1.h>",
                "#include <iostream>",
                "void zstep_metric_reader(const char *file, const char *hist) {",
                "  TFile *f = TFile::Open(file, \"READ\");",
                "  if(!f || f->IsZombie()) { std::cout << \"__ERROR__ open\" << std::endl; return; }",
                "  TH1 *h = (TH1 *)f->Get(hist);",
                "  if(h == nullptr) { std::cout << \"__ERROR__ hist\" << std::endl; f->Close(); return; }",
                "  std::cout << \"__METRIC__ \" << h->Integral() << \" \" << h->GetMean() << std::endl;",
                "  f->Close();",
                "}",
            ]
        )
    )

    rows = []
    for case_key, case_info in cases.items():
        for collision, mc_prefix in collisions.items():
            for zpt in zpts:
                record = {
                    "case": case_key,
                    "case_desc": case_info["description"],
                    "collision": collision,
                    "zpt": zpt,
                    "tag": case_info["tag"],
                }
                for stage in stages:
                    base = repo / "MainAnalysis/20241102_ZhadronVsZPt/plots" / f"{mc_prefix}_{stage}_{case_info['tag']}_ZPT{zpt}"
                    result_file = str(base) + "-result.root"
                    phi_i, phi_m, phi_err = read_hist_metric(result_file, f"DeltaPhi_Result{trkpt}", macro_path)
                    eta_i, eta_m, eta_err = read_hist_metric(result_file, f"DeltaEta_Result{trkpt}", macro_path)
                    record[f"{stage}_phi_integral"] = phi_i
                    record[f"{stage}_phi_mean"] = phi_m
                    record[f"{stage}_eta_integral"] = eta_i
                    record[f"{stage}_eta_mean"] = eta_m
                    record[f"{stage}_error"] = phi_err or eta_err

                record["Z_step_phi_integral"] = safe_subtract(record["ZResidual_phi_integral"], record["nominal_phi_integral"])
                record["Trk_step_phi_integral"] = safe_subtract(record["trkResidual_phi_integral"], record["ZResidual_phi_integral"])
                record["Z_step_eta_integral"] = safe_subtract(record["ZResidual_eta_integral"], record["nominal_eta_integral"])
                record["Trk_step_eta_integral"] = safe_subtract(record["trkResidual_eta_integral"], record["ZResidual_eta_integral"])
                rows.append(record)

    by_key = {(r["case"], r["collision"], r["zpt"]): r for r in rows}
    contrib_rows = []
    for collision in collisions:
        for zpt in zpts:
            a = by_key.get(("A", collision, zpt))
            b = by_key.get(("B", collision, zpt))
            c = by_key.get(("C", collision, zpt))
            d = by_key.get(("D", collision, zpt))
            if not all([a, b, c, d]):
                continue
            contrib_rows.append(
                {
                    "collision": collision,
                    "zpt": zpt,
                    "A_Z_step_phi": a["Z_step_phi_integral"],
                    "B_Z_step_phi": b["Z_step_phi_integral"],
                    "C_Z_step_phi": c["Z_step_phi_integral"],
                    "D_Z_step_phi": d["Z_step_phi_integral"],
                    "PbP_mismatch_component_phi": safe_subtract(b["Z_step_phi_integral"], a["Z_step_phi_integral"]),
                    "VZ_activation_component_phi": safe_subtract(c["Z_step_phi_integral"], a["Z_step_phi_integral"]),
                    "NEW_derivation_component_phi": safe_subtract(d["Z_step_phi_integral"], c["Z_step_phi_integral"]),
                    "A_Trk_step_phi": a["Trk_step_phi_integral"],
                    "B_Trk_step_phi": b["Trk_step_phi_integral"],
                    "C_Trk_step_phi": c["Trk_step_phi_integral"],
                    "D_Trk_step_phi": d["Trk_step_phi_integral"],
                }
            )

    csv_path = outdir / "zstep_audit_metrics.csv"
    json_path = outdir / "zstep_audit_metrics.json"
    contrib_csv_path = outdir / "zstep_audit_contributions.csv"
    md_path = outdir / "zstep_audit_metrics.md"

    fieldnames = [
        "case",
        "case_desc",
        "collision",
        "zpt",
        "tag",
        "Gen_nominal_phi_integral",
        "nominal_phi_integral",
        "ZResidual_phi_integral",
        "trkResidual_phi_integral",
        "Z_step_phi_integral",
        "Trk_step_phi_integral",
        "Gen_nominal_eta_integral",
        "nominal_eta_integral",
        "ZResidual_eta_integral",
        "trkResidual_eta_integral",
        "Z_step_eta_integral",
        "Trk_step_eta_integral",
        "Gen_nominal_error",
        "nominal_error",
        "ZResidual_error",
        "trkResidual_error",
    ]

    with csv_path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow({k: row.get(k) for k in fieldnames})

    with json_path.open("w") as f:
        json.dump({"rows": rows, "contributions": contrib_rows}, f, indent=2)

    contrib_fields = [
        "collision",
        "zpt",
        "A_Z_step_phi",
        "B_Z_step_phi",
        "C_Z_step_phi",
        "D_Z_step_phi",
        "PbP_mismatch_component_phi",
        "VZ_activation_component_phi",
        "NEW_derivation_component_phi",
        "A_Trk_step_phi",
        "B_Trk_step_phi",
        "C_Trk_step_phi",
        "D_Trk_step_phi",
    ]
    with contrib_csv_path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=contrib_fields)
        writer.writeheader()
        for row in contrib_rows:
            writer.writerow({k: row.get(k) for k in contrib_fields})

    md_lines = []
    md_lines.append("# Z-step audit metrics")
    md_lines.append("")
    md_lines.append("## Stage metrics (DeltaPhi integral, DeltaEta integral)")
    md_lines.append("")
    md_lines.append("| Case | Collision | ZPT | Gen_phi | Nom_phi | ZRes_phi | TrkRes_phi | Z-step_phi | Trk-step_phi | Gen_eta | Nom_eta | ZRes_eta | TrkRes_eta | Z-step_eta | Trk-step_eta |")
    md_lines.append("|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|")
    for r in rows:
        md_lines.append(
            f"| {r['case']} | {r['collision']} | {r['zpt']} | "
            f"{format_num(r['Gen_nominal_phi_integral'])} | {format_num(r['nominal_phi_integral'])} | "
            f"{format_num(r['ZResidual_phi_integral'])} | {format_num(r['trkResidual_phi_integral'])} | "
            f"{format_num(r['Z_step_phi_integral'])} | {format_num(r['Trk_step_phi_integral'])} | "
            f"{format_num(r['Gen_nominal_eta_integral'])} | {format_num(r['nominal_eta_integral'])} | "
            f"{format_num(r['ZResidual_eta_integral'])} | {format_num(r['trkResidual_eta_integral'])} | "
            f"{format_num(r['Z_step_eta_integral'])} | {format_num(r['Trk_step_eta_integral'])} |"
        )

    md_lines.append("")
    md_lines.append("## Decomposition toward NEW (DeltaPhi Z-step)")
    md_lines.append("")
    md_lines.append("| Collision | ZPT | A | B | C | D | B-A (PbP mismatch) | C-A (VZ on) | D-C (NEW derivation) |")
    md_lines.append("|---|---|---:|---:|---:|---:|---:|---:|---:|")
    for r in contrib_rows:
        md_lines.append(
            f"| {r['collision']} | {r['zpt']} | {format_num(r['A_Z_step_phi'])} | {format_num(r['B_Z_step_phi'])} | "
            f"{format_num(r['C_Z_step_phi'])} | {format_num(r['D_Z_step_phi'])} | "
            f"{format_num(r['PbP_mismatch_component_phi'])} | {format_num(r['VZ_activation_component_phi'])} | "
            f"{format_num(r['NEW_derivation_component_phi'])} |"
        )

    md_path.write_text("\n".join(md_lines) + "\n")

    print(f"WROTE {csv_path}")
    print(f"WROTE {json_path}")
    print(f"WROTE {contrib_csv_path}")
    print(f"WROTE {md_path}")


if __name__ == "__main__":
    main()
