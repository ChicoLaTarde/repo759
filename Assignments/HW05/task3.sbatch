#!/bin/bash -l
#SBATCH --job-name=task3
#SBATCH --partition=instruction
#SBATCH --ntasks=1
#SBATCH --gpus-per-task=1
#SBATCH --mem=8G
#SBATCH --time=00:15:00
#SBATCH --output=task3-%j.out

set -e

module load nvidia/cuda

nvcc task3.cu vscale.cu -Xcompiler -O3 -Xcompiler -Wall -Xptxas -O3 -std=c++17 -o task3
nvcc task3.cu vscale.cu -DVSCALE_THREADS_PER_BLOCK=16 -Xcompiler -O3 -Xcompiler -Wall -Xptxas -O3 -std=c++17 -o task3_16

csv="task3-${SLURM_JOB_ID}.csv"
printf 'n,threads_per_block,time_ms\n' > "$csv"
for ((exponent = 10; exponent <= 29; ++exponent)); do
    n=$((1 << exponent))
    output=$(./task3 "$n")
    printf '%d,512,%s\n' "$n" "${output%%$'\n'*}" >> "$csv"
    output=$(./task3_16 "$n")
    printf '%d,16,%s\n' "$n" "${output%%$'\n'*}" >> "$csv"
done
