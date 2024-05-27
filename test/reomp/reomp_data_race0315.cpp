#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <mpi.h>
static int reomp_example_data_race(int nth)
{
    uint64_t i;
    volatile int sum = 1;
#pragma omp parallel for private(i) 
    for (i = 0; i < 8L / nth; i++) {                                           
        sum += nth;
    }
    return sum;
}

int main(int argc, char** argv)
{
    
    int ret1 = reomp_example_data_race(2);

    fprintf(stderr, "Test: data_race: (ret:  %15d)\n", ret1);
    return 0;
}

