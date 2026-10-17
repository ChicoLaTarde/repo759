#!/bin/bash

#SBATCH -p instruction
#SBATCH --cpus-per-task=1
#SBATCH --mem=12G
#SBATCH --job-name=task1_scaling
#SBATCH --output=task1_scaling.out
#SBATCH --error=task1_scaling.err

# Output data file
echo "n,time_ms" > task1_data.csv

# Run n = 2^10 through 2^30
for p in {10..30}
do
    n=$((2**p))

    echo "Running n=$n"

    # task1 prints:
    # line 1 = time
    # line 2 = output[0]
    # line 3 = output[n-1]
    time_ms=$(./task1 $n | head -n 1)

    echo "$n,$time_ms" >> task1_data.csv
done

