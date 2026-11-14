#!/bin/bash
#SBATCH --job-name=ece759_task2
#SBATCH --partition=instruction
#SBATCH --output=task2_slurm.out
#SBATCH --error=task2_slurm.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=20
#SBATCH --mem=1G
#SBATCH --time=00:20:00

set -euo pipefail

export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=TRUE

echo "threads,time_ms" > task2_results.csv

for t in $(seq 1 20)
do
    output=$(./task2 1024 "$t")

    time_ms=$(echo "$output" | tail -n 1)

    echo "$t,$time_ms" >> task2_results.csv

    echo "Threads: $t | Time: $time_ms ms"
done
