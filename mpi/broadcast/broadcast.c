#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int number;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of the process and total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        number = 42; // only process 0 sets the value
        printf("Process %d: Number to broadcast = %d\n", rank, number);
    }

    // Broadcast the number from process 0 to all other processes
    MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);

    ,
    printf("Process %d: Received number = %d\n", rank, number);

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}

