#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <stdint.h>

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

static int reomp_example_data_race(int nth)
{
  uint64_t i;
  volatile int sum = 1;
#pragma omp parallel for private(i)
  for (i = 0; i < 3000000L / nth; i++) {
    sum += nth;
  }
  return sum;
}

int main(int argc, char **argv)
{
  int nth;
  if (argc != 2) {
    fprintf(stderr, "%s <# of threads>", argv[0]);
    exit(0);
  }
  nth = atoi(argv[1]);
  fprintf(stderr, "=============================================\n");  
  fprintf(stderr, "# of Thread   : %d\n", nth);
  fprintf(stderr, "=============================================\n");  
  omp_set_num_threads(nth);

  int ret1 = reomp_example_omp_critical(nth);
  int ret2 = reomp_example_data_race(nth);

  fprintf(stderr, "omp_critical: ret = %15d\n", ret1);
  fprintf(stderr, "data_race:    ret = %15d\n", ret2);

  return 0;
}
