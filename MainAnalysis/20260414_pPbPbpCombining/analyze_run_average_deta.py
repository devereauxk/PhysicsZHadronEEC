#!/usr/bin/env python3

import argparse
import csv
import math
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def parse_groups(group_args):
    groups = []
    for item in group_args:
        parts = item.split(":", 2)
        if len(parts) != 3:
            raise ValueError(f"Invalid --group value '{item}'. Expected system:table-label:assessment-label")
        groups.append(
            {
                "system": parts[0],
                "table_label": parts[1],
                "assessment_label": parts[2],
            }
        )
    return groups


def parse_system_order(order_arg):
    if not order_arg:
        return []
    return [item.strip() for item in order_arg.split(",") if item.strip()]


def read_rows(path: Path):
    rows = []
    with path.open() as handle:
        reader = csv.DictReader(handle, delimiter="\t")
        for raw in reader:
            row = {
                "system": raw["system"],
                "run": int(raw["run"]),
                "average_abs_deta": float(raw["average_abs_deta"]),
                "stat_uncertainty": float(raw["average_abs_deta_stat_uncertainty"]),
                "total_track_weight": float(raw["total_track_weight"]),
                "selected_tracks": int(raw["selected_tracks"]),
                "selected_events": int(raw["selected_events"]),
                "effective_tracks": float(raw["effective_tracks"]),
                "events_with_tracks": int(raw.get("events_with_tracks", raw["selected_events"])),
            }
            rows.append(row)
    return rows


def ordered_rows(rows, system_order):
    order_map = {system: index for index, system in enumerate(system_order)}
    return sorted(rows, key=lambda row: (order_map.get(row["system"], len(order_map)), row["run"]))


def assessment_label(chi2_over_nu):
    if chi2_over_nu is None:
        return "insufficient runs"
    if chi2_over_nu < 1.5:
        return "consistent with statistical fluctuations"
    if chi2_over_nu < 2.5:
        return "marginal / mild excess spread"
    return "inconsistent with statistical fluctuations"


def compute_group(rows):
    valid = [
        row for row in rows
        if row["events_with_tracks"] >= 2
        and row["stat_uncertainty"] > 0
        and math.isfinite(row["stat_uncertainty"])
    ]
    if len(valid) < 2:
        return {
            "n_runs": len(valid),
            "mean": None,
            "chi2": None,
            "nu": None,
            "chi2_over_nu": None,
            "excluded": len(rows) - len(valid),
            "assessment": "insufficient runs",
        }

    weights = [1.0 / (row["stat_uncertainty"] ** 2) for row in valid]
    mean = sum(weight * row["average_abs_deta"] for weight, row in zip(weights, valid)) / sum(weights)
    chi2 = sum(((row["average_abs_deta"] - mean) ** 2) / (row["stat_uncertainty"] ** 2) for row in valid)
    nu = len(valid) - 1
    chi2_over_nu = chi2 / nu

    return {
        "n_runs": len(valid),
        "mean": mean,
        "chi2": chi2,
        "nu": nu,
        "chi2_over_nu": chi2_over_nu,
        "excluded": len(rows) - len(valid),
        "assessment": assessment_label(chi2_over_nu),
    }


def format_float(value, digits=6):
    if value is None:
        return "n/a"
    return f"{value:.{digits}f}"


