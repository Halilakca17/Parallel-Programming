#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int value;
    int sum_result, max_result, prod_result;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    
    value = rank + 1;

    
    printf("Process %d: My value = %d\n", rank, value);

    
    MPI_Reduce(&value, &sum_result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&value, &max_result, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&value, &prod_result, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);

    
    if (rank == 0) {
        printf("\nProcess %d: Reduce Results\n", rank);
        printf("Sum  = %d\n", sum_result);
        printf("Max  = %d\n", max_result);
        printf("Prod = %d\n", prod_result);
    }

    MPI_Finalize();
    return 0;
}

