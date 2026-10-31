#!/bin/bash
#SBATCH --job-name=ece759_hw04_task3
#SBATCH --partition=instruction
#SBATCH --output=task3_slurm.out
#SBATCH --error=task3_slurm.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --mem=1G
#SBATCH --time=00:20:00

set -euo pipefail

export OMP_NUM_THREADS="${SLURM_CPUS_PER_TASK}"
export OMP_DYNAMIC=FALSE
export OMP_PLACES=cores
export OMP_PROC_BIND=TRUE

g++ task3.cpp -Wall -O3 -std=c++17 -o task3 -fopenmp
srun --cpu-bind=cores ./task3 "${1:-1000}" "${2:-1.0}" "${SLURM_CPUS_PER_TASK}"