def write_markdown(output: Path, input_path: Path, plot_path: Path, rows, results, label_map, assessment_labels):
    ordered = ordered_rows(rows, list(label_map.keys()))

    lines = []
    lines.append("## Inputs")
    lines.append("")
    lines.append(f"- Input table: `{input_path}`")
    lines.append("- Per-run scalar: `avg |DeltaEta| = sum_i (w_i |DeltaEta|_i) / sum_i w_i`, using the same per-track weights as the maintained sandbox run-average tabulation.")
    lines.append("- Event-clustered uncertainty: for each selected Z event `e`, define `X_e = sum_t w_et` and `Y_e = sum_t (w_et |DeltaEta|_et)` over accepted tracks `t` in that event, then `sigma_run = sqrt(sum_e (Y_e - avg |DeltaEta| X_e)^2) / sum_e X_e`.")
    lines.append("- `effective_tracks = (sum_i w_i)^2 / sum_i (w_i^2)` is reported as a diagnostic only; it is **not** used to set the plotted error bars.")
    lines.append("- Runs with fewer than two selected Z events containing accepted tracks do not have a meaningful event-cluster variance and are excluded from the `chi2/nu` calculation.")
    lines.append("- `chi2/nu` steps:")
    lines.append("  1. Keep runs with positive finite `sigma_run`.")
    lines.append("  2. Compute inverse-variance run weights `1 / sigma_run^2`.")
    lines.append("  3. Form the weighted mean over runs, `xbar = sum_r (x_r / sigma_r^2) / sum_r (1 / sigma_r^2)`.")
    lines.append("  4. Compute `chi2 = sum_r ((x_r - xbar)^2 / sigma_r^2)`.")
    lines.append("  5. Set `nu = N_runs - 1` and report `chi2/nu`.")
    lines.append(f"- Run-order plot: `{plot_path}`")
    lines.append("")
    lines.append("## Per-run values")
    lines.append("")
    lines.append("| Run | Orientation | avg \\|DeltaEta\\| | Stat. unc. | Effective tracks | Selected tracks | Selected events | Events with tracks |")
    lines.append("| ---: | :--- | ---: | ---: | ---: | ---: | ---: | ---: |")
    for row in ordered:
        orientation = label_map.get(row["system"], row["system"])
        lines.append(
            f"| {row['run']} | {orientation} | {row['average_abs_deta']:.6f} | {row['stat_uncertainty']:.6f} | "
            f"{row['effective_tracks']:.2f} | {row['selected_tracks']} | {row['selected_events']} | {row['events_with_tracks']} |"
        )

    lines.append("")
    lines.append("## Run-by-run consistency results")
    lines.append("")
    lines.append("| Grouping | Runs used | Weighted mean avg \\|DeltaEta\\| | chi2 | nu | chi2/nu | Assessment |")
    lines.append("| :--- | ---: | ---: | ---: | ---: | ---: | :--- |")
    for label, result in results.items():
        lines.append(
            f"| {label} | {result['n_runs']} | {format_float(result['mean'])} | {format_float(result['chi2'])} | "
            f"{result['nu'] if result['nu'] is not None else 'n/a'} | {format_float(result['chi2_over_nu'])} | {result['assessment']} |"
        )
    lines.append("")
    lines.append("## Assessment")
    lines.append("")
    for key, result in results.items():
        lines.append(
            f"- {assessment_labels.get(key, key)}: {format_float(result['chi2_over_nu'])} ({result['assessment']})."
        )

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text("\n".join(lines) + "\n")


def compute_y_limits(rows):
    if not rows:
        return None, None
    low = min(row["average_abs_deta"] - row["stat_uncertainty"] for row in rows)
    high = max(row["average_abs_deta"] + row["stat_uncertainty"] for row in rows)
    span = high - low
    padding = 0.05 * span if span > 0 else 0.1
    return low - padding, high + padding


