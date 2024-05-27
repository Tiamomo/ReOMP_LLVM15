#!/bin/sh 
#------ slurm option --------# 
#SBATCH --partition=mpc 
#SBATCH --account=RB230078 
#SBATCH --nodes=1 
#SBATCH --cpus-per-task=2 
#SBATCH --time=1:00:00 
#------- Program execution -------# 
module load intel
srun ./test1
#export OMP_NUM_THREADS=2 
#REOMP_METHOD=0 REOMP_MODE=0 srun --cpus-per-task=2 ./omp_critical1226
#REOMP_METHOD=0 REOMP_MODE=0 srun --cpus-per-task=2 ./omp_critical1226
#srun --cpus-per-task=2 REOMP_METHOD=0 REOMP_MODE=0 ./omp_critical1223

