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
    volatile int sum = 0;

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
    int nth, rank, size;
    double whole_s, whole_e, s, e;
    MPI_Status status;
    MPI_Request request;

    whole_s = reomp_util_get_time();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if(rank == 0){
            fprintf(stderr, "Usage: %s <# of threads>\n", argv[0]);
        }
        MPI_Finalize();
        exit(1);
    }

    nth = atoi(argv[1]);
    omp_set_num_threads(nth);

    s = MPI_Wtime();
    int ret1 = reomp_example_omp_critical(nth);
    e = MPI_Wtime();

    MPI_Request reqs[size];
    int global_sum = 0;
    MPI_Reduce(&ret1, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        fprintf(stderr, "Test: omp_critical - Time = %f, Result: %d\n", e - s, global_sum);
        whole_e = reomp_util_get_time();
        fprintf(stderr, "Test: Total Time = %f\n", whole_e - whole_s);
    }

    MPI_Finalize();

    return 0;
}

