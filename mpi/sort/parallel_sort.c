#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int compare_ints(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

void merge(int* arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

int main(int argc, char* argv[]) {
    int rank, size;
    const int N = 16;  
    int data[N];
    int* sub_data;
    int sub_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) printf("Array size must be divisible by number of processes.\n");
        MPI_Finalize();
        return 1;
    }

    sub_size = N / size;
    sub_data = (int*)malloc(sub_size * sizeof(int));

    if (rank == 0) {
        
        srand(42);
        printf("Original array: ");
        for (int i = 0; i < N; i++) {
            data[i] = rand() % 100;
            printf("%d ", data[i]);
        }
        printf("\n");
    }

    
    MPI_Scatter(data, sub_size, MPI_INT, sub_data, sub_size, MPI_INT, 0, MPI_COMM_WORLD);

   
    printf("Process %d: Received data = ", rank);
    for (int i = 0; i < sub_size; i++) {
        printf("%d ", sub_data[i]);
    }
    printf("\n");

    
    qsort(sub_data, sub_size, sizeof(int), compare_ints);

    
    printf("Process %d: Sorted subarray = ", rank);
    for (int i = 0; i < sub_size; i++) {
        printf("%d ", sub_data[i]);
    }
    printf("\n");

    
    MPI_Gather(sub_data, sub_size, MPI_INT, data, sub_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        
        int current_size = sub_size;
        while (current_size < N) {
            for (int left_start = 0; left_start < N - 1; left_start += 2 * current_size) {
                int mid = left_start + current_size - 1;
                int right_end = (left_start + 2 * current_size - 1 < N - 1) ? (left_start + 2 * current_size - 1) : (N - 1);
                if (mid < right_end)
                    merge(data, left_start, mid, right_end);
            }
            current_size *= 2;
        }

        printf("Process %d: Final sorted array = ", rank);
        for (int i = 0; i < N; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
    }

    free(sub_data);
    MPI_Finalize();
    return 0;
}

