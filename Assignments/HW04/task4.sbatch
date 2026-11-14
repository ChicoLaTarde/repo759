#!/bin/bash
#SBATCH --job-name=ece759_hw04_task4
#SBATCH --partition=instruction
#SBATCH --output=task4_slurm_%j.out
#SBATCH --error=task4_slurm_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=1G
#SBATCH --time=00:45:00

set -euo pipefail

cd "${SLURM_SUBMIT_DIR}"

particles="${1:-1000}"
end_time="${2:-100}"
python3 -c 'import sys; assert int(sys.argv[1]) >= 100 and float(sys.argv[2]) >= 100, "Use at least 100 particles and an end time of at least 100"' "$particles" "$end_time"

export OMP_DYNAMIC=FALSE
export OMP_PLACES=cores
export OMP_PROC_BIND=TRUE
export NBODY_SAVE_POSITIONS=0
export NBODY_SEED=17
export MPLBACKEND=Agg
export MPLCONFIGDIR="${SLURM_TMPDIR:-${TMPDIR:-/tmp}}/ece759-task4-${SLURM_JOB_ID}"

"${CXX:-g++}" task3.cpp -Wall -O3 -std=c++17 -o task3 -fopenmp

results="task4_results.csv"
printf 'schedule,num_threads,number_of_particles,simulation_end_time,time_ms,job_id,node\n' > "$results"
node=$(hostname)

for policy in static dynamic guided; do
    export OMP_SCHEDULE="$policy"
    for threads in {1..8}; do
        export OMP_NUM_THREADS="$threads"
        output=$(srun --ntasks=1 --cpus-per-task="$threads" --cpu-bind=cores ./task3 "$particles" "$end_time" "$threads")
        time_ms=$(printf '%s\n' "$output" | awk '/^time: [0-9.eE+-]+ms$/ {sub(/ms$/, "", $2); print $2}')
        if [[ -z "$time_ms" ]]; then
            printf 'Could not parse timing for %s with %s threads: %s\n' "$policy" "$threads" "$output" >&2
            exit 1
        fi
        printf '%s,%s,%s,%s,%s,%s,%s\n' "$policy" "$threads" "$particles" "$end_time" "$time_ms" "${SLURM_JOB_ID}" "$node" >> "$results"
        printf '%s: %s threads, %s ms\n' "$policy" "$threads" "$time_ms"
    done
done

if python3 -c 'import matplotlib' >/dev/null 2>&1; then
    python3 plot_task4.py "$results" --output task4.pdf
else
    printf 'Saved %s. Matplotlib is unavailable; copy the results and plot_task4.py locally, then run:\n' "$results"
    printf 'python3 plot_task4.py %s --output task4.pdf\n' "$results"
fi
