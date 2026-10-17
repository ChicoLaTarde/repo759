import csv
import os
import matplotlib.pyplot as plt

# ==========================================
# Task 3: Threshold Scaling
# ==========================================

thresholds = []
ts_times = []

with open("task3_ts_results.csv", "r") as file:
    reader = csv.DictReader(file)

    for row in reader:
        thresholds.append(int(row["threshold"]))
        ts_times.append(float(row["time_ms"]))

# Find optimal threshold
best_index = ts_times.index(min(ts_times))
best_ts = thresholds[best_index]
best_time = ts_times[best_index]

print(f"Best threshold: {best_ts}")
print(f"Best execution time: {best_time:.4f} ms")

# Plot threshold vs execution time
plt.figure(figsize=(9, 6))

plt.plot(
    thresholds,
    ts_times,
    marker="o",
    linewidth=2,
    markersize=6,
    label="Parallel Merge Sort"
)

# Logarithmic x-axis, linear y-axis
plt.xscale("log", base=2)

plt.xticks(thresholds, [str(x) for x in thresholds])

# Highlight optimal threshold
plt.scatter(
    best_ts,
    best_time,
    color="red",
    s=90,
    zorder=5,
    label=f"Best threshold = {best_ts}"
)

plt.xlabel("Threshold (ts)")
plt.ylabel("Execution Time (ms)")
plt.title("OpenMP Merge Sort: Threshold Scaling (n = 10⁶, t = 8)")

plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.legend()
plt.tight_layout()

plt.savefig("task3_ts.pdf")
plt.close()

print("Generated task3_ts.pdf")

# ==========================================
# Task 3: Thread Scaling
# ==========================================

if os.path.exists("task3_t_results.csv"):

    threads = []
    thread_times = []

    with open("task3_t_results.csv", "r") as file:
        reader = csv.DictReader(file)

        for row in reader:
            threads.append(int(row["threads"]))
            thread_times.append(float(row["time_ms"]))

    plt.figure(figsize=(9, 6))

    plt.plot(
        threads,
        thread_times,
        marker="o",
        linewidth=2,
        markersize=6,
        label="Parallel Merge Sort"
    )

    plt.xlabel("Number of Threads (t)")
    plt.ylabel("Execution Time (ms)")
    plt.title(
        f"OpenMP Merge Sort: Thread Scaling (n = 10⁶, ts = {best_ts})"
    )

    plt.xticks(range(1, 21))
    plt.xlim(1, 20)

    plt.grid(True, linestyle="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()

    plt.savefig("task3_t.pdf")
    plt.close()

    print("Generated task3_t.pdf")

else:
    print("task3_t_results.csv not found.")
    print("Run the thread scaling benchmark first.")