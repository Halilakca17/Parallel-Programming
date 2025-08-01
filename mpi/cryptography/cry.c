#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_SIZE 1000
#define KEY 0xAB

int main(int argc, char *argv[]) {
    int rank, size;
    unsigned char data[DATA_SIZE];
    unsigned char *sub_data;
    int chunk_size, remainder, i;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    chunk_size = DATA_SIZE / size;
    remainder = DATA_SIZE % size;

    if (rank == 0) {
        for (i = 0; i < DATA_SIZE; i++) {
            data[i] = (unsigned char)(i % 256);
        }
        printf("Original data first 10 bytes:\n");
        for (i = 0; i < 10; i++) {
            printf("%02X ", data[i]);
        }
        printf("\n");
    }

    int *sendcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        sendcounts = (int*) malloc(size * sizeof(int));
        displs = (int*) malloc(size * sizeof(int));

        int offset = 0;
        for (i = 0; i < size; i++) {
            sendcounts[i] = chunk_size + (i < remainder ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    int my_chunk = chunk_size + (rank < remainder ? 1 : 0);

    sub_data = (unsigned char*) malloc(my_chunk * sizeof(unsigned char));
    if (!sub_data) {
        fprintf(stderr, "Memory allocation failed on rank %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    MPI_Scatterv(data, sendcounts, displs, MPI_UNSIGNED_CHAR,
                 sub_data, my_chunk, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // Sıralı yazdırma için
    MPI_Barrier(MPI_COMM_WORLD);
    for (int p = 0; p < size; p++) {
        if (rank == p) {
            printf("Process %d received data (first 10 bytes or less): ", rank);
            for (i = 0; i < (my_chunk < 10 ? my_chunk : 10); i++) {
                printf("%02X ", sub_data[i]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // XOR şifreleme
    for (i = 0; i < my_chunk; i++) {
        sub_data[i] ^= KEY;
    }

    // Şifrelenmiş veriyi sırayla yazdır
    MPI_Barrier(MPI_COMM_WORLD);
    for (int p = 0; p < size; p++) {
        if (rank == p) {
            printf("Process %d encrypted data (first 10 bytes or less): ", rank);
            for (i = 0; i < (my_chunk < 10 ? my_chunk : 10); i++) {
                printf("%02X ", sub_data[i]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Gatherv(sub_data, my_chunk, MPI_UNSIGNED_CHAR,
                data, sendcounts, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Encrypted data first 10 bytes:\n");
        for (i = 0; i < 10; i++) {
            printf("%02X ", data[i]);
        }
        printf("\n");

        printf("Total execution time: %f seconds\n", end_time - start_time);

        free(sendcounts);
        free(displs);
    }

    free(sub_data);
    MPI_Finalize();
    return 0;
}

