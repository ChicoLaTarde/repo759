#!/bin/bash -l
#SBATCH --job-name=task1
#SBATCH --partition=instruction
#SBATCH --ntasks=1
#SBATCH --gres=gpu:1
#SBATCH --time=00:05:00
#SBATCH --output=task1-%j.out

set -e

if ! command -v nvcc >/dev/null 2>&1; then
    module load cuda
fi

nvcc task1.cu -Xcompiler -O3 -Xcompiler -Wall -Xptxas -O3 -std=c++17 -o task1
./task1
