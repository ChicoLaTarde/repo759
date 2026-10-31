import argparse
import csv
import math
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description="Plot OpenMP scheduling experiment timings.")
    parser.add_argument("results", nargs="?", type=Path, default=Path("task4_results.csv"))
    parser.add_argument("--output", type=Path, default=Path("task4.pdf"))
    args = parser.parse_args()

    policies = ("static", "dynamic", "guided")
    measurements = {policy: {} for policy in policies}
    configurations = set()
    sources = set()
    with args.results.open(newline="") as results_file:
        for row in csv.DictReader(results_file):
            policy = row["schedule"]
            threads = int(row["num_threads"])
            time_ms = float(row["time_ms"])
            particles = int(row["number_of_particles"])
            end_time = float(row["simulation_end_time"])
            if policy not in measurements or threads not in range(1, 9):
                parser.error("Expected static, dynamic, or guided with 1 through 8 threads.")
            if threads in measurements[policy]:
                parser.error(f"Duplicate measurement for {policy} with {threads} threads.")
            if not math.isfinite(time_ms) or time_ms <= 0:
                parser.error("Timings must be finite and positive.")
            if particles < 100 or not math.isfinite(end_time) or end_time < 100:
                parser.error("Experiments require at least 100 particles and an end time of 100.")
            measurements[policy][threads] = time_ms / 1000.0
            configurations.add((particles, end_time))
            sources.add((row["job_id"], row["node"]))

    if len(configurations) != 1 or len(sources) != 1:
        parser.error("All measurements must use the same configuration and allocation.")
    if any(set(measurements[policy]) != set(range(1, 9)) for policy in policies):
        parser.error("Each scheduling policy needs exactly one result for each of 1 through 8 threads.")

    particles, end_time = configurations.pop()
    job_id, node = sources.pop()
    source = f"Slurm job {job_id} on {node}" if job_id != "local" else f"Local validation on {node} (not Euler)"
    figure, axes = plt.subplots(1, 3, figsize=(12, 4.5), sharey=True, layout="constrained")
    colors = ("#2563eb", "#d97706", "#059669")
    for axis, policy, color in zip(axes, policies, colors):
        threads = sorted(measurements[policy])
        times = [measurements[policy][thread] for thread in threads]
        axis.plot(threads, times, marker="o", linewidth=2, color=color)
        axis.set_title(policy.capitalize())
        axis.set_xlabel("Number of threads")
        axis.set_xticks(range(1, 9))
        axis.set_xlim(0.75, 8.25)
        axis.set_ylim(bottom=0)
        axis.set_xscale("linear")
        axis.set_yscale("linear")
        axis.grid(True, alpha=0.25)
    axes[0].set_ylabel("Elapsed time (seconds)")
    figure.suptitle(
        f"OpenMP N-body scheduling: N = {particles}, end time = {end_time:g}\n"
        f"{source}; seed = 17; CSV output disabled",
        fontsize=12,
    )
    figure.savefig(args.output, format="pdf")
    plt.close(figure)
    print(f"Saved {args.output}")


if __name__ == "__main__":
    main()
