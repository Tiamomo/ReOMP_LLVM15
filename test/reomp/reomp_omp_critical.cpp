#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <mpi.h>
#include "/home/msm924/rempipro/ReMPI/src/reomp/mutil.h"
#include "/home/msm924/rempipro/ReMPI/src/reomp/reomp.h"
static int reomp_example_omp_critical(int nth)
{
  uint64_t i;
  volatile int sum;
#pragma omp parallel for private(i)
  for (i = 0; i < 30000000L; i++) {
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
  double whole_s, whole_e, s, e;
  //whole_s = reomp_util_get_time();
  MPI_Init(&argc, &argv);

  if (argc != 2) {
    fprintf(stderr, "%s <# of threads>", argv[0]);
    exit(0);
  }
  nth = atoi(argv[1]);
  omp_set_num_threads(nth);

  s = MPI_Wtime();
  int ret1 = reomp_example_omp_critical(nth);
  e = MPI_Wtime();
  fprintf(stderr, "Test: omp_critical: time = %f (ret:  %15d)\n",e - s , ret1);
//  whole_e = reomp_util_get_time();
 // fprintf(stderr, "Test: main_time = %f\n", whole_e - whole_s);
  return 0;
}

