#!/bin/bash
#SBATCH --job-name=ece759_task1
#SBATCH --partition=instruction
#SBATCH --output=task1_slurm.out
#SBATCH --error=task1_slurm.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=20
#SBATCH --time=00:20:00

set -euo pipefail

export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=TRUE

echo "threads,time_ms" > task1_results.csv

for t in $(seq 1 20)
do
    export OMP_NUM_THREADS=$t

    output=$(./task1 1024 "$t")

    time_ms=$(echo "$output" | tail -n 1)

    echo "$t,$time_ms" >> task1_results.csv

    echo "Threads: $t | Time: $time_ms ms"
done
