import csv
import matplotlib.pyplot as plt

threads = []
times = []

with open("task2_results.csv", "r") as file:
    reader = csv.DictReader(file)

    for row in reader:
        threads.append(int(row["threads"]))
        times.append(float(row["time_ms"]))

plt.figure(figsize=(9, 5))

plt.plot(
    threads,
    times,
    marker="o",
    linewidth=2
)

plt.xlabel("Number of Threads (t)")
plt.ylabel("Execution Time (ms)")
plt.title("ECE 759 HW03 Task 2: OpenMP 2D Convolution (n = 1024)")

plt.xticks(range(1, 21))
plt.xlim(1, 20)
plt.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()
plt.savefig("task2.pdf")

print("Saved task2.pdf")