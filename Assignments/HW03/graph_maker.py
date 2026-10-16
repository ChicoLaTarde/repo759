import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("task1_results.csv")

plt.figure(figsize=(8, 5))

plt.plot(
    data["threads"],
    data["time_ms"],
    marker="o",
    linewidth=2
)

plt.xlabel("Number of Threads (t)")
plt.ylabel("Execution Time (ms)")
plt.title("OpenMP Matrix Multiplication (n = 1024)")

plt.xticks(range(1, 21))
plt.grid(True)

plt.tight_layout()
plt.savefig("task1.pdf")