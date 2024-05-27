#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

#define ARRAY_SIZE 100000

int main(int argc, char **argv) {
    int rank, size;
    int local_sum = 0, global_sum = 0;
    int i;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // 使用OpenMP并行计算局部和
    #pragma omp parallel for reduction(+:local_sum)
    for (i = 0; i < ARRAY_SIZE; i++) {
        local_sum += i;
    }

    // 进行阻塞的进程通信，将各进程的局部和相加得到全局和
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    // 输出结果
    if(rank == 0) {
        printf("Global sum: %d\n", global_sum);
    }
    
    MPI_Finalize();
    
    return 0;
}

