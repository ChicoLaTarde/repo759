# OpenMP scheduling experiment

`task3.cpp` uses `schedule(runtime)` for all parallel loops. Set `OMP_SCHEDULE`
to `static`, `dynamic`, or `guided`; without this setting, the program uses static
scheduling. No explicit chunk size is supplied, so OpenMP uses each policy's
default chunking.

On Euler, load the GCC and Python environments provided by the cluster. Python
must have Matplotlib installed. From the directory containing these files, run:

```bash
sbatch task4.sbatch
```

The job requests one node with eight CPU cores. It compiles with
`g++ task3.cpp -Wall -O3 -std=c++17 -o task3 -fopenmp`, then runs each of the three
policies with thread counts 1 through 8. The default problem size is 1000 particles
and an end time of 100 (10,000 timesteps). To use the minimum requested size:

```bash
sbatch task4.sbatch 100 100
```

Each step requests as many cores as its thread count and binds threads to cores.
The benchmark fixes `NBODY_SEED=17` for comparable initial conditions and sets
`NBODY_SAVE_POSITIONS=0` to exclude position-file writes from the measured time.
Ordinary `task3` runs still save positions unless this setting is supplied.

Outputs are `task4_results.csv`, the job's Slurm logs, and `task4.pdf`. The PDF
contains three side-by-side linear-linear plots with a shared elapsed-time axis.
Each point is one run; the timings include program initialization and cleanup.
The CSV records the job ID and compute-node hostname. Avoid running multiple
experiment jobs simultaneously in the same directory because they share output
filenames.

To regenerate the PDF from the collected results:

```bash
python3 plot_task4.py task4_results.csv --output task4.pdf
```

For an individual run in an allocation with at least eight cores:

```bash
OMP_SCHEDULE=guided NBODY_SEED=17 NBODY_SAVE_POSITIONS=0 \
    srun --ntasks=1 --cpus-per-task=8 --cpu-bind=cores ./task3 1000 100 8
```
