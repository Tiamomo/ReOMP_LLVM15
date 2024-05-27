#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <mpi.h>
//#include "/home/msm924/rempipro/ReMPI/src/reomp/mutil.h"
//#include "/home/msm924/rempipro/ReMPI/src/reomp/reomp.h"
static int reomp_example_omp_critical(int nth)
{
  uint64_t i;
  volatile int sum;

#pragma omp parallel for private(i)
  for (i = 0; i < 10000000L / nth; i++) {
#pragma omp critical
    {
      sum = sum * omp_get_thread_num() + 1;
    }
  }
  return sum;
}

int main(int argc, char **argv)
{
  int nth;
  int rank, size;
  double whole_s, whole_e, s, e;
//  whole_s = reomp_util_get_time();
  MPI_Init(&argc, &argv);

 // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 // MPI_Comm_size(MPI_COMM_WORLD, &size);
  s = MPI_Wtime();
  int ret1 = reomp_example_omp_critical(2);
  e = MPI_Wtime();
  fprintf(stderr, "Test: omp_critical:(ret:  %15d)\n", ret1);
//  whole_e = reomp_util_get_time();
//  fprintf(stderr, "Test: main_time = %f\n", whole_e - whole_s);

  MPI_Finalize();
  return 0;
}

