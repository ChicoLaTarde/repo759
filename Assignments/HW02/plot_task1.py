import matplotlib.pyplot as plt
import csv

n_values = []
times = []

with open("task1_data.csv", "r") as file:
    reader = csv.DictReader(file)

    for row in reader:
        n_values.append(int(row["n"]))
        times.append(float(row["time_ms"]))

plt.plot(n_values, times, marker="o")

plt.xlabel("n (Input Size)")
plt.ylabel("Time (ms)")
plt.title("Task 1 Scan Scaling Analysis")

plt.xscale("log", base=2)
plt.grid(True)

plt.tight_layout()
plt.savefig("task1.pdf")
