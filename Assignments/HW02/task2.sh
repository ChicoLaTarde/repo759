#!/bin/bash

#SBATCH -p instruction
#SBATCH --cpus-per-task=1
#SBATCH --job-name=task2
#SBATCH --output=task2.out
#SBATCH --error=task2.err

./task2 1024 3
