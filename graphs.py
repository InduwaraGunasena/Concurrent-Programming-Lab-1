import pandas as pd
import matplotlib.pyplot as plt

# Load results
df = pd.read_csv("results.csv")

# Ensure proper column names
# case,implementation,threads,rep,time_us,seed
df.columns = ["case", "implementation", "threads", "rep", "time_us", "seed"]

# Group by case, implementation, threads
grouped = df.groupby(["case", "implementation", "threads"])["time_us"].agg(["mean", "std"]).reset_index()

cases = grouped["case"].unique()
implementations = ["serial", "mutex", "rwlock"]

for case in cases:
    plt.figure(figsize=(8, 6))
    for impl in implementations:
        data = grouped[(grouped["case"] == case) & (grouped["implementation"] == impl)]
        if data.empty:
            continue
        plt.errorbar(data["threads"], data["mean"], yerr=data["std"], label=impl, marker="o", capsize=4)

    plt.xlabel("Threads")
    plt.ylabel("Time (µs)")
    plt.title(f"Performance - Case {case}")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.savefig(f"case{case}_performance.png")
    plt.close()
