#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    const int elements_per_proc = 2;
    int data[8];       // Only used by rank 0 (root process)
    int recvbuf[2];    // Each process receives 2 elements
    int result[8];     // Only used by rank 0 to gather results

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4) {
        if (rank == 0) {
            printf("This example requires exactly 4 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {

        for (int i = 0; i < 8; i++) {
            data[i] = i + 1;
        }
        printf("Process %d: Original data = ", rank);
        for (int i = 0; i < 8; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
    }

    // Scatter: distribute chunks of the data array from process 0 to all processes
    // Each process receives 'elements_per_proc' elements into 'recvbuf'
    MPI_Scatter(data, elements_per_proc, MPI_INT,
                recvbuf, elements_per_proc, MPI_INT,
                0, MPI_COMM_WORLD);

    printf("Process %d: Received data = %d %d\n", rank, recvbuf[0], recvbuf[1]);

    // Each process modifies its received data (multiplies by 2)
    for (int i = 0; i < elements_per_proc; i++) {
        recvbuf[i] *= 2;
    }

    printf("Process %d: Computed data = %d %d\n", rank, recvbuf[0], recvbuf[1]);

    // Gather: collect the processed chunks from all processes back to process 0
    // The results are stored in the 'result' array on process 0
    MPI_Gather(recvbuf, elements_per_proc, MPI_INT,
               result, elements_per_proc, MPI_INT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Process %d: Final gathered result = ", rank);
        for (int i = 0; i < 8; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
