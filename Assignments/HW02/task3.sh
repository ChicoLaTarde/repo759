#!/bin/bash

#SBATCH -p instruction
#SBATCH --cpus-per-task=1
#SBATCH --job-name=task3
#SBATCH --output=task3.out
#SBATCH --error=task3.err

./task3
