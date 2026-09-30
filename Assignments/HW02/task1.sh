#!/bin/bash

#SBATCH -p instruction
#SBATCH --cpus-per-task=1
#SBATCH --job-name=task1
#SBATCH --output=task1.out
#SBATCH --error=task1.err

./task1 1024