def make_plot(output: Path, rows, label_map, prefix_system_in_ticks: bool,
    y_min=None, y_max=None, plot_min_events_with_tracks=0):
    plot_rows = [
        row for row in ordered_rows(rows, list(label_map.keys()))
        if row["events_with_tracks"] >= plot_min_events_with_tracks
    ]
    x_positions = list(range(1, len(plot_rows) + 1))
    if prefix_system_in_ticks:
        tick_labels = [
            f"{label_map.get(row['system'], row['system'])}:{row['run']}"
            for row in plot_rows
        ]
    else:
        tick_labels = [str(row["run"]) for row in plot_rows]

    output.parent.mkdir(parents=True, exist_ok=True)
    plt.figure(figsize=(16, 6))
    colors = ["tab:blue", "tab:green", "tab:red", "tab:orange", "tab:purple"]
    markers = ["s", "o", "^", "D", "v"]
    system_positions = {}
    for x, row in zip(x_positions, plot_rows):
        system_positions.setdefault(row["system"], []).append((x, row))

    sorted_systems = list(label_map.keys()) + [s for s in system_positions.keys() if s not in label_map]
    boundaries = []
    running_max = 0
    for index, system in enumerate(sorted_systems):
        pairs = system_positions.get(system, [])
        if not pairs:
            continue
        xs = [x for x, _ in pairs]
        ys = [row["average_abs_deta"] for _, row in pairs]
        errs = [row["stat_uncertainty"] for _, row in pairs]
        plt.errorbar(
            xs,
            ys,
            yerr=errs,
            fmt=markers[index % len(markers)],
            color=colors[index % len(colors)],
            capsize=2,
            label=label_map.get(system, system),
        )
        if running_max != 0:
            boundaries.append(running_max + 0.5)
        running_max = max(xs)

    for boundary in boundaries:
        plt.axvline(boundary, color="gray", linestyle="--", linewidth=1)

    plt.xticks(x_positions, tick_labels, rotation=90)
    plt.xlabel("Run number")
    plt.ylabel("avg |DeltaEta|")
    plt.title("Run-by-run avg |DeltaEta| with event-clustered uncertainties")
    if y_min is not None and y_max is not None:
        plt.ylim(y_min, y_max)
    plt.legend()
    plt.grid(axis="y", alpha=0.25)
    plt.tight_layout()
    plt.savefig(output)
    plt.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True)
    parser.add_argument("--markdown-output", required=True)
    parser.add_argument("--plot-output", required=True)
    parser.add_argument("--ppb-label", default="pPb")
    parser.add_argument("--pbp-label", default="Pbp")
    parser.add_argument("--prefix-system-in-ticks", action="store_true")
    parser.add_argument("--group", action="append", default=[])
    parser.add_argument("--combined-label", default="")
    parser.add_argument("--system-order", default="")
    parser.add_argument("--shared-y-range-from", default="")
    parser.add_argument("--plot-min-events-with-tracks", type=int, default=0)
    args = parser.parse_args()

    input_path = Path(args.input)
    markdown_output = Path(args.markdown_output)
    plot_output = Path(args.plot_output)

    rows = read_rows(input_path)
    system_order = parse_system_order(args.system_order)
    if args.group:
        groups = parse_groups(args.group)
        ordered_groups = groups
        if system_order:
            index_map = {system: index for index, system in enumerate(system_order)}
            ordered_groups = sorted(groups, key=lambda group: index_map.get(group["system"], len(index_map)))
        label_map = {group["system"]: group["table_label"] for group in ordered_groups}
        assessment_labels = {group["assessment_label"]: group["assessment_label"] for group in groups}
        results = {
            group["assessment_label"]: compute_group([row for row in rows if row["system"] == group["system"]])
            for group in groups
        }
        if args.combined_label and len(groups) > 1:
            results[args.combined_label] = compute_group(rows)
            assessment_labels[args.combined_label] = args.combined_label
    else:
        if system_order:
            label_lookup = {"pPb": args.ppb_label, "PbP": args.pbp_label}
            label_map = {system: label_lookup.get(system, system) for system in system_order}
        else:
            label_map = {"PbP": args.pbp_label, "pPb": args.ppb_label}
        assessment_labels = {
            "pPb only": "pPb only",
            "Pbp only": "Pbp only",
            "combined pPb+Pbp": "combined pPb+Pbp",
        }
        results = {
            "pPb only": compute_group([row for row in rows if row["system"] == "pPb"]),
            "Pbp only": compute_group([row for row in rows if row["system"] == "PbP"]),
            "combined pPb+Pbp": compute_group(rows),
        }

    y_min = y_max = None
    if args.shared_y_range_from:
        shared_rows = read_rows(Path(args.shared_y_range_from))
        y_min, y_max = compute_y_limits(shared_rows)

    make_plot(plot_output, rows, label_map, args.prefix_system_in_ticks,
        y_min=y_min, y_max=y_max,
        plot_min_events_with_tracks=args.plot_min_events_with_tracks)
    write_markdown(markdown_output, input_path, plot_output, rows, results, label_map, assessment_labels)


if __name__ == "__main__":
    main()
