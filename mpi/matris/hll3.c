#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to print sample elements from the result matrix
void printMatrixSample(double *M, int rows, int cols) {
    printf("C[0][0] = %6.2f\n", M[0]);
    printf("C[0][%d] = %6.2f\n", cols - 1, M[cols - 1]);
    printf("C[%d][0] = %6.2f\n", rows - 1, M[(rows - 1) * cols]);
    printf("C[%d][%d] = %6.2f\n", rows - 1, cols - 1, M[rows * cols - 1]);
}

int main(int argc, char** argv) {
    int N = 1000; // Size of NxN matrices
    int rank, size;
    int i, j, k;

    MPI_Init(&argc, &argv); // Initialize the MPI environment

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get current process ID
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of processes

    // Check if the matrix can be evenly divided among processes
    if (N % size != 0) {
        if (rank == 0)
            printf("Error: Matrix size must be divisible by number of processors.\n");
        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = N / size; // Number of rows per process

    double *A = NULL;
    double *B = NULL;
    double *C = (double *)malloc(rows_per_proc * N * sizeof(double)); // Each process calculates part of C
    if (C == NULL) {
        printf("[Process %d] Memory allocation failed.\n", rank);
        MPI_Finalize();
        return 1;
    }

    // Only rank 0 initializes the full matrices A and B
    if (rank == 0) {
        A = (double *)malloc(N * N * sizeof(double));
        B = (double *)malloc(N * N * sizeof(double));

        srand(time(NULL)); // Seed for random numbers

        for (i = 0; i < N * N; i++) {
            A[i] = (double)(rand() % 10); // Fill A with random numbers
            B[i] = (double)(rand() % 10); // Fill B with random numbers
        }
        printf("Process %d: Matrices randomly initialized, size %d x %d\n", rank, N, N);
    }

    // Allocate buffer for each process’s part of A
    double *sub_A = (double *)malloc(rows_per_proc * N * sizeof(double));
    if (sub_A == NULL) {
        printf("[Process %d] Memory allocation failed.\n", rank);
        MPI_Finalize();
        return 1;
    }

    // Scatter rows of A to all processes
    MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE, sub_A, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Broadcast matrix B to all processes
    if (rank == 0) {
        MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } else {
        B = (double *)malloc(N * N * sizeof(double));
        MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize before starting timer
    double start_time = MPI_Wtime(); // Start timing

    // Perform matrix multiplication: sub_A * B = C
    for (i = 0; i < rows_per_proc; i++) {
        for (j = 0; j < N; j++) {
            C[i * N + j] = 0;
            for (k = 0; k < N; k++) {
                C[i * N + j] += sub_A[i * N + k] * B[k * N + j];
            }
        }
    }

    double end_time = MPI_Wtime(); // End timing
    printf("[Process %d] Computation time: %f seconds\n", rank, end_time - start_time);

    // Gather the results from all processes into result matrix (only on rank 0)
    if (rank == 0) {
        double *result = (double *)malloc(N * N * sizeof(double));
        MPI_Gather(C, rows_per_proc * N, MPI_DOUBLE, result, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        printf("\nSample elements from result matrix C:\n");
        printMatrixSample(result, N, N); // Print some values from final result

        free(result);
        free(A);
        free(B);
    } else {
        MPI_Gather(C, rows_per_proc * N, MPI_DOUBLE, NULL, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(B);
    }

    free(sub_A);
    free(C);

    MPI_Finalize(); // Finalize the MPI environment
    return 0;
}
