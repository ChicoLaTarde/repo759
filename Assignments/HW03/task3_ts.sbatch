#!/bin/bash
#SBATCH --job-name=task3_ts
#SBATCH --partition=instruction
#SBATCH --output=task3_ts.out
#SBATCH --error=task3_ts.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=2G
#SBATCH --time=00:30:00

set -euo pipefail

export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=TRUE
export OMP_PLACES=cores

# Compile
g++ task3.cpp msort.cpp -Wall -O3 -std=c++17 -o task3 -fopenmp

# CSV header
echo "threshold,time_ms" > task3_ts_results.csv

# Test ts = 2^1 through 2^10
for power in $(seq 1 10)
do
    ts=$((2 ** power))

    output=$(./task3 1000000 8 "$ts")

    time_ms=$(echo "$output" | tail -n 1)

    echo "$ts,$time_ms" >> task3_ts_results.csv

    echo "Threshold: $ts | Time: $time_ms ms"
done

echo "Threshold benchmark completed."
